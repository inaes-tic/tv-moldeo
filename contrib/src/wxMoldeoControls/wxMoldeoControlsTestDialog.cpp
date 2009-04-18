#include "wxMoldeoControlsTestDialog.h"

//(*InternalHeaders(wxMoldeoControlsTestDialog)
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/glcanvas.h>

class moGLCANVAS : public wxGLCanvas {


};

//(*IdInit(wxMoldeoControlsTestDialog)
const long wxMoldeoControlsTestDialog::ID_STATICTEXT1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_BUTTON1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER4 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER5 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER3 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER2 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SLIDER1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_STATICTEXT3 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_STATICTEXT2 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SPINCTRL2 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_SPINCTRL1 = wxNewId();
const long wxMoldeoControlsTestDialog::ID_BUTTON2 = wxNewId();
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
//*)

BEGIN_EVENT_TABLE(wxMoldeoControlsTestDialog,wxDialog)
	//(*EventTable(wxMoldeoControlsTestDialog)
	//*)
END_EVENT_TABLE()

wxMoldeoControlsTestDialog::wxMoldeoControlsTestDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(wxMoldeoControlsTestDialog)
	Create(parent, wxID_ANY, _("Moldeo Controls Test"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(966,456));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Moldeo Controls"), wxPoint(8,8), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	wxFont StaticText1Font(20,wxSWISS,wxFONTSTYLE_NORMAL,wxBOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText1->SetFont(StaticText1Font);
	//Button1 = new wxButton(this, ID_BUTTON1, _T("Close"), wxPoint(440,424), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	Slider4 = new wxMoRotaryCtrl(this, ID_SLIDER4, 0, 0, 100, wxPoint(152,112), wxSize(32,32), 0, wxDefaultValidator, _T("ID_SLIDER4"));
	Slider5 = new wxMoRotaryCtrl(this, ID_SLIDER5, 0, 0, 100, wxPoint(192,112), wxSize(32,32), 0, wxDefaultValidator, _T("ID_SLIDER5"));
	Slider3 = new wxMoRotaryCtrl(this, ID_SLIDER3, 0, 0, 100, wxPoint(232,112), wxSize(56,56), 0, wxDefaultValidator, _T("ID_SLIDER3"));
	Slider2 = new wxMoRotaryCtrl(this, ID_SLIDER2, 0, 0, 100, wxPoint(112,112), wxSize(32,32), 0, wxDefaultValidator, _T("ID_SLIDER2"));
	Slider1 = new wxMoLevelCtrl(this, ID_SLIDER1, 0, 0, 100, wxPoint(104,72), wxSize(128,16), 0, wxDefaultValidator, _T("ID_SLIDER1"));
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("wxMoLevelCtrl"), wxPoint(8,72), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("wxMoRotaryCtrl"), wxPoint(8,120), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	SpinCtrl2 = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxPoint(352,112), wxSize(53,21), 0, 0, 100, 0, _T("ID_SPINCTRL2"));
	SpinCtrl2->SetValue(_T("0"));
	SpinCtrl1 = new wxSpinCtrl(this, ID_SPINCTRL1, _T("0"), wxPoint(240,64), wxSize(53,21), 0, 0, 100, 0, _T("ID_SPINCTRL1"));
	SpinCtrl1->SetValue(_T("0"));
	//Button2 = new wxButton(this, ID_BUTTON2, _T("Label"), wxPoint(352,424), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("wxMoTimelineRulerCtrl"), wxPoint(8,168), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("wxMoldeoObjectCanvas"), wxPoint(464,16), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("wxMoTimelineBandCtrl"), wxPoint(8,224), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	Slider6 = new wxMoTimelineRulerCtrl(this, ID_SLIDER6, 0, 0, 100, wxPoint(8,192), wxSize(448,24), 0, wxDefaultValidator, _T("ID_SLIDER6"));
	Slider7 = new wxMoTimelineBandCtrl(this, ID_SLIDER7, 0, 0, 100, wxPoint(8,248), wxSize(448,24), 0, wxDefaultValidator, _T("ID_SLIDER7"));
	ConsoleCanvas = new wxMoldeoObjectCanvas(this, ID_PANEL1, wxPoint(464,40), wxSize(480,344), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	Effect2 = new wxMoldeoObjectCtrl(ConsoleCanvas, ID_EFFECT2, wxPoint(320,32), wxSize(128,184), wxTAB_TRAVERSAL, _T("ID_EFFECT2"));
	Pin2 = new wxMoldeoPinCtrl(Effect2, ID_MOLDEOPIN2, 0, 0, 100, wxPoint(8,32), wxDefaultSize, 0, wxDefaultValidator, _T("ID_MOLDEOPIN2"));
	Effect1 = new wxMoldeoObjectCtrl(ConsoleCanvas, ID_EFFECT1, wxPoint(8,8), wxSize(160,136), wxTAB_TRAVERSAL, _T("ID_EFFECT1"));
	Effect3 = new wxMoldeoObjectCtrl(ConsoleCanvas, ID_EFFECT3, wxPoint(24,176), wxSize(184,144), wxTAB_TRAVERSAL, _T("ID_EFFECT3"));
	Pin1 = new wxMoldeoPinCtrl(Effect3, ID_PIN1, 0, 0, 100, wxPoint(72,32), wxSize(112,24), 0, wxDefaultValidator, _T("ID_PIN1"));
	Panel1 = new wxMoldeoObjectCanvas(this, ID_PANEL2, wxPoint(32,304), wxSize(368,88), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
	Panel2 = new wxMoldeoObjectCtrl(Panel1, ID_PANEL3, wxPoint(24,24), wxSize(120,48), wxTAB_TRAVERSAL, _T("ID_PANEL3"));

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnButton1Click);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnSpinCtrl1Change);
	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnSpinCtrl1Change);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxMoldeoControlsTestDialog::OnButton2Click);
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


    ConsoleCanvas->SetScrollbars(20, 20, 50, 50);
}

wxMoldeoControlsTestDialog::~wxMoldeoControlsTestDialog()
{
	//(*Destroy(wxMoldeoControlsTestDialog)
	//*)
}


void wxMoldeoControlsTestDialog::OnButton1Click(wxCommandEvent& event)
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

void wxMoldeoControlsTestDialog::OnButton2Click(wxCommandEvent& event)
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
