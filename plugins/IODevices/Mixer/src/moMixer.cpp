/*******************************************************************************

                              moMixer.cpp

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

#include "moMixer.h"

#define MOMIXER_CFG_STRCOD 0
#define MOMIXER_CFG_TYPE 1
#define MOMIXER_CFG_MIXERCOD 2

#define MOMIXER_SDL_KEY 0
#define MOMIXER_SDL_KEY_CODE 1

#define BUTTON_PUSHED 0
#define BUTTON_RELEASED 1
#define WHEEL_MOTION 2

#define CANAL0_SELECTOR 229
#define BTN_PRESET_SET 216



//========================
//  Factory
//========================

moMixerFactory *m_MixerFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_MixerFactory==NULL)
		m_MixerFactory = new moMixerFactory();
	return (moIODeviceFactory*) m_MixerFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_MixerFactory;
	m_MixerFactory = NULL;
}

moIODevice*  moMixerFactory::Create() {
	return new moMixer();
}

void moMixerFactory::Destroy(moIODevice* fx) {
	delete fx;
}


moMixer::moMixer() {
	Codes = NULL;
	ncodes = 0;
	SetName("mixer");
}

moMixer::~moMixer() {
	Finish();
}


MOboolean
moMixer::Init() {

	MOuint i,coparam;
	moText conf;

        moText text;

	//INICIALIZAMOS:
	Codes = NULL;

	for(i=0;i<512;i++) {
		Buttons[i].Init();
		Wheels[i].Init();
		Leds[i].Init();
		Texts[i].Init();
	}


	//Mixer

	//levantamos el config de la Mixer
	conf = m_pResourceManager->GetDataMan()->GetDataPath()+moText("/");
    conf += GetConfigName();
    conf += moText(".cfg");
	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		text = moText("Couldn't load mixer config");
		MODebug2->Error(text);
		return false;
	}


	//levantamos los codes definidos
	coparam = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(coparam);
	ncodes = m_Config.GetValuesCount(coparam);
	Codes = new moMixerCode [ncodes];


	m_Config.FirstValue();

	printf("\nMIXER: ncodes:%i\n",ncodes);

	for( i = 0; i < ncodes; i++) {
		Codes[i].strcod = m_Config.GetParam().GetValue().GetSubValue(MOMIXER_CFG_STRCOD).Text();
		Codes[i].devcode = i;
		Codes[i].mixercode = getMixerCode( m_Config.GetParam().GetValue().GetSubValue(MOMIXER_CFG_MIXERCOD).Text() );
		Codes[i].type = m_Config.GetParam().GetValue().GetSubValue(MOMIXER_CFG_TYPE).Int();
		Codes[i].state = MO_FALSE;//inicializamos en vacio
		Codes[i].value = MO_FALSE;

		if(Codes[i].mixercode==-1) {
            text = moText("(keydo): no se encuentra el code de la key del code: ");
            text += Codes[i].strcod;
            MODebug2->Error(text);
    } else {
#ifdef MO_DEBUG
            text = moText("strcod: ");
            text += Codes[i].strcod;
            text += moText(" ");
            MODebug2->Log( moText("cod: ") + IntToStr(Codes[i].devcode) +
                           moText("type: ") + IntToStr(Codes[i].type) +
                           moText("cod interno: ") + IntToStr(Codes[i].mixercode));
#endif
			//nada mas
		}

		m_Config.NextValue();
	}


	return true;
}


//Devuelve el id asociado al string de la key
MOint
moMixer::getMixerCode(moText s) {
	MOuint i,nmixercodes;
	MOint param,value,cons,returned = -1;//debemos restaurar el value luego

	param = m_Config.GetCurrentParamIndex();
	value = m_Config.GetCurrentValueIndex( param );

	cons = m_Config.GetParamIndex("mixer");
	m_Config.SetCurrentParamIndex( cons );
	nmixercodes = m_Config.GetValuesCount( cons );
	m_Config.FirstValue();
	for( i = 0; i < nmixercodes; i++) {
		if(!stricmp( s, m_Config.GetParam().GetValue().GetSubValue(MOMIXER_SDL_KEY).Text() ) ) {
				returned = i;
				break;
		}
		m_Config.NextValue();
	}

	m_Config.SetCurrentParamIndex( param );
	m_Config.SetCurrentValueIndex( param, value );

	/*
	if (returned==-1) {

	}
	*/

	return returned;
}

