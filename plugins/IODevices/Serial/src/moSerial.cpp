/*******************************************************************************

                              moSerial.cpp

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

#include "moSerial.h"

//========================
//  Factory
//========================

moSerialFactory *m_SerialFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_SerialFactory==NULL)
		m_SerialFactory = new moSerialFactory();
	return (moIODeviceFactory*) m_SerialFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_SerialFactory;
	m_SerialFactory = NULL;
}

moIODevice*  moSerialFactory::Create() {
	return new moSerial();
}

void moSerialFactory::Destroy(moIODevice* fx) {
	delete fx;
}



// moSerial class **************************************************

moSerial::moSerial()
{
    SetName("serial");

    //---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------
		nPorts 				= 0;
		bPortsEnumerated 	= false;

		portNamesShort = new char * [MAX_SERIAL_PORTS];
		portNamesFriendly = new char * [MAX_SERIAL_PORTS];
		for (int i = 0; i < MAX_SERIAL_PORTS; i++){
			portNamesShort[i] = new char[10];
			portNamesFriendly[i] = new char[MAX_PATH];
		}
	//---------------------------------------------
	#endif
	//---------------------------------------------
	bVerbose = false;
}

moSerial::~moSerial()
{
	close();



	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------
		nPorts 				= 0;
		bPortsEnumerated 	= false;

		for (int i = 0; i < MAX_SERIAL_PORTS; i++) {
			delete [] portNamesShort[i];
			delete [] portNamesFriendly[i];
		}
		delete [] portNamesShort;
		delete [] portNamesFriendly;

	//---------------------------------------------
	#endif
	//---------------------------------------------

	bVerbose = false;
	bInited = false;

    Finish();
}

MOboolean moSerial::Init()
{
    int dev;
    MOuint i, n, n_dev, n_hosts;
    moText conf, dev_name;

    bInited = false;

    // Loading config file.
	conf = m_pResourceManager->GetDataMan()->GetDataPath() + moText("\\");
    conf += GetConfigName();
    conf += moText(".cfg");

	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		moText text = "Couldn't load serial config";
		MODebug2->Push(text);
		return false;
	}


    enumerateDevices();


	moMoldeoObject::Init();

	moDefineParamIndex( SERIAL_PORT, moText("port") );
	moDefineParamIndex( SERIAL_BAUD, moText("baud") );
	moDefineParamIndex( SERIAL_CODE, moText("code") );

    //setup(  );
    //trying to catch :
    //
    if (setup( (char*)m_Config[moText("port")][MO_SELECTED][0].Text(), m_Config[moText("baud")][MO_SELECTED][0].Int())) {

        MODebug2->Message(moText("Setting up device success!!") +  moText("port: ") + m_Config[moR(SERIAL_PORT)][MO_SELECTED][0].Text() + moText(" baud: ") + IntToStr(m_Config[moR(SERIAL_BAUD)][MO_SELECTED][0].Int()) );

    } else {
        MODebug2->Error( moText("Error in moSerial!! port: ") + m_Config[moText("port")][MO_SELECTED][0].Text() + moText(" baud: ") + IntToStr(m_Config[moText("baud")][MO_SELECTED][0].Int()));
    }


	//levantamos los codes definidos
	int coparam = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(coparam);
	int ncodes = m_Config.GetValuesCount(coparam);
	Codes = new moSerialCode [ncodes];

	for( i = 0; i < ncodes; i++) {
		m_Config.SetCurrentValueIndex(coparam,i);
		Codes[i].strcod = m_Config.GetParam().GetValue().GetSubValue(0).Text();
		Codes[i].code = i;
		//Codes[i].mousecod = getMouseCod( m_Config.GetParam().GetValue().GetSubValue(MO_MOUSE_CFG_MOUSECOD).Text() );
		//Codes[i].type = m_Config.GetParam().GetValue().GetSubValue(MO_MOUSE_CFG_TYPE).Int();
		Codes[i].state = MO_FALSE;//inicializamos en vacio
        Codes[i].value = -1;

		//if(Codes[i].mousecod==-1) {
        //    text = moText("(mouse) : no se encuentra el code: ");
        //    text += Codes[i].strcod;
		//	MODebug2->Error(text);
		//	return false;
		//}
	}

	return true;
}

MOswitch moSerial::SetStatus(MOdevcode codisp, MOswitch state)
{
    return true;
}

MOswitch moSerial::GetStatus(MOdevcode codisp)
{
    return(Codes[codisp].state);
}

MOint moSerial::GetValue(MOdevcode codisp)
{
    return(Codes[codisp].value);
}

MOdevcode
moSerial::GetCode(moText strcod) {

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
		if(!stricmp( strcod, m_Config.GetParam().GetValue().GetSubValue(0).Text() ) )
			return i;
	}

	m_Config.SetCurrentParamIndex( param );
	m_Config.SetCurrentValueIndex( param, value );

	return(-1);//error, no encontro
}

MOboolean moSerial::Finish()
{
	return true;
}

void moSerial::SendEvent(int i)
{
}


//---------------------------------------------
#ifdef TARGET_WIN32
//---------------------------------------------

//------------------------------------
   // needed for serial bus enumeration:
   //4d36e978-e325-11ce-bfc1-08002be10318}
   DEFINE_GUID (GUID_SERENUM_BUS_ENUMERATOR, 0x4D36E978, 0xE325,
   0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);
//------------------------------------

/*

http://www.openframeworks.cc/forum/viewtopic.php?p=1263&sid=2236b82f0cb97a15e7e135e02576acd9

*/

