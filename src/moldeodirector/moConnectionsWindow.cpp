#include "moConnectionsWindow.h"

//(*InternalHeaders(moConnectionsWindow)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/msgdlg.h>
#include <wx/dcclient.h>


#ifdef MO_WXART2D

#include "moArray.cpp"

IMPLEMENT_DYNAMIC_CLASS(moPin, a2dPin)
IMPLEMENT_DYNAMIC_CLASS(moMoldeoCanvasObject, a2dRect)


moDefineDynamicArray( moMoldeoCanvasObjects )
moDefineDynamicArray( moPinOutlets )
moDefineDynamicArray( moPinInlets )

a2dPinClass* moPin::ElementInlet = NULL;
a2dPinClass* moPin::ElementOutlet = NULL;
a2dPinClass* moPin::ElementWire = NULL;

A2D_BEGIN_EVENT_TABLE( a2dConnector, a2dWindowViewConnector )
    A2D_EVT_POST_CREATE_VIEW( a2dConnector::OnPostCreateView )
    A2D_EVT_POST_CREATE_DOCUMENT( a2dConnector::OnPostCreateDocument )
    A2D_EVT_DISCONNECT_VIEW( a2dConnector::OnDisConnectView )
A2D_END_EVENT_TABLE()

a2dConnector::a2dConnector()
        :a2dWindowViewConnector()
{
}

void a2dConnector::OnPostCreateView( a2dTemplateEvent& event )
{
    assert(0);
}

void a2dConnector::OnPostCreateDocument( a2dTemplateEvent& event )
{
    //a2dDocumentTemplate* templ = (a2dDocumentTemplate*) event.GetEventObject();
    if ( !m_display )
       return;

    a2dView* view = ((a2dCanvas*) m_display)->GetCanvasView();
    event.SetView( view );

    if ( view )
    {
        view->SetDocument(event.GetDocument());
        view->Activate( true );
        return;
    }
    wxLogMessage(_("a2dConnector no view was created") );
}

void a2dConnector::OnDisConnectView(  a2dTemplateEvent& event )
{
	a2dSmrtPtr<a2dCanvasView> theViewToDisconnect = (a2dCanvasView*) event.GetEventObject();

	theViewToDisconnect->Activate( false );
	if ( theViewToDisconnect->GetDocument() )
	{
		a2dDocumentEvent eventremove( wxEVT_REMOVE_VIEW, theViewToDisconnect, true, theViewToDisconnect->GetId() );
		eventremove.SetEventObject(theViewToDisconnect);
		theViewToDisconnect->GetDocument()->ProcessEvent( eventremove );
		a2dDocviewGlobals->GetEventDistributer()->ProcessEvent( eventremove );
	}
    if ( theViewToDisconnect->GetCanvasToolContr() )
        theViewToDisconnect->GetCanvasToolContr()->StopAllTools();

    if ( !a2dDocviewGlobals->GetDocviewCommandProcessor()->GetBusyExit() )
    {
        if ( a2dDocviewGlobals->GetDocviewCommandProcessor()->GetDocuments().size() > 1 )
        {
	        a2dDocumentList::const_reverse_iterator iter = a2dDocviewGlobals->GetDocviewCommandProcessor()->GetDocuments().rbegin();
            iter++;
    	    theViewToDisconnect->SetDocument( (*iter) );
        }
        else
        {
			a2dDocviewGlobals->GetDocviewCommandProcessor()->CreateDocuments(wxT("dummy"), a2dREFDOC_NEW );
            a2dCanvasDocument* document = (a2dCanvasDocument* ) a2dDocviewGlobals->GetDocviewCommandProcessor()->GetDocuments().back().Get();
	        theViewToDisconnect->SetDocument( document );
        }

	    if ( theViewToDisconnect->GetCanvasToolContr() )
		    theViewToDisconnect->GetCanvasToolContr()->ReStart();

        theViewToDisconnect->SetMappingShowAll();

	    theViewToDisconnect->Activate( true );
    }
    else
        event.Skip();
}


void a2dConnector::SetDisplayWindow( a2dDocumentViewWindow* display )
{
    a2dWindowViewConnector::SetDisplayWindow( display );
    a2dView* view = ((a2dCanvas*) m_display)->GetCanvasView();
    if ( view )
    {
		a2dViewTemplate* viewtempl;
		viewtempl = a2dDocviewGlobals->GetDocviewCommandProcessor()->GetViewTemplates().front();
        view->SetViewTemplate( viewtempl );
        view->Activate( true );
        return;
    }
}

moPin::moPin()
    : a2dPin()
{
}

moPin::moPin( a2dCanvasObject* parent, const wxString& name, a2dPinClass* pinclass,  double xc, double yc, double angle, int w, int h,  int radius )
    : a2dPin( parent, name, pinclass, xc, yc, angle, w, h, radius )
{
}

moPin::moPin( const a2dPin &other, CloneOptions options )
    : a2dPin( other, options )
{
}

moPin::~moPin()
{
}

void moPin::InitializeExtraClasses()
{
    ElementInlet = new a2dPinClass( wxT("eleInlet"), a2dPinClass::PC_input | a2dPinClass::PC_output );
    ElementOutlet = new a2dPinClass( wxT("eleOutlet"), a2dPinClass::PC_output | a2dPinClass::PC_input);

    ElementWire = new a2dPinClass( wxT("eleWire"), a2dPinClass::PC_input | a2dPinClass::PC_output  );

    a2dPinClass::m_allPinClasses.push_back( ElementInlet );
    a2dPinClass::m_allPinClasses.push_back( ElementOutlet );

    a2dPinClass::m_allPinClasses.push_back( ElementWire );
}

a2dObject* moPin::Clone( CloneOptions options ) const
{
    return new moPin( *this, options );
};

