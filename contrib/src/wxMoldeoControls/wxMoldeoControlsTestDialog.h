#ifndef WXMOLDEOCONTROLSTESTDIALOG_H
#define WXMOLDEOCONTROLSTESTDIALOG_H

//(*Headers(wxMoldeoControlsTestDialog)
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "wxMoldeoControls.h"

class wxMoldeoControlsTestDialog: public wxDialog
{
	public:

		wxMoldeoControlsTestDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~wxMoldeoControlsTestDialog();

		//(*Declarations(wxMoldeoControlsTestDialog)
		wxMoTimelineRulerCtrl* Slider6;
		wxSpinCtrl* SpinCtrl1;
		wxStaticText* StaticText2;
		wxMoRotaryCtrl* Slider3;
		wxButton* Button1;
		wxStaticText* StaticText6;
		wxMoldeoObjectCanvas* ConsoleCanvas;
		wxMoldeoObjectCtrl* Effect1;
		wxMoRotaryCtrl* Slider5;
		wxMoldeoObjectCtrl* Panel2;
		wxMoRotaryCtrl* Slider4;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxMoRotaryCtrl* Slider2;
		wxButton* Button2;
		wxMoTimelineBandCtrl* Slider7;
		wxStaticText* StaticText5;
		wxMoLevelCtrl* Slider1;
		wxMoldeoObjectCanvas* Panel1;
		wxSpinCtrl* SpinCtrl2;
		wxMoldeoObjectCtrl* Effect3;
		wxMoldeoPinCtrl* Pin2;
		wxStaticText* StaticText4;
		wxMoldeoPinCtrl* Pin1;
		wxMoldeoObjectCtrl* Effect2;
		//*)

	protected:

		//(*Identifiers(wxMoldeoControlsTestDialog)
		static const long ID_STATICTEXT1;
		static const long ID_BUTTON1;
		static const long ID_SLIDER4;
		static const long ID_SLIDER5;
		static const long ID_SLIDER3;
		static const long ID_SLIDER2;
		static const long ID_SLIDER1;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTON2;
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
		//*)

	private:

		//(*Handlers(wxMoldeoControlsTestDialog)
		void OnButton1Click(wxCommandEvent& event);
		void OnSpinCtrl1Change(wxSpinEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		//*)

		void OnSlider2CmdScroll(wxCommandEvent& event);
		void OnSlider3CmdScroll(wxCommandEvent& event);
		void OnSlider4CmdScroll(wxCommandEvent& event);
		void OnSlider5CmdScroll(wxCommandEvent& event);

		void OnMoLevelCtrl(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif
