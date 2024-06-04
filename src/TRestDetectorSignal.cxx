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

#include <TCanvas.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TH1.h>
#include <TMath.h>
#include <TRandom3.h>

#include <limits>

using namespace std;

ClassImp(TRestDetectorSignal);

TRestDetectorSignal::TRestDetectorSignal() {
    // TRestDetectorSignal default constructor
    fGraph = nullptr;
    fSignalID = -1;
    fSignalTime.clear();
    fSignalCharge.clear();

    fPointsOverThreshold.clear();
}

TRestDetectorSignal::~TRestDetectorSignal() = default;

void TRestDetectorSignal::NewPoint(Double_t time, Double_t data) {
    fSignalTime.push_back(time);
    fSignalCharge.push_back(data);
}

///////////////////////////////////////////////
/// \brief If the point already exists inside the detector signal event,
/// the amplitude value will be added to the corresponding time.
///
void TRestDetectorSignal::IncreaseAmplitude(Double_t t, Double_t d) { IncreaseAmplitude({t, d}); }

///////////////////////////////////////////////
/// \brief If the point already exists inside the detector signal event,
/// the amplitude value will be added to the corresponding time.
///
/// The input vector should contain a physical time and an amplitude.
///
void TRestDetectorSignal::IncreaseAmplitude(const TVector2& p) {
    Double_t x = p.X();
    Double_t y = p.Y();
    Int_t index = GetTimeIndex(x);

    if (index >= 0) {
        fSignalTime[index] = x;
        fSignalCharge[index] += y;
    } else {
        fSignalTime.push_back(x);
        fSignalCharge.push_back(y);
    }
}

///////////////////////////////////////////////
/// \brief If the point already exists inside the detector signal event,
/// it will be overwritten. If it does not exists, a new point will be
/// added to the points vector.
///
/// The input vector should contain a physical time and an amplitude.
///
void TRestDetectorSignal::SetPoint(const TVector2& p) {
    Int_t index = GetTimeIndex(p.X());
    Double_t x = p.X();
    Double_t y = p.Y();

    if (index >= 0) {
        fSignalTime[index] = x;
        fSignalCharge[index] = y;
    } else {
        fSignalTime.push_back(x);
        fSignalCharge.push_back(y);
    }
}

///////////////////////////////////////////////
/// \brief If the point already exists inside the detector signal event,
/// it will be overwritten. If it does not exists, a new point will be
/// added to the points vector.
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

Double_t TRestDetectorSignal::GetIntegral(Int_t startBin, Int_t endBin) const {
    if (startBin < 0) {
        startBin = 0;
    }
    if (endBin <= 0 || endBin > GetNumberOfPoints()) {
        endBin = GetNumberOfPoints();
    }

    Double_t sum = 0;
    for (int i = startBin; i < endBin; i++) {
        sum += GetData(i);
    }

    return sum;
}

void TRestDetectorSignal::Normalize(Double_t scale) {
    Double_t sum = GetIntegral();
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        fSignalCharge[i] = scale * GetData(i) / sum;
    }
}

Double_t TRestDetectorSignal::GetIntegralWithTime(Double_t startTime, Double_t endTime) const {
    Double_t sum = 0;
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        const auto time = GetTime(i);
        if (time >= startTime && time <= endTime) {
            sum += GetData(i);
        }
    }
    return sum;
}

Double_t TRestDetectorSignal::GetMaxPeakWithTime(Double_t startTime, Double_t endTime) const {
    Double_t max = std::numeric_limits<double>::min();
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        if (GetTime(i) >= startTime && GetTime(i) < endTime) {
            if (this->GetData(i) > max) max = GetData(i);
        }
    }
    return max;
}

