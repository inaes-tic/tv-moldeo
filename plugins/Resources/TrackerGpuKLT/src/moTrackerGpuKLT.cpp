/*******************************************************************************

                             moTrackerGpuKLT.cpp

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
  Based on the GPU_KLT code by Sudipta N. Sinha from the University of North Carolina
  at Chapell Hill:
  http://cs.unc.edu/~ssinha/Research/GPU_KLT/
  Port to libmoldeo:  Andrés Colubri

*******************************************************************************/

#include "moTrackerGpuKLT.h"


//========================
//  Factory
//========================

moTrackerGpuKLTFactory *m_TrackerGpuKLTFactory = NULL;

MO_PLG_API moResourceFactory* CreateResourceFactory(){
	if (m_TrackerGpuKLTFactory==NULL)
		m_TrackerGpuKLTFactory = new moTrackerGpuKLTFactory();
	return (moResourceFactory*) m_TrackerGpuKLTFactory;
}

MO_PLG_API void DestroyResourceFactory(){
	delete m_TrackerGpuKLTFactory;
	m_TrackerGpuKLTFactory = NULL;
}

moResource*  moTrackerGpuKLTFactory::Create() {
	return new moTrackerGpuKLT();
}

void moTrackerGpuKLTFactory::Destroy(moResource* fx) {
	delete fx;
}

//===========================================
//
//     Class: moTrackerGpuKLTSystem
//
//===========================================

moTrackerGpuKLTSystem::moTrackerGpuKLTSystem()
{
	gpuComputor = NULL;
	m_init = false;
	list = NULL;
}

moTrackerGpuKLTSystem::~moTrackerGpuKLTSystem()
{
	Finish();
}

MOboolean moTrackerGpuKLTSystem::Init(MOint p_nFeatures, MOint p_width, MOint p_height,
					moText p_shaders_dir, MOint p_arch,
					MOint p_kltmindist, MOfloat p_klteigenthreshold)
{
	if (p_width <= 0 || p_height == 0) return false;

	strcpy(gopt._CGdirectory, p_shaders_dir);

	gopt._enableTiming = false;
	gopt._verbose = false;

	m_width = p_width;
	m_height = p_height;

	m_ReinjectRate = 1;  // 1

	// Store info for so many video frames in GPU memory. num levels in pyramid.
	gopt._nFrames = 2; // 2
	gopt._nLevels = 4; // 4

	// Pixel range in which to search for features while tracking.
	gopt._klt_search_range = 100;// 25 RUEDA: 11

	gopt._kltwin            = 64;  // KLT Window Size 7 RUEDA:10
	gopt._kltborder         = 8; // Window Border Size to be avoided. 32 RUEDA:8
	gopt._kltnumfeats       = p_nFeatures; // Number of features.
	gopt._klteigenthreshold = p_klteigenthreshold; // Eigen Value threshold for Cornerness (pixel brightness is between 0 to 1).
	gopt._kltmindist        = p_kltmindist; // Minimum Distance between KLT features (in pixels).
	gopt._kltiterations     = 5;    // Number of track iterations within the same klt_pyramid level. 5
	gopt._kltSSDthresh      = 0.05; // KLT SSD Threshold (pixel brightness varies between 0 and 1). 0.05
	gopt._kltConvthresh     = 0.5;  // KLT Min distance convergence threshold (in pixels). 0.5

	if (p_arch == MO_GPU_NV) gopt._arch = GPU_NVIDIA;
	else if (p_arch == MO_GPU_NV) gopt._arch = GPU_ATI;

	gopt._enableDebug = false;

    gpuComputor = new GpuVis(gopt); // Create GpuVis computor object

	m_TrackCount = 0;
	m_init = true;

	return true;
}

