#include "moCTrack.h"



moCTrack::moCTrack() {


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

                if ( Notes[ p_pNote->m_note ].Count() > m_MaxNotes ) {
                    Notes[ p_pNote->m_note ].Remove( 0 );
                }
                Notes[ p_pNote->m_note ].Add( p_pNote );
                break;

        }
    }

}

void moCTrack::Draw(  float x, float y, float w, float h, moEffectState& state, int scrolltime ) {

    Pentagram.Draw( x, y, w, h, state );

    for( int m=0; m < 128 ; m++ ) {


        switch( m_DynamicalMode ) {

            case STATIC_MODE:

                for( int n=0; n < Notes[m].Count(); n++ ) {

                    glPushMatrix();

                    moCNote* pCNote = Notes[m][n];

                    if (pCNote) {

                        //pCNote->x = pCNote->x - 0.005*( state.tempo.delta );
                        MOulong delay = state.tempo.ticks - pCNote->m_time;
                        pCNote->m_x =  (float) delay * (float) w / (float)scrolltime;

                        //MODebug->Push( moText(" Time: ") + IntToStr( pCNote->m_time ) + moText(" X: ") + FloatToStr( pCNote->x ));
            //((float)pCNote->x * (float)m_pResourceManager->GetRenderMan()->ScreenWidth())
                        if (pCNote->m_x > 0.0)
                        pCNote->Draw( (float) w - pCNote->m_x,
                                    (float) w, //(float) hPent / 4.0,
                                    (float) h / 4.0,//(float) hPent / 4.0,
                                    (float) y, //(float) yPent + t*hPent*2.5 ,
                                    (float) h , //(float) hPent );
                                     state );
                    }

                    glPopMatrix();

                }
                break;

            case DYNAMIC_MODE:

                moCNote* pCNoteAnterior = NULL;
                float trailx,traily,trail_velocity;

                for( int n=0; n < Notes[m].Count(); n++ ) {

                    moCNote* pCNote = Notes[m][n];

                    if ( pCNote->m_velocity == 0 ) {

                        pCNoteAnterior = NULL;

                    } else if ( pCNoteAnterior==NULL ) {
                        glPushMatrix();
                        //start note trail
                        MOulong delay = state.tempo.ticks - pCNote->m_time;
                        pCNote->m_x =  (float) delay * (float) w / (float)scrolltime;

                        //MODebug->Push( moText(" Time: ") + IntToStr( pCNote->m_time ) + moText(" X: ") + FloatToStr( pCNote->x ));
            //((float)pCNote->x * (float)m_pResourceManager->GetRenderMan()->ScreenWidth())
                        if (pCNote->m_x > 0.0)
                        pCNote->Draw( (float) w - pCNote->m_x,
                                    (float) w, //(float) hPent / 4.0,
                                    (float) h / 4.0,//(float) hPent / 4.0,
                                    (float) y, //(float) yPent + t*hPent*2.5 ,
                                    (float) h , //(float) hPent );
                                     state );

                        pCNoteAnterior = pCNote;
                        glPopMatrix();

                    } else {
                        //continue note trail
                        glPushMatrix();

                        MOulong delay = state.tempo.ticks - pCNote->m_time;
                        pCNote->m_x =  (float) delay * (float) w / (float)scrolltime;

                        if (pCNote->m_x > 0.0)
                        pCNote->DrawTrail( pCNoteAnterior, (float) w - pCNote->m_x,
                                    (float) w, //(float) hPent / 4.0,
                                    (float) h / 4.0,//(float) hPent / 4.0,
                                    (float) y, //(float) yPent + t*hPent*2.5 ,
                                    (float) h , //(float) hPent );
                                     state );

                        glPopMatrix();


                    }

                }

            break;


        }
    }


}


