/*******************************************************************************

                              moTrackerKLT.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                     *
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

  Copyright(C) 2007 Andrés Colubri

  Authors:
  Andrés Colubri
  Fabricio Costa

*******************************************************************************/

#include "moTrackerKLT.h"

using namespace TUIO;



/*********************

  NOTAS:

  el tracker podrá funcionar como un thread que vaya calculando en funcion de q va llegando la info,
  o mejor aun, que trate de calcular, y cuando llega a un resultado el efecto en cuestion tome ese valor.

  //para que el tracker funcione sin shaders, debemos hacer el calculo antes de que se pase la informacion a la textura,
  para ello quizas no sea conveniente trabajar con threads, sino bloquear la ejecucion hasta terminar de tomar los valores q nos interesan.




***********************/


//========================
//  Factory
//========================

moTrackerKLTFactory *m_TrackerKLTFactory = NULL;

MO_PLG_API moResourceFactory* CreateResourceFactory(){
	if (m_TrackerKLTFactory==NULL)
		m_TrackerKLTFactory = new moTrackerKLTFactory();
	return (moResourceFactory*) m_TrackerKLTFactory;
}

MO_PLG_API void DestroyResourceFactory(){
	delete m_TrackerKLTFactory;
	m_TrackerKLTFactory = NULL;
}

moResource*  moTrackerKLTFactory::Create() {
	return new moTrackerKLT();
}

void moTrackerKLTFactory::Destroy(moResource* fx) {
	delete fx;
}


//===========================================
//
//     Class: moTrackerKLTSystem
//
//===========================================

moTrackerKLTSystem::moTrackerKLTSystem()
{
    m_pTrackerSystemData = NULL;
    m_pTrackerSystemData = new moTrackerSystemData(2,2);

    m_pTUIOSystemData = NULL;
    m_pTUIOSystemData = new moTUIOSystemData();

    m_Objects.Init( 0 , NULL );

    Uplas = NULL;

    m_diffMode = false;
    m_replaceLostFeatures = true;

	m_nCols = 0;
	m_nRows = 0;
	m_nFrames = 0;
	m_nCurrentFrame = 0;

    m_tc = NULL;
    m_fl = NULL;
    m_ft = NULL;

	m_img1 = NULL;
	m_img2 = NULL;
	m_img_diff0 = NULL;
	m_img_diff = NULL;
	m_img_sum = NULL;

	m_sizebuffer = 0;
	m_buffer = NULL;

	m_init = false;
	m_started = false;
	m_just_started = false;
	m_curr_sample = 0;
}

moTrackerKLTSystem::~moTrackerKLTSystem()
{
	Finish();
}

MOboolean moTrackerKLTSystem::Init(
    MOint p_nFeatures,
    MOint p_width,
    MOint p_height,
    MOboolean p_replaceLostFeatures,
    MOboolean p_sequentialMode,
    MOboolean p_diffMode,
	MOint p_nFrames,
	MOint p_num_samples,
	MOint p_mindist,
	MOfloat p_mineigen,
	MOboolean p_lighting_insensitive )
{
	if (p_width<=0 || p_height==0) return false;

    m_nCols = p_width;
	m_nRows = p_height;

    m_img1 = (unsigned char *) malloc(m_nCols * m_nRows *sizeof(unsigned char));
    m_img2 = (unsigned char *) malloc(m_nCols * m_nRows *sizeof(unsigned char));
	m_img_diff0 = (unsigned char *) malloc(m_nCols * m_nRows *sizeof(unsigned char));
    m_img_diff = (unsigned char *) malloc(m_nCols * m_nRows *sizeof(unsigned char));
	m_img_sum = (float *) malloc(m_nCols * m_nRows *sizeof(float));
	for (int i = 0; i < m_nCols * m_nRows; i++) *(m_img_sum + i) = 0.0;

	m_tc = KLTCreateTrackingContext();
	m_tc->sequentialMode = p_sequentialMode;
    m_tc->mindist = p_mindist;
	m_tc->min_eigenvalue = p_mineigen;
	m_tc->lighting_insensitive = p_lighting_insensitive;

	m_tc->borderx = 2;//24
	m_tc->bordery = 2;//24
	m_tc->nPyramidLevels = 2;//2
	m_tc->subsampling = 4;//4
	m_tc->max_iterations = 10;
    m_tc->max_residue = 10;

    m_tc->window_width = 4;//7
    m_tc->window_height = 4;//7
    m_tc->min_displacement = 0.10000001;//0.10000001
    m_tc->min_determinant = 0.0099999978;//0.0099999978
    m_tc->max_residue = 10;


	m_fl = KLTCreateFeatureList(p_nFeatures);
	m_nFrames = p_nFrames;
	if (0 < p_nFrames) m_ft = KLTCreateFeatureTable(p_nFrames, p_nFeatures);

    m_diffMode = p_diffMode && m_tc->sequentialMode;
	m_replaceLostFeatures = p_replaceLostFeatures;

	m_init = true;

	m_num_samples = p_num_samples;

    KLTPrintTrackingContext(m_tc);

	return true;
}

MOboolean moTrackerKLTSystem::Init(MOint p_nFeatures, moVideoSample* p_pVideoSample,
		      MOboolean p_replaceLostFeatures, MOboolean p_sequentialMode, MOboolean p_diffMode,
			  MOint p_nFrames, MOint p_num_samples, MOint p_mindist, MOfloat p_mineigen, MOboolean p_lighting_insensitive)
{
	if ( p_pVideoSample != NULL && m_pTrackerSystemData!=NULL )
		m_pTrackerSystemData->GetVideoFormat() = p_pVideoSample->m_VideoFormat;
	else return false;

	if ( m_pTrackerSystemData->GetVideoFormat().m_Width<=0 || m_pTrackerSystemData->GetVideoFormat().m_Height==0)
		return false;

	MOboolean res = Init(p_nFeatures, m_pTrackerSystemData->GetVideoFormat().m_Width, m_pTrackerSystemData->GetVideoFormat().m_Height,
							p_replaceLostFeatures, p_sequentialMode, p_diffMode,
							p_nFrames, p_num_samples, p_mindist, p_mineigen, p_lighting_insensitive);

	if (res)
	{
        m_pTrackerSystemData->SetMaxFeatures(p_nFeatures);
        m_pTrackerSystemData->m_Distancias = new float* [m_pTrackerSystemData->GetMaxFeatures()];
        m_pTrackerSystemData->m_Pares = new int* [m_pTrackerSystemData->GetMaxFeatures()*2];

        Uplas = new int [m_pTrackerSystemData->GetMaxFeatures()];
        for( int up=0; up<m_pTrackerSystemData->GetMaxFeatures(); up++) {
            Uplas[up] = 0;
        }

        for(int i=0; i<m_pTrackerSystemData->GetMaxFeatures(); i++) {
            m_pTrackerSystemData->m_Distancias[i] = new float [m_pTrackerSystemData->GetMaxFeatures()];
            m_pTrackerSystemData->m_Pares[i*2] = new int [2];
            m_pTrackerSystemData->m_Pares[i*2+1] = new int [2];
        }

        TuioTime::initSession();

		return true;
	}
	else return false;
}

MOboolean moTrackerKLTSystem::Finish()
{
    if (m_pTrackerSystemData) {
        for(int i=0; i<m_pTrackerSystemData->GetMaxFeatures(); i++) {
            delete [] m_pTrackerSystemData->m_Distancias[i];
            delete [] m_pTrackerSystemData->m_Pares[i*2];
            delete [] m_pTrackerSystemData->m_Pares[i*2+1];
        }

        delete [] m_pTrackerSystemData->m_Distancias;
        delete [] m_pTrackerSystemData->m_Pares;

        delete m_pTrackerSystemData;
    }
    m_pTrackerSystemData = NULL;

    if (Uplas) {
        delete [] Uplas;
        Uplas = NULL;
    }

	if (m_tc != NULL)
	{
		KLTFreeTrackingContext(m_tc);
	    m_tc = NULL;
	}
	if (m_fl != NULL)
	{
	    KLTFreeFeatureList(m_fl);
		m_fl = NULL;
	}
	if (m_ft != NULL)
	{
	    KLTFreeFeatureTable(m_ft);
		m_ft = NULL;
	}

	if (m_img1 != NULL)
	{
        free(m_img1);
		m_img1 = NULL;
	}
	if (m_img2 != NULL)
	{
        free(m_img2);
		m_img2 = NULL;
	}
	if (m_img_diff0 != NULL)
	{
        free(m_img_diff0);
		m_img_diff0 = NULL;
	}
	if (m_img_diff != NULL)
	{
        free(m_img_diff);
		m_img_diff = NULL;
	}
	if (m_img_sum != NULL)
	{
        free(m_img_sum);
		m_img_sum = NULL;
	}

	m_sizebuffer = 0;
	m_buffer = NULL;
	return true;
}

void moTrackerKLTSystem::GetFeature(MOint p_feature, MOfloat &x, MOfloat &y, MOfloat &val)
{
    x = m_fl->feature[p_feature]->x;
    y = m_fl->feature[p_feature]->y;
    val = m_fl->feature[p_feature]->val;
}

void moTrackerKLTSystem::GetFeature(MOint p_feature, MOint p_frame, MOfloat &x, MOfloat &y, MOfloat &val)
{
    x = m_ft->feature[p_feature][p_frame]->x;
    y = m_ft->feature[p_feature][p_frame]->y;
    val = m_ft->feature[p_feature][p_frame]->val;
}

void moTrackerKLTSystem::Track(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
	if (m_started) {
      ContinueTracking(p_pBuffer, p_RGB_mode);
	} else StartTracking(p_pBuffer, p_RGB_mode);
}

void moTrackerKLTSystem::StartTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
	if (m_curr_sample < m_num_samples)
	{
        AddBufferToImg(p_pBuffer, p_RGB_mode);
        m_curr_sample++;
	}
	else
	{
		CalcImg1Average();

		if (!m_diffMode)
		{
			KLTSelectGoodFeatures(m_tc, m_img1, m_nCols, m_nRows, m_fl);
			if (0 < m_nFrames) KLTStoreFeatureList(m_fl, m_ft, 0);
		}

		m_started = m_just_started = true;
	}
}

void moTrackerKLTSystem::StartTrackingDiffMode()
{
	CalcImgDiff0(m_img2, m_img1);
	KLTSelectGoodFeatures(m_tc, m_img_diff0, m_nCols, m_nRows, m_fl);
	if (0 < m_nFrames) KLTStoreFeatureList(m_fl, m_ft, 0);
}

void moTrackerKLTSystem::ContinueTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
	CopyBufferToImg(p_pBuffer, m_img2, p_RGB_mode);

	if (m_diffMode && m_just_started)
		// Tracking in difference mode needs to initialization steps, the first was already
		// executed in StartTracking and consisted in saving the first image buffer.
		// The second step consists in generating the reference difference m_img_diff0.
		StartTrackingDiffMode();
	else
	{
		if (m_diffMode)
 		{
			CalcImgDiff(m_img2, m_img1);

			KLTTrackFeatures(m_tc, m_img_diff0, m_img_diff, m_nCols, m_nRows, m_fl);
			if (m_replaceLostFeatures) KLTReplaceLostFeatures(m_tc, m_img_diff, m_nCols, m_nRows, m_fl);

		}
		else
		{
			CalcImgDiff(m_img2, m_img1);
			//MODebug2->Push( moText("Tracking current frame:") + IntToStr(m_nCurrentFrame) + moText("Tracking nframes:")+ IntToStr(m_nFrames) );
			KLTTrackFeatures(m_tc, m_img1, m_img2, m_nCols, m_nRows, m_fl);
			if (m_replaceLostFeatures || m_nCurrentFrame==0) KLTReplaceLostFeatures(m_tc, m_img2, m_nCols, m_nRows, m_fl);
		}

		if (0 < m_nFrames && (m_nCurrentFrame < m_nFrames))
		{
			KLTStoreFeatureList(m_fl, m_ft, m_nCurrentFrame);

			m_nCurrentFrame++;
		} else m_nCurrentFrame = 0;

		if (!m_tc->sequentialMode) CopyImg2To1();
	}

	m_just_started = false;
}



void moTrackerKLTSystem::SystemData2TUIO() {

    //Ahora con nzuestro algoritmo creamos los Cursores y los Objetos en TUIO
/*
    int aa = 0;
    for(int hh = 0; hh < m_pTrackerSystemData->m_ZoneH; hh++) {
        for(int ww = 0 ; ww < m_pTrackerSystemData->m_ZoneW; ww++) {

            moTrackerFeatureArray&  ZONE(m_pTrackerSystemData->m_pZones[aa]);

            aa = aa +1;

            for(int pp=0; pp < ZONE.Count(); pp++) {
                moTrackerFeature* pFeatureA = ZONE[pp];

                    ///self zone
                    IterateZone( ww, hh, pp, pFeatureA );

                if (ww>0)
                    IterateZone( ww-1, hh, 0, pFeatureA );

                if (ww>0 && hh>0)
                    IterateZone( ww-1, hh-1, 0, pFeatureA );

                if (hh>0)
                    IterateZone( ww, hh-1, 0, pFeatureA );

                if (hh>0 && ww<(m_pTrackerSystemData->m_ZoneW-1))
                    IterateZone( ww+1, hh-1, 0, pFeatureA );

                if (ww<(m_pTrackerSystemData->m_ZoneW-1))
                    IterateZone( ww+1, hh, 0, pFeatureA );

                if (ww<(m_pTrackerSystemData->m_ZoneW-1) && hh<(m_pTrackerSystemData->m_ZoneH-1))
                    IterateZone( ww+1, hh+1, 0, pFeatureA );

                if (hh<(m_pTrackerSystemData->m_ZoneH-1))
                    IterateZone( ww, hh+1, 0, pFeatureA );

                if (ww>0 && hh<(m_pTrackerSystemData->m_ZoneH-1))
                    IterateZone( ww-1, hh+1, 0, pFeatureA );


            }
        }
    }
*/
}

moVector2f AverageSystem( moTrackerFeature* pFeature ) {

   moVector2f Average( pFeature->x, pFeature->y );
    for(int i=0; i<pFeature->FeaturesCaptured.Count(); i++) {
        Average+= moVector2f(   pFeature->FeaturesCaptured[i]->x,
                                pFeature->FeaturesCaptured[i]->y );
    }
    Average/=(pFeature->FeaturesCaptured.Count()+1);
    return Average;
}

float Distance(moVector2f V1, moVector2f V2) {

    moVector2f Average = V1 - V2;
    return Average.Length();
}

float Distance( moTrackerFeature* pFeature1, moTrackerFeature* pFeature2) {
    moVector2f Average = AverageSystem(pFeature1) - AverageSystem(pFeature2);
    return Average.Length();
}


void moTrackerKLTSystem::IterateZone( int ww, int hh, int pp, moTrackerFeature* pFeatureA ) {
/*
    ///nos reposicionamos en la nueva zona
    float MINDIST = 0.2;

    int aa = ww + hh * m_pTrackerSystemData->m_ZoneW;

    moTrackerFeatureArray&  ZONE(m_pTrackerSystemData->m_pZones[aa]);


    ///primero chequear con esta zona, a partir del ultimo que vimos...pp
    for(int pp2=pp; pp2 < ZONE.Count(); pp2++) {


        moTrackerFeature* pFeatureB = ZONE[pp2];

        moVector2f distance = moVector2f(
                pFeatureA->x - pFeatureB->x,
                pFeatureA->y - pFeatureB->y
                );

        moVector2i syncro = moVector2i( 0, pFeatureA->stime - pFeatureB->stime );

        ///esta lo suficientemente cerca? y en syncro? entonces son candidatos a objeto
        ///if (!pFeatureB->is_object) { ///dont pass 2 times over the same point
        if (pFeatureB!=pFeatureA
        && pFeatureB->Parent!=pFeatureB
        && pFeatureB->Parent!=pFeatureA
        && pFeatureA->Parent!=pFeatureB
        && pFeatureA->Parent!=pFeatureA )
            if (distance.Length() < MINDIST &&  syncro.Length() <= 60 ) {
                    ///FIRST CASE, A NOTHING, B NOTHING
                    if (!pFeatureA->is_parent && !pFeatureA->is_object && !pFeatureB->is_parent && !pFeatureB->is_object) {
                        ///A //WE MARK BOTH
                        pFeatureA->is_parent = true;
                        pFeatureA->is_object = false;
                        pFeatureA->FeaturesCaptured.Add( pFeatureB );

                        ///B
                        pFeatureB->is_parent = false;
                        pFeatureB->is_object = true;
                        pFeatureB->Parent = pFeatureA; ///child oh him


                    ///SECOND CASE: A IS NOTHING, B IS A CHILD
                    } else if ( !pFeatureA->is_object && !pFeatureA->is_parent && pFeatureB->is_object) {
                        ///we see if the parent of the child is near enough
                        ///maybe we can use an average distance of the all system of the parent (interesting)
                        moVector2f distance2 = moVector2f(
                            pFeatureA->x - pFeatureB->Parent->x,
                            pFeatureA->y - pFeatureB->Parent->y
                            );
                        if (distance2.Length() < MINDIST &&  syncro.Length() <= 60 ) {
                            pFeatureA->is_parent = false;
                            pFeatureA->is_object = true;
                            pFeatureA->Parent = pFeatureB->Parent;

                            pFeatureB->Parent->FeaturesCaptured.Add( pFeatureA );
                        } else {
                            ///do nothing and continue!!! maybe just a cursor
                        }

                    ///THIRD CASE: A IS A PARENT, B IS A PARENT
                    }
                    else if ( pFeatureA->is_parent && pFeatureB->is_parent ) {
                        ///let see if their childs are close enough to bond
                        if ( Distance( pFeatureA, pFeatureB ) < MINDIST ) {
                            pFeatureB->is_parent = false;
                            pFeatureB->is_object = true;
                            for(int i=0; i<pFeatureB->FeaturesCaptured.Count(); i++) {
                                ///assign new parent to childs
                                pFeatureB->FeaturesCaptured[i]->Parent = pFeatureA;
                                ///add childs to parent list
                                pFeatureA->FeaturesCaptured.Add(pFeatureB->FeaturesCaptured[i]);
                            }
                            pFeatureB->FeaturesCaptured.Empty();
                        }
                    ///4TH CASE: A IS A PARENT, B IS A CHILD
                    } else if (pFeatureA->is_parent && pFeatureB->is_object ) {

                        float distanceA = Distance( AverageSystem( pFeatureA ), moVector2f( pFeatureB->x,pFeatureB->y) );
                        float distanceB = Distance( AverageSystem( pFeatureB ), moVector2f( pFeatureB->x,pFeatureB->y) );

                        if ( distanceA < distanceB ) {
                            int remove;
                            for( int re=0; re < pFeatureB->Parent->FeaturesCaptured.Count(); re++ ) {
                                if (pFeatureB->Parent->FeaturesCaptured[re] == pFeatureB) remove = re;
                            }
                            pFeatureB->Parent->FeaturesCaptured.Remove(remove);
                            pFeatureB->Parent = pFeatureA;
                            pFeatureA->FeaturesCaptured.Add(pFeatureB);

                        }

                    }


            }

    }
*/
}




void moTrackerKLTSystem::NewData( moVideoSample* p_pVideoSample )
{
	if (p_pVideoSample)
	{
		moBucket* pBucket = (moBucket*)p_pVideoSample->m_pSampleBuffer;
		if (pBucket && m_pTrackerSystemData )
		{
            m_pTrackerSystemData->GetVideoFormat() = p_pVideoSample->m_VideoFormat;
			m_sizebuffer = pBucket->GetSize();
			m_buffer = pBucket->GetBuffer();
		}
		else return;
	}
	else return;

	if (m_buffer == NULL) return;

	Track(m_buffer, GL_RGB);

	//trasnform data:


	for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
	    delete m_pTrackerSystemData->GetFeatures().Get(i);
	}


    ///RESET DATA !!!!
	m_pTrackerSystemData->GetFeatures().Empty();
	m_pTrackerSystemData->ResetMatrix();

    ///GET NEW DATA!!!!
	moTrackerFeature* TF = NULL;

	int tw = m_pTrackerSystemData->GetVideoFormat().m_Width;
    int th = m_pTrackerSystemData->GetVideoFormat().m_Height;


	float sumX = 0.0f,sumY = 0.0f;
	float sumN = 0.0f;
	float varX = 0.0f, varY = 0.0f;
    float minX = 1.0f, minY = 1.0;
	float maxX = 0.0f, maxY = 0.0;

    float vel,acc;
    m_pTrackerSystemData->nPares = 0;

	for(int i=0; i<m_fl->nFeatures; i++ ) {

	    TF = new moTrackerFeature();
        if (TF) {

           ///NUEVA VERSION
            TF->x = m_fl->feature[i]->x / (float)tw;
            TF->y = m_fl->feature[i]->y / (float)th;
            TF->tr_x = TF->x;
            TF->tr_y = TF->y;

            ///MODebug2->Log( moText("table: x:") + FloatToStr(m_ft->feature[i][0]->x) + moText("table: y:") + FloatToStr(m_ft->feature[i][0]->y ));

            TF->val = m_fl->feature[i]->val;
            TF->valid = (m_fl->feature[i]->val >= KLT_TRACKED);


            ///TAIL
            /// from 4th frame
            if (TF->val>=0) {
               if (m_nCurrentFrame>4 && 4<m_nFrames) {
                   for(int j=0; j<4; j++) {
                       if (m_ft) {
                           if ( m_ft->feature[i][m_nCurrentFrame-j] && m_ft->feature[i][m_nCurrentFrame-j]->val>=KLT_TRACKED ) {
                                TF->tr_x = m_ft->feature[i][m_nCurrentFrame-j]->x /  (float)tw;
                                TF->tr_y = m_ft->feature[i][m_nCurrentFrame-j]->y /  (float)th;
                           } else break;
                       }
                   }
               }


            /**=============================================
            ///CALCULATE DISTANCIAS!!!!
            =============================================*/

               float distancia = 0.0f;
               float dx,dy;
               float a1 = min_segment_len, a2 = max_segment_len;

               a1*=a1;
               a2*=a2;

               ///Reconocer los que tienen un par, de los que tienen 2
               float min = 1000000.0;
               float max = 0.0;
               float dis = 0.0;

               ///ponemos en 0 el set de Uplas....
               Uplas[i] = 0;

               if (m_pTrackerSystemData->m_Pares!=NULL || m_pTrackerSystemData->m_Distancias!=NULL) {
               for(int k=0; k<m_fl->nFeatures; k++ ) {

                   ///no comparamos lo innecesario
                   /// i < k, para no comparar 2 veces los mismos pares
                   /// de la diagonal para arriba , la matriz no se llena
                   ///         (0)(1)(2)  (i)
                   ///     (0)  X  X  X
                   /// (k) (1)  O  X  X   ( ( N -1) x N ) / 2   N = 100 > PARES = 45
                   ///     (2)  O  O  X
                   if (i<k && i!=k && m_fl->feature[k]->val >= KLT_TRACKED) {

                        dx = ( TF->x - ( m_fl->feature[k]->x / (float) tw ));
                        dy = ( TF->y - (m_fl->feature[k]->y / (float) th) );

                        dis = dx*dx + dy*dy;
                        if (dis>max) max = dis;
                        if (dis<min) min = dis;

                        ///tomamos aquellos puntos dentro de un rango de distancia
                        if ( a1 <= dis && dis <= a2 ) {
                            ///guardamos los pares que nos interesan, con sus indices...
                            /// [0] => i
                            /// [1] => k    i < k

                            ///atencion de no pasarnos ( pusimos un max de Nx2 pares que cumpliran con nuestra formula )
                            if (m_pTrackerSystemData->nPares<m_pTrackerSystemData->GetMaxFeatures()*2) {

                                m_pTrackerSystemData->m_Pares[m_pTrackerSystemData->nPares][0] = i;
                                m_pTrackerSystemData->m_Pares[m_pTrackerSystemData->nPares][1] = k;

                            ///sumamos al contador de pares
                                m_pTrackerSystemData->nPares++;
                            }

                        }
                        m_pTrackerSystemData->m_Distancias[i][k] = dis;


                   }

                }
               } else MODebug2->Error(moText("m_Pares NULL"));
                //MODebug2->Push(moText("npares:")+IntToStr(m_pTrackerSystemData->nPares));
                //MODebug2->Push(moText("dis:")+FloatToStr(dis));
                //MODebug2->Push(moText("max:")+FloatToStr(max));

            ///=================================================

/*
                ///CALCULATE AVERAGE FOR BARYCENTER AND VARIANCE
               sumX+= TF->x;
               sumY+= TF->y;

               sumN+= 1.0f;

               ///maximos
               if (TF->x>maxX) maxX = TF->x;
               if (TF->y>maxY) maxY = TF->y;

               ///minimos
               if (TF->x<minX) minX = TF->x;
               if (TF->y<minY) minY = TF->y;

               ///esta es simplemente una matriz que cuenta la cantidad de....
               m_pTrackerSystemData->SetPositionMatrix( TF->x, TF->y, 1 );
               ///genera la matrix de referencia rapida por zonas
               ///m_pTrackerSystemData->SetPositionMatrix( TF );
*/

            }

            //MODebug2->Push( moText("debugging") + IntToStr(m_pTrackerSystemData->GetMaxFeatures()));



            ///CALCULATE VELOCITY AND ACCELERATION
            TF->vp_x = TF->v_x;
            TF->vp_y = TF->v_x;
            TF->v_x = TF->x - TF->tr_x;
            TF->v_y = TF->y - TF->tr_y;
            TF->a_x = TF->v_x - TF->vp_x;
            TF->a_y = TF->v_y - TF->vp_y;


            vel = moVector2f( TF->v_x, TF->v_y ).Length();
            acc = moVector2f( TF->a_x, TF->a_y ).Length();

            //if ( vel >= 0.01 ) m_pTrackerSystemData->SetMotionMatrix( TF->x, TF->y, 1 );
            //if ( acc >= 0.01 ) m_pTrackerSystemData->SetAccelerationMatrix( TF->x, TF->y, 1 );

            m_pTrackerSystemData->GetFeatures().Add(TF);
        }
    }


    /** ===========================================================================

    TUIO SYSTEM

     ===========================================================================*/
