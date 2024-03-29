#include "TRestBrowser.h"
#include "TRestDetectorHitsEvent.h"
#include "TRestTask.h"

#ifndef RestTask_ViewHitsEvents
#define RestTask_ViewHitsEvents

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Detector_ViewHitsEvent(TString fName) {
    TRestBrowser* browser = new TRestBrowser("TRestDetectorHitsEventViewer");

    TRestEvent* eve = new TRestDetectorHitsEvent();
    browser->SetInputEvent(eve);

    browser->OpenFile(fName);

// when we run this macro from restManager from bash,
// we need to call TRestMetadata::GetChar() to prevent returning,
// while keeping GUI alive.
#ifdef REST_MANAGER
    GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}

#endif
