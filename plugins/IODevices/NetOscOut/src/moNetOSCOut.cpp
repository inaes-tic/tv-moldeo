/*******************************************************************************

                              moNetOSCOut.cpp

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

#include "moNetOSCOut.h"

#include "moArray.cpp"

moDefineDynamicArray( moUdpTransmitSocketArray )

//========================
//  Factory
//========================

moNetOscOutFactory *m_NetOscOutFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_NetOscOutFactory==NULL)
		m_NetOscOutFactory = new moNetOscOutFactory();
	return (moIODeviceFactory*) m_NetOscOutFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_NetOscOutFactory;
	m_NetOscOutFactory = NULL;
}

moIODevice*  moNetOscOutFactory::Create() {
	return new moNetOSCOut();
}

void moNetOscOutFactory::Destroy(moIODevice* fx) {
	delete fx;
}



// moNetOSCOut class **************************************************

moNetOSCOut::moNetOSCOut()
{
	packetBuffer = NULL;
	packetStream = NULL;
    SetName("net_osc_out");
}

moNetOSCOut::~moNetOSCOut()
{
    Finish();
}

MOboolean moNetOSCOut::Init()
{
    int dev;
    MOuint i, n, n_dev, n_hosts;
    moText conf, dev_name;

    // Loading config file.
	conf = m_pResourceManager->GetDataMan()->GetDataPath()+moText("/");
    conf += GetConfigName();
    conf += moText(".cfg");
	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		moText text = "Couldn't load netoscout config";
		MODebug->Push(text);
		return false;
	}

    // Reading list of devices which will be used as source of events to send over the network.
    for(dev = MO_IODEVICE_KEYBOARD; dev <= MO_IODEVICE_TABLET; dev++) recog_devices[dev] = false;
    n = m_Config.GetParamIndex("devices");
    n_dev = m_Config.GetValuesCount(n);
    for(i = 0; i < n_dev; i++)
    {
		dev_name = m_Config.GetParam(n).GetValue(i).GetSubValue(0).Text();

        dev = -1;
        if(!stricmp(dev_name, "keyboard")) dev = MO_IODEVICE_KEYBOARD;
        if(!stricmp(dev_name, "mouse")) dev = MO_IODEVICE_MOUSE;
        if(!stricmp(dev_name, "midi")) dev = MO_IODEVICE_MIDI;
        if(!stricmp(dev_name, "mixer")) dev = MO_IODEVICE_MIXER;
        if(!stricmp(dev_name, "joystick")) dev = MO_IODEVICE_JOYSTICK;
		if(!stricmp(dev_name, "tablet")) dev = MO_IODEVICE_TABLET;
		if(!stricmp(dev_name, "tracker")) dev = MO_IODEVICE_TRACKER;

        if(-1 < dev) recog_devices[dev] = true;
    }

    // Reading hosts names and ports.
    n = m_Config.GetParamIndex("host");
	n_hosts = m_Config.GetValuesCount(n);
	host_name.Init(n_hosts, moText(""));
    host_port.Init(n_hosts, 0);
    for(i = 0; i < n_hosts; i++)
    {
		host_name.Set(i, m_Config.GetParam(n).GetValue(i).GetSubValue(0).Text());
		host_port.Set(i, m_Config.GetParam(n).GetValue(i).GetSubValue(1).Int());
	}

    n = m_Config.GetParamIndex("delete_events");
    delete_events = m_Config.GetParam(n).GetValue(i).GetSubValue(0).Int();

	transmitSockets.Init(n_hosts, NULL);
	eventPacket.Init(n_hosts, NULL);

    n = m_Config.GetParamIndex("latency");
	sendInterval = m_Config.GetParam(n).GetValue(i).GetSubValue(0).Float();

	n = m_Config.GetParamIndex("size");
	maxEventNum = m_Config.GetParam(n).GetValue(i).GetSubValue(0).Int();

	n = m_Config.GetParamIndex("reconnect_time");
	minReconnecTime = m_Config.GetParam(n).GetValue(i).GetSubValue(0).Float();

	for(i = 0; i < n_hosts; i++)
	{
		transmitSockets[i] = new UdpTransmitSocket( IpEndpointName( host_name[i], host_port[i] ) );
	    eventPacket[i] = new moEventPacket(sendInterval, maxEventNum);
	}

    OUTPUT_BUFFER_SIZE = 10 * 7 * maxEventNum; // 10 = maximum length of a 32 bit int in decimal rep.
    packetBuffer = new char[OUTPUT_BUFFER_SIZE];
    packetStream = new osc::OutboundPacketStream( packetBuffer, OUTPUT_BUFFER_SIZE );
	return true;
}

MOswitch moNetOSCOut::SetStatus(MOdevcode codisp, MOswitch state)
{
    return true;
}

MOswitch moNetOSCOut::GetStatus(MOdevcode codisp)
{
    return(-1);
}

MOint moNetOSCOut::GetValue(MOdevcode codisp)
{
    return(-1);
}

MOdevcode moNetOSCOut::GetCode(moText strcod)
{
    return(-1);
}

void moNetOSCOut::Update(moEventList *Eventos)
{
	MOuint i;

    bool res;
    moEvent *tmp;
    moEvent *actual;

    // Sending over the network the events that correspond to recognized devices.
    actual = Eventos->First;
    while(actual != NULL)
    {
		if (actual->deviceid>=0 && actual->deviceid<=MO_IODEVICE_TABLET)
		if(recog_devices[actual->deviceid])
	        for (i = 0; i < host_name.Count(); i++)
				{
					res = eventPacket[i]->AddEvent(actual);
                    if (eventPacket[i]->ReadyToSend())
					{
						SendEvent(i);
						eventPacket[i]->ClearPacket();
						if (!res) eventPacket[i]->AddEvent(actual);
					}
			    }

        if (delete_events)
        {
            tmp = actual->next;
            Eventos->Delete(actual);
            actual = tmp;
        }
        else actual = actual->next;
    }
}

MOboolean moNetOSCOut::Finish()
{
	if (packetStream != NULL)
	{
		delete packetStream;
		packetStream = NULL;
	}
	if (packetBuffer != NULL)
	{
		delete packetBuffer;
		packetBuffer = NULL;
	}
	/*
	for (int i = 0; i < eventPacket.Count(); i++)
	{
	    delete transmitSockets[i];
	    delete eventPacket[i];
	}
	*/
	return true;
}

void moNetOSCOut::SendEvent(int i)
{
	int n;
	moEventStruct ev;

	packetStream->Clear();
    (*packetStream) << osc::BeginBundleImmediate;
	for (n = 0; n < eventPacket[i]->GetNumEvents(); n++)
	{
	   ev = eventPacket[i]->GetEvent(n);
       (*packetStream) << osc::BeginMessage( moText("") + IntToStr(n) )
	                   << ev.valid << ev.devicecode << ev.deviceid
		               << ev.reservedvalue0 << ev.reservedvalue1
		               << ev.reservedvalue2 << ev.reservedvalue3
		               << osc::EndMessage;
    }
    (*packetStream) << osc::EndBundle;
    transmitSockets[i]->Send( packetStream->Data(), packetStream->Size() );
}