/*
void SimpleSimulator::run() {
	running=true;
	while (running) {
		currentTime = TuioTime::getSessionTime();
		tuioServer->initFrame(currentTime);
		processEvents();
		tuioServer->stopUntouchedMovingCursors();
		tuioServer->commitFrame();
		drawFrame();
		SDL_Delay(20);
	}
}
*/

if ( m_pTUIOSystemData ) {

    ///Initialization for TUIO frame
    TuioTime currentTime = TuioTime::getSessionTime();
    m_pTUIOSystemData->initFrame( currentTime );


    ///Analisis de uplas
    int k1;
    int k2;

    /// Ahora que tenemos cada par asociado:
    /// registramos cuantos segmentos tiene cada punto

    /// todos arrancan en 0 (se seteo mas arriba mientras se calculaban las distancias)!
    for( int g = 0; g < m_pTrackerSystemData->nPares; g++ ) {
        k1 = m_pTrackerSystemData->m_Pares[g][0];
        k2 = m_pTrackerSystemData->m_Pares[g][1];
        Uplas[k1]++;
        Uplas[k2]++;
    }


    /// Ahora veamos realmente que puntos,
    ///tienen por lo menos mas de 3 segmentos

    ///Utilizamos un arreglo para guardar ademas las referencias a estos puntos

    int myuplas[3] = { -1, -1, -1 };
    moVector2f shape[3];

    float p1x;
    float p1y;
    float angle;
    bool iscursor = true;
    int nupla = 0;

    for( int u=0; u<m_pTrackerSystemData->GetMaxFeatures(); u++) {

        moTrackerFeature* TF = m_pTrackerSystemData->GetFeature(u);
        iscursor = true;

        if (TF) {
            p1x = TF->x - 0.5;
            p1y = -TF->y + 0.5;
            moVector2f feat_v(p1x,p1y);
            moVector2f obj_v = feat_v;
            TF->is_object = false;

            if( Uplas[u]>=2) {

                ///DrawUpla(u);
                ///search for all points!

                ///aquellas esquinas que tienen puntos atados (podrian ser 2, a y c, o a y b)
                /// y que los otros 2 tengan alguno en comun: c y b, a y b
                /// U   a
                ///
                /// c   b
                ///
                if ( Uplas[u]>=3 ) {

                    TF->is_object = true;

                    nupla++;

                    int o = 0;


                    //MODebug2->Push( moText("Upla de 3!") + IntToStr(u) );

                    ///basado en este punto, recorremos los pares
                    ///para completar con los puntos que conectan con este
                    ///q tengan un angulo recto!!!
                    float escalar;

                    for( int pp=0; pp<m_pTrackerSystemData->nPares; pp++) {
                        k1 = m_pTrackerSystemData->m_Pares[pp][0];
                        k2 = m_pTrackerSystemData->m_Pares[pp][1];
                        if (o<3) if (k1==u) myuplas[o++] = k2;
                        if (o<3) if (k2==u) myuplas[o++] = k1;

                    }

                    moVector2f vectMasLargo(0,0);
                    moVector2f vectPromedio(0,0);
                    moVector2f vectCovarianza(0,0);

                    float maxL = 0.0;
                    ///Ahora, si ya podemos calcular nuestro promedio de puntos...1 punto (aver) + 3 vecinos
                    ///intentando aproximar el centro del objeto
                    int countv = 1;
                    for( int pp=0; pp<3; pp++ ) {
                        int e = myuplas[pp];

                        shape[pp] = moVector2f( 0, 0); ///en 0 para medir luego angulos rectos

                        moTrackerFeature* NF = m_pTrackerSystemData->GetFeature(e);
                        if (NF) {
                            moVector2f av2( NF->x - 0.5, -NF->y + 0.5 );
                            obj_v+=av2;
                            countv++;
                            ///calculamos la coneccion mas larga para tomarla como referencia
                            moVector2f vectL = moVector2f( av2 - feat_v);
                            shape[countv-2] = vectL;
                            vectPromedio+=vectL;
                            if ( vectL.Normalize() > maxL ) {
                                 vectMasLargo = vectL;
                            }
                        }
                    }
                    obj_v/=(float)countv; ///promediamos, son 4 contando el origen, con este promedio
                    vectPromedio/=(float)(countv-1);


                    ///search for rect_angles, between them too?
                    int rect_angles = 0;
                    moVector2f direction;
                    float dotp_min = 100.0;

                    for(int r1=0; r1<countv; r1++) {
                        for(int r2=0; r2<countv,r2<r1; r2++) {
                            moVector2f& v1(shape[r1]);
                            moVector2f& v2(shape[r2]);
                            ///ya estan normalizadas
                            if (v1.Length()> 0.0 && v2.Length() > 0.0) {
                                float dotp = v1.Dot(v2);
                                if ( dotp < cos( 10 * moMathf::DEG_TO_RAD) ) {
                                    rect_angles++;
                                    if ( dotp < dotp_min ) {
                                        dotp_min = dotp;
                                        if (v1.Length()>v2.Length()) {
                                            vectMasLargo = v1;
                                        } else {
                                            vectMasLargo = v2;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    //vectMasLargo.Normalize();
                    /*
                    if ( vectMasLargo.Length()>0.001 ) {
                        angle = moMathf::ACos( vectMasLargo.X() );
                    } else angle = 0.0;
                    */
                    vectPromedio.Normalize();
                    angle = moMathf::ACos( vectPromedio.X() );

                    /**
                    ///aqui la idea es calcular el angulo en el que esta el cuadrado...o rectangulo
                    */

                    /**
                    ///calculemos la velocidad de rotacion!!!!
                    //sin necesidad del angulo....

                    ///QUEDA POR HACER, y puede reforzar la idea del sentido del objeto

                    */
                    if (rect_angles>0) {
                        ///usamos mejor el vector de la esquina....
                        //obj_v = feat_v;

                            TuioObject* pObject = NULL;
                            float mindist = 1000.0;

                            pObject = m_pTUIOSystemData->getClosestTuioObject( obj_v.X(), obj_v.Y());
                            if (pObject) {
                                mindist = pObject->getDistance( obj_v.X(), obj_v.Y() );
                                if ( mindist < 0.3 && pObject->getSymbolID()==u) {
                                    float a1 = pObject->getAngle();
                                    float a2 = angle;
                                    //m_pTUIOSystemData->initFrame( TuioTime::getSessionTime() );
                                    //m_pTUIOSystemData->updateTuioObject( pObject, obj_v.X(), obj_v.Y(), angle );
                                    pObject->update( TuioTime::getSessionTime(),  obj_v.X(), obj_v.Y(), angle );
                                   /* MODebug2->Push(
                                            moText(" u: ") +
                                            + IntToStr(u)

                                            + moText(" x:")
                                            + FloatToStr(obj_v.X())
                                            + moText(" y:")
                                            + FloatToStr(obj_v.Y())

                                            + moText(" angle 1:")
                                            + FloatToStr(a1)
                                            + moText(" angle 2:")
                                            + FloatToStr(a2)

                                            + moText(" rspeed:")
                                            + FloatToStr( pObject->getRotationSpeed() )

                                            );
*/

                                    //pObject->rotation_speed = a2 - a1;

                                    iscursor = false;
                                }
                            }
                            if ( mindist > 0.4 ) {
                                //m_pTUIOSystemData->initFrame(TuioTime::getSessionTime());
                                pObject = m_pTUIOSystemData->addTuioObject( u , obj_v.X(), obj_v.Y(), angle );
                                iscursor = false;
                            }


                    } else iscursor = true;

                } ///FIN UPLAS DE 3
                else iscursor = true;


            }///fin UPLAS >= 2   ///FIN ANALISIS de UPLAS OBJETOS Y CURSORS

            ///TODO EL RESTO ES UN CURSORRRR
            if (iscursor) {
                ///es un cursor!!!
                TuioCursor* pCursor = NULL;
                float mindist = 1000.0;

                pCursor = m_pTUIOSystemData->getClosestTuioCursor(feat_v.X(), feat_v.Y());
                if (pCursor) {
                    mindist = pCursor->getDistance( feat_v.X(), feat_v.Y() );
                    if ( mindist < 0.3 ) {
                        //m_pTUIOSystemData->initFrame(TuioTime::getSessionTime());
                        m_pTUIOSystemData->updateTuioCursor( pCursor, feat_v.X(), feat_v.Y() );
                    }
                }
                if ( mindist > 0.3 ) {
                    //m_pTUIOSystemData->initFrame(TuioTime::getSessionTime());
                    pCursor = m_pTUIOSystemData->addTuioCursor( feat_v.X(), feat_v.Y() );
                }
            }

        }///fin asignacion TF feature
    }

    ///marca como parados, aquellos cursores u objetos que no fueron tocados
    ///(o sea, todo aquel cuyo time de actualizacion no corresponde al frame actual, [no confirmo posicion en este frame!!!])

    //MODebug2->Push( moText("upla:") + IntToStr(nupla) );
    //m_pTUIOSystemData->initFrame(TuioTime::getSessionTime());
    m_pTUIOSystemData->stopUntouchedMovingCursors();
    //m_pTUIOSystemData->initFrame(TuioTime::getSessionTime());
    m_pTUIOSystemData->stopUntouchedMovingObjects();


    ///Eliminamos aquellos objetos parados y no tocados... definitivamente!!!
    //m_pTUIOSystemData->initFrame(TuioTime::getSessionTime());
    m_pTUIOSystemData->removeUntouchedStoppedCursors();
    //m_pTUIOSystemData->initFrame(TuioTime::getSessionTime());
    m_pTUIOSystemData->removeUntouchedStoppedObjects();

/*
    MODebug2->Push( moText(" frametime :") + IntToStr(m_pTUIOSystemData->getFrameTime().getSeconds())
                + moText(" frame id :") + IntToStr(m_pTUIOSystemData->getFrameID() )
                 );


    MODebug2->Push( moText("untouched cursors size")
                    + IntToStr(m_pTUIOSystemData->getUntouchedCursors().size())
                    + moText("cursors size")
                    + IntToStr(m_pTUIOSystemData->getTuioCursors().size()));
*/
/*
    MODebug2->Push( moText("objs: ")
                    + IntToStr( m_pTUIOSystemData->getTuioObjects().size() )
                    + moText("uobjs: ")
                    + IntToStr( m_pTUIOSystemData->getUntouchedObjects().size() )

                    + moText("curs: ")
                    + IntToStr( m_pTUIOSystemData->getTuioCursors().size() )
                    + moText("ucurs: ")
                    + IntToStr( m_pTUIOSystemData->getUntouchedCursors().size() )
                    );*/
//    MODebug2->Push(moText("objects size") + IntToStr(m_pTUIOSystemData->getTuioObjects().size()));

}///FIN DE TUIO!!!!


/*
    moVector2f previous_B = m_pTrackerSystemData->GetBarycenter();
    moVector2f previous_BM = m_pTrackerSystemData->GetBarycenterMotion();

    moVector2f BarPos;
    moVector2f BarMot;
    moVector2f BarAcc;

    BarPos = moVector2f( sumX/sumN, sumY/sumN);

    BarMot = BarPos - previous_B;
    BarAcc = BarMot - previous_BM;

    m_pTrackerSystemData->SetBarycenter( BarPos.X(), BarPos.Y() );
    m_pTrackerSystemData->SetBarycenterMotion( BarMot.X(), BarMot.Y() );
    m_pTrackerSystemData->SetBarycenterAcceleration( BarAcc.X(), BarAcc.Y() );

    m_pTrackerSystemData->SetMax( maxX, maxY );
    m_pTrackerSystemData->SetMin( minX, minY );
    m_pTrackerSystemData->SetValidFeatures( (int)sumN );

    ///CALCULATE VARIANCE FOR EACH COMPONENT

    moVector2f Bar = m_pTrackerSystemData->GetBarycenter();
    for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
        TF = m_pTrackerSystemData->GetFeatures().Get(i);
        if (TF) {
            if (TF->val>=0) {
                varX+= moMathf::Sqr( TF->x - Bar.X() );
                varY+= moMathf::Sqr( TF->y - Bar.Y() );
            }
        }
    }
    m_pTrackerSystemData->SetVariance( varX/sumN, varY/sumN );


    ///CALCULATE CIRCULAR MATRIX
    for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
        TF = m_pTrackerSystemData->GetFeatures().Get(i);
        if (TF) {
            if (TF->val>=0) {
                m_pTrackerSystemData->SetPositionMatrixC( TF->x, TF->y, 1 );
                vel = moVector2f( TF->v_x, TF->v_y ).Length();
                //acc = moVector2f( TF->a_x, TF->a_y ).Length();
                if (vel>=0.01) m_pTrackerSystemData->SetMotionMatrixC( TF->x, TF->y, 1 );
            }
        }
    }
    */

}

void moTrackerKLTSystem::CopyBufferToImg(GLubyte *p_pBuffer, unsigned char *p_img, MOuint p_RGB_mode)
{
	int i, pos, byteSize;
	float r, g, b;
	unsigned char l;


    if (p_RGB_mode == GL_RGBA) byteSize = 4;
	else if (p_RGB_mode == GL_RGB) byteSize = 3;
	else return; // Unsupported RGB mode.

    pos = 0;
	for (i = 0; i < m_nCols * m_nRows; i++)
    {
		r = (float)*(p_pBuffer + pos + 0);
		g = (float)*(p_pBuffer + pos + 1);
		b = (float)*(p_pBuffer + pos + 2);
		l = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
        *(p_img + i) = l;
        pos += byteSize;
    }

    //p_img = (unsigned char *)p_pBuffer;
}

void moTrackerKLTSystem::CalcImgDiff0(unsigned char *m_img_new, unsigned char *m_img_ref)
{
    for (int i = 0; i < m_nCols * m_nRows; i++)
        *(m_img_diff0 + i) = (unsigned char)fabs(float(*(m_img_new + i) - *(m_img_ref + i)));
}

void moTrackerKLTSystem::CalcImgDiff(unsigned char *m_img_new, unsigned char *m_img_ref)
{
    for (int i = 0; i < m_nCols * m_nRows; i++)
        *(m_img_diff + i) = (unsigned char)fabs(float(*(m_img_new + i) - *(m_img_ref + i)));
}

void moTrackerKLTSystem::CopyImg2To1()
{
    memcpy(m_img2, m_img1, m_nCols * m_nRows);
}

void moTrackerKLTSystem::AddBufferToImg(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
	int i, pos, byteSize;
	float r, g, b, l;

    if (p_RGB_mode == GL_RGBA) byteSize = 4;
	else if (p_RGB_mode == GL_RGB) byteSize = 3;
	else return; // Unsupported RGB mode.

    pos = 0;
	for (i = 0; i < m_nCols * m_nRows; i++)
    {
		r = (float)*(p_pBuffer + pos + 0);
		g = (float)*(p_pBuffer + pos + 1);
		b = (float)*(p_pBuffer + pos + 2);
		l = 0.299 * r + 0.587 * g + 0.114 * b;

        *(m_img_sum + i) += l;
        pos += byteSize;
    }
}

void moTrackerKLTSystem::CalcImg1Average()
{
    for (int i = 0; i < m_nCols * m_nRows; i++)
        *(m_img1 + i) = (unsigned char) *(m_img_sum + i) / m_num_samples;
}

//===========================================
//
//     Class: moTrackerKLT
//
//===========================================

moTrackerKLT::moTrackerKLT()
{
	SetName( moText("trackerklt") );
}

moTrackerKLT::~moTrackerKLT()
{
    Finish();
}

MOboolean moTrackerKLT::Init()
{

    moText configname;
	MOint nvalues;
	MOint trackersystems;

	configname = m_pResourceManager->GetDataMan()->GetDataPath();
	configname +=  moSlash + (moText)GetConfigName();
    configname +=  moText(".cfg");

	if (m_Config.LoadConfig(configname) != MO_CONFIG_OK ) {
		moText text = moText("Couldn't load trackerklt config:") + (moText)configname;
		MODebug2->Error( text );
		return false;
	}

	moMoldeoObject::Init();

    moDefineParamIndex( TRACKERKLT_NUM_FEAT, moText("num_feat") );
    moDefineParamIndex( TRACKERKLT_REPLACE_LOST_FEAT,moText("replace_lost_feat") );
    moDefineParamIndex( TRACKERKLT_DIFF_MODE,moText("diff_mode") );
    moDefineParamIndex( TRACKERKLT_NUM_SAMPLES,moText("num_samples") );

    moDefineParamIndex( TRACKERKLT_MIN_DIST,moText("min_dist") );
    moDefineParamIndex( TRACKERKLT_MIN_EIGEN,moText("min_eigen") );

    moDefineParamIndex( TRACKERKLT_LIGHT_SENS,moText("light_sens") );
    moDefineParamIndex( TRACKERKLT_NUM_FRAMES,moText("num_frames") );
    moDefineParamIndex( TRACKERKLT_SAMPLE_RATE,moText("sample_rate") );

    moDefineParamIndex( TRACKERKLT_BORDERX,moText("borderx") );
    moDefineParamIndex( TRACKERKLT_BORDERY,moText("bordery") );
    moDefineParamIndex( TRACKERKLT_NPYRAMIDLEVELS,moText("npyramidlevels") );
    moDefineParamIndex( TRACKERKLT_SUBSAMPLING,moText("subsampling") );
    moDefineParamIndex( TRACKERKLT_MAX_ITERATIONS,moText("max_iterations") );

    moDefineParamIndex( TRACKERKLT_WINDOW_WIDTH,moText("window_width") );
    moDefineParamIndex( TRACKERKLT_WINDOW_HEIGHT,moText("window_height") );

    moDefineParamIndex( TRACKERKLT_MIN_DISPLACEMENT,moText("min_displacement") );
    moDefineParamIndex( TRACKERKLT_MIN_DETERMINANT,moText("min_determinant") );
    moDefineParamIndex( TRACKERKLT_MAX_RESIDUE,moText("max_residue") );

    moDefineParamIndex( TRACKERKLT_MIN_SEGMENT_LEN,moText("min_segment_len") );
    moDefineParamIndex( TRACKERKLT_MAX_SEGMENT_LEN,moText("max_segment_len") );


    m_SampleCounter = 0;
	trackersystems = m_Config.GetParamIndex("trackersystems");
    MODebug2->Message("In moTrackerKLT::Init ***********************************************\n");
    MODebug2->Message("Initializing Tracker's System...\n");
	//por cada camara inicializa un system:
	nvalues = m_Config.GetValuesCount( trackersystems );
	m_Config.SetCurrentParamIndex(trackersystems);


/*
    icoordA = m_Config.GetParamIndex("coordA");
    coordA = moVector2f(m_Config.GetParam(icoordA).GetValue().GetSubValue(0).Float(),
                        m_Config.GetParam(icoordA).GetValue().GetSubValue(1).Float() );

    icoordB = m_Config.GetParamIndex("coordB");
    coordB = moVector2f(m_Config.GetParam(icoordB).GetValue().GetSubValue(0).Float(),
                        m_Config.GetParam(icoordB).GetValue().GetSubValue(1).Float() );

    icoordC = m_Config.GetParamIndex("coordC");
    coordC = moVector2f(m_Config.GetParam(icoordC).GetValue().GetSubValue(0).Float(),
                        m_Config.GetParam(icoordC).GetValue().GetSubValue(1).Float() );

    icoordD = m_Config.GetParamIndex("coordD");
    coordD = moVector2f(m_Config.GetParam(icoordD).GetValue().GetSubValue(0).Float(),
                        m_Config.GetParam(icoordD).GetValue().GetSubValue(1).Float() );
*/
	/*
MO_TRACKER1D_SYTEM_LABELNAME	0
MO_TRACKER1D_LIVE_SYSTEM	1
MO_TRACKER1D_SYSTEM_ON 2
	*/
	for( int i = 0; i < nvalues; i++) {

		m_Config.SetCurrentValueIndex( trackersystems, i );

		moTrackerKLTSystemPtr pTSystem = NULL;

		pTSystem = new moTrackerKLTSystem();

		if (pTSystem!=NULL) {
			pTSystem->MODebug = MODebug;
			pTSystem->SetName( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKERKLT_SYTEM_LABELNAME).Text() );
			pTSystem->SetLive( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKERKLT_LIVE_SYSTEM).Text() );
			pTSystem->SetActive( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKERKLT_SYSTEM_ON).Int() );
		}

		m_TrackerSystems.Add( pTSystem );

	}

	m_OutletTuioSystemIndex = GetOutletIndex( "TUIOSYSTEM0" );

	return true;
}

void moTrackerKLT::UpdateParameters() {
    num_feat = m_Config.GetParam(m_Config.GetParamIndex("num_feat")).GetValue().GetSubValue(0).Int();
	replace_lost_feat = m_Config.GetParam(m_Config.GetParamIndex("replace_lost_feat")).GetValue().GetSubValue(0).Int();
	diff_mode = m_Config.GetParam(m_Config.GetParamIndex("diff_mode")).GetValue().GetSubValue(0).Int();
	num_samples = m_Config.GetParam(m_Config.GetParamIndex("num_samples")).GetValue().GetSubValue(0).Int();
	min_dist = m_Config.GetParam(m_Config.GetParamIndex("min_dist")).GetValue().GetSubValue(0).Float();
	min_eigen = m_Config.GetParam(m_Config.GetParamIndex("min_eigen")).GetValue().GetSubValue(0).Float();
	light_sens = m_Config.GetParam(m_Config.GetParamIndex("light_sens")).GetValue().GetSubValue(0).Int();
	num_frames = m_Config.GetParam(m_Config.GetParamIndex("num_frames")).GetValue().GetSubValue(0).Int();
	m_SampleRate = m_Config.GetParam(m_Config.GetParamIndex("sample_rate")).GetValue().GetSubValue().Int();

	borderx = m_Config.GetParam(m_Config.GetParamIndex("borderx")).GetValue().GetSubValue().Int();
	bordery = m_Config.GetParam(m_Config.GetParamIndex("bordery")).GetValue().GetSubValue().Int();

	pyramid_levels = m_Config.GetParam(m_Config.GetParamIndex("pyramid_levels")).GetValue().GetSubValue().Int();
	subsampling = m_Config.GetParam(m_Config.GetParamIndex("subsampling")).GetValue().GetSubValue().Int();
	max_iterations = m_Config.GetParam(m_Config.GetParamIndex("max_iterations")).GetValue().GetSubValue().Int();

	window_width = m_Config.GetParam(m_Config.GetParamIndex("window_width")).GetValue().GetSubValue().Int();
	window_height = m_Config.GetParam(m_Config.GetParamIndex("window_height")).GetValue().GetSubValue().Int();

    min_displacement = m_Config.GetParam(m_Config.GetParamIndex("min_displacement")).GetValue().GetSubValue().Float();
    min_determinant = m_Config.GetParam(m_Config.GetParamIndex("min_determinant")).GetValue().GetSubValue().Float();
    max_residue = m_Config.GetParam(m_Config.GetParamIndex("max_residue")).GetValue().GetSubValue().Float();

    min_segment_len = m_Config.GetParam(m_Config.GetParamIndex("min_segment_len")).GetValue().GetSubValue().Float();
    max_segment_len = m_Config.GetParam(m_Config.GetParamIndex("max_segment_len")).GetValue().GetSubValue().Float();

}

moConfigDefinition* moTrackerKLT::GetDefinition(moConfigDefinition *p_configdefinition) {

    p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
    p_configdefinition->Add( moText("num_feat"), MO_PARAM_NUMERIC, TRACKERKLT_NUM_FEAT, moValue( "100", "INT").Ref() );
    p_configdefinition->Add( moText("replace_lost_feat"), MO_PARAM_NUMERIC, TRACKERKLT_REPLACE_LOST_FEAT, moValue( "1", "INT").Ref() );
    p_configdefinition->Add( moText("diff_mode"), MO_PARAM_NUMERIC, TRACKERKLT_DIFF_MODE, moValue( "0", "INT").Ref() );
    p_configdefinition->Add( moText("num_samples"), MO_PARAM_NUMERIC, TRACKERKLT_NUM_SAMPLES, moValue( "1", "INT").Ref() );

    p_configdefinition->Add( moText("min_dist"), MO_PARAM_NUMERIC, TRACKERKLT_MIN_DIST, moValue( "10.0", "FLOAT").Ref() );
    p_configdefinition->Add( moText("min_eigen"), MO_PARAM_NUMERIC, TRACKERKLT_MIN_EIGEN, moValue( "100.0", "FLOAT").Ref() );

    p_configdefinition->Add( moText("light_sens"), MO_PARAM_NUMERIC, TRACKERKLT_LIGHT_SENS, moValue( "1", "INT").Ref() );
    p_configdefinition->Add( moText("num_frames"), MO_PARAM_NUMERIC, TRACKERKLT_NUM_FRAMES, moValue( "50", "INT").Ref() );
    p_configdefinition->Add( moText("sample_rate"), MO_PARAM_NUMERIC, TRACKERKLT_SAMPLE_RATE, moValue( "1", "INT").Ref() );

    p_configdefinition->Add( moText("borderx"), MO_PARAM_NUMERIC, TRACKERKLT_BORDERX, moValue( "2", "INT").Ref() );
    p_configdefinition->Add( moText("bordery"), MO_PARAM_NUMERIC, TRACKERKLT_BORDERY, moValue( "2", "INT").Ref() );
    p_configdefinition->Add( moText("nPyramidLevels"), MO_PARAM_NUMERIC, TRACKERKLT_NPYRAMIDLEVELS, moValue( "2", "INT").Ref() );
    p_configdefinition->Add( moText("subsampling"), MO_PARAM_NUMERIC, TRACKERKLT_SUBSAMPLING, moValue( "4", "INT").Ref() );
    p_configdefinition->Add( moText("max_iterations"), MO_PARAM_NUMERIC, TRACKERKLT_MAX_ITERATIONS, moValue( "10", "INT").Ref() );

    p_configdefinition->Add( moText("window_width"), MO_PARAM_NUMERIC, TRACKERKLT_WINDOW_WIDTH, moValue( "4", "INT").Ref() );
    p_configdefinition->Add( moText("window_height"), MO_PARAM_NUMERIC, TRACKERKLT_WINDOW_HEIGHT, moValue( "4", "INT").Ref() );

    p_configdefinition->Add( moText("min_displacement"), MO_PARAM_NUMERIC, TRACKERKLT_MIN_DISPLACEMENT, moValue( "0.10000001", "FLOAT").Ref() );
    p_configdefinition->Add( moText("min_determinant"), MO_PARAM_NUMERIC, TRACKERKLT_MIN_DETERMINANT, moValue( "0.0099999978", "FLOAT").Ref() );
    p_configdefinition->Add( moText("max_residue"), MO_PARAM_NUMERIC, TRACKERKLT_MAX_RESIDUE, moValue( "10.0", "FLOAT").Ref() );

    p_configdefinition->Add( moText("min_segment_len"), MO_PARAM_NUMERIC, TRACKERKLT_MIN_SEGMENT_LEN, moValue( "0.0", "FLOAT").Ref() );
    p_configdefinition->Add( moText("max_segment_len"), MO_PARAM_NUMERIC, TRACKERKLT_MAX_SEGMENT_LEN, moValue( "0.4", "FLOAT").Ref() );

}

MOboolean moTrackerKLT::Finish()
{
	moTrackerKLTSystemPtr pTSystem = NULL;
	for( MOuint i = 0; i < m_TrackerSystems.Count(); i++) {
		pTSystem = m_TrackerSystems.Get(i);
		if (pTSystem!=NULL) {
			pTSystem->Finish();
			delete pTSystem;
		}
	}
	m_TrackerSystems.Empty();
	return true;
}

MOswitch moTrackerKLT::SetStatus(MOdevcode devcode, MOswitch state)
{
    return true;
}

void moTrackerKLT::SetValue( MOdevcode cd, MOfloat vl ) {}

void moTrackerKLT::SetValue( MOdevcode cd, MOint vl ) {}

MOswitch moTrackerKLT::GetStatus(MOdevcode devcode)
{
	if ( 0 <= devcode && devcode < (int)m_TrackerSystems.Count() ) {
		return m_TrackerSystems.Get(devcode)->IsInit();
	} else if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return m_TrackerSystems.Get(devcode - m_TrackerSystems.Count())->IsInit();
	}
	return 0;
}

MOint moTrackerKLT::GetValue(MOdevcode devcode)
{
	moTrackerKLTSystemPtr pTS = NULL;

	if ( 0 <= devcode && devcode < m_TrackerSystems.Count() ) {
		return m_TrackerSystems.Get(devcode)->GetData()->GetFeaturesCount();
	} else if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return m_TrackerSystems.Get( devcode - m_TrackerSystems.Count() )->GetData()->GetVideoFormat().m_Width;
	}

    return(-1);
}

