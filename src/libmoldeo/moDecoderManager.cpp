/*******************************************************************************

                              moDecoderManager.cpp

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

*******************************************************************************/

#include "moDecoderManager.h"


#ifdef MO_VDPAU
  #include "moVdpauGraph.h"
  #include "vdpau/vdpau.h"
  #include "vdpau/vdpau_x11.h"
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
#endif



//===========================================
//
//				moVdpauManager
//
//===========================================

moDecoderManager::moDecoderManager()
{
	SetType( MO_OBJECT_RESOURCE );
	SetResourceType( MO_RESOURCETYPE_DECODER );
  SetName("Decoder Manager");


  m_pDecoderDevice = NULL;
  m_pVideoDecoder = NULL;
  m_pVideoMixer = NULL;

  m_pVideoGraph = NULL;

  #ifdef MO_VDPAU
  m_DecoderType = MO_DECODERTYPE_VDPAU;
  #endif

}

moDecoderManager::~moDecoderManager()
{
	Finish();
}

MOboolean moDecoderManager::Init()
{
	if ( m_pResourceManager ) {
		//m_glmanager = m_pResourceManager->GetGLMan();
		#ifdef MO_VDPAU

      m_pDecoderDevice = (moDecoderDevice*) new VdpDevice();

      //typedef VdpStatus VdpDeviceCreateX11(Display *display, int screen,VdpDevice *device, VdpGetProcAddress **get_proc_address)
      void* vdp_xdisplay;
      vdp_xdisplay = NULL;

      VdpStatus vdp_st;
      VdpGetProcAddress *vdp_get_proc_add;
      //vdp_xdisplay =

      //vdp_st = vdp_device_create_x11( (Display *)vdp_xdisplay, 0, (VdpDevice*) m_pDecoderDevice, (VdpGetProcAddress**)&vdp_get_proc_add );
      /*
      m_pVideoDecoder = (moVideoDecoder*) new VdpDecoder(  );
      m_pVideoGraph = (moVideoGraph*) new moVdpauGraph();
      */
      //VdpDecoderProfile profile = 0;

      ///Depends on imageformat!!!
      //profile = VDP_DECODER_PROFILE_H264_MAIN;
      /*
              vdp_st = vdp_decoder_create(
            vdp_device,
            vdp_decoder_profile,
            vid_width,
            vid_height,
            max_reference_frames,
            &decoder
        );
        */
      //VdpStatus Status = VdpDecoderCreate( m_pDecoderDevice, profile, );

		#endif
	}
	return (m_pResourceManager!=NULL);
}

MOboolean moDecoderManager::Finish()
{

	return true;
}

