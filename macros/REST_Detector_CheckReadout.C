#include <algorithm>
#include <iostream>
#include <vector>

#include "TRestDetectorReadout.h"
#include "TRestTask.h"
using namespace std;

TGraph* GetHittedStripMap(TRestDetectorReadoutPlane* p, Int_t mask[4], Double_t region[4], Int_t N,
                          Int_t offset);

#ifndef RESTTask_CheckReadout
#define RESTTask_CheckReadout

//*******************************************************************************************************
//***
//*** The following macro allows to perform tests to validate the proper readout construction. It
//*** will produce Monte Carlo generated positions on the a specific region of the readout plane
//*** definition, mapping the full active area of the detector to a region of area unity.
//*** ---
//*** A 128-bits mask allows to enable the channels we want to be tested. I.e. only Monte Carlo
//*** generated positions falling at the enabled channels will be drawn. Therefore, this macro is now
//*** limited to readouts with only 128-channels. A solution, if needed, would be to add an additional
//*** offset to shift the 128-channels mask.
//*** ---
//*** See also github.com/rest-for-physics/basic-readouts
//*** ---
//*** A region at the top-left bottom corner of the detector would be defined as:
//*** Double_t region[4]; region[0] = 0.0; region[1] = 0.5; region[2] = 0.5; region = 1.0;
//*** ---
//*** The 128-bits mask is split into four 32-bits integer fields identifying each with 32-channels.
//*** A mask enabling just channels from 72 to 80 would be defined as:
//*** Int_t mask[4]; mask[0] = 0x0; mask[1] = 0x0; mask[2] = 0xFF00; mask[3] = 0x0;
//*** ---
//*** The 128-bits mask might be shifted by a given offset, so that the first mask channel is identified
//*** with that offset
//*** ---
//*** A root file containing a TRestReadoutDetector with its name should be provided in the two first
//*** arguments. Optional arguments allow to control the statistics and the readout plane index where
//*** tests will be executed.
//*** ---
//*** Usage: restManager Detector_CheckReadout readouts.root readoutName region mask [offset] [N] [plane]
//***
//*******************************************************************************************************
Int_t REST_Detector_CheckReadout(TString rootFile, TString name, Double_t region[4], Int_t stripsMask[4],
                                 Int_t offset = 0, Int_t N = 1E4, Int_t plane = 0) {
    TFile* f = new TFile(rootFile);
    if (!f) {
        cout << "File not found: " << rootFile << endl;
        return 1;
    }

    TRestDetectorReadout* readout = (TRestDetectorReadout*)f->Get(name);
    if (!readout) {
        cout << "The readout with name " << name << " does not exist inside the file " << rootFile << endl;
        return 2;
    }
    readout->PrintMetadata();

    TRestDetectorReadoutPlane* readoutPlane = &(*readout)[plane];
    if (!readoutPlane) {
        cout << "The readout definition does not contain a readout plane with id : " << plane << endl;
        return 3;
    }
    Int_t nModules = readoutPlane->GetNumberOfModules();

    Int_t totalPixels = 0;
    Int_t totalChannels = 0;
    for (auto mdID = 0; mdID < nModules; mdID++) {
        TRestDetectorReadoutModule* module = &(*readoutPlane)[mdID];
        Int_t nChannels = module->GetNumberOfChannels();
        totalChannels += nChannels;

        for (auto ch = 0; ch < nChannels; ch++) {
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

    double xmin = 1E9, xmax = -1E9, ymin = 1E9, ymax = -1E9;

    Int_t graph = 0;
    Int_t modGraphID = 0;
    Int_t chGraph = 0;
    for (auto mdID = 0; mdID < nModules; mdID++) {
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

        modGraphID++;

        for (auto ch = 0; ch < nChannels; ch++) {
            TRestDetectorReadoutChannel* channel = &(*module)[ch];

            Int_t nPixels = channel->GetNumberOfPixels();

            Double_t xCH[1000], yCH[1000];
            for (int px = 0; px < nPixels; px++) {
                xCH[px] = module->GetPixelCenter(ch, px).X();
                yCH[px] = module->GetPixelCenter(ch, px).Y();

                for (int v = 0; v < 5; v++) {
                    x[v] = module->GetPixelVertex(ch, px, v).X();
                    y[v] = module->GetPixelVertex(ch, px, v).Y();
                }
                pixelGraph[graph] = new TGraph(5, x, y);

                pixelGraph[graph]->SetLineColor(kRed);
                pixelGraph[graph]->SetLineWidth(2);
                graph++;
            }
        }
    }

    TCanvas* c = new TCanvas("ReadoutGraphViewer", "  ", 900, 900);
    c->DrawFrame(xmin, ymin, xmax, ymax);
    c->SetTicks();

    GetHittedStripMap(readoutPlane, stripsMask, region, N, offset)->Draw("Psame");

    for (auto i = 0; i < modGraphID; i++) modGraph[i]->Draw("same");

    for (auto i = 0; i < graph; i++) pixelGraph[i]->Draw("same");

    return 0;
}

TGraph* GetHittedStripMap(TRestDetectorReadoutPlane* p, Int_t mask[4], Double_t region[4], Int_t N,
                          Int_t offset) {
    Double_t xmin, xmax, ymin, ymax;

    p->GetBoundaries(xmin, xmax, ymin, ymax);

    cout << "Readout plane boundaries" << endl;
    cout << "------------------------" << endl;
    cout << "Xmin : " << xmin << " Xmax : " << xmax << endl;
    cout << "Ymin : " << ymin << " Ymax : " << ymax << endl;

    cout << "region 1 : " << region[0] << endl;
    cout << "region 2 : " << region[1] << endl;
    cout << "region 3 : " << region[2] << endl;
    cout << "region 4 : " << region[3] << endl;

    Double_t xMin = region[0] * (xmax - xmin) + xmin;
    Double_t xMax = region[1] * (xmax - xmin) + xmin;

    Double_t yMin = region[2] * (ymax - ymin) + ymin;
    Double_t yMax = region[3] * (ymax - ymin) + ymin;

    cout << "Region boundaries on plane coordinates" << endl;
    cout << "--------------------------------------" << endl;
    cout << "Xmin : " << xMin << " Xmax : " << xMax << endl;
    cout << "Ymin : " << yMin << " Ymax : " << yMax << endl;

    TRandom* rnd = new TRandom();

    Double_t xR, yR;

    std::vector<Int_t> channelIds;

    for (auto n = 0; n < 128; n++) {
        Int_t bit = n % 32;
        Int_t level = n / 32;

        Int_t msk = 0x1 << bit;
        if (msk & mask[level]) {
            cout << "Adding channel : " << 32 * level + bit << endl;
            channelIds.push_back(32 * level + bit + offset);
        }
    }

    std::vector<Double_t> xPos;
    std::vector<Double_t> yPos;

    Int_t cont = 0;
    cout << "Number of points to be launched : " << N << endl;
    cout << "Number of modules : " << p->GetNumberOfModules() << endl;
    while (N > 0) {
        xR = rnd->Uniform(xMin, xMax);
        yR = rnd->Uniform(yMin, yMax);
        for (auto mod = 0; mod < p->GetNumberOfModules(); mod++) {
            Int_t chFound = p->FindChannel(mod, {xR, yR});

            if (chFound != -1 &&
                std::find(channelIds.begin(), channelIds.end(), chFound) != channelIds.end()) {
                xPos.push_back(xR);
                yPos.push_back(yR);
            }
        }
        N--;

        if (N % 10000 == 0) {
            system("date");
            cout << "N: " << N << endl;
        }
    }

    Int_t points = xPos.size();
    TGraph* gr = new TGraph(points, &xPos[0], &yPos[0]);
    gr->SetMarkerStyle(8);

    gr->SetMarkerSize(0.5);

    return gr;
}
#endif
