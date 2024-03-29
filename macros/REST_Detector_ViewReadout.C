#include <iostream>
#include <vector>

#include "TRestBrowser.h"
#include "TRestDetectorReadout.h"
#include "TRestTask.h"
using namespace std;

#ifndef RestTask_ViewReadout
#define RestTask_ViewReadout

//*******************************************************************************************************
//*** Description: This macro will draw on a canvas the readout module topology from the specified
//*** readout plane. If no plane id is provided the first readout plane will be considered.
//*** --------------
//*** Remark : If the root file contains several readout files a particular readout can be specified
//***          by name. If the name is empty "" the first readout will be used.
//*** --------------
//*** Usage inside restRoot: REST_ViewReadout("readouts.root", "", 1);
//*******************************************************************************************************
int REST_Detector_ViewReadout(TString rootFile, TString name = "", Int_t plane = 0) {
    TFile* fFile = new TFile(rootFile);

    TRestDetectorReadout* readout = nullptr;
    TIter nextkey(fFile->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)nextkey()) != nullptr) {
        if (key->GetClassName() == (TString) "TRestDetectorReadout") {
            if (readout == nullptr)
                readout = (TRestDetectorReadout*)fFile->Get(key->GetName());
            else if (key->GetName() == name)
                readout = (TRestDetectorReadout*)fFile->Get(key->GetName());
        }
    }

    delete key;

    TString title =
        "Readout: " + (string)readout->GetName() + " --- plane: " + IntegerToString(plane) + ";X[mm];Y[mm]";

    readout->PrintMetadata();

    TRestDetectorReadoutPlane* readoutPlane = &(*readout)[plane];

    Int_t nModules = readoutPlane->GetNumberOfModules();

    Int_t totalPixels = 0;
    Int_t totalChannels = 0;
    for (int mdID = 0; mdID < nModules; mdID++) {
        TRestDetectorReadoutModule* module = &(*readoutPlane)[mdID];
        Int_t nChannels = module->GetNumberOfChannels();
        totalChannels += nChannels;

        for (int ch = 0; ch < nChannels; ch++) {
            TRestDetectorReadoutChannel* channel = &(*module)[ch];
            Int_t nPixels = channel->GetNumberOfPixels();
            totalPixels += nPixels;
        }
    }

    const Int_t nPixConst = totalPixels;
    const Int_t nModConst = nModules;
    const Int_t nChConst = totalChannels;

    vector<TGraph*> pixelGraph(nPixConst);
    vector<TGraph*> channelGraph(nChConst);
    vector<TGraph*> modGraph(nModConst);
    vector<TLatex*> channelIDLabel(nPixConst);

    double xmin = std::numeric_limits<Double_t>::max(), xmax = std::numeric_limits<Double_t>::min(),
           ymin = std::numeric_limits<Double_t>::max(), ymax = std::numeric_limits<Double_t>::min();

    Int_t graph = 0;
    Int_t modGraphID = 0;
    Int_t chGraph = 0;
    for (int mdID = 0; mdID < nModules; mdID++) {
        TRestDetectorReadoutModule* module = &(*readoutPlane)[mdID];
        Int_t nChannels = module->GetNumberOfChannels();

        Double_t x[5];
        Double_t y[5];

        for (int v = 0; v < 5; v++) {
            x[v] = module->GetVertex(v).X();
            y[v] = module->GetVertex(v).Y();

            if (x[v] < xmin) xmin = x[v];
            if (y[v] < ymin) ymin = y[v];
            if (x[v] > xmax) xmax = x[v];
            if (y[v] > ymax) ymax = y[v];
        }
        modGraph[modGraphID] = new TGraph(5, x, y);

        modGraph[modGraphID]->SetLineColor(kBlack);
        modGraph[modGraphID]->SetLineWidth(2);
        // modGraph[modGraphID]->SetMaximum( 200 );
        // modGraph[modGraphID]->SetMinimum( -200 );
        // modGraph[modGraphID]->GetXaxis()->SetLimits(-200,200);

        modGraphID++;

        for (int ch = 0; ch < nChannels; ch++) {
            TRestDetectorReadoutChannel* channel = &(*module)[ch];

            Int_t nPixels = channel->GetNumberOfPixels();

            Double_t xCH[1000], yCH[1000];
            for (int px = 0; px < nPixels; px++) {
                xCH[px] = module->GetPixelCenter(&(*module)[ch][px]).X();
                yCH[px] = module->GetPixelCenter(&(*module)[ch][px]).Y();

                for (int v = 0; v < 5; v++) {
                    x[v] = module->GetPixelVertex(&(*module)[ch][px], v).X();
                    y[v] = module->GetPixelVertex(&(*module)[ch][px], v).Y();
                }
                pixelGraph[graph] = new TGraph(5, x, y);

                pixelGraph[graph]->SetLineColor(kRed);
                pixelGraph[graph]->SetLineWidth(2);
                // pixelGraph[graph]->SetMaximum( 200 );
                // pixelGraph[graph]->SetMinimum( -200 );
                // pixelGraph[graph]->GetXaxis()->SetLimits(-200,200);

                Double_t xMin = numeric_limits<Double_t>::max();
                Double_t yMin = numeric_limits<Double_t>::max();
                Double_t xMax = numeric_limits<Double_t>::min();
                for (int l = 0; l < 5; l++) {
                    if (x[l] < xMin) xMin = x[l];
                    if (y[l] < yMin) yMin = y[l];
                    if (x[l] > xMax) xMax = x[l];
                }
                channelIDLabel[graph] = new TLatex(xMin + (xMax - xMin) / 2, yMin, Form("%d", ch));
                channelIDLabel[graph]->SetTextSize(0.01);
                channelIDLabel[graph]->SetTextFont(12);
                channelIDLabel[graph]->SetTextAlign(21);
                //           channelIDLable[graph]->SetTextColor( kBlue+4*mdID );

                // pixelGraph[graph]->Draw("same");
                graph++;
            }
            channelGraph[chGraph] = new TGraph(nPixels, xCH, yCH);

            channelGraph[chGraph]->SetLineColor(kBlue);
            channelGraph[chGraph]->SetLineWidth(2);
            // channelGraph[chGraph]->SetMarkerStyle(20);
            // channelGraph[chGraph]->SetMarkerSize(10);
            // channelGraph[chGraph]->SetMaximum( 200 );
            // channelGraph[chGraph]->SetMinimum( -200 );
            // channelGraph[chGraph]->GetXaxis()->SetLimits(-200,200);
            chGraph++;
        }
    }

    TCanvas* c = new TCanvas("ReadoutGraphViewer", "  ", 900, 900);
    c->DrawFrame(xmin, ymin, xmax, ymax, title);
    c->SetTicks();

    for (int i = 0; i < modGraphID; i++) modGraph[i]->Draw("same");

    for (int i = 0; i < graph; i++) {
        pixelGraph[i]->Draw("same");
        // channelIDLabel[i]->Draw("same"); //If commented the graphics are faster
    }

    for (int i = 0; i < chGraph; i++) channelGraph[i]->Draw("same");

// when we run this macro from restManager from bash,
// we need to call TRestMetadata::GetChar() to prevent returning,
// while keeping GUI alive.
#ifdef REST_MANAGER
    GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}

#endif
