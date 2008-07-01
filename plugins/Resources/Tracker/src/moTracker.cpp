/*******************************************************************************

                              moTracker.cpp

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

#include "moTracker.h"

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

moTrackerFactory *m_TrackerFactory = NULL;

MO_PLG_API moResourceFactory* CreateResourceFactory(){ 
	if (m_TrackerFactory==NULL)
		m_TrackerFactory = new moTrackerFactory();
	return (moResourceFactory*) m_TrackerFactory;
} 

MO_PLG_API void DestroyResourceFactory(){ 
	delete m_TrackerFactory;
	m_TrackerFactory = NULL;
} 

moResource*  moTrackerFactory::Create() {
	return new moTracker();
}

void moTrackerFactory::Destroy(moResource* fx) {
	delete fx;
}



//===========================================
//
//     Class: moTracker
//
//===========================================


moTracker::moTracker() 
{
	m_Name = "tracker";
	CALIBRATION = moText("");
	GPU = false;	
}

moTracker::~moTracker() 
{
    Finish();
}

MOboolean moTracker::Init() 
{
    //MOuint port;	
	//MOuint mode;
	//MOuint nvaluees;
    moText conf;
	MOint nvalues;
	MOint trackersystems;

	//int i;

    // Loading config file.
    conf = "data/";
    conf += m_Name;
    conf += moText(".cfg");	
    m_Config.LoadConfig(conf);

	trackersystems = m_Config.GetParamIndex("trackersystems");

	//calibracion: x tiempo, x actividad ( luz global ), x 
	CALIBRATION = m_Config.GetParam("CALIBRATION").GetValue().GetSubValue(0).Text();

	GPU = m_Config.GetParam("GPU").GetValue().GetSubValue(0).Int();
	prev_is_ref = m_Config.GetParam("prev_is_ref").GetValue().GetSubValue(0).Int();
    noise_coeff = m_Config.GetParam("noise_coeff").GetValue().GetSubValue(0).Float(); 
    area_coeff = m_Config.GetParam("area_coeff").GetValue().GetSubValue(0).Float();
    smooth_iter = m_Config.GetParam("smooth_iter").GetValue().GetSubValue(0).Int();
	smooth_length = m_Config.GetParam("smooth_length").GetValue().GetSubValue(0).Int();
    noise_max_threshold = m_Config.GetParam("use_noise_max").GetValue().GetSubValue(0).Int();
	min_length = m_Config.GetParam("min_length").GetValue().GetSubValue(0).Int();
    min_height_coeff = m_Config.GetParam("min_height_coeff").GetValue().GetSubValue(0).Float();
	max_curve_diff = m_Config.GetParam("curve_diff").GetValue().GetSubValue(0).Float();
	update_noise = m_Config.GetParam("update_noise").GetValue().GetSubValue(0).Int();

    bProcessSignal = m_Config.GetParam("process_signal").GetValue().GetSubValue(0).Int();
	bSmoothDiff = m_Config.GetParam("smooth_diff").GetValue().GetSubValue(0).Int();
	bRemoveNoise = m_Config.GetParam("remove_noise").GetValue().GetSubValue(0).Int();
	bAreaFilter = m_Config.GetParam("area_filter").GetValue().GetSubValue(0).Int();
	bHeuristicFilter = m_Config.GetParam("heuristic_filter").GetValue().GetSubValue(0).Int();
	bDiscretize = m_Config.GetParam("discretize_signal").GetValue().GetSubValue(0).Int();

    luminance_shader_fn = m_Config.GetParam("luminance_shader").GetValue().GetSubValue(0).Text();
    luminance_shader_fn = moText("data/") + luminance_shader_fn;
    update_shader_fn = m_Config.GetParam("update_shader").GetValue().GetSubValue(0).Text();
    update_shader_fn = moText("data/") + update_shader_fn;
    diff_shader_fn = m_Config.GetParam("diff_shader").GetValue().GetSubValue(0).Text();
    diff_shader_fn = moText("data/") + diff_shader_fn;

    printf("In moTracker::Init ***********************************************\n");    

    printf("Initializing Tracker's System...\n");

	//por cada camara inicializa un system: 
	nvalues = m_Config.GetValuesCount( trackersystems );
	m_Config.SetCurrentParamIndex(trackersystems);
	/*
MO_TRACKER_SYTEM_LABELNAME	0
MO_TRACKER_LIVE_SYSTEM	1
MO_TRACKER_SYSTEM_ON 2
	*/
	for( int i = 0; i < nvalues; i++) {		
		
		m_Config.SetCurrentValueIndex( trackersystems, i );

		moTrackerSystemPtr pTSystem = NULL;

		pTSystem = new moTrackerSystem();
		
		if (pTSystem!=NULL) {
			pTSystem->MODebug = MODebug;
			pTSystem->SetName( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKER_SYTEM_LABELNAME).Text() );
			pTSystem->SetLive( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKER_LIVE_SYSTEM).Text() );
			pTSystem->SetActive( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKER_SYSTEM_ON).Int() );
		}

		m_TrackerSystems.Add( pTSystem );

	}
	return true;
    
}

MOswitch moTracker::SetStatus(MOdevcode devcode, MOswitch state) 
{
    return true;
}

void moTracker::SetValue( MOdevcode cd, MOfloat vl ) {
	moTrackerSystemPtr pTS = NULL;
	switch(cd) {
		case MO_TRACKER_NOISECOEFF_UP:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->IncNoiseCoeff(vl);
			break;

		case MO_TRACKER_NOISECOEFF_DOWN:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->IncNoiseCoeff(vl);
			break;

		case MO_TRACKER_AREACOEFF_UP:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->IncAreaCoeff(vl);
			break;

		case MO_TRACKER_AREACOEFF_DOWN:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->IncAreaCoeff(vl);
			break;
		default:
			break;
	}
}

void moTracker::SetValue( MOdevcode cd, MOint vl ) {
	moTrackerSystemPtr pTS = NULL;
	switch(cd) {
		case MO_TRACKER_PROCESSSIGNAL:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->SwitchProcessSignal();			
			break;
		case MO_TRACKER_SMOOTHDIFF:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->SwitchSmoothDiff();			
			break;
		case MO_TRACKER_REMOVENOISE:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->SwitchRemoveNoise();			
			break;
		case MO_TRACKER_AREAFILTER:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->SwitchAreaFilter();			
			break;
		case MO_TRACKER_HEURISTICFILTER:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->SwitchHeuristicFilter();			
			break;
		case MO_TRACKER_DISCRETIZE:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->SwitchDiscretize();			
			break;
		case MO_TRACKER_CALIBRATION:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				pTS->SwitchCalibrate();			
			break;

		default:
			break;
	}
}


//0: false    1  -1 :true
MOswitch moTracker::GetStatus(MOdevcode devcode) 
{ 	
	if ( 0 <= devcode && devcode < m_TrackerSystems.Count() ) {
		return m_TrackerSystems.Get(devcode)->IsInit();
	} else if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return m_TrackerSystems.Get(devcode - m_TrackerSystems.Count())->IsInit();
	}
	return 0;
}

MOint moTracker::GetValue(MOdevcode devcode)
{ 
	moTrackerSystemPtr pTS = NULL;

	if ( 0 <= devcode && devcode < m_TrackerSystems.Count() ) {
		return m_TrackerSystems.Get(devcode)->GetData()->m_X;
	} else if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return m_TrackerSystems.Get( devcode - m_TrackerSystems.Count() )->GetData()->m_VideoFormat.m_Width;
	}

    switch (devcode) {
		case MO_TRACKER_NOISECOEFF:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				return 100 * pTS->GetNoiseCoeff();
			break;

		case MO_TRACKER_AREACOEFF:
			pTS = m_TrackerSystems.Get(0);
			if (pTS)
				return 1000 * pTS->GetAreaCoeff();
			break;

		default:
			break;
	}

    return(-1); 
}

MOpointer moTracker::GetPointer(MOdevcode devcode ) {

	if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return (MOpointer)( m_TrackerSystems.Get(devcode-m_TrackerSystems.Count())->GetData() );
	}
	return (NULL);

}

//return devicecode index of corresponding code string
MOdevcode moTracker::GetCode(moText strcod) 
{ 
	moText TrackerSystemName;
	moText TrackerX;
	moText TrackerData;

	//example:
	//TRACKER0_X		-> returns 0
	//TRACKER1_X		-> returns 1
	//TRACKER0_DATA		-> returns 2
	//TRACKER1_DATA		-> returns 3

	for( int i = 0; i< m_TrackerSystems.Count(); i++) {
		
		TrackerSystemName = m_TrackerSystems.Get(i)->GetName();
		TrackerX = TrackerSystemName+"_X";		
		TrackerData = TrackerSystemName+"_DATA";

		if ( !stricmp( strcod, TrackerX ) ) {
		
			return i;

		} else if (!stricmp( strcod, TrackerData )) {
		
			return ( i + m_TrackerSystems.Count() );

		}

	}

    return(-1); 
}

//esto se ejecuta por cada CICLO PRINCIPAL DE moConsole(frame)
//por ahora no nos sirve de nada, porque no mandamos events....ni procesamos events...
//al menos que Ligia...o algun device especial(nada mas el hecho de que se haya 
//enchufado la camara por ejemplo
//podriamos poner una funcion aqui de reconocimiento de DV....
void moTracker::Update(moEventList *Events) 
{	
	//get the pointer from moLive...

	moBucket* pBucket = NULL;
	moVideoSample* pSample = NULL;

//	MOuint i;
	moEvent *actual,*tmp;
	
	actual = Events->First;

	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	while(actual!=NULL) {
		//solo nos interesan los del LIVE IODevice
		if(actual->deviceid == MO_IODEVICE_LIVE ) {

			pSample = (moVideoSample*)actual->pointer;					

			pBucket = (moBucket*)pSample->m_pSampleBuffer;
			
			//atencion! el devicecode corresponde al  iesimo dispositivo de captura...
			if ( actual->devicecode < m_TrackerSystems.Count() && pSample && pBucket) {
				moTrackerSystemPtr pTS=NULL;
				pTS = m_TrackerSystems.Get( actual->devicecode );
				if ( pTS )
					if (pTS->IsActive() ) {
						if (!pTS->IsInit()) {
							pTS->Init( pSample, GPU, prev_is_ref, 
								       noise_coeff, area_coeff, update_noise,
									   smooth_iter, smooth_length, max_curve_diff,
									   noise_max_threshold, min_length, min_height_coeff,
                                       bProcessSignal, bSmoothDiff, 
									   bRemoveNoise, bAreaFilter,
	                                   bHeuristicFilter, bDiscretize,
									   luminance_shader_fn, update_shader_fn, diff_shader_fn,
									   m_pResourceManager->MORenderMan);
						}
						pTS->NewData( pSample );						
					}				
			}
			tmp = actual->next;
			actual = tmp;
		} else actual = actual->next;//no es el que necesitamos...
	}

}