void moPin::DoRender( a2dIterC& ic, OVERLAP clipparent )
{
    //a2dPin::DoRender( ic, clipparent );
    //return;
    double x1, y1;
    ic.GetTransform().TransformPoint( 0.0, 0.0, x1, y1 );
    int dx = (int) ic.GetDrawer2D()->WorldToDeviceX(x1);
    int dy = (int) ic.GetDrawer2D()->WorldToDeviceY(y1);

    // Calculate handle size.
    double w2 = m_width/2 ;
    double h2 = m_height/2;

    if ( m_flags.m_MouseInObject )
    {
        if ( !m_canvaspin ) //connected ?
        {
            if ( m_parent->FindProperty( a2dCanvasObject::PROPID_ToolObject ) )
                ic.GetDrawer2D()->OverRuleFixedStyle( true );

            if ( m_mode == sm_PinUnConnected )
            {
                // This is the usual mode of an unconnecting pin just sitting there
                ic.GetDrawer2D()->SetDrawerStroke( a2dStroke( *wxRED, 0)  );
                ic.GetDrawer2D()->DeviceDrawLine( (int) (dx - w2), (int) (dy + h2),  (int) (dx + w2), (int) (dy - h2));
                ic.GetDrawer2D()->DeviceDrawLine( (int) (dx - w2), (int) (dy - h2),  (int) (dx + w2), (int) (dy + h2));
            }

            else if (  m_mode == sm_PinCanConnectToPinClass )
            {
                // This is the mode of a pin that is hit by the mouse (set in OnEnter)
                ic.GetDrawer2D()->SetDrawerStroke( a2dStroke( wxColour(12,34,78), 0.5) );
                ic.GetDrawer2D()->DeviceDrawCircle(dx, dy, m_width/4);
            }
            else if ( m_mode == sm_PinCanConnect )
            {
                // This is the mode of a pin that can be connected
                int w = m_pinclass->GetPinCanConnect()->GetWidth();
                int h = m_pinclass->GetPinCanConnect()->GetHeight();
                int r = m_pinclass->GetPinCanConnect()->GetRadius();
                ic.GetDrawer2D()->SetDrawerFill( a2dFill( wxColour(12,134,228)) );
                ic.GetDrawer2D()->SetDrawerStroke( a2dStroke( *wxBLUE, 1) );
                ic.GetDrawer2D()->DeviceDrawCenterRoundedRectangle(dx, dy, w, h, r);
            }
            else if (  m_mode == sm_PinCannotConnect )
            {
                // This is the mode of a pin that cannot be connected
                int w = m_pinclass->GetPinCannotConnect()->GetWidth();
                int h = m_pinclass->GetPinCannotConnect()->GetHeight();
                int r = m_pinclass->GetPinCanConnect()->GetRadius();
                ic.GetDrawer2D()->SetDrawerFill( a2dFill( wxColour(245,134,178)) );
                ic.GetDrawer2D()->SetDrawerStroke( a2dStroke( wxColour(12,34,178), 1) );
                ic.GetDrawer2D()->DeviceDrawCenterRoundedRectangle(dx, dy, (int) (w*1.5), (int) (h*1.5), r);
            }

            if ( m_parent->FindProperty( a2dCanvasObject::PROPID_ToolObject ) )
                ic.GetDrawer2D()->OverRuleFixedStyle( false );
        }
        else
        {
            // check if the pin is dislocated
            // This shouldn't happen, but show it, if it does happen
            if( IsDislocated() || m_RenderConnected )
            {
                ic.GetDrawer2D()->SetDrawerFill( *a2dTRANSPARENT_FILL );
                ic.GetDrawer2D()->DeviceDrawLine( (int)(dx - w2), (int)(dy + h2),  (int)(dx + w2), (int)(dy - h2));
                ic.GetDrawer2D()->DeviceDrawLine( (int)(dx - w2), (int)(dy - h2),  (int)(dx + w2), (int)(dy + h2));
            }
        }
    }
    else
    {
        if ( !m_canvaspin ) //connected ?
        {
            // This is the usual mode of an unconnecting pin just sitting there
            ic.GetDrawer2D()->SetDrawerFill( m_pinclass->GetPin()->GetFill() );
            ic.GetDrawer2D()->SetDrawerStroke( m_pinclass->GetPin()->GetStroke() );
            ic.GetDrawer2D()->DeviceDrawCircle( (int)dx, (int)dy, (int) w2 );
        }
    }
}

a2dBoundingBox moPin::DoGetUnTransformedBbox( a2dBboxFlags flags ) const
{
    return a2dPin::DoGetUnTransformedBbox();
}




A2D_BEGIN_EVENT_TABLE( moMoldeoCanvasObject, a2dRect )
    //A2D_EVT_CANVASOBJECT_ENTER_EVENT( moMoldeoCanvasObject::OnEnterObject )
    //A2D_EVT_CANVASOBJECT_MOUSE_EVENT( moMoldeoCanvasObject::OnCanvasObjectMouseEvent )
    //A2D_EVT_CANVASOBJECT_LEAVE_EVENT( moMoldeoCanvasObject::OnLeaveObject )
    //A2D_EVT_CANVASHANDLE_MOUSE_EVENT( moMoldeoCanvasObject::OnHandleEvent )
A2D_END_EVENT_TABLE()

moMoldeoCanvasObject::moMoldeoCanvasObject() {
    m_pMoldeoCanvasObjectTitle = NULL;
}

moMoldeoCanvasObject::moMoldeoCanvasObject( double xc, double yc, double w, double h, double angle, double radius, moMobDescriptor p_MobDescriptor, moIDirectorActions* parenthandler ) : a2dRect( xc, yc, w, h, radius ) {

    SetVisible(true);
    SetEditable(true);
    SetSelectable(true);
    SetDraggable(true);
    //SetShowShadow(true);
    //SetEvtHandlerEnabled(true);
    SetNextActionHandler(parenthandler);

    SetFill( a2dFill( wxColour(220,220,220),a2dFILL_SOLID) );
    SetStroke( a2dStroke( wxColour( 0,0,0), 2, a2dSTROKE_SOLID ) );

    /// MOB NAME
    m_pMoldeoCanvasObjectTitle = new a2dText ( moText2Wx( p_MobDescriptor.GetMobDefinition().GetLabelName() ), 4, 10 );
    m_pMoldeoCanvasObjectTitle->SetFont( a2dFont( a2dGlobals->GetFontPathList().FindValidPath( wxT("/arial.ttf") ), 10 ) );
    m_pMoldeoCanvasObjectTitle->SetFill( a2dFill( wxColour(0,0,0),a2dFILL_SOLID) );
    m_pMoldeoCanvasObjectTitle->SetStroke( a2dStroke( wxColour( 0,0,0), 2, a2dSTROKE_SOLID ) );
    m_pMoldeoCanvasObjectTitle->SetAlignment( wxTOP | wxLEFT );
    Append( m_pMoldeoCanvasObjectTitle );


    a2dSLine*   pLine = new a2dSLine(  0-3, 14 , w+3, 14, 0 );
    pLine->SetStroke( a2dStroke( wxColour( 0,0,0), 2, a2dSTROKE_SOLID ) );
    Append(pLine);


    ShowPins(true);

    this->SetMob( p_MobDescriptor );
  //  SetHitFlags( a2dCANOBJ_EVENT_VISIBLE  );
    //a2dHandle* phandle = new a2dHandle( this, 0, 0, wxT("handle ob"), w, h, 0, 0 );
    //phandle->SetEvtHandlerEnabled(true);
    //SetHandlePos(moText2Wx( p_MobDescriptor.GetMobDefinition().GetLabelName() ), 0, 0);
    //Append( phandle );
    //phandle->SetEvt


}


ElementConnectionGenerator::ElementConnectionGenerator()
{

}

ElementConnectionGenerator::~ElementConnectionGenerator()
{
}

