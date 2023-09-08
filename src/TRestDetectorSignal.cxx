///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignal.cxx
///
///             Event class to store signals from simulation and acquisition
///             events
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia/Javier Galan
///		nov 2015:
///		    Changed vectors fSignalTime and fSignalCharge from <Int_t>
/// to
///< Float_t> 	            JuanAn Garcia
//      dec 2015:
//
//              Javier Galan
///_______________________________________________________________________________

#include "TRestDetectorSignal.h"

#include "TFitResult.h"
#include "TRestPulseShapeAnalysis.h"
using namespace std;

#include <TF1.h>
#include <TH1.h>
#include <TMath.h>
#include <TRandom3.h>

#include "TCanvas.h"

ClassImp(TRestDetectorSignal);

TRestDetectorSignal::TRestDetectorSignal() {
    // TRestDetectorSignal default constructor
    fGraph = nullptr;
    fSignalID = -1;
    fSignalTime.clear();
    fSignalCharge.clear();
}

TRestDetectorSignal::~TRestDetectorSignal() {
    // TRestDetectorSignal destructor
}

void TRestDetectorSignal::NewPoint(Float_t time, Float_t data) {
    fSignalTime.push_back(time);
    fSignalCharge.push_back(data);
}

///////////////////////////////////////////////
/// \brief If the point already exists inside the detector signal event,
/// the amplitude value will be added to the corresponding time.
///
void TRestDetectorSignal::IncreaseAmplitude(Double_t t, Double_t d) {
    TVector2 p(t, d);
    IncreaseAmplitude(p);
}

///////////////////////////////////////////////
/// \brief If the point already exists inside the detector signal event,
/// the amplitude value will be added to the corresponding time.
///
/// The input vector should contain a physical time and an amplitude.
///
void TRestDetectorSignal::IncreaseAmplitude(const TVector2& p) {
    Int_t index = GetTimeIndex(p.X());

    if (index >= 0) {
        fSignalTime[index] = p.X();
        fSignalCharge[index] += p.Y();
    } else {
        fSignalTime.push_back(p.X());
        fSignalCharge.push_back(p.Y());
    }
}

///////////////////////////////////////////////
/// \brief If the point already exists inside the detector signal event,
/// it will be overwritten. If it does not exists, a new point will be
/// added to the poins vector.
///
/// The input vector should contain a physical time and an amplitude.
///
void TRestDetectorSignal::SetPoint(const TVector2& p) {
    Int_t index = GetTimeIndex(p.X());

    if (index >= 0) {
        fSignalTime[index] = p.X();
        fSignalCharge[index] = p.Y();
    } else {
        fSignalTime.push_back(p.X());
        fSignalCharge.push_back(p.Y());
    }
}

///////////////////////////////////////////////
/// \brief If the point already exists inside the detector signal event,
/// it will be overwritten. If it does not exists, a new point will be
/// added to the poins vector.
///
/// In this method the time and amplitude (data) are given as argument
///
void TRestDetectorSignal::SetPoint(Double_t t, Double_t d) {
    TVector2 p(t, d);
    SetPoint(p);
}

///////////////////////////////////////////////
/// \brief If replaces the time and amplitude of the point at the
/// given index
///
void TRestDetectorSignal::SetPoint(Int_t index, Double_t t, Double_t d) {
    fSignalTime[index] = t;
    fSignalCharge[index] = d;
}

Double_t TRestDetectorSignal::GetIntegral(Int_t startBin, Int_t endBin) {
    if (startBin < 0) startBin = 0;
    if (endBin <= 0 || endBin > GetNumberOfPoints()) endBin = GetNumberOfPoints();

    return TRestPulseShapeAnalysis::GetIntegral(fSignalCharge, startBin, endBin);
}

void TRestDetectorSignal::Normalize(Double_t scale) {
    Double_t sum = GetIntegral();

    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] = scale * GetData(i) / sum;
}

Double_t TRestDetectorSignal::GetIntegralWithTime(Double_t startTime, Double_t endTime) {
    Double_t sum = 0;
    for (int i = 0; i < GetNumberOfPoints(); i++)
        if (GetTime(i) >= startTime && GetTime(i) < endTime) sum += GetData(i);

    return sum;
}

Double_t TRestDetectorSignal::GetAverage(Int_t start, Int_t end) {
    this->Sort();

    if (end <= 0) end = this->GetNumberOfPoints();

    return TRestPulseShapeAnalysis::GetAverage(fSignalCharge, start, end);
}

Int_t TRestDetectorSignal::GetMaxPeakWidth() {
    this->Sort();

    return TRestPulseShapeAnalysis::GetMaxPeakWidth(fSignalCharge);
}

