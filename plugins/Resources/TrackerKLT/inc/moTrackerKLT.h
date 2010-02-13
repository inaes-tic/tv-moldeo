/*******************************************************************************

                              moTrackerKLT.h

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

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"
#include "moVideoGraph.h"
#include "moArray.h"
//#include "moLive.h"  //reemplazar esto

#include "klt.h"

#ifndef __MO_TRACKERKLT_H__
#define __MO_TRACKERKLT_H__

#define MO_TRACKERKLT_SYTEM_LABELNAME	0
#define MO_TRACKERKLT_LIVE_SYSTEM	1
#define MO_TRACKERKLT_SYSTEM_ON 2

enum moTrackerKLTParamIndex {
    TRACKERKLT_NUM_FEAT,
    TRACKERKLT_REPLACE_LOST_FEAT,
    TRACKERKLT_DIFF_MODE,
    TRACKERKLT_NUM_SAMPLES,

    TRACKERKLT_MIN_DIST,
    TRACKERKLT_MIN_EIGEN,

    TRACKERKLT_LIGHT_SENS,
    TRACKERKLT_NUM_FRAMES,
    TRACKERKLT_SAMPLE_RATE,

    TRACKERKLT_BORDERX,
    TRACKERKLT_BORDERY,
    TRACKERKLT_NPYRAMIDLEVELS,
    TRACKERKLT_SUBSAMPLING,
    TRACKERKLT_MAX_ITERATIONS,

    TRACKERKLT_WINDOW_WIDTH,
    TRACKERKLT_WINDOW_HEIGHT,

    TRACKERKLT_MIN_DISPLACEMENT,
    TRACKERKLT_MIN_DETERMINANT,
    TRACKERKLT_MAX_RESIDUE,

    TRACKERKLT_MIN_SEGMENT_LEN,
    TRACKERKLT_MAX_SEGMENT_LEN
};

using namespace TUIO;


/*
class moTrackerKLTSystemData : public moTrackerSystemData
{
	public:
		MOint m_NFeatures;

		KLT_FeatureList	m_FeatureList;
		KLT_FeatureTable m_FeatureTable;

};
*/
class moTrackerKLTSystem : public moAbstract
{
public:
    moTrackerKLTSystem();
	virtual ~moTrackerKLTSystem();

	void SetName(moText p_name) { m_Name = p_name; }
	void SetLive( moText p_livecodestr) { m_Live = p_livecodestr; }
	void SetActive( MOboolean p_active) { m_bActive = p_active; }
	moText GetName() { return m_Name; }
	moText GetLive() { return m_Live; }
	MOboolean IsActive() { return m_bActive; }
	MOboolean IsInit() { return m_init; }

	MOboolean Init(MOint p_nFeatures, MOint p_width, MOint p_height,
		      MOboolean p_replaceLostFeatures = true, MOboolean p_sequentialMode = true, MOboolean p_diffMode = false,
			  MOint p_nFrames = 0, MOint p_num_samples = 100, MOint p_mindist = 10, MOfloat p_mineigen = 1.0, MOboolean p_lighting_insensitive = false);
	MOboolean Init(MOint p_nFeatures, moVideoSample* p_pVideoSample,
		      MOboolean p_replaceLostFeatures = true, MOboolean p_sequentialMode = true, MOboolean p_diffMode = false,
			  MOint p_nFrames = 0, MOint p_num_samples = 100, MOint p_mindist = 10, MOfloat p_mineigen = 1.0, MOboolean p_lighting_insensitive = false);

    MOboolean Finish();

	KLT_TrackingContext GetTrackingContext() { return m_tc; }
	KLT_FeatureList GetFeatureList() { return m_fl; }
    KLT_FeatureTable GetFeatureTable() { return m_ft; }

	MOint GetNFeatures() { return m_fl->nFeatures; }
	MOint GetNFrames() { return m_ft->nFrames; }
	void GetFeature(MOint p_feature, MOfloat &x, MOfloat &y, MOfloat &val);
	void GetFeature(MOint p_feature, MOint p_frame, MOfloat &x, MOfloat &y, MOfloat &val);

