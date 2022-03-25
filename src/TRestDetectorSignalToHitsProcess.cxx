/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
///
/// TRestDetectorSignalToHitsProcess will use the readout definition to
/// transform a TRestDetectorSignalEvent into a TRestDetectorHitsEvent.
/// The physical time on the input event will be associated with a physical
/// spatial Z-coordinate using the gas drift velocity given by the user as
/// the metadata parameter "driftVelocity". If the drift velocity is not
/// specified by the user, or it is negative, the process will try to
/// retrieve the drift velocity from the TRestDetectorGas metadata
/// definition that should have been defined, and therefore available to
/// the TRestRun instance. In the case that the drift velocity is retrieved
/// from TRestDetectorGas two more parameters become relevant, the detector
/// pressure, and the drift field, that play a role on the value of the
/// drift velocity.
///
/// Therefore, relevant parameters (see also the process header) are:
/// * **driftVelocity**: The value of the electrons drift velocity.
/// Typically in mm/us.
/// * **gasPressure**: Gas pressure in bar. Relevant only if TRestDetectorGas
/// is used.
/// * **electricField**: Electric field. Typically in V/cm. Relevant only
/// if TRestDetectorGas is used.
///
/// On top of that, this process will need to get access to a
/// TRestDetectorReadout definition in order to transform the corresponding
/// signal id found at each signal in TRestDetectorSignalEvent to a spatial
/// physical position in the XY plane. The readout plane position at
/// TRestDetectorReadout definition is also used as a reference to obtain the
/// final Z-coordinate in this process.
///
/// \note Therefore, the real absolute Z of the event will only be connected
/// to reality if the physical time given at the TRestDetectorSignalEvent
/// corresponds with the drift time to the readout plane. This is usually not
/// the case, since our electronics system does not know the interaction time.
/// Although, this might be the case if the interaction light is measured
/// using a PMT, or the results are coming from Monte Carlo simulated data.
///
/// In order to transform the points found at each TRestSignal inside a
/// TRestDetectorSignalEvent, there are different methods that might lead to
/// different results. Those are defined at the "method" metadata parameter.
/// If this parameter is not given, its default value will be "tripleMax".
///
/// The possible values for the "method" metadata parameter are:
/// * **onlyMax**: It will transport to the TRestDetectorHitsProcess only
/// the point at the maximum amplitude of each TRestSignal found inside the
/// TRestDetectorSignalEvent. The Z-coordinate will be directly connected
/// with the time position at the maximum amplitude of the signal.
/// * **tripleMax(default)**: It will transport to the
/// TRestDetectorHitsProcess only the three points with higher amplitude of
/// each TRestSignal found inside the TRestDetectorSignalEvent. In practice
/// this is very similar to *onlyMax*, but the effective energy is smoothed
/// by including two additional points.
/// * **qCenter**: It will consider the shape of the signal to determine the
/// the time used to transform to a Z-coordinate. The energy is also
/// averaged on all points (Perhaps this is not the most appropiate?).
/// * **all**: It will simply transport all points found at the TRestSignal
/// to the TRestDetetorHitsEvent.
///
/// \htmlonly <style>div.image img[src="signalToHits.png"]{width:800px;}</style> \endhtmlonly
///
/// The following figure shows the results of applying the process to a
/// Monte Carlo artificially signal generated, where the energy deposits
/// are exactly at their corresponding physical drift time values. No shaping or
/// any other electronic effects were included at the input event (left figure).
/// The drift time is translated to a Z-coordinate on the output event, where we
/// show only one readout projection in X (right figure).
///
/// \image html signalToHits.png
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-January: First concept and implementation.
/// \author     Javier Galan
///
/// \class TRestDetectorSignalToHitsProcess
///
/// <hr>
///
#include "TRestDetectorSignalToHitsProcess.h"

#include <TRestDetectorSetup.h>
using namespace std;

ClassImp(TRestDetectorSignalToHitsProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDetectorSignalToHitsProcess::TRestDetectorSignalToHitsProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestDetectorSignalToHitsProcess::TRestDetectorSignalToHitsProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDetectorSignalToHitsProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDetectorSignalToHitsProcess::~TRestDetectorSignalToHitsProcess() { delete fHitsEvent; }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestDetectorSignalToHitsProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fHitsEvent = new TRestDetectorHitsEvent();
    fSignalEvent = 0;

    fGas = NULL;
    fReadout = NULL;
}

///////////////////////////////////////////////
/// \brief Process initialization. Observable names are interpreted and auxiliar
/// observable members, related to VolumeEdep, MeanPos, TracksCounter, TrackEDep
/// observables defined in TRestGeant4AnalysisProcess are filled at this stage.
///
void TRestDetectorSignalToHitsProcess::InitProcess() {
    fGas = GetMetadata<TRestDetectorGas>();
    if (fGas) {
#ifndef REST_GARFIELD
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

///////////////////////////////////////////////
/// \brief The main processing event function
///
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
        } else if (fMethod == "all") {
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
        } else {
            string errMsg = "The method " + (string)fMethod + " is not implemented!";
            SetError(errMsg);
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