MOpointer moTrackerKLT::GetPointer(MOdevcode devcode ) {

	if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return (MOpointer)( m_TrackerSystems.Get(devcode-m_TrackerSystems.Count())->GetData() );
	}
	return (NULL);
}

MOdevcode moTrackerKLT::GetCode(moText strcod)
{
	moText TrackerSystemName;
	moText TrackerX;
	moText TrackerData;

	//example:
	//TRACKERKLT0_N		-> returns 0
	//TRACKERKLT1_N		-> returns 1
	//TRACKERKLT0_DATA		-> returns 2
	//TRACKERKLT1_DATA		-> returns 3

	for( int i = 0; i< m_TrackerSystems.Count(); i++) {

		TrackerSystemName = m_TrackerSystems.Get(i)->GetName();
		TrackerX = TrackerSystemName+moText("_N");
		TrackerData = TrackerSystemName+moText("_DATA");

		if ( !stricmp( strcod, TrackerX ) ) {

			return i;

		} else if (!stricmp( strcod, TrackerData )) {

			return ( i + m_TrackerSystems.Count() );

		}

	}

    return(-1);
}

void moTrackerKLT::Update(moEventList *Events)
{
	//get the pointer from the Moldeo Object sending it...

	moBucket* pBucket = NULL;
	moVideoSample* pSample = NULL;

    UpdateParameters();

//	MOuint i;
	moEvent *actual,*tmp;

	actual = Events->First;

	if (m_SampleCounter>=200000) m_SampleCounter = 0;
	m_SampleCounter++;


	while(actual!=NULL) {
		tmp = actual->next;
		if (actual->deviceid == MO_IODEVICE_TRACKER && actual->reservedvalue3!=MO_MESSAGE) {
			Events->Delete(actual);
		}
		actual = tmp;
	}

	actual = Events->First;
	//Buscamos los eventos del VideoManager...samples... obsoleto, seran datos de Outlets e Inlets...
	//Outlet de videomanager LIVEOUT0 a inlet de tracker
	while(actual!=NULL) {
		//solo nos interesan los del VideoManager por ahora
		if(actual->deviceid == m_pResourceManager->GetVideoMan()->GetId() ) {

			pSample = (moVideoSample*)actual->pointer;

			pBucket = (moBucket*)pSample->m_pSampleBuffer;

			//atencion! el devicecode corresponde al  iesimo dispositivo de captura...
			if ( actual->devicecode < m_TrackerSystems.Count() && pSample && pBucket && ( (m_SampleCounter % m_SampleRate)==0)) {

				moTrackerKLTSystemPtr pTS=NULL;

				//MODebug2->Push( moText("Counter:") + IntToStr(m_SampleCounter) );
                ///el actual->devicecode, es un indice en este caso basado en 0, donde enviar
                ///la camara q esta siendo tomada, a su vez tomada en 0 (el VideoSample)
				pTS = m_TrackerSystems.Get( actual->devicecode );
				if ( pTS )
					if (pTS->IsActive() ) {

					    ///create new pSample here!!
					    ///based on 4 vertices

					    ///coordTextA 0..1,0..1
					    ///coordTextB 0..1,0..1
					    ///coordTextC 0..1,0..1
					    ///coordTextD 0..1,0..1

					    ///create a framebuffer
					    ///assignate texture to it (our output texture)
					    ///draw the polygon with these coord textures on the framebuffer
					    ///using the LIVEIN0 as binding texture
					    ///then get the buffer from the moTexture
					    ///onto our new sample....
					    ///...resize...

					    ///Create our texture output
					    ///tracker_input_id = m_pTextureManager->AddTexture("tracker_input", m_render_width, m_render_height);
					    ///moTexture* TInput = m_pTextureManager->GetTexture(tracker_input_id);

					    ///Create a framebuffer
					    ///MOuint attach_point;
					    /// m_fbo_idx = m_pFBManager->CreateFBO();
					    /// moFBO* pFBO = m_pFBManager->GetFBO(m_fbo_idx);


					    ///Attach our texture to this FrameBuffer
					    /** pFBO->AddTexture(   m_render_width,
                                                m_render_height,
                                                TInput->GetTexParam(),
                                                TInput->GetGLId(),
                                                attach_point );*/

                        /// TInput->SetFBO( pFBO );
                        /// TInput->SetFBOAttachPoint( attach_point );
                        ///

                        ///now we draw:

                        /// m_pFBManager->BindFBO( m_fbo_idx, attach_point );
                        /// glBegin(GL_POLYGON)
                            /// aqui van las coords de la pantalla( 0,1,0,1)
                            /// con sus respectivas coord de texturas (coordA,B,C,D)
                        /// glEnd()

						if (!pTS->IsInit()) {
							pTS->Init(  num_feat,
                                        pSample,
                                        replace_lost_feat, true, diff_mode,
                                        num_frames, num_samples, min_dist,
                                        min_eigen,
                                        !light_sens);


						}


                        KLT_TrackingContext TC = pTS->GetTrackingContext();

                        TC->min_eigenvalue = min_eigen;
                        TC->mindist = min_dist;
                        //TC->lighting_insensitive = light_sens;
                        //TC->sequentialMode = (int)diff_mode;
/*
                        TC->borderx = borderx;//24
                        TC->bordery = bordery;//24
                        TC->nPyramidLevels = pyramid_levels;//2
                        TC->subsampling = subsampling;//4
                        TC->max_iterations = max_iterations;

                        TC->window_width = window_width;//7
                        TC->window_height = window_height;//7
                        TC->min_displacement = min_displacement;//0.10000001
                        TC->min_determinant = min_determinant;//0.0099999978
                        TC->max_residue = max_residue;
                        */

                        pTS->min_segment_len = min_segment_len;
                        pTS->max_segment_len = max_segment_len;

						pTS->NewData( pSample );

/*
						//calcular pesos y otras yerbas
						moTrackerKLTSystemData* pTData;
						pTData = pTS->GetData();

						MOint i;

						MOint NF, TrackersX, TrackersY, Variance;

						MOfloat sumX,sumY;
						MOfloat meanX,meanY,disV;

						NF = 0;
						sumX = 0; meanX = 0;
						sumY = 0; meanY = 0;
						TrackersX = 0;
						TrackersY = 0;
						disV = 0;
						Variance = 0;

						for( i=0; i<pTData->m_NFeatures; i++) {
							if (pTData->m_FeatureList->feature[i]->val>=0) {
								sumX+= pTData->m_FeatureList->feature[i]->x;
								sumY+= pTData->m_FeatureList->feature[i]->y;
								NF++;
							}
						}

						if (NF>0) {
							meanX = sumX / NF;
							meanY = sumY / NF;
							TrackersX = (int) meanX;
							TrackersY = (int) meanY;

							for( i=0; i<pTData->m_NFeatures; i++) {
								if (pTData->m_FeatureList->feature[i]->val>=0) {
									disV+= (meanX - pTData->m_FeatureList->feature[i]->x)*(meanX - pTData->m_FeatureList->feature[i]->x)
										+ (meanY - pTData->m_FeatureList->feature[i]->y)*(meanY - pTData->m_FeatureList->feature[i]->y);
								}
							}
							Variance = disV / NF;
						}

						TrackersX = (TrackersX*320)/pSample->m_VideoFormat.m_Width;
						TrackersY = (TrackersY*240)/pSample->m_VideoFormat.m_Height;

						Events->Add( MO_IODEVICE_TRACKER, NF, TrackersX, TrackersY, Variance, 0 );
*/
                        ///aqui el Outlet que se toma es el mismo en el orden del indice, lo cual no es muy elegante
                        ///el indice del outlet deberiamos preguntarlo antes
						m_Outlets[actual->devicecode]->GetData()->SetPointer( (MOpointer)pTS->GetData(), sizeof(moTrackerSystemData) );
						m_Outlets[actual->devicecode]->Update();

						///ahora agregamos ademas un dato adicional un Outlet de TUIO
						///en cambio este lo buscamos anteriormente!!! por el nombre "TUIO"
						if (m_OutletTuioSystemIndex>-1) {

						    moOutlet* pOutletTUIO = m_Outlets[ m_OutletTuioSystemIndex ];

                            pOutletTUIO->GetData()->SetPointer( (MOpointer)pTS->GetTUIOData(), sizeof(moTUIOSystemData) );
                            pOutletTUIO->Update();
						}

					}
			}
			tmp = actual->next;
			actual = tmp;
		} else actual = actual->next;//no es el que necesitamos...
	}


	moMoldeoObject::Update(Events);

}

