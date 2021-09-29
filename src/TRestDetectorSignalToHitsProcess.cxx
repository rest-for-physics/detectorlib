///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignalToHitsProcess.cxx
///
///             jan 2016:  Javier Galan
///_______________________________________________________________________________

#include "TRestDetectorSignalToHitsProcess.h"

#include <TRestDetectorSetup.h>

#include <string>
using namespace std;

ClassImp(TRestDetectorSignalToHitsProcess);
//______________________________________________________________________________
TRestDetectorSignalToHitsProcess::TRestDetectorSignalToHitsProcess() { Initialize(); }

//______________________________________________________________________________
TRestDetectorSignalToHitsProcess::TRestDetectorSignalToHitsProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

    // TRestDetectorSignalToHitsProcess default constructor
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDetectorSignalToHitsProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
TRestDetectorSignalToHitsProcess::~TRestDetectorSignalToHitsProcess() {
    delete fHitsEvent;
    // TRestDetectorSignalToHitsProcess destructor
}

//______________________________________________________________________________
void TRestDetectorSignalToHitsProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fHitsEvent = new TRestDetectorHitsEvent();
    fSignalEvent = 0;

    fGas = NULL;
    fReadout = NULL;
}

//______________________________________________________________________________
void TRestDetectorSignalToHitsProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();

    fGas = GetMetadata<TRestDetectorGas>();
    if (fGas != NULL) {
#ifndef USE_Garfield
        ferr << "A TRestDetectorGas definition was found but REST was not linked to Garfield libraries."
             << endl;
        ferr << "Please, remove the TRestDetectorGas definition, and add gas parameters inside the process "
                "TRestDetectorSignalToHitsProcess"
             << endl;
        if (!fGas->GetError()) fGas->SetError("REST was not compiled with Garfield.");
        if (!this->GetError()) this->SetError("Attempt to use TRestDetectorGas without Garfield");
#endif
        if (fGasPressure <= 0) fGasPressure = fGas->GetPressure();
        if (fElectricField <= 0) fElectricField = fGas->GetElectricField();

        fGas->SetPressure(fGasPressure);
        fGas->SetElectricField(fElectricField);

        if (fDriftVelocity <= 0) fDriftVelocity = fGas->GetDriftVelocity();
    } else {
        if (fDriftVelocity < 0) {
            if (!this->GetError()) this->SetError("Drift velocity is negative.");
        }
    }

    fReadout = GetMetadata<TRestDetectorReadout>();

    if (fReadout == nullptr) {
        if (!this->GetError()) this->SetError("The readout was not properly initialized.");
    }
}

