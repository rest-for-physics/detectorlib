///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4Event.h
///
///             G4 Event class to store results from Geant4 REST simulation  
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#include "TRestG4Event.h"
using namespace std;

ClassImp(TRestG4Event)
//______________________________________________________________________________
    TRestG4Event::TRestG4Event()
{
    fNVolumes = 0;
   // TRestG4Event default constructor
   Initialize();
}

//______________________________________________________________________________
TRestG4Event::~TRestG4Event()
{
   // TRestG4Event destructor
}

void TRestG4Event::Initialize()
{
    TRestEvent::Initialize();

    fPrimaryEventDirection.clear();
    fPrimaryEventEnergy.clear();
    fPrimaryEventOrigin.SetXYZ( 0, 0, 0 );

    fTrack.clear();
    fNTracks = 0;

    fTotalDepositedEnergy = 0;
    fSensitiveVolumeEnergy = 0;
    fMaxSubEventID = 0;
}

void TRestG4Event::AddActiveVolume( ) 
{
    fNVolumes++; 
    fVolumeStored.push_back( 1 ); 
    fVolumeDepositedEnergy.push_back(0);
}

void TRestG4Event::ClearVolumes( )
{
    fNVolumes = 0;
    fVolumeStored.clear();
    fVolumeDepositedEnergy.clear();
}

void TRestG4Event::AddEnergyDepositToVolume( Int_t volID, Double_t eDep ) 
{ 
    fVolumeDepositedEnergy[volID] += eDep; 
}

void TRestG4Event::SetTrackSubEventID( Int_t n, Int_t id ) 
{
    fTrack[n].SetSubEventID( id ); 
    if( fMaxSubEventID < id ) fMaxSubEventID = id;
}

void TRestG4Event::AddTrack( TRestG4Track trk ) 
{ 
    fTrack.push_back( trk ); 
    fNTracks = fTrack.size(); 
    fTotalDepositedEnergy += trk.GetTotalDepositedEnergy(); 
    for( int n = 0; n < GetNumberOfActiveVolumes(); n++ )
        fVolumeDepositedEnergy[n] += trk.GetEnergyInVolume( n );
}

Double_t TRestG4Event::GetTotalDepositedEnergyFromTracks()
{
    Double_t eDep = 0;

    for( int tk = 0; tk < GetNumberOfTracks(); tk++ )
        eDep += GetTrack( tk )->GetTotalDepositedEnergy();

    return eDep;
}

TRestG4Track *TRestG4Event::GetTrackByID( int id ) 
{ 
    for( int i = 0; i < fNTracks; i++ ) 
        if( fTrack[i].GetTrackID( ) == id ) 
            return &fTrack[i]; 
    return NULL; 
}

Int_t TRestG4Event::GetNumberOfHits( )
{
    Int_t hits = 0;
    for( int i = 0; i < fNTracks; i++ ) 
        hits += GetTrack( i )->GetNumberOfHits();

    return hits;
}

TRestHits TRestG4Event::GetHits( )
{
    TRestHits hits;
    for( int t = 0; t < fNTracks; t++ ) 
    {
        TRestG4Hits *g4Hits = GetTrack( t )->GetHits( );
        for( int n = 0; n < g4Hits->GetNumberOfHits(); n++ )
        {
            Double_t x = g4Hits->GetX(n);
            Double_t y = g4Hits->GetY(n);
            Double_t z = g4Hits->GetZ(n);
            Double_t en = g4Hits->GetEnergy(n);

            hits.AddHit( x, y, z, en );
        }
    }

    return hits;
}

void TRestG4Event::PrintEvent()
{
    TRestEvent::PrintEvent();

    cout.precision(4);

    cout << "Total energy : " << fTotalDepositedEnergy << " keV" << endl;
    cout << "Source origin : (" << fPrimaryEventOrigin.X() << "," << fPrimaryEventOrigin.Y() << "," << fPrimaryEventOrigin.Z() << ") mm" << endl;

    for( int n = 0; n < GetNumberOfPrimaries(); n++ )
    {
        TVector3 *dir = &fPrimaryEventDirection[n];
        cout << "Source " << n << " direction : (" << dir->X() << "," << dir->Y() << "," << dir->Z() << ")" << endl;
        cout << "Source " << n << " energy : " << fPrimaryEventEnergy[n] << " keV" << endl;
    }

    cout << "Number of active volumes : " << GetNumberOfActiveVolumes() << endl;
    for( int i = 0; i < GetNumberOfActiveVolumes(); i++ )
    {
        if( isVolumeStored(i) )
        {
            cout << "Active volume " << i << " has been stored." << endl;
            cout << "Total energy deposit in volume " << i << " : " << fVolumeDepositedEnergy[i] << " keV" << endl;
        }
        else cout << "Active volume " << i << " has not been stored" << endl;
    }

    cout << "---------------------------------------------------------------------------" << endl;
    cout << "Total number of tracks : " << fNTracks << endl;
    for( int n = 0; n < GetNumberOfTracks(); n++ )
    {
        GetTrack(n)->PrintTrack();
    }

}
