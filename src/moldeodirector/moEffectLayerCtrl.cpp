#include "moEffectLayerCtrl.h"

//(*InternalHeaders(moEffectLayerCtrl)
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*IdInit(moEffectLayerCtrl)
const long moEffectLayerCtrl::ID_SLIDERALPHA = wxNewId();
const long moEffectLayerCtrl::ID_SLIDERTINT = wxNewId();
const long moEffectLayerCtrl::ID_SLIDERTEMPO = wxNewId();
const long moEffectLayerCtrl::ID_COMBOBOXPRE = wxNewId();
const long moEffectLayerCtrl::ID_CHECKBOXONOFF = wxNewId();
const long moEffectLayerCtrl::ID_STATICTEXTLABEL = wxNewId();
const long moEffectLayerCtrl::ID_STATICBITMAPCTRLS = wxNewId();
const long moEffectLayerCtrl::ID_PANELLABELCTRLS = wxNewId();
const long moEffectLayerCtrl::ID_STATICBITMAP1 = wxNewId();
const long moEffectLayerCtrl::ID_PANELCOLORS = wxNewId();
const long moEffectLayerCtrl::ID_BITMAPBUTTONVISIBILITY = wxNewId();
const long moEffectLayerCtrl::ID_BITMAPBUTTONDELETE = wxNewId();
//*)

const long moEffectLayerCtrl::ID_LEVELALPHA = wxNewId();
const long moEffectLayerCtrl::ID_LEVELTINT = wxNewId();
const long moEffectLayerCtrl::ID_LEVELSPEED = wxNewId();

BEGIN_EVENT_TABLE(moEffectLayerCtrl,wxPanel)
	//(*EventTable(moEffectLayerCtrl)
	//*)
	EVT_MOUSE_EVENTS( moEffectLayerCtrl::OnMouseEvent )
END_EVENT_TABLE()