//______________________________________________________________________________
TRestEvent* TRestDetectorSignalToHitsProcess::ProcessEvent(TRestEvent* evInput) {
    fSignalEvent = (TRestDetectorSignalEvent*)evInput;

    if (!fReadout) return nullptr;

    fHitsEvent->SetID(fSignalEvent->GetID());
    fHitsEvent->SetSubID(fSignalEvent->GetSubID());
    fHitsEvent->SetTimeStamp(fSignalEvent->GetTimeStamp());
    fHitsEvent->SetSubEventTag(fSignalEvent->GetSubEventTag());

    debug << "TRestDetectorSignalToHitsProcess. Event id : " << fHitsEvent->GetID() << endl;
    if (GetVerboseLevel() >= REST_Debug) fSignalEvent->PrintEvent();

    Int_t numberOfSignals = fSignalEvent->GetNumberOfSignals();

    Int_t planeID, readoutChannel = -1, readoutModule;
    for (int i = 0; i < numberOfSignals; i++) {
        TRestDetectorSignal* sgnl = fSignalEvent->GetSignal(i);
        Int_t signalID = sgnl->GetSignalID();

        if (GetVerboseLevel() >= REST_Debug)
            cout << "Searching readout coordinates for signal ID : " << signalID << endl;
        // for( int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++ )
        //{
        //    TRestDetectorReadoutPlane *plane = fReadout->GetReadoutPlane( p );
        //    for( int m = 0; m < plane->GetNumberOfModules(); m++ )
        //    {
        //        TRestDetectorReadoutModule *mod = plane->GetModule( m );

        //        if( mod->isDaqIDInside( signalID ) )
        //        {
        //            planeID = p;
        //            readoutChannel = mod->DaqToReadoutChannel( signalID );
        //            readoutModule = mod->GetModuleID();

        //            if( GetVerboseLevel() >= REST_Debug ) {
        //            cout <<
        //            "-------------------------------------------------------------------"
        //            << endl; cout << "signal Id : " << signalID << endl; cout <<
        //            "channel : " << readoutChannel << " module : " <<
        //            readoutModule << endl; cout <<
        //            "-------------------------------------------------------------------"
        //            << endl; }
        //        }
        //    }
        //}

        fReadout->GetPlaneModuleChannel(signalID, planeID, readoutModule, readoutChannel);

        if (readoutChannel == -1) {
            // cout << "REST Warning : Readout channel not found for daq ID : " << signalID << endl;
            continue;
        }
        /////////////////////////////////////////////////////////////////////////

        TRestDetectorReadoutPlane* plane = fReadout->GetReadoutPlaneWithID(planeID);

        // For the moment this will only be valid for a TPC with its axis (field
        // direction) being in z
        Double_t fieldZDirection = plane->GetPlaneVector().Z();
        Double_t zPosition = plane->GetPosition().Z();

        Double_t x = plane->GetX(readoutModule, readoutChannel);
        Double_t y = plane->GetY(readoutModule, readoutChannel);

        REST_HitType type = XYZ;
        TRestDetectorReadoutModule* mod = plane->GetModuleByID(readoutModule);
        if (TMath::IsNaN(x)) {
            x = mod->GetPhysicalCoordinates(TVector2(mod->GetModuleSizeX() / 2, mod->GetModuleSizeY() / 2))
                    .X();
            type = YZ;
        } else if (TMath::IsNaN(y)) {
            y = mod->GetPhysicalCoordinates(TVector2(mod->GetModuleSizeX() / 2, mod->GetModuleSizeY() / 2))
                    .Y();
            type = XZ;
        }

        if (fMethod == "onlyMax") {
            Double_t time = sgnl->GetMaxPeakTime();
            Double_t distanceToPlane = time * fDriftVelocity;

            if (GetVerboseLevel() >= REST_Debug) cout << "Distance to plane : " << distanceToPlane << endl;

            Double_t z = zPosition + fieldZDirection * distanceToPlane;

            Double_t energy = sgnl->GetMaxPeakValue();

            if (GetVerboseLevel() >= REST_Debug)
                cout << "Adding hit. Time : " << time << " x : " << x << " y : " << y << " z : " << z
                     << " Energy : " << energy << endl;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);
        } else if (fMethod == "tripleMax") {
            Int_t bin = sgnl->GetMaxIndex();
            int binprev = (bin - 1) < 0 ? bin : bin - 1;
            int binnext = (bin + 1) > sgnl->GetNumberOfPoints() - 1 ? bin : bin + 1;

            Double_t time = sgnl->GetTime(bin);
            Double_t energy = sgnl->GetData(bin);

            Double_t distanceToPlane = time * fDriftVelocity;
            Double_t z = zPosition + fieldZDirection * distanceToPlane;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

            time = sgnl->GetTime(binprev);
            energy = sgnl->GetData(binprev);

            distanceToPlane = time * fDriftVelocity;
            z = zPosition + fieldZDirection * distanceToPlane;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

            time = sgnl->GetTime(binnext);
            energy = sgnl->GetData(binnext);

            distanceToPlane = time * fDriftVelocity;
            z = zPosition + fieldZDirection * distanceToPlane;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

            if (GetVerboseLevel() >= REST_Debug) {
                cout << "Distance to plane : " << distanceToPlane << endl;
                cout << "Adding hit. Time : " << time << " x : " << x << " y : " << y << " z : " << z
                     << " Energy : " << energy << endl;
            }
        } else if (fMethod == "qCenter") {
            Double_t energy_signal = 0;
            Double_t distanceToPlane = 0;

            for (int j = 0; j < sgnl->GetNumberOfPoints(); j++) {
                Double_t energy_point = sgnl->GetData(j);
                energy_signal += energy_point;
                distanceToPlane += sgnl->GetTime(j) * fDriftVelocity * energy_point;
            }
            Double_t energy = energy_signal / sgnl->GetNumberOfPoints();

            Double_t z = zPosition + fieldZDirection * (distanceToPlane / energy_signal);
            fHitsEvent->AddHit(x, y, z, energy, 0, type);
        } else {
            for (int j = 0; j < sgnl->GetNumberOfPoints(); j++) {
                Double_t energy = sgnl->GetData(j);

                Double_t distanceToPlane = sgnl->GetTime(j) * fDriftVelocity;

                if (GetVerboseLevel() >= REST_Debug) {
                    cout << "Time : " << sgnl->GetTime(j) << " Drift velocity : " << fDriftVelocity << endl;
                    cout << "Distance to plane : " << distanceToPlane << endl;
                }

                Double_t z = zPosition + fieldZDirection * distanceToPlane;

                if (GetVerboseLevel() >= REST_Debug)
                    cout << "Adding hit. Time : " << sgnl->GetTime(j) << " x : " << x << " y : " << y
                         << " z : " << z << endl;

                fHitsEvent->AddHit(x, y, z, energy, 0, type);
            }
        }
    }

    debug << "TRestDetectorSignalToHitsProcess. Hits added : " << fHitsEvent->GetNumberOfHits() << endl;
    debug << "TRestDetectorSignalToHitsProcess. Hits total energy : " << fHitsEvent->GetEnergy() << endl;

    if (this->GetVerboseLevel() == REST_Debug) {
        fHitsEvent->PrintEvent(30);
    } else if (this->GetVerboseLevel() == REST_Extreme) {
        fHitsEvent->PrintEvent(-1);
    }

    if (fHitsEvent->GetNumberOfHits() <= 0) {
        string errMsg = "Last event id: " + IntegerToString(fHitsEvent->GetID()) +
                        ". Failed to find readout positions in channel to hit conversion.";
        SetWarning(errMsg);
        return nullptr;
    }

    return fHitsEvent;
}

