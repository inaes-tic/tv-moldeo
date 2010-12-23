#ifndef WXMOLDEOCONTROLSTESTDIALOG_H
#define WXMOLDEOCONTROLSTESTDIALOG_H

//(*Headers(wxMoldeoControlsTestDialog)
#include <wx/notebook.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include <iostream>
using namespace std;

#include <wx/aui/auibook.h>
#include "wxMoldeoControls.h"

#include "WxMoGLCanvas.h"

class wxMoldeoControlsTestDialog: public wxDialog
{
	public:

		wxMoldeoControlsTestDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~wxMoldeoControlsTestDialog();

		//(*Declarations(wxMoldeoControlsTestDialog)
		wxMoColourPanel* ColourPanel;
		wxMoColorLevelCtrl* RedColorLevelCtrl;
		wxMoColorLevelCtrl* SaturationColorLevelCtrl;
		wxPanel* Panel5;
		wxMoTimelineRulerCtrl* Slider6;
		wxSpinCtrl* SpinCtrl1;
		wxStaticText* StaticText2;
		wxPanel* Panel4;
		wxMoRotaryCtrl* Slider3;
		wxStaticText* StaticText6;
		wxMoldeoObjectCanvas* ConsoleCanvas;
		wxStaticText* StaticTextColorLevelCtrl;
		wxAuiNotebook* AuiNotebook;
		wxButton* ButtonOK;
		wxButton* ButtonCLose;
		wxMoColorLevelCtrl* GreenColorLevelCtrl;
		wxMoldeoObjectCtrl* Effect1;
		wxMoRotaryCtrl* Slider5;
		wxMoldeoObjectCtrl* Panel2;
		wxMoRotaryCtrl* Slider4;
		wxStaticText* StaticText1;
		wxMoRotaryCtrl* Slider2;
		wxMoTimelineBandCtrl* Slider7;
		wxPanel* Panel3;
		wxMoColorLevelCtrl* AlphaColorLevelCtrl;
		wxMoColorLevelCtrl* ValueColorLevelCtrl;
		wxStaticText* StaticText5;
		wxStaticText* StaticText7;
		wxMoLevelCtrl* Slider1;
		wxMoldeoObjectCanvas* Panel1;
		wxSpinCtrl* SpinCtrl2;
		wxMoldeoObjectCtrl* Effect3;
		wxMoldeoPinCtrl* Pin2;
		wxMoColorLevelCtrl* BlueColorLevelCtrl;
		wxStaticText* StaticText4;
		wxMoldeoPinCtrl* Pin1;
		wxMoldeoObjectCtrl* Effect2;
		wxMoColorLevelCtrl* HueColorLevelCtrl;
		//*)

	protected:

		//(*Identifiers(wxMoldeoControlsTestDialog)
		static const long ID_STATICTEXT1;
		static const long ID_BUTTONCLOSE;
		static const long ID_SLIDER4;
		static const long ID_SLIDER5;
		static const long ID_SLIDER3;
		static const long ID_SLIDER2;
		static const long ID_SLIDER1;
		static const long ID_VALUECOLORLEVELCTRL;
		static const long ID_SATURATIONCOLORLEVELCTRL;
		static const long ID_HUECOLORLEVELCTRL;
		static const long ID_ALPHACOLORLEVEL;
		static const long ID_BLUECOLORLEVELCTRL;
		static const long ID_GREENCOLORLEVELCTRL;
		static const long ID_REDCOLORLEVELCTRL;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXTCOLORLEVELCTRL;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTONOK;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT6;
		static const long ID_STATICTEXT4;
		static const long ID_SLIDER6;
		static const long ID_SLIDER7;
		static const long ID_MOLDEOPIN2;
		static const long ID_EFFECT2;
		static const long ID_EFFECT1;
		static const long ID_PIN1;
		static const long ID_EFFECT3;
		static const long ID_PANEL1;
		static const long ID_PANEL3;
		static const long ID_PANEL2;
		static const long ID_COLOURPANEL;
		static const long ID_PANEL4;
		static const long ID_PANEL5;
		static const long ID_PANEL6;
		static const long ID_AUINOTEBOOK;
		//*)

	private:

		//(*Handlers(wxMoldeoControlsTestDialog)
		void OnButtonCloseClick(wxCommandEvent& event);
		void OnSpinCtrl1Change(wxSpinEvent& event);
		void OnButtonChangeClick(wxCommandEvent& event);
		//*)

		void OnSlider2CmdScroll(wxCommandEvent& event);
		void OnSlider3CmdScroll(wxCommandEvent& event);
		void OnSlider4CmdScroll(wxCommandEvent& event);
		void OnSlider5CmdScroll(wxCommandEvent& event);

		void OnMoLevelCtrl(wxCommandEvent& event);

    wxAuiManager FrameManager;

		DECLARE_EVENT_TABLE()
};

#endif
