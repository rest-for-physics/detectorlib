
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignalEvent.h
///
///             Event class to store DAQ events either from simulation and
///             acquisition
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia/Javier Galan
///
///	       feb 2016: Added titles to axis in DrawGraph using TMultiGraph
///		  Javier Gracia
///_______________________________________________________________________________

#include "TRestDetectorSignalEvent.h"

#include <TMath.h>

using namespace std;

ClassImp(TRestDetectorSignalEvent);

TRestDetectorSignalEvent::TRestDetectorSignalEvent() {
    // TRestDetectorSignalEvent default constructor
    Initialize();
}

TRestDetectorSignalEvent::~TRestDetectorSignalEvent() {
    // TRestDetectorSignalEvent destructor
}

void TRestDetectorSignalEvent::Initialize() {
    TRestEvent::Initialize();
    fSignal.clear();
    fPad = nullptr;
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;
}

void TRestDetectorSignalEvent::AddSignal(const TRestDetectorSignal& signal) {
    if (signalIDExists(signal.GetSignalID())) {
        cout << "Warning. Signal ID : " << signal.GetSignalID()
             << " already exists. Signal will not be added to signal event" << endl;
        return;
    }

    fSignal.emplace_back(signal);
}

Int_t TRestDetectorSignalEvent::GetSignalIndex(Int_t signalID) {
    for (int i = 0; i < GetNumberOfSignals(); i++)
        if (fSignal[i].GetSignalID() == signalID) return i;
    return -1;
}

Double_t TRestDetectorSignalEvent::GetIntegral(Int_t startBin, Int_t endBin) {
    Double_t sum = 0;

    for (int i = 0; i < GetNumberOfSignals(); i++) sum += fSignal[i].GetIntegral(startBin, endBin);

    return sum;
}

Double_t TRestDetectorSignalEvent::GetIntegralWithTime(Double_t startTime, Double_t endTime) {
    Double_t sum = 0;
    for (int n = 0; n < GetNumberOfSignals(); n++) sum += fSignal[n].GetIntegralWithTime(startTime, endTime);

    return sum;
}

/*
Double_t TRestDetectorSignalEvent::GetIntegralWithThreshold(Int_t from, Int_t to, Int_t startBaseline,
                                                    Int_t endBaseline, Double_t nSigmas,
                                                    Int_t nPointsOverThreshold, Double_t minPeakAmplitude) {
    Double_t sum = 0;

    for (int i = 0; i < GetNumberOfSignals(); i++)
        sum += fSignal[i].GetIntegralWithThreshold(from, to, startBaseline, endBaseline, nSigmas,
                                                   nPointsOverThreshold, minPeakAmplitude);

    return sum;
}
*/

void TRestDetectorSignalEvent::CalculateBaseLineAndSigmaAverage(Int_t startBin, Int_t endBin,
                                                                Double_t& baseLineAvg,
                                                                Double_t& baseLineSigmaAvg) {
    baseLineAvg = 0;
    baseLineSigmaAvg = 0;

    Double_t bL, bLS;

    for (int signal = 0; signal < GetNumberOfSignals(); signal++) {
        GetSignal(signal)->CalculateBaseLineAndSigma(startBin, endBin, bL, bLS);
        baseLineAvg += bL;
        baseLineSigmaAvg += bLS;
    }

    baseLineAvg /= GetNumberOfSignals();
    baseLineSigmaAvg /= GetNumberOfSignals();
}

void TRestDetectorSignalEvent::SubstractBaselines(Int_t startBin, Int_t endBin) {
    for (int signal = 0; signal < GetNumberOfSignals(); signal++)
        GetSignal(signal)->SubstractBaseline(startBin, endBin);
}

void TRestDetectorSignalEvent::AddChargeToSignal(Int_t signalID, Double_t time, Double_t charge) {
    Int_t signalIndex = GetSignalIndex(signalID);
    if (signalIndex == -1) {
        signalIndex = GetNumberOfSignals();

        TRestDetectorSignal signal;
        signal.SetSignalID(signalID);
        AddSignal(signal);
    }

    fSignal[signalIndex].AddDeposit(time, charge);
}

