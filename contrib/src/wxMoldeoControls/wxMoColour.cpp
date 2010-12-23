#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxMoColour.h"

#include <wx/file.h>
#include <wx/dcbuffer.h>



wxMoColour::wxMoColour() {
    m_ColourMode = RGBA;
    UpdateColour( wxColour(0,0,0) );

}

wxMoColour::wxMoColour( const wxColour& col, ColourMode mode ) {
    m_ColourMode = mode;
    UpdateColour( col );
}

wxMoColour::wxMoColour( wxColour::ChannelType red, wxColour::ChannelType green, wxColour::ChannelType blue, wxColour::ChannelType alpha ) {

    m_ColourMode = RGBA;
    UpdateColour( wxColour(red,green,blue,alpha) );

}

wxMoColour::wxMoColour( double hue, double saturation, double value, wxColour::ChannelType alpha ) {

    m_HSVValue.hue = hue;
    m_HSVValue.saturation = saturation;
    m_HSVValue.value = value;

    ///internal RGB values
    ///m_RGBValue = wxImage::HSVtoRGB( m_HSVValue );
    ///m_RGBValue = wxImage::HSVtoRGB( m_HSVValue );
    HSVtoRGB( (short)(m_HSVValue.hue*359),
              (short)(m_HSVValue.saturation*255.0),
              (short)(m_HSVValue.value*255.0),
              (float*)&m_RGBDouble.red,
              (float*)&m_RGBDouble.green,
              (float*)&m_RGBDouble.blue );
    m_RGBValue.red = m_RGBDouble.red;
    m_RGBValue.green = m_RGBDouble.green;
    m_RGBValue.blue = m_RGBDouble.blue;

    ///assign now
    m_ColourValue = wxColour( m_RGBValue.red, m_RGBValue.green, m_RGBValue.blue, alpha );
}

wxMoColour::wxMoColour( double hue, double saturation, double value, double alpha )
{
    (*this) = wxMoColour( hue, saturation, value );
    m_Alpha = (unsigned char) std::max( 0, (int)std::min( 255.0, alpha*255.0) );
    m_ColourValue = wxColour( m_RGBValue.red, m_RGBValue.green, m_RGBValue.blue, m_Alpha );
}


wxMoColour::~wxMoColour() {

}

void
wxMoColour::UpdateColour( wxColour p_Colour ) {

  ///internal RGB values
  m_RGBValue.red = p_Colour.Red();
  m_RGBValue.green = p_Colour.Green();
  m_RGBValue.blue = p_Colour.Blue();

  m_RGBDouble.red = m_RGBValue.red / 255.0;
  m_RGBDouble.green = m_RGBValue.green / 255.0;
  m_RGBDouble.blue = m_RGBValue.blue / 255.0;

  ///internal HSV values
  m_HSVValue = wxImage::RGBtoHSV( m_RGBValue );

  ///assign now
  m_ColourValue = p_Colour;
}

void wxMoColour::UpdateColour( wxMoColour p_MoColour ) {

    switch( p_MoColour.Model() ) {

      case RGBA:
        UpdateColour( p_MoColour.m_ColourValue );
        break;

      case HSV:
        ///internal HSV values
        m_HSVValue.hue = p_MoColour.Hue();
        m_HSVValue.saturation = p_MoColour.Saturation();
        m_HSVValue.value = p_MoColour.Value();

        ///internal RGB values
        HSVtoRGB( (short)(m_HSVValue.hue*359),
              (short)(m_HSVValue.saturation*255.0),
              (short)(m_HSVValue.value*255.0),
              (float*)&m_RGBDouble.red,
              (float*)&m_RGBDouble.green,
              (float*)&m_RGBDouble.blue );
        m_RGBValue.red = m_RGBDouble.red;
        m_RGBValue.green = m_RGBDouble.green;
        m_RGBValue.blue = m_RGBDouble.blue;

        ///assign now
        m_ColourValue = wxColour( m_RGBValue.red, m_RGBValue.green, m_RGBValue.blue, p_MoColour.Alpha() );
        break;

    }

}

double wxMoColour::RedNormal() {
    return m_RGBDouble.red;
}

double wxMoColour::GreenNormal() {
    return m_RGBDouble.green;
}

double wxMoColour::BlueNormal() {
    return m_RGBDouble.blue;
}


wxColour::ChannelType wxMoColour::Red()  {
    return m_RGBDouble.red;
}

wxColour::ChannelType wxMoColour::Green()  {
    return m_RGBValue.green;
}

wxColour::ChannelType wxMoColour::Blue()  {
    return m_RGBValue.blue;
}

wxColour::ChannelType wxMoColour::Alpha() {
    return m_Alpha;
}

double wxMoColour::Hue() {
    return m_HSVValue.hue;
}

double wxMoColour::Saturation() {
    return m_HSVValue.saturation;
}

double wxMoColour::Value() {
    return m_HSVValue.value;
}

wxMoColour::ColourMode wxMoColour::Model() {
    return m_ColourMode;
}

wxMoColour::ColourMode wxMoColour::Mode() {
    return m_ColourMode;
}


void wxMoColour::HSVtoRGB(short iInHue, short iInSaturation, short iInValue, float* piRed, float* piGreen, float* piBlue)
{
	if(iInSaturation == 0) {
		float fInValue;
		fInValue =(float)(iInValue) / 255.0;
		*piRed = fInValue;
		*piGreen = fInValue;
		*piBlue = fInValue;
	} else {
		float fHue, fValue, fSaturation;
		int i;
		float f;
		float p,q,t;

		if(iInHue == 360) iInHue = 0;

		fHue =(float)iInHue / 60;

		i = (int)fHue;
		f = fHue -(float)i;

		fValue =(float)iInValue / 255;
		fSaturation =(float)iInSaturation / 255;
		p = fValue *(1.0 - fSaturation);
		q = fValue *(1.0 -(fSaturation *f));
		t = fValue *(1.0 -(fSaturation *(1.0 - f)));

		switch(i) {
			case 0:
				*piRed = fValue ;//* 255;
				*piGreen = t ;//* 255;
				*piBlue = p ;//* 255;
				break;
			case 1:
				*piRed = q ;//* 255;
				*piGreen = fValue;// * 255;
				*piBlue = p ;//* 255;
				break;
			case 2:
				*piRed = p ;//* 255;
				*piGreen = fValue ;//* 255;
				*piBlue = t ;//* 255;
				break;
			case 3:
				*piRed = p ;//* 255;
				*piGreen = q ;//* 255;
				*piBlue = fValue ;//* 255;
				break;
			case 4:
				*piRed = t ;//* 255;
				*piGreen = p ;//* 255;
				*piBlue = fValue ;//* 255;
				break;
			case 5:
				*piRed = fValue ;//* 255;
				*piGreen = p ;//* 255;
				*piBlue = q ;//* 255;
				break;
		}
	}
}