MOboolean moTrackerGpuKLTSystem::Init(MOint p_nFeatures, moVideoSample* p_pVideoSample,
					moText p_shaders_dir, MOint p_arch,
					MOint p_kltmindist, MOfloat p_klteigenthreshold)
{
	if ( p_pVideoSample != NULL )
		m_TrackerSystemData.m_VideoFormat = p_pVideoSample->m_VideoFormat;
	else return false;

	if ( m_TrackerSystemData.m_VideoFormat.m_Width<=0 || m_TrackerSystemData.m_VideoFormat.m_Height==0)
		return false;

	MOboolean res = Init(p_nFeatures, m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height,
						p_shaders_dir, p_arch,
						p_kltmindist, p_klteigenthreshold);

	if (res)
	{
		m_TrackerSystemData.m_NFeatures = p_nFeatures;
		m_TrackerSystemData.m_FeatureList = list;
		return true;
	}
	else return false;
}

MOboolean moTrackerGpuKLTSystem::Finish()
{
	if (gpuComputor != NULL)
	{
		delete gpuComputor;
		gpuComputor = NULL;
	}
	return true;
}

void moTrackerGpuKLTSystem::GetFeature(MOint p_feature, MOfloat &x, MOfloat &y, MOboolean &v)
{
	if (p_feature < list->_nFeats)
	{
		if (list->_list[p_feature]->valid)
		{
			x = list->_list[p_feature]->x;
			y = list->_list[p_feature]->y;
			v = list->_list[p_feature]->valid;
		}
		else
		{
			x = y = 0;
			v = false;
		}
	}
}

void moTrackerGpuKLTSystem::Track(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
	if (0 < m_TrackCount)
		ContinueTracking(p_pBuffer, p_RGB_mode);
	else
		StartTracking(p_pBuffer, p_RGB_mode);
}

void moTrackerGpuKLTSystem::StartTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
	image.setPointer(p_pBuffer, m_width, m_height);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	gpuComputor->Init(image);
	gpuComputor->computePyramid(image);
	list = gpuComputor->selectGoodFeatures();
	m_TrackerSystemData.m_FeatureList = list;

	m_TrackCount = 1;
}

void moTrackerGpuKLTSystem::ContinueTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
	int addedCount;

	image.setPointer(p_pBuffer, m_width, m_height);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	for(int i=0; i<list->_nFeats; i++) {
		list->_list[i]->tr_x = list->_list[i]->x;
		list->_list[i]->tr_y = list->_list[i]->y;
	}

	gpuComputor->computePyramid(image);
	gpuComputor->trackFeatures(&list);

	if (!(m_TrackCount % m_ReinjectRate))
		addedCount = gpuComputor->reselectGoodFeatures(&list, 1, 0.9); // Re-select in full image.

	gpuComputor->uploadFeaturesToGPU(&list);
	m_TrackerSystemData.m_FeatureList = list;

	m_TrackCount++;
}

