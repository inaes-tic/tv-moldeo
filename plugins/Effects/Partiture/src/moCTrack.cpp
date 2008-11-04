#include "moCTrack.h"



moCTrack::moCTrack() {

        m_number = 0;
        m_x_track = 0;
        m_y_track = 0;
        m_w_track = 300;
        m_h_track = 150;
        m_scrolltime = 2000;
        m_header = 3;
        m_inter_line = 10;
        m_start_draw_line = 0;
        m_stop_draw_line = 1;
        m_stroke_width = 2;
        m_pFont = NULL;
        m_threashold = 20;//in ms
        m_relation_w_time = (float) m_w_track / (float) m_scrolltime;
}

moCTrack::~moCTrack() {


}

void moCTrack::Init( moConfig* pConfig ) {

    m_pConfig = pConfig;
    Pentagram.Init( pConfig );

}

void moCTrack::SetMode( moDynamicalMode Mode, MOint MaxNotes ) {

    m_DynamicalMode = Mode;
    m_MaxNotes = MaxNotes;


}

void moCTrack::AddNote( moCNote *p_pNote ) {

    if ( p_pNote->m_note>=0 && p_pNote->m_note<128) {

        //aqui hay que trackear:
        //si el
        //p_x, MOulong p_time, int p_track, int p_header, int p_note, int p_velocity, moText p_dynamic, int p_modulador, int p_tiempo


        switch( m_DynamicalMode ) {

            //MODO 0 : luego de pasar un máximo de notas...borra las anteriores
            //esto se puede optimizar, borrando luego de haber agregado notas
            //a partir de cierto indice...ya que estan ordenadas las notas.
            case STATIC_MODE:
                if ( Notes[ p_pNote->m_note ].Count() > m_MaxNotes ) {
                    Notes[ p_pNote->m_note ].Remove( 0 );
                }

                Notes[ p_pNote->m_note ].Add( p_pNote );
                break;

            //MODO 1 : dinamico
            case DYNAMIC_MODE:

                //borrar unicamente las notas con su trailer que ya supero un x...
                // o una cantidad de notas...
                /*
                if ( Notes[ p_pNote->m_note ].Count() > m_MaxNotes ) {
                    Notes[ p_pNote->m_note ].Remove( 0 );
                }
                */
                Notes[ p_pNote->m_note ].Add( p_pNote );
                break;

        }
    }

}

void moCTrack::SetPosition( float x_track, float y_track ) {

    m_x_track = x_track;
    m_y_track = y_track;
}

moVector2f moCTrack::GetPosition() {
    return moVector2f( m_x_track, m_y_track);
}

/** Tamaño del track
*
*/
void moCTrack::SetSize( float w_track, float h_track) {

    m_w_track = w_track;
    m_h_track = h_track;

}

moVector2f
moCTrack::GetSize() {
    return moVector2f( m_w_track, m_h_track);
}

/** Lo que corresponde al ancho en milisegunddos
*
*/
void moCTrack::SetScrollTime( int scrolltime ) {
    m_scrolltime = scrolltime;
}

int moCTrack::GetScrollTime() {
    return m_scrolltime;
}

void moCTrack::SetFonts( moFont* pFont, moFont* pSketchFont ) {
        m_pFont = pFont;
        m_pSketchFont = pSketchFont;
}

void moCTrack::DrawGroupToPlayLine( moCNotes &NoteGroup, moEffectState& state ) {

    moCNote* pGroupHead = NULL;
    moCNote* pGroupTail = NULL;
    MOulong delay;
    float triangle_length;

    if (NoteGroup.Count()==0) return;

    pGroupHead = NoteGroup[0];

    triangle_length = 0 ;

    if (pGroupHead) {

        delay = state.tempo.ticks - pGroupHead->m_time;
        pGroupHead->m_x =  -(float) delay * m_relation_w_time;
        glPushMatrix();
        pGroupHead->Draw( (float) pGroupHead->m_x,
                    (float) m_w_track,
                    (float) m_h_track,
                    (float) m_inter_line,
                     state );
        glPopMatrix();

        //draw triangle until play line
        /*
        for( int gi=0; gi < NoteGroup.Count(); gi++ ) {
            pGroupTail = NoteGroup[gi];
        }
        */
        //length is until play line...
        glPushMatrix();
        if (NoteGroup.Count()==1) {
            pGroupTail  = pGroupHead;
        } else {
            pGroupTail = NoteGroup[ NoteGroup.Count() - 1 ];
        }

        if ( ( state.tempo.ticks - pGroupTail->m_time) < this->m_threashold ) {
            triangle_length = fabs( pGroupTail->m_x - ( pGroupTail->m_tiempo )*( m_w_track / 2 ));
            pGroupTail  = pGroupHead;
        } else {
            delay = state.tempo.ticks - pGroupTail->m_time;
            pGroupTail->m_x =  -(float) delay * m_relation_w_time;
            triangle_length = pGroupTail->m_x - pGroupHead->m_x;
        }
        pGroupTail->DrawTriangle( (float) pGroupTail->m_x,
                    (float) m_w_track,
                    (float) m_h_track,
                    (float) m_inter_line,
                    (float) triangle_length,
                     state );
        glPopMatrix();

    }

}

