void REST_Detector_UpdateDecoding(const std::string& rootFile, const std::string& readoutName,
                                  const std::string& decodingFile) {
    TFile* f = TFile::Open(rootFile.c_str(), "UPDATE");
    TRestDetectorReadout* readout = (TRestDetectorReadout*)f->Get(readoutName.c_str());

    // readout->PrintMetadata();

    for (int p = 0; p < readout->GetNumberOfReadoutPlanes(); p++) {
        TRestDetectorReadoutPlane* plane = readout->GetReadoutPlane(p);
        for (int m = 0; m < plane->GetNumberOfModules(); m++) {
            TRestDetectorReadoutModule* module = &(*plane)[m];
            module->SetDecodingFile(decodingFile);
        }
    }

    readout->Write(readoutName.c_str());

    f->Close();
}
