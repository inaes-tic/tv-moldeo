
#ifndef __MOCPENTAGRAM_H
#define __MOCPENTAGRAM_H

#include "moTypes.h"
#include "moEffect.h"
#include "moConfig.h"

class moCPentagram {

    public:
        moCPentagram();
        virtual ~moCPentagram();

        void Init( moConfig* pConfig );

        void Draw( float x, float y, float w, float h, moEffectState& state );

    private:

        moConfig*   m_pConfig;

};


#endif