/* {{{
Double_t TRestDetectorSignal::GetIntegralWithThreshold(Int_t from, Int_t to, Int_t startBaseline, Int_t
endBaseline,
                                               Double_t nSigmas, Int_t nPointsOverThreshold,
                                               Double_t nMinSigmas) {
    if (startBaseline < 0) startBaseline = 0;
    if (endBaseline <= 0 || endBaseline > GetNumberOfPoints()) endBaseline = GetNumberOfPoints();

    Double_t baseLine = GetBaseLine(startBaseline, endBaseline);

    Double_t pointThreshold = nSigmas * GetBaseLineSigma(startBaseline, endBaseline);
    Double_t signalThreshold = nMinSigmas * GetBaseLineSigma(startBaseline, endBaseline);

    return GetIntegralWithThreshold(from, to, baseLine, pointThreshold, nPointsOverThreshold,
                                    signalThreshold);
}

Double_t TRestDetectorSignal::GetIntegralWithThreshold(Int_t from, Int_t to, Double_t baseline,
                                               Double_t pointThreshold, Int_t nPointsOverThreshold,
                                               Double_t signalThreshold) {
    Double_t sum = 0;
    Int_t nPoints = 0;
    fPointsOverThreshold.clear();

    if (to > GetNumberOfPoints()) to = GetNumberOfPoints();

    Float_t maxValue = 0;
    for (int i = from; i < to; i++) {
        if (GetData(i) > baseline + pointThreshold) {
            if (GetData(i) > maxValue) maxValue = GetData(i);
            nPoints++;
        } else {
            if (nPoints >= nPointsOverThreshold) {
                Double_t sig = GetStandardDeviation(i - nPoints, i);
                if (sig > signalThreshold) {
                    for (int j = i - nPoints; j < i; j++) {
                        sum += this->GetData(j);
                        fPointsOverThreshold.push_back(j);
                    }
                }
            }
            nPoints = 0;
            maxValue = 0;
        }
    }

    if (nPoints >= nPointsOverThreshold) {
        Double_t sig = GetStandardDeviation(to - nPoints, to);
        if (sig > signalThreshold) {
            for (int j = to - nPoints; j < to; j++) {
                sum += this->GetData(j);
                fPointsOverThreshold.push_back(j);
            }
        }
    }

    return sum;
}
}}} */

Double_t TRestDetectorSignal::GetAverage(Int_t start, Int_t end) {
    this->Sort();

    if (end == 0) end = this->GetNumberOfPoints();

    Double_t sum = 0;
    for (int i = start; i <= end; i++) {
        sum += this->GetData(i);
    }
    return sum / (end - start + 1);
}

Int_t TRestDetectorSignal::GetMaxPeakWidth() {
    this->Sort();

    Int_t mIndex = this->GetMaxIndex();
    Double_t maxValue = this->GetData(mIndex);

    Double_t value = maxValue;
    Int_t rightIndex = mIndex;
    while (value > maxValue / 2) {
        value = this->GetData(rightIndex);
        rightIndex++;
    }
    Int_t leftIndex = mIndex;
    value = maxValue;
    while (value > maxValue / 2) {
        value = this->GetData(leftIndex);
        leftIndex--;
    }

    return rightIndex - leftIndex;
}

Double_t TRestDetectorSignal::GetMaxPeakValue() { return GetData(GetMaxIndex()); }

Int_t TRestDetectorSignal::GetMaxIndex(Int_t from, Int_t to) {
    Double_t max = std::numeric_limits<double>::min();
    Int_t index = 0;

    if (from < 0) from = 0;
    if (to > GetNumberOfPoints()) to = GetNumberOfPoints();

    if (to == 0) to = GetNumberOfPoints();

    for (int i = from; i < to; i++) {
        if (this->GetData(i) > max) {
            max = GetData(i);
            index = i;
        }
    }

    return index;
}

// z position by gaussian fit

