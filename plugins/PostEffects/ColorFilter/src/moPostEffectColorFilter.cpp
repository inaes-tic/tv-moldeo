/*******************************************************************************

                      moPostEffectColorFilter.cpp

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

#include "moPostEffectColorFilter.h"

//========================
//  Factory
//========================

moPostEffectColorFilterFactory *m_PostEffectColorFilterFactory = NULL;

MO_PLG_API moPostEffectFactory* CreatePostEffectFactory(){ 
	if(m_PostEffectColorFilterFactory==NULL)
		m_PostEffectColorFilterFactory = new moPostEffectColorFilterFactory();
	return(moPostEffectFactory*) m_PostEffectColorFilterFactory;
} 

MO_PLG_API void DestroyPostEffectFactory(){ 
	delete m_PostEffectColorFilterFactory;
	m_PostEffectColorFilterFactory = NULL;
} 

moPostEffect*  moPostEffectColorFilterFactory::Create() {
	return new moPostEffectColorFilter();
}

void moPostEffectColorFilterFactory::Destroy(moPostEffect* fx) {
	delete fx;
}

//========================
//  PostEfecto
//========================
moPostEffectColorFilter::moPostEffectColorFilter() {
	devicecode = NULL;
	ncodes = 0;
	m_Name = "colorfilter";
	m_Type = MO_OBJECT_EFFECT;
}

moPostEffectColorFilter::~moPostEffectColorFilter() {
	Finish();
}

MOboolean
moPostEffectColorFilter::Init()
{
    if (!PreInit()) return false;

	moDefineParamIndex( COLORFILTER_COLOR, moText("color") );
	moDefineParamIndex( COLORFILTER_BLENDING, moText("blending") );
	moDefineParamIndex( COLORFILTER_SENSIBILITY, moText("sensibility") );
	moDefineParamIndex( COLORFILTER_SYNCRO, moText("syncro") );
	UpdateParameters();

	moTextArray filter_str;
	filter_str.Init(1, moText(""));
	filter_str.Set(0, moText("effects_texture shaders/ColorMatrix.cfg colorfilter_texture"));
	bool res = filters.Init(&filter_str, m_pResourceManager->GetGLMan(), m_pResourceManager->GetFBMan(), m_pResourceManager->GetShaderMan(), m_pResourceManager->GetTextureMan(), m_pResourceManager->GetRenderMan());

	int tex_moid = m_pResourceManager->GetTextureMan()->GetTextureMOId("colorfilter_texture", false);
	tex_glid = m_pResourceManager->GetTextureMan()->GetGLId(tex_moid);

	if (!res)
	{
		MODebug->Push("Error: cannot load shader in colorfilter");
		return false;
	}

	FilterParams = MOEmptyTexFilterParam;
	ColorMatrix.Init();
	HSIMatrix.Init(true, 0.0, 4.0, 0.0, 4.0);
	BCMatrix.Init(-1.0, 1.0, 0.0, 4.0);
	intensity0 = saturation0 = intensity = saturation = 0.25;
	hue0 = hue = 0.0;
	brightness0 = brightness = 0.5;
	contrast0 = contrast = 0.25;
	HSIMatrix.Update(intensity, saturation, hue);
	BCMatrix.Update(brightness, contrast);
	ColorMatrix.Copy(HSIMatrix);
	ColorMatrix.Multiply(BCMatrix);
	FilterParams.par_mat4 = ColorMatrix.GetMatrixPointer();

    sWidth = m_pResourceManager->GetRenderMan()->RenderWidth();
    sHeight = m_pResourceManager->GetRenderMan()->RenderHeight();

    if(m_Config.GetPreConfCount() > 0)
        m_Config.PreConfFirst();
		
	return true;
}

void moPostEffectColorFilter::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);
	UpdateParameters();

	filters.Apply(&state.tempo, FilterParams);	

    glDisable(GL_DEPTH_TEST);
	glClear (GL_DEPTH_BUFFER_BIT);
	m_pResourceManager->GetGLMan()->SetOrthographicView(sWidth, sHeight);

    glColor4f(color[0] * state.tintr,
              color[1] * state.tintg,
              color[2] * state.tintb,
              color[3] * state.alpha);


	SetBlendMode();

	glBindTexture(GL_TEXTURE_2D, tex_glid);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, 0.0);

        glTexCoord2f(0.0, 1.0);
        glVertex2f(0.0, sHeight);

        glTexCoord2f(1.0, 1.0);
        glVertex2f(sWidth, sHeight);

        glTexCoord2f(1.0, 0.0);
        glVertex2f(sWidth, 0.0);
	glEnd();
}

void moPostEffectColorFilter::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moEffect::Interaction( IODeviceManager );

	if (devicecode!=NULL)
	for(int i=0; i<ncodes; i++) {
		
		temp = devicecode[i].First;

		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);
			if (state)
			switch(i) {
				case MO_COLORFILTER_BRIGHTNESS:
					brightness0 = brightness;
					brightness -= ((float) valor / (float) sensibility);
					brightness = min(1.0, brightness);
					brightness = max(0.0, brightness);
					//MODebug->Push("brightness: " + FloatToStr(brightness));
					break;
				case MO_COLORFILTER_CONTRAST:
					contrast0 = contrast;
					contrast += ((float) valor / (float) sensibility);
					contrast = min(1.0, contrast);
					contrast = max(0.0, contrast);
					//MODebug->Push("contrast: " + FloatToStr(contrast));
					break;
				case MO_COLORFILTER_INTENSITY:
					intensity0 = intensity;
					intensity -= ((float) valor / (float) sensibility);
					intensity = min(1.0, intensity);
					intensity = max(0.0, intensity);
					//MODebug->Push("intensity: " + FloatToStr(intensity));
					break;
				case MO_COLORFILTER_SATURATION:
					saturation0 = saturation;
					saturation += ((float) valor / (float) sensibility);
					saturation = min(1.0, saturation);
					saturation = max(0.0, saturation);
					//MODebug->Push("saturation: " + FloatToStr(saturation));
					break;
				case MO_COLORFILTER_HUE:
					hue0 = hue;
					hue -= ((float) valor / (float) sensibility);
					hue = min(1.0, hue);
					hue = max(0.0, hue);
					//MODebug->Push("hue: " + FloatToStr(hue));
					break;
				case MO_COLORFILTER_DEFAULT:
					intensity = saturation = 0.25;
					hue = 0.0;
					brightness = 0.5;
					contrast = 0.25;
					break;
			}
		temp = temp->next;
		}
	}

	MOboolean chg = false;
	if ((intensity0 != intensity) || (saturation0 != saturation) || (hue0 != hue))
	{
	    HSIMatrix.Update(intensity, saturation, hue);
		chg = true;
	}

	if ((brightness0 != brightness) || (contrast0 != contrast))
	{
	    BCMatrix.Update(brightness, contrast);
		chg = true;
	}

	if (chg)
	{
		ColorMatrix.Copy(HSIMatrix);
		ColorMatrix.Multiply(BCMatrix);
	}
}

MOboolean moPostEffectColorFilter::Finish()
{
    return PreFinish();
}

moConfigDefinition *
moPostEffectColorFilter::GetDefinition( moConfigDefinition *p_configdefinition ) {
	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("color"), MO_PARAM_COLOR, COLORFILTER_COLOR );
	p_configdefinition->Add( moText("blending"), MO_PARAM_FUNCTION, COLORFILTER_BLENDING );
	p_configdefinition->Add( moText("sensibility"), MO_PARAM_FUNCTION, COLORFILTER_SENSIBILITY );
	p_configdefinition->Add( moText("syncro"), MO_PARAM_FUNCTION, COLORFILTER_SYNCRO );
	return p_configdefinition;
}

void moPostEffectColorFilter::UpdateParameters()
{
	for (int i = MO_RED; i <= MO_ALPHA; i++)
		color[i] = m_Config[moR(COLORFILTER_COLOR)][MO_SELECTED][i].Float();
	blending = m_Config[moR(COLORFILTER_BLENDING)].GetData()->Float();
	sensibility = m_Config[moR(COLORFILTER_SENSIBILITY)].GetData()->Float();
}

void moPostEffectColorFilter::SetBlendMode()
{
    glEnable(GL_BLEND);
	switch (blending) 
	{
		//ALPHA DEPENDENT
		case 0:
			//TRANSPARENCY [Rs * As] + [Rd *(1 -As)] = As*(Rs-Rd) + Rd			
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			break;
		case 1:
			//ADDITIVE WITH TRANSPARENCY: Rs*As + Rd*Ad
			glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
			break;

		//NON ALPHA
		case 2:
			//MIXING [Rs *( 1 - Rd )] + [ Rd * 1] = Rs + Rd - Rs*Rd
			//additive without saturation
			glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ONE );
			break;
		case 3:
			//MULTIPLY: [Rs * Rd] + [Rd * 0] = Rs * Rd 
			glBlendFunc( GL_DST_COLOR, GL_ZERO );
			break;
		case 4:
			//EXCLUSION: [Rs *(1 - Rd)] + [Rd *(1 - Rs)] = Rs + Rd - 2*Rs*Rd
			glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);//
			break;
		case 5:
			//ADDITIVE Rs+Rd
			glBlendFunc( GL_ONE, GL_ONE );		
			break;
		case 6:
			//OVERLAY: 2*Rs*Rd
			glBlendFunc( GL_DST_COLOR, GL_SRC_COLOR );				
			break;
		case 7:
			//SUBSTRACTIVE [Rs *( 1 - Rd )] + [ Rd * 0] = Rs - Rs*Rd
			//substractive
			glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ZERO );
			break;
		case 8:
			// [Rs * min(As,1-Ad) ] + [ Rd * Ad]
			//
			glBlendFunc( GL_SRC_ALPHA_SATURATE,  GL_DST_ALPHA);
			break;
			//Multiply mode:(a*b)
			//Average mode:(a+b)/2
			//Screen mode:  f(a,b) = 1 -(1-a) *(1-b)
			//Difference mode:  f(a,b) = |a - b|
			//Negation mode:  f(a,b) = 1 - |1 - a - b|
			//Exclusion mode f(a,b) = a + b - 2ab or f(a,b) = average(difference(a,b),negation(a,b))
			//Overlay mode f(a,b) =   	2ab(for a < ½) 1 - 2 *(1 - a) *(1 - b)(else)
			//Color dodge mode:  f(a,b) = a /(1 - b)
			//Color burn mode:  f(a,b) = 1 -(1 - a) / b
			//Inverse color dodge mode:  f(a,b) = b /(1 - a)
			//Inverse color burn mode:  f(a,b) = 1 -(1 - b) / a
		default:
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			break;
	}
}