Double_t TRestDetectorSignal::GetMaxPeakValue() { return GetData(GetMaxIndex()); }

TVector2
TRestDetectorSignal::GetMaxGauss()  // returns a 2vector with the time of the peak time in us and the energy
{
    auto gr = GetGraph();
    return TRestPulseShapeAnalysis::GetMaxGauss(gr);
}

// z position by landau fit

TVector2
TRestDetectorSignal::GetMaxLandau()  // returns a 2vector with the time of the peak time in us and the energy
{
    auto gr = GetGraph();
    return TRestPulseShapeAnalysis::GetMaxLandau(gr);
}

TVector2
TRestDetectorSignal::GetMaxAget()  // returns a 2vector with the time of the peak time in us and the energy
{
    auto gr = GetGraph();
    return TRestPulseShapeAnalysis::GetMaxAget(gr);
}

Double_t TRestDetectorSignal::GetMaxPeakTime() { return GetTime(GetMaxIndex()); }

Double_t TRestDetectorSignal::GetMinPeakValue() { return GetData(GetMinIndex()); }

Int_t TRestDetectorSignal::GetMaxIndex() { return TRestPulseShapeAnalysis::GetMaxBin(fSignalCharge); }

Int_t TRestDetectorSignal::GetMinIndex() { return TRestPulseShapeAnalysis::GetMinBin(fSignalCharge); }

Double_t TRestDetectorSignal::GetMinTime() {
    int index = TRestPulseShapeAnalysis::GetMinBin(fSignalTime);
    return GetTime(index);
}

Double_t TRestDetectorSignal::GetMaxTime() {
    int index = TRestPulseShapeAnalysis::GetMaxBin(fSignalTime);
    return GetTime(index);
}

Int_t TRestDetectorSignal::GetTimeIndex(Double_t t) {
    Float_t time = t;

    for (int n = 0; n < GetNumberOfPoints(); n++)
        if (time == fSignalTime[n]) return n;
    return -1;
}

Bool_t TRestDetectorSignal::isSorted() { return is_sorted(fSignalTime.begin(), fSignalTime.end()); }

void TRestDetectorSignal::Sort() {
    while (!isSorted()) {
        for (int i = 0; i < GetNumberOfPoints(); i++) {
            for (int j = i; j < GetNumberOfPoints(); j++) {
                if (GetTime(i) > GetTime(j)) {
                    iter_swap(fSignalTime.begin() + i, fSignalTime.begin() + j);
                    iter_swap(fSignalCharge.begin() + i, fSignalCharge.begin() + j);
                }
            }
        }
    }
}

void TRestDetectorSignal::GetDifferentialSignal(TRestDetectorSignal* diffSgnl, Int_t smearPoints) {
    this->Sort();

    for (int i = 0; i < smearPoints; i++) diffSgnl->IncreaseAmplitude(GetTime(i), 0);

    for (int i = smearPoints; i < this->GetNumberOfPoints() - smearPoints; i++) {
        Double_t value = (this->GetData(i + smearPoints) - GetData(i - smearPoints)) /
                         (GetTime(i + smearPoints) - GetTime(i - smearPoints));
        Double_t time = (GetTime(i + smearPoints) + GetTime(i - smearPoints)) / 2.;

        diffSgnl->IncreaseAmplitude(time, value);
    }

    for (int i = GetNumberOfPoints() - smearPoints; i < GetNumberOfPoints(); i++)
        diffSgnl->IncreaseAmplitude(GetTime(i), 0);
}

void TRestDetectorSignal::GetSignalDelayed(TRestDetectorSignal* delayedSignal, Int_t delay) {
    this->Sort();

    for (int i = 0; i < delay; i++) delayedSignal->IncreaseAmplitude(GetTime(i), GetData(i));

    for (int i = delay; i < GetNumberOfPoints(); i++)
        delayedSignal->IncreaseAmplitude(GetTime(i), GetData(i - delay));
}

void TRestDetectorSignal::GetSignalSmoothed(TRestDetectorSignal* smthSignal, Int_t averagingPoints) {
    this->Sort();

    auto smoothed = TRestPulseShapeAnalysis::GetSignalSmoothed(fSignalCharge, averagingPoints);

    for (int i = 0; i < GetNumberOfPoints(); i++) smthSignal->IncreaseAmplitude(GetTime(i), smoothed[i]);
}

void TRestDetectorSignal::AddOffset(Double_t offset) {
    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] = fSignalCharge[i] + offset;
}

void TRestDetectorSignal::MultiplySignalBy(Double_t factor) {
    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] = factor * fSignalCharge[i];
}

void TRestDetectorSignal::ExponentialConvolution(Double_t fromTime, Double_t decayTime, Double_t offset) {
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        if (fSignalTime[i] > fromTime)
            fSignalCharge[i] =
                (fSignalCharge[i] - offset) * exp(-(fSignalTime[i] - fromTime) / decayTime) + offset;
    }
}

