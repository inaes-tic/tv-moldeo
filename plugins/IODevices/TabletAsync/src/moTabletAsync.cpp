/*******************************************************************************

                              moTabletAsync.cpp

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
  Fabricio Costa
  Andrés Colubri

*******************************************************************************/

#include "moTabletAsync.h"

#define MO_TABLET_CFG_STRCOD 0
#define MO_TABLET_CFG_TYPE 1

#define TABLET_MOTION 0

//========================
//  Factory
//========================

moTabletFactory *m_TabletFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_TabletFactory==NULL)
		m_TabletFactory = new moTabletFactory();
	return (moIODeviceFactory*) m_TabletFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_TabletFactory;
	m_TabletFactory = NULL;
}

moIODevice*  moTabletFactory::Create() {
	return new moTablet();
}

void moTabletFactory::Destroy(moIODevice* fx) {
	delete fx;
}

//========================
//  Tablet
//========================

moTablet::moTablet()
{
	Codes = NULL;
	SetName("tablet");
}

moTablet::~moTablet()
{
	Finish();
}

MOboolean moTablet::Init()
{
	MOuint n, i, coparam;
	moText conf;
    moText text;

	// Primero, ponemos todo en falso:(indefinido)
	for(i = 0; i <= TABLET_PEN_CURSOR; i++) Pen[i].Init();
	Codes = NULL;

	// Levantamos el config del tablet
	//levantamos el config del tablet
	conf = m_pResourceManager->GetDataMan()->GetDataPath()+moText("/");
    conf += GetConfigName();
    conf += moText(".cfg");

	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		text = "Couldn't load tablet config";
		MODebug->Push(text);
		return false;
	}

	moMoldeoObject::Init();

	/*

	//levantamos los codes definidos
	coparam = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(coparam);
	ncodes = m_Config.GetValuesCount(coparam);
	Codes = new moTabletCode [ncodes];

	// Ahora traduzco la info
	printf("\nTABLET: ncodes:%i\n",ncodes);

	for( i = 0; i < ncodes; i++) {
		m_Config.SetCurrentValueIndex(coparam,i);
		Codes[i].strcod = m_Config.GetParam().GetValue().GetSubValue(MO_TABLET_CFG_STRCOD).Text();
		Codes[i].code = i;
		Codes[i].tabletcod = getTabletCod( Codes[i].strcod );
		Codes[i].type = m_Config.GetParam().GetValue().GetSubValue(MO_TABLET_CFG_TYPE).Int();
		Codes[i].state = MO_FALSE;//inicializamos en vacio

		if(Codes[i].tabletcod==-1) {
                        text = "ERROR(tablet):\n no se encuentra el code: ";
                        text +=  Codes[i].strcod;
                        text += moText( "\n");
			printf(text);
			exit(0);
		} else {
#ifdef MO_DEBUG
                        text = "strcod: ";
                        text += Codes[i].strcod;
                        text += moText(" ");
			printf(text);
			printf("cod: %i ",Codes[i].code);
			printf("type: %i ",Codes[i].type);
			printf("cod interno: %i\n",Codes[i].tabletcod);
#endif
		}
	}
*/
    #ifdef MO_WIN32
	//TABLET: Graphics Tablet
	t_hTablet = NULL;		// Tablet context handle, required.
	t_prsNew = 0;
	t_curNew = 0;
	t_ortNew.orAzimuth = 0;		// 0 = north
	t_ortNew.orAltitude = 900;	// 900 = vertical
	t_ortNew.orTwist = 0;

	t_pkXNew = 0;
	t_pkYNew = 0;
	t_pkZNew = 0;

	//EXAMPLE: extra tablet adjustments, used for this example,
	//	they may be useful in some other applications
	t_bTiltSupport = TRUE;	// Table. Is tilt supported
	t_dblAltAdjust = 1;		// Tablet Altitude zero adjust
	t_dblAltFactor = 1;		// Tablet Altitude factor
	t_dblAziFactor = 1;		// Table Azimuth factor
	#else
	#endif

    MO_HANDLE hWnd = m_pResourceManager->GetGuiMan()->GetOpWindowHandle();
    if (hWnd != NULL) {
        if (IsTabletInstalled(hWnd))
        {
            // Getting tablet context.
            t_hTablet = InitTablet(hWnd);
            MODebug2->Push( moText("Tablet installed with name = ") + GetTabletName() );
            m_Listener.SetTablet( t_hTablet );

            m_Listener.CreateThread();

            //for( i = 0; i < ncodes; i++) Codes[i].CreateValuesArray(m_cMaxPkts);
        }
        else
        {
            MODebug2->Push("Tablet not detected.");
            return false;
        }
    }
	else {
		MODebug2->Push("Cannot get window handle.");
		exit(0);
		return false;
	}


	return true;
}

