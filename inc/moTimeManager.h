/*******************************************************************************

                                moTimeManager.h

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
  *                                                                          *
  *   This code is distributed in the hope that it will be useful, but       *
  *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
  *   General Public License for more details.                               *
  *                                                                          *
  *   A copy of the GNU General Public License is available on the World     *
  *   Wide Web at <http://www.gnu.org/copyleft/gpl.html>. You can also       *
  *   obtain it by writing to the Free Software Foundation,                  *
  *   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.         *
  *                                                                          *
  ****************************************************************************

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andrés Colubri

*******************************************************************************/

#ifndef __MO_TIMEMANAGER_H__
#define __MO_TIMEMANAGER_H__

#include "moTypes.h"
#include "moText.h"
#include "moConfig.h"
#include "moLock.h"
#include "moResourceManager.h"

/*

class LIBMOLDEO_API moMathFunction : public  moAbstract
{
public:
protected:
};

template class LIBMOLDEO_API moDynamicArray<moMathFunction*>;
typedef moDynamicArray<moMathFunction*> moMathFunctionArray;


class moCurve : public moMathFunction
{
public:
protected:
};

class moParserFunction : moMathFunction
{
public:
protected:
};

class moTempoFunction : moParserFunction
{
public:
protected:
};*/

enum moTimerState {
  MO_TIMERSTATE_STOPPED,
  MO_TIMERSTATE_PLAYING,
  MO_TIMERSTATE_PAUSED
};


LIBMOLDEO_API void moStartTimer();
LIBMOLDEO_API void moSetDuration( MOulong p_timecode );
LIBMOLDEO_API void moContinueTimer();
LIBMOLDEO_API void moPauseTimer();
LIBMOLDEO_API void moStopTimer();

class LIBMOLDEO_API moTimerAbsolute {
  public:
        bool on;
        bool pause_on;

        long start_tick;
        long start_last;

        long duration;

        long last_duration;

    moTimerAbsolute() {
        on = false;
        pause_on = false;
        start_tick = 0;
        start_last = 0;
        duration = 0;
    }

    int LastDuration() {
        return duration;
    }



    virtual void Start();

    void Stop() {
        on = false;
        pause_on = false;
        start_tick = 0;
        start_last = 0;
        duration = 0;
    }

    void Pause() {
        pause_on = true;
    }

    void Continue() {
        pause_on = false;
    }

    bool Started() {
        return on;
    }

    bool Paused() {
        return pause_on;
    }

    virtual void SetDuration( MOulong p_timecode );

    virtual long Duration();

};
LIBMOLDEO_API void moStartTimer();
LIBMOLDEO_API void moSetDuration( MOulong p_timecode );
LIBMOLDEO_API void moContinueTimer();
LIBMOLDEO_API void moPauseTimer();
LIBMOLDEO_API void moStopTimer();


class LIBMOLDEO_API moTimer : public moTimerAbsolute {

    public:

        moTimer() : moTimerAbsolute() {
            m_TimerId = -1;
            m_ObjectId = -1;
        }

        void Start();
        void SetDuration( MOulong p_timecode );
        long Duration();
        void Fix();

        void SetTimerId( long p_timerid ) {
                m_TimerId = p_timerid;
        }

        long GetTimerId() {
                return m_TimerId;
        }

        void SetObjectId( long p_objectid ) {
                m_ObjectId = p_objectid;
        }

        long GetObjectId() {
                return m_ObjectId;
        }


        long m_TimerId;
        long m_ObjectId;



};

moDeclareExportedDynamicArray( moTimer*, moTimers)

class LIBMOLDEO_API moTimeManager : public moResource
{
	public:

		moTimeManager();
		virtual ~moTimeManager();

		virtual MOboolean Init();
		virtual MOboolean Finish();

        //static MOuint	StartTicks();
		static MOuint	GetTicks();

		moTimer*    NewTimer();
		void AddTimer( moTimer* pTimer ); ///agrega un temporizador para ser manipulado

		void    FixTimers(); /// modifica los temporizadores para adecuarse al cambio ocurrido en el temporizador absoluto...
        void    SetDuration( MOulong p_timecode );

        void ClearByObjectId( long p_objectid );
        void ClearByTimerId( long p_timerid );
        void FixByObjectId( long p_objectid );


        static moTimerAbsolute*     MoldeoTimer;

	protected:

		//static  m_StartTicks;
		//moMathFunctionArray m_func_array;
        moTimers    m_Timers;
};

#endif
