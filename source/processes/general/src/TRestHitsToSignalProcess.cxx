///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsToSignalProcess.cxx
///
///             oct 2015:  Javier Galan
///_______________________________________________________________________________


#include "TRestHitsToSignalProcess.h"

//#include <TRandom.h>
#include "TRestElectronDiffusionProcess.h"

/* Chrono can be used for measuring time with better precision and test the time spent for different parts of the code
	using high_resolution_clock
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;
*/

ClassImp(TRestHitsToSignalProcess)
    //______________________________________________________________________________
TRestHitsToSignalProcess::TRestHitsToSignalProcess()
{
    Initialize();

}

//______________________________________________________________________________
TRestHitsToSignalProcess::TRestHitsToSignalProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfig( "hitsToSignalProcess", cfgFileName ) == -1 ) LoadDefaultConfig( );

    cout << "Values : " << endl;
    cout << fElectricField << endl;
    cout << fCathodePosition << endl;

    fReadout = new TRestReadout( cfgFileName );

    // TRestHitsToSignalProcess default constructor
}

//______________________________________________________________________________
TRestHitsToSignalProcess::~TRestHitsToSignalProcess()
{
    if( fReadout != NULL ) delete fReadout;

    delete fHitsEvent;
    delete fSignalEvent;
    // TRestHitsToSignalProcess destructor
}

void TRestHitsToSignalProcess::LoadDefaultConfig( )
{
    SetName( "hitsToSignalProcess-Default" );
    SetTitle( "Default config" );

    fSampling = 1;

}

