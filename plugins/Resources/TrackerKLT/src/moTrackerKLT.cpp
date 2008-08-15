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

MOboolean moTrackerKLTSystem::Init(MOint p_nFeatures, MOint p_width, MOint p_height,
		      MOboolean p_replaceLostFeatures, MOboolean p_sequentialMode, MOboolean p_diffMode,
			  MOint p_nFrames, MOint p_num_samples, MOint p_mindist, MOfloat p_mineigen, MOboolean p_lighting_insensitive)
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

	m_fl = KLTCreateFeatureList(p_nFeatures);
	m_nFrames = p_nFrames;
	if (0 < p_nFrames) m_ft = KLTCreateFeatureTable(p_nFrames, p_nFeatures);

    m_diffMode = p_diffMode && m_tc->sequentialMode;
	m_replaceLostFeatures = p_replaceLostFeatures;

	m_init = true;

	m_num_samples = p_num_samples;

	return true;
}

MOboolean moTrackerKLTSystem::Init(MOint p_nFeatures, moVideoSample* p_pVideoSample,
		      MOboolean p_replaceLostFeatures, MOboolean p_sequentialMode, MOboolean p_diffMode,
			  MOint p_nFrames, MOint p_num_samples, MOint p_mindist, MOfloat p_mineigen, MOboolean p_lighting_insensitive)
{
	if ( p_pVideoSample != NULL )
		m_TrackerSystemData.m_VideoFormat = p_pVideoSample->m_VideoFormat;
	else return false;

	if ( m_TrackerSystemData.m_VideoFormat.m_Width<=0 || m_TrackerSystemData.m_VideoFormat.m_Height==0)
		return false;

	MOboolean res = Init(p_nFeatures, m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height,
							p_replaceLostFeatures, p_sequentialMode, p_diffMode,
							p_nFrames, p_num_samples, p_mindist, p_mineigen, p_lighting_insensitive);

	if (res)
	{
		m_TrackerSystemData.m_NFeatures = p_nFeatures;
		m_TrackerSystemData.m_FeatureList = m_fl;
		m_TrackerSystemData.m_FeatureTable = m_ft;
		return true;
	}
	else return false;
}

MOboolean moTrackerKLTSystem::Finish()
{
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
	if (m_started) ContinueTracking(p_pBuffer, p_RGB_mode);
	else StartTracking(p_pBuffer, p_RGB_mode);
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
			KLTTrackFeatures(m_tc, m_img1, m_img2, m_nCols, m_nRows, m_fl);
			if (m_replaceLostFeatures) KLTReplaceLostFeatures(m_tc, m_img2, m_nCols, m_nRows, m_fl);
		}

		if (0 < m_nFrames && (m_nCurrentFrame < m_nFrames))
		{
			KLTStoreFeatureList(m_fl, m_ft, m_nCurrentFrame);
			m_nCurrentFrame++;
		}

		if (!m_tc->sequentialMode) CopyImg2To1();
	}

	m_just_started = false;
}

void moTrackerKLTSystem::NewData( moVideoSample* p_pVideoSample )
{
	if (p_pVideoSample)
	{
		moBucket* pBucket = (moBucket*)p_pVideoSample->m_pSampleBuffer;
		if (pBucket)
		{
			m_sizebuffer = pBucket->GetSize();
			m_buffer = pBucket->GetBuffer();
		}
		else return;
	}
	else return;

	if (m_buffer == NULL) return;

	Track(m_buffer, GL_RGB);

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
	configname +=  moText("/") + (moText)GetConfigName();
    configname +=  moText(".cfg");

	if (m_Config.LoadConfig(configname) != MO_CONFIG_OK ) {
		moText text = "Couldn't load trackerklt config:" + configname;
		MODebug2->Error( text );
		return false;
	}

	moMoldeoObject::Init();

	trackersystems = m_Config.GetParamIndex("trackersystems");

    MODebug2->Message("In moTrackerKLT::Init ***********************************************\n");

    MODebug2->Message("Initializing Tracker's System...\n");

	//por cada camara inicializa un system:
	nvalues = m_Config.GetValuesCount( trackersystems );
	m_Config.SetCurrentParamIndex(trackersystems);

	num_feat = m_Config.GetParam(m_Config.GetParamIndex("num_feat")).GetValue().GetSubValue(0).Int();
	replace_lost_feat = m_Config.GetParam(m_Config.GetParamIndex("replace_lost_feat")).GetValue().GetSubValue(0).Int();
	diff_mode = m_Config.GetParam(m_Config.GetParamIndex("diff_mode")).GetValue().GetSubValue(0).Int();
	num_samples = m_Config.GetParam(m_Config.GetParamIndex("num_samples")).GetValue().GetSubValue(0).Int();
	min_dist = m_Config.GetParam(m_Config.GetParamIndex("min_dist")).GetValue().GetSubValue(0).Float();
	min_eigen = m_Config.GetParam(m_Config.GetParamIndex("min_eigen")).GetValue().GetSubValue(0).Float();
	light_sens = m_Config.GetParam(m_Config.GetParamIndex("light_sens")).GetValue().GetSubValue(0).Int();
	num_frames = m_Config.GetParam(m_Config.GetParamIndex("num_frames")).GetValue().GetSubValue(0).Int();
	m_SampleRate = m_Config.GetParam(m_Config.GetParamIndex("sample_rate")).GetValue().GetSubValue().Int();


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
	return true;
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
		return m_TrackerSystems.Get(devcode)->GetData()->m_NFeatures;
	} else if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return m_TrackerSystems.Get( devcode - m_TrackerSystems.Count() )->GetData()->m_VideoFormat.m_Width;
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
		TrackerX = TrackerSystemName+"_N";
		TrackerData = TrackerSystemName+"_DATA";

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
				pTS = m_TrackerSystems.Get( actual->devicecode );
				if ( pTS )
					if (pTS->IsActive() ) {
						if (!pTS->IsInit()) {
							pTS->Init(num_feat, pSample,
								      replace_lost_feat, true, diff_mode,
									  num_frames, num_samples, min_dist, min_eigen, !light_sens);
						}
						pTS->NewData( pSample );

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

						m_Outlets[actual->devicecode]->GetData()->SetPointer( (MOpointer)pTS->GetData(), sizeof(moTrackerKLTSystemData) );
						m_Outlets[actual->devicecode]->Update();
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

	moTrackerKLTSystemData*	m_pTrackerData;

	moTrackerKLTSystemPtr pTS=NULL;
	pTS = m_TrackerSystems.Get( isystem );
	if ( pTS )
		if (pTS->IsActive() ) {

			m_pTrackerData = pTS->GetData();

			tw = m_pTrackerData->m_VideoFormat.m_Width;
			th = m_pTrackerData->m_VideoFormat.m_Height;

			for (i = 0; i < m_pTrackerData->m_NFeatures; i++)
			{
				x = 800.0 * m_pTrackerData->m_FeatureList->feature[i]->x / tw;
				y = 600.0 * m_pTrackerData->m_FeatureList->feature[i]->y / th;
				v = m_pTrackerData->m_FeatureList->feature[i]->val;

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
