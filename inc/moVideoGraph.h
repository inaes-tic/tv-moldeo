/*******************************************************************************

                                moVideoGraph.h

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

#ifndef __MO_VIDEOGRAPH_H__
#define __MO_VIDEOGRAPH_H__

#include "moTypes.h"
#include "moText.h"
#include "moBuckets.h"
#include "moArray.h"
#include "moAbstract.h"

/*
	RESOLUCIONES

  http://en.wikipedia.org/wiki/Display_resolution


Digital TV standard 	Resolution 	Display aspect ratio 	Pixels
Video CD 	352 �240 (NTSC) 	4:3 (non-square pixels) 	84,480
			352 �288 (PAL) 	101,376
China Video Disc 	352 �480 (NTSC) 	4:3 (non-square pixels) 	168,960
					352 �576 (PAL) 	202,725
SVCD 	480 �480 (NTSC) 	4:3 (non-square pixels) 	230,400
		480 �576 (PAL) 	276,480
EDTV 480p 	640 �480 	4:3 or 16:9 	307,200
			704 �480 	337,920
			852 �480 	408,960
DVD 	704 �480 (NTSC) 	4:3 or 16:9 (non-square pixels) 	337,920
		704 �576 (PAL) 	405,504
D1 	720 �480 (NTSC) 	4:3 or 16:9 (non-square pixels) 	345,600
	720 �576 (PAL) 	414,720
D1 (NTSC with square pixels) 	720 �540 	4:3 	388,800
HDTV 720p 	1280 �720 	16:9 	921,600
HDTV 1080p, 1080i 	1920 �1080 	16:9 	2,073,600
*/

enum moVideoMode {
	PAL_D1 = 414720, //720 x 576
	NTSC_D1 = 345600, //720 x 480
	NTSC_SQUARE = 388800, //720x540

	NTSC_VDISC = 168960, //352x480
	PAL_VDISC = 202725, //352x576

	NTSC_SVCD = 230400, //480x480
	PAL_SVCD = 276480, //480x576

	NTSC_VCD = 84480, //352x240
	PAL_VCD = 101376, //352x288

	PAL_DVD = 337920, //704 x 576
	NTSC_DVD = 405504,//704 x 480

	HDTV_720p = 921600, //1280x720
	HDTV_1080p = 2073600, //1920x1080

	VGA = 307200, //640 x 480
	HVGA = 76800, //320 x 240
	CGA = 64000, //320x200
	MOVIDEOMODE_UNKNOWN
};

class LIBMOLDEO_API moVideoFormat {

	public:
		moVideoFormat() {
			m_VideoMode = MOVIDEOMODE_UNKNOWN;
			m_Height = 0;
			m_Width = 0;
			m_BitRate = 0;
			m_TimePerFrame = 0;
			m_BitCount = 0;
			m_BufferSize = 0;
			m_FrameRate = 0;
			m_WaitForFormat = true;
		}

		void SetVideoMode() {
			switch(m_BufferSize) {
				case PAL_D1:
				case NTSC_D1:
				case NTSC_SQUARE:
				case NTSC_VDISC:
				case PAL_VDISC:
				case NTSC_SVCD:
				case PAL_SVCD:
				case NTSC_VCD:
				case PAL_VCD:
				case PAL_DVD:
				case NTSC_DVD:
				case HDTV_720p:
				case HDTV_1080p:
				case VGA:
				case HVGA:
				case CGA:
					m_VideoMode = (moVideoMode)m_BufferSize;
					break;

				default:
					m_VideoMode = MOVIDEOMODE_UNKNOWN;
					break;

			}
		}

		moVideoMode	m_VideoMode;/** modo de video, ver: moVideoMode */
		MOuint m_Height;/** alto en pixels */
		MOuint m_Width;/** ancho en pixels */
		MOuint m_BitRate; /** tasa en bits por segundos*/
		MOuint m_TimePerFrame; /** duración del frame en unidades de 100 nanosegundos (DirectShow) o 1ns (GStreamer), 1 ns = 1e-9 100ns = 1e-7 */
		MOuint m_BitCount;/** bits por cada pixel, 32, 24, 16, 15, 8, 1*/
		MOuint m_BufferSize;/** tamaño en bytes del buffer de imagen*/
        MOuint m_FrameRate;/** cantidad de frames por cada 100 segundos, PAL: 2500 NTSC: 2997*/
        MOboolean m_WaitForFormat;/** bandera que marca la indefinición del formato, default: true*/

};

class LIBMOLDEO_API moVideoSample {

	public:

		moVideoSample() {
			m_pSampleBuffer = NULL;
		}

		moVideoSample& operator = ( moVideoSample& p_vs) {
			m_VideoFormat = p_vs.m_VideoFormat;
			m_pSampleBuffer = p_vs.m_pSampleBuffer;
			return (*this);
		}

