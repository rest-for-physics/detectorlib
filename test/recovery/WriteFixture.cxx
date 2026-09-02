#include <TArrayC.h>
#include <TClass.h>
#include <TFile.h>
#include <TNamed.h>
#include <TRestAnalysisTree.h>
#include <TRestRun.h>
#include <TStreamerInfo.h>
#include <TTree.h>

#include <cstdlib>
#include <string>

#include "LegacyFixture.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) return 2;
    const bool keepSignalStreamerInfo = argc == 3 && std::string(argv[2]) == "--keep-signal-streamer-info";
    if (argc == 3 && !keepSignalStreamerInfo) return 2;
    TFile file(argv[1], "CREATE");
    if (file.IsZombie()) return 3;

    LegacyOpaqueMetadata metadata;
    metadata.fLabel = "must survive without a dictionary";
    metadata.fValue = 1729;
    if (metadata.Write("LegacyOpaqueMetadata") <= 0) return 4;

    TTree tree("EventTree", "legacy fixture");
    auto* signalEvent = new TRestDetectorSignalEvent;
    auto* opaqueEvent = new LegacyOpaqueEvent;
    tree.Branch("LegacyOpaqueEventBranch", "LegacyOpaqueEvent", &opaqueEvent, 32000, 0);
    tree.Branch("TRestDetectorSignalEventBranch", &signalEvent);
    for (Int_t entry = 0; entry < 3; ++entry) {
        signalEvent->SetRunOrigin(12);
        signalEvent->SetSubRunOrigin(2);
        signalEvent->SetID(100 + entry);
        signalEvent->SetSubID(entry);
        signalEvent->SetSubEventTag(TString::Format("tag-%d", entry));
        signalEvent->SetTime(1000 + entry, 200 + entry);
        signalEvent->SetOK(entry != 1);
        signalEvent->fSignal.clear();
        for (Int_t signalIndex = 0; signalIndex < 2; ++signalIndex) {
            TRestDetectorSignal signal;
            signal.fSignalID = 10 * entry + signalIndex;
            signal.fName = "signal-" + std::to_string(signalIndex);
            signal.fType = "legacy";
            for (Int_t point = 0; point < 2; ++point) {
                signal.fSignalTime.push_back(static_cast<Float_t>(entry + 0.25 * signalIndex + 0.1 * point));
                signal.fSignalCharge.push_back(static_cast<Float_t>(100 * entry + 10 * signalIndex + point));
            }
            signalEvent->fSignal.push_back(signal);
        }
        opaqueEvent->fCode = 700 + entry;
        opaqueEvent->fSamples = {static_cast<Float_t>(entry + 0.5), static_cast<Float_t>(entry + 0.75)};
        if (tree.Fill() < 0) return 5;
    }
    tree.SetAlias("opaqueCode", "LegacyOpaqueEventBranch.fCode");
    tree.GetUserInfo()->Add(new TNamed("TreeNote", "must survive fast cloning"));
    if (tree.Write() <= 0) return 6;

    TRestAnalysisTree analysis("AnalysisTree", "unrelated top-level tree");
    for (Int_t entry = 0; entry < 3; ++entry) {
        analysis.SetObservableValue("observable", entry * 1.5);
        analysis.Fill();
    }
    if (analysis.Write() <= 0) return 7;

    TRestRun run;
    if (run.Write("Run") <= 0) return 8;

    if (!keepSignalStreamerInfo) {
        auto* classIndex = file.GetClassIndex();
        if (classIndex == nullptr) return 9;
        for (auto* type : {TRestDetectorSignal::Class(), TRestDetectorSignalEvent::Class()}) {
            auto* info = type->GetStreamerInfo();
            const Int_t number = info == nullptr ? -1 : info->GetNumber();
            if (number < 0 || number >= classIndex->GetSize()) return 10;
            classIndex->fArray[number] = 0;
        }
        classIndex->fArray[0] = 1;
        file.WriteStreamerInfo();
    }
    file.Close();
    return file.TestBit(TFile::kWriteError) ? 11 : EXIT_SUCCESS;
}