a2dPinClass* ElementConnectionGenerator::GetPinClassForTask( a2dPinClass* pinClass, a2dConnectTask task, a2dCanvasObject* obj ) const
{

    if (pinClass==NULL) wxMessageBox("pinClass cant be null2");

    if ( pinClass == a2dPinClass::Any )
    {
        if ( wxDynamicCast( obj, moMoldeoCanvasObject ) )
        {
            //return moPin::ElementObject;
            ///no nos conectamos al objeto...
            return NULL;
        }
        else if ( obj->IsConnect() )
        {
            /*
            if ( wxDynamicCast( obj, a2dWirePolylineL )->GetStartPinClass() ==  moPin::ElementWire )
                return moPin::ElementWire;
                */
            if ( wxDynamicCast( obj, a2dWirePolylineL )->GetStartPinClass() ==  a2dPinClass::WireInput )
                return a2dPinClass::WireOutput;
            if ( wxDynamicCast( obj, a2dWirePolylineL )->GetStartPinClass() ==  a2dPinClass::WireOutput )
                return a2dPinClass::WireInput;
            else
                return a2dPinClass::Wire;
        }
        else
        {
            return a2dPinClass::Object;
        }
    }

    if ( pinClass == moPin::ElementInlet )
    {
        if ( obj->IsConnect() )
            return moPin::ElementOutlet;
        else
            return moPin::ElementOutlet;
    }


    if ( pinClass == moPin::ElementOutlet )
    {
        if ( obj->IsConnect() )
            return moPin::ElementInlet;
        else
            return moPin::ElementInlet;
    }

/*
    if ( pinClass == moPin::ElementWire )
    {
        if ( obj->IsConnect() )
            return moPin::ElementWire;
        else
            return moPin::ElementObject;
    }
*/


    if ( pinClass == a2dPinClass::Standard )
        return a2dPinClass::Standard->CanConnectTo();

    if ( pinClass == a2dPinClass::Object )
    {
        if ( obj->IsConnect() )
            return a2dPinClass::Wire;
        else
            return a2dPinClass::Object;
    }



    if ( pinClass == a2dPinClass::Wire )
    {
        if ( obj->IsConnect() )
            return a2dPinClass::Wire;
        else
            return a2dPinClass::Object;
    }


    if ( pinClass == a2dPinClass::Input )
        return a2dPinClass::Output;
    if ( pinClass == a2dPinClass::Output )
        return a2dPinClass::Input;

    if ( pinClass == a2dPinClass::ObjectInput )
    {
        if ( obj->IsConnect() )
            return a2dPinClass::WireOutput;
        else
            return a2dPinClass::ObjectOutput;
    }

    if ( pinClass == a2dPinClass::ObjectOutput )
    {
        if ( obj->IsConnect() )
            return a2dPinClass::WireInput;
        else
            return a2dPinClass::ObjectInput;
    }

    return NULL;
}


moMoldeoCanvasObject::~moMoldeoCanvasObject() {

    //delete m_pMoldeoCanvasObjectTitle;


}

bool moMoldeoCanvasObject::ProcessCanvasObjectEvent( a2dIterC &ic, a2dHitEvent &hitEvent ) {

    a2dHitEvent eee = hitEvent;

    bool test = a2dCanvasObject::ProcessCanvasObjectEvent( ic, hitEvent );

    return test;
}

void moMoldeoCanvasObject::OnEnterObject(a2dCanvasObjectMouseEvent &event) {

    SetStroke( a2dStroke( wxColour( 0,0,255), 3, a2dSTROKE_SOLID ) );
    //a2dCanvasObject::OnEnterObject( event );
    event.Skip();

}

void moMoldeoCanvasObject::OnCanvasObjectMouseEvent(a2dCanvasObjectMouseEvent &event) {

    //SetStroke( a2dStroke( wxColour( 0,0,0), 2, a2dSTROKE_SOLID ) );
    wxMouseEvent& mevent( event.GetMouseEvent() );
    if (mevent.LeftDown()) {
        //select it
        wxMessageBox("left down clicked");
        //SetSelected(true);
    }

    //a2dCanvasObject::OnCanvasObjectMouseEvent(event);
    event.Skip();

}

void moMoldeoCanvasObject::OnLeaveObject(a2dCanvasObjectMouseEvent &event) {

    if (IsSelected()) {
        SetStroke( a2dStroke( wxColour( 0,255,0), 2, a2dSTROKE_SOLID ) );
    } else {
        SetStroke( a2dStroke( wxColour( 0,0,0), 3, a2dSTROKE_SOLID ) );
    }
    //a2dCanvasObject::OnLeaveObject( event );
    event.Skip();

}

void
moMoldeoCanvasObject::OnLeftDown( wxMouseEvent& event ) {

        wxMessageBox("left down");
        event.Skip();

}

void
moMoldeoCanvasObject::OnHandleEvent(a2dHandleMouseEvent &event) {


    wxMouseEvent &mevent( event.GetMouseEvent() );
    if (mevent.LeftIsDown()) {
        //start wiring while dragging


    }
    event.Skip();

}

void
moMoldeoCanvasObject::Render (a2dIterC &ic, OVERLAP clipparent) {

    a2dDrawer2D* pDC2D = ic.GetDrawer2D();

    a2dRect::Render( ic, clipparent );

    pDC2D->BeginDraw();
    //pDC2D->DrawText( wxT("prueba"), 0, 0 );
/*    pDC2D->SetTransform( a2dIDENTITY_MATRIX );

    pDC2D->SetDrawerStroke( *a2dTRANSPARENT_STROKE );
    pDC2D->SetDrawerFill( *a2dBLACK_FILL );
    pDC2D->DeviceDrawRectangle( this->GetPosX(), this->GetPosY(), this->GetWidth(), this->GetHeight() );

    //pDC2D->SetDrawerStroke( a2dStroke( *wxRED, 4.0 ) );
    //pDC2D->SetDrawerFill( a2dFill( *wxBLUE ) );

    //pDC2D->DrawCircle( this->GetX(), this->GetY(), 25.0 );
*/
    pDC2D->EndDraw();



}

moDirectorStatus
moMoldeoCanvasObject::SetMob( moMobDescriptor p_MobDescriptor ) {

        m_MobDescriptor = p_MobDescriptor;

        if (m_pMoldeoCanvasObjectTitle) {
                m_pMoldeoCanvasObjectTitle->SetText( moText2Wx( m_MobDescriptor.GetMobDefinition().GetLabelName() ) );
        }

        /// CREATE PINS (outlets and inlets)
        moParameterDescriptors pParamDescriptors = GetParameterDescriptors( m_MobDescriptor );
        moValueDescriptors pValDescriptors;
        moValueDescriptor pVD;

        //delete childs
/*
        a2dPinClass*    m_pPinInlet = new a2dPinClass( "inlet 0", 0x0001 );
        a2dPinClass*    m_pPinOutlet = new a2dPinClass( "outlet 0", 0x0002 );

        m_pPinInlet->AddConnect( m_pPinOutlet );
        m_pPinOutlet->AddConnect( m_pPinInlet );
*/
        a2dText*  m_pPinText;
        a2dPin*  m_pMoldeoObjectPin;

        wxString pvdname;
        wxString pvdtypestr;
        double xpos;
        double ypos;

        for(MOuint i=0; i<pParamDescriptors.Count(); i++) {
            moParameterDescriptor pP = pParamDescriptors[i];
            switch((int)pP.GetParamDefinition().GetType()) {
                case MO_PARAM_INLET:
                    pValDescriptors = GetValueDescriptors(pP);
                    for( MOuint j=0; j<pValDescriptors.Count(); j++) {

                        pVD = pValDescriptors[j];
                        pVD = GetValue(pVD);
                        pvdname = moText2Wx( pVD.GetValue().GetSubValue(0).Text());
                        pvdtypestr = moText2Wx( pVD.GetValue().GetSubValue(1).Text());

                        xpos = 5;
                        ypos = j*11+24;

                        m_pPinText = new a2dText ( pvdname, xpos+10, ypos );
                        m_pPinText->SetFont(a2dFont( a2dGlobals->GetFontPathList().FindValidPath( wxT("/arial.ttf") ), 5 ));
                        m_pPinText->SetStroke( a2dStroke( wxColour( 50,50,50), 2, a2dSTROKE_SOLID ) );

                        m_pPinText->SetAlignment( wxLEFT | wxCENTER );
                        //m_pMoldeoObjectPin = this->AddPin( pvdname, xpos, ypos, a2dPin::objectPin, m_pPinOutlet );
                        moPin* pin = new moPin(this, pvdname, moPin::ElementInlet, xpos, ypos );
                        Append( pin );
                        Append(m_pPinText);

                    }
                    this->SetHeight( max( GetHeight(), (double)ypos+4) );

                    break;

                case MO_PARAM_OUTLET:
                    pValDescriptors = GetValueDescriptors(pP);
                    for( MOuint j=0; j<pValDescriptors.Count(); j++) {

                        pVD = pValDescriptors[j];
                        pVD = GetValue(pVD);
                        pvdname = moText2Wx( pVD.GetValue().GetSubValue(0).Text());
                        pvdtypestr = moText2Wx( pVD.GetValue().GetSubValue(1).Text());

                        xpos = GetWidth() - 5;
                        ypos = j*11+24;

                        m_pPinText = new a2dText ( pvdname, xpos-10, ypos );
                        m_pPinText->SetFont(a2dFont( a2dGlobals->GetFontPathList().FindValidPath( wxT("/arial.ttf") ), 5 ));
                        m_pPinText->SetStroke( a2dStroke( wxColour( 50,50,50), 2, a2dSTROKE_SOLID ) );
                        m_pPinText->SetAlignment( wxRIGHT | wxCENTER );
                        //m_pMoldeoObjectPin = this->AddPin( pvdname, xpos, ypos, a2dPin::objectPin, m_pPinOutlet );
                        moPin* pin = new moPin(this, pvdname, moPin::ElementOutlet, xpos, ypos );
                        Append( pin );
                        Append(m_pPinText);

                    }
                    this->SetHeight( max( GetHeight(), (double)ypos+4) );

                    break;
            }
        }

        /*


        m_pPinInlet->AddConnect(m_pPinOutlet);
        m_pPinOutlet->AddConnect(m_pPinInlet);

        a2dText*  m_pMoldeoObjectPin1Text = new a2dText ( "enable", 100-15, 70 );
        m_pMoldeoObjectPin1Text->SetFont( a2dFont( 10, wxDEFAULT ) );
        m_pMoldeoObjectPin1Text->SetAlignment( wxRIGHT | wxCENTER );
        a2dPin*  m_pMoldeoObjectPin1 = this->AddPin( "enable", 100-10, 70, a2dPin::objectPin, m_pPinOutlet );

        a2dText*  m_pMoldeoObjectPin2Text = new a2dText ( "alpha", 100-15, 50 );
        m_pMoldeoObjectPin2Text->SetFont( a2dFont( 10, wxDEFAULT ) );
        m_pMoldeoObjectPin2Text->SetAlignment( wxRIGHT | wxCENTER );
        a2dPin*  m_pMoldeoObjectPin2 = this->AddPin( "alpha", 100-10, 50, a2dPin::objectPin, m_pPinOutlet );

        a2dText*  m_pMoldeoObjectPin3Text = new a2dText ( "phase", 100-15, 30 );
        m_pMoldeoObjectPin3Text->SetFont( a2dFont( 10, wxDEFAULT ) );
        m_pMoldeoObjectPin3Text->SetAlignment( wxRIGHT | wxCENTER );
        a2dPin*  m_pMoldeoObjectPin3 = this->AddPin( "phase", 100-10, 30, a2dPin::objectPin, m_pPinOutlet );

        this->Append( m_pMoldeoObjectPin1Text );
        this->Append( m_pMoldeoObjectPin2Text );
        this->Append( m_pMoldeoObjectPin3Text );
        */

        //a2dPin*  m_pMoldeoObjectPin1 = new a2dPin ( m_pMoldeoCanvasObject, "enable", m_pPinOutlet, /*x*/20, /*y*/0, /*angle*/0,/*w*/10, /*h*/10, 0 );
        //a2dPin*  m_pMoldeoObjectPin2 = new a2dPin ( m_pMoldeoCanvasObject, "alpha", m_pPinOutlet, /*x*/20, /*y*/10, /*angle*/0,/*w*/10, /*h*/10, 0 );

}

moMobDescriptor&
moMoldeoCanvasObject::GetMob() {

    return m_MobDescriptor;

}

A2D_BEGIN_EVENT_TABLE( moConnectionsToolContr, a2dStToolContr )

    A2D_EVT_MOUSE_EVENTS(moConnectionsToolContr::OnMouseEvent)
    A2D_EVT_KEY_DOWN(moConnectionsToolContr::OnKeyDown)
    A2D_EVT_KEY_UP(moConnectionsToolContr::OnKeyUp)

A2D_END_EVENT_TABLE()


moConnectionsToolContr::moConnectionsToolContr( a2dCanvasView* view, wxFrame* where) : a2dStToolContr( view, where ) {

    ///deleting default menu
    /*
    if(m_mousemenu) {
        delete m_mousemenu;
        m_mousemenu = new wxMenu();
    }*/

    ///delete any pushed tool
    /*
    a2dSmrtPtr< a2dBaseTool > tool;
    a2dToolContr::PopTool( tool );
    while ( tool )
    {
    tool = NULL;
    a2dToolContr::PopTool( tool );
    }
    */

    m_pDragTool = NULL;
    m_pSelectTool = NULL;

    SetZoomFirst(false);

    this->SetTopTool( new a2dGraphicsMasterTool( this ) );
    this->SetSnap( true );

}


