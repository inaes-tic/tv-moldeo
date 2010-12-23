#include "wxMoldeoControlsTestDialog.h"

//(*InternalHeaders(wxMoldeoControlsTestDialog)
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/msgdlg.h>

//(*IdInit(wxMoldeoControlsTestDialog)
const long wxMoldeoControlsTestDialog::ID_STATICTEXT1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_BUTTONCLOSE = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER4 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER5 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER3 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER2 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_VALUECOLORLEVELCTRL = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SATURATIONCOLORLEVELCTRL = wxNewId();
const long wxMoldeoControlsTestDialog::ID_HUECOLORLEVELCTRL = wxNewId();
const long wxMoldeoControlsTestDialog::ID_ALPHACOLORLEVEL = wxNewId();
const long wxMoldeoControlsTestDialog::ID_BLUECOLORLEVELCTRL = wxNewId();
const long wxMoldeoControlsTestDialog::ID_GREENCOLORLEVELCTRL = wxNewId();
const long wxMoldeoControlsTestDialog::ID_REDCOLORLEVELCTRL = wxNewId();
const long wxMoldeoControlsTestDialog::ID_STATICTEXT7 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_STATICTEXTCOLORLEVELCTRL = wxNewId();
const long wxMoldeoControlsTestDialog::ID_STATICTEXT2 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SPINCTRL2 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SPINCTRL1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_BUTTONOK = wxNewId();
const long wxMoldeoControlsTestDialog::ID_STATICTEXT5 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_STATICTEXT6 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_STATICTEXT4 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER6 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER7 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_MOLDEOPIN2 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_EFFECT2 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_EFFECT1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_PIN1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_EFFECT3 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_PANEL1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_PANEL3 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_PANEL2 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_COLOURPANEL = wxNewId();
const long wxMoldeoControlsTestDialog::ID_PANEL4 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_PANEL5 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_PANEL6 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_AUINOTEBOOK = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxMoldeoControlsTestDialog,wxDialog)
	//(*EventTable(wxMoldeoControlsTestDialog)
	//*)
END_EVENT_TABLE()