void moTrackerKLT::DrawTrackerFeatures( int isystem )
{
	int i, tw, th, v;
    float x, y;
    int w,h;

    w = m_pResourceManager->GetRenderMan()->RenderWidth();
    h = m_pResourceManager->GetRenderMan()->RenderHeight();

	moTrackerSystemData*	m_pTrackerData;

	moTrackerKLTSystemPtr pTS=NULL;
	pTS = m_TrackerSystems.Get( isystem );
	if ( pTS )
		if (pTS->IsActive() ) {

			m_pTrackerData = pTS->GetData();

			tw = m_pTrackerData->GetVideoFormat().m_Width;
			th = m_pTrackerData->GetVideoFormat().m_Height;

			for (i = 0; i < m_pTrackerData->GetFeaturesCount(); i++)
			{
				x = w * m_pTrackerData->GetFeature(i)->x / tw;
				y = h * m_pTrackerData->GetFeature(i)->y / th;
				v = m_pTrackerData->GetFeature(i)->val;

				if (v == KLT_TRACKED) glColor4f(0.5, 1.0, 0.5, 1.0);
				else if (v == KLT_NOT_FOUND) glColor4f(0.0, 0.0, 0.0, 1.0);
				else if (v == KLT_SMALL_DET) glColor4f(1.0, 0.0, 0.0, 1.0);
				else if (v == KLT_MAX_ITERATIONS) glColor4f(0.0, 1.0, 0.0, 1.0);
				else if (v == KLT_OOB) glColor4f(0.0, 0.0, 1.0, 1.0);
				else if (v == KLT_LARGE_RESIDUE) glColor4f(1.0, 1.0, 0.0, 1.0);
				glBegin(GL_QUADS);
					glVertex2f(x - 5, y - 5);
					glVertex2f(x - 5, y + 5);
					glVertex2f(x + 5, y + 5);
					glVertex2f(x + 5, y - 5);
				glEnd();
			}
		}

}
