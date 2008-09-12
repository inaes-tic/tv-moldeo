#include "moCHeader.h"

int moCHeader::claveSol[12] = {
    0, //sol
    0, //sol#
    1, //la
    1, //la#
    2, //si
    3, //do
    3, //do#
    4, //re
    4, //re#
    5, //mi
    6,//fa
    6 //fa#
     };

int moCHeader::claveSolSostenidos[12] = {
    0, //sol
    1, //sol#
    0, //la
    1, //la#
    0, //si
    0, //do
    1, //do#
    0, //re
    1, //re#
    0, //mi
    0,//fa
    1 //fa#
     };


/*
55: sol

57: la

59: si
60: do

62: re

64: mi
65: fa
*/

void
moCHeader::HeaderToY( moCNote *p_CNote, float h, float yPent, float& y, bool &sostenido)  {

    int inty;
    int restoctav;
    float basey;
    float divoctav;

    // 1 octava son
    float yoctava = 7.0; //radios de nota

    float dy = ( h / 2.0 ) ;

    switch(p_CNote->m_header) {

        case 3:
            // entre 55 y 89, empezando por el sol
            inty = p_CNote->m_note - 55;

            restoctav = inty % 12; //resto
            divoctav = floor( inty / (float)12 ); //cuantas octavas???

            basey = yPent - 5.0 * dy;

            basey+= (float)yoctava * (float)divoctav * dy;
            basey+= (float) moCHeader::claveSol[ restoctav ] * dy;
            if ( moCHeader::claveSolSostenidos[restoctav] ==1 )
                sostenido = true;
            else
                sostenido = false;
            y = basey;
            break;
        default:
            break;
    }

}