void TRestDetectorSignal::SignalAddition(TRestDetectorSignal* inSgnl) {
    if (this->GetNumberOfPoints() != inSgnl->GetNumberOfPoints()) {
        cout << "ERROR : I cannot add two signals with different number of points" << endl;
        return;
    }

    Int_t badSignalTimes = 0;

    for (int i = 0; i < GetNumberOfPoints(); i++)
        if (GetTime(i) != inSgnl->GetTime(i)) {
            cout << "Time : " << GetTime(i) << " != " << inSgnl->GetTime(i) << endl;
            badSignalTimes++;
        }

    if (badSignalTimes) {
        cout << "ERROR : The times of signal addition must be the same" << endl;
        return;
    }

    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] += inSgnl->GetData(i);
}

void TRestDetectorSignal::AddGaussianSignal(Double_t amp, Double_t sigma, Double_t time, Int_t N,
                                            Double_t fromTime, Double_t toTime) {
    for (int i = 0; i < N; i++) {
        Double_t tme = fromTime + (double)i / (N - 1) * (toTime - fromTime);

        Double_t dta = 300 + amp * TMath::Exp(-0.5 * (tme - time) * (tme - time) / sigma / sigma);

        cout << "T : " << tme << " D : " << dta << endl;
        IncreaseAmplitude(tme, dta);
    }
}

void TRestDetectorSignal::GetWhiteNoiseSignal(TRestDetectorSignal* noiseSgnl, Double_t noiseLevel) {
    this->Sort();

    for (int i = 0; i < GetNumberOfPoints(); i++) {
        TRandom3 fRandom(0);
        noiseSgnl->IncreaseAmplitude(GetTime(i), GetData(i) + fRandom.Gaus(0, noiseLevel));
    }
}

void TRestDetectorSignal::GetSignalGaussianConvolution(TRestDetectorSignal* convSgnl, Double_t sigma,
                                                       Int_t nSigmas) {
    this->Sort();

    Int_t nPoints = GetMaxTime() - GetMinTime();
    TF1* fGaus = new TF1("fGauss", "exp(-0.5*((x-[1])/[2])**2)", -nPoints, nPoints);
    sigma = sigma * 1000.;          // conversion to nanoseconds
    fGaus->SetParameter(2, sigma);  // the width of the gaussian is set

    Double_t totChargeInitial = 0.;
    Double_t totChargeFinal = 0.;

    Double_t sum;

    // We calculate the charge of the event before convolution
    for (int i = 0; i < GetNumberOfPoints(); i++) totChargeInitial += fSignalCharge[i];

    // The gaussian convolution of the initial signal is performed
    for (int i = GetMinTime() - nSigmas * sigma; i < GetMaxTime() + nSigmas * sigma; i++) {
        for (int j = 0; j < GetNumberOfPoints(); j++) {
            if (TMath::Abs(i - GetTime(j)) > nSigmas * sigma) continue;
            if (TMath::Abs(i - GetTime(j)) > nSigmas * sigma && i < GetTime(j)) break;

            fGaus->SetParameter(1, GetTime(j));
            sum = fSignalCharge[j] / TMath::Sqrt(2. * TMath::Pi()) / sigma * fGaus->Integral(i, i + 1);

            convSgnl->IncreaseAmplitude(i, sum);
            totChargeFinal += sum;
        }
    }

    cout << "Initial charge of the pulse " << totChargeInitial << endl;
    cout << "Final charge of the pulse " << totChargeFinal << endl;
}

void TRestDetectorSignal::WriteSignalToTextFile(TString filename) {
    FILE* fff = fopen(filename.Data(), "w");
    for (int i = 0; i < GetNumberOfPoints(); i++) fprintf(fff, "%e\t%e\n", GetTime(i), GetData(i));
    fclose(fff);
}

void TRestDetectorSignal::Print() {
    for (int i = 0; i < GetNumberOfPoints(); i++)
        cout << "Time : " << GetTime(i) << " Charge : " << GetData(i) << endl;
}

TGraph* TRestDetectorSignal::GetGraph(Int_t color) {
    if (fGraph != nullptr) {
        delete fGraph;
        fGraph = nullptr;
    }

    fGraph = new TGraph(GetNumberOfPoints(), fSignalTime.data(), fSignalCharge.data());

    //   cout << "Signal ID " << this->GetSignalID( ) << " points " <<
    //   this->GetNumberOfPoints() << endl;

    fGraph->SetLineWidth(2);
    fGraph->SetLineColor(color);
    fGraph->SetMarkerStyle(7);

    return fGraph;
}