wxMoldeoControlsTestDialog::wxMoldeoControlsTestDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(wxMoldeoControlsTestDialog)
	Create(parent, wxID_ANY, _("Moldeo Controls Test"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(966,485));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Moldeo Controls"), wxPoint(8,8), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	wxFont StaticText1Font(20,wxSWISS,wxFONTSTYLE_NORMAL,wxBOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText1->SetFont(StaticText1Font);
	ButtonCLose = new wxButton(this, ID_BUTTONCLOSE, _("Close"), wxPoint(440,448), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONCLOSE"));
	ButtonCLose->SetDefault();
	Slider4 = new wxMoRotaryCtrl(this, ID_SLIDER4, 0, 0, 100, wxPoint(48,144), wxSize(32,32), 0, wxDefaultValidator, _T("ID_SLIDER4"));
	Slider5 = new wxMoRotaryCtrl(this, ID_SLIDER5, 0, 0, 100, wxPoint(88,144), wxSize(32,32), 0, wxDefaultValidator, _T("ID_SLIDER5"));
	Slider3 = new wxMoRotaryCtrl(this, ID_SLIDER3, 0, 0, 100, wxPoint(128,136), wxSize(56,56), 0, wxDefaultValidator, _T("ID_SLIDER3"));
	Slider2 = new wxMoRotaryCtrl(this, ID_SLIDER2, 0, 0, 100, wxPoint(8,144), wxSize(32,32), 0, wxDefaultValidator, _T("ID_SLIDER2"));
	Slider1 = new wxMoLevelCtrl(this, ID_SLIDER1, 0, 0, 100, wxPoint(104,72), wxSize(128,16), 0, wxDefaultValidator, _T("ID_SLIDER1"));
	ValueColorLevelCtrl = new wxMoColorLevelCtrl(this, ID_VALUECOLORLEVELCTRL, 0, 0, 100, wxPoint(304,184), wxSize(120,16), 0, wxDefaultValidator, _T("ID_VALUECOLORLEVELCTRL"));
	SaturationColorLevelCtrl = new wxMoColorLevelCtrl(this, ID_SATURATIONCOLORLEVELCTRL, 0, 0, 100, wxPoint(304,160), wxSize(120,16), 0, wxDefaultValidator, _T("ID_SATURATIONCOLORLEVELCTRL"));
	HueColorLevelCtrl = new wxMoColorLevelCtrl(this, ID_HUECOLORLEVELCTRL, 0, 0, 100, wxPoint(304,136), wxSize(120,16), 0, wxDefaultValidator, _T("ID_HUECOLORLEVELCTRL"));
	AlphaColorLevelCtrl = new wxMoColorLevelCtrl(this, ID_ALPHACOLORLEVEL, 0, 0, 100, wxPoint(304,280), wxSize(120,16), 0, wxDefaultValidator, _T("ID_ALPHACOLORLEVEL"));
	BlueColorLevelCtrl = new wxMoColorLevelCtrl(this, ID_BLUECOLORLEVELCTRL, 0, 0, 100, wxPoint(304,256), wxSize(120,16), 0, wxDefaultValidator, _T("ID_BLUECOLORLEVELCTRL"));
	GreenColorLevelCtrl = new wxMoColorLevelCtrl(this, ID_GREENCOLORLEVELCTRL, 0, 0, 100, wxPoint(304,232), wxSize(120,16), 0, wxDefaultValidator, _T("ID_GREENCOLORLEVELCTRL"));
	RedColorLevelCtrl = new wxMoColorLevelCtrl(this, ID_REDCOLORLEVELCTRL, 0, 0, 100, wxPoint(304,208), wxSize(120,16), 0, wxDefaultValidator, _T("ID_REDCOLORLEVELCTRL"));
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("wxMoLevelCtrl"), wxPoint(8,72), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	StaticTextColorLevelCtrl = new wxStaticText(this, ID_STATICTEXTCOLORLEVELCTRL, _("wxMoColorLevelCtrl"), wxPoint(312,104), wxDefaultSize, 0, _T("ID_STATICTEXTCOLORLEVELCTRL"));
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("wxMoRotaryCtrl"), wxPoint(8,112), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	SpinCtrl2 = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxPoint(200,152), wxSize(53,21), 0, 0, 100, 0, _T("ID_SPINCTRL2"));
	SpinCtrl2->SetValue(_T("0"));
	SpinCtrl1 = new wxSpinCtrl(this, ID_SPINCTRL1, _T("0"), wxPoint(240,64), wxSize(53,21), 0, 0, 100, 0, _T("ID_SPINCTRL1"));
	SpinCtrl1->SetValue(_T("0"));
	ButtonOK = new wxButton(this, ID_BUTTONOK, _("OK"), wxPoint(352,448), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONOK"));
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("wxMoTimelineRulerCtrl"), wxPoint(8,208), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("wxMoldeoObjectCanvas"), wxPoint(464,16), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("wxMoTimelineBandCtrl"), wxPoint(8,280), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	Slider6 = new wxMoTimelineRulerCtrl(this, ID_SLIDER6, 0, 0, 100, wxPoint(8,232), wxSize(264,24), 0, wxDefaultValidator, _T("ID_SLIDER6"));
	Slider7 = new wxMoTimelineBandCtrl(this, ID_SLIDER7, 0, 0, 100, wxPoint(8,304), wxSize(264,24), 0, wxDefaultValidator, _T("ID_SLIDER7"));
	ConsoleCanvas = new wxMoldeoObjectCanvas(this, ID_PANEL1, wxPoint(464,40), wxSize(344,272), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	Effect2 = new wxMoldeoObjectCtrl(ConsoleCanvas, ID_EFFECT2, wxPoint(216,40), wxSize(112,184), wxTAB_TRAVERSAL, _T("ID_EFFECT2"));
	Pin2 = new wxMoldeoPinCtrl(Effect2, ID_MOLDEOPIN2, 0, 0, 100, wxPoint(8,32), wxDefaultSize, 0, wxDefaultValidator, _T("ID_MOLDEOPIN2"));
	Effect1 = new wxMoldeoObjectCtrl(ConsoleCanvas, ID_EFFECT1, wxPoint(8,8), wxSize(160,136), wxTAB_TRAVERSAL, _T("ID_EFFECT1"));
	Effect3 = new wxMoldeoObjectCtrl(ConsoleCanvas, ID_EFFECT3, wxPoint(24,176), wxSize(184,88), wxTAB_TRAVERSAL, _T("ID_EFFECT3"));
	Pin1 = new wxMoldeoPinCtrl(Effect3, ID_PIN1, 0, 0, 100, wxPoint(32,40), wxSize(112,24), 0, wxDefaultValidator, _T("ID_PIN1"));
	Panel1 = new wxMoldeoObjectCanvas(this, ID_PANEL2, wxPoint(464,320), wxSize(344,88), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
	Panel2 = new wxMoldeoObjectCtrl(Panel1, ID_PANEL3, wxPoint(24,24), wxSize(120,48), wxTAB_TRAVERSAL, _T("ID_PANEL3"));
	ColourPanel = new wxMoColourPanel(this, ID_COLOURPANEL, wxPoint(304,304), wxSize(120,28), wxTAB_TRAVERSAL, _T("ID_COLOURPANEL"));
	AuiNotebook = new wxAuiNotebook(this, ID_AUINOTEBOOK, wxPoint(24,344), wxSize(304,128), wxBORDER_NONE | wxNO_3D | wxNO_BORDER );
	AuiNotebook->SetArtProvider( new wxMoAuiTabArt());
	//AuiNotebook->SetForegroundColour(wxColor(0,0,0));
	//AuiNotebook->SetBackgroundColour(wxColor(0,0,0));

	Panel3 = new wxPanel(AuiNotebook, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
	Panel4 = new wxPanel(AuiNotebook, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
	Panel5 = new wxPanel(AuiNotebook, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL6"));
	AuiNotebook->AddPage(Panel3, _("Project"), false);
	AuiNotebook->AddPage(Panel4, _("Data"), false);
	AuiNotebook->AddPage(Panel5, _("Custom"), false);

	Connect(ID_BUTTONCLOSE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnButtonCloseClick);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnSpinCtrl1Change);
	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnSpinCtrl1Change);
	Connect(ID_BUTTONOK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnButtonCloseClick);
	//*)

    Connect(wxMoldeoControlsTestDialog::ID_SLIDER1, wxEVT_MOLEVELCTRL,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnMoLevelCtrl);
    Slider1->ChangeValue( (int) 44 );

    Connect(ID_SLIDER2,wxEVT_MOROTARYCTRL,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnSlider2CmdScroll);
    Slider2->ChangeValue( (int) 44 );

    Connect(ID_SLIDER3,wxEVT_MOROTARYCTRL,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnSlider3CmdScroll);
    Slider3->ChangeValue( (int) 44 );
    Slider3->SetRangeAngle( 0.4*3.141516, 1.6*3.141516);

    Connect(ID_SLIDER4,wxEVT_MOROTARYCTRL,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnSlider4CmdScroll);
    Slider4->ChangeValue( (int) 44 );

    Connect(ID_SLIDER5,wxEVT_MOROTARYCTRL,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnSlider5CmdScroll);
    Slider5->ChangeValue( (int) 44 );

    RedColorLevelCtrl->Init( wxMoColorLevelCtrl::RED );
    GreenColorLevelCtrl->Init( wxMoColorLevelCtrl::GREEN );
    BlueColorLevelCtrl->Init( wxMoColorLevelCtrl::BLUE );
    AlphaColorLevelCtrl->Init( wxMoColorLevelCtrl::ALPHA );

    HueColorLevelCtrl->Init( wxMoColorLevelCtrl::HUE );
    SaturationColorLevelCtrl->Init( wxMoColorLevelCtrl::SATURATION );
    ValueColorLevelCtrl->Init( wxMoColorLevelCtrl::VALUE );

    RedColorLevelCtrl->SetRGBAColorComponents( HueColorLevelCtrl, GreenColorLevelCtrl, BlueColorLevelCtrl, AlphaColorLevelCtrl );
    RedColorLevelCtrl->SetHSVColorComponents( HueColorLevelCtrl, SaturationColorLevelCtrl, ValueColorLevelCtrl );
    RedColorLevelCtrl->SetColourPanel( ColourPanel );

    GreenColorLevelCtrl->SetRGBAColorComponents( RedColorLevelCtrl, GreenColorLevelCtrl, BlueColorLevelCtrl, AlphaColorLevelCtrl );
    GreenColorLevelCtrl->SetHSVColorComponents( HueColorLevelCtrl, SaturationColorLevelCtrl, ValueColorLevelCtrl );
    GreenColorLevelCtrl->SetColourPanel( ColourPanel );

    BlueColorLevelCtrl->SetRGBAColorComponents( RedColorLevelCtrl, GreenColorLevelCtrl, BlueColorLevelCtrl, AlphaColorLevelCtrl );
    BlueColorLevelCtrl->SetHSVColorComponents( HueColorLevelCtrl, SaturationColorLevelCtrl, ValueColorLevelCtrl );
    BlueColorLevelCtrl->SetColourPanel( ColourPanel );

    AlphaColorLevelCtrl->SetRGBAColorComponents( RedColorLevelCtrl, GreenColorLevelCtrl, BlueColorLevelCtrl, AlphaColorLevelCtrl );
    AlphaColorLevelCtrl->SetHSVColorComponents( HueColorLevelCtrl, SaturationColorLevelCtrl, ValueColorLevelCtrl );
    AlphaColorLevelCtrl->SetColourPanel( ColourPanel );

    HueColorLevelCtrl->SetRGBAColorComponents( RedColorLevelCtrl, GreenColorLevelCtrl, BlueColorLevelCtrl, AlphaColorLevelCtrl );
    HueColorLevelCtrl->SetHSVColorComponents( HueColorLevelCtrl, SaturationColorLevelCtrl, ValueColorLevelCtrl );
    HueColorLevelCtrl->SetColourPanel( ColourPanel );

    SaturationColorLevelCtrl->SetRGBAColorComponents( RedColorLevelCtrl, GreenColorLevelCtrl, BlueColorLevelCtrl, AlphaColorLevelCtrl );
    SaturationColorLevelCtrl->SetHSVColorComponents( HueColorLevelCtrl, SaturationColorLevelCtrl, ValueColorLevelCtrl );
    SaturationColorLevelCtrl->SetColourPanel( ColourPanel );

    ValueColorLevelCtrl->SetRGBAColorComponents( RedColorLevelCtrl, GreenColorLevelCtrl, BlueColorLevelCtrl, AlphaColorLevelCtrl );
    ValueColorLevelCtrl->SetHSVColorComponents( HueColorLevelCtrl, SaturationColorLevelCtrl, ValueColorLevelCtrl );
    ValueColorLevelCtrl->SetColourPanel( ColourPanel );

    wxColour micol = wxColour( 0, 0, 0, 255);

    RedColorLevelCtrl->SetColourValue( micol, true );

    AuiNotebook->SetForegroundColour( wxColour( 0,0,0 ) );
    AuiNotebook->SetBackgroundColour( wxColour( 0,0,0 ) );

    for( int i=0; i<AuiNotebook->GetPageCount(); i++ ) {
      AuiNotebook->GetPage(i)->SetBackgroundColour(wxColour( 0,0,0 ));
    }

    ConsoleCanvas->SetScrollbars(20, 20, 50, 50);

   wxImageHandler* hndPNG = wxImage::FindHandler((long)wxBITMAP_TYPE_PNG);
   if (!hndPNG)
        cout << "Warning: PNG Image handler not loaded..." << endl;

    wxImage* img = new wxImage( "./moldeologo.png" );
    int w,h;
    if ( img->IsOk()) {
        w = img->GetWidth();
        h = img->GetHeight();
    }

    ColourPanel->SetBitmap( wxBitmap( *img ) );


    int context_format[] =
      {
        WX_GL_RGBA,
        WX_GL_DOUBLEBUFFER,
        WX_GL_MIN_RED, 8,
        WX_GL_MIN_GREEN, 8,
        WX_GL_MIN_BLUE, 8,
        WX_GL_MIN_ALPHA, 8,
        WX_GL_DEPTH_SIZE, 24,
        WX_GL_STENCIL_SIZE, 8,
        /*WX_GL_LEVEL, 0,
        WX_GL_AUX_BUFFERS, 0*/
        0
      };



    wxMoGLCanvas* pCanvas = new wxMoGLCanvas( this, -1, context_format, wxPoint(40,40), wxSize(50,50));
    wxGLContext* pContext = new wxGLContext(pCanvas);

    pContext->SetCurrent(*pCanvas);
    Show();


	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Problem: glewInit failed, something is seriously wrong.
		//MODebug2->Error(moText("GLEW Error: ")+ moText((char*)glewGetErrorString(err)));
        wxMessageBox(wxString("1-")+wxString(glewGetErrorString(err)));		//cout << glewGetErrorString(err) << endl;


	} else {
	    wxMessageBox("1- ok");
   }


    if (pCanvas) {
        pCanvas->Init();
        pCanvas->SwapBuffers();
        pCanvas->SwapBuffers();
        pCanvas->SwapBuffers();
    }


}

wxMoldeoControlsTestDialog::~wxMoldeoControlsTestDialog()
{
	//(*Destroy(wxMoldeoControlsTestDialog)
	//*)
}


void wxMoldeoControlsTestDialog::OnButtonCloseClick(wxCommandEvent& event)
{
        Close();
}

void wxMoldeoControlsTestDialog::OnMoLevelCtrl(wxCommandEvent& event)
{
    SpinCtrl1->SetValue( event.GetInt() );
}

void wxMoldeoControlsTestDialog::OnSpinCtrl1Change(wxSpinEvent& event)
{
    Slider1->ChangeValue( event.GetPosition() );
}

void wxMoldeoControlsTestDialog::OnButtonChangeClick(wxCommandEvent& event)
{
    Slider1->ChangeValue( 20 );
}

void wxMoldeoControlsTestDialog::OnSlider2CmdScroll(wxCommandEvent& event)
{
    SpinCtrl2->SetValue( event.GetInt() );
}

void wxMoldeoControlsTestDialog::OnSlider3CmdScroll(wxCommandEvent& event)
{
    SpinCtrl2->SetValue( event.GetInt() );
}

void wxMoldeoControlsTestDialog::OnSlider4CmdScroll(wxCommandEvent& event)
{
    SpinCtrl2->SetValue( event.GetInt() );
}

void wxMoldeoControlsTestDialog::OnSlider5CmdScroll(wxCommandEvent& event)
{
    SpinCtrl2->SetValue( event.GetInt() );
}
