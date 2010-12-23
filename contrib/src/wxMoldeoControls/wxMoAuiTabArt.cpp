#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wxMoAuiTabArt.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/image.h"
    #include "wx/menu.h"
#endif

#include "wx/aui/tabmdi.h"
#include "wx/dcbuffer.h"
#include "wx/log.h"


#ifdef __WXMSW__
#include  "wx/msw/private.h"
#endif

#ifdef __WXMAC__
#include "wx/mac/carbon/private.h"
#endif

#ifdef __WXGTK__
#include <gtk/gtk.h>
#include "wx/gtk/win_gtk.h"
#endif


// these functions live in dockart.cpp -- they'll eventually
// be moved to a new utility cpp file

wxColor wxAuiStepColour(const wxColor& c, int percent);

wxBitmap wxAuiBitmapFromBits(const unsigned char bits[], int w, int h,
                             const wxColour& color);

wxString wxAuiChopText(wxDC& dc, const wxString& text, int max_size);

static void DrawButtons(wxDC& dc,
                        const wxRect& _rect,
                        const wxBitmap& bmp,
                        const wxColour& bkcolour,
                        int button_state)
{
    wxRect rect = _rect;

    if (button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        rect.x++;
        rect.y++;
    }

    if (button_state == wxAUI_BUTTON_STATE_HOVER ||
        button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        dc.SetBrush(wxBrush(wxAuiStepColour(bkcolour, 120)));
        dc.SetPen(wxPen(wxAuiStepColour(bkcolour, 75)));

        // draw the background behind the button
        dc.DrawRectangle(rect.x, rect.y, 15, 15);
    }

    // draw the button itself
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
}

static void IndentPressedBitmap(wxRect* rect, int button_state)
{
    if (button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        rect->x++;
        rect->y++;
    }
}

static void DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
#ifdef __WXMSW__
    wxUnusedVar(win);
    wxUnusedVar(flags);

    RECT rc;
    wxCopyRectToRECT(rect, rc);

    ::DrawFocusRect(GetHdcOf(dc), &rc);

#elif defined(__WXGTK20__)
    GdkWindow* gdk_window = dc.GetGDKWindow();
    wxASSERT_MSG( gdk_window,
                  wxT("cannot draw focus rectangle on wxDC of this type") );

    GtkStateType state;
    //if (flags & wxCONTROL_SELECTED)
    //    state = GTK_STATE_SELECTED;
    //else
        state = GTK_STATE_NORMAL;

    gtk_paint_focus( win->m_widget->style,
                     gdk_window,
                     state,
                     NULL,
                     win->m_wxwindow,
                     NULL,
                     dc.LogicalToDeviceX(rect.x),
                     dc.LogicalToDeviceY(rect.y),
                     rect.width,
                     rect.height );
#elif (defined(__WXMAC__))

#if wxMAC_USE_CORE_GRAPHICS
    {
        CGRect cgrect = CGRectMake( rect.x , rect.y , rect.width, rect.height ) ;

#if 0
        Rect bounds ;
        win->GetPeer()->GetRect( &bounds ) ;

        wxLogDebug(wxT("Focus rect %d, %d, %d, %d"), rect.x, rect.y, rect.width, rect.height);
        wxLogDebug(wxT("Peer rect %d, %d, %d, %d"), bounds.left, bounds.top, bounds.right - bounds.left, bounds.bottom - bounds.top);
#endif

        HIThemeFrameDrawInfo info ;
        memset( &info, 0 , sizeof(info) ) ;

        info.version = 0 ;
        info.kind = 0 ;
        info.state = kThemeStateActive;
        info.isFocused = true ;

        CGContextRef cgContext = (CGContextRef) win->MacGetCGContextRef() ;
        wxASSERT( cgContext ) ;

        HIThemeDrawFocusRect( &cgrect , true , cgContext , kHIThemeOrientationNormal ) ;
    }
 #else
    {
        Rect r;
        r.left = rect.x; r.top = rect.y; r.right = rect.GetRight(); r.bottom = rect.GetBottom();
        wxTopLevelWindowMac* top = win->MacGetTopLevelWindow();
        if ( top )
        {
            wxPoint pt(0, 0) ;
            wxMacControl::Convert( &pt , win->GetPeer() , top->GetPeer() ) ;
            OffsetRect( &r , pt.x , pt.y ) ;
        }

        DrawThemeFocusRect( &r , true ) ;
    }