void TRestDetectorSignalEvent::PrintEvent() {
    TRestEvent::PrintEvent();

    for (int i = 0; i < GetNumberOfSignals(); i++) {
        cout << "================================================" << endl;
        cout << "Signal ID : " << fSignal[i].GetSignalID() << endl;
        cout << "Integral : " << fSignal[i].GetIntegral() << endl;
        cout << "------------------------------------------------" << endl;
        fSignal[i].Print();
        cout << "================================================" << endl;
    }
}

// TODO: GetMaxTimeFast, GetMinTimeFast, GetMaxValueFast that return the value of fMinTime, fMaxTime, etc.
void TRestDetectorSignalEvent::SetMaxAndMin() {
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;

    for (int s = 0; s < GetNumberOfSignals(); s++) {
        if (fMinTime > fSignal[s].GetMinTime()) fMinTime = fSignal[s].GetMinTime();
        if (fMaxTime < fSignal[s].GetMaxTime()) fMaxTime = fSignal[s].GetMaxTime();

        if (fMinValue > fSignal[s].GetMinValue()) fMinValue = fSignal[s].GetMinValue();
        if (fMaxValue < fSignal[s].GetMaxValue()) fMaxValue = fSignal[s].GetMaxValue();
    }
}

Double_t TRestDetectorSignalEvent::GetMaxValue() {
    SetMaxAndMin();
    return fMaxValue;
}

Double_t TRestDetectorSignalEvent::GetMinValue() {
    SetMaxAndMin();
    return fMinValue;
}

Double_t TRestDetectorSignalEvent::GetMinTime() {
    Double_t minTime = 1.e10;
    for (int s = 0; s < GetNumberOfSignals(); s++)
        if (minTime > fSignal[s].GetMinTime()) minTime = fSignal[s].GetMinTime();
    return minTime;
}

Double_t TRestDetectorSignalEvent::GetMaxTime() {
    Double_t maxTime = -1.e10;
    for (int s = 0; s < GetNumberOfSignals(); s++)
        if (maxTime < fSignal[s].GetMaxTime()) maxTime = fSignal[s].GetMaxTime();
    return maxTime;
}

// Draw current event in a TPad
TPad* TRestDetectorSignalEvent::DrawEvent(const TString& option) {
    delete fPad;

    int nSignals = this->GetNumberOfSignals();

    if (nSignals == 0) {
        cout << "Empty event " << endl;
        return nullptr;
    }

    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;

    fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);
    fPad->Draw();
    fPad->cd();
    fPad->DrawFrame(GetMinTime(), GetMinValue(), GetMaxTime(), GetMaxValue());

    char title[256];
    sprintf(title, "Event ID %d", this->GetID());

    TMultiGraph* mg = new TMultiGraph();
    mg->SetTitle(title);
    mg->GetXaxis()->SetTitle("Time [us]");
    mg->GetXaxis()->SetTitleOffset(1.1);
    mg->GetYaxis()->SetTitle("Amplitude [a.u.]");
    mg->GetYaxis()->SetTitleOffset(0.8);

    mg->GetYaxis()->SetTitleSize(1.4 * mg->GetYaxis()->GetTitleSize());
    mg->GetXaxis()->SetTitleSize(1.4 * mg->GetXaxis()->GetTitleSize());
    mg->GetYaxis()->SetLabelSize(1.25 * mg->GetYaxis()->GetLabelSize());
    mg->GetXaxis()->SetLabelSize(1.25 * mg->GetXaxis()->GetLabelSize());

    for (int n = 0; n < nSignals; n++) {
        TGraph* gr = fSignal[n].GetGraph(n + 1);
        mg->Add(gr);
    }

    fPad->cd();
    mg->Draw("");

    return fPad;
}
