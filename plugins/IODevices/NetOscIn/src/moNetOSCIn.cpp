/*******************************************************************************

                              moNetOSCIn.cpp

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

#include "moNetOSCIn.h"

#include "moArray.cpp"

moDefineDynamicArray( moOscPacketListeners )

//========================
//  Factory
//========================

moNetOSCInFactory *m_NetOSCInFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_NetOSCInFactory==NULL)
		m_NetOSCInFactory = new moNetOSCInFactory();
	return (moIODeviceFactory*) m_NetOSCInFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_NetOSCInFactory;
	m_NetOSCInFactory = NULL;
}

moIODevice*  moNetOSCInFactory::Create() {
	return new moNetOSCIn();
}

void moNetOSCInFactory::Destroy(moIODevice* fx) {
	delete fx;
}



// moNetOSCIn class **************************************************

moNetOSCIn::moNetOSCIn()
{
    SetName("net_osc_in");
}

moNetOSCIn::~moNetOSCIn()
{
    Finish();
}

MOboolean moNetOSCIn::Init()
{
    int dev;
    MOuint i, n, n_dev, n_hosts;
    moText conf, dev_name;

    //==========================
    // CORRESPONDE A UN PREINIT
    //==========================

    // Loading config file.
	conf = m_pResourceManager->GetDataMan()->GetDataPath() + moSlash;
    conf += GetConfigName();
    conf += moText(".cfg");

	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		moText text = "Couldn't load netoscin config";
		MODebug->Push(text);
		return false;
	}

    moMoldeoObject::Init();


    //==========================
    // INIT
    //==========================

	moDefineParamIndex( NETOSCIN_HOST , moText("host") );
	moDefineParamIndex( NETOSCIN_INLET, moText("inlet") );
	moDefineParamIndex( NETOSCIN_OUTLET, moText("outlet") );

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

	for(i = 0; i < n_hosts; i++)
	{
		moOscPacketListener*    pListener = NULL;

		pListener = new moOscPacketListener();
        if (pListener) {
            if ( host_name[i] == moText("all") ) {
                if (host_port[i]>0)
                    pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS,
                                                                host_port[i] ),
                                                               pListener ) );
                else
                    pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS,
                                                                IpEndpointName::ANY_PORT ),
                                                               pListener ) );
            } else if ( host_name[i] != moText("") ) {
                moTextArray ipNumbers;
                unsigned long ipaddress = 0;
                int i1=0, i2=0, i3=0, i4=0;
                ipNumbers = host_name[i].Explode(".");
                if (ipNumbers.Count()==4) {
                    i1 = atoi(ipNumbers[0]);
                    i2 = atoi(ipNumbers[1]);
                    i3 = atoi(ipNumbers[2]);
                    i4 = atoi(ipNumbers[3]);
                    ipaddress = (i1 << 24) & (i2<<16) & (i3<<8) & i4;
                } else {
                    ipaddress = IpEndpointName::ANY_ADDRESS;
                }
                if (host_port[i]>0)
                    pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( ipaddress,
                                                                host_port[i] ),
                                                               pListener ) );
                else
                    pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( ipaddress,
                                                                IpEndpointName::ANY_PORT ),
                                                               pListener ) );
            } else {
                pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS,
                                                                IpEndpointName::ANY_PORT ),
                                                               pListener ) );
            }
            m_OscPacketListeners.Add( pListener );
            pListener->CreateThread();
       }
	}


	return true;
}


moConfigDefinition *
moNetOSCIn::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("host"), MO_PARAM_TEXT, NETOSCIN_HOST );
	return p_configdefinition;
}

MOswitch moNetOSCIn::SetStatus(MOdevcode codisp, MOswitch state)
{
    return true;
}

MOswitch moNetOSCIn::GetStatus(MOdevcode codisp)
{
    return(-1);
}

MOint moNetOSCIn::GetValue(MOdevcode codisp)
{
    return(-1);
}

MOdevcode moNetOSCIn::GetCode(moText strcod)
{
    return(-1);
}

void moNetOSCIn::Update(moEventList *Eventos)
{

    MOuint i;

    bool res;
    moEvent *tmp;
    moEvent *actual;


    //clean every outlet!!! important
    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            pOutlet->GetMessages().Empty();
            pOutlet->Update(false);
        }
    }

	for(i=0; i<m_OscPacketListeners.Count(); i++ ) {

        //pass outlets thru listeners to populate the outlets
        moOscPacketListener* pListener = NULL;
        pListener = m_OscPacketListeners[i];

        if (pListener) {
            pListener->Update( GetOutlets() );
        }

	}

	moMoldeoObject::Update(Eventos);

    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            if (pOutlet->Updated()) {
                MODebug2->Push(moText("NetOscIn messages: ")+IntToStr(pOutlet->GetMessages().Count() )
                             +moText(" [1s int]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(0).Int())
                             +moText(" [2d int]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(1).Int())
                             +moText(" [3rd int]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(2).Int())
                             );
            }
        }
    }

}

MOboolean moNetOSCIn::Finish()
{

	return true;
}

void moNetOSCIn::SendEvent(int i)
{

}