#endif
#else
    wxUnusedVar(win);
    wxUnusedVar(flags);

    // draw the pixels manually because the "dots" in wxPen with wxDOT style
    // may be short traits and not really dots
    //
    // note that to behave in the same manner as DrawRect(), we must exclude
    // the bottom and right borders from the rectangle
    wxCoord x1 = rect.GetLeft(),
            y1 = rect.GetTop(),
            x2 = rect.GetRight(),
            y2 = rect.GetBottom();

    dc.SetPen(*wxBLACK_PEN);

#ifdef __WXMAC__
    dc.SetLogicalFunction(wxCOPY);
#else
    // this seems to be closer than what Windows does than wxINVERT although
    // I'm still not sure if it's correct
    dc.SetLogicalFunction(wxAND_REVERSE);
#endif

    wxCoord z;
    for ( z = x1 + 1; z < x2; z += 2 )
        dc.DrawPoint(z, rect.GetTop());

    wxCoord shift = z == x2 ? 0 : 1;
    for ( z = y1 + shift; z < y2; z += 2 )
        dc.DrawPoint(x2, z);

    shift = z == y2 ? 0 : 1;
    for ( z = x2 - shift; z > x1; z -= 2 )
        dc.DrawPoint(z, y2);

    shift = z == x1 ? 0 : 1;
    for ( z = y2 - shift; z > y1; z -= 2 )
        dc.DrawPoint(x1, z);

    dc.SetLogicalFunction(wxCOPY);
#endif
}

// -- bitmaps --

#if defined( __WXMAC__ )
 static unsigned char close_bits[]={
     0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFE, 0x03, 0xF8, 0x01, 0xF0, 0x19, 0xF3,
     0xB8, 0xE3, 0xF0, 0xE1, 0xE0, 0xE0, 0xF0, 0xE1, 0xB8, 0xE3, 0x19, 0xF3,
     0x01, 0xF0, 0x03, 0xF8, 0x0F, 0xFE, 0xFF, 0xFF };
