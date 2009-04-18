/*******************************************************************************

                              moFilterManager.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
  *                                                                          *
  *   This code is distributed in the hope that it will be useful, but       *
  *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
  *   General Public License for more details.                               *
  *                                                                          *
  *   A copy of the GNU General Public License is available on the World     *
  *   Wide Web at <http://www.gnu.org/copyleft/gpl.html>. You can also       *
  *   obtain it by writing to the Free Software Foundation,                  *
  *   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.         *
  *                                                                          *
  ****************************************************************************

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andrés Colubri

*******************************************************************************/

#include "moFilterManager.h"

#include "moArray.cpp"
moDefineDynamicArray(moTrackerFeatureArray)

moTrackerSystemData::moTrackerSystemData() {
        m_PositionMatrix =  new int [4*4];
        m_MotionMatrix =  new int [4*4];
        m_AccelerationMatrix =  new int [4*4];
        m_CircularPositionMatrix =  new int [3*12];
        m_CircularMotionMatrix =  new int [3*12];
        ResetMatrix();
}

moTrackerSystemData::~moTrackerSystemData() {
        delete [] m_PositionMatrix;
        delete [] m_MotionMatrix;
        delete [] m_CircularMotionMatrix;
        delete [] m_CircularPositionMatrix;
}

void moTrackerSystemData::SetMax( float x, float y ) {
    m_Max = moVector2f(x,y);
}

void moTrackerSystemData::SetMin( float x, float y ) {
    m_Min = moVector2f(x,y);
}

moVector2f moTrackerSystemData::GetMax() {
    return m_Max;
}

moVector2f moTrackerSystemData::GetMin() {
    return m_Min;
}

int moTrackerSystemData::GetFeaturesCount() {
    return m_Features.Count();
}

moTrackerFeature* moTrackerSystemData::GetFeature(int i) {
    return m_Features.Get(i);
}

moTrackerFeatureArray& moTrackerSystemData::GetFeatures() {
    return m_Features;
}

moVector2f moTrackerSystemData::GetBarycenter() {
        return m_Barycenter;
}

moVector2f moTrackerSystemData::GetBarycenterMotion() {
        return m_BarycenterMotion;
}

moVector2f moTrackerSystemData::GetBarycenterAcceleration() {
        return m_BarycenterAcceleration;
}

moVector2f moTrackerSystemData::GetVariance() {
        return m_Variance;
}


int moTrackerSystemData::GetValidFeatures() {
        return m_ValidFeatures;
}

void moTrackerSystemData::ResetMatrix() {
    for(int i=0; i<16;i++) {
        m_PositionMatrix[i] = 0;
        m_MotionMatrix[i] = 0;
        m_AccelerationMatrix[i] = 0;
    }

    for(int i=0; i<32;i++) {
        m_CircularMotionMatrix[i] = 0;
        m_CircularPositionMatrix[i] = 0;
    }
}

///Transforma el vector de posicion (0..1) al indice de zona de la matriz cuadrada
/// x must be 0..1
int moTrackerSystemData::PositionToZone( float x, float y ) {
        float i = floor( x * (float) 3.999999 );
        float j = floor( y * (float) 3.999999 );

        return ( (int)i + ((int)j)*4 );
}

///Transforma el indice de zona al vector posicion
/// es el centro del cuadrado lo q devuelve!!!
moVector2f moTrackerSystemData::ZoneToPosition( int zone ) {
        int j = zone / 4;
        int i = zone - j*4;
        return moVector2f( 0.125 + (float)i / (float) 4.0,0.125 + (float)j / (float) 4.0);
}

///Transforma el vector de posicion (0..1) a coordenadas polares luego al indice de zona de la matriz
int moTrackerSystemData::PositionToZoneC( float x, float y )  {

        moVector2f PosRePos;
        float Teta;
        float Radius;
        float MaxRadius;
        moVector2f MaxCuad;

        PosRePos = moVector2f( x - m_Barycenter.X(), y - m_Barycenter.Y() );

        if ( PosRePos.X() > 0 && PosRePos.Y() >= 0 ) {
            Teta = atan( PosRePos.Y() / PosRePos.X() );
        } else if ( PosRePos.X() > 0 && PosRePos.Y() < 0 ) {
            Teta = atan( PosRePos.Y() / PosRePos.X() ) + moMathf::TWO_PI;
        } else if( PosRePos.X() < 0 ) {
            Teta = atan( PosRePos.Y() / PosRePos.X() ) + moMathf::PI;
        } else if (PosRePos.X() == 0 && PosRePos.Y() > 0) {
            Teta = moMathf::PI / 2;
        } else if (PosRePos.X() == 0 && PosRePos.Y() < 0) {
            Teta = 3 * moMathf::PI / 2;
        }

        Radius = PosRePos.Length();

        MaxCuad = ( m_Max - m_Min ) ;
        MaxRadius = fabs( MaxCuad.Length() ); ///&????
        ( MaxRadius > 0 ) ? MaxRadius = MaxRadius : MaxRadius = 1.5; ///sqrt(2)

        float i = floor( Teta / moMathf::TWO_PI * (float) 11.999999 );
        float j = floor( Radius / MaxRadius * (float) 2.999999 );

        return ( (int)i + ((int)j)*12 );
}

///Transforma el indice de zona al vector posicion circular!
moVector2f moTrackerSystemData::ZoneToPositionC( int zone ) {

        moVector2f PosRePos;
        float Teta;
        float Radius;
        float MaxRadius;
        moVector2f MaxCuad;

        int j = zone / 12;
        int i = zone - j*12;

        PosRePos = moVector2f( m_Barycenter.X(), m_Barycenter.Y() );

        MaxCuad = ( m_Max - m_Min ) ;
        MaxRadius = fabs( MaxCuad.Length() ); ///&????
        ( MaxRadius > 0 ) ? MaxRadius = MaxRadius : MaxRadius = 1.5; ///sqrt(2)

        Teta = ( moMathf::TWO_PI * i ) / 12.0;
        Radius = ( MaxRadius * j ) / 3.0;

        float x,y;

        x = Radius * cos ( Teta ) + PosRePos.X();
        y = Radius * sin ( Teta ) + PosRePos.Y();

        return moVector2f( x, y);
}





/*
moVector2f& pos
*/
///POSITION


void moTrackerSystemData::SetPositionMatrix( float x, float y, int nfeatures ) {
    m_PositionMatrix[PositionToZone(x,y)]+=1;
}

void moTrackerSystemData::SetPositionMatrix( moVector2f pos, int nfeatures ) {
    m_PositionMatrix[PositionToZone(pos.X(),pos.Y())]+=1;
}


int moTrackerSystemData::GetPositionMatrix( float x, float y ) {
    return m_PositionMatrix[PositionToZone(x,y)];
}

int moTrackerSystemData::GetPositionMatrix( moVector2f pos ) {
    return m_PositionMatrix[PositionToZone(pos.X(),pos.Y())];
}


///MOTION



void moTrackerSystemData::SetMotionMatrix( float x, float y, int nfeatures ) {
    m_MotionMatrix[PositionToZone(x,y)]+=1;
}

int moTrackerSystemData::GetMotionMatrix( float x, float y ) {
    return m_MotionMatrix[PositionToZone(x,y)];
}

void moTrackerSystemData::SetMotionMatrix( moVector2f pos, int nfeatures ) {
    m_MotionMatrix[PositionToZone(pos.X(),pos.Y())]+=1;
}

int moTrackerSystemData::GetMotionMatrix( moVector2f pos ) {
    return m_MotionMatrix[PositionToZone(pos.X(),pos.Y())];
}



///ACCELERATION

void moTrackerSystemData::SetAccelerationMatrix( float x, float y, int nfeatures ) {
    m_AccelerationMatrix[PositionToZone(x,y)]+=1;
}

