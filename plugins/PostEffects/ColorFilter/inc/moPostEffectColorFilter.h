/*******************************************************************************

                      moPostEffectColorFilter.h

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

#ifndef __MO_POST_EFFECT_COLORFILTER_H__
#define __MO_POST_EFFECT_COLORFILTER_H__

#include "moPostPlugin.h"
#include "moTextureFilterIndex.h"

enum moColorFilterIndex {
	COLORFILTER_COLOR,
	COLORFILTER_BLENDING,
	COLORFILTER_SENSIBILITY,
	COLORFILTER_SYNCRO
};

#define MO_COLORFILTER_BRIGHTNESS	0
#define MO_COLORFILTER_CONTRAST		1
#define MO_COLORFILTER_INTENSITY	2
#define MO_COLORFILTER_SATURATION	3
#define MO_COLORFILTER_HUE			4
#define MO_COLORFILTER_DEFAULT		5

#ifndef MO_RED
#define MO_RED      0
#define MO_GREEN    1
#define MO_BLUE     2
#define MO_ALPHA    3
#endif

class moPostEffectColorFilter : public moPostEffect
{
public:
    //config

    moPostEffectColorFilter();
    virtual ~moPostEffectColorFilter();

    MOboolean Init();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
	void Interaction( moIODeviceManager *IODeviceManager );
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

protected:
	int sWidth, sHeight;
	MOuint tex_glid;

	float color[4];
	int blending;
	float sensibility;
	
    moTextureIndex Images;
	moTextureFilterIndex filters;

	moColorMatrix ColorMatrix;
	moHueSatIntMatrix HSIMatrix;
	moBrightContMatrix BCMatrix;
	moTextFilterParam FilterParams;
	float intensity0, saturation0, hue0;
	float intensity, saturation, hue;
	float brightness0, contrast0;
	float brightness, contrast;

	void UpdateParameters();
	void SetBlendMode();
};

class moPostEffectColorFilterFactory : public moPostEffectFactory
{
public:
    moPostEffectColorFilterFactory() {}
    virtual ~moPostEffectColorFilterFactory() {}
    moPostEffect* Create();
    void Destroy(moPostEffect* fx);
};

extern "C"
{
MO_PLG_API moPostEffectFactory* CreatePostEffectFactory();
MO_PLG_API void DestroyPostEffectFactory();
}
#endif
