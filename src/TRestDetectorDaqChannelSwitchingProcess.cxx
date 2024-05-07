///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorDaqChannelSwitchingProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestDetectorSignalAnalysisProcess
///             Date : Jun/2018
///             Author : Ni Kaixiang
///
///_______________________________________________________________________________

#include "TRestDetectorDaqChannelSwitchingProcess.h"

#include <TLegend.h>
#include <TPaveText.h>

using namespace std;

ClassImp(TRestDetectorDaqChannelSwitchingProcess);

TRestDetectorDaqChannelSwitchingProcess::TRestDetectorDaqChannelSwitchingProcess() { Initialize(); }

TRestDetectorDaqChannelSwitchingProcess::~TRestDetectorDaqChannelSwitchingProcess() {}

void TRestDetectorDaqChannelSwitchingProcess::Initialize() {
    SetLibraryVersion(LIBRARY_VERSION);
    SetSectionName(this->ClassName());

    fFirstDaqChannelDef.clear();
    fIgnoreUndefinedModules = false;
    fReadout = nullptr;
    fEvent = nullptr;
}

void TRestDetectorDaqChannelSwitchingProcess::InitProcess() {
    fReadout = GetMetadata<TRestDetectorReadout>();
    if (fReadout != nullptr) {
        auto iter = fFirstDaqChannelDef.begin();
        while (iter != fFirstDaqChannelDef.end()) {
            auto mod = fReadout->GetReadoutModuleWithID(iter->first);
            if (mod == nullptr) continue;
            // finding out the old "firstdaqchannel" value
            int mindaq = std::numeric_limits<Int_t>::max();
            for (size_t i = 0; i < mod->GetNumberOfChannels(); i++) {
                if (mod->GetChannel(i)->GetDaqID() < mindaq) {
                    mindaq = mod->GetChannel(i)->GetDaqID();
                }
            }

            // re-setting the value
            for (size_t i = 0; i < mod->GetNumberOfChannels(); i++) {
                mod->GetChannel(i)->SetDaqID(mod->GetChannel(i)->GetDaqID() - mindaq + iter->second);
            }

            iter++;
        }

        if (fIgnoreUndefinedModules) {
            for (int i = 0; i < fReadout->GetNumberOfReadoutPlanes(); i++) {
                TRestDetectorReadoutPlane& plane = (*fReadout)[i];

                for (size_t j = 0; j < plane.GetNumberOfModules(); j++) {
                    TRestDetectorReadoutModule& mod = plane[j];
                    if (fFirstDaqChannelDef.count(mod.GetModuleID()) == 0) {
                        for (size_t i = 0; i < mod.GetNumberOfChannels(); i++) {
                            mod.GetChannel(i)->SetDaqID(-1e9);
                        }
                    }
                }
            }
        }
    }
}

TRestEvent* TRestDetectorDaqChannelSwitchingProcess::ProcessEvent(TRestEvent* inputEvent) {
    fEvent = inputEvent;
    return inputEvent;
}

void TRestDetectorDaqChannelSwitchingProcess::EndProcess() {}

// redefining module's first daq channel:
// <module id="1" firstdaqchannel="136*3" />
// ignore undefined modules modules by setting their channel's daq id to -1e9
// <parameter name="ignoreUndefinedModules" value="true" />
void TRestDetectorDaqChannelSwitchingProcess::InitFromConfigFile() {
    TiXmlElement* ele = fElement->FirstChildElement("module");
    while (ele != nullptr) {
        int id = StringToInteger(GetParameter("id", ele));
        int channel = StringToInteger(GetParameter("firstdaqchannel", ele));
        if (id == -1 || channel == -1) continue;
        fFirstDaqChannelDef[id] = channel;
        ele = ele->NextSiblingElement("module");
    }
    fIgnoreUndefinedModules = StringToBool(GetParameter("ignoreUndefinedModules", "false"));
}
