///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorGeometry.h
///
///             A geometry class to store detector geometry
///
///             jul 2015:   First concept
///                 J. Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorGeometry
#define RestCore_TRestDetectorGeometry

#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoVolume.h>
#include <TObject.h>

#include <iostream>
#include <vector>

#include "TRestDetectorGas.h"

#if defined REST_GARFIELD

#include <Component.hh>
#include <GeometryRoot.hh>
#include <Sensor.hh>

#endif

class TRestDetectorGeometry : public TGeoManager {
   protected:
#if defined REST_GARFIELD
    Garfield::GeometryRoot* fGfGeometry =
        nullptr;  //!///< Pointer to Garfield::GeometryRoot object of the geometry
    std::vector<Garfield::Component*> vGfComponent;  //!///< Vector of pointers to Garfield Component object
    std::vector<Garfield::Sensor*> vGfSensor;        //!///< Vector of pointers to Garfield Sensor object
    TGeoNode* fDriftElec = nullptr;                  //!///< pointer to drift electrode
    std::vector<TGeoNode*> vReadoutElec;             //!///< vector of pointers to readout planes
#endif

   public:
    // Constructor
    TRestDetectorGeometry();
    // Destructor
    virtual ~TRestDetectorGeometry();

    /// initialize Garfield::GeometryRoot geometry object
    void InitGfGeometry();

#if defined REST_GARFIELD
    /// Return pointer to Garfield::GeometryRoot geometry object
    Garfield::GeometryRoot* GetGfGeometry() { return fGfGeometry; }

    /// Set Garfield medium of gas volume to TGeometry one
    void SetGfGeoMedium(const char* geoMatName, TRestDetectorGas* gas) {
        fGfGeometry->SetMedium(geoMatName, gas->GetGasMedium());
    }

    /// Set Garfield field component
    void AddGfComponent(Garfield::Component* c) {
        c->SetGeometry(fGfGeometry);
        vGfComponent.push_back(c);
    }

    /// Set Garfield field component
    void AddGfSensor(Garfield::Sensor* s) { vGfSensor.push_back(s); }

    /// Set drift electrode node
    void SetDriftElecNode(TGeoNode* n) { fDriftElec = n; }

    /// Add readout electrode node to the list
    void AddReadoutElecNode(TGeoNode* n) { vReadoutElec.push_back(n); }

    /// Get drift electrode node
    TGeoNode* GetDriftElecNode() { return fDriftElec; }

    /// Get number of readout electrode nodes
    int GetNReadoutElecNodes() { return vReadoutElec.size(); }

    /// Get i^th readout electrode node
    TGeoNode* GetReadoutElecNode(unsigned int i) {
        if (i < vReadoutElec.size())
            return vReadoutElec[i];
        else
            return nullptr;
    }

    /// Get i^th Gf component
    Garfield::Component* GetGfComponent(unsigned int i) {
        if (i < vGfComponent.size())
            return vGfComponent[i];
        else
            return nullptr;
    }

    /// Get number of Gf components
    int GetNbOfGfComponent() { return vGfComponent.size(); }

    /// Get i^th Gf sensor
    Garfield::Sensor* GetGfSensor(unsigned int i) {
        if (i < vGfSensor.size())
            return vGfSensor[i];
        else
            return nullptr;
    }

    /// Get number of Gf sensors
    int GetNbOfGfSensor() { return vGfSensor.size(); }

    /// Get Gf Medium at position
    Garfield::Medium* GetGfMedium(double x, double y, double z) {
        FindNode(x / 10., y / 10., z / 10.);  // needed due to a bug in TGeoNavigator
        return fGfGeometry->GetMedium(x / 10., y / 10., z / 10.);
    }  // mm to cm
#endif

    void PrintGeometry();

    ClassDef(TRestDetectorGeometry, 1);
};

#endif
