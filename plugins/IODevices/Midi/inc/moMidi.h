/*******************************************************************************

                                moMidi.h

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
  Andr� Colubri

  Class:
  moMidi

  Description:
  Motion Analyzer


*******************************************************************************/

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"
#include "moVideoGraph.h"
#include "moArray.h"
//#include "moLive.h"
#include "moShaderGLSL.h"

#ifdef WIN32
	//#include "setupapi.h"
	//#include "hidsdi.h"
	#include "mmsystem.h"
#endif

#ifndef __MO_MIDIDEVICES_H
#define __MO_MIDIDEVICES_H

#define	MOMIDI_CFG_STRCOD			0
#define	MOMIDI_CFG_ROTARYBUTTON		1
#define	MOMIDI_CFG_MIDICHANNEL		2
#define	MOMIDI_CFG_MIDICC			3

#define	MO_MIDI_SYTEM_LABELNAME		0
#define	MO_MIDI_SYSTEM_ON			1

enum moMidiParamIndex {

  MIDI_DEVICE,
  MIDI_CODES

};

enum moEncoderType {
	MOMIDI_ROTARY=0,
	MOMIDI_PUSHBUTTON=1,
	MOMIDI_FADER=2
};

class moMidiData {
	public:
		moMidiData();
		virtual ~moMidiData();

		moMidiData& operator = ( const moMidiData& mididata);
		void Copy( const moMidiData& mididata );


		moEncoderType	m_Type;
		MOint	m_Channel;
		MOint	m_CC;
		MOint	m_Val;

};

template class moDynamicArray<moMidiData>;
typedef  moDynamicArray<moMidiData> moMidiDatas;

class moMidiDataCode {
	public:
		moText		strcode;
		MOint		devcode;
		MOint		state;
		moMidiData	mididata;

};
template class moDynamicArray<moMidiDataCode>;
typedef  moDynamicArray<moMidiDataCode> moMidiDataCodes;


class moMidiDevice : /*public moThread,*/ public moAbstract {

	public:

		moMidiDevice();
		virtual ~moMidiDevice();

		void SetName( moText p_name) {
			m_Name = p_name;
		}
		void SetActive( MOboolean p_active) {
			m_bActive = p_active;
		}

		MOboolean		Init();
		MOboolean		Init( moText devicetext );
		MOboolean		Finish();

		MOboolean			IsInit() {
			return m_bInit;
		}
		moText GetName() {
			return m_Name;
		}

		MOboolean IsActive() {
			return m_bActive;
		}

		void				NewData( moMidiData p_mididata );
		moMidiData*			GetData();
		void Update(moEventList *Events);
	//
	#ifdef WIN32
	static void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	#else
	#endif
	void PrintMidiInErrorMsg(unsigned long err);

	//=============================================================================
	/*	A buffer to hold incoming System Exclusive bytes. I arbitrarily make this 256 bytes. Note:
		For Win 3.1 16-bit apps, this buffer should be allocated using GlobalAlloc with the
		GMEM_MOVEABLE flag to get a handle of the memory object. Then pass this handle to the
		GlobalLock function to get a pointer to the memory object. To free a data block, use
		GlobalUnlock and GlobalFree. But Win32 doesn't appear to have this limitation.
	*/
	unsigned char SysXBuffer[256];

	/* A flag to indicate whether I'm currently receiving a SysX message */
	unsigned char SysXFlag;

	protected:

		moLock			m_lock;
		moMidiDatas		m_MidiDatas;
		MOint			m_DeviceId;
		moText			m_Name;
		MOboolean		m_bActive;
		MOboolean		m_bInit;

		moDataMessage   m_DataMessage;

};


typedef moMidiDevice* moMidiDevicePtr;

template class moDynamicArray<moMidiDevicePtr>;
typedef  moDynamicArray<moMidiDevicePtr> moMidiDevicePtrs;

class moMidi : public moIODevice
{
public:
    moMidi();
    ~moMidi();

    void Update(moEventList*);
    MOboolean Init();
    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
	void SetValue( MOdevcode cd, MOint vl );
	void SetValue( MOdevcode cd, MOfloat vl );

    moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

    MOint GetValue(MOdevcode);
	MOpointer GetPointer(MOdevcode devcode );
    MOdevcode GetCode( moText);
    MOboolean Finish();

private:
    moConfig config;

    moEventList *events;

protected:

	MOint	mididevices;
   	moMidiDevicePtrs		m_MidiDevices;
	moMidiDataCodes			m_Codes;

};


class moMidiFactory : public moIODeviceFactory {

    public:
        moMidiFactory() {}
        virtual ~moMidiFactory() {}
        moIODevice* Create();
        void Destroy(moIODevice* fx);

};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif
