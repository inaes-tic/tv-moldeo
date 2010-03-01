/*******************************************************************************

                              moNetTUIOIn.h

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

#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#include "TuioClient.h"


#include "moConnectors.h"

enum moNetTUIOInParamIndex {
    NETTUIOIN_PORT,
    NETTUIOIN_HOST,
	NETTUIOIN_LATENCY,
	NETTUIOIN_SIZE,
	NETTUIOIN_RECONNECTTIME,
	NETTUIOIN_INLET,
	NETTUIOIN_OUTLET
};

using namespace TUIO;

class moNetTUIOListener : public TuioListener, public moAbstract {

public:
	moNetTUIOListener(int port);
	~moNetTUIOListener() {
		tuioClient->disconnect();
		delete tuioClient;
	}

	void addTuioObject(TuioObject *tobj);
	void updateTuioObject(TuioObject *tobj);
	void removeTuioObject(TuioObject *tobj);

	void addTuioCursor(TuioCursor *tcur);
	void updateTuioCursor(TuioCursor *tcur);
	void removeTuioCursor(TuioCursor *tcur);

	void refresh(TuioTime frameTime);

	TuioClient *tuioClient;

private:

	void drawObjects();

	bool verbose, fullscreen, running;

	int width, height;
	int screen_width, screen_height;
	int window_width, window_height;
};

// moNetTUIOIn class **************************************************

class moNetTUIOIn : public moIODevice
{
public:
    moNetTUIOIn();
    virtual ~moNetTUIOIn();

    virtual void Update(moEventList*);
    MOboolean Init();
    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
    MOint GetValue(MOdevcode);
    MOdevcode GetCode( moText);
    MOboolean Finish();
    moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

private:

    moNetTUIOListener*   m_pListener;

    // Parameters.
    moTextArray host_name;
    moIntArray host_port;

	float sendInterval;
	int maxEventNum;
	float minReconnecTime;

    void SendEvent(int i);
};

class moNetTUIOInFactory : public moIODeviceFactory {

public:
    moNetTUIOInFactory() {}
    virtual ~moNetTUIOInFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif
