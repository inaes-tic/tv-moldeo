/*******************************************************************************

                              moMidi.cpp

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

#include "moMidi.h"
#include <errno.h>

#ifdef WIN32
#include <conio.h>
#endif

//========================
//  Factory
//========================

moMidiFactory *m_MidiFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_MidiFactory==NULL)
		m_MidiFactory = new moMidiFactory();
	return (moIODeviceFactory*) m_MidiFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_MidiFactory;
	m_MidiFactory = NULL;
}

moIODevice*  moMidiFactory::Create() {
	return new moMidi();
}

void moMidiFactory::Destroy(moIODevice* fx) {
	delete fx;
}

//========================
//  class
//========================


		moMidiData::moMidiData() {
		    	m_Type = MOMIDI_ROTARY;
                m_Channel = 0;
                m_CC = 0;
                m_Val = 0;
		}

		moMidiData::~moMidiData() {

        }

moMidiData& moMidiData::operator =( const moMidiData& mididata)
{
	m_Type = mididata.m_Type;
	m_Channel = mididata.m_Channel;
	m_CC = mididata.m_CC;
	m_Val = mididata.m_Val;
    return *this;
}

void
moMidiData::Copy( const moMidiData& mididata ) {
    (*this) = mididata;
}

moMidiDevice::moMidiDevice() {
	SysXFlag = 0;
	m_DeviceId = -1;
	m_bInit = false;
	m_bActive = false;
}


moMidiDevice::~moMidiDevice() {
	m_MidiDatas.Empty();
}

void moMidiDevice::PrintMidiInErrorMsg(unsigned long err)
{
#ifdef WIN32
#define BUFFERSIZE 200
	char	buffer[BUFFERSIZE];

	if (!(err = midiInGetErrorText(err, &buffer[0], BUFFERSIZE)))
	{
		printf("%s\r\n", &buffer[0]);
	}
	else if (err == MMSYSERR_BADERRNUM)
	{
		printf("Strange error number returned!\r\n");
	}
	else if (err == MMSYSERR_INVALPARAM)
	{
		printf("Specified pointer is invalid!\r\n");
	}
	else
	{
		printf("Unable to allocate/lock memory!\r\n");
	}
#endif
}

#ifdef WIN32
void moMidiDevice::midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	LPMIDIHDR		lpMIDIHeader;
	unsigned char *	ptr;
	TCHAR			buffer[80];
	unsigned char 	bytes;
	moMidiDevice*	pMidiDevice = NULL;
	pMidiDevice = (moMidiDevice*)dwInstance;

	/* Determine why Windows called me */
	switch (uMsg)
	{
		/* Received some regular MIDI message */
		case MIM_DATA:
		{
			/* Display the time stamp, and the bytes. (Note: I always display 3 bytes even for
			Midi messages that have less) */
			sprintf(&buffer[0], "0x%08X 0x%02X 0x%02X 0x%02X\0", dwParam2, dwParam1 & 0x000000FF, (dwParam1>>8) & 0x000000FF, (dwParam1>>16) & 0x000000FF);

			_cputs(&buffer[0]);


			moMidiData		mididata;

			mididata.m_Type = MOMIDI_ROTARY;//????
			mididata.m_Channel = (dwParam1 & 0x000000FF) - 175;//channel 1 = 0xB0
			mididata.m_CC = (dwParam1>>8) & 0x000000FF;
			mididata.m_Val = (dwParam1>>16) & 0x000000FF;

			if (pMidiDevice) pMidiDevice->NewData( mididata );

			break;
		}

		/* Received all or part of some System Exclusive message */
		case MIM_LONGDATA:
		{
			/* If this application is ready to close down, then don't midiInAddBuffer() again */
			if (!(pMidiDevice->SysXFlag & 0x80))
			{
				/*	Assign address of MIDIHDR to a LPMIDIHDR variable. Makes it easier to access the
					field that contains the pointer to our block of MIDI events */
				lpMIDIHeader = (LPMIDIHDR)dwParam1;

				/* Get address of the MIDI event that caused this call */
				ptr = (unsigned char *)(lpMIDIHeader->lpData);

				/* Is this the first block of System Exclusive bytes? */
				if (!pMidiDevice->SysXFlag)
				{
					/* Print out a noticeable heading as well as the timestamp of the first block.
						(But note that other, subsequent blocks will have their own time stamps). */
					printf("*************** System Exclusive **************\r\n0x%08X ", dwParam2);

					/* Indicate we've begun handling a particular System Exclusive message */
					pMidiDevice->SysXFlag |= 0x01;
				}

				/* Is this the last block (ie, the end of System Exclusive byte is here in the buffer)? */
				if (*(ptr + (lpMIDIHeader->dwBytesRecorded - 1)) == 0xF7)
				{
					/* Indicate we're done handling this particular System Exclusive message */
					pMidiDevice->SysXFlag &= (~0x01);
				}

				/* Display the bytes -- 16 per line */
				bytes = 16;

				while((lpMIDIHeader->dwBytesRecorded--))
				{
					if (!(--bytes))
					{
						sprintf(&buffer[0], "0x%02X\r\n", *(ptr)++);
						bytes = 16;
					}
					else
						sprintf(&buffer[0], "0x%02X ", *(ptr)++);

					_cputs(&buffer[0]);
				}

				/* Was this the last block of System Exclusive bytes? */
				if (!pMidiDevice->SysXFlag)
				{
					/* Print out a noticeable ending */
					_cputs("\r\n******************************************\r\n");
				}

				/* Queue the MIDIHDR for more input */
				midiInAddBuffer(handle, lpMIDIHeader, sizeof(MIDIHDR));
			}

			break;
		}

		/* Process these messages if you desire */
/*
		case MIM_OPEN:
        case MIM_CLOSE:
        case MIM_ERROR:
        case MIM_LONGERROR:
        case MIM_MOREDATA:

			break;
*/
  }
}
#endif

MOboolean
moMidiDevice::Init() {
	return true;
}

MOboolean
moMidiDevice::Finish() {
	return true;
}

MOboolean
moMidiDevice::Init( moText devicetext ) {

	SetName(devicetext);


#ifdef WIN32
/*
	BOOL		result;
	GUID		hidGUID;
	HDEVINFO	hardwareDeviceInfoSet;
	SP_DEVICE_INTERFACE_DATA	deviceInterfaceData;
	DWORD Index = 0;
	DWORD requiredSize;

	//Get the HID GUID value - used as mask to get list of devices
	HidD_GetHidGuid ( &hidGUID );

	//Get a list of devices matching the criteria (hid interface, present)
	hardwareDeviceInfoSet = SetupDiGetClassDevs (&hidGUID,
					NULL, // Define no enumerator (global)
					NULL, // Define no
					(DIGCF_PRESENT | DIGCF_ALLCLASSES |DIGCF_DEVICEINTERFACE));


	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	//Go through the list and get the interface data
	for(int i = 0; i < 10; i++)
	{
		result = SetupDiEnumDeviceInterfaces (hardwareDeviceInfoSet,
					NULL, //infoData,
					&hidGUID, //interfaceClassGuid,
					Index,
					&deviceInterfaceData);
		if (result == FALSE) {
			Index++;
		}
	}

	// Failed to get a device - possibly the index is larger than the number of devices
	if (result == FALSE) {
		SetupDiDestroyDeviceInfoList (hardwareDeviceInfoSet);
		return;// INVALID_HANDLE_VALUE;
	}

	//Get the details with null values to get the required size of the buffer
	SetupDiGetDeviceInterfaceDetail (hardwareDeviceInfoSet,
				&deviceInterfaceData,
				NULL, //interfaceDetail,
				0, //interfaceDetailSize,
				&requiredSize,
				0); //infoData))
*/

	MIDIINCAPS     moc;
	unsigned long   iNumDevs, i;



	/* Get the number of MIDI Out devices in this computer */
	iNumDevs = midiInGetNumDevs();

	if (iNumDevs==0) {
	    MODebug2->Message( moText("ERROR! NO MIDI DEVICES FOUND"));
    }

	/* Go through all of those devices, displaying their names */
	for (i = 0; i < iNumDevs; i++)
	{
		/* Get info about the next device */
		if (!midiInGetDevCaps(i, &moc, sizeof(MIDIINCAPS)))
		{
			/* Display its Device ID and name */
			MODebug2->Message( moText("Device ID #") + IntToStr(i) + moText(":") + moText(moc.szPname));
			if ( !stricmp(moc.szPname, devicetext) ) {
				m_DeviceId = i;
				break;
			}
		}
	}


	/*
	unsigned long result;
	HMIDIOUT      outHandle;

	// Open the MIDI Mapper
	result = midiOutOpen(&outHandle, i, 0, 0, CALLBACK_WINDOW);
	if (!result)
	{
		// Output the C note (ie, sound the note)
		midiOutShortMsg(outHandle, 0x00403C90);

		// Output the E note
		midiOutShortMsg(outHandle, 0x00404090);

		// Output the G note
		midiOutShortMsg(outHandle, 0x00404390);

		// Here you should insert a delay so that you can hear the notes sounding
		Sleep(1000);

		// Now let's turn off those 3 notes
		midiOutShortMsg(outHandle, 0x00003C90);
		midiOutShortMsg(outHandle, 0x00004090);
		midiOutShortMsg(outHandle, 0x00004390);

		 // Close the MIDI device
		 midiOutClose(outHandle);
	} else {
		printf("There was an error opening MIDI Mapper!\r\n");
	}
	*/
	HMIDIIN			handle;
	MIDIHDR			midiHdr;
	unsigned long	err;

	if (m_DeviceId!=-1) {
		/* Open default MIDI In device */
		if (!(err = midiInOpen(&handle, m_DeviceId, (DWORD)midiCallback, (DWORD)this, CALLBACK_FUNCTION)))
		{
			// Store pointer to our input buffer for System Exclusive messages in MIDIHDR
			midiHdr.lpData = (LPSTR)&SysXBuffer[0];

			// Store its size in the MIDIHDR
			midiHdr.dwBufferLength = sizeof(SysXBuffer);

			// Flags must be set to 0
			midiHdr.dwFlags = 0;

			// Prepare the buffer and MIDIHDR
			err = midiInPrepareHeader(handle, &midiHdr, sizeof(MIDIHDR));
			if (!err)
			{
				// Queue MIDI input buffer
				err = midiInAddBuffer(handle, &midiHdr, sizeof(MIDIHDR));
				if (!err)
				{
					// Start recording Midi
					err = midiInStart(handle);
					m_bInit = true;
					return true;
					/*
					if (!err)
					{
						// Wait for user to abort recording
						printf("Press any key to stop recording...\r\n\n");
						_getch();

						// We need to set a flag to tell our callback midiCallback()
						// not to do any more midiInAddBuffer(), because when we
						// call midiInReset() below, Windows will send a final
						// MIM_LONGDATA message to that callback. If we were to
						// allow midiCallback() to midiInAddBuffer() again, we'd
						// never get the driver to finish with our midiHdr

						SysXFlag |= 0x80;
						printf("\r\nRecording stopped!\n");
					}*/

					/* Stop recording */
					//midiInReset(handle);
				}
			}
	/*
			// If there was an error above, then print a message
			if (err) PrintMidiInErrorMsg(err);

			// Close the MIDI In device
			while ((err = midiInClose(handle)) == MIDIERR_STILLPLAYING) Sleep(0);
			if (err) PrintMidiInErrorMsg(err);

			// Unprepare the buffer and MIDIHDR. Unpreparing a buffer that has not been prepared is ok
			midiInUnprepareHeader(handle, &midiHdr, sizeof(MIDIHDR));
			*/
		}
		else
		{
			printf("Error opening the default MIDI In Device!\r\n");
			PrintMidiInErrorMsg(err);
			return false;
		}
	}

#else

#endif
		return false;
}


