#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxMoColourPanel.h"

#include <wx/file.h>
#include <wx/dcbuffer.h>



BEGIN_EVENT_TABLE( wxMoColourPanel, wxPanel )
    EVT_PAINT( wxMoColourPanel::OnPaint )
    EVT_SIZE( wxMoColourPanel::OnSize )
END_EVENT_TABLE()

wxMoColourPanel::wxMoColourPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name  ) :
wxPanel( parent, id,  pos, size, style | wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, name ), wxMoColour( (wxColour::ChannelType) 0, (wxColour::ChannelType)0, (wxColour::ChannelType)0, (wxColour::ChannelType)255 ) {

  SetBackgroundStyle(wxBG_STYLE_CUSTOM );

  if (!m_Pattern.Create( size.GetWidth(), size.GetHeight(), 24 )) {
        wxMessageBox(wxString("wxMoColourPanel error creating pattern bitmap."));
  }

}

wxMoColourPanel::~wxMoColourPanel() {


}

void
wxMoColourPanel::SetColourValue( const wxColour &p_Colour ) {

  UpdateColour( p_Colour );

  Refresh();
}

void
wxMoColourPanel::SetBitmap( const wxBitmap& bitmap ) {

  if (bitmap.IsOk()) {

      m_BitmapOriginal = bitmap;

      ResizeBitmap();

  }

  Refresh();

}

void
wxMoColourPanel::ResizeBitmap() {

    wxImage resizer;
    wxBitmap& bitmap(m_BitmapOriginal);

    if (bitmap.IsOk()) {
      ///if bitmap size is different from Panel,resize it to fit into the control...with good quality
      if (  bitmap.GetWidth()>0 &&
            bitmap.GetHeight()> 0 &&
            (
              bitmap.GetWidth()!=GetSize().GetWidth() ||
              bitmap.GetHeight()!=GetSize().GetHeight())
            )
      {

        resizer = bitmap.ConvertToImage();


        float bitmap_prop;
        float control_prop;
        int sizew;
        int sizeh;

        control_prop = (float)GetSize().GetWidth() / (float)GetSize().GetHeight();
        bitmap_prop = (float)bitmap.GetWidth() / (float)bitmap.GetHeight();

        if ( control_prop > bitmap_prop ) {
            sizeh = GetSize().GetHeight();
            sizew = ( bitmap.GetWidth() * sizeh ) / bitmap.GetHeight();
        } else {
            sizew = GetSize().GetWidth();
            sizeh = ( bitmap.GetHeight() * sizew ) / bitmap.GetWidth();
        }

        resizer.Rescale( sizew, sizeh, wxIMAGE_QUALITY_HIGH );

        ///wxMessageBox(wxString("resized"));

        int w,h,b;

        if (resizer.IsOk()) {

            w = resizer.GetWidth();
            h = resizer.GetHeight();

            m_Bitmap = wxBitmap( resizer, 32 );
            if ( m_Bitmap.IsOk() ) {
                w = m_Bitmap.GetWidth();
                h = m_Bitmap.GetHeight();
            } else {
                m_Bitmap = wxBitmap( resizer );
                if ( m_Bitmap.IsOk() ) {
                    w = m_Bitmap.GetWidth();
                    h = m_Bitmap.GetHeight();
                }

            }

        }


      } else {

        ///just assign
        m_Bitmap = bitmap;

      }
    }

}


