#ifndef WX_MOCOLOURLEVELCTRL_H
#define WX_MOCOLOURLEVELCTRL_H

#ifdef WXMAKINGDLL_MOCOLOURLEVELCTRL
    #define WXDLLIMPEXP_MOCOLOURLEVELCTRL WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_MOCOLOURLEVELCTRL WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_MOCOLOURLEVELCTRL
#endif

#include <wx/image.h>
#include <wx/bitmap.h>
#include "wxMoLevelCtrl.h"
#include "wxMoColour.h"
#include "wxMoColourPanel.h"

#define wxMoColorLevelCtrl wxMoColourLevelCtrl

#define MAX_HUE_FLOAT 0.998f


class wxMoColourLevelCtrl : public wxMoLevelCtrl, public wxMoColour {

    public:

        wxMoColourLevelCtrl( wxWindow* parent,
          wxWindowID id,
          const int value,
          const int minvalue,
          const int maxvalue,
          const wxPoint& pos = wxDefaultPosition,
          const wxSize& size = wxDefaultSize,
          long style = 0,
          const wxValidator& validator = wxDefaultValidator,
          const wxString& name = wxPanelNameStr );

        virtual ~wxMoColorLevelCtrl();

        /// Set the actual channel control and initialize the control
        /**
        *   Set the actual channel and initialize the control
        */
        void Init( ColourChannel p_channel = RED );


        ColourChannel GetColourChannel() {
          return m_ColourChannel;
        }

        wxImage::RGBValue GetRGBValue() {
            UpdateColour( m_ColourValue );
            return m_RGBValue;
        }

        wxImage::HSVValue GetHSVValue() {
            UpdateColour( m_ColourValue );
            return m_HSVValue;
        }


        /// Ask for initialization
        /**
        *   Ask for initialization
        */
        void IsInit();


        /// Set the minimum color
        /**
        *   Set the left minimum color defined where the gradients begin
        */
        virtual void SetMinColor( wxColour p_min_color );

        /// Set the maximum color
        /**
        *   Set the right maximum color defined where the gradients ends
        */
        virtual void SetMaxColor( wxColour p_max_color );


        /// Set the actual colour value
        /**
        *   Set the actual colour value
        */
        virtual void SetColourValue( wxColour p_ColourValue, bool propagate = false );


        /// Get the actual colour value
        /**
        *   Get the actual colour value
        */
        virtual wxColour GetColourValue();


        /// Set the red value
        /**
        *   Set the red channel value
        *   @param red  red channel value (0..255)
        */
        virtual void SetRedValue( wxColour::ChannelType red, bool propagate = false );

        /// Set the green value
        /**
        *   Set the green channel value
        *   @param green  green channel value (0..255)
        */
        virtual void SetGreenValue( wxColour::ChannelType green, bool propagate = false );

        /// Set the blue value
        /**
        *   Set the blue channel value
        *   @param blue  blue channel value (0..255)
        */
        virtual void SetBlueValue( wxColour::ChannelType blue, bool propagate = false );

        /// Set the alpha value
        /**
        *   Set the alpha channel value
        *   @param blue  alpha channel value (0..255)
        */
        virtual void SetAlphaValue( wxColour::ChannelType alpha, bool propagate = false );

        /// Set the hue value
        /**
        *   Set the hue channel value
        *   @param hue  hue channel value (0..359)
        */
        virtual void SetHueValue( unsigned int p_hue_value, bool propagate = false );

        /// Set the saturation value
        /**
        *   Set the saturation channel value
        *   @param saturation  saturation channel value (0..255)
        */
        virtual void SetSaturationValue( wxColour::ChannelType p_saturation_value, bool propagate = false );

        /// Set the value value
        /**
        *   Set the value channel value
        *   @param value  value channel value (0..255)
        */
        virtual void SetValueValue( wxColour::ChannelType p_value_value, bool propagate = false );



        /// Set RGBA Color Components Controls
        /**
        * Set the level controls for each components in RGBA model
        * Auto propagation is avoided
        *
        * @param red red control pointer
        * @param green green control pointer
        * @param blue blue control pointer
        * @param alpha alpha control pointer
        */
        virtual void SetRGBAColorComponents( wxMoColorLevelCtrl* p_red_level=NULL, wxMoColorLevelCtrl* p_green_level=NULL, wxMoColorLevelCtrl* p_blue_level=NULL, wxMoColorLevelCtrl* p_alpha_level=NULL );


        /// Set HSV Color Components Controls
        /**
        * Set the level controls for each components in HSV model
        * Auto propagation is avoided
        *
        * @param red red control pointer
        * @param green green control pointer
        * @param blue blue control pointer
        */
        virtual void SetHSVColorComponents( wxMoColorLevelCtrl* p_hue_level=NULL, wxMoColorLevelCtrl* p_saturation_level=NULL, wxMoColorLevelCtrl* p_value_level=NULL );

        /// Set Colour Panel
        /**
        * Set the colour panel pointer to reflect colour
        *
        * @param red red control pointer, if null means its dynamic
        */
        virtual void SetColourPanel( wxMoColourPanel* p_colourpanel );


    protected:

        bool  m_Init;

        ColourChannel m_ColourChannel;

        wxBitmap m_Pattern;
        wxBitmap   m_AlphaMask;

        wxColour m_ColourMin;
        wxColour m_ColourMax;

        wxMoColorLevelCtrl* m_pRedLevel;
        wxMoColorLevelCtrl* m_pGreenLevel;
        wxMoColorLevelCtrl* m_pBlueLevel;

        wxMoColorLevelCtrl* m_pHueLevel;
        wxMoColorLevelCtrl* m_pSaturationLevel;
        wxMoColorLevelCtrl* m_pValueLevel;

        wxMoColorLevelCtrl* m_pAlphaLevel;
        wxMoColourPanel*    m_pColourPanel;

        /**
        * Function to propagate colour changes to other controls
        */
        virtual void Propagate();
        virtual void ChangeValue( int value );
        virtual void CreatePattern();
        virtual void UpdatePattern();
        virtual void OnPaint(wxPaintEvent& event);

        void ProcessColour();

        DECLARE_EVENT_TABLE()
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_MOCOLOURLEVELCTRL, wxEVT_MOCOLOURLEVELCTRL, 7777)
END_DECLARE_EVENT_TYPES()


#endif