/*
void
moMixer::SetValue(MOdevcode codid, MOswitch state) {

}
*/

MOswitch
moMixer::SetStatus(MOdevcode devcode, MOswitch state) {


	//este va a servir para setear led s de Salida!!!
	//Leds de seleccion
	//Leds de on/off
	//Boomers
	//Texts
	return true;
}


//notar que el devcode es a su vez el indice dentro del array de Codes
MOswitch
moMixer::GetStatus(MOdevcode devcode) {
	return(Codes[devcode].state);
}

MOint
moMixer::GetValue( MOdevcode devcode) {
	return(Codes[devcode].value);
}


MOdevcode
moMixer::GetCode(moText strcod) {

	MOint codi,i;
	MOint param,value;

	param = m_Config.GetCurrentParamIndex();
	value = m_Config.GetCurrentValueIndex(param);

	codi = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(codi);
	m_Config.FirstValue();

	for( i = 0; i < m_Config.GetValuesCount(codi); i++) {
		if(!stricmp( strcod, m_Config.GetParam().GetValue().GetSubValue( MOMIXER_CFG_STRCOD ).Text() ))
			return(i);
		m_Config.NextValue();
	}

	m_Config.SetCurrentParamIndex( param );
	m_Config.SetCurrentValueIndex( param, value );

	return(-1);//error, no encontro
}

