/*******************************************************************************

                              moEffectImageFlow.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                     *
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

  Copyright(C) 2007 Andrés Colubri

  Authors:
  Andrés Colubri
  Fabricio Costa

*******************************************************************************/

#include "moEffectImageFlow.h"

//========================
//  Factory
//========================

moEffectImageFlowFactory *m_EffectImageFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectImageFactory==NULL)
		m_EffectImageFactory = new moEffectImageFlowFactory();
	return(moEffectFactory*) m_EffectImageFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectImageFactory;
	m_EffectImageFactory = NULL;
}

moEffect*  moEffectImageFlowFactory::Create() {
	return new moEffectImageFlow();
}

void moEffectImageFlowFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectImageFlow::moEffectImageFlow() {
	SetName("imageflow");
}

moEffectImageFlow::~moEffectImageFlow() {
	Finish();
}

MOboolean
moEffectImageFlow::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( IMAGEFLOW_ALPHA, moText("alpha") );
	moDefineParamIndex( IMAGEFLOW_COLOR, moText("color") );
	moDefineParamIndex( IMAGEFLOW_SYNCRO, moText("syncro") );
	moDefineParamIndex( IMAGEFLOW_PHASE, moText("phase") );
	moDefineParamIndex( IMAGEFLOW_BLENDING, moText("blending") );
	moDefineParamIndex( IMAGEFLOW_IMAGES, moText("images") );
	moDefineParamIndex( IMAGEFLOW_VELOCITY, moText("velocity") );

	updateParameters();

	minx = 0.0;
	maxx = m_pResourceManager->GetRenderMan()->ScreenWidth();
	miny = 0.0;
	maxy = m_pResourceManager->GetRenderMan()->ScreenHeight();

	flow_mode = 0;
	if (0 < flow_velocity0)
	{
		flow_coord = minx;
	    idx0 = 1;
	    idx1 = 0;
	}
	else
	{
		flow_coord = maxx;
	    idx0 = 0;
	    idx1 = 1;
	}

	Images.Init( GetConfig(), m_Config.GetParamIndex("images"), m_pResourceManager->GetTextureMan() );
	tex0 = Images.GetGLId(1);
	tex1 = Images.GetGLId(0);

    flow_velocity_bak = flow_velocity = flow_velocity0;

	return true;
}

void moEffectImageFlow::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw(tempogral, parentstate);

	minx = 0.0;
	maxx = m_pResourceManager->GetRenderMan()->ScreenWidth();
	miny = 0.0;
	maxy = m_pResourceManager->GetRenderMan()->ScreenHeight();



    glDisable(GL_DEPTH_TEST);
	glClear ( GL_DEPTH_BUFFER_BIT );
	m_pResourceManager->GetGLMan()->SetOrthographicView(maxx, maxy);

    SetColor( m_Config[moR(IMAGEFLOW_COLOR)][MO_SELECTED], m_Config[moR(IMAGEFLOW_ALPHA)][MO_SELECTED], state );

    SetBlending( (moBlendingModes) m_Config[moR(IMAGEFLOW_BLENDING)][MO_SELECTED][0].Int() );

	float t;

	updateParameters( );

	flow_velocity_bak = flow_velocity;
	if (state.tempo.delta <= 1.0) flow_velocity = state.tempo.delta * flow_velocity0;
	else flow_velocity = (1.0 + 10.0 * (state.tempo.delta - 1.0)) * flow_velocity0;
	if (flow_velocity != flow_velocity_bak) MODebug2->Push(moText("Flow velocity: ") + FloatToStr(flow_velocity));

    //MODebug2->Push(moText("Flow velocity: ") + FloatToStr(flow_velocity));

    if (flow_mode == 0)
	{
        // horizontal flow.

		flow_coord = flow_coord + flow_velocity;

		if ((flow_coord < minx) || (maxx < flow_coord))
		{
			if (0 < flow_velocity)
			{
				idx1 = idx0;
				idx0 = (idx0 + 1) % Images.Count();
			}
			else
			{
                idx0 = idx1;
                idx1 = (idx1 + 1) % Images.Count();
			}
            tex0 = Images.GetGLId(idx0);
            tex1 = Images.GetGLId(idx1);

			if (flow_coord < minx) flow_coord = maxx;
			if (maxx < flow_coord) flow_coord = minx;
		}

        t = 1.0 - (flow_coord  - minx) / (maxx - minx);

		glBindTexture(GL_TEXTURE_2D, tex0);
        glBegin(GL_QUADS);
            glTexCoord2f(t, 0.0);
            glVertex2f(minx, miny);

            glTexCoord2f(t, 1.0);
            glVertex2f(minx, maxy);

            glTexCoord2f(1.0, 1.0);
            glVertex2f(flow_coord, maxy);

            glTexCoord2f(1.0, 0.0);
            glVertex2f(flow_coord, miny);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex1);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0);
            glVertex2f(flow_coord, miny);

            glTexCoord2f(0.0, 1.0);
            glVertex2f(flow_coord, maxy);

		    glTexCoord2f(t, 1.0);
            glVertex2f(maxx, maxy);

            glTexCoord2f(t, 0.0);
            glVertex2f(maxx, miny);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else
	{

        // vertical flow.
	}

}


void moEffectImageFlow::Interaction( moIODeviceManager *IODeviceManager ) {
}

MOboolean moEffectImageFlow::Finish()
{
    return PreFinish();
}

void moEffectImageFlow::updateParameters()
{
	flow_velocity0 = m_Config[moR(IMAGEFLOW_VELOCITY)].GetData()->Fun()->Eval(state.tempo.ang);
}

moConfigDefinition *
moEffectImageFlow::GetDefinition( moConfigDefinition *p_configdefinition ) {
	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("images"), MO_PARAM_TEXTURE, IMAGEFLOW_IMAGES );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, IMAGEFLOW_BLENDING );
	p_configdefinition->Add( moText("velocity"), MO_PARAM_FUNCTION, IMAGEFLOW_VELOCITY, moValue("2.0", "FUNCTION").Ref() );
	return p_configdefinition;
}
