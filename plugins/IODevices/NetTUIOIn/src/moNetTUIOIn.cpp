/*******************************************************************************

                              moNetTUIOIn.cpp

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

#include "moNetTUIOIn.h"

//========================
//  Factory
//========================

moNetTUIOInFactory *m_NetTUIOInFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_NetTUIOInFactory==NULL)
		m_NetTUIOInFactory = new moNetTUIOInFactory();
	return (moIODeviceFactory*) m_NetTUIOInFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_NetTUIOInFactory;
	m_NetTUIOInFactory = NULL;
}

moIODevice*  moNetTUIOInFactory::Create() {
	return new moNetTUIOIn();
}

void moNetTUIOInFactory::Destroy(moIODevice* fx) {
	delete fx;
}


void moNetTUIOListener::addTuioObject(TuioObject *tobj) {
    MODebug2->Push(moText("Add Object"));
/*	if (verbose)
		std::cout << "add obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() << std::endl;
*/

}

void moNetTUIOListener::updateTuioObject(TuioObject *tobj) {
    MODebug2->Push(moText("Update Object"));
    /*
	if (verbose)
		std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle()
		<< " " << tobj->getMotionSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
*/
}

void moNetTUIOListener::removeTuioObject(TuioObject *tobj) {
    if (verbose)
        MODebug2->Push(moText("Remove Object"));

	/*if (verbose)
		std::cout << "del obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;
		*/
}

void moNetTUIOListener::addTuioCursor(TuioCursor *tcur) {
    if (verbose)
    MODebug2->Push(
        moText("Add Cursor: id: ")
        + IntToStr(tcur->getCursorID())
        + moText(" x: ")
        + FloatToStr(tcur->getX())
        + moText(" y: ")
        + FloatToStr(tcur->getY())
    );
/*	if (verbose)
		std::cout << "add cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << std::endl;
*/
}

void moNetTUIOListener::updateTuioCursor(TuioCursor *tcur) {
    if (verbose)
    MODebug2->Push(
        moText("Update Cursor: id: ")
        + IntToStr(tcur->getCursorID())
        + moText(" x: ")
        + FloatToStr(tcur->getX())
        + moText(" y: ")
        + FloatToStr(tcur->getY())
    );
/*
	if (verbose)
		std::cout << "set cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY()
					<< " " << tcur->getMotionSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
*/
}

void moNetTUIOListener::removeTuioCursor(TuioCursor *tcur) {
    if (verbose)
    MODebug2->Push(
        moText("Remove Cursor id: ")
        + IntToStr(tcur->getCursorID())
        + moText(" session id: ")
        + IntToStr(tcur->getSessionID())

    );
/*
	if (verbose)
		std::cout << "del cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ")" << std::endl;
    */
}

void moNetTUIOListener::refresh(TuioTime frameTime) {
	//drawObjects();
}


void moNetTUIOListener::drawObjects() {

	char id[3];

	// draw the cursors
	std::list<TuioCursor*> cursorList = tuioClient->getTuioCursors();
	tuioClient->lockCursorList();

	//MODebug2->Push(moText("Draw Cursors"));

	for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {

		TuioCursor *tuioCursor = (*iter);

		std::list<TuioPoint> path = tuioCursor->getPath();

		if (path.size()>0) {

            TuioPoint last_point = path.front();


            for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
                //glVertex3f(last_point.getX()*width, last_point.getY()*height, 0.0f);
                //glVertex3f(point->getX()*width, point->getY()*height, 0.0f);
                last_point.update(point->getX(),point->getY());

                //MODebug2->Push(moText("cursor: x:") + IntToStr(point->getX()) );

            }

		}
	}
	tuioClient->unlockCursorList();

	// draw the objects

	//MODebug2->Push(moText("Draw ObjectsS"));

	std::list<TuioObject*> objectList = tuioClient->getTuioObjects();
	tuioClient->lockObjectList();
	for (std::list<TuioObject*>::iterator iter = objectList.begin(); iter!=objectList.end(); iter++) {
		TuioObject* tuioObject = (*iter);
		int pos_size = height/20.0f;
		int neg_size = -1*pos_size;
		float xpos  = tuioObject->getX()*width;
		float ypos  = tuioObject->getY()*height;
		float angle = tuioObject->getAngle()/M_PI*180;

        //MODebug2->Push(moText("angle:") + FloatToStr(angle) );
		/*


		glColor3f(0.0, 0.0, 0.0);
		glPushMatrix();
		glTranslatef(xpos, ypos, 0.0);
		glRotatef(angle, 0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
			glVertex2f(neg_size, neg_size);
			glVertex2f(neg_size, pos_size);
			glVertex2f(pos_size, pos_size);
			glVertex2f(pos_size, neg_size);
		glEnd();
		glPopMatrix();

		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(xpos,ypos+5);
		sprintf(id,"%d",tuioObject->getSymbolID());
		drawString(id);
		*/
	}



	tuioClient->unlockObjectList();

}

