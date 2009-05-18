/*******************************************************************************

                              moKeyboard.cpp

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

#include "moKeyboard.h"

#ifdef MO_WIN32
  #include "SDL.h"
#else
  #include "SDL/SDL.h"
#endif


#define MO_KEYBOARD_CFG_STRCOD 0
#define MO_KEYBOARD_CFG_TYPE 1
#define MO_KEYBOARD_CFG_KEY 2

#define MO_KEYBOARD_SDL_KEY 0
#define MO_KEYBOARD_SDL_KEY_CODE 1

#define BUTTON_PUSHED 0
#define BUTTON_RELEASED 1


//========================
//  Factory
//========================

moKeyboardFactory *m_KeyboardFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_KeyboardFactory==NULL)
		m_KeyboardFactory = new moKeyboardFactory();
	return (moIODeviceFactory*) m_KeyboardFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_KeyboardFactory;
	m_KeyboardFactory = NULL;
}

moIODevice*  moKeyboardFactory::Create() {
	return new moKeyboard();
}

void moKeyboardFactory::Destroy(moIODevice* fx) {
	delete fx;
}

//========================
//  Keyboard
//========================



moKeyboard::moKeyboard() {

	Codes = NULL;
	ncodes = 0;
	SetName("keyboard");
	SetType(MO_OBJECT_IODEVICE);
}

moKeyboard::~moKeyboard() {
	Finish();
}


MOboolean
moKeyboard::Init() {



	MOuint i,j,coparam;
	MOuint nvirtualcodes,cv,ncv,tid;
	moText cvstr;//para hacer comparacion
	moText conf;

        moText text;

	//INICIALIZAMOS:
	Codes = NULL;
	for(i=0;i<1024;i++) Keys[i].Init();
	for(i=0;i<1024;i++) panelvirtual[i] = MO_UNDEFINED;//indefinidos
	for(i=0;i<1024;i++) channel0virtual[i] = MO_UNDEFINED;//indefinidos
	for(i=0;i<1024;i++) masterchannelvirtual[i] = MO_UNDEFINED;//indefinidos



	//KEYBOARD COMUN

	//levantamos el config del keyboard
	conf = m_pResourceManager->GetDataMan()->GetDataPath()+moText("/");
    conf += GetConfigName();
    conf += moText(".cfg");
	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		text = moText("Couldn't load keyboard config");
		MODebug2->Error(text);
		return false;
	}

    moMoldeoObject::Init();


	//levantamos los codes definidos
	coparam = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(coparam);
	ncodes = m_Config.GetValuesCount(coparam);
	Codes = new moKeyCode [ncodes];

	m_Config.FirstValue();

	MODebug2->Log( moText("KEYBOARD: ncodes:") + IntToStr(ncodes) );

	for( i = 0; i < ncodes; i++) {
		Codes[i].strcod = m_Config.GetParam().GetValue().GetSubValue(MO_KEYBOARD_CFG_STRCOD).Text();
		Codes[i].devcode = i;
		Codes[i].key = getKey( m_Config.GetParam().GetValue().GetSubValue(MO_KEYBOARD_CFG_KEY).Text() );
		Codes[i].type = m_Config.GetParam().GetValue().GetSubValue(MO_KEYBOARD_CFG_TYPE).Int();
		Codes[i].state = MO_FALSE;//inicializamos en vacio
		Codes[i].value = MO_FALSE;


		if(Codes[i].key==-1) {
            text = moText("(keyboard):\n no se encuentra el code de la key del code: ");
            text +=  Codes[i].strcod;
            text +=  moText("\n");
			MODebug2->Error(text);
		} else {
#ifdef MO_DEBUG
            text = moText("strcod: ");
            text += Codes[i].strcod;
            text += moText(" ");
			MODebug2->Message(text);
			MODebug2->Message(moText("cod: ")+ IntToStr(Codes[i].devcode) );
			MODebug2->Message(moText("type: ")+ IntToStr(Codes[i].type) );
			MODebug2->Message(moText("type: ")+ IntToStr(Codes[i].type) );

#endif
			//nada mas
		}

		m_Config.NextValue();
	}



	//PANEL VIRTUAL
  ncv = 0;
	//levantamos los codes de la consola virtual
	if (configcv.LoadConfig(m_pResourceManager->GetDataMan()->GetDataPath()+moText("/mixer.cfg"))==MO_CONFIG_OK) {

		cv = configcv.GetParamIndex("mixer");
		ncv = configcv.GetValuesCount(cv);
		configcv.SetCurrentParamIndex(cv);

		//y asociamos a los virtuals del keyboard
		coparam = m_Config.GetParamIndex("panelvirtual");
		m_Config.SetCurrentParamIndex(coparam);
		nvirtualcodes = m_Config.GetValuesCount(coparam);

		m_Config.FirstValue();

		printf("KEYBOARD simulando panelvirtual: nvirtualcodes:%i\n",nvirtualcodes);

		for( i = 0; i < nvirtualcodes; i++) {

			configcv.FirstValue();
			tid = getKey( m_Config.GetParam().GetValue().GetSubValue(0).Text() );
			cvstr = m_Config.GetParam().GetValue().GetSubValue(1).Text();

			panelvirtual[tid] = MO_UNDEFINED;//indefinido
			//salvo que lo encuentre definidio en consolavirtual.cfg
			for(j=0; j<ncv; j++) {
				if(!stricmp( cvstr, configcv.GetParam().GetValue().GetSubValue(0).Text() ) ) {
					panelvirtual[tid] = j;
					break;
				}
				configcv.NextValue();
			}


			if(panelvirtual[tid]==-1) {
                text = moText("(panelvirtual):\n couldn't find devicecode of string code: ");
                text += cvstr;
				MODebug2->Error(text);
			}
			m_Config.NextValue();
		}

  //CANAL0VIRTUAL

      //y asociamos a los virtuals del keydo
    coparam = m_Config.GetParamIndex("channel0virtual");
    m_Config.SetCurrentParamIndex(coparam);
    nvirtualcodes = m_Config.GetValuesCount(coparam);

    m_Config.FirstValue();

    printf("KEYBOARD simulando channel 0 virtual: nvirtualcodes:%i\n",nvirtualcodes);

    for( i = 0; i < nvirtualcodes; i++) {

      configcv.FirstValue();
      tid = getKey( m_Config.GetParam().GetValue().GetSubValue(0).Text() );
      cvstr = m_Config.GetParam().GetValue().GetSubValue(1).Text();

      channel0virtual[tid] = MO_UNDEFINED;//indefinido
      //salvo que lo encuentre definidio en consolavirtual.cfg
      for(j=0; j<ncv; j++) {
        if(!stricmp( cvstr, configcv.GetParam().GetValue().GetSubValue(0).Text() )) {
          channel0virtual[tid] = j;
          break;
        }
        configcv.NextValue();
      }


      if(channel0virtual[tid]==-1) {
            text = moText("(channel0virtual):\n no se encuentra el code de la key del code: ");
            text += cvstr;
            MODebug2->Error(text);
      }
      m_Config.NextValue();
    }

  //CANALMAESTRO VIRTUAL
      //y asociamos a los virtuals del keydo
    coparam = m_Config.GetParamIndex("masterchannelvirtual");
    m_Config.SetCurrentParamIndex(coparam);
    nvirtualcodes = m_Config.GetValuesCount(coparam);

    m_Config.FirstValue();

    printf("KEYBOARD simulating channel master virtual: nvirtualcodes:%i\n",nvirtualcodes);

    for( i = 0; i < nvirtualcodes; i++) {

      configcv.FirstValue();
      tid = getKey( m_Config.GetParam().GetValue().GetSubValue(0).Text() );
      cvstr = m_Config.GetParam().GetValue().GetSubValue(1).Text();

      masterchannelvirtual[tid] = MO_UNDEFINED;//indefinido
      //salvo que lo encuentre definidio en consolavirtual.cfg
      for(j=0; j<ncv; j++) {
        if(!stricmp( cvstr, configcv.GetParam().GetValue().GetSubValue(0).Text() )) {
          masterchannelvirtual[tid] = j;
          break;
        }
        configcv.NextValue();
      }


      if(masterchannelvirtual[tid]==-1) {
            text = moText("(masterchannelvirtual):\n no se encuentra el code de la key del code: ");
            text += cvstr;
            MODebug2->Error(text);
      }
      m_Config.NextValue();
    }
	}
	return true;

}



//Devuelve el id asociado al string de la key
MOint
moKeyboard::getKey(moText s) {

	MOuint i,nkeys;
	MOint param,value,tecl,returned = -1;//debemos restaurar el value luego
	moText s1,s2;

	param = m_Config.GetCurrentParamIndex();
	value = m_Config.GetCurrentValueIndex(param);

	tecl = m_Config.GetParamIndex("keyboard");
	m_Config.SetCurrentParamIndex(tecl);
	nkeys = m_Config.GetValuesCount(tecl);
	m_Config.FirstValue();
	for( i = 0; i < nkeys; i++) {
		if(!stricmp(s,m_Config.GetParam().GetValue().GetSubValue(MO_KEYBOARD_SDL_KEY).Text() )) {
				returned =(m_Config.GetParam().GetValue().GetSubValue(MO_KEYBOARD_SDL_KEY_CODE).Int() );
				break;
		}
		m_Config.NextValue();
	}

	m_Config.SetCurrentParamIndex(param);
	m_Config.SetCurrentValueIndex(param,value);

	return returned;

}

/*
void
moKeyboard::SetValue(MOdevcode codid, MOswitch state) {

}
*/