void
moMidiDevice::NewData( moMidiData p_mididata ) {

	//m_lock.Lock();
	/*if (m_DataMessage.Count()>127) m_DataMessage.Empty();
	//p_mididata.m_CC = 0;
	moData pData;
	pData.SetInt( p_mididata.m_Val );
	m_DataMessage.Add( pData );
	*/

	if (m_MidiDatas.Count()>127) m_MidiDatas.Empty();
	m_MidiDatas.Add(p_mididata);



	//m_lock.Unlock();


}

void
moMidiDevice::Update(moEventList *Events) {

	MOint i;
	moMidiData	mididata;

	//m_lock.Lock();


	for( i=0; i < m_MidiDatas.Count(); i++ ) {

		mididata = m_MidiDatas.Get( i );

		MODebug2->Push(moText("MIDI Data CC:") + IntToStr(mididata.m_CC) + moText(" val:") + IntToStr(mididata.m_Val) );

		Events->Add( MO_IODEVICE_MIDI, (MOint)(mididata.m_Type), mididata.m_Channel, mididata.m_CC, mididata.m_Val );

	}
	m_MidiDatas.Empty();


	//mididata = m_MidiDatas.Get( m_MidiDatas.Count()-1 );
	//moData pData = m_DataMessage.Get(m_DataMessage.Count()-1);
	//MODebug2->Push(moText("MIDI n:") + IntToStr(m_DataMessage.Count()) + moText(" code:") + IntToStr(pData.Int()) );
    //m_DataMessage.Empty();

	//m_lock.Unlock();

}