/*
void moNetTUIOListener::drawString(char *str) {
	if (str && strlen(str)) {
		while (*str) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *str);
			str++;
		}
	}
}
*/

moNetTUIOListener::moNetTUIOListener(int port) {

	tuioClient = new TuioClient(port);
	tuioClient->addTuioListener(this);
	tuioClient->connect();

	if (!tuioClient->isConnected()) {

	    MODebug2->Error(moText("TUIO Listener Error : unable to connect to port: ") + IntToStr(port));


	} else {
        MODebug2->Message(moText("TUIO Listener Connected to port: ") + IntToStr(port));
    }


}



// moNetTUIOIn class **************************************************

moNetTUIOIn::moNetTUIOIn()
{
    SetName("nettuioin");
    m_pListener = NULL;
}

moNetTUIOIn::~moNetTUIOIn()
{
    Finish();
}

MOboolean moNetTUIOIn::Init()
{
    int dev;
    int port;
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
		moText text = "Couldn't load nettuioin config";
		MODebug->Push(text);
		return false;
	}

    moMoldeoObject::Init();


    //==========================
    // INIT
    //==========================

    moDefineParamIndex( NETTUIOIN_PORT , moText("port") );
	moDefineParamIndex( NETTUIOIN_HOST , moText("host") );
	moDefineParamIndex( NETTUIOIN_INLET, moText("inlet") );
	moDefineParamIndex( NETTUIOIN_OUTLET, moText("outlet") );

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


    port = m_Config.GetValue( moText("port"), 0).GetSubValue(0).Int();

    MODebug2->Message(moText("Initializing listener on port: ") + IntToStr(port));

    m_pListener = new moNetTUIOListener( port );

	for(i = 0; i < n_hosts; i++)
	{
	    //TUIO::TuioListener listener;
		/*
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
       */
	}


	return true;
}


moConfigDefinition *
moNetTUIOIn::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("port"), MO_PARAM_NUMERIC, NETTUIOIN_PORT, moValue( moText("3333"), moText("NUM")).Ref() );
	p_configdefinition->Add( moText("host"), MO_PARAM_TEXT, NETTUIOIN_HOST );
	return p_configdefinition;
}

MOswitch moNetTUIOIn::SetStatus(MOdevcode codisp, MOswitch state)
{
    return true;
}

MOswitch moNetTUIOIn::GetStatus(MOdevcode codisp)
{
    return(-1);
}

MOint moNetTUIOIn::GetValue(MOdevcode codisp)
{
    return(-1);
}

MOdevcode moNetTUIOIn::GetCode(moText strcod)
{
    return(-1);
}

void moNetTUIOIn::Update(moEventList *Eventos)
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

    if (m_pListener) {
        //m_pListener->drawObjects();
    }
/*
	for(i=0; i<m_OscPacketListeners.Count(); i++ ) {

        //pass outlets thru listeners to populate the outlets
        moOscPacketListener* pListener = NULL;
        pListener = m_OscPacketListeners[i];

        if (pListener) {
            pListener->Update( GetOutlets() );
        }

	}
*/
	moMoldeoObject::Update(Eventos);

    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            if (pOutlet->Updated()) {
                /*
                MODebug2->Push(moText("NetOscIn messages: ")+IntToStr(pOutlet->GetMessages().Count() )
                             +moText(" [1s int]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(0).Int())
                             +moText(" [2d int]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(1).Int())
                             +moText(" [3rd int]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(2).Int())
                             );
                             */
            }
        }
    }

}

MOboolean moNetTUIOIn::Finish()
{

	return true;
}

void moNetTUIOIn::SendEvent(int i)
{

}
