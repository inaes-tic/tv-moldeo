/*******************************************************************************

                              moNetOSCOut.h

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

#include "moTypes.h"

#ifndef __MO_NET_OSC_OUT_H__
#define __MO_NET_OSC_OUT_H__

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"

#include "OscOutboundPacketStream.h"
#include "UdpSocket.h"

//template class LIBMOLDEO_API moDynamicArray<UdpTransmitSocket*>;
//typedef moDynamicArray<UdpTransmitSocket*> moUdpTransmitSocketArray;
moDeclareDynamicArray( UdpTransmitSocket*, moUdpTransmitSocketArray )


// moNetOSCOut class **************************************************

class moNetOSCOut : public moIODevice
{
public:
    moNetOSCOut();
    virtual ~moNetOSCOut();

    void Update(moEventList*);
    MOboolean Init();
    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
    MOint GetValue(MOdevcode);
    MOdevcode GetCode( moText);
    MOboolean Finish();
private:
	moUdpTransmitSocketArray transmitSockets;
	osc::OutboundPacketStream* packetStream;
	char* packetBuffer;
	moEventPacketArray eventPacket;
	int OUTPUT_BUFFER_SIZE;

    // Parameters.
    moTextArray host_name;
    moIntArray host_port;

    bool recog_devices[MO_IODEVICE_TABLET + 1];
    bool delete_events;

	float sendInterval;
	int maxEventNum;
	float minReconnecTime;

    void SendEvent(int i);
};

class moNetOscOutFactory : public moIODeviceFactory {

public:
    moNetOscOutFactory() {}
    virtual ~moNetOscOutFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif
