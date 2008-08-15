#include "moEffectPartiture.h"
#include "moCPentagram.h"



moCPentagram::moCPentagram() {


}


moCPentagram::~moCPentagram() {


}

void moCPentagram::Init( moConfig* pConfig ) {

    m_pConfig = pConfig;

}

void moCPentagram::Draw( float x, float y, float w, float h, moEffectState& state ) {

    int i = 0;
    int dy = h / 4;

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
        glVertex2f( x, y-dy*3 );

        glTexCoord2f( 0, 1);
        glVertex2f(  x+w, y-dy*3 );

        glTexCoord2f( 1, 1);
        glVertex2f(  x+w,  y + h + dy*3 );

        glTexCoord2f( 1, 0);
        glVertex2f( x,  y + h + dy*3 );
    glEnd();

	moEffect::SetColor( m_pConfig->GetParam(moR(PARTITURE_PENTAGRAMAFRONT)).GetValue(),
            m_pConfig->GetParam(moR(PARTITURE_ALPHA)).GetValue(), state );

    //glColor4f( 1, 1, 1, 1);

    for( i=0; i<5; i++) {
        glBegin(GL_QUADS);
            glTexCoord2f( 0, 0);
            glVertex2f( x, y + dy*i );

            glTexCoord2f( 0, 1);
            glVertex2f(  x+w, y + dy*i );

            glTexCoord2f( 1, 1);
            glVertex2f(  x+w,  y + dy*i + 2 );

            glTexCoord2f( 1, 0);
            glVertex2f( x,  y + dy*i + 2 );
        glEnd();
    }

}
