///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorReadoutEventViewer.h inherited from TRestEventViewer
///
///             dec 2015:   First concept
///                 Viewer class for a TRestDetectorSignalEvent and TRestDetectorReadout
///                 JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorReadoutEventViewer
#define RestCore_TRestDetectorReadoutEventViewer

#include <TCanvas.h>
#include <TPad.h>
#include <TRestEventViewer.h>

#include "TH2Poly.h"
#include "TRestDetectorReadout.h"
#include "TRestDetectorSignalEvent.h"

class TRestDetectorReadoutEventViewer : public TRestEventViewer {
   protected:
    TCanvas* fCanvasXY;
    TCanvas* fCanvasXZYZ;

    TRestDetectorReadout* fReadout;

    TRestDetectorSignalEvent* fSignalEvent;

    TH2Poly* fHistoXY;
    TH2D* fHistoXZ;
    TH2D* fHistoYZ;

    bool isFirstevent;

    double xmin, xmax, ymin, ymax, zmin, zmax;

   public:
    void Initialize() override;
    // Finalize initialization based on arg. TRestDetectorReadout
    void SetReadout(TRestDetectorReadout* readout);

    void AddEvent(TRestEvent* ev) override;
    void DrawReadoutPulses();
    TRestDetectorReadoutChannel* GetChannel(int readoutChannel);
    TRestDetectorReadoutModule* GetModule(int readoutChannel);

    // Constructor
    TRestDetectorReadoutEventViewer();
    // Destructor
    ~TRestDetectorReadoutEventViewer() override;

    ClassDefOverride(TRestDetectorReadoutEventViewer, 1);  // class inherited from
                                                           // TRestEventViewer
};
#endif