//=============================================================================



//=============================================================================

moMidi::moMidi() {
	SetName("midi");
}

moMidi::~moMidi() {
	Finish();
}

MOboolean
moMidi::Init() {

	moText conf;
	MOint i;

	// Loading config file.
	//levantamos el config del keyboard
	conf = m_pResourceManager->GetDataMan()->GetDataPath()+moText("/");
    conf += GetConfigName();
    conf += moText(".cfg");
	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		moText text = "Couldn't load midi config";
		MODebug2->Error(text);
		return false;
	}

	moMoldeoObject::Init();

	moDefineParamIndex( MIDI_DEVICE, moText("mididevice") );


	mididevices = m_Config.GetParamIndex("mididevice");

	MOint nvalues = m_Config.GetValuesCount( mididevices );
	m_Config.SetCurrentParamIndex( mididevices );

	/*
        MO_MIDI_SYTEM_LABELNAME	0
        MO_MIDI_SYSTEM_ON 1
	*/
	for( int i = 0; i < nvalues; i++) {

		m_Config.SetCurrentValueIndex( mididevices, i );

		moMidiDevicePtr pDevice = NULL;
		pDevice = new moMidiDevice();

		if (pDevice!=NULL) {
			pDevice->MODebug = MODebug;
			moText MidiDeviceCode = m_Config.GetParam().GetValue().GetSubValue(MO_MIDI_SYTEM_LABELNAME).Text();
			if ( pDevice->Init( MidiDeviceCode ) ) {
				pDevice->SetActive( m_Config.GetParam().GetValue().GetSubValue(MO_MIDI_SYSTEM_ON).Int() );
			} else {
				MODebug2->Error( moText("Midi Device not found: ") + (moText)MidiDeviceCode );
			}
		}

		m_MidiDevices.Add( pDevice );

	}



	//levantamos los codes definidos
	MOint coparam = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(coparam);
	MOint ncodes = m_Config.GetValuesCount(coparam);

	m_Config.FirstValue();

	printf("\nMIDI: ncodes:%i\n",ncodes);

	for( i = 0; i < ncodes; i++) {
		moMidiDataCode mididatacode;
		mididatacode.strcode = m_Config.GetParam().GetValue().GetSubValue(0).Text();
		mididatacode.devcode = i;
		mididatacode.mididata.m_Type = (moEncoderType)m_Config.GetParam().GetValue().GetSubValue(1).Int();
		mididatacode.mididata.m_Channel = m_Config.GetParam().GetValue().GetSubValue(2).Int();
		mididatacode.mididata.m_CC = m_Config.GetParam().GetValue().GetSubValue(3).Int();
		mididatacode.mididata.m_Val = 0;
		mididatacode.state = MO_FALSE;
/*

		Codes[i].value = MO_FALSE;
		*/
		m_Codes.Add( mididatacode );
		m_Config.NextValue();
	}
	return true;
}


