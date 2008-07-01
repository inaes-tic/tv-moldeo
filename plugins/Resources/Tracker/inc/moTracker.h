/*******************************************************************************

                                moTracker.h

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

  Class:
  moTracker

  Description:
  Motion Analyzer


*******************************************************************************/

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"
#include "moVideoGraph.h"
#include "moArray.h"
#include "moShaderGLSL.h"
#include "moRenderManager.h"

#ifndef __MO_TRACKER_H
#define __MO_TRACKER_H

#define MO_TRACKER_SYTEM_LABELNAME	0
#define MO_TRACKER_LIVE_SYSTEM	1
#define MO_TRACKER_SYSTEM_ON 2

//2 samples, first as reference, second as last calculated
#define MO_TRACKER_SAMPLES_ANALYSIS		2

#define MO_TRACKER_IMAGE_REFERENCE		0
#define MO_TRACKER_IMAGE_ACTUAL			1

#define MO_TRACKER_PROCESSSIGNAL			94
#define MO_TRACKER_SMOOTHDIFF   			95
#define MO_TRACKER_REMOVENOISE   			96
#define MO_TRACKER_AREAFILTER   			97
#define MO_TRACKER_HEURISTICFILTER 			98
#define MO_TRACKER_DISCRETIZE				99
#define MO_TRACKER_CALIBRATION				100
#define MO_TRACKER_NOISECOEFF_UP			101
#define MO_TRACKER_NOISECOEFF_DOWN			102
#define MO_TRACKER_AREACOEFF_UP				103
#define MO_TRACKER_AREACOEFF_DOWN			104
#define MO_TRACKER_NOISECOEFF    			105
#define MO_TRACKER_AREACOEFF    			106

class LIBMOLDEO_API moTrackerSystem : public moAbstract {
	
	public:	

		moTrackerSystem();
		virtual ~moTrackerSystem();

		void SetName( moText p_name) { 
			m_Name = p_name;
		}
		void SetLive( moText p_livecodestr) { 
			m_Live = p_livecodestr;
		}
		void SetActive( MOboolean p_active) { 
			m_bActive = p_active;
		}
		void SwitchProcessSignal() { 
			m_bProcessSignal = !m_bProcessSignal;			
		}
		void SwitchSmoothDiff() { 
			m_bSmoothDiff = !m_bSmoothDiff;			
		}
		void SwitchRemoveNoise() { 
			m_bRemoveNoise = !m_bRemoveNoise;	
		}
		void SwitchAreaFilter() {
			m_bAreaFilter = !m_bAreaFilter;
		}
		void SwitchHeuristicFilter() {
            m_bHeuristicFilter = !m_bHeuristicFilter;
        }
		void SwitchDiscretize() { 
			m_bDiscretize = !m_bDiscretize;
		}
		void SwitchCalibrate() { 
			m_bCalibration = !m_bCalibration;
			if (m_bCalibration==true) m_framecount = 0;
		}
		void IncAreaCoeff( float areacf ) {
			m_area_coeff+= areacf;
			if (m_area_coeff<0) m_area_coeff = 0.0;
		}
		void IncNoiseCoeff( float noisecf ) {
			m_noise_coeff+= noisecf;
			if (m_noise_coeff<0) m_noise_coeff = 0.0;
		}
		MOfloat GetAreaCoeff() { return m_area_coeff; }
		MOfloat GetNoiseCoeff() { return m_noise_coeff; }