moEffectLayerCtrl::moEffectLayerCtrl(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(moEffectLayerCtrl)
	wxStaticBitmap* StaticBitmap1;
	
	Create(parent, wxID_ANY, wxPoint(0,0), wxSize(168,71), 0, _T("wxID_ANY"));
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetForegroundColour(wxColour(255,255,255));
	SetBackgroundColour(wxColour(0,0,0));
	SliderAlpha = new wxSlider(this, ID_SLIDERALPHA, 0, 0, 100, wxPoint(0,28), wxSize(56,9), 0, wxDefaultValidator, _T("ID_SLIDERALPHA"));
	SliderTint = new wxSlider(this, ID_SLIDERTINT, 0, 0, 100, wxPoint(0,42), wxSize(56,9), 0, wxDefaultValidator, _T("ID_SLIDERTINT"));
	SliderTempo = new wxSlider(this, ID_SLIDERTEMPO, 0, 0, 100, wxPoint(0,54), wxSize(56,9), 0, wxDefaultValidator, _T("ID_SLIDERTEMPO"));
	ComboBoxPreConfig = new wxComboBox(this, ID_COMBOBOXPRE, wxEmptyString, wxPoint(112,0), wxSize(56,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOXPRE"));
	ComboBoxPreConfig->SetSelection( ComboBoxPreConfig->Append(_("Pre 0")) );
	ComboBoxPreConfig->Append(_("Pre 1"));
	ComboBoxPreConfig->Append(_("Pre 2"));
	ComboBoxPreConfig->Append(_("Pre 3"));
	ComboBoxPreConfig->Append(_("Pre 4"));
	ComboBoxPreConfig->Append(_("Pre 5"));
	ComboBoxPreConfig->Append(_("Pre 6"));
	ComboBoxPreConfig->Append(_("Pre 7"));
	ComboBoxPreConfig->Append(_("Pre 8"));
	ComboBoxPreConfig->Append(_("Pre 9"));
	ComboBoxPreConfig->Append(_("Pre 10"));
	CheckBoxOnOff = new wxCheckBox(this, ID_CHECKBOXONOFF, wxEmptyString, wxPoint(108,48), wxSize(16,21), 0, wxDefaultValidator, _T("ID_CHECKBOXONOFF"));
	CheckBoxOnOff->SetValue(false);
	TextCtrlLabel = new wxStaticText(this, ID_STATICTEXTLABEL, _("label"), wxPoint(8,4), wxDefaultSize, 0, _T("ID_STATICTEXTLABEL"));
	PanelLabelCtrls = new wxPanel(this, ID_PANELLABELCTRLS, wxPoint(55,24), wxSize(48,48), wxTAB_TRAVERSAL, _T("ID_PANELLABELCTRLS"));
	PanelLabelCtrls->SetBackgroundColour(wxColour(0,0,0));
	StaticBitmapCtrls = new wxStaticBitmap(PanelLabelCtrls, ID_STATICBITMAPCTRLS, wxBitmap(wxImage(_T("D:\\_data\\proyectos\\SINESTESIA\\OPENSVN\\trunk\\0.7\\doc\\icons\\layercontrols.png"))), wxPoint(0,0), wxDefaultSize, 0, _T("ID_STATICBITMAPCTRLS"));
	PanelColors = new wxPanel(this, ID_PANELCOLORS, wxPoint(132,26), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANELCOLORS"));
	PanelColors->SetBackgroundColour(wxColour(0,0,0));
	StaticBitmap1 = new wxStaticBitmap(PanelColors, ID_STATICBITMAP1, wxBitmap(wxImage(_T("D:\\_data\\proyectos\\SINESTESIA\\OPENSVN\\trunk\\0.7\\doc\\icons\\csvColors32.png"))), wxPoint(0,0), wxDefaultSize, 0, _T("ID_STATICBITMAP1"));
	BitmapButtonVisibility = new wxBitmapButton(this, ID_BITMAPBUTTONVISIBILITY, wxBitmap(wxImage(_T("D:\\_data\\proyectos\\SINESTESIA\\OPENSVN\\trunk\\0.7\\doc\\icons\\visibility.png"))), wxPoint(100,26), wxSize(26,21), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTONVISIBILITY"));
	BitmapButtonVisibility->SetBitmapDisabled(wxBitmap(wxImage(_T("D:\\_data\\proyectos\\SINESTESIA\\OPENSVN\\trunk\\0.7\\doc\\icons\\visibility.png"))));
	BitmapButtonVisibility->SetBitmapSelected(wxBitmap(wxImage(_T("D:\\_data\\proyectos\\SINESTESIA\\OPENSVN\\trunk\\0.7\\doc\\icons\\visibility.png"))));
	BitmapButtonVisibility->SetBitmapFocus(wxBitmap(wxImage(_T("D:\\_data\\proyectos\\SINESTESIA\\OPENSVN\\trunk\\0.7\\doc\\icons\\visibility.png"))));
	BitmapButtonVisibility->SetDefault();
	BitmapButtonDelete = new wxBitmapButton(this, ID_BITMAPBUTTONDELETE, wxBitmap(wxImage(_T("../../doc/icons/delete.png"))), wxPoint(88,4), wxSize(13,13), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTONDELETE"));
	BitmapButtonDelete->SetDefault();
	
	Connect(ID_SLIDERALPHA,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&moEffectLayerCtrl::OnSliderAlphaCmdScroll);
	Connect(ID_SLIDERALPHA,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&moEffectLayerCtrl::OnSliderAlphaCmdScroll);
	Connect(ID_SLIDERTINT,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&moEffectLayerCtrl::OnSliderTintCmdScrollThumbTrack);
	Connect(ID_SLIDERTINT,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&moEffectLayerCtrl::OnSliderTintCmdScrollThumbTrack);
	Connect(ID_SLIDERTEMPO,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&moEffectLayerCtrl::OnSliderTempoCmdScroll);
	Connect(ID_SLIDERTEMPO,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&moEffectLayerCtrl::OnSliderTempoCmdScroll);
	Connect(ID_COMBOBOXPRE,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&moEffectLayerCtrl::OnComboBoxPreConfigSelect);
	Connect(ID_CHECKBOXONOFF,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&moEffectLayerCtrl::OnCheckBoxOnOffClick);
	Connect(ID_BITMAPBUTTONVISIBILITY,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&moEffectLayerCtrl::OnBitmapButtonVisibilityClick);
	Connect(ID_BITMAPBUTTONDELETE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&moEffectLayerCtrl::OnBitmapButtonDeleteClick);
	//*)

	m_pLevelAlphaCtrl =  new moWxLevelCtrl(this, ID_LEVELALPHA, SliderAlpha->GetPosition(), SliderAlpha->GetSize(), 0, wxDefaultValidator, _T("ID_LEVELALPHA") );
    Connect(ID_LEVELALPHA, MO_EVT_DIRECTOR_LEVEL, (wxObjectEventFunction)&moEffectLayerCtrl::OnLevelAlpha );
	if (SliderAlpha) SliderAlpha->Destroy();

	m_pLevelTintCtrl =  new moWxLevelCtrl(this, ID_LEVELTINT, SliderTint->GetPosition(), SliderTint->GetSize(), 0, wxDefaultValidator, _T("ID_LEVELTINT") );
    Connect(ID_LEVELTINT, MO_EVT_DIRECTOR_LEVEL, (wxObjectEventFunction)&moEffectLayerCtrl::OnLevelTint );
	if (SliderTint) SliderTint->Destroy();

	m_pLevelSpeedCtrl =  new moWxLevelCtrl(this, ID_LEVELSPEED, SliderTempo->GetPosition(), SliderTempo->GetSize(), 0, wxDefaultValidator, _T("ID_LEVELSPEED") );
    Connect(ID_LEVELSPEED, MO_EVT_DIRECTOR_LEVEL, (wxObjectEventFunction)&moEffectLayerCtrl::OnLevelSpeed );
	if (SliderTempo) SliderTempo->Destroy();


}

moEffectLayerCtrl::~moEffectLayerCtrl()
{
	//(*Destroy(moEffectLayerCtrl)
	//*)
}

void moEffectLayerCtrl::Set( moMobDescriptor p_MobDesc ) {

    m_MobDescriptor = p_MobDesc;

    /*
    TextCtrlName->SetValue((wxChar*)(char*) m_MobDescriptor.GetMobDefinition().GetName());
    TextCtrlName->SetEditable(false);
*/

    TextCtrlLabel->SetLabel((wxChar*)(char*) m_MobDescriptor.GetMobDefinition().GetLabelName());
    //TextCtrlLabel->SetEditable(false);
    TextCtrlLabel->SetForegroundColour( wxColour(255,255,255) );
	TextCtrlLabel->SetBackgroundColour( wxColour(0,0,0) );
/*
    TextCtrlConfigName->SetValue((wxChar*)(char*) m_MobDescriptor.GetMobDefinition().GetConfigName());
    TextCtrlConfigName->SetEditable(false);
*/
    switch( p_MobDesc.GetState().GetEffectState().on ) {
            case MO_OFF:
                CheckBoxOnOff->SetValue(false);
                break;
            case MO_ON:
                CheckBoxOnOff->SetValue(true);
                break;
    }

    /*
    SliderAlpha->SetValue( (int)p_MobDesc.GetState().GetEffectState().alpha*100 );
    SliderTempo->SetValue( (int)p_MobDesc.GetState().GetEffectState().tempo.delta*50 );
    SliderTint->SetValue( (int)p_MobDesc.GetState().GetEffectState().tint*100 );
    */

    m_pLevelAlphaCtrl->ChangeValue( (int)p_MobDesc.GetState().GetEffectState().alpha*100 );
    m_pLevelSpeedCtrl->ChangeValue( (int)p_MobDesc.GetState().GetEffectState().tempo.delta*50 );
    m_pLevelTintCtrl->ChangeValue( (int)p_MobDesc.GetState().GetEffectState().tint*100 );


}


void moEffectLayerCtrl::OnSliderAlphaCmdScroll(wxScrollEvent& event)
{

}

void moEffectLayerCtrl::OnSliderTempoCmdScroll(wxScrollEvent& event)
{

}

void moEffectLayerCtrl::OnSliderTintCmdScrollThumbTrack(wxScrollEvent& event)
{

}

void moEffectLayerCtrl::OnCheckBoxOnOffClick(wxCommandEvent& event)
{
    moMobState MobState = m_MobDescriptor.GetState();

    MobState.GetEffectState().on*= -1;

    m_MobDescriptor.SetState(MobState);

    SetMob( m_MobDescriptor );
}


void moEffectLayerCtrl::OnLevelAlpha(wxCommandEvent& event) {

    moMobState MobState = m_MobDescriptor.GetState();

    MobState.GetEffectState().alpha = (float) event.GetInt() / 100.0;

    m_MobDescriptor.SetState(MobState);

    SetMob( m_MobDescriptor );

}


void moEffectLayerCtrl::OnLevelTint(wxCommandEvent& event) {

    moMobState MobState = m_MobDescriptor.GetState();

    MobState.GetEffectState().tint = (float) event.GetInt() / 100.0;

    m_MobDescriptor.SetState(MobState);

    SetMob( m_MobDescriptor );

}

void moEffectLayerCtrl::OnLevelSpeed(wxCommandEvent& event) {

    moMobState MobState = m_MobDescriptor.GetState();
    MobState.GetEffectState().synchronized = MO_DEACTIVATED;
    MobState.GetEffectState().tempo.delta = (float) event.GetInt() / 50.0;

    m_MobDescriptor.SetState(MobState);

    SetMob( m_MobDescriptor );

}

void moEffectLayerCtrl::OnComboBoxPreConfigSelect(wxCommandEvent& event)
{
}


void moEffectLayerCtrl::OnPanelVisibilityMouseMove(wxMouseEvent& event)
{
    wxMessageBox("hi there");
    SetCursor( wxCursor(wxCURSOR_HAND ));
}

void moEffectLayerCtrl::OnBitmapButtonVisibilityClick(wxCommandEvent& event)
{
    moMobState MobState = m_MobDescriptor.GetState();

    MobState.GetEffectState().on*= -1;

    m_MobDescriptor.SetState(MobState);

    SetMob( m_MobDescriptor );
}

void moEffectLayerCtrl::OnMouseEvent( wxMouseEvent& event ) {

    if ( event.Button(wxMOUSE_BTN_LEFT) || event.ButtonDown() ) {

        GetParent()->ProcessEvent(event);

        OpenMob( m_MobDescriptor );
    }
    event.Skip();

}


moDirectorStatus
moEffectLayerCtrl::ValueUpdated( moValueDescriptor p_ValueDesc ) {


    return MO_DIRECTOR_STATUS_OK;

}


moDirectorStatus
moEffectLayerCtrl::MobUpdated( moMobDescriptor p_MobDesc ) {


    return MO_DIRECTOR_STATUS_OK;

}



void moEffectLayerCtrl::OnBitmapButtonDeleteClick(wxCommandEvent& event)
{
    if( wxMessageBox(_T("Really Delete this layer ?"), _T("Please confirm"),
                          wxICON_QUESTION | wxYES_NO) == wxYES )
    {
        DeleteMob( m_MobDescriptor );
	}
}


BEGIN_EVENT_TABLE(moEffectLayerTimelineCtrl,wxPanel)
    EVT_PAINT( moEffectLayerTimelineCtrl::OnPaint )
    EVT_MOUSE_EVENTS( moEffectLayerTimelineCtrl::OnMouseEvent )
END_EVENT_TABLE()

moEffectLayerTimelineCtrl::moEffectLayerTimelineCtrl(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{

    Create(parent, wxID_ANY, wxPoint(0,0), wxSize(800,71), wxNO_BORDER, _T("wxID_ANY"));
    SetBackgroundColour( wxColour(200,200,200));

    m_BackgroundImage = wxImage( 800, 71, true ); //to black

    m_BackgroundName = moText("");

}

moEffectLayerTimelineCtrl::~moEffectLayerTimelineCtrl()
{

}



void moEffectLayerTimelineCtrl::OnPaint( wxPaintEvent& event ) {

    wxPaintDC dc(this);

    int w,h;

    GetSize(&w,&h);

    wxBrush BackBrush( wxColour(120,120,120), wxSOLID );

    //draw background bitmap
    if (m_BackgroundImage.IsOk()) {
        dc.DrawBitmap( wxBitmap( m_BackgroundImage.Rescale(m_BackgroundImage.GetWidth(),h), -1 ), 0, 0, true );
        BackBrush.SetStipple( wxBitmap( m_BackgroundImage, -1 ) );
        BackBrush.SetStyle( wxSTIPPLE );
    }

    dc.SetBrush( BackBrush );
    dc.SetPen( wxPen( wxColour(170,170,170), 1, wxSOLID) );

    //background
    dc.DrawRectangle( 0, 0, GetSize().x, GetSize().y );



    //gauge
    //dc.SetBrush( wxBrush( wxColour(220,220,220), wxSOLID) );
    //dc.SetPen( wxPen( wxColour(140,140,140), 1, wxTRANSPARENT ) );
    //dc.DrawRectangle( 0, 0, SlidePosition(), GetSize().y );

    //ticks:
    dc.SetBrush( wxBrush( wxColour(120,120,120), wxSOLID ) );
    dc.SetPen( wxPen( wxColour(170,170,170), 1, wxSOLID) );

    int linedelta = m_MobDescriptor.GetState().GetEffectState().tempo.delta;

    for(int i =0; i<GetSize().x; i+= ( (GetSize().x / 40 ) * linedelta )) {
        dc.DrawLine( i, 0, i, GetSize().y );
    }

}

void moEffectLayerTimelineCtrl::OnMouseEvent( wxMouseEvent& event ) {

    if ( event.Button(wxMOUSE_BTN_LEFT) || event.ButtonDown() ) {

        GetParent()->ProcessEvent(event);

        OpenMob( m_MobDescriptor );

    }
    event.Skip();

}

moDirectorStatus
moEffectLayerTimelineCtrl::ValueUpdated( moValueDescriptor p_ValueDesc ) {

    switch(p_ValueDesc.GetParamDescriptor().GetParamDefinition().GetType() ) {
        case MO_PARAM_TEXTURE:
            //LOAD IMAGE....
            //Log("loading background");
            if ( p_ValueDesc.GetParamDescriptor().GetParamDefinition().GetType()==MO_PARAM_TEXTURE
             && p_ValueDesc.GetValue().GetSubValueCount()>=1 ) {

                moText tname = p_ValueDesc.GetValue().GetSubValue(0).Text();

                if(tname!=m_BackgroundName) {

                    m_BackgroundName = tname;

                    moTextureDescriptor pTextDescriptor( GetResourceDescriptor( moTextureDescriptor( tname ) ) );

                    m_BackgroundImage.LoadFile( moText2Wx( pTextDescriptor.GetResourceDefinition().GetFileName() ) );

                    Refresh();
                }
            }
            break;
        default:
            break;
    }

    return MO_DIRECTOR_STATUS_OK;

}


moDirectorStatus
moEffectLayerTimelineCtrl::MobUpdated( moMobDescriptor p_MobDesc ) {


    return MO_DIRECTOR_STATUS_OK;

}

