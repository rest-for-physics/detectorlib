
Int_t hitmap(Bool_t draw = false) {
    TRestDetectorHitsEvent* ev = new TRestDetectorHitsEvent();

    Double_t xCenter = 5;
    Double_t yCenter = 10;

    TRandom3* random = new TRandom3(137);
    for (int n = 0; n < 10000; n++) {
        Double_t x = random->Gaus(xCenter, 3);
        Double_t y = random->Gaus(yCenter, 3);

        ev->AddHit(x, y, 0, 1);
    }

    // Initializing processes
    string cfgFile1 = "transform.rml";
    TRestDetectorHitmapAnalysisProcess* t = new TRestDetectorHitmapAnalysisProcess((char*)cfgFile1.c_str());
    t->PrintMetadata();

    TRestDetectorHitsEvent* tEv = (TRestDetectorHitsEvent*)t->ProcessEvent(ev);

    cout << "[\033[92m OK \x1b[0m]" << endl;
    return 0;
}