void
moMixer::Update(moEventList *Events) {

	MOuint i;
	moEvent *actual,*tmp;


	//LIMPIAMOS:(se resetean los presseds porque no son states continuos)
	for(i=0;i<512;i++) {
		Buttons[i].pressed = MO_FALSE;
		Wheels[i].Init();
		MidiControllers[i].Init();
	}

	//RECORREMOS LOS EVENTOS

	actual = Events->First;

	while(actual!=NULL) {

		if(actual->deviceid == MO_IODEVICE_MIXER) {

			switch(actual->devicecode) {
				case BUTTON_PUSHED:
					Buttons[actual->reservedvalue0].Push(actual->reservedvalue1);
					break;
				case BUTTON_RELEASED:
					Buttons[actual->reservedvalue0].Release(actual->reservedvalue1);
					break;
				case WHEEL_MOTION:
					Wheels[actual->reservedvalue0].Motion(actual->reservedvalue1);
					break;
				default:
					break;
			}
			//ya usado lo borramos de la lista
			tmp = actual->next;
			Events->Delete(actual);
			actual = tmp;
		} else if (actual->deviceid == MO_IODEVICE_MIDI) {
			switch(actual->devicecode) {
				/*case BUTTON_PUSHED:
					Buttons[actual->reservedvalue0].Push(actual->reservedvalue1);
					break;
				case BUTTON_RELEASED:
					Buttons[actual->reservedvalue0].Release(actual->reservedvalue1);
					break;
				case WHEEL_MOTION:
					Wheels[actual->reservedvalue0].Motion(actual->reservedvalue1);
					break;
					*/
				case 0:
					//value1 = CC , entre 1 y 110, 382 es la posicion del
					//primer MIDI_ en la codificacion del parametro :mixer
					MidiControllers[actual->reservedvalue1+382-1].Motion(actual->reservedvalue2);
					break;
				default:
					break;
			}
			//no los borramos por las dudas alguien mas lo use
			//lo borrara el midi device cuando termine el loop
			tmp = actual->next;
			actual = tmp;
		}
		else actual = actual->next;//no es nuestro pasamos al next
	}


	//ACTUALIZAMOS LOS CODIGOS
/*
#0: pushed
#1: pressed
#2: pressed + selector channel0
#3: pressed + selector channel1
#4: pressed + selector channel2
#5: pressed + selector channel3
#6: pressed + selector masterchannel
#7: pressed + preseteador
#8: led state
#9: rueda se movio
#10: text
*/
	//moMixerCode *MC;
	for( i = 0; i < ncodes; i++) {

		//limpiamos:
		Codes[i].state = 0;
		Codes[i].value = 0;
		//MC = &Codes[i];

		//actualizamos
		switch(Codes[i].type) {
			case 0:
				Codes[i].state = Buttons[Codes[i].mixercode].pushed;
				Codes[i].value = Buttons[Codes[i].mixercode].pushed;
				break;
			case 1:
				//condicion, boton sin selectores
				if(!(Buttons[CANAL0_SELECTOR].pushed)) {
					Codes[i].state = Buttons[Codes[i].mixercode].pressed;
					Codes[i].value = Buttons[Codes[i].mixercode].pressed;
				}
				break;
			case 2:
				//condicion, boton con selector channel0
				if(Buttons[CANAL0_SELECTOR].pushed) {
					Codes[i].state = Buttons[Codes[i].mixercode].pressed;
					Codes[i].value = Buttons[Codes[i].mixercode].pressed;
				}
				break;
			case 3:
				//condicion, boton con selector channel1
				/*
				if(Buttons[CANAL1_SELECTOR].pushed) {
					Codes[i].state = Buttons[Codes[i].mixercode].pressed;
					Codes[i].value = Buttons[Codes[i].mixercode].pressed;
				}
				*/
				break;
			case 4:
				//condicion, boton con selector channel2
				/*
				if(Buttons[CANAL2_SELECTOR].pushed) {
					Codes[i].state = Buttons[Codes[i].mixercode].pressed;
					Codes[i].value = Buttons[Codes[i].mixercode].pressed;
				}
				*/
				break;
			case 5:
				//condicion, boton con selector channel3
				/*
				if(Buttons[CANAL3_SELECTOR].pushed) {
					Codes[i].state = Buttons[Codes[i].mixercode].pressed;
					Codes[i].value = Buttons[Codes[i].mixercode].pressed;
				}
				*/
				break;
			case 6:
				//condicion, boton con selector masterchannel
				//no se para que sirve
				break;
			case 7:
				//condicion, boton con PRESETEADOR
				if(Buttons[BTN_PRESET_SET].pushed) {
					Codes[i].state = Buttons[Codes[i].mixercode].pressed;
					Codes[i].value = Buttons[Codes[i].mixercode].pressed;
				}
				break;
			case 8:
				Codes[i].state = Leds[Codes[i].mixercode].led;
				Codes[i].value = Leds[Codes[i].mixercode].led;
				break;
			case 9:
				if ( Codes[i].mixercode < 382 ) {
					Codes[i].state = Wheels[Codes[i].mixercode].motion;
					Codes[i].value = Wheels[Codes[i].mixercode].rel;
				} else {
					Codes[i].state = MidiControllers[Codes[i].mixercode].motion;
					Codes[i].value = MidiControllers[Codes[i].mixercode].rel;
				}
				break;
			case 10:
				Codes[i].state = Texts[Codes[i].mixercode].n;
				Codes[i].value = Texts[Codes[i].mixercode].n;
				Codes[i].text = Texts[Codes[i].mixercode].text;
				break;/*
			case 11:
				//condicion, boton con selector channel0
				if(Buttons[CANAL0_SELECTOR2].pushed) {
					Codes[i].state = Buttons[Codes[i].mixercode].pressed;
					Codes[i].value = Buttons[Codes[i].mixercode].pressed;
				}
				break;*/
			default:
				break;
		}
	}

}



MOboolean
moMixer::Finish() {
	if(Codes!=NULL)
		delete[] Codes;
	Codes = NULL;
	ncodes = 0;
	return true;
}