//______________________________________________________________________________
void TRestHitsToSignalProcess::Initialize()
{

    fHitsEvent = new TRestHitsEvent();

    fSignalEvent = new TRestSignalEvent();

    fInputEvent = fHitsEvent;
    fOutputEvent = fSignalEvent;
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    cout << __PRETTY_FUNCTION__ << endl;

    if( fReadout == NULL ) cout << "REST ERRORRRR : Readout has not been initialized" << endl;


    fGas = GetGasFromRunMetadata( );

    if ( fElectricField == PARAMETER_NOT_FOUND_DBL )
    {	
	fElectricField = GetElectricFieldFromElectronDiffusionProcess( );
        if( fElectricField != PARAMETER_NOT_FOUND_DBL ) cout << "Getting electric field from electronDiffusionProcess : " << fElectricField << " V/cm" << endl;
	else { cout << "WARNING!!! : Electric field has NO value" << endl; getchar(); }
    }

    if ( fCathodePosition == PARAMETER_NOT_FOUND_DBL )
    {
	    fCathodePosition = GetCathodePositionFromElectronDiffusionProcess( );
	    if( fCathodePosition != PARAMETER_NOT_FOUND_DBL ) cout << "Getting cathode position from electronDiffusionProcess : " << fCathodePosition << " mm" << endl;
		else { cout << "WARNING!!! : Cathode position has NO value" << endl; getchar(); }
    }


/*
	cout << "Name : " << fGas->GetName() << endl;
	printf( "Drift velocity : %e\n",  fGas->GetDriftVelocity( fElectricField ) );
	cout << "Electric field : " << fElectricField <<  endl;
	cout << "Cathode position : " << fCathodePosition << endl;
	getchar();
*/
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::BeginOfEventProcess() 
{
    cout << "Begin of event process" << endl;
    fSignalEvent->Initialize(); 

    cout << "Number of signals : " << fSignalEvent->GetNumberOfSignals() << endl;
}

Int_t TRestHitsToSignalProcess::FindModuleAndChannel( Double_t x, Double_t y, Int_t &module, Int_t &channel )
{
    for ( int md = 0; md < fReadout->GetNumberOfModules(); md++ )
    {
        if( fReadout->GetReadoutModule( md )->isInside( x, y ) )
            for( int ch = 0; ch < fReadout->GetReadoutModule( md )->GetNumberOfChannels(); ch++ )
            {
                if ( fReadout->GetReadoutModule( md )->isInsideChannel( ch, x , y ) )
                {
                    module = md;
                    channel = ch;
                    return 1;
                }
            }
    }
    return 0;

}

//______________________________________________________________________________
TRestEvent* TRestHitsToSignalProcess::ProcessEvent( TRestEvent *evInput )
{
    fHitsEvent = (TRestHitsEvent *) evInput;

    cout << "Event ID : " << fHitsEvent->GetEventID() << endl;
    cout << "Number of hits : " << fHitsEvent->GetNumberOfHits() << endl;

    fSignalEvent->SetEventTime( fHitsEvent->GetEventTime() );
    fSignalEvent->SetEventID( fHitsEvent->GetEventID() );

    Int_t mod = -1;
    Int_t chnl = -1;

    for( int hit = 0; hit < fHitsEvent->GetNumberOfHits(); hit++ )
    {
        Double_t x = fHitsEvent->GetX( hit );
        Double_t y = fHitsEvent->GetY( hit );

    //high_resolution_clock::time_point t1 = high_resolution_clock::now();
    int found = FindModuleAndChannel( x, y, mod, chnl );
    //high_resolution_clock::time_point t2 = high_resolution_clock::now();

    //auto duration = duration_cast<microseconds>( t2 - t1 ).count();
    //cout << duration << endl;
    //getchar();

        if( found )
        {
            Int_t channelID = 0;
            for( int n = 0; n < mod-1; n++ )
                channelID += fReadout->GetReadoutModule(n)->GetNumberOfChannels();
            channelID += chnl;

            /* This is now done internally in TRestSignalEvent
            if ( !fSignalEvent->signalIDExists( channelID ) )
            {
                TRestSignal sgnl;
                sgnl.SetSignalID( channelID );
                fSignalEvent->AddSignal( sgnl );
            }
            */

            Double_t energy = fHitsEvent->GetEnergy( hit );
            Double_t time = fHitsEvent->GetZ( hit );

            time = fSampling * (Int_t) (time/fSampling);

            fSignalEvent->AddChargeToSignal( channelID, time, energy );
        }
    }

    fSignalEvent->SortSignals();

 //   fSignalEvent->PrintEvent();


    return fSignalEvent;
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestHitsToSignalProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::InitFromConfigFile( )
{
    fSampling = StringToDouble( GetParameter( "sampling" ) );
    fCathodePosition = StringToDouble( GetParameter( "cathodePosition" ) );
    fElectricField = StringToDouble( GetParameter( "electricField" )  );

}

TRestGas *TRestHitsToSignalProcess::GetGasFromRunMetadata( )
{
	// For the moment this function will return the first occurence of TRestGas.
 	// What happens if there are several? And if I want to use one gas from the config file? 
	// We need to introduce an option somewhere.
	// For the moment I know there will be an existing gas file since the hits come from electronDiffusion.

	for( size_t i = 0; i < fRunMetadata.size(); i++ )
		if ( fRunMetadata[i]->ClassName() == (TString) "TRestGas" )
		{
			((TRestGas *) fRunMetadata[i])->LoadGasFile();
			return (TRestGas *) fRunMetadata[i];
		}

	return NULL;
}

Double_t TRestHitsToSignalProcess::GetCathodePositionFromElectronDiffusionProcess( )
{
	cout << __PRETTY_FUNCTION__ << endl;
	
	for( size_t i = 0; i < fRunMetadata.size(); i++ )
		if ( fRunMetadata[i]->ClassName() == (TString) "TRestElectronDiffusionProcess" )
		{
			cout <<"HOLA1" << endl;
			return ((TRestElectronDiffusionProcess *) fRunMetadata[i])->GetCathodePosition();
		}

	return PARAMETER_NOT_FOUND_DBL;

}

Double_t TRestHitsToSignalProcess::GetElectricFieldFromElectronDiffusionProcess( )
{
	cout << __PRETTY_FUNCTION__ << endl;
	
	for( size_t i = 0; i < fRunMetadata.size(); i++ )
	{
								
		if ( fRunMetadata[i]->ClassName() == (TString) "TRestElectronDiffusionProcess" )
		{
			return ((TRestElectronDiffusionProcess *) fRunMetadata[i])->GetElectricField();
		}
	}

	return PARAMETER_NOT_FOUND_DBL;

}