int moTrackerSystemData::GetAccelerationMatrix( float x, float y ) {
    return m_AccelerationMatrix[PositionToZone(x,y)];
}

void moTrackerSystemData::SetAccelerationMatrix( moVector2f pos, int nfeatures ) {
    m_AccelerationMatrix[PositionToZone(pos.X(),pos.Y())]+=1;
}

int moTrackerSystemData::GetAccelerationMatrix( moVector2f pos ) {
    return m_AccelerationMatrix[PositionToZone(pos.X(),pos.Y())];
}


///POSITION CIRCULAR



void moTrackerSystemData::SetPositionMatrixC( float x, float y, int nfeatures ) {
    m_CircularPositionMatrix[PositionToZoneC(x,y)]+=1;
}

int moTrackerSystemData::GetPositionMatrixC( float x, float y ) {
    return m_CircularPositionMatrix[PositionToZoneC(x,y)];
}

void moTrackerSystemData::SetMotionMatrixC( float x, float y, int nfeatures ) {
    m_CircularMotionMatrix[PositionToZoneC(x,y)]+=1;
}

int moTrackerSystemData::GetMotionMatrixC( float x, float y ) {
    return m_CircularMotionMatrix[PositionToZoneC(x,y)];
}


/*!
\fn moTrackerFeature::moTrackerFeature()
\brief Constructor
*/
moTrackerFeature::moTrackerFeature()
{
	x = -1; y = -1; valid = false;
	track.Empty();
}


moTrackerFeature::~moTrackerFeature()
{
}

/*!
\fn void GpuKLT_Feature::print();
\brief Display Feature Details
*/
void moTrackerFeature::print()
{
	printf("Pos:\t%10.5f,%10.5f\t(%7.2f,%7.2f)\n",normx,normy,x,y);
}

/*!
\fn void GpuKLT_Feature::printTracks()
\brief Display Feature Tracks
*/
void moTrackerFeature::printTracks()
{
	MOuint	n = track.Count();

	if (n==0)
		printf("(%7.2f,%7.2f)\n", x, y);
	else
		printf("(%7.2f,%7.2f) -> (%7.2f,%7.2f)\n", track[n-1]->X(), track[n-1]->Y(), x, y);
}



/*!
\fn int moTrackerFeature::updatePos(float kltConvergeThreshold, float kltSSDthresh, int kltborder, float delta, float res, float d1, float d2, float w, float h)
\brief Update the Feature Position (d1,d2) are the delta_x and delta_y resp.
*/
int moTrackerFeature::updatePos(float kltConvergeThreshold, float kltSSDthresh, int kltborder, float delta, float res, float d1, float d2, float w, float h)
{

	double delx, dely;
	bool  discardFlag;

	if (res > kltSSDthresh)
		discardFlag = true;
	else if (d1 < 0.0 && d2 < 0.0)
		discardFlag = true;
	else
	{
		moVector2f * pt = new moVector2f(x,y);
		track.Add(pt);

		delx         = normx - d1;
		dely         = normy - d2;
		normx        = normx - delx;
		normy		 = normy - dely;
		x			 = normx * w;
		y			 = normy * h;

		if ( (x < kltborder) || (x > (w - kltborder)) || (y < kltborder) || (y > (h - kltborder)) )
			discardFlag = true;
		else
			discardFlag = false;
	}

	if (discardFlag)
	{

		//for (int j=0;j<track.size();j++)
		//	delete track[j];
		track.Empty();
		valid = false;
		return -1;
	}
	else
		return 1;
}


//===========================================
//
//				moFilter
//
//===========================================




//===========================================
//
//				moFilterManager
//
//===========================================

moFilterManager::moFilterManager() {

	SetType( MO_OBJECT_RESOURCE );
	SetResourceType( MO_RESOURCETYPE_FILTER );

	SetName("Filter Manager");

}

moFilterManager::~moFilterManager() {

}

MOboolean moFilterManager::Init() {
	return true;
}


MOboolean moFilterManager::Finish() {
	return true;
}



