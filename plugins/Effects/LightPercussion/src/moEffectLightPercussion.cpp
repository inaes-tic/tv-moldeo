/*******************************************************************************

                             moEffectLightPercussion.cpp

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

#include "moEffectLightPercussion.h"

//resultados queridos: al cabo de 20 segundos de dejar apretada una tecla se inscribe una cruz

//========================
//  Factory
//========================

moEffectLightPercussionFactory *m_EffectLightPercussionFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){ 
	if(m_EffectLightPercussionFactory==NULL)
		m_EffectLightPercussionFactory = new moEffectLightPercussionFactory();
	return(moEffectFactory*) m_EffectLightPercussionFactory;
} 

MO_PLG_API void DestroyEffectFactory(){ 
	delete m_EffectLightPercussionFactory;
	m_EffectLightPercussionFactory = NULL;
} 

moEffect*  moEffectLightPercussionFactory::Create() {
	return new moEffectLightPercussion();
}

void moEffectLightPercussionFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moLightPercussionPoint::moLightPercussionPoint() {
	int i,j;
	for(j=0;j<3;j++)
		for(i=0;i<3;i++)
			neighbors[i][j] = NULL;
	
	E_stable = 50;
	E_excited = 100;
	E_explosion = 400;
	E_depressed = 20;
	Energy = E_stable;//se le podria agregar un random a ver que onda
	Decay_time = 100;
	Time = 0;
}

moLightPercussionPoint::~moLightPercussionPoint() {

}

void
moLightPercussionPoint::EnergyPulse() {
	Increase(64.0);	
        moText energia_str("Energia: ");
	MODebug->Push(energia_str+IntToStr(Energy));
}

void
moLightPercussionPoint::Increase(MOfloat inc) {
	Energy+=inc;
	//ENERGIA > ENERGIA EXPLOSION ->todo mal
	if(Energy>E_explosion) {
		Decay();//no lo hagamos esperar		
	} else if(Energy<0.0) {
		Energy = 0.0;
	}
}

void
moLightPercussionPoint::Decay() {

	int t;
	t = SDL_GetTicks();

	if((Energy>=E_stable) &&(Energy<E_excited)) {
	//ESTADO ESTABLE(entre E_stable y E_excitado)
	//no pasa nada, solo aumenta su inestabilidad(mas propenso a recibir descargas y a entablarlas)
		Time = t;	
	} else if((Energy>=E_excited) &&(Energy<E_explosion)) {
	//ESTADO EXCITADO(entre E_excitado y E_explosion)
	//debe volver al state mas estable, a E_excitado, se produce una radiacion sistematica
		if((t-Time)>=Decay_time) {
			Irradiate(32.0);
			Time = t;
		}
	} else if(Energy>=E_explosion) {
	//ESTADO EXPLOSIVO!!!
	//explota y libera una considerable carga de energia pasando al state deprimido
		Irradiate(Energy-E_excited);//si no la puede liberar,,,,,		
	} else if((Energy<E_stable) &&(Energy>E_depressed)) {
	//ESTADO DEPRIMIDO(entre E_deprimido y E_stable)
	//recupera de a poco la energia(en forma inversa, de la nada)
		Time = t;//por ahora nada	
	}

}

void
moLightPercussionPoint::Irradiate(float ir) {

	float vi,vj,d;
	int I,J,i,j,h1,h2;

	h1 = moRand(3);
	h2 = moRand(3);

	vi = vj = 0.0;
	for(j=0;j<3;j++)
		for(i=0;i<3;i++) {
			if(neighbors[i][j]!=NULL) {
				vi+=(i-1)*(neighbors[i][j]->Energy - neighbors[i][j]->E_excited);
				vj+=(j-1)*(neighbors[i][j]->Energy - neighbors[i][j]->E_excited);
			}
		}
	d = sqrt(vi*vi + vj*vj);
	vi = vi / d;
	vj = vj / d;
	if(vi>=0.0)  vi =(float)(vi + 0.3);
	else vi =(float)(vi - 0.3);

	if(vj>=0.0)  vj =(float)(vj + 0.3);
	else vj =(float)(vj - 0.3);

	I =(int) vi;
	I = -I; //al de menor energia
	J =(int) vj;
	J = -J;
	if((I==0) &&(J==0)) {
		I = h1 - 1;
		J = h2 - 1;
	}
	if(neighbors[I+1][J+1]!=NULL) neighbors[I+1][J+1]->Increase(ir/4.0);

	this->Increase(-ir); //nos sacamos esa energia de encima

}


//kakaklsd

moEffectLightPercussion::moEffectLightPercussion() {
	devicecode = NULL;
	ncodes = 0;
	m_Name = "percussionlights";
}

moEffectLightPercussion::~moEffectLightPercussion() {
	Finish();
}

MOboolean
moEffectLightPercussion::Init() {
	int i,j,u,v;

    if (!PreInit()) return false;

    // Hacer la inicializacion de este efecto en particular.
    color = m_Config.GetParamIndex("color");	
    images = m_Config.GetParamIndex("textura");
    rotatx = m_Config.GetParamIndex("rotatex");
    rotaty = m_Config.GetParamIndex("rotatey");
    rotatz = m_Config.GetParamIndex("rotatez");
    scalex = m_Config.GetParamIndex("scalex");
    scaley = m_Config.GetParamIndex("scaley");
    scalez = m_Config.GetParamIndex("scalez");

    //Levanta imagees
	Images.MODebug = MODebug;
	Images.Init(GetConfig(),images,m_pResourceManager->GetTextureMan());

	Mwidth = 20;
	Mheight = 8;

	MESHALIGHTS = new moLightPercussionPoint* [Mwidth];
	for(i=0;i<Mwidth;i++) MESHALIGHTS[i] = new moLightPercussionPoint [Mheight];

	for(i=0;i<Mwidth;i++) 
	for(j=0;j<Mheight;j++) {
		MESHALIGHTS[i][j].MODebug = MODebug;
		MESHALIGHTS[i][j].Pressed = MO_RELEASED;
		MESHALIGHTS[i][j].I = i;
		MESHALIGHTS[i][j].J = j;		
		MESHALIGHTS[i][j].P0X =(float)(i-(Mwidth/2))*0.25;
		MESHALIGHTS[i][j].P0Y =(float)((Mheight/2)-j)*0.3;
		MESHALIGHTS[i][j].P0Z =(float) -3.5;
		MESHALIGHTS[i][j].Energy =(float) morand()*40.0;
		
		for(v=0;v<3;v++)
			for(u=0;u<3;u++) 
				if(!((u==1) &&(v==1)) &&
					((i+(u-1))>=0) &&((i+(u-1))<Mwidth) &&
					((j+(v-1))>=0) &&((j+(v-1))<Mheight))
						MESHALIGHTS[i][j].neighbors[u][v] = &MESHALIGHTS[i+(u-1)][j+(v-1)];	
	}

	//Image = Images[0];
    if(preconfig.GetPreConfNum() > 0)
        preconfig.PreConfFirst( GetConfig());

	return true;
}

void moEffectLightPercussion::Draw( moTempo* tempogral,moEffectState* parentstate)
{
	MOint i, j;
	//MOfloat r;//random
    
	PreDraw( tempogral, parentstate);

    glMatrixMode( GL_PROJECTION );
	glPushMatrix();
    glMatrixMode( GL_MODELVIEW );
	glPushMatrix();


	glLoadIdentity();

    glColor4f(  m_Config.GetParam(color).GetValue().GetSubValue(MO_RED).Float()*state.tintr,
				m_Config.GetParam(color).GetValue().GetSubValue(MO_GREEN).Float()*state.tintg,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_BLUE).Float()*state.tintb,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_ALPHA).Float()*state.alpha);
	
	glDisable(GL_DEPTH_TEST);	
	glDisable(GL_ALPHA);
	glDisable(GL_CULL_FACE);	
	glEnable(GL_BLEND);		
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

	glBindTexture(GL_TEXTURE_2D, Images.GetGLId(m_Config.GetCurrentValueIndex(images)));
  
	//recorre todas las lights y enciende las que estaban apagadas
	float al;

	for(j=0;j<Mheight;j++) {
	for(i=0;i<Mwidth;i++) {
		
		//decaen!!
		MESHALIGHTS[i][j].Decay();

		//carga energia!!!
		if(MESHALIGHTS[i][j].Pressed!=MO_RELEASED) MESHALIGHTS[i][j].EnergyPulse();
		

		if(MESHALIGHTS[i][j].Energy>=MESHALIGHTS[i][j].E_excited) {
			glLoadIdentity();
			al = 1.0;
			if(MESHALIGHTS[i][j].Energy<MESHALIGHTS[i][j].E_explosion) 
					al =(MESHALIGHTS[i][j].Energy -  MESHALIGHTS[i][j].E_excited)/(MESHALIGHTS[i][j].E_explosion -  MESHALIGHTS[i][j].E_excited);
			//glScalef(1.0*state.amplitude*cos(state.tempo.ang),1.0*state.amplitude*cos(state.tempo.ang),1.0*state.amplitude);
		    glColor4f(  m_Config.GetParam(color).GetValue().GetSubValue(MO_RED).Float()*state.tintr,
				m_Config.GetParam(color).GetValue().GetSubValue(MO_GREEN).Float()*state.tintg,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_BLUE).Float()*state.tintb,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_ALPHA).Float()*state.alpha*al);

			glTranslatef(MESHALIGHTS[i][j].P0X,MESHALIGHTS[i][j].P0Y,MESHALIGHTS[i][j].P0Z);
			glRotatef(  mov.movx(m_Config.GetParam(rotatx), state.tempo), 1.0, 0.0, 0.0 );
			glRotatef(  mov.movy(m_Config.GetParam(rotaty), state.tempo), 0.0, 1.0, 0.0 );
			glRotatef(  mov.movz(m_Config.GetParam(rotatz), state.tempo)*rand(), 0.0, 0.0, 1.0 );
			glScalef(   mov.movz(m_Config.GetParam(scalex), state.tempo),
						mov.movz(m_Config.GetParam(scaley), state.tempo),
						mov.movz(m_Config.GetParam(scalez), state.tempo));
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0);
				glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, 0);
				glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, 0);
				glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, 0);
			glEnd();

		}
		/*
			glLoadIdentity();
			al =(MESHALIGHTS[i][j].Energy -  MESHALIGHTS[i][j].E_stable)/(MESHALIGHTS[i][j].E_explosion -  MESHALIGHTS[i][j].E_stable);
			glColor4f(  m_Config.GetParam(color).GetValue().GetSubValue(MO_RED)*state.tintr,
				m_Config.GetParam(color).GetValue().GetSubValue(MO_GREEN)*state.tintg,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_BLUE)*state.tintb,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_ALPHA)*state.alpha);
			glScalef(0.1,0.1,0.1);
			glTranslatef(MESHALIGHTS[i][j].P0X,MESHALIGHTS[i][j].P0Y,MESHALIGHTS[i][j].P0Z);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0);
				glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, 0);
				glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, 0);
				glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, 0);
			glEnd();
		*/

		MESHALIGHTS[i][j].Pressed = MO_RELEASED;//desactivamos, si la tecla sigue activada se vovlera a presionar

	}
	}
	
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
}


void
moEffectLightPercussion::Interaction(moIODeviceManager *IODeviceManager) {
	//aqui se parsean los codisp
	moDeviceCode *temp;
	MOint did,cid,state,valor;

	MOint d,i2;

	if(devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->array[did]->GetStatus(cid);
			valor = IODeviceManager->array[did]->GetValue(cid);
			if(state) {
				//switch(i) {
				//	default:
						d = i / 10;
						i2 = i - d*10;
						MESHALIGHTS[i2*(Mwidth/(10))][d*(Mheight/(4))].Pressed = MO_PRESSED;
						MODebug->Push(moText("tecla presionada")); //(by Andres)
				//		break;
				//}
						
			}
		temp = temp->next;
		}
	}
}


MOboolean moEffectLightPercussion::Finish()
{
	Images.Finish();
    return PreFinish();
}
