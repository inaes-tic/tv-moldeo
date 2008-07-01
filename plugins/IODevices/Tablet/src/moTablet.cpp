/*******************************************************************************

                              moTablet.cpp

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

#include "moTablet.h"

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

	if (IsTabletInstalled())
	{
		MO_HANDLE hWnd = m_pResourceManager->GetGuiMan()->GetOpWindowHandle();

		// Getting tablet context.
		if (hWnd != NULL) t_hTablet = InitTablet(hWnd);
		else
		{
			MODebug2->Push("Cannot get window handle.");
			exit(0);
			return false;
		}
		MODebug2->Push( moText("Tablet installed with name = ") + GetTabletName() );

        // Checking the queue size with
		// int WTQueueSizeGet(hCtx), where
	    // This function returns the number of packets the context's queue can hold.
	    // Parameter	Type/Description
	    // hCtx	HCTX  Identifies the context whose queue size is being returned.
	    //WTQueueSizeSet( t_hTablet, 200 );
	    m_cMaxPkts = WTQueueSizeGet(t_hTablet);
		MODebug2->Push( moText("Tablet queue size = ") + IntToStr( m_cMaxPkts ) );

		// Creating packet buffer.
    	m_lpPkts = new PACKET[m_cMaxPkts];
		for( i = 0; i < ncodes; i++) Codes[i].CreateValuesArray(m_cMaxPkts);
	}
	else
	{
		MODebug2->Push("Tablet not detected.");
		return false;
	}

	return true;
}

MOboolean moTablet::Finish()
{
	if (Codes!=NULL)
	{
        for(int i = 0; i < ncodes; i++) Codes[i].DeleteValuesArray();
		delete[] Codes;
	    Codes = NULL;
	}
	ncodes = 0;

	if (t_hTablet)
	{
	    WTClose(t_hTablet);
	}
	return true;
}

MOswitch moTablet::SetStatus(MOdevcode devcode, MOswitch state)
{
	return true;
}

//notar que el devcode es a su vez el indice dentro del array de Codes
MOswitch moTablet::GetStatus(MOdevcode devcode)
{
	return(Codes[devcode].state);
}

MOint moTablet::GetValue(MOdevcode devcode)
{
    return(Codes[devcode].value);
}

MOint moTablet::GetNValues(MOdevcode devcode) {
    return(Codes[devcode].nValues);
}

MOint moTablet::GetValue(MOdevcode devcode, MOint i)
{
    return(Codes[devcode].values[i]);
}

MOdevcode moTablet::GetCode(moText strcod)
{
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

	return(-1);//error, no encontro
}

MOint moTablet::getTabletCod(moText s)
{
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
}

void moTablet::Update(moEventList *Events)
{
	if (!t_hTablet) return;

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

		/*
		if ((TABLET_PEN_MOTION_X <= tabletcode) && (tabletcode <= TABLET_PEN_CURSOR))
			if (Pen[tabletcode].change)
			{
				Codes[tabletcode].state = true;
				Codes[tabletcode].AddValueToArray(Pen[tabletcode].value);
			}
			*/

		if (t_buttonNew > 1)
		{
			//MODebug->Push("Pressure when pressing button: " + IntToStr(t_prsNew));
			tabletcode = TABLET_PEN_BUTTON;
			Codes[tabletcode].state = true;
			Codes[tabletcode].AddValueToArray(t_buttonNew);
		}
	}
}

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

BOOL moTablet::IsTabletInstalled()
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

moText moTablet::GetTabletName()
{
	char	chrWName[50];			// String to hold window name
	WTInfo(WTI_DEVICES, DVC_NAME, chrWName);
	moText strName = chrWName;
	return strName;
}

int moTablet::MOCursorCode(int wt_code)
{
	MODebug2->Push( moText("WT cursor code: ") + IntToStr(wt_code));
	if (wt_code % 2 == 1) return MO_TABLET_CURSOR_PEN;
	else return MO_TABLET_CURSOR_ERASER;
}
