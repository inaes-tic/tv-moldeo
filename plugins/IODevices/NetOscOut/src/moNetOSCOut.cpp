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

    m_Port = 7400;
    m_SendEvents = 0;
    sendInterval = 100;

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


    moDefineParamIndex( NETOSCOUT_HOSTS, moText("hosts") );
    moDefineParamIndex( NETOSCOUT_PORT, moText("port") );
    moDefineParamIndex( NETOSCOUT_DEVICES, moText("devices") );
    moDefineParamIndex( NETOSCOUT_LATENCY, moText("latency") );
    moDefineParamIndex( NETOSCOUT_MAXEVENTS, moText("max_events") );
    moDefineParamIndex( NETOSCOUT_SENDEVENTS, moText("send_events") );
    moDefineParamIndex( NETOSCOUT_DELETEEVENTS, moText("delete_events") );

    // Reading list of devices which will be used as source of events to send over the network.
    /*
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
    */

    // Reading hosts names and ports.
    n = m_Config.GetParamIndex("hosts");
	n_hosts = m_Config.GetValuesCount(n);

	host_name.Init(n_hosts, moText(""));

    host_port.Init(n_hosts, 0);

    transmitSockets.Init(n_hosts,NULL);

    for(i = 0; i < n_hosts; i++)
    {
		host_name.Set(i, m_Config.GetParam(n).GetValue(i).GetSubValue(0).Text());
		host_port.Set(i, m_Config.GetParam(n).GetValue(i).GetSubValue(1).Int());
	}

    i = 0;

    UpdateParameters();


	for(i = 0; i < n_hosts; i++)
	{
	    unsigned long ii = GetHostByName(host_name[i]);

        char buffer[100];
        snprintf(buffer, 100, "%lu", ii); // Memory-safe version of sprintf.

        moText str = buffer;

	    //MODebug2->Message( moText("moNetOscOut:: host: ") + moText(host_name[i]) + moText(" ip int:") + (moText)str );



	    IpEndpointName ipendpointname( ii, host_port[i] ) ;


		transmitSockets[i] = new UdpTransmitSocket( ipendpointname );
		if (transmitSockets[i]) {
            MODebug2->Message(moText("NetOSCOut UdptransmitSocket Created") );
        }


	    //eventPacket[i] = new moEventPacket(sendInterval, maxEventNum);
	}

    OUTPUT_BUFFER_SIZE = 1024; // 10 = maximum length of a 32 bit int in decimal rep.
    packetBuffer = new char[OUTPUT_BUFFER_SIZE];
    packetStream = new osc::OutboundPacketStream( packetBuffer, OUTPUT_BUFFER_SIZE );
	return true;
}

void
moNetOSCOut::UpdateParameters() {

    sendInterval = m_Config.Double( moR( NETOSCOUT_LATENCY ) );
    m_Port = m_Config.Int( moR( NETOSCOUT_PORT ) );
    maxEventNum = m_Config.Int( moR( NETOSCOUT_MAXEVENTS ) );
    m_SendEvents  = m_Config.Int( moR( NETOSCOUT_SENDEVENTS ) );
    delete_events = m_Config.Int( moR( NETOSCOUT_DELETEEVENTS ) );

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

moConfigDefinition *
moNetOSCOut::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("hosts"), MO_PARAM_TEXT, NETOSCOUT_HOSTS, moValue( "127.0.0.1", "TXT") );
	p_configdefinition->Add( moText("port"), MO_PARAM_NUMERIC, NETOSCOUT_PORT, moValue( "7400", "INT") );

    p_configdefinition->Add( moText("devices"), MO_PARAM_TEXT, NETOSCOUT_DEVICES, moValue( "keyboard", "TXT") );
	p_configdefinition->Add( moText("latency"), MO_PARAM_NUMERIC, NETOSCOUT_LATENCY, moValue( "30", "FLOAT") );
	p_configdefinition->Add( moText("max_events"), MO_PARAM_NUMERIC, NETOSCOUT_MAXEVENTS, moValue( "10", "INT") );

	p_configdefinition->Add( moText("send_events"), MO_PARAM_NUMERIC, NETOSCOUT_SENDEVENTS, moValue( "0", "INT") );
	p_configdefinition->Add( moText("delete_events"), MO_PARAM_NUMERIC, NETOSCOUT_DELETEEVENTS, moValue( "0", "INT") );

	return p_configdefinition;
}

void moNetOSCOut::Update(moEventList *Eventos)
{
	MOuint i;

    bool res;
    moEvent *tmp;
    moEvent *actual;


    UpdateParameters();
    // Sending over the network the events that correspond to recognized devices.
    //Eventos->Add( MO_IODEVICE_TRACKER, moGetTicks(), 112, 113, 114, 115 );

    if (m_SendEvents) {
        actual = Eventos->First;
        while(actual != NULL)
        {
            if ( actual->deviceid==MO_IODEVICE_KEYBOARD ) {

            //if(recog_devices[actual->deviceid])

   			    moDataMessage event_data_message;
			    moData pData;

                pData.SetText( moText("EVENT") );
                event_data_message.Add(pData);

                pData.SetInt( actual->deviceid );
                event_data_message.Add(pData);

                pData.SetInt( actual->devicecode );
                event_data_message.Add(pData);

                pData.SetInt( actual->reservedvalue0 );
                event_data_message.Add(pData);

                pData.SetInt( actual->reservedvalue1 );
                event_data_message.Add(pData);

                pData.SetInt( actual->reservedvalue2 );
                event_data_message.Add(pData);

                pData.SetInt( actual->reservedvalue3 );
                event_data_message.Add(pData);

                for (i = 0; i < host_name.Count(); i++)
                    {
                        /*
                        res = eventPacket[i]->AddEvent(actual);
                        if (eventPacket[i]->ReadyToSend())
                        {
                            SendEvent(i);
                            eventPacket[i]->ClearPacket();
                            if (!res) eventPacket[i]->AddEvent(actual);
                        }
                        */
                        SendDataMessage( i, event_data_message );
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

/*

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

                    MODebug2->Message( moText("netoscout: receiving tracker data: bx:") + (moText)FloatToStr(m_pTrackerData->GetBarycenter().X()) );
                    //MODebug2->Push( moText("N:") + (moText)IntToStr( m_pTrackerData->GetValidFeatures() ) );
                    for (i = 0; i < host_name.Count(); i++)
                    {
                        //res = eventPacket[i]->AddEvent(actual);
                        //if (eventPacket[i]->ReadyToSend())
                        {
                            //MODebug2->Push( moText("I:") + (moText)IntToStr( i ) );
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

    (*packetStream) << osc::BeginMessage( moText("")+ IntToStr(datamessage.Count()) );

    for(int j=0; j< datamessage.Count(); j++) {
        moData data = datamessage[j];
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
    if (transmitSockets[i]) {
        transmitSockets[i]->Send( packetStream->Data(), packetStream->Size() );
        MODebug2->Push(moText("sending") + IntToStr(i));
    }

    //MODebug2->Push(moText("sending"));

}