		MOboolean	Init( moVideoSample* p_pVideoSample, MOboolean p_gpu, MOboolean p_prev_is_ref = false,
					MOfloat p_noise_coeff = 2.0, MOfloat p_area_coeff = 0.0, MOboolean p_m_update_noise = false,
					MOint p_smooth_iter = 1, MOint p_smooth_length = 5, MOint p_max_curve_diff = 0,
					MOboolean p_noise_max_threshold = false, MOint p_min_length = 0, MOfloat p_min_height_coeff = 0.0,
                    MOboolean p_bProcessSignal = false,	MOboolean p_bSmoothDiff = false, 
					MOboolean p_bRemoveNoise = false, MOboolean p_bAreaFilter = false,
					MOboolean p_bHeuristicFilter = false, MOboolean p_bDiscretize = false,
					moText p_luminance_shader_fn = moText(""), 
					moText p_update_shader_fn = moText(""), 
					moText p_diff_shader_fn = moText(""),
					moRenderManager *m_pRenderManager = NULL);
		MOboolean Finish();
		MOboolean	IsInit();
		void	NewData( moVideoSample* p_pVideoSample );
		moTrackerSystemData*	GetData();			
		moText GetName() { return m_Name; }
		moText GetLive() { return m_Live; }
		MOboolean IsActive() { return m_bActive; }


	protected:		
		moRenderManager *RenderMan;

        // Signal processing parameters.
	    MOfloat m_noise_coeff;
		MOfloat m_area_coeff;
		MOint m_smooth_iter;
		MOint m_smooth_length;
		MOboolean m_noise_max_threshold;
		MOboolean m_prev_is_ref;
		MOint m_min_length;
		MOfloat m_min_height_coeff;
		MOint m_max_curve_diff;
		MOboolean m_update_noise;

		// Noise statistics.
		MOfloat m_noise_ave;
		MOfloat m_noise_std;
		MOfloat m_noise_max;
		MOfloat m_noise_ave_raw;
		MOfloat m_noise_std_raw;
		MOfloat m_noise_max_raw;
		MOfloat m_noise_ave_smooth;
		MOfloat m_noise_std_smooth;
		MOfloat m_noise_max_smooth;

		moText m_Name;
		moText m_Live;
		MOboolean m_bActive;
		MOboolean m_bCalibration;
        MOboolean m_bProcessSignal;
		MOboolean m_bSmoothDiff;
		MOboolean m_bRemoveNoise;
		MOboolean m_bAreaFilter;
		MOboolean m_bHeuristicFilter;
		MOboolean m_bDiscretize;
		moTrackerSystemData	m_TrackerSystemData;

		//=================================
		//Image analysis values
		//=================================
		
		//histograms
		MOfloat		m_Histogram[100];//levels of luminance
		MOfloat		m_HistogramArray[MO_TRACKER_SAMPLES_ANALYSIS][100];//1 sample for analysis
		
		//global luminance
		MOfloat		m_GlobalLuminance;
		MOfloat		m_GlobalLuminanceArray[MO_TRACKER_SAMPLES_ANALYSIS];//1 sample left for analysis
		
		//invariant area luminance, for fine adjusting
		MOfloat		m_InvariantLuminance;
		MOfloat		m_InvariantLuminanceArray[MO_TRACKER_SAMPLES_ANALYSIS];

		//global contrast for quality analysis...
		MOfloat		m_GlobalContrastFactor;
		MOfloat		m_GlobalContrastFactorArray[MO_TRACKER_SAMPLES_ANALYSIS];

		//=================================
		//data analysis
		//=================================
		MOint		m_sizebuffer;
		MOpointer	m_buffer;
		MOpointer	m_refbuffer;
		MOfloat		m_fresult;
		MOfloat*	m_difsumY;
		MOfloat*    m_difsumY_tmp;

		MOboolean	m_init;
		MOint		m_framecount;


		//temporary vars
		GLfloat *m_diff_col;

		//=================================
		// GPU members and methods.
		//=================================
		MOboolean m_gpu;
		moReduceShader* m_reduce_shader;
		GLuint TEXFP_TARGET, TEXFP_FORMAT;
		GLint current_viewport[4];

		// Framebuffer objects.
		GLuint m_fbo0;
		GLint m_draw_buffer0;

		// Texture ids.
        GLuint m_in_tex, m_ref_tex[2], m_curr_tex, m_diff_tex;