void moTrackerGpuKLTSystem::NewData( moVideoSample* p_pVideoSample )
{
	MOint m_sizebuffer;
	MOpointer m_buffer;

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

//===========================================
//
//     Class: moTrackerGpuKLT
//
//===========================================

moTrackerGpuKLT::moTrackerGpuKLT() : moResource()
{
	SetResourceType(MO_RESOURCETYPE_FILTER);
	SetName("trackergpuklt");
}

moTrackerGpuKLT::~moTrackerGpuKLT()
{
    Finish();
}

MOboolean moTrackerGpuKLT::Init()
{
    //MOuint port;
	//MOuint mode;
	//MOuint nvaluees;
    moText conf;
	MOint nvalues;
	MOint trackersystems;

	if (m_pResourceManager==NULL)
		return false;

    char vendor[80];
	strcpy(vendor, (const char*)glGetString(GL_VENDOR));

    if (strstr(vendor, "NVIDIA") != NULL) gpu_arch = MO_GPU_NV;
    else if (strstr(vendor, "ATI") != NULL) gpu_arch = MO_GPU_ATI;
    else if (strstr(vendor, "INTEL") != NULL) gpu_arch = MO_GPU_INTEL;
    else gpu_arch = MO_GPU_OTHER;

	//int i;

    // Loading config file.
	conf = m_pResourceManager->GetDataMan()->GetDataPath()+moText("/");
	conf += GetConfigName();
    conf += moText(".cfg");
	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		moText text = "Couldn't load trackergpuklt config:" + conf;
		MODebug2->Error(text);
		return false;
	}

	moMoldeoObject::Init();

	trackersystems = m_Config.GetParamIndex("trackersystems");

	num_feat = m_Config.GetParam(m_Config.GetParamIndex("num_feat")).GetValue().GetSubValue().Int();
	min_dist = m_Config.GetParam(m_Config.GetParamIndex("min_dist")).GetValue().GetSubValue().Float();
	threshold_eigen = m_Config.GetParam(m_Config.GetParamIndex("threshold_eigen")).GetValue().GetSubValue().Float();;
	klt_shaders_dir = m_Config.GetParam(m_Config.GetParamIndex("klt_shaders_dir")).GetValue().GetSubValue().Text();;
	klt_shaders_dir = (moText)m_pResourceManager->GetDataMan()->GetDataPath()+moText("/") + (moText)klt_shaders_dir;
	m_SampleRate = m_Config.GetParam(m_Config.GetParamIndex("sample_rate")).GetValue().GetSubValue().Int();
	num_frames = m_Config.GetParam(m_Config.GetParamIndex("num_frames")).GetValue().GetSubValue(0).Int();

    MODebug2->Message( moText("In moTrackerGpuKLT::Init ***********************************************\n") );

    MODebug2->Message( moText("Initializing Tracker's System...\n"));

	//por cada camara inicializa un system:
	nvalues = m_Config.GetValuesCount( trackersystems );
	m_Config.SetCurrentParamIndex(trackersystems);
	/*
MO_TRACKER1D_SYTEM_LABELNAME	0
MO_TRACKER1D_LIVE_SYSTEM	1
MO_TRACKER1D_SYSTEM_ON 2
	*/
	for( int i = 0; i < nvalues; i++) {

		m_Config.SetCurrentValueIndex( trackersystems, i );

		moTrackerGpuKLTSystemPtr pTSystem = NULL;

		pTSystem = new moTrackerGpuKLTSystem();

		if (pTSystem!=NULL) {
			pTSystem->MODebug = MODebug;
			pTSystem->MODebug2 = MODebug2;
			pTSystem->SetName( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKERGPUKLT_SYTEM_LABELNAME).Text() );
			pTSystem->SetLive( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKERGPUKLT_LIVE_SYSTEM).Text() );
			pTSystem->SetActive( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKERGPUKLT_SYSTEM_ON).Int() );
			MODebug2->Message( moText(" Tracker system: Name:") + pTSystem->GetName() + moText(" mLive:") + pTSystem->GetLive() );
		}

		m_TrackerSystems.Add( pTSystem );

	}
	return true;
}

MOboolean moTrackerGpuKLT::Finish()
{
	moTrackerGpuKLTSystemPtr pTSystem = NULL;
	for( int i = 0; i < m_TrackerSystems.Count(); i++) {
		pTSystem = m_TrackerSystems.Get(i);
		if (pTSystem!=NULL) {
			pTSystem->Finish();
			delete pTSystem;
		}
	}
	m_TrackerSystems.Empty();
	return true;
}

MOswitch moTrackerGpuKLT::SetStatus(MOdevcode devcode, MOswitch state)
{
    return true;
}

void moTrackerGpuKLT::SetValue( MOdevcode cd, MOfloat vl ) {}

void moTrackerGpuKLT::SetValue( MOdevcode cd, MOint vl ) {}

MOswitch moTrackerGpuKLT::GetStatus(MOdevcode devcode)
{
	if ( 0 <= devcode && devcode < m_TrackerSystems.Count() ) {
		return m_TrackerSystems.Get(devcode)->IsInit();
	} else if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return m_TrackerSystems.Get(devcode - m_TrackerSystems.Count())->IsInit();
	}
	return 0;
}

