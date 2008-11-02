#include "moWxLevelCtrl.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE( moWxLevelCtrl, wxControl )
    EVT_PAINT( moWxLevelCtrl::OnPaint )
    EVT_MOUSE_EVENTS( moWxLevelCtrl::OnMouseEvent )
    EVT_MOUSE_CAPTURE_LOST( moWxLevelCtrl::OnCaptureLostEvent )
    EVT_MOUSE_CAPTURE_CHANGED( moWxLevelCtrl::OnCaptureChangedEvent )
END_EVENT_TABLE()

moWxLevelCtrl::moWxLevelCtrl( wxWindow* parent, wxWindowID id, const int value, const int minvalue, const int maxvalue, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name  ) :
wxControl( parent, id,  pos, size, style | wxNO_BORDER, validator, name ) {
    m_min = minvalue;
    m_max = maxvalue;
    m_value = value;
    m_bStartDragging = false;
    SetBackgroundStyle(wxBG_STYLE_CUSTOM );

}

moWxLevelCtrl::~moWxLevelCtrl() {


}

void
moWxLevelCtrl::OnPaint(wxPaintEvent& event) {

    wxBufferedPaintDC dc(this);

    dc.SetBrush( wxBrush( wxColour(80,80,80), wxSOLID ) );
    dc.SetPen( wxPen( wxColour(255,255,255), 1, wxSOLID ) );

    ///background
    dc.DrawRectangle( 0, 0, GetSize().x, GetSize().y );

    ///gauge
    dc.SetBrush( wxBrush( wxColour(0,200,50), wxSOLID) );
    dc.SetPen( wxPen( wxColour(140,140,140), 1, wxTRANSPARENT ) );
    //dc.DrawRectangle( 1, 1, SlidePosition()-2, GetSize().y-2 );
    wxRect Rgauge( 1, 1, SlidePosition()-2, GetSize().y-2 );
    dc.GradientFillLinear( Rgauge, wxColour(0,50,0), wxColour(0,200,0) );

    ///ticks
    dc.SetBrush( wxBrush( wxColour(128,128,128), wxSOLID ) );
    dc.SetPen( wxPen( wxColour(255,255,255), 1, wxSOLID ) );
    int nticks = 5;
    int dticks = (GetSize().x / nticks );
    int ysize;

    for(int i =0; i<nticks; i++ ) {
        ( i % 3 ) == 0 ? ysize = GetSize().y / 2 : ysize = GetSize().y / 3;
        dc.DrawLine( i*dticks, ysize, i*dticks, GetSize().y );
    }


}


void moWxLevelCtrl::SetRange( int min, int max ) {
    if ( min >= max ) {
        wxMessageBox(wxT("moWxLevelCtrl error : min is inferior or equal to max"));
    }

    this->m_min = min;
    this->m_max = max;

}

void moWxLevelCtrl::ChangeValue( int value ) {
    if (value<=m_min) value = m_min;
    if (value>m_max) value = m_max;

    this->m_value = value;
}

void moWxLevelCtrl::SetValue( int value ) {

    ChangeValue(value);

    wxCommandEvent event( MO_EVT_DIRECTOR_LEVEL, m_windowId );
    event.SetInt( GetValue() );
    event.SetEventObject( this );

    ProcessEvent(event );

}


int moWxLevelCtrl::GetValue( ) {

    return m_value;

}

//from value to x coord position
int moWxLevelCtrl::SlidePosition() {

    int pWidth = m_max - m_min;
    if (pWidth<=0) pWidth = 1;

    int gsx = GetSize().x;
    if (gsx==0) gsx = 1;

    int slideposition = (( m_value - m_min ) * gsx ) / pWidth;

    return slideposition;
}

//from x coord position to value
int moWxLevelCtrl::SlideValue( int pointerxposition ) {

    int pWidth = m_max - m_min;
    if (pWidth<=0) pWidth = 1;


    int value = (pointerxposition * pWidth ) / GetSize().x;

    if (value<=0) value = 0;
    if (value>pWidth) value = pWidth;

    value+=m_min;
    return value;
}

void moWxLevelCtrl::OnMouseEvent( wxMouseEvent & event ) {

    //wxMessageBox("moWxLevelCtrl::Mouse event");

    if ( event.Entering() ) {
        //SHINE!!!

    }
    if (event.Button(wxMOUSE_BTN_LEFT )) {
        SetValue( SlideValue( event.GetX() ) );
        Refresh();
    }

    if (event.Dragging()) {
        if (m_bStartDragging==false) {
            m_bStartDragging = true;
            wxWindow::CaptureMouse();
        }
        SetValue( SlideValue( event.GetX() ) );
        Refresh();
    }

    if (!event.Dragging()) {
        if (m_bStartDragging==true) {
            m_bStartDragging = false;
            wxWindow::ReleaseMouse();
        }
    }

    event.Skip();
}

void moWxLevelCtrl::OnCaptureLostEvent( wxMouseCaptureLostEvent& event ) {

    m_bStartDragging = false;

}

void moWxLevelCtrl::OnCaptureChangedEvent( wxMouseCaptureChangedEvent& event ) {

    m_bStartDragging = false;

}