TVector2
TRestDetectorSignal::GetMaxGauss()  // returns a 2vector with the time of the peak time in us and the energy
{
    Int_t maxRaw = GetMaxIndex();  // The bin where the maximum of the raw signal is found
    Double_t maxRawTime =
        GetTime(maxRaw);  // The time of the bin where the maximum of the raw signal is found
    Double_t energy = 0, time = 0;

    // Define fit limits
    Double_t threshold = GetData(maxRaw) * 0.9;  // 90% of the maximum value

    Double_t lowerLimit = maxRawTime, upperLimit = maxRawTime;

    // Find the lower limit: time where signal drops to 90% of the max before the peak
    for (int i = maxRaw; i >= 0; --i) {
        if (GetData(i) <= threshold) {
            lowerLimit = GetTime(i);
            break;
        }
    }

    // Find the upper limit: time where signal drops to 90% of the max after the peak
    for (int i = maxRaw; i < GetNumberOfPoints(); ++i) {
        if (GetData(i) <= threshold) {
            upperLimit = GetTime(i);
            break;
        }
    }

    TF1 gaus("gaus", "gaus", lowerLimit, upperLimit);
    TH1F h1("h1", "h1", GetNumberOfPoints(), GetTime(0), GetTime(GetNumberOfPoints() - 1));

    // copying the signal peak to a histogram
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        h1.SetBinContent(i + 1, GetData(i));
    }
    /*
    TCanvas* c = new TCanvas("c", "Signal fit", 200, 10, 1280, 720);
    h1->GetXaxis()->SetTitle("Time (us)");
    h1->GetYaxis()->SetTitle("Amplitude");
    h1->Draw();
    */

    TFitResultPtr fitResult =
        h1.Fit(&gaus, "QNRS");  // Q = quiet, no info in screen; N = no plot; R = fit in the function range; S
                                // = save and return the fit result

    if (fitResult->IsValid()) {
        energy = gaus.GetParameter(0);
        time = gaus.GetParameter(1);
    } else {
        // the fit failed, return -1 to indicate failure
        energy = -1;
        time = -1;
        cout << endl
             << "WARNING: bad fit to signal with ID " << GetID() << " with maximum at time = " << maxRawTime
             << " ns "
             << "\n"
             << "Failed fit parameters = " << gaus.GetParameter(0) << " || " << gaus.GetParameter(1) << " || "
             << gaus.GetParameter(2) << "\n"
             << "Assigned fit parameters : energy = " << energy << ", time = " << time << endl;
        /*
        TCanvas* c2 = new TCanvas("c2", "Signal fit", 200, 10, 1280, 720);
        h1->Draw();
        c2->Update();
        getchar();
        delete c2;
        */
    }

    TVector2 fitParam(time, energy);

    return fitParam;
}

// z position by landau fit

TVector2
TRestDetectorSignal::GetMaxLandau()  // returns a 2vector with the time of the peak time in us and the energy
{
    Int_t maxRaw = GetMaxIndex();  // The bin where the maximum of the raw signal is found
    Double_t maxRawTime =
        GetTime(maxRaw);  // The time of the bin where the maximum of the raw signal is found
    Double_t energy = 0, time = 0;
    
    // Define fit limits
    Double_t threshold = GetData(maxRaw) * 0.9;  // 90% of the maximum value

    Double_t lowerLimit = maxRawTime, upperLimit = maxRawTime;

    // Find the lower limit: time where signal drops to 90% of the max before the peak
    for (int i = maxRaw; i >= 0; --i) {
        if (GetData(i) <= threshold) {
            lowerLimit = GetTime(i);
            break;
        }
    }

    // Find the upper limit: time where signal drops to 90% of the max after the peak
    for (int i = maxRaw; i < GetNumberOfPoints(); ++i) {
        if (GetData(i) <= threshold) {
            upperLimit = GetTime(i);
            break;
        }
    }

    TF1 landau("landau", "landau", lowerLimit, upperLimit);
    TH1F h1("h1", "h1", GetNumberOfPoints(), GetTime(0), GetTime(GetNumberOfPoints() - 1));

    // copying the signal peak to a histogram
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        h1.SetBinContent(i + 1, GetData(i));
    }

    TFitResultPtr fitResult =
        h1.Fit(&landau, "QNRS");  // Q = quiet, no info in screen; N = no plot; R = fit in the function range;
                                  // S = save and return the fit result
    if (fitResult->IsValid()) {
        energy = landau.GetParameter(0);
        time = landau.GetParameter(1);
    } else {
        // the fit failed, return -1 to indicate failure
        energy = -1;
        time = -1;
        cout << endl
             << "WARNING: bad fit to signal with ID " << GetID() << " with maximum at time = " << maxRawTime
             << " ns "
             << "\n"
             << "Failed fit parameters = " << landau.GetParameter(0) << " || " << landau.GetParameter(1)
             << " || " << landau.GetParameter(2) << "\n"
             << "Assigned fit parameters : energy = " << energy << ", time = " << time << endl;
        /*
        TCanvas* c2 = new TCanvas("c2", "Signal fit", 200, 10, 1280, 720);
        h1->Draw();
        c2->Update();
        getchar();
        delete c2;
        */
    }

    TVector2 fitParam(time, energy);

    return fitParam;
}

