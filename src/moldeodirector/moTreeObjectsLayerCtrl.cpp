
#include "moTreeObjectsLayerCtrl.h"


BEGIN_EVENT_TABLE( moTreeObjectLayerCtrl, wxPanel)
        EVT_MOUSE_EVENTS( moTreeObjectLayerCtrl::OnMouseEvent )
END_EVENT_TABLE()

void moTreeObjectLayerCtrl::Set( moMobDescriptor   p_MobDescriptor ) {

    m_MobDescriptor = p_MobDescriptor;

}

moMobDescriptor&
moTreeObjectLayerCtrl::Get() {

    return m_MobDescriptor;

}

void moTreeObjectLayerCtrl::OnMouseEvent( wxMouseEvent& event ) {

    if ( event.Button(wxMOUSE_BTN_LEFT) || event.ButtonDown() ) {

        GetParent()->ProcessEvent(event);
    }
    event.Skip();

}
//=========================================

void
moTreeObjectsLayerCtrl::AddLayer( wxTreeMultiItem id, moTreeObjectLayerCtrl* p_ObjectLayerCtrl ) {

    //add an object to the tree, must be a window two!!!



}

moTreeObjectLayerCtrl*
moTreeObjectsLayerCtrl::FindObjectByMob( moMobDescriptor p_MobDesc, TreeMultiItemNode *n ) {

    moTreeObjectLayerCtrl* result = NULL;

	// take root node if not assigned one
	if(!n)
		n = (TreeMultiItemNode *)&_root;

	// check on this level for the wxWindow pointer

	TreeMultiItemWindow *w;

	for(int i = 0; i < n->GetNodeCount(); i++)
	{
		// if window node
		w = n->GetNode(i)->IsTreeMultiItemWindow();

		if(w) {
            wxWindow* pWnd  = w->GetWindow();
            moTreeObjectLayerCtrl* pObj = (moTreeObjectLayerCtrl*) pWnd;

            //Comparamos por MoldeoId 's
            if (pObj->Get().GetMobDefinition().GetLabelName() == p_MobDesc.GetMobDefinition().GetLabelName() ) {
                return pObj;
            }

		}

		// if node, go deeper
		if(n->GetNode(i)->IsTreeMultiItemNode())
			result = FindObjectByMob( p_MobDesc, (TreeMultiItemNode*)n->GetNode(i));

        if (result)
            return result;
	}

    return result;

}


moDirectorStatus
moTreeObjectsLayerCtrl::ValueUpdated( moValueDescriptor p_ValueDesc ) {

    //recorre los mobs a ver a quien le pertenece!!! este valor y se lo pasa
    moTreeObjectLayerCtrl* pLayerCtrl = FindObjectByMob( p_ValueDesc.GetParamDescriptor().GetMobDescriptor() );

    if (pLayerCtrl) {

            moMobDescriptor p_MobDesc = pLayerCtrl->Get();

            return pLayerCtrl->ValueUpdated( p_ValueDesc );

    } else {
        LogError( moText("No layer object control founded!! ::") + (moText)p_ValueDesc.GetParamDescriptor().GetMobDescriptor().GetMobDefinition().GetLabelName() );

        return MO_DIRECTOR_STATUS_ERROR;

    }


    return MO_DIRECTOR_STATUS_OK;

}


moDirectorStatus
moTreeObjectsLayerCtrl::MobUpdated( moMobDescriptor p_MobDesc ) {

    //recorre los mobs y lo actualiza


    return MO_DIRECTOR_STATUS_OK;

}

BEGIN_EVENT_TABLE( moLayersPanelCtrl, wxSplitterWindow )

END_EVENT_TABLE()

moLayersPanelCtrl::moLayersPanelCtrl( wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name ) : wxSplitterWindow(parent, id, pos, size, style, name) {

   /* wxFlexGridSizer *topsizer = new wxFlexGridSizer( 3 );
    topsizer->SetFlexibleDirection(wxBOTH);
    topsizer->AddGrowableCol(0);
    topsizer->AddGrowableCol(2);
    topsizer->AddGrowableRow(0);


    topsizer->Add( new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(300,300)),
                1, wxALL, 10 );

    //topsizer->AddSpacer(300);

    topsizer->AddStretchSpacer(20);

    //topsizer->AddSpacer(500);

    topsizer->Add( new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(300,300)),
                1, wxALL, 10  );


    SetSizer( topsizer );      // use the sizer for layout

    topsizer->Layout();

    //topsizer->SetSizeHints( this );
*/

}