#elif defined( __WXGTK__)
 static unsigned char close_bits[]={
     0xff, 0xff, 0xff, 0xff, 0x07, 0xf0, 0xfb, 0xef, 0xdb, 0xed, 0x8b, 0xe8,
     0x1b, 0xec, 0x3b, 0xee, 0x1b, 0xec, 0x8b, 0xe8, 0xdb, 0xed, 0xfb, 0xef,
     0x07, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
#else
 static unsigned char close_bits[]={
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xf3, 0xcf, 0xf9,
     0x9f, 0xfc, 0x3f, 0xfe, 0x3f, 0xfe, 0x9f, 0xfc, 0xcf, 0xf9, 0xe7, 0xf3,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

static unsigned char left_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfe, 0x3f, 0xfe,
   0x1f, 0xfe, 0x0f, 0xfe, 0x1f, 0xfe, 0x3f, 0xfe, 0x7f, 0xfe, 0xff, 0xfe,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static unsigned char right_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0x9f, 0xff, 0x1f, 0xff,
   0x1f, 0xfe, 0x1f, 0xfc, 0x1f, 0xfe, 0x1f, 0xff, 0x9f, 0xff, 0xdf, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static unsigned char list_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

// -- GUI helper classes and functions --

class wxAuiCommandCapture : public wxEvtHandler
{
public:

    wxAuiCommandCapture() { m_last_id = 0; }
    int GetCommandId() const { return m_last_id; }

    bool ProcessEvent(wxEvent& evt)
    {
        if (evt.GetEventType() == wxEVT_COMMAND_MENU_SELECTED)
        {
            m_last_id = evt.GetId();
            return true;
        }

        if (GetNextHandler())
            return GetNextHandler()->ProcessEvent(evt);

        return false;
    }

private:
    int m_last_id;
};

// -- wxMoAuiTabArt class implementation --

wxMoAuiTabArt::wxMoAuiTabArt()
{
    m_normal_font = *wxNORMAL_FONT;
    m_selected_font = *wxNORMAL_FONT;
    m_selected_font.SetWeight(wxBOLD);
    m_measuring_font = m_selected_font;

    m_fixed_tab_width = 100;
    m_tab_ctrl_height = 0;

#ifdef __WXMAC__
    wxBrush toolbarbrush;
    toolbarbrush.MacSetTheme( kThemeBrushToolbarBackground );
    wxColor base_colour = toolbarbrush.GetColour();
#else
    wxColor base_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#endif

    // the base_colour is too pale to use as our base colour,
    // so darken it a bit --
    if ((255-base_colour.Red()) +
        (255-base_colour.Green()) +
        (255-base_colour.Blue()) < 60)
    {
        base_colour = wxAuiStepColour(base_colour, 92);
    }

    m_base_colour = base_colour;
    wxColor border_colour = wxAuiStepColour(base_colour, 75);

    m_border_pen = wxPen(border_colour);
    m_base_colour_pen = wxPen(m_base_colour);
    m_base_colour_brush = wxBrush(m_base_colour);

    m_active_close_bmp = wxAuiBitmapFromBits(close_bits, 16, 16, *wxBLACK);
    m_disabled_close_bmp = wxAuiBitmapFromBits(close_bits, 16, 16, wxColour(128,128,128));

    m_active_left_bmp = wxAuiBitmapFromBits(left_bits, 16, 16, *wxBLACK);
    m_disabled_left_bmp = wxAuiBitmapFromBits(left_bits, 16, 16, wxColour(128,128,128));

    m_active_right_bmp = wxAuiBitmapFromBits(right_bits, 16, 16, *wxBLACK);
    m_disabled_right_bmp = wxAuiBitmapFromBits(right_bits, 16, 16, wxColour(128,128,128));

    m_active_windowlist_bmp = wxAuiBitmapFromBits(list_bits, 16, 16, *wxBLACK);
    m_disabled_windowlist_bmp = wxAuiBitmapFromBits(list_bits, 16, 16, wxColour(128,128,128));

    m_flags = 0;
}

wxMoAuiTabArt::~wxMoAuiTabArt()
{
}

wxAuiTabArt* wxMoAuiTabArt::Clone()
{
    wxMoAuiTabArt* art = new wxMoAuiTabArt;
    art->SetNormalFont(m_normal_font);
    art->SetSelectedFont(m_selected_font);
    art->SetMeasuringFont(m_measuring_font);

    return art;
}

void wxMoAuiTabArt::SetFlags(unsigned int flags)
{
    m_flags = flags;
}

void wxMoAuiTabArt::SetSizingInfo(const wxSize& tab_ctrl_size,
                                       size_t tab_count)
{
    m_fixed_tab_width = 100;

    int tot_width = (int)tab_ctrl_size.x - GetIndentSize() - 4;

    if (m_flags & wxAUI_NB_CLOSE_BUTTON)
        tot_width -= m_active_close_bmp.GetWidth();
    if (m_flags & wxAUI_NB_WINDOWLIST_BUTTON)
        tot_width -= m_active_windowlist_bmp.GetWidth();

    if (tab_count > 0)
    {
        m_fixed_tab_width = tot_width/(int)tab_count;
    }


    if (m_fixed_tab_width < 100)
        m_fixed_tab_width = 100;

    if (m_fixed_tab_width > tot_width/2)
        m_fixed_tab_width = tot_width/2;

    if (m_fixed_tab_width > 220)
        m_fixed_tab_width = 220;

    m_tab_ctrl_height = tab_ctrl_size.y;
}


void wxMoAuiTabArt::DrawBackground(wxDC& dc,
                                        wxWindow* WXUNUSED(wnd),
                                        const wxRect& rect)
{
    // draw background
   wxColor top_color       = wxAuiStepColour(m_base_colour, 70);
   wxColor bottom_color   = wxAuiStepColour(m_base_colour, 120);
   wxRect r;

   top_color = wxColor( 0, 0, 0 );
   bottom_color = wxColor( 0, 0, 0 );

   if (m_flags &wxAUI_NB_BOTTOM)
       r = wxRect(rect.x, rect.y, rect.width+2, rect.height);
   // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
   // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
   else //for wxAUI_NB_TOP
       r = wxRect(rect.x, rect.y, rect.width+2, rect.height-3);

    //dc.GradientFillLinear(r, top_color, bottom_color, wxSOUTH);
    dc.GradientFillLinear(r, top_color, bottom_color, wxSOUTH);

   // draw base lines

   dc.SetPen(m_border_pen);
   int y = rect.GetHeight();
   int w = rect.GetWidth();

   if (m_flags &wxAUI_NB_BOTTOM)
   {
       dc.SetBrush(wxBrush(bottom_color));
       dc.DrawRectangle(-1, 0, w+2, 4);
   }
   // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
   // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
   else //for wxAUI_NB_TOP
   {
       dc.SetBrush(bottom_color);
       dc.DrawRectangle(-1, y-4, w+2, 4);
   }

}


// DrawTab() draws an individual tab.
//
// dc       - output dc
// in_rect  - rectangle the tab should be confined to
// caption  - tab's caption
// active   - whether or not the tab is active
// out_rect - actual output rectangle
// x_extent - the advance x; where the next tab should start

void wxMoAuiTabArt::DrawTab(wxDC& dc,
                                 wxWindow* wnd,
                                 const wxAuiNotebookPage& page,
                                 const wxRect& in_rect,
                                 int close_button_state,
                                 wxRect* out_tab_rect,
                                 wxRect* out_button_rect,
                                 int* x_extent)
{
    wxCoord normal_textx, normal_texty;
    wxCoord selected_textx, selected_texty;
    wxCoord texty;

    // if the caption is empty, measure some temporary text
    wxString caption = page.caption;
    if (caption.empty())
        caption = wxT("Xj");

    dc.SetFont(m_selected_font);
    dc.GetTextExtent(caption, &selected_textx, &selected_texty);

    dc.SetFont(m_normal_font);
    dc.GetTextExtent(caption, &normal_textx, &normal_texty);

    // figure out the size of the tab
    wxSize tab_size = GetTabSize(dc,
                                 wnd,
                                 page.caption,
                                 page.bitmap,
                                 page.active,
                                 close_button_state,
                                 x_extent);

    wxCoord tab_height = m_tab_ctrl_height - 3;
    wxCoord tab_width = tab_size.x;
    wxCoord tab_x = in_rect.x;
    wxCoord tab_y = in_rect.y + in_rect.height - tab_height;


    caption = page.caption;


    // select pen, brush and font for the tab to be drawn

    if (page.active)
    {
        dc.SetFont(m_selected_font);
        texty = selected_texty;
    }
     else
    {
        dc.SetFont(m_normal_font);
        texty = normal_texty;
    }


    // create points that will make the tab outline

    int clip_width = tab_width;
    if (tab_x + clip_width > in_rect.x + in_rect.width)
        clip_width = (in_rect.x + in_rect.width) - tab_x;

/*
    wxPoint clip_points[6];
    clip_points[0] = wxPoint(tab_x,              tab_y+tab_height-3);
    clip_points[1] = wxPoint(tab_x,              tab_y+2);
    clip_points[2] = wxPoint(tab_x+2,            tab_y);
    clip_points[3] = wxPoint(tab_x+clip_width-1, tab_y);
    clip_points[4] = wxPoint(tab_x+clip_width+1, tab_y+2);
    clip_points[5] = wxPoint(tab_x+clip_width+1, tab_y+tab_height-3);

    // FIXME: these ports don't provide wxRegion ctor from array of points
#if !defined(__WXDFB__) && !defined(__WXCOCOA__)
    // set the clipping region for the tab --
    wxRegion clipping_region(WXSIZEOF(clip_points), clip_points);
    dc.SetClippingRegion(clipping_region);
#endif // !wxDFB && !wxCocoa
*/
    // since the above code above doesn't play well with WXDFB or WXCOCOA,
    // we'll just use a rectangle for the clipping region for now --
    dc.SetClippingRegion(tab_x, tab_y, clip_width+1, tab_height-3);


    wxPoint border_points[6];
    if (m_flags &wxAUI_NB_BOTTOM)
    {
       border_points[0] = wxPoint(tab_x,             tab_y);
       border_points[1] = wxPoint(tab_x,             tab_y+tab_height-6);
       border_points[2] = wxPoint(tab_x+2,           tab_y+tab_height-4);
       border_points[3] = wxPoint(tab_x+tab_width-2, tab_y+tab_height-4);
       border_points[4] = wxPoint(tab_x+tab_width,   tab_y+tab_height-6);
       border_points[5] = wxPoint(tab_x+tab_width,   tab_y);
    }
    else //if (m_flags & wxAUI_NB_TOP) {}
    {
       border_points[0] = wxPoint(tab_x,             tab_y+tab_height-4);
       border_points[1] = wxPoint(tab_x,             tab_y+2);
       border_points[2] = wxPoint(tab_x+2,           tab_y);
       border_points[3] = wxPoint(tab_x+tab_width-2, tab_y);
       border_points[4] = wxPoint(tab_x+tab_width,   tab_y+2);
       border_points[5] = wxPoint(tab_x+tab_width,   tab_y+tab_height-4);
    }
    // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
    // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}

    int drawn_tab_yoff = border_points[1].y;
    int drawn_tab_height = border_points[0].y - border_points[1].y;


    if (page.active)
    {
        // draw active tab

        // draw base background color
        wxRect r(tab_x, tab_y, tab_width, tab_height);
        dc.SetPen(m_base_colour_pen);
        dc.SetBrush(m_base_colour_brush);
        dc.DrawRectangle(r.x+1, r.y+1, r.width-1, r.height-4);

        // this white helps fill out the gradient at the top of the tab
        dc.SetPen(*wxWHITE_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRectangle(r.x+2, r.y+1, r.width-3, r.height-4);

        // these two points help the rounded corners appear more antialiased
        dc.SetPen(m_base_colour_pen);
        dc.DrawPoint(r.x+2, r.y+1);
        dc.DrawPoint(r.x+r.width-2, r.y+1);

        // set rectangle down a bit for gradient drawing
        r.SetHeight(r.GetHeight()/2);
        r.x += 2;
        r.width -= 2;
        r.y += r.height;
        r.y -= 2;

        // draw gradient background
        wxColor top_color = *wxWHITE;
        wxColor bottom_color = m_base_colour;

        top_color = wxColor(255,255,255);
        bottom_color = wxColor(255,255,255);

        dc.GradientFillLinear(r, bottom_color, top_color, wxNORTH);
    }
     else
    {
        // draw inactive tab

        wxRect r(tab_x, tab_y+1, tab_width, tab_height-3);

        // start the gradent up a bit and leave the inside border inset
        // by a pixel for a 3D look.  Only the top half of the inactive
        // tab will have a slight gradient
        r.x += 3;
        r.y++;
        r.width -= 4;
        r.height /= 2;
        r.height--;

        // -- draw top gradient fill for glossy look
        wxColor top_color = m_base_colour;
        wxColor bottom_color = wxAuiStepColour(top_color, 160);

        top_color = wxColor(148,148,148);
        bottom_color = top_color;

        dc.GradientFillLinear(r, bottom_color, top_color, wxNORTH);

        r.y += r.height;
        r.y--;

        // -- draw bottom fill for glossy look
        top_color = m_base_colour;
        bottom_color = m_base_colour;

        top_color = wxColor(148,148,148);
        bottom_color = top_color;

        dc.GradientFillLinear(r, top_color, bottom_color, wxSOUTH);
    }

    // draw tab outline
    dc.SetPen(m_border_pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawPolygon(WXSIZEOF(border_points), border_points);

    // there are two horizontal grey lines at the bottom of the tab control,
    // this gets rid of the top one of those lines in the tab control
    if (page.active)
    {
       if (m_flags &wxAUI_NB_BOTTOM)
           dc.SetPen(wxPen(wxColour(wxAuiStepColour(m_base_colour, 170))));
       // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
       // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
       else //for wxAUI_NB_TOP
           dc.SetPen(m_base_colour_pen);
        dc.DrawLine(border_points[0].x+1,
                    border_points[0].y,
                    border_points[5].x,
                    border_points[5].y);
    }


    int text_offset = tab_x + 8;
    int close_button_width = 0;
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
        close_button_width = m_active_close_bmp.GetWidth();
    }


    int bitmap_offset = 0;
    if (page.bitmap.IsOk())
    {
        bitmap_offset = tab_x + 8;

        // draw bitmap
        dc.DrawBitmap(page.bitmap,
                      bitmap_offset,
                      drawn_tab_yoff + (drawn_tab_height/2) - (page.bitmap.GetHeight()/2),
                      true);

        text_offset = bitmap_offset + page.bitmap.GetWidth();
        text_offset += 3; // bitmap padding
    }
     else
    {
        text_offset = tab_x + 8;
    }


    wxString draw_text = wxAuiChopText(dc,
                          caption,
                          tab_width - (text_offset-tab_x) - close_button_width);

    // draw tab text
    dc.DrawText(draw_text,
                text_offset,
                drawn_tab_yoff + (drawn_tab_height)/2 - (texty/2) - 1);

    // draw close button if necessary
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
        wxBitmap bmp = m_disabled_close_bmp;

        if (close_button_state == wxAUI_BUTTON_STATE_HOVER ||
            close_button_state == wxAUI_BUTTON_STATE_PRESSED)
        {
            bmp = m_active_close_bmp;
        }

        wxRect rect(tab_x + tab_width - close_button_width - 1,
                    tab_y + (tab_height/2) - (bmp.GetHeight()/2),
                    close_button_width,
                    tab_height);
        IndentPressedBitmap(&rect, close_button_state);
        dc.DrawBitmap(bmp, rect.x, rect.y, true);

        *out_button_rect = rect;
    }

    *out_tab_rect = wxRect(tab_x, tab_y, tab_width, tab_height);

#ifndef __WXMAC__
    // draw focus rectangle
    if (page.active && (wnd->FindFocus() == wnd))
    {
        wxRect focusRectText(text_offset, (drawn_tab_yoff + (drawn_tab_height)/2 - (texty/2) - 1),
            selected_textx, selected_texty);

        wxRect focusRect;
        wxRect focusRectBitmap;

        if (page.bitmap.IsOk())
            focusRectBitmap = wxRect(bitmap_offset, drawn_tab_yoff + (drawn_tab_height/2) - (page.bitmap.GetHeight()/2),
                                            page.bitmap.GetWidth(), page.bitmap.GetHeight());

        if (page.bitmap.IsOk() && draw_text.IsEmpty())
            focusRect = focusRectBitmap;
        else if (!page.bitmap.IsOk() && !draw_text.IsEmpty())
            focusRect = focusRectText;
        else if (page.bitmap.IsOk() && !draw_text.IsEmpty())
            focusRect = focusRectText.Union(focusRectBitmap);

        focusRect.Inflate(2, 2);

        DrawFocusRect(wnd, dc, focusRect, 0);
    }
#endif

    dc.DestroyClippingRegion();
}

int wxMoAuiTabArt::GetIndentSize()
{
    return 5;
}

wxSize wxMoAuiTabArt::GetTabSize(wxDC& dc,
                                      wxWindow* WXUNUSED(wnd),
                                      const wxString& caption,
                                      const wxBitmap& bitmap,
                                      bool WXUNUSED(active),
                                      int close_button_state,
                                      int* x_extent)
{
    wxCoord measured_textx, measured_texty, tmp;

    dc.SetFont(m_measuring_font);
    dc.GetTextExtent(caption, &measured_textx, &measured_texty);

    dc.GetTextExtent(wxT("ABCDEFXj"), &tmp, &measured_texty);

    // add padding around the text
    wxCoord tab_width = measured_textx;
    wxCoord tab_height = measured_texty;

    // if the close button is showing, add space for it
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
        tab_width += m_active_close_bmp.GetWidth() + 3;

    // if there's a bitmap, add space for it
    if (bitmap.IsOk())
    {
        tab_width += bitmap.GetWidth();
        tab_width += 3; // right side bitmap padding
        tab_height = wxMax(tab_height, bitmap.GetHeight());
    }

    // add padding
    tab_width += 16;
    tab_height += 10;

    if (m_flags & wxAUI_NB_TAB_FIXED_WIDTH)
    {
        tab_width = m_fixed_tab_width;
    }

    *x_extent = tab_width;

    return wxSize(tab_width, tab_height);
}


void wxMoAuiTabArt::DrawButton(wxDC& dc,
                                    wxWindow* WXUNUSED(wnd),
                                    const wxRect& in_rect,
                                    int bitmap_id,
                                    int button_state,
                                    int orientation,
                                    wxRect* out_rect)
{
    wxBitmap bmp;
    wxRect rect;

    switch (bitmap_id)
    {
        case wxAUI_BUTTON_CLOSE:
            if (button_state & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabled_close_bmp;
                 else
                bmp = m_active_close_bmp;
            break;
        case wxAUI_BUTTON_LEFT:
            if (button_state & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabled_left_bmp;
                 else
                bmp = m_active_left_bmp;
            break;
        case wxAUI_BUTTON_RIGHT:
            if (button_state & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabled_right_bmp;
                 else
                bmp = m_active_right_bmp;
            break;
        case wxAUI_BUTTON_WINDOWLIST:
            if (button_state & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabled_windowlist_bmp;
                 else
                bmp = m_active_windowlist_bmp;
            break;
    }


    if (!bmp.IsOk())
        return;

    rect = in_rect;

    if (orientation == wxLEFT)
    {
        rect.SetX(in_rect.x);
        rect.SetY(((in_rect.y + in_rect.height)/2) - (bmp.GetHeight()/2));
        rect.SetWidth(bmp.GetWidth());
        rect.SetHeight(bmp.GetHeight());
    }
     else
    {
        rect = wxRect(in_rect.x + in_rect.width - bmp.GetWidth(),
                      ((in_rect.y + in_rect.height)/2) - (bmp.GetHeight()/2),
                      bmp.GetWidth(), bmp.GetHeight());
    }

    IndentPressedBitmap(&rect, button_state);
    dc.DrawBitmap(bmp, rect.x, rect.y, true);

    *out_rect = rect;
}


int wxMoAuiTabArt::ShowDropDown(wxWindow* wnd,
                                     const wxAuiNotebookPageArray& pages,
                                     int active_idx)
{
    wxMenu menuPopup;

    size_t i, count = pages.GetCount();
    for (i = 0; i < count; ++i)
    {
        const wxAuiNotebookPage& page = pages.Item(i);
        wxString caption = page.caption;

        // if there is no caption, make it a space.  This will prevent
        // an assert in the menu code.
        if (caption.IsEmpty())
            caption = wxT(" ");

        menuPopup.AppendCheckItem(1000+i, caption);
    }

    if (active_idx != -1)
    {
        menuPopup.Check(1000+active_idx, true);
    }

    // find out where to put the popup menu of window items
    wxPoint pt = ::wxGetMousePosition();
    pt = wnd->ScreenToClient(pt);

    // find out the screen coordinate at the bottom of the tab ctrl
    wxRect cli_rect = wnd->GetClientRect();
    pt.y = cli_rect.y + cli_rect.height;

    wxAuiCommandCapture* cc = new wxAuiCommandCapture;
    wnd->PushEventHandler(cc);
    wnd->PopupMenu(&menuPopup, pt);
    int command = cc->GetCommandId();
    wnd->PopEventHandler(true);

    if (command >= 1000)
        return command-1000;

    return -1;
}

int wxMoAuiTabArt::GetBestTabCtrlSize(wxWindow* wnd,
                                           const wxAuiNotebookPageArray& pages,
                                           const wxSize& required_bmp_size)
{
    wxClientDC dc(wnd);
    dc.SetFont(m_measuring_font);

    // sometimes a standard bitmap size needs to be enforced, especially
    // if some tabs have bitmaps and others don't.  This is important because
    // it prevents the tab control from resizing when tabs are added.
    wxBitmap measure_bmp;
    if (required_bmp_size.IsFullySpecified())
    {
        measure_bmp.Create(required_bmp_size.x,
                           required_bmp_size.y);
    }


    int max_y = 0;
    size_t i, page_count = pages.GetCount();
    for (i = 0; i < page_count; ++i)
    {
        wxAuiNotebookPage& page = pages.Item(i);

        wxBitmap bmp;
        if (measure_bmp.IsOk())
            bmp = measure_bmp;
             else
            bmp = page.bitmap;

        // we don't use the caption text because we don't
        // want tab heights to be different in the case
        // of a very short piece of text on one tab and a very
        // tall piece of text on another tab
        int x_ext = 0;
        wxSize s = GetTabSize(dc,
                              wnd,
                              wxT("ABCDEFGHIj"),
                              bmp,
                              true,
                              wxAUI_BUTTON_STATE_HIDDEN,
                              &x_ext);

        max_y = wxMax(max_y, s.y);
    }

    return max_y+2;
}

void wxMoAuiTabArt::SetNormalFont(const wxFont& font)
{
    m_normal_font = font;
}

void wxMoAuiTabArt::SetSelectedFont(const wxFont& font)
{
    m_selected_font = font;
}

void wxMoAuiTabArt::SetMeasuringFont(const wxFont& font)
{
    m_measuring_font = font;
}
