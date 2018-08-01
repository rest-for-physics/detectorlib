#include "TRestTask.h"
#include "TRestG4EventViewer.h"
#include "TRestBrowser.h"

#ifndef RestTask_ViewG4Events
#define RestTask_ViewG4Events

Int_t REST_Viewer_G4Event( TString fName)
{
    TRestBrowser *browser = new TRestBrowser( "TRestG4EventViewer");

	TRestEvent*eve = new TRestG4Event();
	browser->SetInputEvent(eve);

    browser->OpenFile(fName);

	//when we run this macro from restManager from bash,
	//we need to call TRestMetadata::GetChar() to prevent returning,
	//while keeping GUI alive.
#ifdef REST_MANAGER
	browser->GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}
#endif