moConnectionsToolContr::~moConnectionsToolContr() {


}

void moConnectionsToolContr::OnKeyDown(wxKeyEvent& event)
{
	//wxLogDebug(wxT("key %d"), event.GetKeyCode());
/*
    if( m_defaultBehavior & wxTC_NoDefaultKeys )
    {
       event.Skip();
       return;
    }

    switch(event.GetKeyCode())
    {


        case WXK_CONTROL:
        {

            if  ( !GetFirstTool() || (GetFirstTool() && !GetFirstTool()->GetBusy()) )
            {
                m_pSelectTool = wxDynamicCast(GetFirstTool(), a2dSelectTool);
                if (!m_pSelectTool)
                {
                    //wxMessageBox("select on");
                    m_pSelectTool = new a2dSelectTool(this);
                    PushTool(m_pSelectTool);
                    m_pSelectTool->SetOneShot();
                }
            }

        }
        break;

        case WXK_ESCAPE:
        {
           StopAllTools();

            if ( GetFirstTool() && GetFirstTool()->AllowPop() )
            {
                a2dSmrtPtr<a2dBaseTool> poped;
                PopTool( poped );
            }

            ShowMouse();

        }
        break;
        case 'Z':
        case 'z':
        {
            if ( event.m_controlDown && event.m_shiftDown )
                m_view->GetCanvasDocument()->GetCommandProcessor()->Redo();
            else if ( event.m_controlDown )
                m_view->GetCanvasDocument()->GetCommandProcessor()->Undo();
            else if  ( !GetFirstTool() || (GetFirstTool() && !GetFirstTool()->GetBusy()) )
            {
                a2dBaseTool* tool = m_toolForZoom->TClone( clone_deep );
                PushTool( tool );
            }
            break;
        }
        case 'Y':
        case 'y':
        {
            if ( event.m_controlDown )
                m_view->GetCanvasDocument()->GetCommandProcessor()->Redo();
            break;
        }
        case WXK_NUMPAD_ADD:
        {
            if ( event.m_controlDown )
                ZoomIn2AtMouse();
            else
                Zoomin2();
            break;
        }
        case WXK_NUMPAD_SUBTRACT:
        {
            if ( event.m_controlDown )
                ZoomOut2AtMouse();
            else
                Zoomout2();
            break;
        }
        default:
            event.Skip();
    }*/
    event.Skip();
}

void moConnectionsToolContr::OnKeyUp(wxKeyEvent& event)
{
    /*
    if( m_defaultBehavior & wxTC_NoDefaultKeys )
    {
       event.Skip();
       return;
    }

    if  ( GetFirstTool() && !GetFirstTool()->GetBusy())
    {
        switch(event.GetKeyCode())
        {
            case WXK_F1:
                break;


           case WXK_CONTROL:
            {
                StopAllTools();


                a2dSelectTool *c = wxDynamicCast(GetFirstTool(), a2dSelectTool);
                if (c)
                {
                    a2dSmrtPtr<a2dBaseTool> poped;
                    PopTool( poped );
                }


                break;
            }

            default:
                event.Skip();
        }
    }
    else
        event.Skip();
*/
    event.Skip();
}

void moConnectionsToolContr::OnMouseEvent(wxMouseEvent& event)
{

    /*
    wxPoint pos = event.GetPosition();

    m_mouse_x = pos.x;
    m_mouse_y = pos.y;

    ShowMouse();

    if ( event.LeftDown() && wxWindow::FindFocus() != m_view->GetDisplayWindow() )
	{

		// normally handler in a2dCanvasView::OnActive()
		m_view->GetDisplayWindow()->SetFocus();

	}*/
/*
	if ( event.ButtonDClick() ) {

        a2dSelectTool* select= new a2dSelectTool(this);

        PushTool(select);
        select->SetOneShot();

	}
*/
/*
    if ( event.RightDown())
    {
		//we first try the tools here, since it can have a popup menu too.
		//If not processed, and not processed in here either, then it will once more
		//be processed by the tools. This fact is not a problem.
		if ( ToolsProcessEvent(event) )
			return;

        if ( ! (m_defaultBehavior & wxTC_NoContextMenu) && m_mousemenu )
		{
            m_view->GetDisplayWindow()->PopupMenu(m_mousemenu,pos.x,pos.y);
            event.Skip(false); //event is processed.
			return;
		}
    }
*/
    /*
    else if  ( GetFirstTool() && !GetFirstTool()->GetBusy()
             && ! (m_defaultBehavior & wxTC_NoDefaultMouseActions) )
    {

        if ( !wxDynamicCast(GetFirstTool(), a2dZoomTool) && event.m_controlDown && event.m_shiftDown )
        {
            a2dBaseTool* tool = m_toolForZoom->TClone( clone_deep );
            tool->SetOneShot();
            PushTool( tool );
        }
        if (!wxDynamicCast(GetFirstTool(), a2dDragTool) && event.m_controlDown)
        {

            a2dDragTool* drag = new a2dDragTool(this);
            PushTool(drag);
            drag->SetOneShot();
        }
        else if ( !wxDynamicCast(GetFirstTool(), a2dSelectTool) && event.m_shiftDown)
        {
            a2dSelectTool* select= new a2dSelectTool(this);
            PushTool(select);
            select->SetOneShot();
        }

    }
    */
/*
    if (event.LeftIsDown() && !event.m_shiftDown && !event.m_controlDown ) {
        ///if no tools are working we set the drag...
             if  ( !GetFirstTool() || (GetFirstTool() && !GetFirstTool()->GetBusy()) )
            {
                a2dDragTool *c = wxDynamicCast(GetFirstTool(), a2dDragTool);
                if (!c)
                {
                    m_pDragTool = new a2dDragTool(this);
                    PushTool(m_pDragTool);
                    m_pDragTool->SetOneShot();
                    //wxMessageBox("Drag tool ok");
                }
            }
    } else if ( event.LeftUp()  && !event.m_shiftDown && !event.m_controlDown ) {
        if (GetFirstTool() && !GetFirstTool()->GetBusy()) {
            a2dDragTool *c = wxDynamicCast(GetFirstTool(), a2dDragTool);
            if (c)
            {
                a2dSmrtPtr<a2dBaseTool> poped;
                PopTool( poped );
            }
        }
    }
*/
    //let the baseclass a2dToolContr
    //handle the event, in order to redirect it to the tools
    event.Skip();
}

#endif

//(*IdInit(moConnectionsWindow)
//*)

BEGIN_EVENT_TABLE(moConnectionsWindow,wxPanel)
	//(*EventTable(moConnectionsWindow)
	//*)
	EVT_SIZE(moConnectionsWindow::OnSize)
	//EVT_MOUSE_EVENTS(moConnectionsWindow::OnMouseEvents)
END_EVENT_TABLE()

moConnectionsWindow::moConnectionsWindow(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size) : m_pCanvas(NULL)
{
	//(*Initialize(moConnectionsWindow)
	Create(parent, id, wxDefaultPosition, wxSize(200,100), wxTAB_TRAVERSAL | wxNO_FULL_REPAINT_ON_RESIZE, _T("id"));
	SetBackgroundColour(wxColour(255,0,0));

	Connect(wxID_ANY,wxEVT_PAINT,(wxObjectEventFunction)&moConnectionsWindow::OnPaint);
	Connect(wxID_ANY,wxEVT_ERASE_BACKGROUND,(wxObjectEventFunction)&moConnectionsWindow::OnEraseBackground);
	//*)

    SetForegroundColour(wxColour(0,0,0));

    #ifdef MO_WXART2D
        a2dDocviewGlobals->GetEventDistributer()->Register( this );
        a2dRestrictionEngine *restrict=new a2dRestrictionEngine();

        restrict->SetSnapToFeatures( a2dRestrictionEngine::snapToGridPos
        |a2dRestrictionEngine::snapToObjectPos|a2dRestrictionEngine::snapToPinsUnconnected
        |a2dRestrictionEngine::snapToObjectVertexes );
        restrict->SetSnapGrid( 2, 2 );
        restrict->SetRotationAngle(45);

        a2dCanvasGlobals->SetSelectStroke( a2dStroke(wxColour(255,55,0), 2, a2dSTROKE_SOLID ) );
        a2dCanvasGlobals->SetSelectFill( *a2dTRANSPARENT_FILL );
        a2dCanvasGlobals->SetSelectDrawStyle( RenderWIREFRAME_SELECT );
        a2dIOHandlerCVGIn* cvghin = new a2dIOHandlerCVGIn();
        a2dIOHandlerCVGOut* cvghout = new a2dIOHandlerCVGOut();

        a2dCentralCanvasCommandProcessor* docManager = new a2dCentralEditorCommandProcessor();
        a2dDocviewGlobals->SetDocviewCommandProcessor( docManager );

        //connector to couple the single and only canvas + view to new documents.
        a2dSmrtPtr<a2dConnector> singleconnector = new a2dConnector();

        a2dDocumentTemplate* doctemplatenew;

        // Create a template relating drawing documents to their files
        doctemplatenew = new a2dDocumentTemplate( wxT("CVG Drawing"), wxT("*.cvg"), wxT(""), wxT("cvg"), wxT("a2dCanvasDocument"),
            CLASSINFO(a2dCanvasDocument), singleconnector, a2dTemplateFlag::DEFAULT, cvghin,cvghout );
        a2dDocviewGlobals->GetDocviewCommandProcessor()->AssociateDocTemplate(doctemplatenew);

        //THE DOCUMENT where objets are located
        a2dGlobals->GetFontPathList().Add( wxT("../../doc/fonts") );
        a2dGlobals->GetImagePathList().Add( wxT("../../doc/images") );
        a2dGlobals->GetImagePathList().Add( wxT("./") );
        a2dGlobals->GetImagePathList().Add( wxT("../") );
        a2dGlobals->GetFontPathList().Add( wxT(".") );
        a2dGlobals->GetImagePathList().Add( wxT(".") );
        a2dGlobals->GetFontPathList().Add( wxT("..") );
        a2dGlobals->GetImagePathList().Add( wxT("..") );
        a2dGlobals->GetIconPathList().Add( wxT("..") );
        a2dGlobals->GetIconPathList().Add( wxT(".") );
        a2dGlobals->GetIconPathList().Add( wxT("../../doc/images") );
        a2dGlobals->GetIconPathList().Add( wxT("../../doc/icons") );

        a2dGeneralGlobals->GetVariablesHash().SetVariableString(  wxT( "WXART2D" ), wxT("../../") );
        a2dGeneralGlobals->GetVariablesHash().SetVariableString(  wxT( "WXART2D_ART" ), wxT("../../art") );

        /*
        wxString* artdir = a2dGeneralGlobals->GetVariablesHash().GetVariableString(  wxT( "WXART2D_ART" ) );
        wxString artroot;
        if (artdir==NULL) {
            artdir = a2dGeneralGlobals->GetVariablesHash().GetVariableString(  wxT( "WXART2D" ) );
            if (artdir==NULL) {
                wxGetEnv(_T("WXART2D"), &artroot );
                if( artroot.IsEmpty() )
                {
                    a2dGeneralGlobals->GetVariablesHash().SetVariableString(  wxT( "WXART2D_ART" ), wxT("../../art") );
                    //a2dGeneralGlobals->GetVariablesHash().SetVariableString(  wxT( "WXART2D" ), wxT("../../art") );
                    wxMessageBox( "NO WXART2D DIRECTORY" );
                } else wxMessageBox( "WXART2D" + artroot );
            }
        }

        if (artdir!=NULL) {
            wxString t( artdir->c_str() );

            wxMessageBox( t );
        }
        */
        //m_pConnectionsCanvasDocument = new a2dCanvasDocument();
       // m_pConnectionsCanvasDocument->SetEvtHandlerEnabled(true);

        //a2dSmrtPtr<a2dCanvasDocument> root = m_pConnectionsCanvasDocument;

        int dvx = GetSize().GetWidth();
        int dvy = GetSize().GetHeight();

        if ( GetSize() == wxDefaultSize )
        {
            dvx = 1000;
            dvy = 1000;
        }
        else
            GetClientSize(&dvx,&dvy); //this will be visible width and height in  world coordinates

        //a2dFontSearchPathDialog * fdlg = new a2dFontSearchPathDialog(this);
        //if (fldg) fdlg->ShowModal();

        //a2dCanvasGlobal* a2dCanvasGlobals = new a2dCanvasGlobal();
        //a2dCanvasGlobals->GetFontPathList().Add( wxT(".") );
        //a2dCanvasGlobals->GetFontPathList().Add( wxT("..") );
        //a2dGeneralGlobal->GetFontPathList().Add( wxT(".") );
        //a2dGeneralGlobal->GetFontPathList().Add( wxT("..") );

       #if wxART2D_USE_AGGDRAWER
           m_pDrawer2D = new a2dAggDrawer( dvx, dvy );
       #else
            m_pDrawer2D = new a2dMemDcDrawer( dvx, dvy );
       #endif

        m_pDrawer2D->SetDrawerStroke( a2dStroke( *wxRED, 4.0 ) );
        m_pDrawer2D->SetDrawerFill( a2dFill( *wxBLUE ) );
        //blit to this
        m_pDrawer2D->SetDisplay( this );
        m_pCanvas = new a2dCanvas( this,  -1 , wxPoint(0,0), wxSize(100,50), wxHSCROLL|wxVSCROLL, m_pDrawer2D );

        //defines the minimum x and y to show and the number of world units per pixel in x and y
        m_pCanvas->SetMappingUpp(0,0,1,1);
        m_pCanvas->SetYaxis(false);
        m_pCanvas->FixScrollMaximum( true );
        m_pCanvas->SetScrollBarsVisible(true);
        //defines the whole area in world coordinates that can be seen by scrolling
        m_pCanvas->SetScrollMaximum(0,0,1600,1600);
        //defines the number of world units scrolled when line up or down events in X
        m_pCanvas->SetScrollStepX(1);
        //defines the number of world units scrolled when line up or down events in Y
        m_pCanvas->SetScrollStepY(1);
        a2dFill back = a2dFill(wxColour(255,255,255 ),a2dFILL_SOLID);
        m_pCanvas->SetBackgroundFill(back);
        m_pCanvas->SetGridX( 20.0 );
        m_pCanvas->SetGridY( 20.0 );

        m_pCanvas->SetGrid( true );
        m_pCanvas->SetGridLines( true );
        //m_canvas->SetGridAtFront( true );
        m_pCanvas->SetGridStroke( a2dStroke( wxColour( 0xf0, 0xf0, 0xf0), 2 ) );
        //m_pCanvas->SetMouseEvents(true);
        //m_pCanvas->AppendEventHandler(this);
        a2dRouteData::SetRaster( 2.0 );

        a2dCanvasGlobals->SetRestrictionEngine(restrict);



        m_pConnectionsCanvasDocument = m_pCanvas->GetCanvasDocument();
        //m_pCanvas->SetCanvasDocument(m_pConnectionsCanvasDocument);
        m_pConnectionsCanvasDocument->SetDocumentTemplate( doctemplatenew );
        m_pConnectionsCanvasDocument->CreateCommandProcessor();
        m_pConnectionsCanvasDocument->SetDocumentTypeName( doctemplatenew->GetDocumentTypeName() );
        m_pConnectionsCanvasDocument->SetTitle( wxT("wire document") );
        m_pConnectionsCanvasDocument->SetCanvasDocumentRecursive();
        a2dDocviewGlobals->GetDocviewCommandProcessor()->AddDocument( m_pConnectionsCanvasDocument );
        m_pCanvas->SetShowObject( m_pConnectionsCanvasDocument->GetRootObject() );

        //m_pConnectionsCanvasDocument->EnableAllViews(true);

        //m_pCanvas->SetMappingShowAll();
/*

        m_pConnectionsCanvasView->SetDisplayWindow( this );
        m_pConnectionsCanvasView->SetGridLines( true );
        //m_pConnectionsCanvasView->ShowOrigin();

        m_pConnectionsCanvasView->SetShowObject(m_pMoldeoCanvasObject);
*/
        //m_pConnectionsCanvasView->SetDocument(m_pConnectionsCanvasDocument);
        //m_pConnectionsCanvasView->SetMappingShowAll();

	#endif
}

moConnectionsWindow::~moConnectionsWindow()
{
	//(*Destroy(moConnectionsWindow)
	//*)
	//delete m_pDrawer2D;
	//delete m_pCanvas;
	//delete m_pConnectionsCanvasDocument;
}


void moConnectionsWindow::Init( moIDirectorActions*   pActionsHandler) {


    SetNextActionHandler( pActionsHandler );

#ifdef MO_WXART2D


    //moConnectionsToolContr* pTools = new moConnectionsToolContr(m_pCanvas->GetCanvasView(),(wxFrame*) wxGetApp().GetTopWindow() );
    a2dStToolContr* pTools = new a2dStToolContr(m_pCanvas->GetCanvasView(),(wxFrame*) wxGetApp().GetTopWindow() );
    pTools->SetTopTool( new a2dGraphicsMasterTool( pTools ) );


    ///SETTING UP PIN CLASSES
    int pinwidth = 9;//7; //pixels

    moPin::InitializeExtraClasses();


    //Setup a wire/pin to define which pins can connect, and with which wire
    a2dWirePolylineL* wire = new a2dWirePolylineL();
    wire->SetStroke(a2dStroke(wxColour(255,0,0), 8,a2dSTROKE_SHORT_DASH));
    //!todo oops putting this, somehow prevents connecting wires to this wire.
    // this is a bug.
    // wire->SetLayer( 2 );
    wire->SetStartPinClass( a2dPinClass::Wire );
    wire->SetEndPinClass( a2dPinClass::Wire );
    a2dPinClass::Wire->SetAngleLine( false );
    a2dPinClass::Object->SetAngleLine( false );
    a2dPinClass::Wire->SetConnectObject( wire );

    //Setup a wire/pin to define which  directional  pins can connect, and with which wire

    a2dWirePolylineL* wiredirect = new a2dWirePolylineL();
    a2dStroke wirestroke = a2dStroke(wxColour(0,255,255), 4,a2dSTROKE_LONG_DASH);
    wiredirect->SetStroke(wirestroke);
    wiredirect->SetStartPinClass( a2dPinClass::WireInput );
    wiredirect->SetEndPinClass( a2dPinClass::WireOutput );
    a2dArrow* arrow2 = new  a2dArrow( 0,0,4,0,2 );
    arrow2->SetStroke(wirestroke);
    wiredirect->SetEnd(arrow2);
    wiredirect->SetBegin(arrow2);


    // if you do not want wires connecting to wires

    a2dPinClass::WireInput->RemoveConnect( a2dPinClass::WireOutput );
    a2dPinClass::WireOutput->RemoveConnect( a2dPinClass::WireInput );
    wiredirect->SetGeneratePins( false );


    a2dPinClass::WireInput->SetAngleLine( false );
    a2dPinClass::WireOutput->SetAngleLine( false );
    a2dPinClass::ObjectInput->SetAngleLine( false );
    a2dPinClass::WireInput->SetConnectObject( wiredirect );
    a2dPinClass::ObjectOutput->SetAngleLine( false );
    a2dPinClass::WireOutput->SetConnectObject( wiredirect );

    //define the pin which will be used for generating dynamic connection pins
    /*
    a2dPin* defPin = new moPin( NULL, wxT("global"), moPin::ElementObject, 0, 0, 0, pinwidth, pinwidth );
    defPin->SetFill( wxColour( 5, 219, 225 ) );
    defPin->SetStroke( wxColour( 5, 219, 225 ), 0 );
    a2dPin* defPin3 = new moPin( NULL, wxT("global"), moPin::ElementObject, 0, 0, 0, pinwidth, pinwidth );
    defPin3->SetFill( wxColour( 5, 19, 225 ) );
    defPin3->SetStroke( wxColour( 75, 19, 225 ), 0 );
    a2dPin* defPin2 = new moPin( NULL, wxT("global"), moPin::ElementObject, 0, 0, 0, pinwidth, pinwidth );
    defPin2->SetFill( wxColour( 5, 219, 25 ) );
    defPin2->SetStroke( wxColour( 5, 219, 225 ), 0 );
    */
    a2dPin* defPin = new moPin( NULL, wxT("global"), moPin::ElementInlet, 0, 0, 0, pinwidth, pinwidth );
    defPin->SetFill( wxColour( 5, 219, 225 ) );
    defPin->SetStroke( wxColour( 5, 219, 225 ), 0 );
    a2dPin* defPin2 = new moPin( NULL, wxT("global"), moPin::ElementOutlet, 0, 0, 0, pinwidth, pinwidth );
    defPin2->SetFill( wxColour( 255, 0, 225 ) );
    defPin2->SetStroke( wxColour( 255, 255, 0 ), 0 );

    //Setup a pin class map array to define which pins can connect, and with which wire
/*
    a2dWirePolylineL* wireele = new a2dWirePolylineL();
    wireele->SetStroke(a2dStroke(wxColour(5,250,0), 0.5,a2dSTROKE_SOLID ));
    wireele->SetStartPinClass( moPin::ElementWire );
    wireele->SetEndPinClass( moPin::ElementWire );
    wireele->SetGeneratePins( false );
    moPin::ElementWire->SetConnectObject( wireele );
*/
    // define the template pins for new or rending features for pins of this class.
//    moPin::ElementObject->SetPin( defPin ) ;
//    moPin::ElementObject->SetPinCanConnect( defPin3 );
//    moPin::ElementObject->SetPinCannotConnect( defPin2 );

    moPin::ElementWire->SetPin( defPin2 ) ;
    moPin::ElementWire->SetPinCanConnect( defPin );
    moPin::ElementWire->SetPinCannotConnect( defPin2 );

    moPin::ElementInlet->SetPin( defPin ) ;
    moPin::ElementInlet->SetPinCanConnect( defPin2 );
    moPin::ElementInlet->SetPinCannotConnect( defPin );

    moPin::ElementOutlet->SetPin( defPin2 ) ;
    moPin::ElementOutlet->SetPinCanConnect( defPin );
    moPin::ElementOutlet->SetPinCannotConnect( defPin2 );

	a2dConnectionGenerator* elecon = new ElementConnectionGenerator();
    //moPin::ElementObject->SetConnectionGenerator( elecon );
    moPin::ElementInlet->SetConnectionGenerator( elecon );
    moPin::ElementOutlet->SetConnectionGenerator( elecon );
    moPin::ElementWire->SetConnectionGenerator( elecon );

    // define which connector should be used for connecting to the automatically generated on pin a2dPinClass::Any
    a2dPinClass::Any->SetConnectionGenerator( elecon );

    //moPin::ElementObject->AddConnect( moPin::ElementWire );
    //moPin::ElementObject->AddConnect( moPin::ElementObject );
    moPin::ElementInlet->AddConnect( moPin::ElementOutlet );
    moPin::ElementOutlet->AddConnect( moPin::ElementInlet );
    //moPin::ElementWire->AddConnect( moPin::ElementObject );
    moPin::ElementWire->AddConnect( moPin::ElementWire );

#endif
}

