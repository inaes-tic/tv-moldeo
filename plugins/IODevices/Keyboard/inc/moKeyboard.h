/*******************************************************************************

                                moKeyboard.h

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
#include "moIODevicePlugin.h"

#ifndef _MO_KEYBOARD_H
#define _MO_KEYBOARD_H

class  moKey {
public:
	MOboolean pushed; 
	MOint pushedmod;
	MOboolean released;
	MOint releasedmod;
	MOint pressed;
	MOint pressedmod;

	moKey() {
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

//a cada ciclo se actualiza
class  moKeyCode {
public:
	moText strcod;
	MOint devcode;
	MOint key;//mapeo type SDL
	MOint type;//apretada:0; botoneada(sin keys especiales):1;botoneada(con SHIFT);botoneada(con CTRL):2;
	MOint state;//0 MO_FALSE(no esta activo el device, o no sucedio ningun evento nuevo) MO_ON(está on) MO_OFF(está off)
	MOint value;//
};

class  moKeyboard : public moIODevice {
public:	
    moKeyboard();
    virtual ~moKeyboard();
    
    void			Update(moEventList*);
	MOboolean		Init();
    MOswitch		GetStatus( MOdevcode);
    MOswitch		SetStatus( MOdevcode,MOswitch);
	MOint			GetValue( MOdevcode);
    MOdevcode    GetCode( moText);
	MOint			getKey( moText);
	MOboolean		Finish();
	MOswitch		getSpecials(MOint iii);
	

private:
    moConfig config;

	moKey Keys[1024];
	MOswitch Specials[16];

	moKeyCode *Codes;
	MOuint ncodes;

	//BUFFER DE CARACTERES
	MOint nchars;
	MOint Text[256];
	MOswitch TextActive;

	//CONSOLA VIRTUAL
	moConfig configcv;
	MOuint panelvirtual[1024];//contiene el code respectivo de la consola
	MOuint channel0virtual[1024];
	MOuint masterchannelvirtual[1024];
};

class moKeyboardFactory : public moIODeviceFactory {

public:
    moKeyboardFactory() {}
    virtual ~moKeyboardFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


/*
	//MOUSE
	MOswitch EventsMouse[16];
	MOswitch ButtonsMouse[16];	
	MOint MouseMotionX,MouseMotionY;//entre-255 y 255
	MOfloat MouseDefinition;
*/
#endif
