#include <TRestDetectorSignalEvent.h>
#include <TRestRun.h>

#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 2) return 2;
    TRestRun run;
    run.OpenInputFile(argv[1]);
    if (run.GetEntries() != 3) return 3;
    run.GetEntry(0);
    auto* event = dynamic_cast<TRestDetectorSignalEvent*>(run.GetInputEvent());
    if (event == nullptr || event->GetNumberOfSignals() != 2) return 4;
    auto* signal = event->GetSignal(1);
    return signal != nullptr && signal->GetNumberOfPoints() == 2 &&
                   signal->GetTime(1) == static_cast<Double_t>(0.35F) && signal->GetData(1) == 11.0
               ? EXIT_SUCCESS
               : 5;
}
