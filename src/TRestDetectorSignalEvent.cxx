//////////////////////////////////////////////////////////////////////////
/// TRestDetectorSignalEvent stores the detector-channel signals associated
/// with one REST event.
///
/// A TRestDetectorSignalEvent is a collection of TRestDetectorSignal objects.
/// Each TRestDetectorSignal corresponds to one readout channel, identified by
/// its signal ID, and stores the signal as time-charge points in physical
/// coordinates. The event object groups those channel signals and provides
/// event-level operations such as signal lookup, drawing, sorting, integrals,
/// and global time or amplitude limits.
///
/// This event type is usually found after the raw ADC representation has been
/// converted into detector-level signals, or after simulated detector hits have
/// been projected onto a readout. It sits between TRestRawSignalEvent, which
/// keeps ADC samples, and TRestDetectorHitsEvent, which stores reconstructed or
/// simulated spatial hits. Processes such as TRestDetectorHitsToSignalProcess,
/// TRestRawToDetectorSignalProcess, and TRestDetectorSignalToHitsProcess use
/// this class as the detector-signal representation.
///
/// A minimal inspection from a `restRoot` session or ROOT macro looks like:
///
/// \code
/// TRestRun run("detectorSignals.root");
/// run.GetEntry(0);
///
/// auto signalEvent = run.GetInputEvent<TRestDetectorSignalEvent>();
/// std::cout << "Signals in event: " << signalEvent->GetNumberOfSignals()
///           << std::endl;
///
/// auto signal = signalEvent->GetSignalById(12);
/// if (signal != nullptr) {
///     std::cout << "Signal 12 integral: " << signal->GetIntegral()
///               << std::endl;
/// }
///
/// signalEvent->DrawEvent();
/// \endcode
///
/// Signals can be accessed by collection index with GetSignal(), or by channel
/// identifier with GetSignalById(). Analysis code should normally use
/// GetSignalById() when referring to a physical readout channel, because the
/// internal ordering of the signal collection is not the channel mapping.
///
/// AddChargeToSignal() is a convenient way to fill an event when several charge
/// contributions may arrive at the same channel and time. It creates the
/// corresponding TRestDetectorSignal if the signal ID is not yet present, and
/// otherwise accumulates the charge in the existing signal.
///
/// DrawEvent() displays all contained TRestDetectorSignal objects in a
/// TMultiGraph, with time on the horizontal axis and amplitude on the vertical
/// axis.
///
/// The following figure shows a detector signal event drawn with DrawEvent().
/// Each trace corresponds to one TRestDetectorSignal in the event, with the
/// horizontal axis expressed in physical time.
///
/// \htmlonly <style>div.image img[src="detector_signal_event.png"]{width:650px;}</style> \endhtmlonly
///
/// ![Detector signal event drawn with DrawEvent](detector_signal_event.png)
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2015-September: First concept.
///                 JuanAn Garcia/Javier Galan
///
/// 2016-February: Added titles to axis in DrawGraph using TMultiGraph.
///                Javier Gracia
///
/// \class      TRestDetectorSignalEvent
/// \author     JuanAn Garcia
/// \author     Javier Galan
///
/// <hr>
///

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
    fMinValue = std::numeric_limits<Double_t>::max();
    fMaxValue = std::numeric_limits<Double_t>::min();
    fMinTime = std::numeric_limits<Double_t>::max();
    fMaxTime = std::numeric_limits<Double_t>::min();
}

void TRestDetectorSignalEvent::AddSignal(const TRestDetectorSignal& signal) {
    if (signalIDExists(signal.GetSignalID())) {
        cout << "Warning. Signal ID : " << signal.GetSignalID()
             << " already exists. Signal will not be added to signal event" << endl;
        return;
    }

    fSignal.emplace_back(signal);
}

void TRestDetectorSignalEvent::RemoveSignalWithId(Int_t sId) {
    Int_t index = GetSignalIndex(sId);

    if (index == -1) {
        std::cout << "Warning. Signal ID : " << sId
                  << " does not exist. Signal will not be removed from signal event" << std::endl;
        return;
    }

    fSignal.erase(fSignal.begin() + index);
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

void TRestDetectorSignalEvent::AddChargeToSignal(Int_t signalID, Double_t time, Double_t charge) {
    Int_t signalIndex = GetSignalIndex(signalID);
    if (signalIndex == -1) {
        signalIndex = GetNumberOfSignals();
        TRestDetectorSignal signal;
        signal.SetSignalID(signalID);
        AddSignal(signal);
    }

    fSignal[signalIndex].IncreaseAmplitude(time, charge);
}

void TRestDetectorSignalEvent::PrintEvent() {
    TRestEvent::PrintEvent();

    for (int i = 0; i < GetNumberOfSignals(); i++) {
        const auto& signal = fSignal[i];
        signal.Print();
    }
}

// TODO: GetMaxTimeFast, GetMinTimeFast, GetMaxValueFast that return the value of fMinTime, fMaxTime, etc.
void TRestDetectorSignalEvent::SetMaxAndMin() {
    fMinValue = std::numeric_limits<Double_t>::max();
    fMaxValue = std::numeric_limits<Double_t>::min();
    fMinTime = std::numeric_limits<Double_t>::max();
    fMaxTime = std::numeric_limits<Double_t>::min();

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
    Double_t minTime = numeric_limits<Double_t>::max();
    for (int s = 0; s < GetNumberOfSignals(); s++) {
        if (minTime > fSignal[s].GetMinTime()) {
            minTime = fSignal[s].GetMinTime();
        }
    }
    return minTime;
}

Double_t TRestDetectorSignalEvent::GetMaxTime() {
    Double_t maxTime = numeric_limits<Double_t>::min();
    for (int s = 0; s < GetNumberOfSignals(); s++) {
        if (maxTime < fSignal[s].GetMaxTime()) {
            maxTime = fSignal[s].GetMaxTime();
        }
    }
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

    fMinValue = std::numeric_limits<Double_t>::max();
    fMaxValue = std::numeric_limits<Double_t>::min();
    fMinTime = std::numeric_limits<Double_t>::max();
    fMaxTime = std::numeric_limits<Double_t>::min();

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