void moCTrack::DrawGroup( moCNotes &NoteGroup, moEffectState& state ) {

    moCNote* pGroupHead = NULL;
    moCNote* pGroupTail = NULL;
    MOulong delay;
    float triangle_length;

    if (NoteGroup.Count()==0) return;

    pGroupHead = NoteGroup[0];

    triangle_length = 0 ;

    if (pGroupHead) {

        delay = state.tempo.ticks - pGroupHead->m_time;
        pGroupHead->m_x =  -(float) delay * m_relation_w_time;
        glPushMatrix();
        pGroupHead->Draw( (float) pGroupHead->m_x,
                    (float) m_w_track,
                    (float) m_h_track,
                    (float) m_inter_line,
                     state );
        glPopMatrix();

        //draw triangle until play line
        /*
        for( int gi=0; gi < NoteGroup.Count(); gi++ ) {
            pGroupTail = NoteGroup[gi];
        }
        */
        //length is until play line...
        //draw triangle until last note
        pGroupTail = NoteGroup[ NoteGroup.Count() - 1 ];
        delay = state.tempo.ticks - pGroupTail->m_time;
        pGroupTail->m_x =  -(float) delay * m_relation_w_time;
        triangle_length = pGroupTail->m_x - pGroupHead->m_x;
        glPushMatrix();
        pGroupHead->DrawTriangle( (float) pGroupHead->m_x,
                    (float) m_w_track,
                    (float) m_h_track,
                    (float) m_inter_line,
                    (float) triangle_length,
                     state );

        glPopMatrix();

    }

}

void moCTrack::UpdateHeaderParameters() {


    m_relation_w_time = (float) m_w_track / (float)m_scrolltime;

    switch(m_header) {
        case 7:
            m_start_draw_line = 1;
            m_stop_draw_line = 2;
            m_stroke_width = 3;
            m_inter_line = m_h_track / 2;
            break;
        default:
            m_inter_line = m_h_track / 11;
            //m_start_draw_line = 0;
            //m_stop_draw_line = 11;
            m_start_draw_line = 3;
            m_stop_draw_line = 8;
            m_stroke_width = 3;
            break;
    }

}

