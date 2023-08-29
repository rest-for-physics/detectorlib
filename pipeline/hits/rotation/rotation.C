
Int_t rotation(Bool_t draw = false) {
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
    string cfgFile1 = "rotation90.rml";
    TRestDetectorHitsRotationProcess* rotation1 =
        new TRestDetectorHitsRotationProcess((char*)cfgFile1.c_str());
    rotation1->PrintMetadata();

    cfgFile1 = "rotation45off.rml";
    TRestDetectorHitsRotationProcess* rotation2 =
        new TRestDetectorHitsRotationProcess((char*)cfgFile1.c_str());
    rotation2->PrintMetadata();

    TRestDetectorHitsEvent* rotEvent1 = (TRestDetectorHitsEvent*)rotation1->ProcessEvent(ev);
    TRestDetectorHitsEvent* rotEvent2 = (TRestDetectorHitsEvent*)rotation2->ProcessEvent(ev);

    /* Used to generate a combined plot */
    if (draw) {
        TCanvas* c = new TCanvas("c0", "", 800, 600);
        std::vector<float> ranges{50, -25, 25, 50, -25, 25};
        TH2F* h = ev->GetXYHistogram(ranges);
        h->Draw("col");
        h->Draw("cont3 same");
        c->Print("original.png");

        h = rotEvent1->GetXYHistogram(ranges);
        // pad->cd();
        h->Draw("col");
        h->Draw("cont3 same");
        c->Print("rotation1.png");

        h = rotEvent2->GetXYHistogram(ranges);
        h->Draw("col");
        h->Draw("cont3 same");
        c->Print("rotation2.png");
    }

    cout << "[\033[92m OK \x1b[0m]" << endl;
    return 0;
}
