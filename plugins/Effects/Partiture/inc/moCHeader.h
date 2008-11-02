
#ifndef __MOCHEADER_H
#define __MOCHEADER_H

#include "moTypes.h"
#include "moEffect.h"

class moCNote;

class moCHeader {

  public:

    static int claveSol[12];

    static int claveSolSostenidos[12];


    static void HeaderToY( moCNote *p_CNote, float interline, float& y, bool &sostenido);
};

#include "moCNote.h"


#endif