MOboolean moTracker::Finish() 
{
	moTrackerSystemPtr pTSystem = NULL;
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



//===========================================
//
//     Class: moTrackerSystem
//
//===========================================



moTrackerSystem::moTrackerSystem() {
	MODebug = NULL;
	m_framecount = 0;

	m_GlobalLuminance = 0.0;
	m_InvariantLuminance = 0.0;	
	m_GlobalContrastFactor = 0.0;

	m_sizebuffer = 0 ;
	m_fresult = 0.0;
	m_buffer = NULL;
	m_refbuffer = NULL;
	m_difsumY = NULL;
	m_difsumY_tmp = NULL;
	m_diff_col = NULL;
	
	m_init = false;
	m_gpu = false;
	m_bCalibration = false;
    m_bProcessSignal = false;
	m_bSmoothDiff = false;
	m_bRemoveNoise = false;
	m_bAreaFilter = false;
	m_bHeuristicFilter = false;
	m_bDiscretize = false;
}

moTrackerSystem::~moTrackerSystem() {
	Finish();
}

MOboolean moTrackerSystem::Finish() {
	m_framecount = 0;
	m_sizebuffer = 0 ;
	m_fresult = 0.0;
	m_buffer = NULL;

	if (m_diff_col!=NULL) {
		delete [] m_diff_col;
		m_diff_col = NULL;
	}
	if (m_difsumY!=NULL) {
		delete [] m_difsumY;
		m_difsumY = NULL;
	}
	if (m_difsumY_tmp!=NULL) {
		delete [] m_difsumY_tmp;
		m_difsumY_tmp = NULL;
	}
	if (m_refbuffer!=NULL) {
		delete [] m_refbuffer;
		m_refbuffer = NULL;
	}

	if (m_gpu && m_init) 
		FreeGPUmem();

	m_gpu = false;
	m_init = false;
	return true;
}

MOboolean moTrackerSystem::IsInit() {
	return m_init;
}


MOboolean moTrackerSystem::Init( moVideoSample* p_pVideoSample, MOboolean p_gpu, MOboolean p_prev_is_ref,
					MOfloat p_noise_coeff,  MOfloat p_area_coeff, MOboolean p_update_noise,
					MOint p_smooth_iter, MOint p_smooth_length, MOint p_max_curve_diff,
					MOboolean p_noise_max_threshold, MOint p_min_length, MOfloat p_min_height_coeff,
                    MOboolean p_bProcessSignal,	MOboolean p_bSmoothDiff, 
					MOboolean p_bRemoveNoise, MOboolean p_bAreaFilter,
					MOboolean p_bHeuristicFilter, MOboolean p_bDiscretize,
					moText p_luminance_shader_fn, moText p_update_shader_fn, moText p_diff_shader_fn, 
					moRenderManager *m_pRenderManager )
{
    RenderMan = m_pRenderManager;

	int i;
	m_framecount = 0;
	if ( p_pVideoSample != NULL ) 
		m_TrackerSystemData.m_VideoFormat = p_pVideoSample->m_VideoFormat;
	else 
		return false;

	if ( m_TrackerSystemData.m_VideoFormat.m_Width<=0 || m_TrackerSystemData.m_VideoFormat.m_Height==0) 
		return false;

	m_refbuffer = (MOpointer) new MOfloat [ m_TrackerSystemData.m_VideoFormat.m_Width * m_TrackerSystemData.m_VideoFormat.m_Height ]; //solo intensidad, esperamos tener un fondo uniforme

	m_difsumY = new MOfloat[m_TrackerSystemData.m_VideoFormat.m_Width];	
	for( i=0; i<m_TrackerSystemData.m_VideoFormat.m_Width; i++ ) {
		m_difsumY[i] = 0.0;
	}

	m_difsumY_tmp = new MOfloat[m_TrackerSystemData.m_VideoFormat.m_Width];	
	for( i=0; i<m_TrackerSystemData.m_VideoFormat.m_Width; i++ ) {
		m_difsumY_tmp[i] = 0.0;
	}

    m_diff_col = new GLfloat[m_TrackerSystemData.m_VideoFormat.m_Width];
	for( i=0; i<m_TrackerSystemData.m_VideoFormat.m_Width; i++) {
		 m_diff_col[i] = 0.0;
	}
	
	if ( m_refbuffer!=NULL && m_difsumY!=NULL && m_diff_col!=NULL)
		m_init = true;
	else 
		return false;

	m_prev_is_ref = p_prev_is_ref;
    m_noise_ave = 0.0;
	m_noise_std = 0.0;
	m_noise_coeff = p_noise_coeff;
	m_area_coeff = p_area_coeff;
    m_smooth_iter = p_smooth_iter;
	m_smooth_length = p_smooth_length;
	m_noise_max_threshold = p_noise_max_threshold;
	m_min_length = p_min_length; 
	m_min_height_coeff = p_min_height_coeff;
	m_max_curve_diff = p_max_curve_diff;
	m_update_noise = p_update_noise;

    m_bProcessSignal = p_bProcessSignal;
	m_bSmoothDiff = p_bSmoothDiff;
	m_bRemoveNoise = p_bRemoveNoise;
	m_bAreaFilter = p_bAreaFilter;
	m_bHeuristicFilter = p_bHeuristicFilter;
	m_bDiscretize = p_bDiscretize;

	m_gpu = p_gpu;
	if (m_gpu)
		InitGPUcalc(p_luminance_shader_fn, p_update_shader_fn, p_diff_shader_fn);

	m_TrackerSystemData.m_X = m_TrackerSystemData.m_Y = m_TrackerSystemData.m_Z = 0.0;
	m_TrackerSystemData.m_XValues = m_difsumY;
	m_TrackerSystemData.m_YValues = NULL;

	return m_init;
}


moTrackerSystemData* moTrackerSystem::GetData() {
	return &m_TrackerSystemData;
}


//Aqui est�la funci� que analiza el movimiento:
//NOTA: descubrir hacia donde se movi�algo o alguien es sencillo siempre y cuando:
// 1) solo haya un objeto movil en la imagen, o a lo sumo un �ea no necesariamente convexa pero reducida en relacion a la imagen
// 2) la c�ara est�fija y la luz no cambie sustencialmente
// 3) el objeto no se acerque demasiado a la camara obstruy�dola
// 4) se puede perfeccionar el an�isis, utilizando balance de blancos...para eso, se puede disponer
//		en la pared frente a la c�ara un �ea blanca que se tomar�como referencia para el balance...
//		o sea, si ese color cambia, se analiza primero esa peque� �ea, corrigi�dola con un factor que se aplicar�
//		al resto de la imagen, cuando se calcule....
void moTrackerSystem::NewData( moVideoSample* p_pVideoSample ) {

	int i,j;
	//int sumY;
	
	//float fsumY;
	float ISUM1,ISUM2;
	int idxMax;

	float dY, difY;
	moText txt;

	if ( !m_init || !m_bActive )
		return;

	if (p_pVideoSample) {		
		moBucket* pBucket = (moBucket*)p_pVideoSample->m_pSampleBuffer;		
		if (pBucket) {
			m_sizebuffer = pBucket->GetSize();
			m_buffer = pBucket->GetBuffer();
		} else return;
	} else 
		return;

	if (m_buffer==NULL) return;

	idxMax = 0;
	ISUM1 = 0;
	ISUM2 = 0;	

	if (m_framecount>10000000) m_framecount = 200;
	else m_framecount = m_framecount + 1;
	
	printf("moTracker: framecount %i:\n", m_framecount);
	
    if (m_gpu)
	{
		// GPU version.

        SetGLConf();
        SetOrthoView();

		CopyBufferToInputTex();
		
	    // Reference texture, average of first 50-100 frames.
	    if ( (m_framecount >= 50) && (m_framecount <= 100)) 
	    {
		    if (m_framecount == 50) CalcPixelLuminance(m_ref_tex_attach_point[m_ref_tex_read]); 
			else UpdateRefTex();

		    if (m_framecount == 100) 
		    {
				// calculate image luminances by reduction.
                m_reduce_shader->ReduceAll(m_ref_tex[m_ref_tex_read], 
				                           m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height, m_GlobalLuminance, m_fbo0);

				m_GlobalLuminance /= m_TrackerSystemData.m_VideoFormat.m_Width * m_TrackerSystemData.m_VideoFormat.m_Height;
			    m_GlobalLuminanceArray[MO_TRACKER_IMAGE_REFERENCE] = m_GlobalLuminance;
		    }
	    }
			
	    if ((m_framecount > 100) && ((m_framecount % 1) == 0)) 
	    {
            CalcPixelLuminance(GL_COLOR_ATTACHMENT2_EXT);

            m_reduce_shader->ReduceAll(m_curr_tex, 
				                       m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height, m_GlobalLuminance, m_fbo0);

			m_GlobalLuminance /= m_TrackerSystemData.m_VideoFormat.m_Width * m_TrackerSystemData.m_VideoFormat.m_Height;
		    m_GlobalLuminanceArray[MO_TRACKER_IMAGE_ACTUAL] = m_GlobalLuminance;

            CalcLuminanceDiff(m_ref_tex[m_ref_tex_read], m_GlobalLuminanceArray[MO_TRACKER_IMAGE_REFERENCE]);
			m_reduce_shader->ReduceRow(m_diff_tex, 
				                       m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height, m_diff_col, m_fbo0);
							
            if (m_prev_is_ref)
			{
				CopyCurrentTex(m_ref_tex_attach_point[m_ref_tex_read]);
                m_GlobalLuminanceArray[MO_TRACKER_IMAGE_REFERENCE] = m_GlobalLuminanceArray[MO_TRACKER_IMAGE_ACTUAL];
			}

            NormalizeDiff();
			if (m_framecount == 101)
			{
				CalculateNoise(m_noise_ave_raw, m_noise_std_raw, m_noise_max_raw); // Noise without smoothing.
			    	if (MODebug)
			    	{
					MODebug->Push( moText("Raw noise"));

                                        txt = "Noise - ave: ";
					txt = txt + FloatToStr(m_noise_ave_raw);
	                		MODebug->Push( txt );

                                        txt = "Noise - std: ";
					txt = txt + FloatToStr(m_noise_ave_raw);
					MODebug->Push( txt );

                                        txt = "Noise - max: ";
					txt = txt + FloatToStr(m_noise_std_raw);
					MODebug->Push( txt );
			    	}
			        for (i = 0; i < m_smooth_iter; i++) SmoothDiff();
				CalculateNoise(m_noise_ave_smooth, m_noise_std_smooth, m_noise_max_smooth); // Noise with smoothing.
				if (MODebug)
				{
					MODebug->Push( moText("Smooth noise"));

                                        txt = "Noise - ave: ";
					txt = txt + FloatToStr(m_noise_ave_smooth);
	                		MODebug->Push( txt );

                                        txt = "Noise - std: ";
					txt = txt + FloatToStr(m_noise_ave_smooth);
					MODebug->Push( txt );

                                        txt = "Noise - max: ";
					txt = txt + FloatToStr(m_noise_std_smooth);
					MODebug->Push( txt );
				}
			}
			else if (m_bProcessSignal)
			{
			    if (m_bSmoothDiff) 
				{
					for (i = 0; i < m_smooth_iter; i++) SmoothDiff();
					if (m_update_noise) CalculateNoise(m_noise_ave_smooth, m_noise_std_smooth, m_noise_max_smooth);
			        m_noise_ave = m_noise_ave_smooth; 
					m_noise_std = m_noise_std_smooth; 
					m_noise_max = m_noise_max_smooth;
				}
				else
				{
					if (m_update_noise) CalculateNoise(m_noise_ave_raw, m_noise_std_raw, m_noise_max_raw);
			        m_noise_ave = m_noise_ave_raw; 
					m_noise_std = m_noise_std_raw; 
					m_noise_max = m_noise_max_raw;
				}
                if (m_bRemoveNoise) RemoveNoise();
			    if (m_bAreaFilter) AreaFilterSignal();
				if (m_bHeuristicFilter) HeuristicFilterSignal();
				if (m_bDiscretize) DiscretizeSignal();
			}
		}
		
		RestoreGLConf();
		SetScreenView();
	}
	else 
	{
		// CPU version

	    // Reference texture, average of first 50-100 frames.
	    if ((m_framecount >= 50) && (m_framecount <= 100)) 
	    {
		    if (m_framecount == 50) 
				for(i=0; i<m_TrackerSystemData.m_VideoFormat.m_Width; i++) {
			        for(j=0; j<m_TrackerSystemData.m_VideoFormat.m_Height; j++) {
				        int idx = (i+j*m_TrackerSystemData.m_VideoFormat.m_Width)*3;
						m_refbuffer[ i + j*m_TrackerSystemData.m_VideoFormat.m_Width ] = 0.299 * m_buffer[idx]+ 0.587 * m_buffer[idx+1] + 0.114 * m_buffer[idx+2];
					}
				}
			else 
                for(i=0; i<m_TrackerSystemData.m_VideoFormat.m_Width; i++) {
			        for(j=0; j<m_TrackerSystemData.m_VideoFormat.m_Height; j++) {
				        int idx = (i+j*m_TrackerSystemData.m_VideoFormat.m_Width)*3;
                        float current_luminance = 0.299 * m_buffer[idx]+ 0.587 * m_buffer[idx+1] + 0.114 * m_buffer[idx+2];
						m_refbuffer[ i + j*m_TrackerSystemData.m_VideoFormat.m_Width ] = 0.5 * (m_refbuffer[i+j*m_TrackerSystemData.m_VideoFormat.m_Width] + current_luminance);
					}
				}						

		    if (m_framecount == 100) 
		    {
				m_GlobalLuminance = 0;
                for(i=0; i<m_TrackerSystemData.m_VideoFormat.m_Width; i++) {
			        for(j=0; j<m_TrackerSystemData.m_VideoFormat.m_Height; j++) {
                        m_GlobalLuminance += (float) m_refbuffer[i+j*m_TrackerSystemData.m_VideoFormat.m_Width];
					}
				}						

				m_GlobalLuminance /= m_TrackerSystemData.m_VideoFormat.m_Width * m_TrackerSystemData.m_VideoFormat.m_Height;
			    m_GlobalLuminanceArray[MO_TRACKER_IMAGE_REFERENCE] = m_GlobalLuminance;
		    }
	    }
			
	    if ((m_framecount > 100) && ((m_framecount % 1) == 0)) 
	    {
            m_GlobalLuminance = 0;
            for(i=0; i<m_TrackerSystemData.m_VideoFormat.m_Width; i++) {
			    for(j=0; j<m_TrackerSystemData.m_VideoFormat.m_Height; j++) {
					int idx =  (i+j*m_TrackerSystemData.m_VideoFormat.m_Width)*3;
                    m_GlobalLuminance += (float) 0.299 * m_buffer[idx]+ 0.587 * m_buffer[idx+1] + 0.114 * m_buffer[idx+2];
				}
			}
			m_GlobalLuminance /= m_TrackerSystemData.m_VideoFormat.m_Width * m_TrackerSystemData.m_VideoFormat.m_Height;
		    m_GlobalLuminanceArray[MO_TRACKER_IMAGE_ACTUAL] = m_GlobalLuminance;

			for(i=0; i<m_TrackerSystemData.m_VideoFormat.m_Width; i++) {
				difY = 0;
				for(j=0; j<m_TrackerSystemData.m_VideoFormat.m_Height; j++) {
					int idx =  (i+j*m_TrackerSystemData.m_VideoFormat.m_Width)*3;
					float current_luminance = 0.299 * m_buffer[idx]+ 0.587 * m_buffer[idx+1] + 0.114 * m_buffer[idx+2];
                    dY = fabs(current_luminance / m_GlobalLuminanceArray[MO_TRACKER_IMAGE_ACTUAL] - 
						      m_refbuffer[ i + j*m_TrackerSystemData.m_VideoFormat.m_Width ] / m_GlobalLuminanceArray[MO_TRACKER_IMAGE_REFERENCE]);
					difY += dY;												
				}
				m_diff_col[i] = difY;
			}

            if (m_prev_is_ref)
            {
		    	// Saving the current luminance data as reference for the next iteration.
                for(i=0; i<m_TrackerSystemData.m_VideoFormat.m_Width; i++) {
			        for(j=0; j<m_TrackerSystemData.m_VideoFormat.m_Height; j++) {
			            int idx =  (i+j*m_TrackerSystemData.m_VideoFormat.m_Width)*3;
                        m_refbuffer[ i + j*m_TrackerSystemData.m_VideoFormat.m_Width ] = 0.299 * m_buffer[idx]+ 0.587 * m_buffer[idx+1] + 0.114 * m_buffer[idx+2];
				    }
				}
                m_GlobalLuminanceArray[MO_TRACKER_IMAGE_REFERENCE] = m_GlobalLuminanceArray[MO_TRACKER_IMAGE_ACTUAL];
			}

            NormalizeDiff();
			if (m_framecount == 101)
			{
    		    if (MODebug) MODebug->Push( moText("Raw noise"));
				CalculateNoise(m_noise_ave_raw, m_noise_std_raw, m_noise_max_raw); // Noise without smoothing.
				for (i = 0; i < m_smooth_iter; i++) SmoothDiff();
				if (MODebug) MODebug->Push( moText("Smoothed noise"));
				CalculateNoise(m_noise_ave_smooth, m_noise_std_smooth, m_noise_max_smooth); // Noise with smoothing.
			}
			else if (m_bProcessSignal)
			{
			    if (m_bSmoothDiff) 
				{
					for (i = 0; i < m_smooth_iter; i++) SmoothDiff();
					// if (m_update_noise) CalculateNoise(m_noise_ave_smooth, m_noise_std_smooth, m_noise_max_smooth);
			        m_noise_ave = m_noise_ave_smooth; 
					m_noise_std = m_noise_std_smooth; 
					m_noise_max = m_noise_max_smooth;
				}
				else
				{
					// if (m_update_noise) CalculateNoise(m_noise_ave_raw, m_noise_std_raw, m_noise_max_raw);
			        m_noise_ave = m_noise_ave_raw; 
					m_noise_std = m_noise_std_raw; 
					m_noise_max = m_noise_max_raw;
				}
                if (m_bRemoveNoise) RemoveNoise();
			    if (m_bAreaFilter) AreaFilterSignal();
				if (m_bHeuristicFilter) HeuristicFilterSignal();
				if (m_bDiscretize) DiscretizeSignal();
			}
		}
	}

}

void moTrackerSystem::InitGPUcalc(moText p_luminance_shader_fn, 
									  moText p_update_shader_fn, 
									  moText p_diff_shader_fn)
{
    bool res = GLEW_ARB_fragment_program;
	if (!res)
	{
		m_gpu = false;
	    return;
	}

    int vendor = RenderMan->GetGPUvendor();
    if (vendor == MO_GPU_NV)
    {
        TEXFP_TARGET = GL_TEXTURE_RECTANGLE_NV;
        TEXFP_FORMAT = GL_FLOAT_R32_NV;
    }
    else if (vendor == MO_GPU_ATI)
    {
        TEXFP_TARGET = GL_TEXTURE_RECTANGLE_ARB;
        TEXFP_FORMAT = GL_LUMINANCE_FLOAT32_ATI;
    }
    else
    {
        TEXFP_TARGET = GL_TEXTURE_RECTANGLE_ARB;
        TEXFP_FORMAT = GL_LUMINANCE32F_ARB;
    }

	// Setting-up textures.
	SetupInTex();

    glGenTextures(2, m_ref_tex);
    SetupFPTex(m_ref_tex[0]);
    SetupFPTex(m_ref_tex[1]);

    glGenTextures(1, &m_curr_tex);
	SetupFPTex(m_curr_tex);

    glGenTextures(1, &m_diff_tex);
	SetupFPTex(m_diff_tex);

	// Setting-up framebuffers.
	glGenFramebuffersEXT(1, &m_fbo0);

    m_ref_tex_read = 0;
    m_ref_tex_write = 1;
    m_ref_tex_attach_point[0] = GL_COLOR_ATTACHMENT0_EXT;
    m_ref_tex_attach_point[1] = GL_COLOR_ATTACHMENT1_EXT;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo0);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_COLOR_ATTACHMENT0_EXT,
                              TEXFP_TARGET, m_ref_tex[0], 0);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_COLOR_ATTACHMENT1_EXT,
                              TEXFP_TARGET, m_ref_tex[1], 0);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_COLOR_ATTACHMENT2_EXT,
                              TEXFP_TARGET, m_curr_tex, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_COLOR_ATTACHMENT3_EXT,
                              TEXFP_TARGET, m_diff_tex, 0);
	RenderMan->CheckFBOStatus();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    m_reduce_shader = new moReduceShader();
	m_reduce_shader->Init(TEXFP_TARGET, TEXFP_FORMAT, m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height);

	// Setting-up shaders.
	printf("Setting up luminance shader...\n");
	m_luminance_shader = new moShaderGLSL();
    m_luminance_shader->LoadFragShader(p_luminance_shader_fn);
	m_luminance_shader_in_tex_sampler = m_luminance_shader->GetUniformID("in_tex_sampler");
	m_luminance_shader->PrintFragShaderLog();

	printf("Setting up update shader...\n");
	m_update_shader = new moShaderGLSL();
    m_update_shader->LoadFragShader(p_update_shader_fn);
    m_update_shader_in_tex_sampler = m_update_shader->GetUniformID("in_tex_sampler");
    m_update_shader_ref_tex_sampler = m_update_shader->GetUniformID("ref_tex_sampler");
	m_update_shader->PrintFragShaderLog();

	printf("Setting up diff shader...\n");
	m_diff_shader = new moShaderGLSL();
	m_diff_shader->LoadFragShader(p_diff_shader_fn);
    m_diff_shader_curr_tex_sampler = m_diff_shader->GetUniformID("curr_tex_sampler");
	m_diff_shader_ref_tex_sampler = m_diff_shader->GetUniformID("ref_tex_sampler");
    m_diff_shader_ref_global_luminance = m_diff_shader->GetUniformID("ref_global_luminance");
    m_diff_shader_curr_global_luminance = m_diff_shader->GetUniformID("curr_global_luminance");
	m_diff_shader->PrintFragShaderLog();

    printf("Setting up copy shader...\n");
	m_copy_shader = new moShaderGLSL();
    m_copy_shader->CreateFragShader(FragShader_Copy_GLSL);
    m_copy_shader_curr_tex_sampler = m_copy_shader->GetUniformID("tex_unit_source"); 
	m_copy_shader_num_out = m_copy_shader->GetUniformID("num_out");
	m_copy_shader->PrintFragShaderLog();
}