void moSerial::enumerateWin32Ports(){

	if (bPortsEnumerated == true) return;

	HDEVINFO hDevInfo = NULL;
	SP_DEVINFO_DATA DeviceInterfaceData;
	int i = 0;
	DWORD dataType, actualSize = 0;
	unsigned char dataBuf[MAX_PATH+1];

	// Reset Port List
	nPorts = 0;
	// Search device set
 	hDevInfo = SetupDiGetClassDevs((struct _GUID *)&GUID_SERENUM_BUS_ENUMERATOR,0,0,DIGCF_PRESENT);
  	if ( hDevInfo ){
		while (TRUE){
			ZeroMemory(&DeviceInterfaceData, sizeof(DeviceInterfaceData));
			DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);
			if (!SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInterfaceData)){
		    	// SetupDiEnumDeviceInfo failed
		    	break;
			}

			if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		   		&DeviceInterfaceData,
		    	SPDRP_FRIENDLYNAME,
		    	&dataType,
		    	dataBuf,
		    	sizeof(dataBuf),
		    	&actualSize)){

		    	sprintf(portNamesFriendly[nPorts], "%s", dataBuf);
				portNamesShort[nPorts][0] = 0;

				// turn blahblahblah(COM4) into COM4

                char *   begin    = NULL;
                char *   end    = NULL;
                begin          = strstr((char *)dataBuf, "COM");


                if (begin)
                    {
                    end     = strstr(begin, ")");
                    if (end) {
                          *end = 0;   // get rid of the )...
                          strcpy(portNamesShort[nPorts], begin);
                    }
                    if (nPorts++ > MAX_SERIAL_PORTS)
                            break;
                }

			}
            i++;
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);

	bPortsEnumerated = false;
}

//---------------------------------------------
#endif
//---------------------------------------------


//----------------------------------------------------------------
void moSerial::enumerateDevices(){

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	//---------------------------------------------

		//----------------------------------------------------
		//We will find serial devices by listing the directory

		DIR *dir;
		struct dirent *entry;
		dir = opendir("/dev");
		string str			= "";
		string device		= "";
		int deviceCount		= 0;

		if (dir == NULL){
			MODebug2->Message(moText("moSerial: error listing devices in /dev\n"));
		} else {
			MODebug2->Message(moText("moSerial: listing devices\n"));
			while ((entry = readdir(dir)) != NULL){
				str = (char *)entry->d_name;
				if( str.substr(0,3) == "cu." ){
					MODebug2->Message(moText("device") + IntToStr(deviceCount) + motext("-") + moText(str.c_str()));
					deviceCount++;
				}
			}
		}

	//---------------------------------------------
    #endif
    //---------------------------------------------

	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------

		enumerateWin32Ports();
		MODebug2->Message(moText("moSerial: listing devices (") + IntToStr(nPorts) + moText(" total)"));
		for (int i = 0; i < nPorts; i++){
			MODebug2->Message(moText("device ") + IntToStr(i) + moText("-- ") + moText(portNamesFriendly[i]) );
		}

	//---------------------------------------------
    #endif
    //---------------------------------------------

}

