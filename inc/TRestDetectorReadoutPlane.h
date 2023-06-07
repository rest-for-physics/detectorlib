/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestDetectorReadoutPlane
#define RestCore_TRestDetectorReadoutPlane

#include <TGraph.h>
#include <TH2Poly.h>

#include <iostream>

#include "TRestDetectorReadoutChannel.h"
#include "TRestDetectorReadoutModule.h"

/// A class to store the readout plane definition used in TRestDetectorReadout. It
/// allows to integrate any number of independent readout modules.
class TRestDetectorReadoutPlane {
   private:
    /// The plane id number imposed by the order of creation. Being the first id=0.
    Int_t fId = -1;  //<

    /// The position of the readout plane that defines the origin (0,0) in plane coordinates
    TVector3 fPosition = {0, 0, 0};  //<

    /// A vector that defines the plane orientation and the side of the active volume.
    TVector3 fNormal = {0, 0, 1};  //<

    /// A vector contained in the plane that defines the plane local reference system
    TVector3 fAxisX = {1, 0, 0};  //<

    /// An orthonormal to fAxisX contained in the plane. It is calculated internally
    TVector3 fAxisY = {0, 1, 0};  //!

    /// The cathode position which delimites the active volume together with the readout plane.
    TVector3 fCathodePosition = {0, 0, 1};  //<

    /// The fraction of charge/energy this readout plane collects from a hit postion.
    Double_t fChargeCollection = 1;  //<

    /// The total drift distance defined between cathode and readout plane. It is calculated internally.
    Double_t fTotalDriftDistance = 0;  //!

    /// Keeps track of the number of modules that have been added to the readout plane
    Int_t fNModules = 0;  //<

    ///< A list of TRestDetectorReadoutModule components contained in the readout plane.
    std::vector<TRestDetectorReadoutModule> fReadoutModules;  //<

    void Initialize();

   protected:
   public:
    // Setters
    /// Sets the planeId. This is done by TRestDetectorReadout during initialization
    void SetID(int id) { fId = id; }

    /// Sets the readout plane position
    void SetPosition(const TVector3& position) { fPosition = position; }

    /// Sets the cathode plane position. By default is parallel to the readout plane.
    void SetCathodePosition(const TVector3& position) { fCathodePosition = position; }

    /// Sets the orientation of the readout plane, and defines the side of the active volume.
    void SetNormal(const TVector3& vect) { fNormal = vect.Unit(); }

    /// Sets the vector contained in the plane that defines the internal X-axis
    void SetAxisX(const TVector3& vect) { fAxisX = vect.Unit(); }

    /// Sets the vector contained in the plane that defines the internal Y-axis
    void SetAxisY(const TVector3& vect) { fAxisY = vect.Unit(); }

    /// Sets the value for the charge collection.
    void SetChargeCollection(Double_t charge) { fChargeCollection = charge; }

    /// Sets the value for the total drift distance
    void SetTotalDriftDistance(Double_t d) { fTotalDriftDistance = d; }

    // Getters
    /// Returns an integer with the plane id number.
    inline Int_t GetID() const { return fId; }

    /// Returns a TVector3 with the readout plane position
    inline TVector3 GetPosition() const { return fPosition; }

    /// Returns a TVector3 with the cathode position
    inline TVector3 GetCathodePosition() const { return fCathodePosition; }

    /// Returns a TVector3 with a vector normal to the readout plane
    inline TVector3 GetNormal() const { return fNormal; }

    /// Returns a TVector3 with a vector that defines the X-axis plane coordinate system
    inline TVector3 GetAxisX() const { return fAxisX; }

    /// Returns a TVector3 with a vector that defines the Y-axis plane coordinate system
    inline TVector3 GetAxisY() const { return fAxisY; }

    /// Returns the charge collection ratio at this readout plane
    inline Double_t GetChargeCollection() const { return fChargeCollection; }

    /// Returns the total drift distance
    inline Double_t GetTotalDriftDistance() const { return fTotalDriftDistance; }

    /// Returns the perpendicular distance to the readout plane from a given
    /// position *pos*.
    Double_t GetDistanceTo(TVector3 position);

    /// Returns the perpendicular distance to the readout plane from a given
    /// position *x*, *y*, *z*.
    Double_t GetDistanceTo(Double_t x, Double_t y, Double_t z);

    /// Returns a TVector2 oriented as the shortest distance of a given position
    /// *pos* on the plane to a specific module with id *mod*
    TVector2 GetDistanceToModule(Int_t mod, const TVector2& position) {
        return GetModuleByID(mod)->GetDistanceToModule(position);
    }

    TRestDetectorReadoutModule& operator[](int mod) { return fReadoutModules[mod]; }

    /// Returns a pointer to a readout module using its std::vector index
    TRestDetectorReadoutModule* GetModule(size_t mod) {
        if (mod >= GetNumberOfModules()) return nullptr;
        return &fReadoutModules[mod];
    }

    /// Returns the total number of modules in the readout plane
    size_t GetNumberOfModules() { return fReadoutModules.size(); }

    /// Adds a new module to the readout plane
    void AddModule(TRestDetectorReadoutModule& rModule) {
        fReadoutModules.push_back(rModule);
        fNModules++;
    }

    /// Prints the readout plane description
    void PrintMetadata() { Print(); }

    Int_t GetNumberOfChannels();

    TRestDetectorReadoutModule* GetModuleByID(Int_t modID);

    Int_t isZInsideDriftVolume(Double_t z);

    Int_t isZInsideDriftVolume(const TVector3& position);

    Bool_t isDaqIDInside(Int_t daqId);

    Int_t GetModuleIDFromPosition(TVector3 position);

    Int_t GetModuleIDFromPosition(Double_t x, Double_t y, Double_t z);

    void SetDriftDistance();

    void Draw();

    void Print(Int_t DetailLevel = 0);

    Int_t FindChannel(Int_t module, const TVector2& position);

    Double_t GetX(Int_t modID, Int_t chID);
    Double_t GetY(Int_t modID, Int_t chID);

    TH2Poly* GetReadoutHistogram();
    void GetBoundaries(double& xmin, double& xmax, double& ymin, double& ymax);

    // Constructor
    TRestDetectorReadoutPlane();
    // Destructor
    virtual ~TRestDetectorReadoutPlane();

    ClassDef(TRestDetectorReadoutPlane, 3);
};
#endif
