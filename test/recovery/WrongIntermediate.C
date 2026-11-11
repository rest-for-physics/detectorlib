#include <TFile.h>
#include <TNamed.h>
#include <TParameter.h>
#include <TSystem.h>
#include <TTree.h>

void WrongIntermediate() {
    const char* filename = gSystem->Getenv("REST_LEGACY_RECOVERY_INTERMEDIATE");
    TFile output(filename, "CREATE");
    TTree data("LegacySignalData", "deliberately mismatched test intermediate");
    data.Write();
    TNamed("sourceUuid", "not-the-source-uuid").Write();
    TParameter<Long64_t>("sourceSize", 0).Write();
    TParameter<Long64_t>("sourceEntries", 0).Write();
    TParameter<Long64_t>("sourceSignalVersion", 3).Write();
    TParameter<Long64_t>("recoveredSignals", 0).Write();
    TParameter<Long64_t>("recoveredPoints", 0).Write();
    output.Close();
    gSystem->Exit(output.TestBit(TFile::kWriteError) ? 1 : 0);
}