//----------------------------------------------------------------
void moSerial::close(){

	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------
		if (bInited){
			SetCommTimeouts(hComm,&oldTimeout);
			CloseHandle(hComm);
			hComm 		= INVALID_HANDLE_VALUE;
			bInited 	= false;
		}
	//---------------------------------------------
    #else
    //---------------------------------------------
    	if (bInited){
    		tcsetattr(fd,TCSANOW,&oldoptions);
    		::close(fd);
    	}
    	// [CHECK] -- anything else need to be reset?
    //---------------------------------------------
    #endif
    //---------------------------------------------

}

//----------------------------------------------------------------
bool moSerial::setup(){
	return setup(0,9600);		// the first one, at 9600 is a good choice...
}

//----------------------------------------------------------------
bool moSerial::setup(int deviceNumber, int baud){

	int deviceCount = 0;

	string str			= "";
	string device		= "";
	bool deviceFound	= false;

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	//---------------------------------------------

		//----------------------------------------------------
		//We will find serial devices by listing the directory

		DIR *dir;
		struct dirent *entry;
		dir = opendir("/dev");

		if (dir == NULL){
			printf("moSerial: error listing devices in /dev\n");
		}

		while ((entry = readdir(dir)) != NULL){
			str = (char *)entry->d_name;
			if( str.substr(0,3) == "cu." ){
				if(deviceCount == deviceNumber){
					device = "/dev/"+str;
					deviceFound = true;
					printf("moSerial device %i - /dev/%s  <--selected\n", deviceCount, str.c_str());
				}else printf("moSerial device %i - /dev/%s\n", deviceCount, str.c_str());
				deviceCount++;
			}
		}

        if(deviceFound){
            return setup(device, baud);
        }else{
            printf("moSerial: could not find device %i - only %i devices found\n", deviceNumber, deviceCount);
            return false;
        }

	//---------------------------------------------
    #endif
    //---------------------------------------------

	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------

		enumerateWin32Ports();
		if (deviceNumber < nPorts){
			device = portNamesShort[deviceNumber];
			deviceFound = true;
		}

        if(deviceFound){
            return setup(device, baud);
        }else{
            printf("moSerial: could not find device %i - only %i devices found\n", deviceNumber, nPorts);
            return false;
        }

	//---------------------------------------------
    #endif
    //---------------------------------------------


}

