#ifndef WX_MOCOLOURPANEL_H
#define WX_MOCOLOURPANEL_H

#ifdef WXMAKINGDLL_MOCOLOURPANEL
    #define WXDLLIMPEXP_MOCOLOURPANEL WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_MOCOLOURPANEL WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_MOCOLOURPANEL
#endif

#include <wx/image.h>
#include <wx/bitmap.h>

#include "wxMoColour.h"

class wxMoColourPanel : public wxPanel, public wxMoColour {

    public:

        wxMoColourPanel( wxWindow* parent,
          wxWindowID id = wxID_ANY,
          const wxPoint& pos = wxDefaultPosition,
          const wxSize& size = wxDefaultSize,
          long style = wxTAB_TRAVERSAL,
          const wxString& name = wxPanelNameStr );

        virtual ~wxMoColourPanel();

        void SetColourValue( const wxColour &p_Colour );

        void SetBitmap( const wxBitmap& bitmap );

        void OnSize( wxSizeEvent& event );

    protected:

        void UpdatePattern();
        wxBitmap m_Pattern;
        void  ResizeBitmap();

        virtual void OnPaint(wxPaintEvent& event);


        wxBitmap    m_Bitmap;
        wxBitmap    m_BitmapOriginal;

        DECLARE_EVENT_TABLE()
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_MOCOLOURPANEL, wxEVT_MOCOLOURPANEL, 7777)
END_DECLARE_EVENT_TYPES()


#endif
