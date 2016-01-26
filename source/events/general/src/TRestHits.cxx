///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHits.cxx
///
///             Event class to store hits 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///		nov 2015:
///		    Changed vectors fX fY fZ and fEnergy from <Int_t> to <Float_t>
///	            JuanAn Garcia
///_______________________________________________________________________________


#include "TRestHits.h"
using namespace std;

ClassImp(TRestHits)

TRestHits::TRestHits()
{
    fNHits = 0;
    fTotEnergy = 0;
}

TRestHits::~TRestHits()
{
}

Bool_t TRestHits::areXY()
{
    for( int i = 0; i < GetNumberOfHits(); i++ )
        if( GetZ(i) != 0 ) return false;
    return true;
}

Bool_t TRestHits::areXZ()
{
    for( int i = 0; i < GetNumberOfHits(); i++ )
        if( GetY(i) != 0 ) return false;
    return true;
}

Bool_t TRestHits::areYZ()
{
    for( int i = 0; i < GetNumberOfHits(); i++ )
        if( GetX(i) != 0 ) return false;
    return true;
}

Bool_t TRestHits::areXYZ()
{
    for( int i = 0; i < GetNumberOfHits(); i++ )
        if( GetX(i) == 0 ||  GetY(i) == 0 || GetZ(i) == 0 ) return false;
    return true;
}

void TRestHits::GetXArray( Float_t *x )
{
    for( int i = 0; i < GetNumberOfHits(); i++ )
        x[i] = GetX(i);
}

void TRestHits::GetYArray( Float_t *y )
{
    for( int i = 0; i < GetNumberOfHits(); i++ )
        y[i] = GetY(i);
}

void TRestHits::GetZArray( Float_t *z )
{
    for( int i = 0; i < GetNumberOfHits(); i++ )
        z[i] = GetZ(i);
}

Double_t TRestHits::GetEnergyIntegral()
{
    Double_t sum = 0;
    for( int i = 0; i < GetNumberOfHits(); i++ )
        sum += GetEnergy(i);
    return sum;
}

void TRestHits::AddHit( Double_t x, Double_t y, Double_t z, Double_t en )
{
    fNHits++;
    fX.push_back( (Float_t) (x) );
    fY.push_back( (Float_t) (y ));
    fZ.push_back( (Float_t) ( z));
    fEnergy.push_back( (Float_t) ( en ) );

    fTotEnergy += en;
}

void TRestHits::AddHit( TVector3 pos, Double_t en )
{
    fNHits++;

    fX.push_back( (Float_t) (pos.X()) );
    fY.push_back( (Float_t) (pos.Y() ));
    fZ.push_back( (Float_t) ( pos.Z()  ));
    fEnergy.push_back( (Float_t) ( en ) );

    fTotEnergy += en;
}

void TRestHits::RemoveHits( )
{
    fNHits = 0;
    fX.clear();
    fY.clear();
    fZ.clear();
    fEnergy.clear();
    fTotEnergy = 0;
}


void TRestHits::Translate(double origx, double origy, double origz)
{
	for(int i = 0; i < fNHits; i++)
	{
		fX[i] += origx;
		fY[i] += origy;
		fZ[i] += origz;
	}
}
Double_t TRestHits::GetMaximumHitEnergy( )
{
    Double_t energy = 0;
    for ( int i = 0; i < GetNumberOfHits( ); i++ )
        if( GetEnergy(i) > energy ) energy = GetEnergy(i);
    return energy;
}

Double_t TRestHits::GetMinimumHitEnergy( )
{
    Double_t energy = GetMaximumHitEnergy( );
    for ( int i = 0; i < GetNumberOfHits( ); i++ )
        if( GetEnergy(i) < energy ) energy = GetEnergy(i);
    return energy;
}

Double_t TRestHits::GetMeanHitEnergy( )
{
    return GetTotalEnergy( )/GetNumberOfHits( );
}

void TRestHits::MergeHits( int n, int m )
{
    Double_t totalEnergy = fEnergy[n] + fEnergy[m];
    fX[n] = (fX[n]*fEnergy[n] + fX[m]*fEnergy[m])/totalEnergy;
    fY[n] = (fY[n]*fEnergy[n] + fY[m]*fEnergy[m])/totalEnergy;
    fZ[n] = (fZ[n]*fEnergy[n] + fZ[m]*fEnergy[m])/totalEnergy;

    fEnergy[n] += fEnergy[m];

    fNHits--;
    fX.erase( fX.begin() + m);
    fY.erase( fY.begin() + m);
    fZ.erase( fZ.begin() + m);
    fEnergy.erase( fEnergy.begin() + m );
}

void TRestHits::SwapHits( Int_t i, Int_t j )
{
    iter_swap(fX.begin() + i, fX.begin() + j);
    iter_swap(fY.begin() + i, fY.begin() + j);
    iter_swap(fZ.begin() + i, fZ.begin() + j);
    iter_swap(fEnergy.begin() + i, fEnergy.begin() + j);
}

Bool_t TRestHits::isSortedByEnergy( )
{
    for( int i = 0; i < GetNumberOfHits()-1; i++ )
    {
        if( GetEnergy( i+1 ) > GetEnergy( i ) ) return false;
    }
    return true;
}

void TRestHits::RemoveHit( int n )
{
    fNHits--;
    fTotEnergy -= GetEnergy( n );
    fX.erase(fX.begin()+n);
    fY.erase(fY.begin()+n);
    fZ.erase(fZ.begin()+n);
    fEnergy.erase(fEnergy.begin()+n);
}

Double_t TRestHits::GetTotalDistance()
{
    Double_t distance = 0;
    for( int i = 0; i < GetNumberOfHits()-1; i++ )
        distance += TMath::Sqrt( GetDistance2( i, i+1 ) );
    return distance;
}

Double_t TRestHits::GetDistance2( int n, int m )
{
    return (GetX(n)-GetX(m))*(GetX(n)-GetX(m)) +  (GetY(n)-GetY(m))*(GetY(n)-GetY(m)) + (GetZ(n)-GetZ(m))*(GetZ(n)-GetZ(m));
}

void TRestHits::PrintHits()
{
	//TRestEvent::PrintEvent();
	for( int n = 0; n < GetNumberOfHits(); n++ )
	{
		cout << "Hit " << n << " X: " << GetX(n) << " Y: " << GetY(n) << " Z: " << GetZ(n) <<  " Energy: " << GetEnergy(n) << endl;
	}


}

