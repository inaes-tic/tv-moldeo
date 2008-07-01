#ifndef MOEFFECTLAYERCTRL_H
#define MOEFFECTLAYERCTRL_H

#include "moDirectorTypes.h"

//(*Headers(moEffectLayerCtrl)
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/bmpbuttn.h>
#include <wx/statbmp.h>
#include <wx/combobox.h>
//*)

#include "moIDirectorActions.h"
#include "moTreeObjectsLayerCtrl.h"

#include "moWxLevelCtrl.h"


class moEffectLayerCtrl: public moTreeObjectLayerCtrl
{
	public:

		moEffectLayerCtrl(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~moEffectLayerCtrl();

		virtual void Set( moMobDescriptor p_MobDesc );

		//(*Declarations(moEffectLayerCtrl)
		wxStaticBitmap* StaticBitmapCtrls;
		wxStaticText* TextCtrlLabel;
		wxPanel* PanelColors;
		wxSlider* SliderTempo;
		wxCheckBox* CheckBoxOnOff;
		wxBitmapButton* BitmapButtonVisibility;
		wxSlider* SliderTint;
		wxSlider* SliderAlpha;
		wxComboBox* ComboBoxPreConfig;
		wxBitmapButton* BitmapButtonDelete;
		wxPanel* PanelLabelCtrls;
		//*)

		moWxLevelCtrl* m_pLevelAlphaCtrl;
		moWxLevelCtrl* m_pLevelTintCtrl;
		moWxLevelCtrl* m_pLevelSpeedCtrl;

    public:
        //===============================
        // INTERFACE DIRECTOR ACTIONS
        //===============================

        moDirectorStatus    ValueUpdated( moValueDescriptor p_ValueDesc );
        moDirectorStatus    MobUpdated( moMobDescriptor p_MobDesc );

	protected:

		//(*Identifiers(moEffectLayerCtrl)
		static const long ID_SLIDERALPHA;
		static const long ID_SLIDERTINT;
		static const long ID_SLIDERTEMPO;
		static const long ID_COMBOBOXPRE;
		static const long ID_CHECKBOXONOFF;
		static const long ID_STATICTEXTLABEL;
		static const long ID_STATICBITMAPCTRLS;
		static const long ID_PANELLABELCTRLS;
		static const long ID_STATICBITMAP1;
		static const long ID_PANELCOLORS;
		static const long ID_BITMAPBUTTONVISIBILITY;
		static const long ID_BITMAPBUTTONDELETE;
		//*)

		static const long ID_LEVELALPHA;
		static const long ID_LEVELTINT;
		static const long ID_LEVELSPEED;

	private:

		//(*Handlers(moEffectLayerCtrl)
		void OnOnOffButtonToggle(wxCommandEvent& event);
		void OnSliderAlphaCmdScroll(wxScrollEvent& event);
		void OnSliderTempoCmdScroll(wxScrollEvent& event);
		void OnCheckBoxOnOffClick(wxCommandEvent& event);
		void OnSliderTintCmdScrollThumbTrack(wxScrollEvent& event);
		void OnComboBoxPreConfigSelect(wxCommandEvent& event);
		void OnPanelVisibilityMouseMove(wxMouseEvent& event);
		void OnBitmapButtonVisibilityClick(wxCommandEvent& event);
		void OnBitmapButtonDeleteClick(wxCommandEvent& event);
		//*)

		void OnLevelAlpha(wxCommandEvent& event);
		void OnLevelTint(wxCommandEvent& event);
		void OnLevelSpeed(wxCommandEvent& event);

        void OnMouseEvent( wxMouseEvent& event );

		DECLARE_EVENT_TABLE()
};


class moEffectLayerTimelineCtrl : public moTreeObjectLayerCtrl {

    public:
		moEffectLayerTimelineCtrl(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~moEffectLayerTimelineCtrl();

   public:
        //===============================
        // INTERFACE DIRECTOR ACTIONS
        //===============================

        moDirectorStatus    ValueUpdated( moValueDescriptor p_ValueDesc );
        moDirectorStatus    MobUpdated( moMobDescriptor p_MobDesc );

    protected:

        wxImage    m_BackgroundImage;
        moText     m_BackgroundName;


        void OnPaint( wxPaintEvent& event );
        void OnMouseEvent( wxMouseEvent& event );


		DECLARE_EVENT_TABLE()
};

template class moDynamicArray<moEffectLayerCtrl*>;
typedef  moDynamicArray<moEffectLayerCtrl*> moEffectLayerCtrls;

#endif
