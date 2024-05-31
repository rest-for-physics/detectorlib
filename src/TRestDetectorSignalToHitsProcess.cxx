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
/// * **intWindow**: In case intwindow method is selected, it defines the
/// integral window (us) that will be used to define the window in which
/// the data points are integrated
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
/// * **tripleMax**: It will transport to the
/// TRestDetectorHitsProcess only the three points with higher amplitude of
/// each TRestSignal found inside the TRestDetectorSignalEvent. In practice
/// this is very similar to *onlyMax*, but the effective energy is smoothed
/// by including two additional points.
/// * **tripleMaxAverage(default)**: It will transport to the
/// TRestDetectorHitsProcess the average of the three points with higher
/// amplitude of each TRestSignal found inside the TRestDetectorSignalEvent.
/// In practice this is very similar to *onlyMax*, but the effective energy
/// is smoothed by including two additional points. Another advantage is that
/// it avoids tripling (x3) the number of hits compared to *tripleMax*.
/// * **gaussFit**: It performs a gaussian fit to each signal to determine the
/// z position and the energy.
/// * **landauFit**: It performs a landau fit to each signal to determine the
/// z position and the energy.
/// * **agetFit**: It performs a fit to each signal based on the aget response
/// function to determine the z position and the energy.
/// * **qCenter**: It will consider the shape of the signal to determine the
/// the time used to transform to a Z-coordinate. The energy is also
/// averaged on all points (Perhaps this is not the most appropiate?).
/// * **all**: It will simply transport all points found at the TRestSignal
/// to the TRestDetetorHitsEvent.
/// * **intwindow**: Splits the time into a window defined by fIntwindow
/// while performing the average of the data points. Every point corresponds
/// to a Hit
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
/// 2022-January: Implementing new method intwindod
/// \author     JuanAn Garcia
///
/// \class TRestDetectorSignalToHitsProcess
///
/// <hr>
///
#include "TRestDetectorSignalToHitsProcess.h"

#include "TRestDetectorSetup.h"

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
/// \param configFilename A const char* giving the path to an RML file.
///
TRestDetectorSignalToHitsProcess::TRestDetectorSignalToHitsProcess(const char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename) == -1) {
        LoadDefaultConfig();
    }
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
    fSignalEvent = nullptr;

    fGas = nullptr;
    fReadout = nullptr;
}