	void Track(GLubyte *p_pBuffer, MOuint p_RGB_mode);
    void StartTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode);
	void StartTrackingDiffMode();
    void ContinueTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode);

   void StopSequentialMode() { KLTStopSequentialMode(m_tc); m_diffMode = false; }


    void SystemData2TUIO();
    void IterateZone( int ww, int hh, int pp, moTrackerFeature* pFeatureA  );


	void NewData( moVideoSample* p_pVideoSample );
	moTrackerSystemData*	GetData() {	return m_pTrackerSystemData; }
    moTUIOSystemData*	GetTUIOData() {	return m_pTUIOSystemData; }

    float min_segment_len,max_segment_len;
private:


	moText m_Name;
	moText m_Live;
	MOboolean m_bActive;
	MOboolean m_init;

	moTrackerSystemData* m_pTrackerSystemData;
	int* Uplas;

    /**
    TUIO Support for Cursors and Objects
    */
	moTUIOSystemData* m_pTUIOSystemData; ///for compatibility with TUIO
	//std::list<TuioObject*> objectList;
	moTuioObjectArray       m_Objects;
	moTuioCursorArray       m_Cursors;


	MOint m_sizebuffer;
	MOpointer m_buffer;

	int m_curr_sample, m_num_samples;
	bool m_started, m_just_started;
    bool m_diffMode;
    bool m_replaceLostFeatures;
    int m_nCols, m_nRows, m_nFrames, m_nCurrentFrame;
	unsigned char *m_img1, *m_img2;
	float *m_img_sum;
	unsigned char *m_img_diff0, *m_img_diff;
    KLT_TrackingContext m_tc;
    KLT_FeatureList m_fl;
    KLT_FeatureTable m_ft;


	void CopyBufferToImg(GLubyte *p_pBuffer, unsigned char *p_img, MOuint p_RGB_mode);
	void CalcImgDiff0(unsigned char *m_img_new, unsigned char *m_img_ref);
	void CalcImgDiff(unsigned char *m_img_new, unsigned char *m_img_ref);
	void CopyImg2To1();
	void AddBufferToImg(GLubyte *p_pBuffer, MOuint p_RGB_mode);
	void CalcImg1Average();
};

typedef moTrackerKLTSystem* moTrackerKLTSystemPtr;

template class moDynamicArray<moTrackerKLTSystemPtr>;
typedef  moDynamicArray<moTrackerKLTSystemPtr> moTrackerKLTSystems;

class moTrackerKLT : public moResource
{
public:
    moTrackerKLT();
    virtual ~moTrackerKLT();

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
    moConfigDefinition* GetDefinition( moConfigDefinition *p_configdefinition );

	void DrawTrackerFeatures( int isystem );

protected:
    void UpdateParameters();

	// Parameters.
	MOint num_feat, replace_lost_feat, num_samples, num_frames;
	MOint diff_mode, light_sens;
	MOfloat min_dist, min_eigen;

	MOint borderx, bordery;
	MOint pyramid_levels, subsampling, max_iterations;

	MOint window_width, window_height; /// 7
	float min_displacement, min_determinant, max_residue;

	float min_segment_len,max_segment_len;

	MOint m_SampleCounter;
	MOint m_SampleRate;
	MOint m_OutletTuioSystemIndex;

    moVector2f coordA,coordB,coordC,coordD;
    int  icoordA,icoordB,icoordC,icoordD;

	moTrackerKLTSystems		m_TrackerSystems;
};

class moTrackerKLTFactory : public moResourceFactory {

public:
    moTrackerKLTFactory() {}
    virtual ~moTrackerKLTFactory() {}
    moResource* Create();
    void Destroy(moResource* fx);
};


extern "C"
{
MO_PLG_API moResourceFactory* CreateResourceFactory();
MO_PLG_API void DestroyResourceFactory();
}


#endif