MOint moTrackerGpuKLT::GetValue(MOdevcode devcode)
{
	moTrackerGpuKLTSystemPtr pTS = NULL;

	if ( 0 <= devcode && devcode < m_TrackerSystems.Count() ) {
		return m_TrackerSystems.Get(devcode)->GetData()->m_NFeatures;
	} else if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return m_TrackerSystems.Get( devcode - m_TrackerSystems.Count() )->GetData()->m_VideoFormat.m_Width;
	}

    return(-1);
}

MOpointer moTrackerGpuKLT::GetPointer(MOdevcode devcode ) {

	if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return (MOpointer)( m_TrackerSystems.Get(devcode-m_TrackerSystems.Count())->GetData() );
	}
	return (NULL);
}

MOdevcode moTrackerGpuKLT::GetCode(moText strcod)
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

void moTrackerGpuKLT::Update(moEventList *Events)
{
	//get the pointer from moLive...

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
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	while(actual!=NULL) {
		//solo nos interesan los del LIVE IODevice
		if(actual->deviceid == m_pResourceManager->GetVideoMan()->GetId() ) {

            //MODebug2->Push( moText("new sample received") );

			pSample = (moVideoSample*)actual->pointer;

			pBucket = (moBucket*)pSample->m_pSampleBuffer;

			//atencion! el devicecode corresponde al  iesimo dispositivo de captura...
			if ( actual->devicecode < m_TrackerSystems.Count() && pSample && pBucket && ( (m_SampleCounter % m_SampleRate)==0)) {
				moTrackerGpuKLTSystemPtr pTS=NULL;
				pTS = m_TrackerSystems.Get( actual->devicecode );
				if ( pTS )
					if (pTS->IsActive() ) {
						if (!pTS->IsInit()) {
						    MOboolean res = false;
							res = pTS->Init(num_feat, pSample,
									  klt_shaders_dir, gpu_arch,
									  min_dist, threshold_eigen);
                            if (res) {
                                MODebug2->Message( "moTrackerGpuKLT::Update TrackerGPUKltSystem initialized..." );
                            } else {
                                MODebug2->Error( "moTrackerGpuKLT::Update Error Couldn't initialized effect" );
                            }
						}

						pTS->NewData( pSample );

						//calcular pesos y otras yerbas
						moTrackerGpuKLTSystemData* pTData;
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

						for( i=0; i<pTData->m_FeatureList->_nFeats; i++) {
							if (pTData->m_FeatureList->_list[i]->valid) {
								sumX+= pTData->m_FeatureList->_list[i]->x;
								sumY+= pTData->m_FeatureList->_list[i]->y;
								NF++;
							}
						}

						if (NF>0) {
							meanX = sumX / (float)NF;
							meanY = sumY / (float)NF;
							TrackersX = (int) meanX;
							TrackersY = (int) meanY;

							for( i=0; i<pTData->m_FeatureList->_nFeats; i++) {
								if (pTData->m_FeatureList->_list[i]->valid) {
									disV+= (meanX - pTData->m_FeatureList->_list[i]->x)*(meanX - pTData->m_FeatureList->_list[i]->x)
										+ (meanY - pTData->m_FeatureList->_list[i]->y)*(meanY - pTData->m_FeatureList->_list[i]->y);
								}
							}
							Variance =(int) ( (float)disV / (float)NF);
						}

						TrackersX = (TrackersX*320)/pSample->m_VideoFormat.m_Width;
						TrackersY = (TrackersY*240)/pSample->m_VideoFormat.m_Height;


						Events->Add( MO_IODEVICE_TRACKER, NF, TrackersX , TrackersY, Variance, 0 );

						m_Outlets[actual->devicecode]->GetData()->SetPointer( (MOpointer)pTS->GetData(), sizeof(moTrackerGpuKLTSystemData) );
						m_Outlets[actual->devicecode]->Update();
					}
			}

			tmp = actual->next;
			actual = tmp;
		} else actual = actual->next;//no es el que necesitamos...
	}

	moMoldeoObject::Update(Events);

}