MOboolean moTablet::Finish()
{
	/*if (Codes!=NULL)
	{
        for(int i = 0; i < ncodes; i++) Codes[i].DeleteValuesArray();
		delete[] Codes;
	    Codes = NULL;
	}
	ncodes = 0;
*/
    m_Listener.KillThread();
    #ifdef MO_WIN32
	if (t_hTablet)
	{
	    WTClose(t_hTablet);
	}
    #else
    #endif
	return true;
}

MOswitch moTablet::SetStatus(MOdevcode devcode, MOswitch state)
{
	return true;
}

//notar que el devcode es a su vez el indice dentro del array de Codes
MOswitch moTablet::GetStatus(MOdevcode devcode)
{
	//return(Codes[devcode].state);
	return -1;
}

MOint moTablet::GetValue(MOdevcode devcode)
{
    //return(Codes[devcode].value);
    return -1;
}

MOint moTablet::GetNValues(MOdevcode devcode) {
    //return(Codes[devcode].nValues);
    return -1;
}

MOint moTablet::GetValue(MOdevcode devcode, MOint i)
{
    //return(Codes[devcode].values[i]);
    return -1;
}

MOdevcode moTablet::GetCode(moText strcod)
{
    /*
	MOint codi;
	MOuint i;
	MOint param,value;

	param = m_Config.GetCurrentParamIndex();
	value = m_Config.GetCurrentValueIndex( param );

	codi = m_Config.GetParamIndex( "code" );
	m_Config.SetCurrentParamIndex( codi );
	m_Config.FirstValue();

	for( i = 0; i < m_Config.GetValuesCount( codi ); i++) {
		m_Config.SetCurrentValueIndex( codi, i );
		if(!stricmp( strcod, m_Config.GetParam().GetValue().GetSubValue(MO_TABLET_CFG_STRCOD).Text() ) )
			return i;
	}

	m_Config.SetCurrentParamIndex( param );
	m_Config.SetCurrentValueIndex( param, value );
*/
	return(-1);//error, no encontro
}

MOint moTablet::getTabletCod(moText s)
{
    /*
	MOuint i, ntabletcods;
	MOint param,value,tab,returned = -1;//debemos restaurar el value luego

	param = m_Config.GetCurrentParamIndex();
	value = m_Config.GetCurrentValueIndex(param);

	tab = m_Config.GetParamIndex("pen");
	m_Config.SetCurrentParamIndex(tab);
	ntabletcods = m_Config.GetValuesCount(tab);
	for( i = 0; i < ntabletcods; i++) {
		m_Config.SetCurrentValueIndex(tab,i);
		if(!stricmp( s, m_Config.GetParam().GetValue().GetSubValue(MO_TABLET_CFG_STRCOD).Text() )) {
				returned = i;
				break;
		}
	}

	m_Config.SetCurrentParamIndex(param);
	m_Config.SetCurrentValueIndex(param,value);

	return returned;
	*/
	return -1;
}