MOswitch
moKeyboard::SetStatus(MOdevcode devcode, MOswitch state) {

	return true;
}


//notar que el devcode es a su vez el indice dentro del array de Codes
MOswitch
moKeyboard::GetStatus(MOdevcode devcode) {
	return(Codes[devcode].state);
}

MOint
moKeyboard::GetValue( MOdevcode devcode) {
	return(Codes[devcode].value);
}


MOdevcode
moKeyboard::GetCode(moText strcod) {

	MOint codi,i;
	MOint param,value;

	param = m_Config.GetCurrentParamIndex();
	value = m_Config.GetCurrentValueIndex(param);

	codi = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(codi);
	m_Config.FirstValue();

	for( i = 0; i < m_Config.GetValuesCount(codi); i++) {
		if(!stricmp( strcod, m_Config.GetParam().GetValue().GetSubValue(MO_KEYBOARD_CFG_STRCOD).Text() ))
			return(i);
		m_Config.NextValue();
	}

	m_Config.SetCurrentParamIndex(param);
	m_Config.SetCurrentValueIndex(param,value);

	return(-1);//error, no encontro
}

MOswitch
moKeyboard::getSpecials(MOint iii) {
	return Specials[iii];
}

void
moKeyboard::Update(moEventList *Events) {
	MOuint i;
	moEvent *actual,*tmp;

	//keys especiales
	MOboolean numlock;
	SDLMod KeysSpecials;
	Uint8 *keystate;
	KeysSpecials = SDL_GetModState();
	keystate = SDL_GetKeyState(NULL);

	if(KeysSpecials&KMOD_NUM) numlock = MO_TRUE; else numlock = MO_FALSE;

	if(KeysSpecials&KMOD_NUM) { Specials[0] = MO_ACTIVATED; }
	else { Specials[0] = MO_DEACTIVATED; }
	if(KeysSpecials&KMOD_CAPS) { Specials[1] = MO_ACTIVATED; }
	else { Specials[1] = MO_DEACTIVATED; }
	if(KeysSpecials&KMOD_LSHIFT) { Specials[2] = MO_ACTIVATED; }
	else { Specials[2] = MO_DEACTIVATED; }
	if(KeysSpecials&KMOD_RSHIFT) { Specials[3] = MO_ACTIVATED; }
	else { Specials[3] = MO_DEACTIVATED; }
	if(KeysSpecials&KMOD_RCTRL) { Specials[4] = MO_ACTIVATED; }
	else { Specials[4] = MO_DEACTIVATED; }

	//LIMPIAMOS:(se resetean los botones porque no son states continuos)
	for(i=0;i<1024;i++) {
		Keys[i].pressed = MO_FALSE;
		Keys[i].pressedmod = -1;
	}


	//RECORREMOS LOS EVENTOS Y GENERAMOS LOS VIRTUALES

	actual = Events->First;

	while(actual!=NULL) {

		if(actual->deviceid == MO_IODEVICE_KEYBOARD) {

			switch(actual->devicecode) {
				case SDL_KEYDOWN:
					Keys[actual->reservedvalue0].Push(actual->reservedvalue1);
					//genero el evento virtual para la consola virtual(pushed de una key)
					//disp,devcode,val0=botonid,val1=mod(keys especiales)

					//si pertenece al panel
					if(panelvirtual[actual->reservedvalue0]!=-1) {
						Events->Add(MO_IODEVICE_MIXER,BUTTON_PUSHED,panelvirtual[actual->reservedvalue0],actual->reservedvalue1);
					}

					//si pertenece a un channel
					if(numlock) {
						if(masterchannelvirtual[actual->reservedvalue0]!=-1) {
							Events->Add(MO_IODEVICE_MIXER,BUTTON_PUSHED,masterchannelvirtual[actual->reservedvalue0],actual->reservedvalue1);
						}
					}
					else {
						if(channel0virtual[actual->reservedvalue0]!=-1) {
							Events->Add(MO_IODEVICE_MIXER,BUTTON_PUSHED,channel0virtual[actual->reservedvalue0],actual->reservedvalue1);
						}
					}

					break;

				case SDL_KEYUP:
					Keys[actual->reservedvalue0].Release(actual->reservedvalue1);
					//genero el evento virtual para la consola virtual(released de una key)
					//si pertenece al panel
					if(panelvirtual[actual->reservedvalue0]!=-1) {
						Events->Add(MO_IODEVICE_MIXER,BUTTON_RELEASED,panelvirtual[actual->reservedvalue0],actual->reservedvalue1);
					}

					//si pertenece a un channel
					if(numlock) {
						if(masterchannelvirtual[actual->reservedvalue0]!=-1) {
							Events->Add(MO_IODEVICE_MIXER,BUTTON_RELEASED,masterchannelvirtual[actual->reservedvalue0],actual->reservedvalue1);
						}
						//OJO
						//aca hay que agregar un evento para soltar la key,
						//que corresponde al mismo code para otro channel virtual(comparten las keys)
					}
					else {
						if(channel0virtual[actual->reservedvalue0]!=-1) {
							Events->Add(MO_IODEVICE_MIXER,BUTTON_RELEASED,channel0virtual[actual->reservedvalue0],actual->reservedvalue1);
						}
					}

					break;
				default:
					break;
			}
			//ya usado lo borramos de la lista
			tmp = actual->next;
			Events->Delete(actual);
			actual = tmp;
		}
		else actual = actual->next;//no es nuestro pasamos al next
	}


	//ACTUALIZAMOS LOS CODIGOS


	for( i = 0; i < ncodes; i++) {

		//limpiamos:
		Codes[i].state = 0;
		Codes[i].value = 0;

		//actualizamos

		switch(Codes[i].type) {
			case 0:
				Codes[i].state = Keys[Codes[i].key].pushed;
				Codes[i].value = Keys[Codes[i].key].pushed;
				break;
			case 1:
				//condicion, pressedmod->sin SHIFT ni CTRL ni ALT
				if(!( Keys[Codes[i].key].pressedmod &(KMOD_SHIFT & KMOD_CTRL & KMOD_ALT))) {
					Codes[i].state = Keys[Codes[i].key].pressed;
					Codes[i].value = Keys[Codes[i].key].pressed;
				}
				break;
			case 2:
				//condicion, pressedmod->con SHIFT ni CTRL ni ALT
				if(
					(Keys[Codes[i].key].pressedmod & KMOD_SHIFT) &&
					!(Keys[Codes[i].key].pressedmod &(KMOD_CTRL & KMOD_ALT) )
					) {
					Codes[i].state = Keys[Codes[i].key].pressed;
					Codes[i].value = Keys[Codes[i].key].pressed;
				}
				break;
			case 3:
				//condicion, pressedmod->sin SHIFT con CTRL sin ALT
				if(
					(Keys[Codes[i].key].pressedmod & KMOD_CTRL) &&
					!(Keys[Codes[i].key].pressedmod &(KMOD_SHIFT & KMOD_ALT) )
					) {
					Codes[i].state = Keys[Codes[i].key].pressed;
					Codes[i].value = Keys[Codes[i].key].pressed;
				}
				break;
			default:
				break;
		}
	}

}



MOboolean
moKeyboard::Finish() {
	if(Codes!=NULL)
		delete[] Codes;
	Codes = NULL;
	ncodes = 0;
	return true;
}