void moTrackerSystem::FreeGPUmem()
{
	glDeleteTextures(1, &m_in_tex);
    glDeleteTextures(2, m_ref_tex);
    glDeleteTextures(1, &m_curr_tex);
    glDeleteTextures(1, &m_diff_tex);

    glDeleteFramebuffersEXT(1, &m_fbo0);

	delete m_reduce_shader;

	delete m_luminance_shader;
	delete m_update_shader;
	delete m_diff_shader;
	delete m_copy_shader;
}

void moTrackerSystem::SetupInTex()
{
	glGenTextures(1, &m_in_tex);
    glBindTexture(GL_TEXTURE_2D, m_in_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);	
}

void moTrackerSystem::SetupFPTex(GLuint texid)
{
    glBindTexture(TEXFP_TARGET, texid);
    glTexParameteri(TEXFP_TARGET, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(TEXFP_TARGET, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(TEXFP_TARGET, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(TEXFP_TARGET, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(TEXFP_TARGET, 0, TEXFP_FORMAT, m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height, 0, GL_LUMINANCE, GL_FLOAT, NULL);
}

void moTrackerSystem::SetOrthoView()
{
    glGetIntegerv(GL_VIEWPORT, current_viewport);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, m_TrackerSystemData.m_VideoFormat.m_Width, 0.0, m_TrackerSystemData.m_VideoFormat.m_Height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height);
}

void moTrackerSystem::SetScreenView()
{
    glViewport(0, 0, current_viewport[2], current_viewport[3]);
}

void moTrackerSystem::SetGLConf()
{
	glGetIntegerv(GL_DRAW_BUFFER, &m_draw_buffer0);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo0);

    // Saving current projection and modelview matrices.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

	glPolygonMode(GL_FRONT, GL_FILL);
    glDisable(GL_BLEND);
}

void moTrackerSystem::RestoreGLConf()
{
    // Restoring original projection and modelview matrices.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_BLEND);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDrawBuffer(m_draw_buffer0);
}

void moTrackerSystem::CopyBufferToInputTex()
{
    glBindTexture(GL_TEXTURE_2D, m_in_tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height, GL_RGB, GL_UNSIGNED_BYTE, m_buffer);
}

void moTrackerSystem::CalcPixelLuminance(GLenum draw_buffer)
{
	m_luminance_shader->StartShader();
	glUniform1iARB(m_luminance_shader_in_tex_sampler, 0);
	
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_in_tex);
	glDrawBuffer(draw_buffer);
	RenderTexQuad(1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, 0);

	m_luminance_shader->StopShader();
}

void moTrackerSystem::UpdateRefTex()
{
	m_update_shader->StartShader();
	glUniform1iARB(m_update_shader_in_tex_sampler, 0);
	glUniform1iARB(m_update_shader_ref_tex_sampler, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_in_tex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(TEXFP_TARGET, m_ref_tex[m_ref_tex_read]);

    glDrawBuffer(m_ref_tex_attach_point[m_ref_tex_write]);

    RenderMultiTexQuad(1.0, 1.0, m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height);

    glBindTexture(TEXFP_TARGET, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_update_shader->StopShader();
    SwapRefTex();
}

void moTrackerSystem::CopyCurrentTex(GLenum draw_buffer)
{
    m_copy_shader->StartShader();
    glUniform1iARB(m_copy_shader_curr_tex_sampler, 0);
	glUniform1iARB(m_copy_shader_num_out, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(TEXFP_TARGET, m_curr_tex);

    glDrawBuffer(draw_buffer);

    RenderTexQuad(m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height);

    glBindTexture(TEXFP_TARGET, 0);

    m_copy_shader->StopShader();
}

void moTrackerSystem::CalcLuminanceDiff(GLuint ref_tex, MOfloat ref_lum)
{
    m_diff_shader->StartShader();
    glUniform1iARB(m_diff_shader_curr_tex_sampler, 0);
	glUniform1iARB(m_diff_shader_ref_tex_sampler, 1);
	glUniform1fARB(m_diff_shader_curr_global_luminance, m_GlobalLuminanceArray[MO_TRACKER_IMAGE_ACTUAL]);
	glUniform1fARB(m_diff_shader_ref_global_luminance, ref_lum);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(TEXFP_TARGET, m_curr_tex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(TEXFP_TARGET, ref_tex);

    glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);

	RenderTexQuad(m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height);

    glBindTexture(TEXFP_TARGET, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(TEXFP_TARGET, 0);

    m_diff_shader->StopShader();
}

void moTrackerSystem::SwapRefTex()
{
    int t = m_ref_tex_read;
	m_ref_tex_read = m_ref_tex_write; 
    m_ref_tex_write = t;
}

void moTrackerSystem::RenderTexQuad(float max_s, float max_t)
{
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, 0.0);

        glTexCoord2f(max_s, 0.0);
        glVertex2f(m_TrackerSystemData.m_VideoFormat.m_Width, 0.0);

        glTexCoord2f(max_s, max_t);
        glVertex2f(m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height);

        glTexCoord2f(0.0, max_t);
        glVertex2f(0.0, m_TrackerSystemData.m_VideoFormat.m_Height);
    glEnd();
}

void moTrackerSystem::RenderMultiTexQuad(float max_s0, float max_t0, float max_s1, float max_t1)
{
    glBegin(GL_QUADS);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0, 0.0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0, 0.0);
        glVertex2f(0.0, 0.0);

        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, max_s0, 0.0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, max_s1, 0.0);
		glVertex2f(m_TrackerSystemData.m_VideoFormat.m_Width, 0.0);

        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, max_s0, max_t0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, max_s1, max_t1);
        glVertex2f(m_TrackerSystemData.m_VideoFormat.m_Width, m_TrackerSystemData.m_VideoFormat.m_Height);

        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0, max_t0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0, max_t1);
        glVertex2f(0.0, m_TrackerSystemData.m_VideoFormat.m_Height);
    glEnd();
}

