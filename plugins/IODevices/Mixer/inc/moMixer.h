/*******************************************************************************

                                moMixer.h

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
// Dispositivo de Keydo y Mouse //

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"
#include "moText.h"

#ifndef _MO_MIXER_H
#define _MO_MIXER_H

class moMixerButton {
public:
	MOboolean pushed; 
	MOint pushedmod;
	MOboolean released;
	MOint releasedmod;
	MOint pressed;
	MOint pressedmod;

	moMixerButton() {
		pushed = MO_FALSE;
		pushedmod = 0;
		released = MO_FALSE;
		releasedmod = 0;
		pressed = 0;
		pressedmod = 0;
	}

	MOboolean Init() {
		pushed = MO_FALSE;
		pushedmod = 0;
		released = MO_FALSE;
		releasedmod = 0;
		pressed = 0;
		pressedmod = 0;
		return true;
	}

	void Push(MOint mod) {
		pushed = MO_TRUE;
		pushedmod =  mod;
		released = MO_FALSE;
		releasedmod = 0;//indefinido	
		pressedmod = 0;
	}

	MOint Release(MOint mod) {
		released = MO_TRUE;
		releasedmod = mod;
		//se soltó pero nunca se pulsó(es un error) o no coincide el mapa de keys especiales
		if((pushed == MO_FALSE) ||(releasedmod!=pushedmod)) {
			Init();			
			return MO_FALSE;
		} else {
			pressed = pressed + 1;
			pressedmod = mod;
			pushed = MO_FALSE;
			pushedmod = 0;
			released = MO_FALSE;
			releasedmod = 0;
			return MO_TRUE;
		}
	}
};

class moMixerWheel {
public:
	MOboolean motion;//verdadero si se movio
	MOint rel;

	moMixerWheel() {
		motion = MO_FALSE;
		rel = 0;
	}

	MOboolean Init() {
		motion = MO_FALSE;
		rel = 0;
		return true;
	}

	void Motion(MOint r) {
		if(r!=0) {
			rel = r;
			motion = MO_TRUE;
		}
	}
};

class moMixerLed {
public:
	MOint led;

	moMixerLed() {
		led = 0;
	}

	MOboolean Init() {
		led = 0;
		return true;
	}

	void Set(MOint r) {
		if(r!=0) {
			led = r;			
		}
	}
};

class moMixerText {
public:
	char n;
	moText text;

	moMixerText() {
		text = "";
		n = 0;
	}

	MOboolean Init() {
		text = "";
		n = 0;
		return true;
	}

	void Set(char *tt) {
		text = tt;
		n = strlen(tt);
	}
};


class moMixerCode {
public:
	moText strcod;
	MOint devcode;
	MOint mixercode;//code interno de cada disp  fisico
	MOint type;//pushedr:0 ,prendedor:1, selector:2, seteador:3,...otros
	MOint especiales;//keys especiales, como alt, caps lock, etc...
	MOint state;//0 MO_FALSE(no esta activo el device) MO_ON(está on) MO_OFF(está off)
	MOint value;//depende
	moText text;//para el text de los params
};

class moMixer : public moIODevice {
public:	
    moMixer();
    virtual ~moMixer();
	virtual MOboolean		Init();
	virtual MOboolean		Finish();
    
    void			Update(moEventList*);	
    MOswitch		GetStatus( MOdevcode);
    MOswitch		SetStatus( MOdevcode,MOswitch);
	MOint			GetValue( MOdevcode);
	char*			getText( MOdevcode);	
    MOdevcode    GetCode( moText);   
	

private:
	MOint	getMixerCode(moText);    
	moConfig config;

	//el indice esta definido en el archivo de configuracion()
	moMixerButton	Buttons[512];
	moMixerWheel	Wheels[512];
	moMixerLed		Leds[512];
	moMixerText	Texts[512];
	moMixerWheel	MidiControllers[512];

	moMixerCode *Codes;
	MOuint ncodes;

	//BUFFER DE CARACTERES
	MOint nchars;
	MOint Text[256];
	MOswitch TextActive;

};

/*
	//MOUSE
	MOswitch EventsMouse[16];
	MOswitch ButtonsMouse[16];	
	MOint MouseMotionX,MouseMotionY;//entre-255 y 255
	MOfloat MouseDefinition;
*/

class moMixerFactory : public moIODeviceFactory {

public:
    moMixerFactory() {}
    virtual ~moMixerFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}

#endif