// z position by aget fit

Double_t agetResponseFunction(Double_t* x, Double_t* par) {  // x contains as many elements as the number of
                                                             // dimensions (in this case 1, i.e. x[0]), and
    // par contains as many elements as the number of free parameters in my function.

    Double_t arg =
        (x[0] - par[1] + 1.1664) /
        par[2];  // 1.1664 is the x value where the maximum of the base function (i.e. without parameters) is.
    Double_t f = par[0] / 0.0440895 * exp(-3 * (arg)) * (arg) * (arg) *
                 (arg)*sin(arg);  // to rescale the Y axis and get amplitude.
    return f;
}

TVector2
TRestDetectorSignal::GetMaxAget()  // returns a 2vector with the time of the peak time in us and the energy
{
    Int_t maxRaw = GetMaxIndex();  // The bin where the maximum of the raw signal is found
    Double_t maxRawTime =
        GetTime(maxRaw);  // The time of the bin where the maximum of the raw signal is found
    Double_t energy = 0, time = 0;
    
    // Define fit limits
    Double_t threshold = GetData(maxRaw) * 0.9;  // 90% of the maximum value

    Double_t lowerLimit = maxRawTime, upperLimit = maxRawTime;

    // Find the lower limit: time where signal drops to 90% of the max before the peak
    for (int i = maxRaw; i >= 0; --i) {
        if (GetData(i) <= threshold) {
            lowerLimit = GetTime(i);
            break;
        }
    }

    // Find the upper limit: time where signal drops to 90% of the max after the peak
    for (int i = maxRaw; i < GetNumberOfPoints(); ++i) {
        if (GetData(i) <= threshold) {
            upperLimit = GetTime(i);
            break;
        }
    }

    TF1 aget("aget", agetResponseFunction, lowerLimit, upperLimit, 3);  //
    TH1F h1("h1", "h1", GetNumberOfPoints(), GetTime(0), GetTime(GetNumberOfPoints() - 1));
    aget.SetParameters(500, maxRawTime, 1.2);

    // copying the signal peak to a histogram
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        h1.SetBinContent(i + 1, GetData(i));
    }

    TFitResultPtr fitResult =
        h1.Fit(&aget, "QNRS");  // Q = quiet, no info in screen; N = no plot; R = fit in
                                // the function range; S = save and return the fit result

    if (fitResult->IsValid()) {
        energy = aget.GetParameter(0);
        time = aget.GetParameter(1);
    } else {
        // the fit failed, return -1 to indicate failure
        energy = -1;
        time = -1;
        cout << endl
             << "WARNING: bad fit to signal with ID " << GetID() << " with maximum at time = " << maxRawTime
             << " ns "
             << "\n"
             << "Failed fit parameters = " << aget.GetParameter(0) << " || " << aget.GetParameter(1) << " || "
             << aget.GetParameter(2) << "\n"
             << "Assigned fit parameters : energy = " << energy << ", time = " << time << endl;
    }

    TVector2 fitParam(time, energy);

    return fitParam;
}
Double_t TRestDetectorSignal::GetMaxPeakTime(Int_t from, Int_t to) { return GetTime(GetMaxIndex(from, to)); }

Double_t TRestDetectorSignal::GetMinPeakValue() { return GetData(GetMinIndex()); }

Int_t TRestDetectorSignal::GetMinIndex() const {
    Double_t min = numeric_limits<Double_t>::max();
    Int_t index = 0;

    for (int i = 0; i < GetNumberOfPoints(); i++) {
        if (this->GetData(i) < min) {
            min = GetData(i);
            index = i;
        }
    }

    return index;
}

Double_t TRestDetectorSignal::GetMinTime() const {
    if (GetNumberOfPoints() == 0) {
        return 0;
    }
    Double_t minTime = numeric_limits<Double_t>::max();
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        const Double_t time = GetTime(i);
        if (time < minTime) {
            minTime = time;
        }
    }
    return minTime;
}

