
#ifndef TRestSoft_TRestDetectorHitsEvent
#define TRestSoft_TRestDetectorHitsEvent

#include <TAxis.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TH2F.h>
#include <TMath.h>
#include <TRestEvent.h>
#include <TRestHits.h>
#include <TVector3.h>

#include <iostream>

//! An event data type that register a std::vector of TRestHits,
//! allowing us to save a 3-coordinate position and energy.
class TRestDetectorHitsEvent : public TRestEvent {
   private:
    /// An auxiliary TRestHits structure to register hits on XZ projection
    TRestHits* fXZHits;  //!
    /// An auxiliary TRestHits structure to register hits on YZ projection
    TRestHits* fYZHits;  //!

    /// An auxiliary TRestHits structure to register hits on XYZ projection
    TRestHits* fXYZHits;  //!

   protected:
    // TODO These graphs should be placed in TRestHits?
    // (following similar GetGraph implementation in TRestDetectorSignal)

    /// An auxiliary TGraph pointer to visualize hits on XY-projection.
    TGraph* fXYHitGraph;  //!
    /// An auxiliary TGraph pointer to visualize hits on XZ-projection.
    TGraph* fXZHitGraph;  //!
    /// An auxiliary TGraph pointer to visualize hits on YZ-projection.
    TGraph* fYZHitGraph;  //!

    /// An auxiliary TH2F histogram to visualize hits on XY-projection.
    TH2F* fXYHisto;  //!
    /// An auxiliary TH2F histogram to visualize hits on YZ-projection.
    TH2F* fYZHisto;  //!
    /// An auxiliary TH2F histogram to visualize hits on XZ-projection.
    TH2F* fXZHisto;  //!

    /// An auxiliary TH1F histogram to visualize hits on X-projection.
    TH1F* fXHisto;  //!
    /// An auxiliary TH1F histogram to visualize hits on Y-projection.
    TH1F* fYHisto;  //!
    /// An auxiliary TH1F histogram to visualize hits on Z-projection.
    TH1F* fZHisto;  //!

    /// The hits structure that is is saved to disk.
    TRestHits* fHits;  //->

   public:
    void AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t t = 0, REST_HitType type = XYZ);
    void AddHit(TVector3 pos, Double_t en, Double_t t = 0, REST_HitType type = XYZ);

    void Sort(bool(compareCondition)(const TRestHits::iterator& hit1,
                                     const TRestHits::iterator& hit2) = nullptr);
    void Shuffle(int NLoop);

    inline size_t GetNumberOfHits() const { return fHits->GetNumberOfHits(); }

    inline TRestHits* GetHits() const { return fHits; }

    /// Returns the X-coordinate of hit entry `n` in mm.
    inline Double_t GetX(int n) const { return fHits->GetX(n); }

    /// Returns the Y-coordinate of hit entry `n` in mm.
    inline Double_t GetY(int n) const { return fHits->GetY(n); }

    /// Returns the Z-coordinate of hit entry `n` in mm.
    inline Double_t GetZ(int n) const { return fHits->GetZ(n); }

    inline REST_HitType GetType(int n) const { return fHits->GetType(n); }

    /// Returns the squared distance between hits `n` and `m`.
    inline Double_t GetDistance2(int n, int m) const { return fHits->GetDistance2(n, m); }

    TRestHits* GetXZHits();
    TRestHits* GetYZHits();
    TRestHits* GetXYZHits();

    virtual void Initialize();

    inline virtual void PrintEvent() const { TRestDetectorHitsEvent::PrintEvent(-1); };
    virtual void PrintEvent(Int_t nHits) const;

    inline TVector3 GetPosition(int n) const { return fHits->GetPosition(n); }

    inline TVector3 GetMeanPosition() const { return fHits->GetMeanPosition(); }

    inline Int_t GetNumberOfHitsX() const { return fHits->GetNumberOfHitsX(); }
    inline Int_t GetNumberOfHitsY() const { return fHits->GetNumberOfHitsY(); }

    inline Double_t GetMeanPositionX() const { return fHits->GetMeanPositionX(); }
    inline Double_t GetMeanPositionY() const { return fHits->GetMeanPositionY(); }
    inline Double_t GetMeanPositionZ() const { return fHits->GetMeanPositionZ(); }
    inline Double_t GetSigmaXY2() const { return fHits->GetSigmaXY2(); }
    inline Double_t GetSigmaX() const { return fHits->GetSigmaX(); }
    inline Double_t GetSigmaY() const { return fHits->GetSigmaY(); }

    inline Double_t GetGaussSigmaX() { return GetXZHits()->GetGaussSigmaX(); }
    inline Double_t GetGaussSigmaY() { return GetYZHits()->GetGaussSigmaY(); }
    inline Double_t GetGaussSigmaZ() const { return fHits->GetGaussSigmaZ(); }

    inline Double_t GetSigmaZ2() const { return fHits->GetSigmaZ2(); }
    inline Double_t GetSkewXY() const { return fHits->GetSkewXY(); }
    inline Double_t GetSkewZ() const { return fHits->GetSkewZ(); }

    inline Double_t GetMaximumHitEnergy() const { return fHits->GetMaximumHitEnergy(); }
    inline Double_t GetMinimumHitEnergy() const { return fHits->GetMinimumHitEnergy(); }
    inline Double_t GetMeanHitEnergy() const { return fHits->GetMeanHitEnergy(); }

    inline Double_t GetEnergyX() const { return fHits->GetEnergyX(); }
    inline Double_t GetEnergyY() const { return fHits->GetEnergyY(); }
    inline Double_t GetTotalDepositedEnergy() const { return fHits->GetTotalDepositedEnergy(); }
    inline Double_t GetTotalEnergy() const { return fHits->GetTotalEnergy(); }
    inline Double_t GetEnergy() const { return fHits->GetEnergy(); }
    inline Double_t GetEnergy(int n) const { return fHits->GetEnergy(n); }
    inline Double_t GetTime(int n) const { return GetHits()->GetTime(n); }  // return value in us

    inline Int_t GetClosestHit(const TVector3& position) const { return fHits->GetClosestHit(position); }

    // Inside Cylinder methods
    Bool_t anyHitInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Bool_t allHitsInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Double_t GetEnergyInCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Int_t GetNumberOfHitsInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    TVector3 GetMeanPositionInCylinder(TVector3 x0, TVector3 x1, Double_t radius);

    // Inside Prim methods
    Bool_t anyHitInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta);
    Bool_t allHitsInsidePrism(TVector3 x0, TVector3 x1, Double_t sX, Double_t sY, Double_t theta);

    Double_t GetEnergyInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta);
    Int_t GetNumberOfHitsInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta);
    TVector3 GetMeanPositionInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta);

    // Get distance to cylinder walls methods
    Double_t GetClosestHitInsideDistanceToCylinderWall(TVector3 x0, TVector3 x1, Double_t radius);
    Double_t GetClosestHitInsideDistanceToCylinderTop(TVector3 x0, TVector3 x1, Double_t radius);
    Double_t GetClosestHitInsideDistanceToCylinderBottom(TVector3 x0, TVector3 x1, Double_t radius);

    // Get distance to prism walls methods
    Double_t GetClosestHitInsideDistanceToPrismWall(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                                    Double_t theta);
    Double_t GetClosestHitInsideDistanceToPrismTop(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                                   Double_t theta);
    Double_t GetClosestHitInsideDistanceToPrismBottom(const TVector3& x0, const TVector3& x1, Double_t sizeX,
                                                      Double_t sizeY, Double_t theta);

    TPad* DrawEvent(const TString& option = "");

    void DrawHistograms(Int_t& column, const TString& histOption = "", double pitch = 0);
    void DrawGraphs(Int_t& column);

    // Constructor
    TRestDetectorHitsEvent();
    // Destructor
    ~TRestDetectorHitsEvent();

    ClassDef(TRestDetectorHitsEvent, 1);
};
#endif
