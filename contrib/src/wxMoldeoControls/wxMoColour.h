#ifndef WX_MOCOLOUR_H
#define WX_MOCOLOUR_H

#ifdef WXMAKINGDLL_MOCOLOUR
    #define WXDLLIMPEXP_MOCOLOUR WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_MOCOLOUR WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_MOCOLOUR
#endif

#include <wx/image.h>


/// Class defining extended colour data
/**
* This class was designed to deal with
* the differents color models as RGB, HSV, HSL, etc
*
*/
class WXDLLIMPEXP_MOCOLOUR wxMoColour {

  public:

    /// Enumerate the different control modes
    /**
    *   Set the actual mode, if not set
    */
    enum ColourMode {
        RGBA = 0,
        HSV = 1,
        HSL = 2
    };

    /// Enumerate the different control modes
    /**
    *   Set the actual mode, if not set
    */
    enum ColourModeIntMax {
        MAX_RED = 255,
        MAX_GREEN = 255,
        MAX_BLUE = 255,
        MAX_ALPHA = 255,
        MAX_HUE = 359,
        MAX_SATURATION = 255,
        MAX_VALUE = 255
    };

    /// Enumerate the different control levels
    /**
    *   Set the actual channel, if not set
    */
    enum ColourChannel {
        RED = 0,
        GREEN = 1,
        BLUE = 2,
        ALPHA = 3,
        HUE = 4,
        SATURATION = 5,
        VALUE = 6
    };

    typedef struct {
      double red;
      double green;
      double blue;

    } RGBDouble;



    wxMoColour();
    wxMoColour( const wxColour& col, ColourMode mode=RGBA );
    wxMoColour( wxColour::ChannelType red, wxColour::ChannelType green=0, wxColour::ChannelType blue=0, wxColour::ChannelType alpha=wxALPHA_OPAQUE );
    wxMoColour( double hue, double saturation, double value, wxColour::ChannelType alpha=wxALPHA_OPAQUE );
    wxMoColour( double hue, double saturation, double value, double alpha );
    virtual ~wxMoColour();

    /// Update this colour with new one
    /**
    *   Update the current colour with wxColour rgba values for argument.
    */
    void UpdateColour( wxColour p_Colour );

    /// Update this colour with new one, could be a HSV colour
    /**
    *   Update the current colour with wxMoColour rgba values or hsva values for argument
    */
    void UpdateColour( wxMoColour p_MoColour );

    wxColour::ChannelType Red();
    wxColour::ChannelType Green();
    wxColour::ChannelType Blue();
    wxColour::ChannelType Alpha();
    double RedNormal();
    double GreenNormal();
    double BlueNormal();

    double Hue();
    double Saturation();
    double Value();

    ColourMode Model();
    ColourMode Mode();
    static void HSVtoRGB(short iInHue, short iInSaturation, short iInValue, float* piRed, float* piGreen, float* piBlue);

  protected:



    /// Colour mode RGB, HSV, HSL
    /**
    * Colour mode is by default RGB colour model but can be changed to HSL (CSV)
    *
    */
    ColourMode          m_ColourMode;
    wxColour            m_ColourValue;

    RGBDouble           m_RGBDouble;
    wxImage::RGBValue   m_RGBValue;
    wxImage::HSVValue   m_HSVValue;
    unsigned int        m_Alpha;


};


#endif
