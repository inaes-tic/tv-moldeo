/*******************************************************************************

                                moMouse.h

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

#include "moTypes.h"
#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"

#ifdef MO_WIN32
  #include "SDL.h"
#else
  #include "SDL/SDL.h"
#endif

#ifndef __MO_MOUSE_H
#define __MO_MOUSE_H

//Buttons
#define MOUSE_BUTTON_LEFT 0
#define MOUSE_BUTTON_MIDDLE 1
#define MOUSE_BUTTON_RIGHT 2
//Wheels
#define MOTION_REL_X 0
#define MOTION_REL_Y 1
//Position
#define POSITION_X 0
#define POSITION_Y 1

//channel0virtual
#define SDL_MOTION_REL_X_LEFT	0
#define SDL_MOTION_REL_Y_LEFT	1
#define SDL_MOTION_REL_X_MIDDLE	2
#define SDL_MOTION_REL_Y_MIDDLE	3
#define SDL_MOTION_REL_X_RIGHT	4
#define SDL_MOTION_REL_Y_RIGHT	5
#define SDL_MOTION_POS_X		6
#define SDL_MOTION_POS_Y		7

class  moMouseButton {
public:
	MOboolean pushed;
	SDLMod pushedmod;
	MOboolean released;
	SDLMod releasedmod;
	MOint pressed;
	SDLMod pressedmod;

	moMouseButton() {
		pushed = MO_FALSE;
		pushedmod = KMOD_NONE;
		released = MO_FALSE;
		releasedmod = KMOD_NONE;
		pressed = 0;
		pressedmod = KMOD_NONE;
	}

	MOboolean Init() {
		pushed = MO_FALSE;
		pushedmod = KMOD_NONE;
		released = MO_FALSE;
		releasedmod = KMOD_NONE;
		pressed = 0;
		pressedmod = KMOD_NONE;
		return true;
	}

	void Push(SDLMod mod) {
		pushed = MO_TRUE;
		pushedmod =  mod;
		released = MO_FALSE;
		releasedmod = KMOD_NONE;
		pressedmod = KMOD_NONE;
	}

	MOint Release(SDLMod mod) {
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
			pushedmod = KMOD_NONE;
			released = MO_FALSE;
			releasedmod = KMOD_NONE;
			return MO_TRUE;
		}
	}
};

class  moMouseWheel {
public:
	MOboolean motion;//verdadero si se movio
	MOint rel;
	MOint mousemod;

	moMouseWheel() {
		motion = MO_FALSE;
		rel = 0;
		mousemod = 0;
	}

	MOboolean Init() {
		motion = MO_FALSE;
		rel = 0;
		mousemod = 0;
		return true;
	}

	void Motion(MOint r) {
		if(r!=0) {
			rel = r;
			motion = MO_TRUE;
		}
	}
};

class  moMousePosition {
public:
	MOboolean motion;//verdadero si se movio
	MOint pos;
	MOint mousemod;

	moMousePosition() {
		motion = MO_FALSE;
		pos = 0;
	}

	MOboolean Init() {
		motion = MO_FALSE;
		pos = 0;
		return true;
	}

	void Motion(MOint r) {
		if(r!=pos) {
			pos = r;
			motion = MO_TRUE;
		}
	}
};


class  moMouseCode {
public:
	moText strcod;
	MOint code;
	MOint mousecod;//mapeo type SDL
	MOint type;//pushedr:0 ,prendedor:1, selector:2, seteador:3,...otros
	MOint especiales;//keys especiales, como alt, caps lock, etc...
	MOint state;//0 MO_FALSE(no esta activo el device) MO_ON(está on) MO_OFF(está off)
	MOint value;//depende
};

class  moMouse : public moIODevice {
public:
    moMouse();
    virtual ~moMouse();
	MOboolean		Init();
	MOboolean		Finish();

    void			Update(moEventList*);
    MOswitch		GetStatus( MOdevcode);
    MOswitch		SetStatus( MOdevcode,MOswitch);
	MOint			GetValue( MOdevcode);
    MOdevcode		GetCode( moText);
	MOint			getMouseCod(moText);

private:
    moConfig config;

	//MOUSE
	moMouseButton Buttons[3];
	moMouseWheel Wheels[2];
	moMousePosition Position[2];

	moMouseCode *Codes;
	MOuint ncodes;

	//BUFFER DE POSICIONES ANTERIORES
	//MOint nchars;
	//MOint Text[256];
	//MOswitch TextActive;

	//CONSOLA VIRTUAL
	moConfig configcv;
	MOuint channel0virtual[6];//simulamos rueditas para el channel virtual
	MOuint masterchannelvirtual[6];//simulamos rueditas para el channel virtual


};


class moMouseFactory : public moIODeviceFactory {

public:
    moMouseFactory() {}
    virtual ~moMouseFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}

#endif
