
#ifndef __MOCPENTAGRAM_H
#define __MOCPENTAGRAM_H

#include "moTypes.h"
#include "moEffect.h"
#include "moConfig.h"

class moCPentagram {

    public:
        moCPentagram();
        virtual ~moCPentagram();

        void Init( moConfig* pConfigPartiture );

        void Draw( int header, int m_inter_line, int start_draw_line, int stop_draw_line, int m_stroke_width, float x_track, float y_track, float w_track, float h_track, moEffectState& state );

    private:

        moConfig*   m_pConfig;

};


#endif


