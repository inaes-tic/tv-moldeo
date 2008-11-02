

#ifndef __MOCNOTE_H
#define __MOCNOTE_H

#include "moTypes.h"
#include "moEffect.h"
#include "moFontManager.h"
#include "moCHeader.h"


class moCNote  {

  public:

        /** generic constructor
        *
        */
        moCNote();

        /** constructor with parameters
        *
        * @param    p_x     x position
        * @param    p_time  time
        * @param
        */
        moCNote( float p_x, MOulong p_time, int p_track, int p_header, int p_note, int p_velocity, moText p_dynamic, int p_modulador, int p_tiempo, moFont* pFont = NULL );

        /** Drawing function for CNote
        *
        * @param    x   position for note relative to middle pentagram position
        * @param    w   width of pentagram
        * @param    h   height of pentagram
        * @param    interline   interline of notes
        */
        void Draw( float note_delay_x, float w_track, float h_track, float interline, moEffectState& state );
        void DrawTrail( moCNote* pCNoteAnterior, float note_delay_x, float w_track, float h_track, float interline, moEffectState& state );
        void DrawTriangle( float note_delay_x, float w_track, float h_track, float inter_line, float triangle_length, moEffectState& state );


        /**
        *
        */
        virtual ~moCNote() {

        }

        /**
        *
        */
        MOulong m_time;

        /**
        *   Direcciona  los mensajes a cada track, a determinada pos según
            [POSICION Y de TRACK]

        */
        int m_track;

        /**
        * Define sobre que tipo de fondo se imprimen los datos.

            0, Blanco. [NOTA] lleva información de texto, el alineamiento es siempre central en Y.

            1, Una linea.

                    Claves.

            2, Clave de Sol 8va arriba. MAESTRO ALT-160
            3, Clave de Sol   		MAESTRO   SHIFT+7
                Mininote 60-79,    60 Do central una linea adicional debajo, a la misma distancia que el pentagrama.

            4, Clave de Sol 8va abajo  MAESTRO   SHIFT+V
            5, Clave de Fa                     MAESTRO /
                Mininote 40-60,   40 Mi una linea adicional debajo

            6, Clave de Fa 8va abajo    MAESTRO
            7, Pentagrama sin clave. [NOTA] recibe texto, pero sobreimpreso con pentagrama.


            Todos estos header disparan pentagrama

            Pentagrama 5 lineas, que cuando aparece el header . La clave aparece una vez y dibuja las lineas del pentagrama. Puede haber una opcion para que la clave quede en el borde izquierdo.
            Tambien se puede usar MAESTRO = para pentagrama



            7, Clave de percusión         MAESTRO 0139,

            Este header dispara una linea solamente.

        */
        int m_header;

        /**
        *
        */
        int m_note;

        /**
        * De 0 a 127, básicamente define la duración proporcional, que es una barra que se desprende de la nota con un margen minimo, quedandose pegada a la linea de play hasta que VELOCITY=0 de la misma nota.

            Luego de esto hace un scroll al lado de notehead



        */
        int m_velocity; //fuerza de la nota

        /**
        * DEBAJO de notehead, con distancia seteable respecto de ultima linea de pentagrama.

            SIEMPRE FUERA DEL PENTAGRAMA.


            1         MAESTRO ALT+0236            fff
            2         MAESTRO ALT+0196            ff
            3         MAESTRO ALT+0235            fp
            4         MAESTRO SHIFT+S	     sf
            5         MAESTRO F	                f
            6         MAESTRO SHIFT+F           mf
            7         MAESTRO SHIFT+P            mp
            8         MAESTRO P	                 p
            9         MAESTRO ALT+0185            pp
            10         MAESTRO ALT+0184        ppp

        */
        moText m_dynamic; //pianisimo

        /**
        * •	ENCIMA de notehead, con distancia variable FUERA DEL PENTAGRAMA O  ENCIMA DE NOTEHEAD.

            1
            2
            3
            4
            5
            7
            8
            9
            10


            •	CAMBIO DE NOTEHEAD

            11 NOTEHEAD  MAESTRO ALT-0207  (Default)
            12 NOTEHEAD  MAESTRO PERCUSSION Y
            13 NOTEHEAD  MAESTRO PERCUSSION SHIFT-Y
            14 NOTEHEAD  MAESTRO ALT+0209
            15 NOTEHEAD  MAESTRO PERCUSSION  F
            16 NOTEHEAD  MAESTRO ALT-0208
            17 NOTEHEAD  MAESTRO ALT-0250
        */
        int m_modulador; //

        /**
        * es la coordenada en X que define la posición de los objetos en el espacio
        * siempre centrados en NOTA. EN MILISEGUNDOS
        */
        int m_tiempo; // 0 -1


        float m_x;

        moFont* m_pFont;

        bool    m_grouphead;

};


moDeclareDynamicArray( moCNote*, moCNotes )

#endif