		moVideoSample( moVideoFormat p_videoformat, MOpointer p_buffer ) {

			m_VideoFormat = p_videoformat;
			m_pSampleBuffer = p_buffer;

		}

		void SetVideoSample( moVideoFormat p_videoformat, MOpointer p_buffer ) {
			m_VideoFormat = p_videoformat;
			m_pSampleBuffer = p_buffer;
		}

		moVideoFormat	m_VideoFormat;
		void*			m_pSampleBuffer;

};


class LIBMOLDEO_API moCaptureDevice {
	public:

		moCaptureDevice() {
			m_Name = "";
			m_Description = "";
			m_Path  = "";
			m_bPresent = false;
		}

		moCaptureDevice( const moText &p_name, const moText &p_description, const moText &p_path ) {
			m_Name = p_name;
			m_Description = p_description;
			m_Path  = p_path;
			m_bPresent = true;
		}

		//Copy constructor
		moCaptureDevice( const moCaptureDevice &src ) {
			*this = src;
		}

		moText GetName() {
			return m_Name;
		}

		moText GetDescription() {
			return m_Description;
		}

		moText GetPath() {
			return m_Path;
		}

		moVideoFormat GetVideoFormat() {

			return m_VideoFormat;

		}

		void SetVideoFormat( const moVideoFormat &p_videoformat ) {

			m_VideoFormat = p_videoformat;

		}

		void Present( bool p = true ) {
			m_bPresent = p;
		}

		bool IsPresent() {
			return m_bPresent;
		}

		void SetCodeName( const moText &p_codename ) {
			m_CodeName = p_codename;
		}

		moText GetCodeName() {
			return m_CodeName;
		}


		moCaptureDevice &operator = (const moCaptureDevice &src)
		{
			m_bPresent = src.m_bPresent;
			m_Name = src.m_Name;
			m_Description = src.m_Description;
			m_Path = src.m_Path ;
			m_VideoFormat = src.m_VideoFormat;
			m_CodeName = src.m_CodeName;

			return *this;
		}
	private:
		bool			m_bPresent;
		moText			m_Name;
		moText			m_Description;
		moText			m_Path;
		moVideoFormat	m_VideoFormat;

		moText			m_CodeName;

};

/*
template class LIBMOLDEO_API moDynamicArray<moCaptureDevice>;
typedef  moDynamicArray<moCaptureDevice> moCaptureDevices;
*/
moDeclareExportedDynamicArray(moCaptureDevice, moCaptureDevices)

class LIBMOLDEO_API moVideoFramework : public moAbstract {

	public:

		moVideoFramework();
		virtual ~moVideoFramework();
		virtual void SetPreferredDevices( moCaptureDevices* p_pPreferredDevices );
		virtual void SetPreferredFormat( moCaptureDevice &p_CaptureDevice );
		virtual moCaptureDevices*	GetPreferredDevices() {
			return &m_PreferredDevices;
		}
		virtual moCaptureDevices*	GetCaptureDevices() {
			return &m_CaptureDevices;
		}

		virtual moCaptureDevices* LoadCaptureDevices() = 0;
		virtual moCaptureDevices* UpdateCaptureDevices() = 0;
		virtual bool	CheckCaptureDevice( int i ) = 0;
		virtual void CleanCaptureDevices() {
			m_CaptureDevices.Empty();
		}

	protected:
		moCaptureDevices	m_CaptureDevices;
		moCaptureDevices	m_PreferredDevices;

};

class LIBMOLDEO_API moVideoGraph : public moAbstract {

	public:

	moVideoGraph();
	virtual ~moVideoGraph();

//================================================
//	INITIALIZATION AND FINALIZATION METHODS
//================================================
	virtual bool InitGraph() = 0;
	virtual bool FinishGraph() = 0;
	virtual bool BuildLiveGraph( moBucketsPool *pBucketsPool, moCaptureDevice p_capdev) = 0;
	virtual bool BuildLiveVideoGraph( moText filename, moBucketsPool* pBucketsPool) = 0;
	virtual bool BuildLiveQTVideoGraph( moText filename, moBucketsPool* pBucketsPool) = 0;

//================================================
//	CONTROL METHODS
//================================================
	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual void Pause() = 0;
	virtual void Seek( MOuint frame ) = 0;
	virtual bool IsRunning() = 0;
	virtual MOulong	GetFramesLength() = 0;
	virtual MObyte* GetFrameBuffer(MOlong *size) = 0;

	moVideoMode		GetVideoMode();
	moVideoFormat	GetVideoFormat();

	protected:

	moVideoFormat		m_VideoFormat;


};









#endif
