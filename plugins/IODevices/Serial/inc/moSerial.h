/*******************************************************************************

                              moSerial.h

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

#ifndef __MO_SERIAL_H__
#define __MO_SERIAL_H__

#include "moTypes.h"
#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"

#ifdef MO_WIN32
    #undef TARGET_LINUX
    #define TARGET_WIN32 1
#else
    #define TARGET_LINUX 1
    #undef TARGET_WIN32
#endif


#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	#include <termios.h>
	#include <sys/ioctl.h>
	#include <getopt.h>
	#include <dirent.h>
#else
	#include <Winbase.h>
	#include <tchar.h>
	#include <iostream>
	#include <string.h>
	#include <setupapi.h>
	#include <regstr.h>
	#define MAX_SERIAL_PORTS 256

	#ifdef __MINGW32__
			#define INITGUID
			#include <initguid.h> // needed for dev-c++ & DEFINE_GUID
    #endif




#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

// moSerial class **************************************************

class moSerial : public moIODevice
{
    public:
        moSerial();
        virtual ~moSerial();

        void Update(moEventList*);
        MOboolean Init();
        MOswitch GetStatus(MOdevcode);
        MOswitch SetStatus( MOdevcode,MOswitch);
        MOint GetValue(MOdevcode);
        MOdevcode GetCode( moText);
        MOboolean Finish();

    private:

        void 			enumerateDevices();

        void 			close();
        bool			setup();	// use default port, baud (0,9600)
        bool			setup(string portName, int baudrate);
        bool			setup(int deviceNumber, int baudrate);
        int				writeBytes(string str);
        string			readBytes(int length);
        bool			writeByte(unsigned char singleByte);
        unsigned char	readByte();

        bool 			bVerbose;
        void 			setVerbose(bool bLoudmouth) { bVerbose = bLoudmouth; };



    protected:

        bool 	bInited;

        #ifdef TARGET_WIN32

            char 		** portNamesShort;//[MAX_SERIAL_PORTS];
            char 		** portNamesFriendly; ///[MAX_SERIAL_PORTS];
            HANDLE  	hComm;		// the handle to the serial port pc
            int	 		nPorts;
            bool 		bPortsEnumerated;
            void 		enumerateWin32Ports();
            COMMTIMEOUTS 	oldTimeout;	// we alter this, so keep a record

        #else
            int 		fd;			// the handle to the serial port mac
            struct 	termios oldoptions;
        #endif

    void SendEvent(int i);
};

class moSerialFactory : public moIODeviceFactory {

    public:
        moSerialFactory() {}
        virtual ~moSerialFactory() {}
        moIODevice* Create();
        void Destroy(moIODevice* fx);

};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif
