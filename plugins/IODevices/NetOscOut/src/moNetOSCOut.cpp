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
    SetName("netoscout");
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
		MODebug2->Error(text);
		return false;
	}

	moMoldeoObject::Init();

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
    //Eventos->Add( MO_IODEVICE_TRACKER, moGetTicks(), 112, 113, 114, 115 );

/*
    actual = Eventos->First;
    while(actual != NULL)
    {

		if (actual->deviceid>=0 && actual->deviceid<=MO_IODEVICE_TABLET) {
		//if(recog_devices[actual->deviceid])
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
		}

        if (delete_events)
        {
            tmp = actual->next;
            Eventos->Delete(actual);
            actual = tmp;
        }
        else actual = actual->next;
    }
*/

    //inlets outlets
    moMoldeoObject::Update(Eventos);

    moTrackerSystemData* m_pTrackerData = NULL;
    //bool m_bTrackerInit = false;
	//Procesar Inlets

	//MODebug2->Message("netoscout:  updating");

	for(int i=0; i<m_Inlets.Count(); i++) {
		moInlet* pInlet = m_Inlets[i];
		if (pInlet->Updated() && ( pInlet->GetConnectorLabelName()==moText("TRACKERKLT") || pInlet->GetConnectorLabelName()==moText("TRACKERGPUKLT") || pInlet->GetConnectorLabelName()==moText("TRACKERGPUKLT2")) ) {

            //MODebug2->Message("netoscout: receiving tracker data");

			m_pTrackerData = (moTrackerSystemData *)pInlet->GetData()->Pointer();
			if (m_pTrackerData /*&& !m_bTrackerInit*/ ) {

			    moDataMessage tracker_data_message;

			    moData pData;
/*
                pData.SetText( moText("speak") );
                tracker_data_message.Add(pData);

                pData.SetText( moText("N") );
                tracker_data_message.Add(pData);
                */

                pData.SetInt( m_pTrackerData->GetValidFeatures() );
                tracker_data_message.Add(pData);
                //MODebug2->Message( moText("N:") + (moText)IntToStr( m_pTrackerData->GetValidFeatures() ) );

                //if (m_pTrackerData->GetValidFeatures()>0 && m_pTrackerData->GetValidFeatures()<80) {

                    pData.SetText( moText("MAX") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetMin().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetMin().Y() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetMax().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetMax().Y() );
                    tracker_data_message.Add(pData);

                    pData.SetText( moText("BAR") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetBarycenter().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenter().Y() );
                    tracker_data_message.Add(pData);

                    pData.SetText( moText("VAR") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetVariance().X() );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetVariance().Y() );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetVariance().Length() );
                    tracker_data_message.Add(pData);

                    pData.SetText( moText("MP") );
                    tracker_data_message.Add(pData);
                    for(int i=0;i<16;i++) {
                        pData.SetInt( m_pTrackerData->GetPositionMatrix(m_pTrackerData->ZoneToPosition(i)) );
                        tracker_data_message.Add(pData);
                    }

                    pData.SetText( moText("MM") );
                    tracker_data_message.Add(pData);
                    for(int i=0;i<16;i++) {
                        pData.SetInt( m_pTrackerData->GetMotionMatrix(m_pTrackerData->ZoneToPosition(i)) );
                        tracker_data_message.Add(pData);
                    }

                    pData.SetText( moText("MA") );
                    tracker_data_message.Add(pData);
                    for(int i=0;i<16;i++) {
                        pData.SetInt( m_pTrackerData->GetAccelerationMatrix(m_pTrackerData->ZoneToPosition(i)) );
                        tracker_data_message.Add(pData);
                    }

                    pData.SetText( moText("VEL") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetBarycenterMotion().Length() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenterMotion().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenterMotion().Y() );
                    tracker_data_message.Add(pData);

                    pData.SetText( moText("ACC") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetBarycenterAcceleration().Length() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenterAcceleration().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenterAcceleration().Y() );
                    tracker_data_message.Add(pData);

                    //MODebug2->Message( moText("netoscout: receiving tracker data: bx:") + (moText)FloatToStr(m_pTrackerData->GetBarycenter().X()) );

                    for (i = 0; i < host_name.Count(); i++)
                    {
                        //res = eventPacket[i]->AddEvent(actual);
                        //if (eventPacket[i]->ReadyToSend())
                        {
                            SendDataMessage( i, tracker_data_message );
                            //eventPacket[i]->ClearPacket();
                            //if (!res) eventPacket[i]->AddEvent(actual);
                        }
                    }
               // }

			}
		}
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

void moNetOSCOut::SendDataMessage( int i, moDataMessage &datamessage ) {

	packetStream->Clear();
    (*packetStream) << osc::BeginBundleImmediate;

    (*packetStream) << osc::BeginMessage( moText("") );

    for(int i=0; i< datamessage.Count(); i++) {
        moData data = datamessage[i];
        switch(data.Type()) {
            case MO_DATA_NUMBER_FLOAT:
                (*packetStream) << data.Float();
                break;
            case MO_DATA_NUMBER_INT:
                (*packetStream) << data.Int();
                break;
            case MO_DATA_NUMBER_LONG:
                (*packetStream) << data.Long();
                break;
            case MO_DATA_NUMBER_DOUBLE:
                (*packetStream) << data.Double();
                break;
            case MO_DATA_TEXT:
                (*packetStream) << (char*)data.Text();
                break;

        }

    }
    (*packetStream) << osc::EndMessage;
    (*packetStream) << osc::EndBundle;
    transmitSockets[i]->Send( packetStream->Data(), packetStream->Size() );


}

