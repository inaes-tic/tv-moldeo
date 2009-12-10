#include "moEffectPartiture.h"
#include "moCPentagram.h"



moCPentagram::moCPentagram() {


}


moCPentagram::~moCPentagram() {


}

void moCPentagram::Init( moConfig* pConfigPartiture ) {

    m_pConfig = pConfigPartiture;

}

void moCPentagram::Draw( int header, int m_inter_line, int start_draw_line, int stop_draw_line, int m_stroke_width, float x_track, float y_track, float w_track, float h_track, moEffectState& state ) {

    int i = 0;
    float displace = 0.5 * m_inter_line;

    //OOJO!!! esta informacion dependera del
        //header.... (clave)
            //percusion, header -> una sola linea en el medio...
    switch(header) {

        default:
            break;
    }


    glBindTexture( GL_TEXTURE_2D, 0);
    //moTexture* pImage = (moTexture*) m_Config[moR(PARTITURE_PENTAGRAMATEXTURE)].GetData()->Pointer();

    //glBindTexture( GL_TEXTURE_2D, m_Config[moR(PARTITURE_PENTAGRAMATEXTURE)].GetData()->GetGLId(&state.tempo) );

    moParam& param( m_pConfig->GetParam( moR(PARTITURE_PENTAGRAMABACK) ) );
    moValue& col( m_pConfig->GetParam( moR(PARTITURE_PENTAGRAMABACK) ).GetValue() );
    //moValue& alp( m_Config[moR(PARTITURE_ALPHA)][MO_SELECTED] );
    //MODebug2->Push( moText("col back:") + param.GetParamDefinition().GetName() );

    moEffect::SetColor( m_pConfig->GetParam("pentagramaback").GetValue(), m_pConfig->GetParam(moR(PARTITURE_ALPHA)).GetValue(), state );

    //glColor4f( 0, 0, 0, 1);

    glBegin(GL_QUADS);
        glTexCoord2f( 0, 0);
        glVertex2f( x_track - w_track/2, y_track - h_track/2 );

        glTexCoord2f( 0, 1);
        glVertex2f(  x_track + w_track/2, y_track - h_track/2 );

        glTexCoord2f( 1, 1);
        glVertex2f(  x_track + w_track/2,  y_track + h_track/2 );

        glTexCoord2f( 1, 0);
        glVertex2f( x_track - w_track/2,  y_track + h_track/2 );
    glEnd();

	moEffect::SetColor( m_pConfig->GetParam(moR(PARTITURE_PENTAGRAMAFRONT)).GetValue(),
            m_pConfig->GetParam(moR(PARTITURE_ALPHA)).GetValue(), state );

    //glColor4f( 1, 1, 1, 1);

    for( i=start_draw_line; i<stop_draw_line; i++) {
        glBegin(GL_QUADS);
            glTexCoord2f( 0, 0);
            glVertex2f( x_track-w_track/2, y_track - h_track/2  + m_inter_line*i - m_stroke_width/2 + displace );

            glTexCoord2f( 0, 1);
            glVertex2f(  x_track+w_track/2, y_track - h_track/2 + m_inter_line*i - m_stroke_width/2 + displace );

            glTexCoord2f( 1, 1);
            glVertex2f(  x_track+w_track/2,  y_track - h_track/2 + m_inter_line*i + m_stroke_width/2 + displace );

            glTexCoord2f( 1, 0);
            glVertex2f( x_track-w_track/2,  y_track - h_track/2 + m_inter_line*i + m_stroke_width/2 + displace );
        glEnd();
    }

}
