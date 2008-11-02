/*******************************************************************************

                              moMouse.cpp

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

#include "moMouse.h"

#define MO_MOUSE_CFG_STRCOD 0
#define MO_MOUSE_CFG_TYPE 1
#define MO_MOUSE_CFG_MOUSECOD 2

#define MO_MOUSE_SDL_KEY 0
#define MO_MOUSE_SDL_KEY_CODE 1

#define WHEEL_MOTION 2


//========================
//  Factory
//========================

moMouseFactory *m_MouseFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_MouseFactory==NULL)
		m_MouseFactory = new moMouseFactory();
	return (moIODeviceFactory*) m_MouseFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_MouseFactory;
	m_MouseFactory = NULL;
}

moIODevice*  moMouseFactory::Create() {
	return new moMouse();
}

void moMouseFactory::Destroy(moIODevice* fx) {
	delete fx;
}


//========================
//  Mouse
//========================


moMouse::moMouse() {
	Codes = NULL;
	SetName("mouse");
}

moMouse::~moMouse() {
	Finish();
}

MOboolean
moMouse::Init() {

	MOuint i,j,coparam;
	MOuint nvirtualcodes,cv,ncv;
	moText cvstr;
	moText conf;

        moText text;

	//primero, ponemos todo en falso:(indefinido)
	for(i=0;i<3;i++) Buttons[i].Init();
	for(i=0;i<2;i++) Wheels[i].Init();
	for(i=0;i<2;i++) Position[i].Init();
	Codes = NULL;

	//levantamos el config del mouse
	conf = m_pResourceManager->GetDataMan()->GetDataPath()+moText("/");
    conf += GetConfigName();
    conf += moText(".cfg");
	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		text = moText("Couldn't load mouse config");
		MODebug2->Error(text);
		return false;
	}

	//levantamos los codes definidos
	coparam = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(coparam);
	ncodes = m_Config.GetValuesCount(coparam);
	Codes = new moMouseCode [ncodes];

	//ahora traduzco la info
    #ifdef _DEBUG
        text = moText("(MOUSE) : ncodes: ") + IntToStr(ncodes);
        MODebug2->Log(text);
    #endif



	for( i = 0; i < ncodes; i++) {
		m_Config.SetCurrentValueIndex(coparam,i);
		Codes[i].strcod = m_Config.GetParam().GetValue().GetSubValue(MO_MOUSE_CFG_STRCOD).Text();
		Codes[i].code = i;
		Codes[i].mousecod = getMouseCod( m_Config.GetParam().GetValue().GetSubValue(MO_MOUSE_CFG_MOUSECOD).Text() );
		Codes[i].type = m_Config.GetParam().GetValue().GetSubValue(MO_MOUSE_CFG_TYPE).Int();
		Codes[i].state = MO_FALSE;//inicializamos en vacio

		if(Codes[i].mousecod==-1) {
            text = moText("(mouse) : no se encuentra el code: ");
            text += Codes[i].strcod;
			MODebug2->Error(text);
			return false;
		}
	}


	//CANAL0 VIRTUAL

	//levantamos los codes de la consola virtual
	if (configcv.LoadConfig(m_pResourceManager->GetDataMan()->GetDataPath()+moText("/mixer.cfg"))==MO_CONFIG_OK) {

		cv = configcv.GetParamIndex("mixer");
		ncv = configcv.GetValuesCount(cv);
		configcv.SetCurrentParamIndex(cv);

		//y asociamos a los virtuals del mouse
		coparam = m_Config.GetParamIndex("channel0virtual");
		m_Config.SetCurrentParamIndex(coparam);
		nvirtualcodes = m_Config.GetValuesCount(coparam);

		printf("channel0 virtual: nvirtualcodes:%i\n",nvirtualcodes);

		for( i = 0; i < nvirtualcodes; i++) {

			m_Config.SetCurrentValueIndex(coparam,i);
			cvstr = m_Config.GetParam().GetValue().GetSubValue(1).Text();

			channel0virtual[i] = MO_UNDEFINED;
			for(j=0; j<ncv; j++) {
				configcv.SetCurrentValueIndex( cv, j );
				if(!stricmp( cvstr, configcv.GetParam().GetValue().GetSubValue(0).Text() )) {
					channel0virtual[i] = j;
					break;
				}
			}


			if(channel0virtual[i]==-1) {
				text = moText("(channel0 virtual): no se encuentra el code de la key del code: ");
				text += cvstr;
				MODebug2->Error(text);
				return false;
			}
		}
	} else {
		text = moText("Error: ") + (m_pResourceManager->GetDataMan()->GetDataPath()+moText("/mixer.cfg"))+moText("NOT FOUND");
		MODebug2->Error(text);
		return false;
	}

	//CANAL MAESTRO VIRTUAL
	//y asociamos a los virtuals del keydo
	coparam = m_Config.GetParamIndex("masterchannelvirtual");
	m_Config.SetCurrentParamIndex(coparam);
	nvirtualcodes = m_Config.GetValuesCount(coparam);

	//printf("channel master virtual: nvirtualcodes:%i\n",nvirtualcodes);

	for( i = 0; i < nvirtualcodes; i++) {

		m_Config.SetCurrentValueIndex(coparam,i);
		cvstr = m_Config.GetParam().GetValue().GetSubValue(1).Text();

		masterchannelvirtual[i] = MO_UNDEFINED;
		for(j=0; j<ncv; j++) {
			configcv.SetCurrentValueIndex(cv,j);
			if(!stricmp(cvstr , configcv.GetParam().GetValue().GetSubValue(0).Text() )) {
				masterchannelvirtual[i] = j;
				break;
			}
		}


		if(masterchannelvirtual[i]==-1) {
            text = moText("(channel master virtual): no se encuentra el code de la key del code: ");
            text += cvstr;
			MODebug2->Error(text);
			return false;
		}
	}

	return true;
}

MOint
moMouse::getMouseCod(moText s) {
	MOuint i,nmousecods;
	MOint param,value,mous,returned = -1;//debemos restaurar el value luego

	param = m_Config.GetCurrentParamIndex();
	value = m_Config.GetCurrentValueIndex(param);

	mous = m_Config.GetParamIndex("buttons");
	m_Config.SetCurrentParamIndex( mous );
	nmousecods = m_Config.GetValuesCount(mous);
	for( i = 0; i < nmousecods; i++) {
		m_Config.SetCurrentValueIndex( mous, i );
		if(!stricmp( s, m_Config.GetParam().GetValue().GetSubValue(MO_MOUSE_SDL_KEY).Text() )) {
				returned = i;
				break;
		}
	}

	mous = m_Config.GetParamIndex("wheels");
	m_Config.SetCurrentParamIndex( mous );
	nmousecods = m_Config.GetValuesCount( mous );
	m_Config.FirstValue();
	for( i = 0; i < nmousecods; i++) {
		m_Config.SetCurrentValueIndex( mous, i );
		if(!stricmp( s, m_Config.GetParam().GetValue().GetSubValue(MO_MOUSE_SDL_KEY).Text() ) ) {
				returned = i;
				break;
		}
	}

	mous = m_Config.GetParamIndex("position");
	m_Config.SetCurrentParamIndex( mous );
	nmousecods = m_Config.GetValuesCount( mous );
	m_Config.FirstValue();
	for( i = 0; i < nmousecods; i++) {
		m_Config.SetCurrentValueIndex( mous, i );
		if(!stricmp(s, m_Config.GetParam().GetValue().GetSubValue(MO_MOUSE_SDL_KEY).Text() )) {
				returned = i;
				break;
		}
	}

	m_Config.SetCurrentParamIndex(param);
	m_Config.SetCurrentValueIndex(param,value);
	return returned;
}

MOswitch
moMouse::SetStatus(MOdevcode devcode, MOswitch state) {

	return true;
}


//notar que el devcode es a su vez el indice dentro del array de Codes
MOswitch
moMouse::GetStatus(MOdevcode devcode) {
	return(Codes[devcode].state);
}

MOint
moMouse::GetValue( MOdevcode devcode) {
	return(Codes[devcode].value);
}


MOdevcode
moMouse::GetCode(moText strcod) {

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
		if(!stricmp( strcod, m_Config.GetParam().GetValue().GetSubValue(MO_MOUSE_CFG_STRCOD).Text() ) )
			return i;
	}

	m_Config.SetCurrentParamIndex( param );
	m_Config.SetCurrentValueIndex( param, value );

	return(-1);//error, no encontro
}

void
moMouse::Update(moEventList *Events) {
	MOuint i;
	moEvent *actual,*tmp;

	MOboolean numlock;
	SDLMod KeysSpecials;
	//Uint8 *keystate;

	KeysSpecials = SDL_GetModState();
	if(KeysSpecials&KMOD_NUM) numlock = MO_TRUE; else numlock = MO_FALSE;

	//debo poner todos los movimientos en cero(si no lo viste perdiste!!!)
	//y events de botones
	for(i=0;i<3;i++) Buttons[i].pressed = MO_FALSE;
	for(i=0;i<2;i++) Wheels[i].Init();
	for(i=0;i<2;i++) Position[i].motion = MO_FALSE;

	actual = Events->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	while(actual!=NULL) {
		//solo nos interesan los del mouse
		if(actual->deviceid == MO_IODEVICE_MOUSE) {

			switch(actual->devicecode) {

				case SDL_MOUSEBUTTONDOWN:
					switch(actual->reservedvalue0) {
						case SDL_BUTTON_LEFT:	Buttons[MOUSE_BUTTON_LEFT].Push(KeysSpecials);
												break;
						case SDL_BUTTON_MIDDLE:	Buttons[MOUSE_BUTTON_MIDDLE].Push(KeysSpecials);
												break;
						case SDL_BUTTON_RIGHT:	Buttons[MOUSE_BUTTON_RIGHT].Push(KeysSpecials);
												break;
					}
					break;

				case SDL_MOUSEBUTTONUP:
					switch(actual->reservedvalue0) {
						case SDL_BUTTON_LEFT:	Buttons[MOUSE_BUTTON_LEFT].Release(KeysSpecials);
												break;
						case SDL_BUTTON_MIDDLE:	Buttons[MOUSE_BUTTON_MIDDLE].Release(KeysSpecials);
												break;
						case SDL_BUTTON_RIGHT:	Buttons[MOUSE_BUTTON_RIGHT].Release(KeysSpecials);
												break;
					}
					break;
				case SDL_MOUSEMOTION:
					if(numlock) {
						//events virtuals rueditas para el channel master
						if((actual->reservedvalue0!=0) &&(Buttons[MOUSE_BUTTON_LEFT].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,masterchannelvirtual[SDL_MOTION_REL_X_LEFT],actual->reservedvalue0);
						}
						if((actual->reservedvalue1!=0) &&(Buttons[MOUSE_BUTTON_LEFT].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,masterchannelvirtual[SDL_MOTION_REL_Y_LEFT],actual->reservedvalue1);
						}
						if((actual->reservedvalue0!=0) &&(Buttons[MOUSE_BUTTON_MIDDLE].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,masterchannelvirtual[SDL_MOTION_REL_X_MIDDLE],actual->reservedvalue0);
						}
						if((actual->reservedvalue1!=0) &&(Buttons[MOUSE_BUTTON_MIDDLE].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,masterchannelvirtual[SDL_MOTION_REL_Y_MIDDLE],actual->reservedvalue1);
						}
						if((actual->reservedvalue0!=0) &&(Buttons[MOUSE_BUTTON_RIGHT].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,masterchannelvirtual[SDL_MOTION_REL_X_RIGHT],actual->reservedvalue0);
						}
						if((actual->reservedvalue1!=0) &&(Buttons[MOUSE_BUTTON_RIGHT].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,masterchannelvirtual[SDL_MOTION_REL_Y_RIGHT],actual->reservedvalue1);
						}
					} else {
						//events virtuals rueditas para el channel0
						if((actual->reservedvalue0!=0) &&(Buttons[MOUSE_BUTTON_LEFT].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,channel0virtual[SDL_MOTION_REL_X_LEFT],actual->reservedvalue0);
						}

						if((actual->reservedvalue1!=0) &&(Buttons[MOUSE_BUTTON_LEFT].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,channel0virtual[SDL_MOTION_REL_Y_LEFT],actual->reservedvalue1);
						}

						if((actual->reservedvalue0!=0) &&(Buttons[MOUSE_BUTTON_MIDDLE].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,channel0virtual[SDL_MOTION_REL_X_MIDDLE],actual->reservedvalue0);
						}

						if((actual->reservedvalue1!=0) &&(Buttons[MOUSE_BUTTON_MIDDLE].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,channel0virtual[SDL_MOTION_REL_Y_MIDDLE],actual->reservedvalue1);
						}

						if((actual->reservedvalue0!=0) &&(Buttons[MOUSE_BUTTON_RIGHT].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,channel0virtual[SDL_MOTION_REL_X_RIGHT],actual->reservedvalue0);
						}

						if((actual->reservedvalue1!=0) &&(Buttons[MOUSE_BUTTON_RIGHT].pushed==MO_TRUE)) {
							Events->Add(MO_IODEVICE_MIXER,WHEEL_MOTION,channel0virtual[SDL_MOTION_REL_Y_RIGHT],actual->reservedvalue1);
						}
					}
					//x
					Wheels[MOTION_REL_X].Motion(actual->reservedvalue0);
					Position[POSITION_X].Motion(actual->reservedvalue2);
					//y
					Wheels[MOTION_REL_Y].Motion(actual->reservedvalue1);
					Position[POSITION_Y].Motion(actual->reservedvalue3);
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

moMouseCode *MC;
	//ACTUALIZAMOS LOS CODIGOS
	for( i = 0; i < ncodes; i++) {

		//limpia:
		Codes[i].state = MO_FALSE;
		Codes[i].value = 0;
		MC = &Codes[i];
		switch(Codes[i].type) {
			case 0://pushed
				Codes[i].state = Buttons[Codes[i].mousecod].pushed;
				Codes[i].value = Buttons[Codes[i].mousecod].pushed;
				break;
			case 1://pressed
				Codes[i].state = Buttons[Codes[i].mousecod].pressed;
				Codes[i].value = Buttons[Codes[i].mousecod].pressed;
				break;
			case 2://movimiento solo(sin importar botones)
				Codes[i].state = Wheels[Codes[i].mousecod].motion;
				Codes[i].value = Wheels[Codes[i].mousecod].rel;
				break;
			case 3:
				Codes[i].state = Position[Codes[i].mousecod].motion;
				Codes[i].value = Position[Codes[i].mousecod].pos;
				break;
			case 4://movimiento con boton left
				if(Buttons[MOUSE_BUTTON_LEFT].pushed &&(Wheels[0].motion || Wheels[1].motion)) {
					Codes[i].state = Wheels[Codes[i].mousecod].motion;
					Codes[i].value = Wheels[Codes[i].mousecod].rel;
				}
				break;
			case 5://movimiento con boton middle
				if(Buttons[MOUSE_BUTTON_MIDDLE].pushed &&(Wheels[0].motion || Wheels[1].motion)) {
					Codes[i].state = Wheels[Codes[i].mousecod].motion;
					Codes[i].value = Wheels[Codes[i].mousecod].rel;
				}
				break;
			case 6://movimiento con boton right
				if(Buttons[MOUSE_BUTTON_RIGHT].pushed &&(Wheels[0].motion || Wheels[1].motion)) {
					Codes[i].state = Wheels[Codes[i].mousecod].motion;
					Codes[i].value = Wheels[Codes[i].mousecod].rel;
				}
				break;
			case 7://movimiento con boton left
				if(Buttons[MOUSE_BUTTON_LEFT].pushed &&(Position[0].motion || Position[1].motion)) {
					Codes[i].state = Position[Codes[i].mousecod].motion;
					Codes[i].value = Position[Codes[i].mousecod].pos;
				}
				break;
			case 8://movimiento con boton middle
				if(Buttons[MOUSE_BUTTON_MIDDLE].pushed &&(Position[0].motion || Position[1].motion)) {
					Codes[i].state = Position[Codes[i].mousecod].motion;
					Codes[i].value = Position[Codes[i].mousecod].pos;
				}
				break;
			case 9://movimiento con boton right
				if(Buttons[MOUSE_BUTTON_RIGHT].pushed &&(Position[0].motion || Position[1].motion)) {
					Codes[i].state = Position[Codes[i].mousecod].motion;
					Codes[i].value = Position[Codes[i].mousecod].pos;
				}
				break;
			default:
				break;
		}
	}

}



MOboolean
moMouse::Finish() {
	if(Codes!=NULL)
		delete [] Codes;
	Codes = NULL;
	ncodes = 0;
	return true;
}



