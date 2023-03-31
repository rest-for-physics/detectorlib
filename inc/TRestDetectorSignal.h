///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignal.h
///
///             Event class to store signals fromm simulation and acquisition
///             events
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia/Javier Galan
///		nov 2015:
///		    Changed vectors fSignalTime and fSignalCharge from <Int_t> to
///< Float_t> 	            JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorSignal
#define RestCore_TRestDetectorSignal

#include <TGraph.h>
#include <TString.h>
#include <TVector2.h>

#include <iostream>

class TRestDetectorSignal {
   private:
    Int_t GetMinIndex();
    Int_t GetTimeIndex(Double_t t);

   protected:
    Int_t fSignalID = -1;

    std::vector<Float_t> fSignalTime;    // Vector with the time of the signal
    std::vector<Float_t> fSignalCharge;  // Vector with the charge of the signal

   public:
    TGraph* fGraph;  //!

    Int_t GetMaxIndex();

    TVector2 GetMaxGauss();
    TVector2 GetMaxLandau();
    TVector2 GetMaxAget();

    // Getters
    TVector2 GetPoint(Int_t n) {
        TVector2 vector2(GetTime(n), GetData(n));

        return vector2;
    }

    inline Int_t GetSignalID() const { return fSignalID; }
    inline Int_t GetID() const { return fSignalID; }

    void IncreaseTimeBinBy(Int_t bin, Double_t data) {
        if (bin >= GetNumberOfPoints()) {
            std::cout << "Increase time bin: outside limits" << std::endl;
            return;
        }

        fSignalCharge[bin] += data;
    }

    Int_t GetNumberOfPoints() {
        if (fSignalTime.size() != fSignalCharge.size()) {
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "WARNING, the two std::vector sizes did not match" << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }
        return fSignalTime.size();
    }

    Double_t GetIntegralWithTime(Double_t startTime, Double_t endTime);
    Double_t GetIntegral(Int_t startBin = 0, Int_t endBin = 0);

    void Normalize(Double_t scale = 1.);

    Double_t GetAverage(Int_t start = 0, Int_t end = 0);
    Int_t GetMaxPeakWidth();

    Double_t GetMaxPeakValue();
    Double_t GetMinPeakValue();

    Double_t GetMaxPeakTime();

    Double_t GetMaxValue() { return GetMaxPeakValue(); }
    Double_t GetMinValue() { return GetMinPeakValue(); }

    Double_t GetMinTime();
    Double_t GetMaxTime();

    Double_t GetData(Int_t index) { return (double)fSignalCharge[index]; }
    Double_t GetTime(Int_t index) { return (double)fSignalTime[index]; }

    // Setters
    void SetSignalID(Int_t sID) { fSignalID = sID; }
    void SetID(Int_t sID) { fSignalID = sID; }

    void NewPoint(Float_t time, Float_t data);
    void IncreaseAmplitude(const TVector2& p);
    void IncreaseAmplitude(Double_t t, Double_t d);

    void SetPoint(Double_t t, Double_t d);
    void SetPoint(const TVector2& p);
    void SetPoint(Int_t index, Double_t t, Double_t d);

    void CalculateBaseLineAndSigma(Int_t startBin, Int_t endBin, Double_t& baseline, Double_t& baseLineSigma);

    void SubstractBaseline(Int_t startBin, Int_t endBin);
    void AddOffset(Double_t offset);

    void MultiplySignalBy(Double_t factor);
    void ExponentialConvolution(Double_t fromTime, Double_t decayTime, Double_t offset = 0);
    void SignalAddition(TRestDetectorSignal* inSgnl);

    Bool_t isSorted();
    void Sort();

    void GetDifferentialSignal(TRestDetectorSignal* diffSgnl, Int_t smearPoints = 5);
    void GetSignalDelayed(TRestDetectorSignal* delayedSignal, Int_t delay);
    void GetSignalSmoothed(TRestDetectorSignal* smthSignal, Int_t averagingPoints = 3);

    void GetWhiteNoiseSignal(TRestDetectorSignal* noiseSgnl, Double_t noiseLevel = 1.);
    void GetSignalGaussianConvolution(TRestDetectorSignal* convSgnl, Double_t sigma = 100.,
                                      Int_t nSigmas = 5);

    void AddGaussianSignal(Double_t amp, Double_t sigma, Double_t time, Int_t N, Double_t fromTime,
                           Double_t toTime);

    void Reset() {
        fSignalTime.clear();
        fSignalCharge.clear();
    }

    void WriteSignalToTextFile(TString filename);
    void Print();

    TGraph* GetGraph(Int_t color = 1);

    // Constructor
    TRestDetectorSignal();
    // Destructor
    ~TRestDetectorSignal();

    ClassDef(TRestDetectorSignal, 2);
};
#endif