//----------------------------------------------------------------
bool moSerial::setup(string portName, int baud){

	bInited = false;

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	//---------------------------------------------

	    printf("moSerialInit: opening port %s @ %d bps\n", portName.c_str(), baud);
		fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
		if(fd == -1){
			printf("moSerial: unable to open port\n");
			return false;
		}

		struct termios options;
		tcgetattr(fd,&oldoptions);
		options = oldoptions;
		switch(baud){
		   case 300: 	cfsetispeed(&options,B300);
						cfsetospeed(&options,B300);
						break;
		   case 1200: 	cfsetispeed(&options,B1200);
						cfsetospeed(&options,B1200);
						break;
		   case 2400: 	cfsetispeed(&options,B2400);
						cfsetospeed(&options,B2400);
						break;
		   case 4800: 	cfsetispeed(&options,B4800);
						cfsetospeed(&options,B4800);
						break;
		   case 9600: 	cfsetispeed(&options,B9600);
						cfsetospeed(&options,B9600);
						break;
		   case 14400: 	cfsetispeed(&options,B14400);
						cfsetospeed(&options,B14400);
						break;
		   case 19200: 	cfsetispeed(&options,B19200);
						cfsetospeed(&options,B19200);
						break;
		   case 28800: 	cfsetispeed(&options,B28800);
						cfsetospeed(&options,B28800);
						break;
		   case 38400: 	cfsetispeed(&options,B38400);
						cfsetospeed(&options,B38400);
						break;
		   case 57600:  cfsetispeed(&options,B57600);
						cfsetospeed(&options,B57600);
						break;
		   case 115200: cfsetispeed(&options,B115200);
						cfsetospeed(&options,B115200);
						break;

			default:	cfsetispeed(&options,B9600);
						cfsetospeed(&options,B9600);
						printf("moSerialInit: cannot set %i baud setting baud to 9600\n", baud);
						break;
		}

		options.c_cflag |= (CLOCAL | CREAD);
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;
		tcsetattr(fd,TCSANOW,&options);

		bInited = true;
		printf("sucess in opening serial connection\n");

	    return true;
	//---------------------------------------------
    #endif
    //---------------------------------------------


    //---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------

	// open the serial port:
	// "COM4", etc...

	hComm=CreateFileA(portName.c_str(),GENERIC_READ|GENERIC_WRITE,0,0,
					OPEN_EXISTING,0,0);

	if(hComm==INVALID_HANDLE_VALUE){
		MODebug2->Message(moText("moSerial: unable to open port"));
		return false;
	}


	// now try the settings:
	COMMCONFIG cfg;
	DWORD cfgSize;
	char  buf[80];

	cfgSize=sizeof(cfg);
	GetCommConfig(hComm,&cfg,&cfgSize);
	int bps = baud;
	sprintf(buf,"baud=%d parity=N data=8 stop=1",bps);

	#if (_MSC_VER)       // microsoft visual studio
		// msvc doesn't like BuildCommDCB,
		//so we need to use this version: BuildCommDCBA
		if(!BuildCommDCBA(buf,&cfg.dcb)){
			MODebug2->Error(moText("moSerial: unable to build comm dcb; (") + moText(buf) + moText(")"));
		}
	#else
		if(!BuildCommDCB(buf,&cfg.dcb)){
			MODebug2->Error(moText("moSerial: Can't build comm dcb; ") + moText(buf) );
		}
	#endif


	// Set baudrate and bits etc.
	// Note that BuildCommDCB() clears XON/XOFF and hardware control by default

	if(!SetCommState(hComm,&cfg.dcb)){
		MODebug2->Error(moText("moSerial: Can't set comm state"));
	}
	//printf(buf,"bps=%d, xio=%d/%d\n",cfg.dcb.BaudRate,cfg.dcb.fOutX,cfg.dcb.fInX);

	// Set communication timeouts (NT)
	COMMTIMEOUTS tOut;
	GetCommTimeouts(hComm,&oldTimeout);
	tOut = oldTimeout;
	// Make timeout so that:
	// - return immediately with buffered characters
	tOut.ReadIntervalTimeout=MAXDWORD;
	tOut.ReadTotalTimeoutMultiplier=0;
	tOut.ReadTotalTimeoutConstant=0;
	SetCommTimeouts(hComm,&tOut);

	bInited = true;
	return true;
	//---------------------------------------------
	#endif
	//---------------------------------------------
}


//----------------------------------------------------------------
int moSerial::writeBytes(string str){

	if (!bInited){
		MODebug2->Error(moText("moSerial: serial not inited"));
		return 0;
	}

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	    int len = str.length();
	    int numWritten = write(fd, str.c_str(), len);
		if(numWritten == 0 && len > 0) MODebug2->Error(moText("moSerial: Can't write to com port"));
		else{
			if (bVerbose){
			 	MODebug2->Message(moText("moSerial: numWritten ") + IntToStr(numWritten) );
	    	}
	    }

	    return numWritten;
    #endif
    //---------------------------------------------

    //---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD written;
		int len = (int)str.length();
		if(!WriteFile(hComm, str.c_str(), len, &written,0)){
			 MODebug2->Error(moText("moSerial: Can't write to com port"));
			 return 0;
		}else{
			if (bVerbose){
                MODebug2->Message(moText("moSerial: numWritten") + IntToStr((int)written));
	    	}
	    }
		return (int)written;
	#endif
	//---------------------------------------------

}