void moCTrack::Draw(  moEffectState& state ) {

    UpdateHeaderParameters();

    if (m_pFont) {
        moText message = moText("Track:") + IntToStr(m_number) +  moText(" Header:") + IntToStr(this->m_header);
        m_pFont->SetForegroundColor( 0.0, 1.0, 0.0 );
        m_pFont->SetSize( m_inter_line );
        m_pFont->Draw( -m_w_track/2, m_h_track/2 ,  message);
    }
    Pentagram.Draw( m_header, m_inter_line, m_start_draw_line, m_stop_draw_line, m_stroke_width, m_x_track, m_y_track, m_w_track, m_h_track, state );


    moCNotes NoteGroup;
    moCNote* pCNote = NULL;
    MOulong delay;


    //127 notas máximas posibles
    for( int m=0; m < 128 ; m++ ) {

        switch( m_DynamicalMode ) {

            case STATIC_MODE:

                for( int n=0; n < Notes[m].Count(); n++ ) {

                    glPushMatrix();

                    pCNote = Notes[m][n];

                    if (pCNote) {

                        delay = state.tempo.ticks - pCNote->m_time;
                        pCNote->m_x =  -(float) delay * m_relation_w_time;

                        pCNote->Draw( (float) pCNote->m_x,
                                    (float) m_w_track,
                                    (float) m_h_track,
                                    (float) m_inter_line,
                                     state );
                    }

                    glPopMatrix();

                }
                break;

            case DYNAMIC_MODE:

                moCNote* pCNoteAnterior;

                NoteGroup.Empty();
                pCNoteAnterior = NULL;

                for( int n=0; n < Notes[m].Count(); n++ ) {

                    pCNote = Notes[m][n];

                    //tomar las notas distintas de velocity 0 con un intervalo entre ellas menor a m_threshold

                    //NOTA: armado de grupos:
                        //1) el grupo empieza con la primera nota cuyo velocity es distinto de cero
                        //2) la nota q es cabeza de grupo es marcada (m_grouphead=true)
                        //  2.1) luego las notas son borradas por grupos tambien... para hacer coherente la lectura
                        //3)

                    if (pCNote) {

                        /**** PRIMERA NOTA.... ***/
                        if ( pCNoteAnterior == NULL && pCNote->m_velocity != 0) {
                            //primera nota cuya velocity es distinta de cero empieza el grupo
                            //MODebug2->Push("Group starting: first note");

                            pCNote->m_grouphead = true;
                            NoteGroup.Add( pCNote );
                            pCNoteAnterior = pCNote;

                            ///solo se dibuja si esta sola (unica nota disponible)
                            if ( Notes[m].Count() == 1) {
                                this->DrawGroup( NoteGroup, state );
                            }
                        /**** SIGUIENTE NOTA EN THREASHOLD ****/
                        ///GRUPO NO CERRADO
                        ///SE DIBUJA SOLO SI ES LA ULTIMA NOTA DEL TONO
                        } else if ( pCNoteAnterior != NULL  && ( ( pCNote->m_time - pCNoteAnterior->m_time ) <= this->m_threashold ) ) {
                            //la nota esta cerca asi que la agregamos al grupo actual
                            //el velocity nos es indistinto
                            //MODebug2->Push("Group continuiing: next note");

                            NoteGroup.Add( pCNote );
                            pCNoteAnterior = pCNote;

                            //SI ES LA ULTIMA NOTA, ENTONCES
                                //CERRAMOS EL GRUPO
                                //LO DIBUJAMOS
                            if ( (Notes[m].Count()-1) == n ) {
                                    this->DrawGroup( NoteGroup, state  );
                            }

                        /**** NOTA FUERA DE THREASHOLD, PROXIMA NUEVA NOTA ****/
                        ///GRUPO CERRADO
                        ///SE DIBUJA INEXORABLEMENTE EL GRUPO
                        ///SE DIBUJA ESTA NOTA EN PARTICULAR SI ES LA ULTIMA
                        } else if ( pCNoteAnterior != NULL && ( ( pCNote->m_time - pCNoteAnterior->m_time ) > this->m_threashold ) ) {
                            //la nota esta suficientemente alejada para ser un nuevo grupo
                            //CERRAMOS EL GRUPO.....Y LO DIBUJAMOS
                            //y tiene velocity
                            //MODebug2->Push("New Group: first new note");

                            //AQUI DIBUJAMOS EL GRUPO SI TIENE NOTAS
                            this->DrawGroup( NoteGroup, state  );

                            NoteGroup.Empty();
                            //primera nota de un grupo....
                            pCNote->m_grouphead = true;
                            NoteGroup.Add( pCNote );
                            pCNoteAnterior = pCNote;

                            if ( (Notes[m].Count()-1) == n ) {
                                this->DrawGroup( NoteGroup, state);
                            }

                        } else {
                            //todavia no obtuvimos la primera nota distinta cuyo velocity sea distinto de 0

                        }

                    }

                    /*


                    if ( pCNote->m_velocity == 0 ) {

                        pCNoteAnterior = NULL;

                    } else if ( pCNoteAnterior==NULL ) {
                        glPushMatrix();
                        //start note trail
                        MOulong delay = state.tempo.ticks - pCNote->m_time;
                        pCNote->m_x =  -(float) delay * (float) m_w_track / (float)m_scrolltime;

                        //MODebug->Push( moText(" Time: ") + IntToStr( pCNote->m_time ) + moText(" X: ") + FloatToStr( pCNote->x ));
            //((float)pCNote->x * (float)m_pResourceManager->GetRenderMan()->ScreenWidth())
                        //if (pCNote->m_x > 0.0)
                        pCNote->Draw( (float) pCNote->m_x,
                                    (float) m_w_track,
                                    (float) m_h_track,
                                    (float) m_inter_line,
                                     state );

                        pCNoteAnterior = pCNote;
                        glPopMatrix();

                    } else {
                        //continue note trail
                        glPushMatrix();

                        MOulong delay = state.tempo.ticks - pCNote->m_time;
                        pCNote->m_x =  -(float) delay * (float) m_w_track / (float)m_scrolltime;

                        //if (pCNote->m_x > 0.0)
                        pCNote->DrawTrail( pCNoteAnterior, (float) m_w_track - pCNote->m_x,
                                    (float) m_w_track,
                                    (float) m_h_track ,
                                    (float) m_inter_line,
                                     state );

                        glPopMatrix();


                    }

                    */


                }

            break;


        }
    }

    if (pCNote) m_header = pCNote->m_header;


}


