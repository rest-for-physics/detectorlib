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
#include <TRestStringOutput.h>
#include <TString.h>
#include <TVector2.h>

#include <iostream>
#include <optional>

class TRestDetectorSignal {
   private:
    Int_t GetMinIndex() const;
    Int_t GetTimeIndex(Double_t t);

   protected:
    Int_t fSignalID = -1;

    std::vector<Double_t> fSignalTime;    // Vector with the time of the signal
    std::vector<Double_t> fSignalCharge;  // Vector with the charge of the signal

    // TODO: remove this and use readout
    std::string fName;  // Name of the signal
    std::string fType;  // Type of the signal

   public:
    TGraph* fGraph;  //!

    std::vector<Int_t> fPointsOverThreshold;  //!

    void IncreaseAmplitude(const TVector2& p);
    void SetPoint(const TVector2& p);

    // TODO other objects should probably skip using GetMaxIndex directly
    Int_t GetMaxIndex(Int_t from = 0, Int_t to = 0);

    std::optional<std::pair<Double_t, Double_t>> GetPeakGauss();
    std::optional<std::pair<Double_t, Double_t>> GetPeakLandau();
    std::optional<std::pair<Double_t, Double_t>> GetPeakAget();

    std::string GetSignalName() const { return fName; }
    std::string GetSignalType() const { return fType; }

    void SetSignalName(const std::string& name) { fName = name; }
    void SetSignalType(const std::string& type) { fType = type; }

    // Getters
    TVector2 GetPoint(Int_t n) { return {GetTime(n), GetData(n)}; }

    inline Int_t GetSignalID() const { return fSignalID; }
    inline Int_t GetID() const { return fSignalID; }

    void IncreaseTimeBinBy(Int_t bin, Double_t data) {
        if (bin >= GetNumberOfPoints()) {
            std::cout << "Increase time bin: outside limits" << std::endl;
            return;
        }

        fSignalCharge[bin] += data;
    }

    Int_t GetNumberOfPoints() const {
        if (fSignalTime.size() != fSignalCharge.size()) {
            RESTError << "WARNING, the two std::vector sizes did not match" << RESTendl;
            exit(1);
        }
        return fSignalTime.size();
    }

    Double_t GetIntegralWithTime(Double_t startTime, Double_t endTime) const;
    Double_t GetIntegral(Int_t startBin = 0, Int_t endBin = 0) const;

    void Normalize(Double_t scale = 1.);

    std::vector<Int_t> GetPointsOverThreshold() const { return fPointsOverThreshold; }

    Double_t GetAverage(Int_t start = 0, Int_t end = 0);
    Int_t GetMaxPeakWidth();
    Double_t GetMaxPeakWithTime(Double_t startTime, Double_t endTime) const;

    Double_t GetMaxPeakValue();
    Double_t GetMinPeakValue();

    Double_t GetMaxPeakTime(Int_t from = 0, Int_t to = 0);

    Double_t GetMaxValue() { return GetMaxPeakValue(); }
    Double_t GetMinValue() { return GetMinPeakValue(); }

    Double_t GetMinTime() const;
    Double_t GetMaxTime() const;

    Double_t GetData(Int_t index) const { return fSignalCharge[index]; }
    Double_t GetTime(Int_t index) const { return fSignalTime[index]; }

    // Setters
    void SetSignalID(Int_t sID) { fSignalID = sID; }
    void SetID(Int_t sID) { fSignalID = sID; }

    void NewPoint(Double_t time, Double_t data);
    void IncreaseAmplitude(Double_t t, Double_t d);

    void SetPoint(Double_t t, Double_t d);
    void SetPoint(Int_t index, Double_t t, Double_t d);

    Double_t GetStandardDeviation(Int_t startBin, Int_t endBin);
    Double_t GetBaseLine(Int_t startBin, Int_t endBin);
    Double_t GetBaseLineSigma(Int_t startBin, Int_t endBin, Double_t baseline = 0);

    Double_t SubstractBaseline(Int_t startBin, Int_t endBin);
    void AddOffset(Double_t offset);

    void MultiplySignalBy(Double_t factor);
    void ExponentialConvolution(Double_t fromTime, Double_t decayTime, Double_t offset = 0);
    void SignalAddition(TRestDetectorSignal* inSgnl);

    Bool_t isSorted() const;
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

    void WriteSignalToTextFile(const TString& filename) const;
    void Print() const;

    TGraph* GetGraph(Int_t color = 1);

    // Constructor
    TRestDetectorSignal();
    // Destructor
    ~TRestDetectorSignal();

    ClassDef(TRestDetectorSignal, 4);
};
#endif
