

#ifndef __MOCTRACK_H
#define __MOCTRACK_H

#include "moTypes.h"
#include "moConfig.h"
#include "moCNote.h"
#include "moCPentagram.h"


/**
Linea de Duración.


El mensaje [DINAMICA]  0 3 [NOTA] [DINAMICA]

va a evolucionar y mandarse por cada nota, entonces quedaria por ejemplo si un cantante mantiene un DO CENTRAL por 10 segundos, a 1 mensaje por segundo:

0 3 60 100     Momento del ataque, a dinamica 100
0 3 60 101     Fluctuacion de la dinamica
0 3 60 101
0 3 60 100
0 3 60  99
0 3 60  98
0 3 60 100
0 3 60 101
0 3 60   0     RELEASE DE NOTA,

Entonces tenemos dos modos de funcionamiento, como habiamos hablado,
uno MIDI, de NOTA estatica, o sea DINAMICA DE ATAQUE Y DE RELEASE, y otro modo de NOTA DINAMICA, donde la DINAMICA EVOLUCIONA y SE DEDUCE EL RELEASE POR el VALOR 0.

Esto va a dibujar una linea de duración a la derecha de la nota, que se extiende hasta recibir nuevamente el mensaje DINAMICA = 0,
en ambos tipos de modos de mensaje. Esto en un primer paso,
En un segundo paso, podria extenderse la visualizacion del MODO de NOTA DINAMICA, a graficar los REGULADORES de INTENSIDAD que aparecen en musica que marcan las transiciiones entre pp y fff, por debaje del pentagrama.

IMPORTANTE, una vez que la nota recibio su RELEASE debe seguir su SCROLL hacia la linea de play, con linea de duracion incluida, porque esta es la informacion que va a darle al interprete la duracion de la nota, es decir que este sistema de notacion (lineas cuya prolongacion marca la duracion) reemplaza al sist tradicional de corchea negra blanca, que evitamos en CI.


------------------------------------------------------------------------
Un poco del motor algoritmico, por lo menos los titulos.



Generacion macroformal a traves de VARIABLES MACROFORMALES,

SONIDO SILENCIA, y.durtotal, x.0-127
FORMA y.durtotal.

Metaforas evolutivas, evolucion, mutacion de la herencia, 1. Adaptativo, Anagenesis, 2.Filogenetivo, cladogenesis.

Reconocimiento del atributo.
1.ANAGENESIS, cambio en la freq de los atributos en una poblacion a traves del tiempo.
              ,PERDIDA extncion del atributo.
              ,ADOPCION universal del atributo.
La anagensis se establece tambien en lo MICRO, termina en donde se define la especie TAXON.

UNIDAD de CAMBIO GEN, atributo heredado o caracter heredado, GEN es el portador o transmisor cultural.

TAXON, categoria de clasficiacion, necesida de difinicion de poblacion.

Elementos de TICK, instante.

GEN, pattern unidad de cambio transmisor: depende de TAXON, categoria de clasficiacion, poblamcion, accionadas por

ANAGENESIS, reconocer el atributo, cabmio en la freq de aparicion, repeticion, extincion del atrubo, adopcion universal.

Disparador que lo activa, contador cuantes veces paso, tamaño de captura, out, registro, ut , loop.
Descicion de cambio MACRO: METATAXON, shift extincion del atributo, adopcion Universal.


Convertir al oyente en lector de lo que oye.

*/

enum moDynamicalMode {
  STATIC_MODE, /// Modo estatico
  DYNAMIC_MODE /// Modo dinámico
};

class moCTrack : public moAbstract {

    public:

        moCTrack();
        virtual ~moCTrack();

        /** Agrega una nota a ser dibujada
        *
        */
        void AddNote( moCNote *p_pNote );

        void Init( moConfig* pConfig );

        /** Cambia la clave, por ende la forma de visualizacion del track y su pentagrama asociado
        *
        */
        void SetHeader( int p_header );
        int GetHeader(  );

        /**
        * Modo de creación de notas
        * Estatico:
        * Dinamico:
        */
        void SetMode( moDynamicalMode Mode, MOint MaxNotes );

        /**
        * Posicion del centro del track
        */
        void SetPosition( float x_track, float y_track );
        moVector2f GetPosition();

        /** Tamaño del track
        *
        */
        void SetSize( float w_track, float h_track);
        moVector2f GetSize();

        /** Lo que corresponde al ancho en milisegunddos
        *
        */
        void SetScrollTime( int scrolltime );
        int GetScrollTime();

        void SetFont(moFont* pFont);

        /**
        * Dibuja el track en la posicion centrada en x e y, con el ancho w, y el alto h
        * nota: la posicion y el tamaño puede ser modificadas dinámicamente
        *
        */
        void Draw( moEffectState& state );
        void DrawGroupToPlayLine( moCNotes &NoteGroup, moEffectState& state );
        void DrawGroup( moCNotes &NoteGroup, moEffectState& state );

        void UpdateHeaderParameters();


        moCNotes        Notes[128];
        moCPentagram    Pentagram;

        moConfig*       m_pConfig;

        moDynamicalMode m_DynamicalMode;
        MOint           m_MaxNotes;

        int             m_number;
        float           m_x_track;
        float           m_y_track;
        float           m_w_track;
        float           m_h_track;
        int             m_scrolltime;
        int             m_header;
        int             m_inter_line;
        int             m_start_draw_line;
        int             m_stop_draw_line;
        int             m_stroke_width;
        int             m_threashold;
        float           m_relation_w_time;


        moFont*         m_pFont;
};


#endif
