/*******************************************************************************

                              moTrackerGpuKLT.h

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

#include "moTypes.h"
#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"
#include "moVideoGraph.h"
#include "moArray.h"
#include "GpuVis.h"

#ifndef __MO_TRACKER_GPUKLT_H__
#define __MO_TRACKER_GPUKLT_H__

#define MO_TRACKERGPUKLT_SYTEM_LABELNAME	0
#define MO_TRACKERGPUKLT_LIVE_SYSTEM		1
#define MO_TRACKERGPUKLT_SYSTEM_ON 		2

class moTrackerGpuKLTSystemData : public moTrackerSystemData
{
	public:
		MOint m_NFeatures;

		GpuKLT_FeatureList*	m_FeatureList;

};

class moTrackerGpuKLTSystem : public moAbstract
{
public:
    moTrackerGpuKLTSystem();
	virtual ~moTrackerGpuKLTSystem();

	void SetName(moText p_name) { m_Name = p_name; }
	void SetLive( moText p_livecodestr) { m_Live = p_livecodestr; }
	void SetActive( MOboolean p_active) { m_bActive = p_active; }
	moText GetName() { return m_Name; }
	moText GetLive() { return m_Live; }
	MOboolean IsActive() { return m_bActive; }
	MOboolean IsInit() { return m_init; }

	MOboolean Init(MOint p_nFeatures, MOint p_width, MOint p_height,
					moText p_shaders_dir, MOint p_arch,
					MOint p_kltmindist = 10, MOfloat p_klteigenthreshold = 0.015);
	MOboolean Init(MOint p_nFeatures, moVideoSample* p_pVideoSample,
					moText p_shaders_dir, MOint p_arch,
					MOint p_kltmindist = 10, MOfloat p_klteigenthreshold = 0.015);

    MOboolean Finish();

	GpuVis_Options* GetTrackingOptions() { return &gopt; }
	GpuKLT_FeatureList* GetFeatureList() { return list; }

	MOint GetNFeatures() { return list->_nFeats; }
	void GetFeature(MOint p_feature, MOfloat &x, MOfloat &y, MOboolean &v);

	void Track(GLubyte *p_pBuffer, MOuint p_RGB_mode);
    void StartTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode);
    void ContinueTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode);

	void NewData( moVideoSample* p_pVideoSample );
	moTrackerGpuKLTSystemData*	GetData() {	return &m_TrackerSystemData; }

private:
	moText m_Name;
	moText m_Live;
	MOboolean m_bActive;
	MOboolean m_init;
	moTrackerGpuKLTSystemData m_TrackerSystemData;
	MOint m_TrackCount;
	MOint m_ReinjectRate;
	MOint m_width, m_height;


	GpuVis_Image			image;			// GpuVis Image object
	GpuKLT_FeatureList	   *list;			// GpuVis Feature List object
	GpuVis_Options 			gopt;			// GpuVis Options object
	GpuVis				   *gpuComputor;	// GpuVis Computor Object
};

typedef moTrackerGpuKLTSystem* moTrackerGpuKLTSystemPtr;

template class moDynamicArray<moTrackerGpuKLTSystemPtr>;
typedef  moDynamicArray<moTrackerGpuKLTSystemPtr> moTrackerGpuKLTSystems;

class moTrackerGpuKLT : public moResource
{
public:
    moTrackerGpuKLT();
    virtual ~moTrackerGpuKLT();

    virtual MOboolean Init();
    virtual MOboolean Finish();

    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
	void SetValue( MOdevcode cd, MOint vl );
	void SetValue( MOdevcode cd, MOfloat vl );
    MOint GetValue(MOdevcode);
	MOpointer GetPointer(MOdevcode devcode );
    MOdevcode GetCode( moText);

    void Update(moEventList*);

protected:
	// Parameters.
	MOint num_feat;
	MOint num_frames;
	MOint min_dist;
	MOfloat threshold_eigen;
	moText klt_shaders_dir;

	MOint gpu_arch;

	MOint m_SampleCounter;
	MOint m_SampleRate;

	moTrackerGpuKLTSystems		m_TrackerSystems;
};


class moTrackerGpuKLTFactory : public moResourceFactory {

public:
    moTrackerGpuKLTFactory() {}
    virtual ~moTrackerGpuKLTFactory() {}
    moResource* Create();
    void Destroy(moResource* fx);
};


extern "C"
{
MO_PLG_API moResourceFactory* CreateResourceFactory();
MO_PLG_API void DestroyResourceFactory();
}

#endif