void moConnectionsWindow::OnSize(wxSizeEvent& event) {

#ifdef MO_WXART2D
    if (m_pCanvas) {
        m_pCanvas->SetSize( event.GetSize() );
    }
#endif
    event.Skip();

}


void moConnectionsWindow::OnPaint(wxPaintEvent& event)
{
#ifdef MO_WXART2D
    wxPaintDC dc(this);

    //ininiate the wxDc's etc. in a2dCanvasView.
    m_pDrawer2D->BeginDraw();

    //get the regions to update and add to the list
    //of areas that need to be blitted
    wxRegionIterator it( GetUpdateRegion() );
    while (it)
    {
        int x = it.GetX();
        int y = it.GetY();

        int w = it.GetWidth();
        int h = it.GetHeight();

        /* wxScrolledWindow
        int xx;
        int yy;
        CalcUnscrolledPosition( x, y, &xx, &yy);
        m_pDrawer2D->BlitBuffer( xx, yy, w, h );
        */

        m_pDrawer2D->BlitBuffer( x, y, w, h );

        it++;
    }
    m_pDrawer2D->EndDraw();
    #endif
}

void moConnectionsWindow::OnEraseBackground(wxEraseEvent& event)
{
}


moDirectorStatus moConnectionsWindow::ProjectUpdated( moProjectDescriptor p_projectdescriptor ) {

#ifdef MO_WXART2D
        moMobDescriptors pMobDescriptors = GetMobDescriptors();

        /// reload everything
        int icount = 0;
        moMoldeoCanvasObject*   pObj = NULL;
        a2d3DShape*   p3DObj = NULL;

        if (m_pConnectionsCanvasDocument)
            m_pConnectionsCanvasDocument->DeleteContents();
        else return MO_DIRECTOR_STATUS_ERROR;
        m_MoldeoCanvasObjects.Empty();

        for( MOuint i=0;  i < pMobDescriptors.Count(); i++) {

            moMobDescriptor pMobDescriptor = pMobDescriptors[i];

            switch(pMobDescriptor.GetMobDefinition().GetType()) {
                case MO_OBJECT_EFFECT:
                case MO_OBJECT_PREEFFECT:
                case MO_OBJECT_POSTEFFECT:
                case MO_OBJECT_MASTEREFFECT:
                case MO_OBJECT_IODEVICE:

                    pObj = new moMoldeoCanvasObject( icount*160+100,0+100,141,100,0,5, pMobDescriptor, this );
                    //p3DObj = new a2d3DShape( pObj , 5, 5);


                    m_MoldeoCanvasObjects.Add( pObj );
                    m_pConnectionsCanvasDocument->Append(pObj);

                    icount++;
                    break;

                case MO_OBJECT_RESOURCE:
                    break;

                default:
                    break;
            }


        }
#endif

         return MO_DIRECTOR_STATUS_OK;

}

/// only mob updated, search for mob and update it....
moDirectorStatus moConnectionsWindow::MobUpdated( moMobDescriptor p_MobDesc ) {

#ifdef MO_WXART2D
        for( MOuint i=0;  i < m_MoldeoCanvasObjects.Count(); i++) {
            moMoldeoCanvasObject*   pObj = m_MoldeoCanvasObjects[i];

            if (pObj) {

                moMobDescriptor& pMobDescriptor( pObj->GetMob() );
                if (p_MobDesc.GetMobDefinition().GetMoldeoId()==pMobDescriptor.GetMobDefinition().GetMoldeoId()) {
                    //m_pNextActionHandler->MobUpdated(  );
                    pObj->SetMob( p_MobDesc );
                }
            }
        }
#endif
        return MO_DIRECTOR_STATUS_OK;

}

void moConnectionsWindow::DeleteAll() {
#ifdef MO_WXART2D
        if (m_pConnectionsCanvasDocument)
            m_pConnectionsCanvasDocument->DeleteContents();
        m_MoldeoCanvasObjects.Empty();
#endif
}
