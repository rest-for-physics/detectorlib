#include <TFile.h>
#include <TNamed.h>
#include <TTree.h>

#include <cstdlib>
#include <string>

#include "LegacyFixture.h"

int main(int argc, char* argv[]) {
    if (argc != 2) return 2;
    TFile file(argv[1], "CREATE");
    if (file.IsZombie()) return 3;

    LegacyOpaqueMetadata metadata;
    metadata.fLabel = "must survive without a dictionary";
    metadata.fValue = 1729;
    if (metadata.Write("LegacyOpaqueMetadata") <= 0) return 4;

    TTree tree("EventTree", "legacy fixture");
    auto* signalEvent = new TRestDetectorSignalEvent;
    auto* opaqueEvent = new LegacyOpaqueEvent;
    tree.Branch("TRestDetectorSignalEventBranch", &signalEvent);
    tree.Branch("LegacyOpaqueEventBranch", "LegacyOpaqueEvent", &opaqueEvent, 32000, 0);
    for (Int_t entry = 0; entry < 3; ++entry) {
        signalEvent->fRunOrigin = 12;
        signalEvent->fSubRunOrigin = 2;
        signalEvent->fEventID = 100 + entry;
        signalEvent->fSubEventID = entry;
        signalEvent->fSubEventTag = TString::Format("tag-%d", entry);
        signalEvent->fEventTime.SetSec(1000 + entry);
        signalEvent->fEventTime.SetNanoSec(200 + entry);
        signalEvent->fOk = entry != 1;
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

    TTree analysis("AnalysisTree", "unrelated top-level tree");
    Double_t observable = 0;
    analysis.Branch("observable", &observable);
    for (Int_t entry = 0; entry < 3; ++entry) {
        observable = entry * 1.5;
        analysis.Fill();
    }
    if (analysis.Write() <= 0) return 7;
    file.Close();
    return file.TestBit(TFile::kWriteError) ? 8 : EXIT_SUCCESS;
}
