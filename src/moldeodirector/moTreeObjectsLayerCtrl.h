/*******************************************************************************

                                moTreeObjectsLayerCtrl.h

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
  *                                                                          *
  *   This code is distributed in the hope that it will be useful, but       *
  *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
  *   General Public License for more details.                               *
  *                                                                          *
  *   A copy of the GNU General Public License is available on the World     *
  *   Wide Web at <http://www.gnu.org/copyleft/gpl.html>. You can also       *
  *   obtain it by writing to the Free Software Foundation,                  *
  *   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.         *
  *                                                                          *
  ****************************************************************************

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa


*******************************************************************************/
#ifndef _MO_TREEOBJECTSLAYER_CTRL
#define _MO_TREEOBJECTSLAYER_CTRL

#include "moDirectorTypes.h"
#include "moIDirectorActions.h"
#include "wxTreeMultiCtrl.h"

class moTreeObjectLayerCtrl : public wxPanel, public moIDirectorActions {

    public:

        moTreeObjectLayerCtrl() {}
        virtual ~moTreeObjectLayerCtrl() {}

        virtual void Set( moMobDescriptor   p_MobDescriptor );
        moMobDescriptor& Get();

    public:
        //===============================
        // INTERFACE DIRECTOR ACTIONS
        //===============================

        virtual moDirectorStatus    ValueUpdated( moValueDescriptor p_ValueDesc ) = 0;
        virtual moDirectorStatus    MobUpdated( moMobDescriptor p_MobDesc ) = 0;

    protected:
        moMobDescriptor m_MobDescriptor;

        void OnMouseEvent( wxMouseEvent& event );

        DECLARE_EVENT_TABLE()
};

class moTreeObjectsLayerCtrl : public wxTreeMultiCtrl, public moIDirectorActions {

	public:

		moTreeObjectsLayerCtrl(wxWindow *parent, wxWindowID id = -1,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxTMC_DEFAULT_STYLE,
                        const wxValidator &validator = wxDefaultValidator,
                        const wxString& name = wxTreeMultiCtrlNameStr) : wxTreeMultiCtrl(parent, id,
						pos, size, style, validator, name)
		{

        }

		MOboolean Init();
		MOboolean Finish();
		void AddLayer( wxTreeMultiItem id, moTreeObjectLayerCtrl* p_ObjectLayerCtrl );

    public:
        //===============================
        // INTERFACE DIRECTOR ACTIONS
        //===============================

        moDirectorStatus    ValueUpdated( moValueDescriptor p_ValueDesc );
        moDirectorStatus    MobUpdated( moMobDescriptor p_MobDesc );

    /*
		moObjectLayerCtrls m_pPreEffectsLayers;
		moObjectLayerCtrls m_pEffectsLayers;
		moObjectLayerCtrls m_pPostEffectsLayers;*/

    private:
        moTreeObjectLayerCtrl* FindObjectByMob( moMobDescriptor p_MobDesc, TreeMultiItemNode *n=NULL );

};


class moLayersPanelCtrl : public wxSplitterWindow {

    public:
        moLayersPanelCtrl( wxWindow *parent, wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxSP_3D,
                        const wxString& name = wxTreeMultiCtrlNameStr );
        virtual ~moLayersPanelCtrl() {
        }

    protected:

    DECLARE_EVENT_TABLE()

};


#endif