		// Attachement point variables for ping-pong rendering.
        GLenum m_ref_tex_attach_point[2];
		MOint m_ref_tex_read, m_ref_tex_write;

		// Shaders.
        moShaderGLSL *m_luminance_shader;
        MOint m_luminance_shader_in_tex_sampler;

        moShaderGLSL *m_update_shader;
        MOint m_update_shader_in_tex_sampler;
		MOint m_update_shader_ref_tex_sampler;

        moShaderGLSL *m_diff_shader;
		MOint m_diff_shader_curr_tex_sampler;
		MOint m_diff_shader_ref_tex_sampler;
		MOint m_diff_shader_ref_global_luminance;
		MOint m_diff_shader_curr_global_luminance;

		moShaderGLSL *m_copy_shader;
		MOint m_copy_shader_curr_tex_sampler;
		MOint m_copy_shader_num_out;

		// Functions.
		void InitGPUcalc(moText p_luminance_shader_fn, 
			             moText p_update_shader_fn, 
			    	 	 moText p_diff_shader_fn);
		void FreeGPUmem();
		void SetupInTex();
		void SetupFPTex(GLuint texid);
		void SetOrthoView();
		void SetScreenView();
		void SetGLConf();
		void RestoreGLConf();
		void CopyBufferToInputTex();
		void CalcPixelLuminance(GLenum draw_buffer);
		void UpdateRefTex();
		void CopyCurrentTex(GLenum draw_buffer);
		void CalcLuminanceDiff(GLuint ref_tex, MOfloat ref_lum);
		void SwapRefTex();
		void RenderTexQuad(float max_s, float max_t);
		void RenderMultiTexQuad(float max_s0, float max_t0, float max_s1, float max_t1);
		void NormalizeDiff();
		void SmoothDiff();
		void CalculateNoise(int i0, int i1, MOfloat& noise_ave, MOfloat& noise_std, MOfloat& noise_max);
		void CalculateNoise(MOfloat& noise_ave, MOfloat& noise_std, MOfloat& noise_max);
		void RemoveNoise();
        void AreaFilterSignal();
		void HeuristicFilterSignal();
		void DiscretizeSignal();
};


typedef moTrackerSystem* moTrackerSystemPtr; 

template class LIBMOLDEO_API moDynamicArray<moTrackerSystemPtr>;
typedef  moDynamicArray<moTrackerSystemPtr> moTrackerSystems;

class LIBMOLDEO_API moTracker : public moIODevice
{
public:
    moTracker();
    ~moTracker();
    
    void Update(moEventList*);
    MOboolean Init();
    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
	void SetValue( MOdevcode cd, MOint vl );
	void SetValue( MOdevcode cd, MOfloat vl );
    MOint GetValue(MOdevcode);
	MOpointer GetPointer(MOdevcode devcode );
    MOdevcode GetCode( moText);
    MOboolean Finish();


private:
    moConfig config;

    moEventList *events;

protected:

    MOboolean GPU;
	moText CALIBRATION;

	MOboolean prev_is_ref, update_noise;
	MOboolean noise_max_threshold;
	MOint min_length;
	MOint min_height_coeff;
	MOint max_curve_diff;

    MOboolean bProcessSignal;
	MOboolean bSmoothDiff;
	MOboolean bRemoveNoise;
	MOboolean bAreaFilter;
	MOboolean bHeuristicFilter;
	MOboolean bDiscretize;

	MOfloat noise_coeff, area_coeff;
    MOint smooth_iter, smooth_length;
	moText luminance_shader_fn, update_shader_fn, diff_shader_fn;

	moTrackerSystems		m_TrackerSystems;

};


class moTrackerFactory : public moTrackerFactory {

public:
    moTrackerFactory() {}
    virtual ~moTrackerFactory() {}
    moResource* Create();
    void Destroy(moResource* fx);
};


extern "C"
{
MO_PLG_API moResourceFactory* CreateResourceFactory();
MO_PLG_API void DestroyResourceFactory();
}


#endif
