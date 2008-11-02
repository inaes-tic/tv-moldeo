#include "moCNote.h"

#include "moArray.cpp"
moDefineDynamicArray( moCNotes )

moCNote::moCNote() {
        m_x = 0.0;
        m_time = 0;
        m_track = 0;
        m_header = 0;
        m_note = 0;
        m_velocity = 0;
        m_dynamic = "";
        m_modulador = 0;
        m_tiempo = 0;
        m_pFont = NULL;
        m_grouphead = false;
}

moCNote::moCNote( float p_x, MOulong p_time, int p_track, int p_header, int p_note, int p_velocity, moText p_dynamic, int p_modulador, int p_tiempo, moFont* pFont ) {
        m_x = p_x;
        m_time = p_time;
        m_track = p_track;
        m_header = p_header;
        m_note = p_note;
        m_velocity = p_velocity;
        m_dynamic = p_dynamic;
        m_modulador = p_modulador;
        m_tiempo = p_tiempo;
        m_pFont = pFont;
        m_grouphead = false;
}

void moCNote::DrawTriangle( float note_delay_x, float w_track, float h_track, float inter_line, float triangle_length, moEffectState& state ) {

    bool bSostenido = false;
    float dy = ( inter_line / 2.0 ) ;
    float y = 0.0;

    moCHeader::HeaderToY( this, inter_line, y, bSostenido);

    glTranslatef( m_x - ( this->m_tiempo )*( w_track / 2 ) , y , 1.0 );

    glBindTexture( GL_TEXTURE_2D, 0 );

    glBegin(GL_QUADS);

        glTexCoord2i(0,0);
        glVertex2f( 0 , - (dy/2));

        glTexCoord2i( 0, 1);
        glVertex2f( 0, (dy/2) );

        glTexCoord2i(1,1);
        glVertex2f( triangle_length, (dy/2) );

        glTexCoord2i(1,0);
        glVertex2f( triangle_length, -(dy/2));

    glEnd();

}

void moCNote::DrawTrail( moCNote* pCNoteAnterior, float note_delay_x, float w_track, float h_track, float inter_line, moEffectState& state ) {

    bool bSostenido = false;
    //bool bSostenidoAnt = false;
    float dy = ( inter_line / 2.0 ) ;
    float y = 0.0;
    //float yant = 0.0;

    moCHeader::HeaderToY( this, inter_line, y, bSostenido);
    //moCHeader::HeaderToY( pCNoteAnterior, h, yPent, yant, bSostenidoAnt);

    //glTranslatef( m_x - ( this->m_tiempo )*( w / 2 ) , y , 1.0 );

    glBindTexture( GL_TEXTURE_2D, 0 );

    glBegin(GL_QUADS);

        glTexCoord2i(0,0);
        glVertex2f( m_x - ( this->m_tiempo )*( w_track / 2 ) , y );

        glTexCoord2i(0,1);
        glVertex2f( pCNoteAnterior->m_x - ( this->m_tiempo )*( w_track / 2 ), y );

        glTexCoord2i(1,1);
        glVertex2f( pCNoteAnterior->m_x - ( this->m_tiempo )*( w_track / 2 ), y - (dy/2)*(pCNoteAnterior->m_velocity/10) );

        glTexCoord2i(1,0);
        glVertex2f( m_x - ( this->m_tiempo )*( w_track / 2 ) , y - (dy/2)*(this->m_velocity/10) );

    glEnd();

}

void moCNote::Draw( float note_delay_x, float w_track, float h_track, float inter_line, moEffectState& state ) {

    glBindTexture( GL_TEXTURE_2D, 0 );
    glColor4f( 0, 0, 0, 1 );
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);

    bool bSostenido = false;
    float dy = ( inter_line / 2.0 ) ;
    float y = 0.0;

    moCHeader::HeaderToY( this, inter_line, y, bSostenido);

    glTranslatef( m_x - ( this->m_tiempo )*( w_track / 2 ) , y , 1.0 );

    if (bSostenido) {
        if (m_pFont) {
            moText sostxt = "#";
            m_pFont->SetForegroundColor( 0.0, 0.0, 0.0 );
            m_pFont->SetSize( dy*2 );
            m_pFont->Draw( - 2*dy , 0-dy/2, sostxt );
        }
    }
    if (m_pFont) {
        m_pFont->SetForegroundColor( 0.0, 0.0, 0.0 );
        m_pFont->SetSize( dy*4 );
    }

    glScalef( 1.0, 0.6, 1.0 );
    glRotatef( 45, 0.0, 0.0, 1.0 );

    int saturation = this->m_velocity;

    float cr,cg,cb;

    HSVtoRGB( saturation*2 , 255, 255, &cr, &cg, &cb);

    glColor4f( cr, cg, cb, 1.0 );

    gluDisk( quadric, 0, inter_line / 2.0, 12, 2 );

    gluDeleteQuadric(quadric);

}