void moTablet::Update(moEventList *Events)
{

    #ifdef MO_WIN32
	if (!t_hTablet) return;
	#else
	return;
    #endif


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

    m_Listener.Update( GetOutlets() );

	moMoldeoObject::Update(Events);

    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            if (pOutlet->Updated()) {



                MODebug2->Push(moText("Tablet messages: ")+IntToStr(pOutlet->GetMessages().Count() )
                             +moText(" [XPOS]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(0).Int())
                             +moText(" [YPOS]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(1).Int())
                             +moText(" [ZPOS]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(2).Int())
                             +moText(" [CURSOR]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(3).Int())
                             +moText(" [PRESSURE]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(4).Int())
                             +moText(" [BUTTONS]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(5).Int())
                             );
            }
        }
    }

/*
    int i, n, tabletcode;

	UINT nTabPressure = 0;	// 0 = nothing 255 = hardest
	int nTabAlt = 0;	    // Which way up for Wacom pens, negative = eraser, positive = normal tip
	int nTabTwist = 0;	    // Spin about x axis, I don't think this is used on Wacom pens
	int nTabCompass = 0;	// 0 = North, 900 = east etc.
	UINT nTabCursor = 0;	// A number for the selected cursor or pointer, Wacom 1=normal, 2=eraser

	UINT  		t_buttonNew;
	UINT  		prsOld;
	UINT  		curOld;
	ORIENTATION ortOld;

	LONG         pkXOld;
	LONG         pkYOld;
	LONG         pkZOld;

	PACKET* pkt;     // the current packet

	// Update codes.
	for(n = 0; n < ncodes; n++) Codes[n].Cleanup();

    // Polling the packets in the queue with
	// int WTPacketsGet(hCtx, cMaxPkts, lpPkts), where
	// Parameter	Type/Description
	// hCtx	HCTX  Identifies the context whose packets are being returned.
	// cMaxPkts	int  Specifies the maximum number of packets to return.
	// lpPkts	LPVOID  Points to a buffer to receive the event packets.
    // Return Value	The return value is the number of packets copied in the buffer.
	UINT cPkts = WTPacketsGet(t_hTablet, m_cMaxPkts, m_lpPkts);

    if (cPkts == 0)
	{
		// No events... setting pressure to zero.
		tabletcode = TABLET_PEN_PRESSURE;
        Pen[tabletcode].Change(0);
		if (Pen[tabletcode].change)
		{
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);

			MODebug2->Push( moText(" Tablet: ") + IntToStr(Pen[tabletcode].value) );
		}
	}

	for (i = 0; i < cPkts; i++)
	{
		pkt = &m_lpPkts[i];

		// Saving old data...
		prsOld = t_prsNew;
		curOld = t_curNew;
		ortOld = t_ortNew;

		pkXOld = t_pkXNew;
		pkYOld = t_pkYNew;
		pkZOld = t_pkZNew;

		t_pkXNew = m_pResourceManager->GetRenderMan()->RenderWidth() * ((float)pkt->pkX / (float)lcMine.lcOutExtX);
		t_pkYNew = m_pResourceManager->GetRenderMan()->RenderHeight() * ((float)pkt->pkY / (float)lcMine.lcOutExtY);
		t_pkZNew = pkt->pkZ;

		t_curNew = pkt->pkCursor;
		t_prsNew = pkt->pkNormalPressure;
		t_ortNew = pkt->pkOrientation;

		t_buttonNew = pkt->pkButtons;

		nTabPressure = t_prsNew;
		nTabCompass = t_ortNew.orAzimuth ;	//Clockwise rotation about z-azis, 0 is north, 900 is east, 1800 is south and 2700 is west, 3599 is nearly north again.
		nTabAlt = t_ortNew.orAltitude ;	    //Tilt.  If Wacom then Positive = normal tip, negative = eraser tip
		nTabTwist = t_ortNew.orTwist ;	    //I don't think it's used on Wacom pen tablets
		nTabCursor = t_curNew;	            //1=normal 2=eraser.

		tabletcode = -1;

		if (t_pkXNew != pkXOld)
		{
			tabletcode = TABLET_PEN_MOTION_X;
			Pen[tabletcode].Change(t_pkXNew);
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);

            MODebug2->Push( moText(" motion x: ") + IntToStr(Pen[tabletcode].value) );
		}

		if (t_pkYNew != pkYOld)
		{
			tabletcode = TABLET_PEN_MOTION_Y;
			Pen[tabletcode].Change(t_pkYNew);
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);

			MODebug2->Push( moText(" motion y: ") + IntToStr(Pen[tabletcode].value) );
		}

		if (t_pkZNew != pkZOld)
		{
			tabletcode = TABLET_PEN_MOTION_Z;
			Pen[tabletcode].Change(t_pkZNew);
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);
		}

		if (t_prsNew != prsOld)
		{
			tabletcode = TABLET_PEN_PRESSURE;
            Pen[tabletcode].Change(t_prsNew);
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);
			//MODebug->Push(moText("Pressure: ") + IntToStr(t_prsNew));
			MODebug2->Push( moText(" pressure: ") + IntToStr(Pen[tabletcode].value) );
		}

		if (t_ortNew.orAzimuth != ortOld.orAzimuth)
		{
			tabletcode = TABLET_PEN_AZIMUTH;
            Pen[tabletcode].Change(t_ortNew.orAzimuth);
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);

			MODebug2->Push( moText(" azimuth: ") + IntToStr(Pen[tabletcode].value) );
		}

		if (t_ortNew.orAltitude != ortOld.orAltitude)
		{
			tabletcode = TABLET_PEN_TILT;
            Pen[tabletcode].Change(t_ortNew.orAltitude);
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);
		}

		if (t_ortNew.orTwist != ortOld.orTwist)
		{
			tabletcode = TABLET_PEN_TWIST;
            Pen[tabletcode].Change(t_ortNew.orTwist);
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);
		}

		if (t_curNew != curOld)
		{
			tabletcode = TABLET_PEN_CURSOR;
            Pen[tabletcode].Change(MOCursorCode(t_curNew));
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);
		}
*/
		/*
		if ((TABLET_PEN_MOTION_X <= tabletcode) && (tabletcode <= TABLET_PEN_CURSOR))
			if (Pen[tabletcode].change)
			{
				Codes[tabletcode].state = true;
				Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);
			}
			*/
/*
		if (t_buttonNew > 1)
		{
			//MODebug->Push("Pressure when pressing button: " + IntToStr(t_prsNew));
			tabletcode = TABLET_PEN_BUTTON;
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(t_buttonNew);
		}
	}
	*/
}

#ifdef MO_WIN32
HCTX moTablet::InitTablet(HWND hWnd)
{
	//TABLET: get current settings as a starting point for this context of the tablet.
	//WTInfo(WTI_DEFCONTEXT, 0, &lcMine);	// default settings may be different to current settings
	WTInfo(WTI_DEFSYSCTX, 0, &lcMine);	// current settings as set in control panel

	lcMine.lcOptions |= CXO_MESSAGES;	// keep existing options and make sure message handling is on for this context
	//TABLET: PACKETDATA must be defined FIRST before including pktdef.h. See the header file of this class for more details
	lcMine.lcPktData = PACKETDATA;	// these settings MUST be defined in the pktdef.h file, see notes
	lcMine.lcPktMode = PACKETMODE;
	lcMine.lcMoveMask = PACKETDATA;

	return WTOpen(hWnd, &lcMine, TRUE);
}

BOOL moTablet::IsTabletInstalled(MO_HANDLE hWnd)
{
	struct	tagAXIS TpOri[3];	// The capabilities of tilt (required)
	double	dblTpvar;				// A temp for converting fix to double (for example)

	BOOL bReturn = TRUE;

	// check if WinTab available.
	if (!WTInfo(0, 0, NULL))
	{
		MODebug2->Push("WinTab Services Not Available.");
		bReturn = FALSE;
	}

	if (bReturn)
	{
		// get info about tilt
		t_bTiltSupport = WTInfo(WTI_DEVICES,DVC_ORIENTATION,&TpOri);
		if (t_bTiltSupport)
		{
			//used for example
			// does the tablet support azimuth and altitude
			if (TpOri[0].axResolution && TpOri[1].axResolution) {

				// convert azimuth resulution to double
				dblTpvar = FIX_DOUBLE(TpOri[0].axResolution);
				// convert from resolution to radians
				t_dblAziFactor = dblTpvar/(2*moMathf::PI);

				// convert altitude resolution to double
				dblTpvar = FIX_DOUBLE(TpOri[1].axResolution);
				// scale to arbitrary value to get decent line length
				t_dblAltFactor = dblTpvar/1000;
				 // adjust for maximum value at vertical
				t_dblAltAdjust = (double)TpOri[1].axMax/t_dblAltFactor;
			}
			//end of used for example
		}
		else {  // no so don't do tilt stuff
			t_bTiltSupport = FALSE;
			MODebug2->Push("Tablet does NOT supports tilt!");
		}	//end tilt support
	}	//end does tablet exists
	return bReturn;
}
#else

Display* moTablet::InitTablet(MO_HANDLE hWnd) {

    Display* display = (Display*)hWnd;

    t_hScreen = DefaultScreen(display);

    GetDeviceInfo(display);

    return display;
}

bool moTablet::IsTabletInstalled(MO_HANDLE hWnd) {

    struct stat sts;
    if ((stat ("/dev/input/wacom", &sts)) == -1 && errno == ENOENT)
    {
        return false;
    }
    return true;

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Find all extension-devices containing the strings 'eraser'/'stylus'.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void moTablet::GetDeviceInfo(Display* display)
{
	int i;
	int len;
	int found;
	int nr_devices;

	char read_buffer[MAXBUFFER];
	char write_buffer[MAXBUFFER];

	t_XDeviceList = XListInputDevices(display, &nr_devices);

	for(i = 0; i < nr_devices; i++) {
		if (t_XDeviceList[i].use == IsXExtensionDevice) {
			len = strlen(t_XDeviceList[i].name);
			snprintf(read_buffer, MAXBUFFER, "%s", t_XDeviceList[i].name);
			if (CheckDeviceName(read_buffer, write_buffer, len)) {
				found = 0;


				if (strstr(write_buffer, "stylus")) {
					found = 1;
				}
				if ((strstr(write_buffer, "stylus")) && (found)) {
					FollowStylus(display, i);
				}


/*
				if (strstr(write_buffer, "eraser")) {
					found = 1;
				}
				if ((strstr(write_buffer, "eraser")) && (found)) {
					FollowEraser(xdevice_list, i);
				}
*/

			}
		}
	}

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Open a stylus-device and start collecting data:
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void moTablet::FollowStylus(Display* display, int number)
{
	int i;

	XValuatorInfoPtr valuator;
	XAnyClassPtr anyclass;

	if ((tmp_device = XOpenDevice(display, t_XDeviceList[number].id))) {
		if (RegisterEvents(display, "stylus")) {

				stylus_xdevice = tmp_device;
				stylus_id = &tmp_device->device_id;
				stylus_name = t_XDeviceList[number].name;

				anyclass = (XAnyClassPtr)(t_XDeviceList[number].inputclassinfo);
				for (i = 0; i < t_XDeviceList[number].num_classes; i++) {
					if (anyclass->c_class == ValuatorClass) {
						valuator = (XValuatorInfoPtr)anyclass;
						stylus_mode = &valuator->mode;
					}
					anyclass = (XAnyClassPtr)((char*)anyclass + anyclass->length);
				}

		}
	}

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Start looking for supported event types. The scope should be the root window
 (ie everywhere). We're interested in motion events and proximity in/out for
 the touch strips, and button press/release for the pad buttons. For the
 styli we ask about button press and proximity in. Having found the info
 we ask the X server to keep us continuously notified about these events:
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int moTablet::RegisterEvents(Display* display, const char* name)
{
	int i;
	int count = 0;

	XInputClassInfo* ip;
	Window root_win;

	root_win = RootWindow(display, t_hScreen);

	if (name == "stylus") {
		XEventClass event_list[2];
		if (tmp_device->num_classes > 0) {
			for (ip = tmp_device->classes,
				i = 0; i < tmp_device->num_classes; ip++, i++) {
				switch (ip->input_class) {

				case ButtonClass:
				printf("Registering device button press event LOCO\n");
				DeviceButtonPress(tmp_device,
					button_press_type, event_list[count]);
				count++;
				break;

				case ValuatorClass:
				printf("Registering proximity in event LOCO\n");
				ProximityIn(tmp_device,
					proximity_in_type, event_list[count]);
				count++;
				break;

				default:
				break;
				}
			}
		}
		if (XSelectExtensionEvent(display, root_win,
					event_list, count)) {
			return 0;
		}
		return count;
	}
	return 0;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 First processing of an extension-device's name-string:
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int moTablet::CheckDeviceName(char* read_buffer, char* write_buffer, int len)
{
	int i;

	if (len >= MAXBUFFER) {
		len = MAXBUFFER - 1;
	}

	for (i = 0; i < len; i++) {
		write_buffer[i] = tolower(read_buffer[i]);
	}
	write_buffer[i] = '\0';

	if (strstr(write_buffer, "stylus") !=NULL) {
		return 1;
	}
	return 0;

}

#endif

moText moTablet::GetTabletName()
{
    moText strName = moText("");

    #ifdef MO_WIN32
	char	chrWName[50];			// String to hold window name
	WTInfo(WTI_DEVICES, DVC_NAME, chrWName);
	strName = chrWName;
	#else

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     This code uses the "popen" command which creates a pipe, forks and
     invokes a shell where xsetwacom can be run. First action is to ensure that
     version 0.0.7 or greater of xsetwacom is present (ie linuxwacom-0.7.5-2
     where the option GetTabletID was introduced). Thereafter we match the tablet
     decimal number string against known tablet numbers. A full table can be
     found in src/xdrv/wcmUSB.c of the linuxwacom sources (Hex numbers).
     +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	int ok_xsetwacom = 0;

    // volito
    const char* volito = "96";          /* 0x60 */

    // volito2
    const char* volito2_pp = "97";      /* 0x61 */
	const char* volito2_4x5 = "98";     /* 0x62 */
	const char* volito2_2x3 = "99";     /* 0x63 */
	const char* volito2_pp2 = "100";    /* 0x64 */

    // cintiqPartner
	const char* cintiqpartner = "3";    /* 0x03 */

	// cintiq
    const char* cintiq_pl400 = "48";    /* 0x30 */
    const char* cintiq_pl500 = "49";    /* 0x31 */
    const char* cintiq_pl600 = "50";    /* 0x32 */
    const char* cintiq_pl600sx = "51";  /* 0x33 */
    const char* cintiq_pl550 = "52";    /* 0x34 */
    const char* cintiq_pl800 = "53";    /* 0x35 */
    const char* cintiq_pl700 = "55";    /* 0x37 */
    const char* cintiq_pl510 = "56";    /* 0x38 */
    const char* cintiq_pl710 = "57";    /* 0x39 */
    const char* cintiq_pl720 = "192";   /* 0xC0 */
    const char* cintiq_dtf521 = "196";  /* 0xC4 */

    // cintiqV5
	const char* cintiq_21ux = "63";     /* 0x3F */
	const char* cintiq_20wsx = "197";   /* 0xC5 */
	const char* cintiq_12wx = "198";    /* 0xC6 */

    // intuos1
	const char* i1_4x5 = "32";          /* 0x20 */
	const char* i1_6x8 = "33";          /* 0x21 */
	const char* i1_9x12 = "34";         /* 0x22 */
	const char* i1_12x12 = "35";        /* 0x23 */
	const char* i1_12x18 = "36";        /* 0x24 */

    // intuos2
	const char* i2_4x5 = "65";          /* 0x41 */
	const char* i2_6x8 = "66";          /* 0x42 */
	const char* i2_9x12 = "67";         /* 0x43 */
	const char* i2_12x12 = "68";        /* 0x44 */
	const char* i2_12x18 = "69";        /* 0x45 */
	const char* i2_6x8b = "71";          /* 0x47 */

    // intuos3
	const char* i3_6x8 = "177";     /* 0xB1 */
	const char* i3_9x12 = "178";    /* 0xB2 */
	const char* i3_12x12 = "179";   /* 0xB3 */
	const char* i3_12x19 = "180";   /* 0xB4 */
	const char* i3_6x11 = "181";    /* 0xB5 */
	const char* i3s_4x5 = "176";    /* 0xB0 */
	const char* i3s_4x6 = "183";    /* 0xB7 */

    // penpartner
	const char* ppartner = "0";     /* 0x00 */

	// graphire1
	const char* g1 = "16";          /* 0x10 */

    // graphire2
	const char* g2_4x5 = "17";      /* 0x11 */
	const char* g2_5x7 = "18";      /* 0x12 */

    // graphire3
	const char* g3_4x5 = "19";      /* 0x13 */
	const char* g3_6x8 = "20";      /* 0x14 */

    // graphire4
	const char* g4_4x5 = "21";      /* 0x15 */
	const char* g4_6x8 = "22";      /* 0x16 */
	const char* g4b_6x8 = "129";    /* 0x81 */

    // bamboo
    const char* bamboo = "101";     /* 0x65 */
    const char* bamboo1 = "105";    /* 0x69 */

    // bambooFun
    const char* bamboofun_4x5 = "23"; /* 0x17 */
    const char* bamboofun_6x8 = "24"; /* 0x18 */

    // Minimum xsetwacom version we can use is 0.0.7
	const int min_xsetwacom = 7;

	char read_buffer[MAXBUFFER];
	char write_buffer[MAXBUFFER];

	int i;
	int j = 0;
	int len = 0;
	int ok_value = 0;

	FILE* execfp = NULL;
	read_buffer[0] = '\0';

	if ((execfp = popen("xsetwacom -V", "r")) != NULL) {
		fgets(read_buffer, MAXBUFFER, execfp);
		if (((pclose(execfp)) != NON_VALID)
		&& (isdigit(read_buffer[0]))) {
			len = strcspn(read_buffer, " \t\n");
			for (i = 0; i < len; i++) {
				if (isdigit(read_buffer[i])) {
					write_buffer[j] = read_buffer[i];
					j++;
				}
			}
			write_buffer[j] = '\0';
			if ((atoi(write_buffer)) >= min_xsetwacom) {
				ok_xsetwacom = 1;
			}
		}
	}

    // linuxwacom-0.7.7-3 changed GetTabletID to plain TabletID. Later, support
    // for both strings were introduced. We follow the same pattern here, defaulting
    // to the new way, should the old format disappear in a linuxwacom future:
	if (ok_xsetwacom) {
		read_buffer[0] = '\0';
		snprintf(write_buffer,MAXBUFFER, "xsetwacom get %s TabletID",
								"stylus");
		if ((execfp = popen(write_buffer, "r")) != NULL) {
			fgets(read_buffer, MAXBUFFER, execfp);
			if (((pclose(execfp)) != NON_VALID)
			&& (isdigit(read_buffer[0]))) {
				ok_value = 1;
			} else {
				read_buffer[0] = '\0';
				snprintf(write_buffer,MAXBUFFER,
				"xsetwacom get %s GetTabletID", "stylus");
				if ((execfp = popen(write_buffer, "r"))
								!= NULL) {
					fgets(read_buffer, MAXBUFFER, execfp);
					if (((pclose(execfp)) != NON_VALID)
					&& (isdigit(read_buffer[0]))) {
						ok_value = 1;
					}
				}
			}
		}

		if (ok_value) {
			len = strcspn(read_buffer, " \t\n");

			if (((strncmp(read_buffer, volito, len)) == 0)) {
				strName = moText("volito");
			}
			else if (((strncmp(read_buffer, volito2_pp, len)) == 0)
				|| ((strncmp(read_buffer, volito2_4x5, len)) == 0)
				|| ((strncmp(read_buffer, volito2_2x3, len)) == 0)
				|| ((strncmp(read_buffer, volito2_pp2, len)) == 0)) {
				strName = moText("volito2");
			}
			else if (((strncmp(read_buffer, cintiqpartner, len)) == 0)) {
				strName = moText("cintiqpartner");
			}
			else if (((strncmp(read_buffer, cintiq_pl400, len)) == 0)
				|| ((strncmp(read_buffer, cintiq_pl500, len)) == 0)
				|| ((strncmp(read_buffer, cintiq_pl600, len)) == 0)
				|| ((strncmp(read_buffer, cintiq_pl600sx, len)) == 0)
				|| ((strncmp(read_buffer, cintiq_pl550, len)) == 0)
				|| ((strncmp(read_buffer, cintiq_pl800, len)) == 0)
				|| ((strncmp(read_buffer, cintiq_pl700, len)) == 0)
                || ((strncmp(read_buffer, cintiq_pl510, len)) == 0)
				|| ((strncmp(read_buffer, cintiq_pl710, len)) == 0)
				|| ((strncmp(read_buffer, cintiq_pl720, len)) == 0)
				|| ((strncmp(read_buffer, cintiq_dtf521, len)) == 0))	{
				strName = moText("cintiq");
			}
			else if (((strncmp(read_buffer, cintiq_21ux, len)) == 0)
			  || ((strncmp(read_buffer, cintiq_20wsx, len)) == 0)
			  || ((strncmp(read_buffer, cintiq_12wx, len)) == 0)) {
				strName = moText("cintiqV5");
            }
			else if (((strncmp(read_buffer, i1_4x5, len)) == 0)
			  || ((strncmp(read_buffer, i1_6x8, len)) == 0)
			  || ((strncmp(read_buffer, i1_9x12, len)) == 0)
              || ((strncmp(read_buffer, i1_12x12, len)) == 0)
			  || ((strncmp(read_buffer, i1_12x18, len)) == 0)) {
				strName = moText("intuos1");
			}
			else if (((strncmp(read_buffer, i2_4x5, len)) == 0)
			  || ((strncmp(read_buffer, i2_6x8, len)) == 0)
			  || ((strncmp(read_buffer, i2_9x12, len)) == 0)
              || ((strncmp(read_buffer, i2_12x12, len)) == 0)
			  || ((strncmp(read_buffer, i2_12x18, len)) == 0)
			  || ((strncmp(read_buffer, i2_6x8b, len)) == 0)) {
				strName = moText("intuos2");
			}
			else if (((strncmp(read_buffer, i3_6x8, len)) == 0)
				|| ((strncmp(read_buffer, i3_9x12, len)) == 0)
				|| ((strncmp(read_buffer, i3_12x12, len)) == 0)
				|| ((strncmp(read_buffer, i3_12x19, len)) == 0)
				|| ((strncmp(read_buffer, i3_6x11, len)) == 0)
				|| ((strncmp(read_buffer, i3s_4x5, len)) == 0)
				|| ((strncmp(read_buffer, i3s_4x6, len)) == 0))	{
				strName = moText("intuos3");
			}
			else if (((strncmp(read_buffer, ppartner, len)) == 0)) {
				strName = moText("penpartner");
			}
			else if (((strncmp(read_buffer, g1, len)) == 0)) {
				strName = moText("graphire1");
			}
			else if (((strncmp(read_buffer, g2_4x5, len)) == 0)
				|| ((strncmp(read_buffer, g2_5x7, len)) == 0)) {
				strName = moText("graphire2");
			}
			else if (((strncmp(read_buffer, g3_4x5, len)) == 0)
				|| ((strncmp(read_buffer, g3_6x8, len)) == 0)) {
				strName = moText("graphire3");
			}
			else if (((strncmp(read_buffer, g4_4x5, len)) == 0)
				|| ((strncmp(read_buffer, g4_6x8, len)) == 0)
				|| ((strncmp(read_buffer, g4b_6x8, len)) == 0)) {
				strName = moText("graphire4");
			}
			else if (((strncmp(read_buffer, bamboo, len)) == 0)
				|| ((strncmp(read_buffer, bamboo1, len)) == 0)) {
				strName = moText("bamboo");
			}
			else if (((strncmp(read_buffer, bamboofun_4x5, len)) == 0)
				|| ((strncmp(read_buffer, bamboofun_6x8, len)) == 0)) {
				strName = moText("bamboofun");
			}
			else strName = "unrecognized_tablet_model";
		}
	}
	else strName = "unrecognized_tablet_model";

	#endif

	return strName;
}

int moTablet::MOCursorCode(int wt_code)
{
	MODebug2->Push( moText("WT cursor code: ") + IntToStr(wt_code));
	if (wt_code % 2 == 1) return MO_TABLET_CURSOR_PEN;
	else return MO_TABLET_CURSOR_ERASER;
}