Double_t TRestDetectorSignal::GetMaxTime() const {
    if (GetNumberOfPoints() == 0) {
        return 0;
    }
    Double_t maxTime = numeric_limits<Double_t>::min();
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        const auto time = GetTime(i);
        if (time > maxTime) {
            maxTime = time;
        }
    }
    return maxTime;
}

Int_t TRestDetectorSignal::GetTimeIndex(Double_t t) {
    Double_t time = t;

    for (int n = 0; n < GetNumberOfPoints(); n++) {
        if (time == fSignalTime[n]) {
            return n;
        }
    }
    return -1;
}

Bool_t TRestDetectorSignal::isSorted() {
    for (int i = 0; i < GetNumberOfPoints() - 1; i++) {
        if (GetTime(i + 1) < GetTime(i)) {
            return false;
        }
    }
    return true;
}

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

void TRestDetectorSignal::GetDifferentialSignal(TRestDetectorSignal* diffSignal, Int_t smearPoints) {
    this->Sort();

    for (int i = 0; i < smearPoints; i++) diffSignal->IncreaseAmplitude(GetTime(i), 0);

    for (int i = smearPoints; i < this->GetNumberOfPoints() - smearPoints; i++) {
        Double_t value = (this->GetData(i + smearPoints) - GetData(i - smearPoints)) /
                         (GetTime(i + smearPoints) - GetTime(i - smearPoints));
        Double_t time = (GetTime(i + smearPoints) + GetTime(i - smearPoints)) / 2.;

        diffSignal->IncreaseAmplitude(time, value);
    }

    for (int i = GetNumberOfPoints() - smearPoints; i < GetNumberOfPoints(); i++) {
        diffSignal->IncreaseAmplitude(GetTime(i), 0);
    }
}

void TRestDetectorSignal::GetSignalDelayed(TRestDetectorSignal* delayedSignal, Int_t delay) {
    this->Sort();

    for (int i = 0; i < delay; i++) delayedSignal->IncreaseAmplitude(GetTime(i), GetData(i));

    for (int i = delay; i < GetNumberOfPoints(); i++) {
        delayedSignal->IncreaseAmplitude(GetTime(i), GetData(i - delay));
    }
}

void TRestDetectorSignal::GetSignalSmoothed(TRestDetectorSignal* smthSignal, Int_t averagingPoints) {
    this->Sort();

    averagingPoints = (averagingPoints / 2) * 2 + 1;  // make it odd >= averagingPoints

    Double_t sum = GetIntegral(0, averagingPoints);
    for (int i = 0; i <= averagingPoints / 2; i++)
        smthSignal->IncreaseAmplitude(GetTime(i), sum / averagingPoints);

    for (int i = averagingPoints / 2 + 1; i < GetNumberOfPoints() - averagingPoints / 2; i++) {
        sum -= this->GetData(i - (averagingPoints / 2 + 1));
        sum += this->GetData(i + averagingPoints / 2);
        smthSignal->IncreaseAmplitude(this->GetTime(i), sum / averagingPoints);
    }

    for (int i = GetNumberOfPoints() - averagingPoints / 2; i < GetNumberOfPoints(); i++)
        smthSignal->IncreaseAmplitude(GetTime(i), sum / averagingPoints);
}

Double_t TRestDetectorSignal::GetBaseLine(Int_t startBin, Int_t endBin) {
    if (endBin - startBin <= 0) return 0.;

    Double_t baseLine = 0;
    for (int i = startBin; i < endBin; i++) baseLine += fSignalCharge[i];

    return baseLine / (endBin - startBin);
}

Double_t TRestDetectorSignal::GetStandardDeviation(Int_t startBin, Int_t endBin) {
    Double_t bL = GetBaseLine(startBin, endBin);
    return GetBaseLineSigma(startBin, endBin, bL);
}

Double_t TRestDetectorSignal::GetBaseLineSigma(Int_t startBin, Int_t endBin, Double_t baseline) {
    Double_t bL = baseline;
    if (bL == 0) bL = GetBaseLine(startBin, endBin);

    Double_t baseLineSigma = 0;
    for (int i = startBin; i < endBin; i++)
        baseLineSigma += (bL - fSignalCharge[i]) * (bL - fSignalCharge[i]);

    return TMath::Sqrt(baseLineSigma / (endBin - startBin));
}