//----------------------------------------------------------------
string moSerial::readBytes(int length){

	if (!bInited){
		MODebug2->Error(moText("moSerial: serial not inited"));
		return "";
	}

	char * tmpBuff = new char[length];
	memset(tmpBuff, 0, length);
	string str = "";

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		int nRead = read(fd, tmpBuff,length);
		if(nRead == 0) MODebug2->Error(moText("moSerial: trouble reading from port"?);

    #endif
    //---------------------------------------------

    //---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD nRead;
		if (!ReadFile(hComm,tmpBuff,length,&nRead,0)){
		MODebug2->Error(moText("moSerial: trouble reading from port"));
		}
	#endif
	//---------------------------------------------

	str = tmpBuff;
	delete tmpBuff;
	return str;
}

//----------------------------------------------------------------
bool moSerial::writeByte(unsigned char singleByte){

	if (!bInited){
		MODebug2->Error(moText("moSerial: serial not inited"));
		return 0;
	}

	unsigned char tmpByte[10];
	tmpByte[0] = singleByte;

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	    int len = 1;
	    int numWritten = 0;
	    numWritten = write(fd, tmpByte, len);
		if(numWritten == 0 && len > 0){
			 MODebug2->Error(moText("moSerial: Can't write to com port"));
		} else{
			if (bVerbose){
			 	MODebug2->Message(moText("moSerial: written byte "));
	    	}
	    }
		return (numWritten > 0 ? true : false);
    #endif
    //---------------------------------------------

    //---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD written = 0;
		int len = 1;
		if(!WriteFile(hComm, tmpByte, len, &written,0)){
			 MODebug2->Error(moText("moSerial: Can't write to com port"));
			 return 0;
		} else{
			if (bVerbose){
			 	MODebug2->Message(moText("moSerial: written byte "));
	    	}
	    }
		return ((int)written > 0 ? true : false);
	#endif
	//---------------------------------------------

}

//----------------------------------------------------------------
unsigned char moSerial::readByte(){

	if (!bInited){
		MODebug2->Error(moText("moSerial: serial not inited"));
		return 0;
	}

	unsigned char tmpByte[10];

	memset( (void*)&tmpByte, 0, 10 );

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		int nRead = read(fd, tmpByte, 1);
		if(nRead == 0) MODebug2->Error(moText("moSerial: trouble reading from port"));

    #endif
    //---------------------------------------------

    //---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD nRead;
		if (!ReadFile(hComm, tmpByte, 1, &nRead, 0)){
		MODebug2->Error(moText("moSerial: trouble reading from port"));
		}
	#endif
	//---------------------------------------------

	return tmpByte[0];
}

void
moSerial::Update(moEventList *Events) {
	MOuint i;
	moEvent *actual,*tmp;

	actual = Events->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	while(actual!=NULL) {
		//solo nos interesan los del serial q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == this->GetId()) {
			//ya usado lo borramos de la lista
			tmp = actual->next;
			Events->Delete(actual);
			actual = tmp;
		} else actual = actual->next;//no es nuestro pasamos al next
	}



	//m_Codes
/*
	actual = Events->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	MOint tempval;
	while(actual!=NULL) {
		//solo nos interesan los del serial q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == this->GetId()) {

			moMidiDataCode pcode = m_Codes.Get( actual->reservedvalue1 );
            pcode.mididata.m_Val = actual->reservedvalue2;
			m_Codes.Set( actual->reservedvalue1, pcode );

			actual = actual->next;
		} else actual = actual->next;//no es nuestro pasamos al next
	}
*/
    MOuchar readchar;

    if (bInited) {

        int nbyte = 0;
        int nbytes = 10;
        bool start = true;

        //reset every bytes to state false
        for(int i=0; i<nbytes; i++) {
            Codes[nbyte].state = MO_FALSE;
            Codes[nbyte].value = 0;
        }

        while (nbyte<nbytes) {
            readchar = readByte();
            if (readchar!='\0') {
                if (start) { MODebug2->Message( moText("readchar:")); start = false; }
                MODebug2->Message( moText(" byte ") + IntToStr(nbyte) + moText(":") + IntToStr(readchar) );
                Codes[nbyte].state = MO_TRUE;
                Codes[nbyte].value = (int) readchar;

            }
            nbyte++;
        }
    }

}

moConfigDefinition *
moSerial::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moIODevice::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("port"), MO_PARAM_TEXT, SERIAL_PORT, moValue( "COM4", "TXT") );
	p_configdefinition->Add( moText("baud"), MO_PARAM_NUMERIC, SERIAL_BAUD, moValue( "9600", "NUM") );
	p_configdefinition->Add( moText("code"), MO_PARAM_TEXT, SERIAL_CODE );
	return p_configdefinition;
}
