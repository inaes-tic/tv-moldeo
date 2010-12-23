#ifndef WXMOLEVELCTRL_H
#define WXMOLEVELCTRL_H

#ifdef WXMAKINGDLL_MOLEVELCTRL
    #define WXDLLIMPEXP_MOLEVELCTRL WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_MOLEVELCTRL WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_MOLEVELCTRL
#endif

class wxMoLevelCtrl : public wxControl {

    public:
        wxMoLevelCtrl( wxWindow* parent, wxWindowID id, const int value, const int minvalue, const int maxvalue, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxPanelNameStr );
        virtual ~wxMoLevelCtrl();

        virtual void SetRange( int min, int max );
        virtual void SetValue( int value );
        virtual void ChangeValue( int value );
        virtual int GetValue();
        virtual int SlidePosition();
        virtual int SlideValue( int pointerxposition );

    protected:

        int m_min;
        int m_max;
        int m_value;

        bool m_bStartDragging;
        int Px,Mx,Sx,Vx;

        virtual void OnPaint(wxPaintEvent& event);
        virtual void OnMouseEvent( wxMouseEvent & event );
        virtual void OnCaptureLostEvent( wxMouseCaptureLostEvent& event );
        virtual void OnCaptureChangedEvent( wxMouseCaptureChangedEvent& event );
        virtual void OnKillFocus( wxFocusEvent& event);

        DECLARE_EVENT_TABLE()
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_MOLEVELCTRL, wxEVT_MOLEVELCTRL, 7777)
END_DECLARE_EVENT_TYPES()

#endif