void moTrackerSystem::NormalizeDiff()
{
	for (int i = 0; i < m_TrackerSystemData.m_VideoFormat.m_Width; i++) 
		m_difsumY[i] = m_diff_col[i] / m_TrackerSystemData.m_VideoFormat.m_Height;
}

// Smoothing signal by taking the average inside a sliding window.
void moTrackerSystem::SmoothDiff()
{
	int i, j, i0, i1, l;
	MOfloat sum;

	for (i = 0; i < m_TrackerSystemData.m_VideoFormat.m_Width; i++) m_difsumY_tmp[i] = m_difsumY[i];

	for (i = 0; i < m_TrackerSystemData.m_VideoFormat.m_Width; i++) 
	{
		i0 = max(0, i - m_smooth_length);
	    i1 = min(m_TrackerSystemData.m_VideoFormat.m_Width - 1, i + m_smooth_length);

		sum = 0.0;
		l = 0;
		for (j = i0; j <= i1; j++) 
		{
		    sum += m_difsumY_tmp[j];
		    l++;
		}
        sum /= l;

        m_difsumY[i] = sum;
	}
}

void moTrackerSystem::CalculateNoise(int i0, int i1, MOfloat& noise_ave, MOfloat& noise_std, MOfloat& noise_max)
{
	MOfloat sum, sum_sqr;
	int i, N;

	noise_max = 0.0;
	N = i1 - i0 + 1;
	sum = sum_sqr = 0.0;
	for (i = i0; i <= i1; i++) 
	{
		if (noise_max < m_difsumY[i]) noise_max = m_difsumY[i];
		sum += m_difsumY[i];
		sum_sqr += m_difsumY[i] * m_difsumY[i];
	}
	
    noise_ave = sum / N;
    if (1 < N)
    {
        noise_std = (sum_sqr - N * noise_ave * noise_ave) / (N - 1);
        if (0 <= noise_std) noise_std = sqrt(noise_std); else noise_std = 0;
    }
    else noise_std = 0;
}

// Calcuates max, mean and standard deviation of the noise.
void moTrackerSystem::CalculateNoise(MOfloat& noise_ave, MOfloat& noise_std, MOfloat& noise_max)
{
	int i00, i10, i01, i11, t, chosen_half;
	MOfloat f0, f1;
	MOfloat noise_ave0, noise_std0, noise_max0;
	MOfloat noise_ave1, noise_std1, noise_max1;

	i00 = 0;
	i10 = 0.5 * m_TrackerSystemData.m_VideoFormat.m_Width;
	i01 = i10 + 1;
    i11 = m_TrackerSystemData.m_VideoFormat.m_Width - 1;
	while (true)
	{
        CalculateNoise(i00, i10, noise_ave0, noise_std0, noise_max0);
        CalculateNoise(i01, i11, noise_ave1, noise_std1, noise_max1);
        f0 = noise_std0 / noise_ave0;
		f1 = noise_std1 / noise_ave1;
        
		if (f0 < f1)
            if (f0 < 0.1)
			{
			    chosen_half = 0;
				break;
			}
			else
			{
				t = 0.5 * (i00 + i10);
				i01 = t + 1;
				i11 = i10;
				i10 = t;
			}
		else
		    if (f1 < 0.1)
			{
			    chosen_half = 1;
				break;
			}
			else
			{
				t = 0.5 * (i01 + i11);
                i00 = i01;
				i10 = t;
				i01 = t + 1;
			}

		if ((i10 - i00 < 1) || (i11 - i01 < 1)) 
		{
			noise_std0 = noise_std1 = 0.0;
			break;
		}
	}

	if (chosen_half == 0)
	{
	    noise_ave = noise_ave0;
		noise_std = noise_std0;
		noise_max = noise_max0;
	}
	else
	{
	    noise_ave = noise_ave1;
		noise_std = noise_std1;
		noise_max = noise_max1;
	}
}

