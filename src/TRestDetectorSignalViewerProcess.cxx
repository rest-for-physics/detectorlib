///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignalViewerProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestDetectorSignalViewerProcess
///             Date : feb/2017
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestDetectorSignalViewerProcess.h"

#include <TLegend.h>
#include <TPaveText.h>

using namespace std;

int rawCounter3 = 0;

ClassImp(TRestDetectorSignalViewerProcess);

TRestDetectorSignalViewerProcess::TRestDetectorSignalViewerProcess() { Initialize(); }

TRestDetectorSignalViewerProcess::TRestDetectorSignalViewerProcess(const char* configFilename) {
    Initialize();
    if (LoadConfigFromFile(configFilename)) {
        LoadDefaultConfig();
    }
}

TRestDetectorSignalViewerProcess::~TRestDetectorSignalViewerProcess() {}

void TRestDetectorSignalViewerProcess::LoadDefaultConfig() { SetTitle("Default config"); }

void TRestDetectorSignalViewerProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fSignalEvent = nullptr;

    fDrawRefresh = 0;

    fSingleThreadOnly = true;
}

void TRestDetectorSignalViewerProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name)) LoadDefaultConfig();
}

void TRestDetectorSignalViewerProcess::InitProcess() { this->CreateCanvas(); }

TRestEvent* TRestDetectorSignalViewerProcess::ProcessEvent(TRestEvent* inputEvent) {
    TString obsName;

    TRestDetectorSignalEvent* fInputSignalEvent = (TRestDetectorSignalEvent*)inputEvent;

    /// Copying the signal event to the output event
    fSignalEvent = fInputSignalEvent;
    //   fSignalEvent->SetID( fInputSignalEvent->GetID() );
    //   fSignalEvent->SetSubID( fInputSignalEvent->GetSubID() );
    //   fSignalEvent->SetTimeStamp( fInputSignalEvent->GetTimeStamp() );
    //   fSignalEvent->SetSubEventTag( fInputSignalEvent->GetSubEventTag() );

    //   //for( int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals();
    //   sgnl++ ) Int_t N = fInputSignalEvent->GetNumberOfSignals();
    //  // if( GetVerboseLevel() >= REST_Debug ) N = 1;
    //   for( int sgnl = 0; sgnl < N; sgnl++ )
    // fSignalEvent->AddSignal( *fInputSignalEvent->GetSignal( sgnl ) );
    /////////////////////////////////////////////////

    GetCanvas()->cd();
    GetCanvas()->SetGrid();
    fCanvas->cd();
    eveCounter++;
    if (eveCounter >= fDrawRefresh) {
        eveCounter = 0;
        sgnCounter = 0;
        if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
            GetAnalysisTree()->PrintObservables();
        }
        for (auto object : fDrawingObjects) {
            delete object;
        }
        fDrawingObjects.clear();

        TPad* pad2 = DrawSignal(sgnCounter);

        fCanvas->cd();
        pad2->Draw();
        fCanvas->Update();

        RESTFout.setborder("");
        RESTFout.setorientation(TRestStringOutput::REST_Display_Orientation::kLeft);
        RESTFout << "Press Enter to continue\nPress Esc to stop viewing\nPress n/p to "
                "switch signals"
             << RESTendl;

        while (true) {
            int a = GetChar("");
            if (a == 10)  // enter
            {
                break;
            } else if (a == 27)  // esc
            {
                fDrawRefresh = 1e99;
                while (getchar() != '\n')
                    ;
                break;
            } else if (a == 110 || a == 78)  // n
            {
                sgnCounter++;
                if (sgnCounter >= 0 && sgnCounter < fInputSignalEvent->GetNumberOfSignals()) {
                    TPad* pad2 = DrawSignal(sgnCounter);
                    fCanvas->cd();
                    pad2->Draw();
                    fCanvas->Update();
                } else {
                    RESTWarning << "cannot plot signal with id " << sgnCounter << RESTendl;
                }
            } else if (a == 112 || a == 80)  // p
            {
                sgnCounter--;
                if (sgnCounter >= 0 && sgnCounter < fInputSignalEvent->GetNumberOfSignals()) {
                    TPad* pad2 = DrawSignal(sgnCounter);
                    fCanvas->cd();
                    pad2->Draw();
                    fCanvas->Update();
                } else {
                    RESTWarning << "cannot plot signal with id " << sgnCounter << RESTendl;
                }
            }
            while (getchar() != '\n')
                ;
        }
    }

    return fSignalEvent;
}

void TRestDetectorSignalViewerProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

void TRestDetectorSignalViewerProcess::InitFromConfigFile() {
    fDrawRefresh = StringToDouble(GetParameter("refreshEvery", "0"));

    fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));

    fCanvasSize = StringTo2DVector(GetParameter("canvasSize", "(800,600)"));
}

TPad* TRestDetectorSignalViewerProcess::DrawSignal(Int_t signal) {
    TPad* pad = new TPad(this->GetName(), this->GetTitle(), 0, 0, 1, 1);
    pad->cd();
    pad->DrawFrame(fSignalEvent->GetMinTime() - 1, -10, fSignalEvent->GetMaxTime() + 1,
                   fSignalEvent->GetMaxValue() + 10, this->GetTitle());

    TMultiGraph* mg = new TMultiGraph();
    mg->SetTitle(this->GetTitle());
    mg->GetXaxis()->SetTitle("time bins");
    mg->GetYaxis()->SetTitleOffset(1.4);
    mg->GetYaxis()->SetTitle("Energy");

    for (int n = 0; n < fSignalEvent->GetNumberOfSignals(); n++) {
        TGraph* gr = fSignalEvent->GetSignal(n)->GetGraph((n + 1) % 6);
        mg->Add(gr);
    }

    mg->Draw("");

    return pad;
}