///////////////////////////////////////////////
/// \brief Process initialization. Observable names are interpreted and auxiliar
/// observable members, related to VolumeEdep, MeanPos, TracksCounter, TrackEDep
/// observables defined in TRestGeant4AnalysisProcess are filled at this stage.
///
void TRestDetectorSignalToHitsProcess::InitProcess() {
    fGas = GetMetadata<TRestDetectorGas>();
    if (fGas != nullptr) {
#ifndef USE_Garfield
        RESTError << "A TRestDetectorGas definition was found but REST was not linked to Garfield libraries."
                  << RESTendl;
        RESTError
            << "Please, remove the TRestDetectorGas definition, and add gas parameters inside the process "
               "TRestDetectorSignalToHitsProcess"
            << RESTendl;
        if (!fGas->GetError()) fGas->SetError("REST was not compiled with Garfield.");
        if (!this->GetError()) this->SetError("Attempt to use TRestDetectorGas without Garfield");
#endif

        if (fGasPressure <= 0) {
            fGasPressure = fGas->GetPressure();
        }
        if (fElectricField <= 0) {
            fElectricField = fGas->GetElectricField();
        }

        fGas->SetPressure(fGasPressure);
        fGas->SetElectricField(fElectricField);

        if (fDriftVelocity <= 0) {
            fDriftVelocity = fGas->GetDriftVelocity();
        }
    } else {
        if (fDriftVelocity < 0) {
            if (!this->GetError()) {
                this->SetError("Drift velocity is negative.");
            }
        }
    }

    fReadout = GetMetadata<TRestDetectorReadout>();

    if (fReadout == nullptr) {
        if (!this->GetError()) {
            this->SetError("The readout was not properly initialized.");
        }
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDetectorSignalToHitsProcess::ProcessEvent(TRestEvent* inputEvent) {
    fSignalEvent = (TRestDetectorSignalEvent*)inputEvent;

    if (!fReadout) {
        return nullptr;
    }

    fHitsEvent->SetID(fSignalEvent->GetID());
    fHitsEvent->SetSubID(fSignalEvent->GetSubID());
    fHitsEvent->SetTimeStamp(fSignalEvent->GetTimeStamp());
    fHitsEvent->SetSubEventTag(fSignalEvent->GetSubEventTag());

    RESTDebug << "TRestDetectorSignalToHitsProcess. Event id : " << fHitsEvent->GetID() << RESTendl;
    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Extreme) {
        fSignalEvent->PrintEvent();
    }

    Int_t numberOfSignals = fSignalEvent->GetNumberOfSignals();

    if (numberOfSignals == 0) return nullptr;

    Int_t planeID, readoutChannel = -1, readoutModule;
    for (int i = 0; i < numberOfSignals; i++) {
        TRestDetectorSignal* signal = fSignalEvent->GetSignal(i);
        Int_t signalID = signal->GetSignalID();

        if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug)
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
        Double_t fieldZDirection = plane->GetNormal().Z();
        Double_t zPosition = plane->GetPosition().Z();

        Double_t x = plane->GetX(readoutModule, readoutChannel);
        Double_t y = plane->GetY(readoutModule, readoutChannel);

        REST_HitType type = XYZ;
        TRestDetectorReadoutModule* mod = plane->GetModuleByID(readoutModule);
        if (TMath::IsNaN(x)) {
            x = mod->GetPlaneCoordinates(TVector2(mod->GetSize().X() / 2, mod->GetSize().Y() / 2)).X();
            type = YZ;
        } else if (TMath::IsNaN(y)) {
            y = mod->GetPlaneCoordinates(TVector2(mod->GetSize().X() / 2, mod->GetSize().Y() / 2)).Y();
            type = XZ;
        }

        if (fMethod == "onlyMax") {
            Double_t hitTime = signal->GetMaxPeakTime();
            Double_t distanceToPlane = hitTime * fDriftVelocity;

            if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug)
                cout << "Distance to plane : " << distanceToPlane << endl;

            Double_t z = zPosition + fieldZDirection * distanceToPlane;

            Double_t energy = signal->GetMaxPeakValue();

            if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug)
                cout << "Adding hit. Time : " << hitTime << " x : " << x << " y : " << y << " z : " << z
                     << " Energy : " << energy << endl;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);
        } else if (fMethod == "tripleMax") {
            Int_t bin = signal->GetMaxIndex();
            int binprev = (bin - 1) < 0 ? bin : bin - 1;
            int binnext = (bin + 1) > signal->GetNumberOfPoints() - 1 ? bin : bin + 1;

            Double_t hitTime = signal->GetTime(bin);
            Double_t energy = signal->GetData(bin);

            Double_t distanceToPlane = hitTime * fDriftVelocity;
            Double_t z = zPosition + fieldZDirection * distanceToPlane;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

            hitTime = signal->GetTime(binprev);
            energy = signal->GetData(binprev);

            distanceToPlane = hitTime * fDriftVelocity;
            z = zPosition + fieldZDirection * distanceToPlane;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

            hitTime = signal->GetTime(binnext);
            energy = signal->GetData(binnext);

            distanceToPlane = hitTime * fDriftVelocity;
            z = zPosition + fieldZDirection * distanceToPlane;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

            if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
                cout << "Distance to plane : " << distanceToPlane << endl;
                cout << "Adding hit. Time : " << hitTime << " x : " << x << " y : " << y << " z : " << z
                     << " Energy : " << energy << endl;
            }
        } else if (fMethod == "tripleMaxAverage") {
            Int_t bin = signal->GetMaxIndex();
            int binprev = (bin - 1) < 0 ? bin : bin - 1;
            int binnext = (bin + 1) > signal->GetNumberOfPoints() - 1 ? bin : bin + 1;

            Double_t hitTime = signal->GetTime(bin);
            Double_t energy1 = signal->GetData(bin);

            Double_t distanceToPlane = hitTime * fDriftVelocity;
            Double_t z1 = zPosition + fieldZDirection * distanceToPlane;

            hitTime = signal->GetTime(binprev);
            Double_t energy2 = signal->GetData(binprev);

            distanceToPlane = hitTime * fDriftVelocity;
            Double_t z2 = zPosition + fieldZDirection * distanceToPlane;

            hitTime = signal->GetTime(binnext);
            Double_t energy3 = signal->GetData(binnext);

            distanceToPlane = hitTime * fDriftVelocity;
            Double_t z3 = zPosition + fieldZDirection * distanceToPlane;

            Double_t eTot = energy1 + energy2 + energy3;

            Double_t zAvg = ((z1 * energy1) + (z2 * energy2) + (z3 * energy3)) / eTot;
            Double_t eAvg = eTot / 3.0;

            fHitsEvent->AddHit(x, y, zAvg, eAvg, 0, type);

            if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
                cout << "Distance to plane: " << distanceToPlane << endl;
                cout << "Adding hit. Time : " << hitTime << " x : " << x << " y : " << y << " z : " << zAvg
                     << " Energy : " << eAvg << endl;
                cout << "z1, z2, z3 = " << z1 << ", " << z2 << ", " << z3 << endl;
                cout << "E1, E2, E3 = " << energy1 << ", " << energy2 << ", " << energy3 << endl;
            }

        } else if (fMethod == "gaussFit") {
            const auto peak = signal->GetMaxGauss();
            if (peak) {
                const auto [time, energy] = peak.value();

                const auto distanceToPlane = time * fDriftVelocity;
                const auto z = zPosition + fieldZDirection * distanceToPlane;

                if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
                    cout << "Signal event : " << signal->GetSignalID()
                         << "--------------------------------------------------------" << endl;
                    cout << "GaussFit : time " << time << " ns and energy : " << energy << endl;
                    cout << "Signal to hit info : zPosition : " << zPosition
                         << "; fieldZDirection : " << fieldZDirection << " and driftV : " << fDriftVelocity
                         << endl;
                    cout << "Adding hit. Time : " << time << " ns x : " << x << " y : " << y << " z : " << z
                         << " Energy : " << energy << endl;
                }
            } else {
                // Perhaps we should just skip it
                double energy = -1;
                double z = -1;
                fHitsEvent->AddHit(x, y, z, energy, 0, type);
            }
        } else if (fMethod == "landauFit") {
            TVector2 landauFit = signal->GetMaxLandau();

            Double_t hitTime = 0;
            Double_t z = -1.0;
            if (landauFit.X() >= 0.0) {
                hitTime = landauFit.X();
                Double_t distanceToPlane = hitTime * fDriftVelocity;
                z = zPosition + fieldZDirection * distanceToPlane;
            }
            Double_t energy = -1.0;
            if (landauFit.Y() >= 0.0) {
                energy = landauFit.Y();
            }

            if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
                cout << "Signal event : " << signal->GetSignalID()
                     << "--------------------------------------------------------" << endl;
                cout << "landauFit : time bin " << landauFit.X() << " and energy : " << landauFit.Y() << endl;
                cout << "Signal to hit info : zPosition : " << zPosition
                     << "; fieldZDirection : " << fieldZDirection << " and driftV : " << fDriftVelocity
                     << endl;
                cout << "Adding hit. Time : " << hitTime << " x : " << x << " y : " << y << " z : " << z
                     << " Energy : " << energy << endl;
            }

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

        } else if (fMethod == "agetFit") {
            TVector2 agetFit = signal->GetMaxAget();

            Double_t hitTime = 0;
            Double_t z = -1.0;
            if (agetFit.X() >= 0.0) {
                hitTime = agetFit.X();
                Double_t distanceToPlane = hitTime * fDriftVelocity;
                z = zPosition + fieldZDirection * distanceToPlane;
            }
            Double_t energy = -1.0;
            if (agetFit.Y() >= 0.0) {
                energy = agetFit.Y();
            }

            if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
                cout << "Signal event : " << signal->GetSignalID()
                     << "--------------------------------------------------------" << endl;
                cout << "agetFit : time bin " << agetFit.X() << " and energy : " << agetFit.Y() << endl;
                cout << "Signal to hit info : zPosition : " << zPosition
                     << "; fieldZDirection : " << fieldZDirection << " and driftV : " << fDriftVelocity
                     << endl;
                cout << "Adding hit. Time : " << hitTime << " x : " << x << " y : " << y << " z : " << z
                     << " Energy : " << energy << endl;
            }

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

        } else if (fMethod == "qCenter") {
            Double_t energy_signal = 0;
            Double_t distanceToPlane = 0;

            for (int j = 0; j < signal->GetNumberOfPoints(); j++) {
                Double_t energy_point = signal->GetData(j);
                energy_signal += energy_point;
                distanceToPlane += signal->GetTime(j) * fDriftVelocity * energy_point;
            }
            Double_t energy = energy_signal / signal->GetNumberOfPoints();

            Double_t z = zPosition + fieldZDirection * (distanceToPlane / energy_signal);
            fHitsEvent->AddHit(x, y, z, energy, 0, type);
        } else if (fMethod == "all") {
            for (int j = 0; j < signal->GetNumberOfPoints(); j++) {
                Double_t energy = signal->GetData(j);

                Double_t distanceToPlane = signal->GetTime(j) * fDriftVelocity;

                if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
                    cout << "Time : " << signal->GetTime(j) << " Drift velocity : " << fDriftVelocity << endl;
                    cout << "Distance to plane : " << distanceToPlane << endl;
                }

                Double_t z = zPosition + fieldZDirection * distanceToPlane;

                if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug)
                    cout << "Adding hit. Time : " << signal->GetTime(j) << " x : " << x << " y : " << y
                         << " z : " << z << endl;

                fHitsEvent->AddHit(x, y, z, energy, 0, type);
            }
        } else if (fMethod == "intwindow") {
            Int_t nPoints = signal->GetNumberOfPoints();
            std::map<int, std::pair<int, double> > windowMap;
            for (int j = 0; j < nPoints; j++) {
                int index = signal->GetTime(j) / fIntWindow;
                auto it = windowMap.find(index);
                if (it != windowMap.end()) {
                    it->second.first++;
                    it->second.second += signal->GetData(j);
                } else {
                    windowMap[index] = std::make_pair(1, signal->GetData(j));
                }
            }

            for (const auto& [index, pair] : windowMap) {
                Double_t hitTime = index * fIntWindow + fIntWindow / 2.;
                Double_t energy = pair.second / pair.first;
                if (energy < fThreshold) {
                    continue;
                }
                RESTDebug << "TimeBin " << index << " Time " << hitTime << " Charge: " << energy
                          << " Thr: " << (fThreshold) << RESTendl;
                Double_t distanceToPlane = hitTime * fDriftVelocity;
                Double_t z = zPosition + fieldZDirection * distanceToPlane;

                RESTDebug << "Time : " << hitTime << " Drift velocity : " << fDriftVelocity
                          << "\nDistance to plane : " << distanceToPlane << RESTendl;
                RESTDebug << "Adding hit. Time : " << hitTime << " x : " << x << " y : " << y << " z : " << z
                          << " type " << type << RESTendl;

                fHitsEvent->AddHit(x, y, z, energy, 0, type);
            }
        } else {
            string errMsg = "The method " + (string)fMethod + " is not implemented!";
            SetError(errMsg);
        }
    }

    RESTDebug << "TRestDetectorSignalToHitsProcess. Hits added : " << fHitsEvent->GetNumberOfHits()
              << RESTendl;
    RESTDebug << "TRestDetectorSignalToHitsProcess. Hits total energy : " << fHitsEvent->GetTotalEnergy()
              << RESTendl;

    if (this->GetVerboseLevel() == TRestStringOutput::REST_Verbose_Level::REST_Debug) {
        fHitsEvent->PrintEvent(30);
    } else if (this->GetVerboseLevel() == TRestStringOutput::REST_Verbose_Level::REST_Extreme) {
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
