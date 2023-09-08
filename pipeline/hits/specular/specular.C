
Int_t specular(Bool_t draw = false) {
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
    string cfgFile1 = "specularXY.rml";
    TRestDetectorHitsSpecularProcess* specular1 =
        new TRestDetectorHitsSpecularProcess((char*)cfgFile1.c_str());
    specular1->PrintMetadata();

    cfgFile1 = "specularY.rml";
    TRestDetectorHitsSpecularProcess* specular2 =
        new TRestDetectorHitsSpecularProcess((char*)cfgFile1.c_str());
    specular2->PrintMetadata();

    cfgFile1 = "specularYoff.rml";
    TRestDetectorHitsSpecularProcess* specular3 =
        new TRestDetectorHitsSpecularProcess((char*)cfgFile1.c_str());
    specular3->PrintMetadata();

    TRestDetectorHitsEvent* specEvent1 = (TRestDetectorHitsEvent*)specular1->ProcessEvent(ev);
    TRestDetectorHitsEvent* specEvent2 = (TRestDetectorHitsEvent*)specular2->ProcessEvent(ev);
    TRestDetectorHitsEvent* specEvent3 = (TRestDetectorHitsEvent*)specular3->ProcessEvent(ev);

    /* Used to generate a combined plot */
    if (draw) {
        TCanvas* c = new TCanvas("c0", "", 800, 600);
        c->SetBorderSize(0.1);
        //       TPad *pad = new TPad("pad","The pad with the function", 0.05,0.50,0.95,0.95,21);
        //       pad->SetLeftMargin(0.15);
        std::vector<float> ranges{50, -25, 25, 50, -25, 25};
        TH2F* h = ev->GetXYHistogram(ranges);
        h->Draw("col");
        h->Draw("cont3 same");
        c->Print("original.png");

        h = specEvent1->GetXYHistogram(ranges);
        // pad->cd();
        h->Draw("col");
        h->Draw("cont3 same");
        c->Print("specular1.png");

        h = specEvent2->GetXYHistogram(ranges);
        h->Draw("col");
        h->Draw("cont3 same");
        c->Print("specular2.png");

        h = specEvent3->GetXYHistogram(ranges);
        h->Draw("col");
        h->Draw("cont3 same");
        c->Print("specular3.png");
    }

    cout << "[\033[92m OK \x1b[0m]" << endl;
    return 0;
}