// Isolating signals above the noise level.
void moTrackerSystem::RemoveNoise()
{
    MOfloat threshold;
		 
	if (m_noise_max_threshold) threshold = m_noise_max + m_noise_coeff * m_noise_std;
	else threshold = m_noise_ave + m_noise_coeff * m_noise_std;

	for (int i = 0; i < m_TrackerSystemData.m_VideoFormat.m_Width; i++)
	    if (threshold < m_difsumY[i]) m_difsumY[i] -= threshold;
		else m_difsumY[i] = 0;
}

// Runs an area test on the signal curve.
void moTrackerSystem::AreaFilterSignal()
{
	if (m_area_coeff == 0) return;

	int i, j, i0, i1;
	MOfloat total_area, f;

	i0 = -1;
	for (i = 0; i <= m_TrackerSystemData.m_VideoFormat.m_Width; i++)
	{
        if ((i < m_TrackerSystemData.m_VideoFormat.m_Width) && (0 < m_difsumY[i])) 
        {
            if (i0 == -1)
			{
				i0 = i; // A new signal starts.
			    total_area = 0.0;
			}
			total_area += m_difsumY[i];
		}
        else if (-1 < i0)
		{
			// The signal ended at i - 1.
			i1 = i - 1;

			f = total_area / (i1 - i0 + 1);

			// Area check.
            if (f < m_area_coeff * m_noise_std)
                // The area under the signal is too small. Removing it.
				for (j = i0; j <= i1; j++) m_difsumY[j] = 0;

            i0 = -1;
		}
	}
}