Double_t TRestDetectorSignal::SubstractBaseline(Int_t startBin, Int_t endBin) {
    Double_t bL = GetBaseLine(startBin, endBin);

    AddOffset(-bL);

    return bL;
}

void TRestDetectorSignal::AddOffset(Double_t offset) {
    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] = fSignalCharge[i] + offset;
}

void TRestDetectorSignal::MultiplySignalBy(Double_t factor) {
    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] = factor * fSignalCharge[i];
}

void TRestDetectorSignal::ExponentialConvolution(Double_t fromTime, Double_t decayTime, Double_t offset) {
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        if (fSignalTime[i] > fromTime) {
            fSignalCharge[i] =
                (fSignalCharge[i] - offset) * exp(-(fSignalTime[i] - fromTime) / decayTime) + offset;
        }
    }
}

void TRestDetectorSignal::SignalAddition(TRestDetectorSignal* inSignal) {
    if (this->GetNumberOfPoints() != inSignal->GetNumberOfPoints()) {
        cout << "ERROR : I cannot add two signals with different number of points" << endl;
        return;
    }

    Int_t badSignalTimes = 0;

    for (int i = 0; i < GetNumberOfPoints(); i++)
        if (GetTime(i) != inSignal->GetTime(i)) {
            cout << "Time : " << GetTime(i) << " != " << inSignal->GetTime(i) << endl;
            badSignalTimes++;
        }

    if (badSignalTimes) {
        cout << "ERROR : The times of signal addition must be the same" << endl;
        return;
    }

    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] += inSignal->GetData(i);
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

void TRestDetectorSignal::GetWhiteNoiseSignal(TRestDetectorSignal* noiseSignal, Double_t noiseLevel) {
    this->Sort();

    for (int i = 0; i < GetNumberOfPoints(); i++) {
        TRandom3* fRandom = new TRandom3(0);

        noiseSignal->IncreaseAmplitude(GetTime(i), GetData(i) + fRandom->Gaus(0, noiseLevel));

        delete fRandom;
    }
}

void TRestDetectorSignal::GetSignalGaussianConvolution(TRestDetectorSignal* convSignal, Double_t sigma,
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

            convSignal->IncreaseAmplitude(i, sum);
            totChargeFinal += sum;
        }
    }

    cout << "Initial charge of the pulse " << totChargeInitial << endl;
    cout << "Final charge of the pulse " << totChargeFinal << endl;
}

void TRestDetectorSignal::WriteSignalToTextFile(const TString& filename) {
    FILE* fff = fopen(filename.Data(), "w");
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        fprintf(fff, "%e\t%e\n", GetTime(i), GetData(i));
    }
    fclose(fff);
}

void TRestDetectorSignal::Print() const {
    cout << "Signal ID : " << GetSignalID() << endl;
    cout << "Integral : " << GetIntegral() << endl;
    if (!GetSignalName().empty()) {
        cout << "Name: " << GetSignalName() << endl;
    }
    if (!GetSignalType().empty()) {
        cout << "Type: " << GetSignalType() << endl;
    }

    cout << "------------------------------------------------" << endl;
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        cout << "Time : " << GetTime(i) << " Charge : " << GetData(i) << endl;
    }

    cout << "================================================" << endl;
}

TGraph* TRestDetectorSignal::GetGraph(Int_t color) {
    if (fGraph != nullptr) {
        delete fGraph;
        fGraph = nullptr;
    }

    fGraph = new TGraph();

    //   cout << "Signal ID " << this->GetSignalID( ) << " points " <<
    //   this->GetNumberOfPoints() << endl;

    fGraph->SetLineWidth(2);
    fGraph->SetLineColor(color);
    fGraph->SetMarkerStyle(7);

    int points = 0;
    for (int n = 0; n < GetNumberOfPoints(); n++) {
        fGraph->SetPoint(points, GetTime(n), GetData(n));
        points++;
    }

    return fGraph;
}