void wxMoColourPanel::UpdatePattern() {

    ///Get control size
    wxSize sz = GetSize();
    wxMemoryDC mdc;
    int i,j;
    int pat = 8;
    wxImage* pPatternImage;

    ///intialize memory dc to the bitmap
    mdc.SelectObject( m_Pattern );
    mdc.SetPen( wxPen( wxColour(255,255,255), 1, wxTRANSPARENT ) );

    pat = std::min( m_Pattern.GetHeight() / 4, m_Pattern.GetWidth() / 4);
    if (pat==0) pat = 4;

    for( i=0; i<sz.GetWidth(); i+=pat) {
      for( j=0; j<sz.GetHeight(); j+=pat) {
        if ( ( ( i/pat + j/pat ) % 2 ) == 0 ) {
          mdc.SetBrush( wxBrush( wxColour(  0,
                                            0,
                                            0), wxSOLID) );
        }
        else {
          mdc.SetBrush( wxBrush( wxColour(  255,
                                           255,
                                            255), wxSOLID) );
        }
        mdc.DrawRectangle( i, j, pat, pat );
      }
    }


    if ( m_Bitmap.IsOk() && m_Bitmap.GetHeight()>0 && m_Bitmap.GetWidth()>0 )
    {
      pPatternImage = new wxImage( m_Bitmap.ConvertToImage() );

      ///We draw the bitmap as a transparent coloured and filtered by the actual colour value wxImage

      if ( !pPatternImage ) {
        wxMessageBox(wxString("wxMoColourPanel error creating m_ImagePattern."));
      } else {

              ///wxMessageBox("draw bitmap");

              pPatternImage->InitAlpha();
              for( i=0; i<pPatternImage->GetWidth(); i+=1) {
                for( j=0; j<pPatternImage->GetHeight(); j+=1) {
                  ///rgb colour bitmap colour multiply actual colour value
                  wxColour RGBColour(
                      ( pPatternImage->GetRed(i,j)   * m_ColourValue.Red() )    / 255.0,
                      ( pPatternImage->GetGreen(i,j) * m_ColourValue.Green() )  / 255.0,
                      ( pPatternImage->GetBlue(i,j)  * m_ColourValue.Blue() )    / 255.0
                       );

                  pPatternImage->SetRGB( i, j, RGBColour.Red(), RGBColour.Green(), RGBColour.Blue() );
                  pPatternImage->SetAlpha( i, j, ( pPatternImage->GetAlpha(i,j)  * m_ColourValue.Alpha() )    / 255.0);

                }

              }

              int leftx = GetSize().GetWidth() / 2 - m_Bitmap.GetWidth() / 2;
              int topx = GetSize().GetHeight() / 2 - m_Bitmap.GetHeight() / 2;

              mdc.DrawBitmap( wxBitmap(*pPatternImage), leftx, topx, true );

              delete pPatternImage;
      }

    } else {

      pPatternImage = new wxImage( GetSize().GetWidth(), GetSize().GetHeight(), true );

      ///We draw the colour value as a transparent image over the pattern
      ///wxMessageBox("draw color");
      if (pPatternImage) {

              pPatternImage->InitAlpha();
              for( i=0; i<pPatternImage->GetWidth(); i+=1) {
                for( j=0; j<pPatternImage->GetHeight(); j+=1) {
                  pPatternImage->SetRGB( i, j, m_ColourValue.Red(), m_ColourValue.Green(), m_ColourValue.Blue() );
                  pPatternImage->SetAlpha( i, j, m_ColourValue.Alpha() );
                }
              }

              mdc.DrawBitmap( wxBitmap( *pPatternImage ), 0, 0, true );

      }

    }

}


void
wxMoColourPanel::OnSize( wxSizeEvent& event ) {

    //wxString s;
    //s.Printf( "w: %i", GetSize().GetWidth() );

    //wxMessageBox( s );

    if (  m_Pattern.GetWidth()!=event.GetSize().GetWidth() ||
          m_Pattern.GetHeight()!=event.GetSize().GetHeight() ) {

      #ifdef __WXMSW__
      m_Pattern.FreeResource();
      #endif

      if (!m_Pattern.Create( event.GetSize().GetWidth(), event.GetSize().GetHeight(), 24 )) {
        wxMessageBox(wxString("wxMoColourPanel error creating pattern bitmap."));
      }

      //SetSize( event );

      ResizeBitmap();
    }

    wxPanel::OnSize( event );

}


void
wxMoColourPanel::OnPaint(wxPaintEvent& event) {

    wxBufferedPaintDC dc(this);

    if (m_Pattern.IsOk()) {
      UpdatePattern();
      dc.DrawBitmap( m_Pattern,  0, 0, true );
    }

}


DEFINE_EVENT_TYPE(wxEVT_MOCOLOURPANEL)