// Agregar heuristica en funcion de cantidad de seniales, distancia entre ellas, 
// base y altura. En el futuro estaria bueno poner en script.
void moTrackerSystem::HeuristicFilterSignal()
{
	if ((m_min_length == 0) && (m_min_height_coeff == 0) && (m_max_curve_diff == 0)) return;

	int i, j, i0, i00, i1, curve_length, diff;
	MOfloat curve_max_height, t;

	// Removing small signals.
	i0  = -1;
	for (i = 0; i <= m_TrackerSystemData.m_VideoFormat.m_Width; i++)
	{
        if ((i < m_TrackerSystemData.m_VideoFormat.m_Width) && (0 < m_difsumY[i])) 
        {
            if (i0 == -1)
			{
				i0 = i; // A new signal starts.
				curve_max_height = 0.0;
			}
			if (curve_max_height < m_difsumY[i]) curve_max_height = m_difsumY[i];
		}
        else if (-1 < i0)
		{
			// The signal ended at i - 1.
			i1 = i - 1;
			curve_length = i1 - i0 + 1;

            // Length check.
            if ((0 < m_min_length) && (curve_length < m_min_length))
                // The length of the curve is too small. Removing it.
				for (j = i0; j <= i1; j++) m_difsumY[j] = 0;

			// Height check.
			if ((0 < m_min_height_coeff) && (curve_max_height < m_min_height_coeff * m_noise_std))
                // The max height of the curve is too small. Removing it.
				for (j = i0; j <= i1; j++) m_difsumY[j] = 0;

            i0 = -1;
		}
	}

	// Joining remaining close signals.
	i0 = i1 = -1;
	for (i = 0; i <= m_TrackerSystemData.m_VideoFormat.m_Width; i++)
	{
        if ((i < m_TrackerSystemData.m_VideoFormat.m_Width) && (0 < m_difsumY[i])) 
        {
            if (i0 == -1)
			{
				i0 = i; // A new signal starts.
				if (-1 < i1)
				{
					diff = i0 - i1 - 1;
					if (diff < m_max_curve_diff)
					{
                        // It's joined with the previous signal.
                        for (j = i1 + 1; j < i0; j++)
		    			{
			    			t = MOfloat(j - i1 - 1) / diff;
				    		m_difsumY[j] = m_difsumY[i1] * (1 - t) + m_difsumY[i0] * t;
					    }
						i0 = i00;
					}
				}
			}
		}
        else if (-1 < i0)
		{
			// The signal ended at i - 1.
			i1 = i - 1;

			i00 = i0;
            i0 = -1;
		}
	}
}

void moTrackerSystem::DiscretizeSignal()
{
	int i, j, i0, i1, i_center;
	MOfloat total_area, i_averaged;

	i0 = -1;
	for (i = 0; i <= m_TrackerSystemData.m_VideoFormat.m_Width; i++)
	{
        if ((i < m_TrackerSystemData.m_VideoFormat.m_Width) && (0 < m_difsumY[i])) 
        {
            if (i0 == -1)
			{
				i0 = i; // A new signal starts.
			    total_area = 0.0;
				i_averaged = 0.0;
			}
			total_area += m_difsumY[i];
			i_averaged += i * m_difsumY[i];
		}
        else if (-1 < i0)
		{
			// The signal ended at i - 1.
		    i1 = i - 1;

		 	// Reducing the signal to a single point at i_center.
            i_center = (int)(i_averaged / total_area);
            for (j = i0; j <= i1; j++) m_difsumY[j] = 0;
            m_difsumY[i_center] = 1;

            i0 = -1;
		}
	}
}