void
moMidi::SetValue( MOdevcode cd, MOint vl ) {
    m_Codes[cd].mididata.m_Val = (int)vl;
}

void
moMidi::SetValue( MOdevcode cd, MOfloat vl ) {
    m_Codes[cd].mididata.m_Val = (int)vl;
}


MOswitch
moMidi::SetStatus(MOdevcode devcode, MOswitch state) {

    m_Codes[devcode].state = state;

	return true;
}


//notar que el devcode es a su vez el indice dentro del array de Codes
MOswitch
moMidi::GetStatus(MOdevcode devcode) {
	return(m_Codes[devcode].state);
}

MOint
moMidi::GetValue( MOdevcode devcode) {
	return(m_Codes[devcode].mididata.m_Val);
}


MOdevcode
moMidi::GetCode(moText strcod) {

	MOint codi,i;

	codi = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(codi);
	m_Config.FirstValue();

	for( i = 0; i < m_Config.GetValuesCount(codi); i++) {
		if(!stricmp( strcod, m_Config.GetParam().GetValue().GetSubValue(MOMIDI_CFG_STRCOD).Text() ))
			return(i);
		m_Config.NextValue();
	}

	return(-1);//error, no encontro
}

MOpointer
moMidi::GetPointer(MOdevcode devcode ) {

	return NULL;
}

void
moMidi::Update(moEventList *Events) {
	MOuint i;
	moEvent *actual,*tmp;

	actual = Events->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	while(actual!=NULL) {
		//solo nos interesan los del midi q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == MO_IODEVICE_MIDI) {
			//ya usado lo borramos de la lista
			tmp = actual->next;
			Events->Delete(actual);
			actual = tmp;
		} else actual = actual->next;//no es nuestro pasamos al next
	}


    ///ACTUALIZAMOS CADA DISPOSITIVO
	for( i = 0; i < m_MidiDevices.Count(); i++ ) {

		moMidiDevicePtr MidiDevPtr;
		MidiDevPtr = m_MidiDevices.Get(i);

		if (MidiDevPtr!=NULL) {
			if (MidiDevPtr->IsInit()) {
				MidiDevPtr->Update( Events );
			}
		}


	}

	//m_Codes

	actual = Events->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	MOint tempval;
	while(actual!=NULL) {
		//solo nos interesan los del midi q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == MO_IODEVICE_MIDI) {

		    ///actual->reservedvalue1 corresponds to CC midi code : it works as a n index in m_Codes (has to be defined in param "code" in the config file...
		    ///actual->reservedvalue2 corresponds to VAL

			moMidiDataCode pcode = m_Codes.Get( actual->reservedvalue1 );

			//tempval = actual->reservedvalue2;
			//calculamos la diferencia y modificamos el valor del evento...
			//actual->reservedvalue2 = (tempval - pcode.mididata.m_Val) * 8;
			//actual->reservedvalue2 = ( tempval - 64)*4;
			//guardamos el valor actual para calcular la proxima
			pcode.mididata.m_Val = actual->reservedvalue2;
			m_Codes.Set( actual->reservedvalue1, pcode );
			if (actual->reservedvalue2>0) SetStatus( actual->reservedvalue1, MO_ON );
			else  SetStatus( actual->reservedvalue1, MO_OFF );

			actual = actual->next;
		} else actual = actual->next;//no es nuestro pasamos al next
	}



}

moConfigDefinition *
moMidi::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moIODevice::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("mididevice"), MO_PARAM_TEXT, MIDI_DEVICE, moValue( "BCR2000[2]", "TXT") );
	return p_configdefinition;
}


MOboolean
moMidi::Finish() {
	return true;
}
