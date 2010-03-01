/*******************************************************************************

                              moEffectParticlesSimple.h

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


  TODO:
  1) ColorIn
  2) ColorOut
  3) BreakIn
  4) BreakOut



  5) Script general
  6) Script por particula ( se ejecuta por cada particula ) ( acceso a eventos [tecla presionada...] )

  *) movimiento y posicionamieno de particuclas....

  7) parametro desfazaje de movimiento??? que sea una opcion...

  8) Improve the orientation modes... they dont work correctly

*******************************************************************************/

#ifndef __MO_EFFECT_PARTICLES_H__
#define __MO_EFFECT_PARTICLES_H__

#include "moPlugin.h"
#include "moScript.h"

#define MO_PARTICLES_TRANSLATE_X 0
#define MO_PARTICLES_TRANSLATE_Y 1
#define MO_PARTICLES_SCALE_X 2
#define MO_PARTICLES_SCALE_Y 3

///Emitter Type
/**
* simple geometry type emitters available
*/
enum moParticlesSimpleEmitterType {
  PARTICLES_EMITTERTYPE_GRID = 0,
  PARTICLES_EMITTERTYPE_SPHERE = 1,
  PARTICLES_EMITTERTYPE_TUBE = 2,
  PARTICLES_EMITTERTYPE_JET = 3,
  PARTICLES_EMITTERTYPE_POINT = 4,
  PARTICLES_EMITTERTYPE_TRACKER = 5,
  PARTICLES_EMITTERTYPE_TRACKER2 = 6,
  PARTICLES_EMITTERTYPE_SPIRAL = 7,
  PARTICLES_EMITTERTYPE_CIRCLE = 8
};

///Folder Shot Type
/**
*   Folder
*/
enum moParticlesSimpleFolderShotType {
  PARTICLES_SHOTTYPE_FOLDER_RANDOM = 0,
  PARTICLES_SHOTTYPE_FOLDER_SEQUENTIAL_BY_FILEDATE = 1,
  PARTICLES_SHOTTYPE_FOLDER_SEQUENTIAL_BY_FILENAME = 2
};

///Attractor Type
enum moParticlesSimpleAttractorType {
  PARTICLES_ATTRACTORTYPE_POINT = 0, /// each particle attract diretly to the same point
  PARTICLES_ATTRACTORTYPE_GRID = 1, /// each particle attract perp to a face of the grid
  PARTICLES_ATTRACTORTYPE_SPHERE = 2, /// each particle attract perp to a face of the grid
  PARTICLES_ATTRACTORTYPE_TUBE = 3, ///  each particle attract perp to a face of the grid
  PARTICLES_ATTRACTORTYPE_JET = 4, /// each particle attract perpendicular to jet vector
  PARTICLES_ATTRACTORTYPE_TRACKER = 5, /// each particle attract each one to a dot of the tracker
  PARTICLES_ATTRACTORTYPE_VERTEX = 6 /// each particle attract each one to a dot of the tracker
};

///Attractor Mode
enum moParticlesSimpleAttractorMode {
  PARTICLES_ATTRACTORMODE_ACCELERATION = 0, ///accelerate with no stop
  PARTICLES_ATTRACTORMODE_STICK = 1, ///accelerate, reach and stop instantly
  PARTICLES_ATTRACTORMODE_BOUNCE = 2, ///accelerate and bounce....(inverse direction)
  PARTICLES_ATTRACTORMODE_BREAKS = 3, ///accelerate and breaks (generate debris on place)
  PARTICLES_ATTRACTORMODE_BRAKE = 4,  ///accelerate then brake and slowdown slowly
  PARTICLES_ATTRACTORMODE_LINEAR = 5 ///constant speed to attractortype
};

///Behaviour Mode
enum moParticlesSimpleBehaviourMode {
  PARTICLES_BEHAVIOUR_COHESION = 0,
  PARTICLES_BEHAVIOUR_SEPARATION = 1,
  PARTICLES_BEHAVIOUR_AVOIDANCE = 2,
  PARTICLES_BEHAVIOUR_ALIGNEMENT = 3
};


///Texture Mode
enum moParticlesSimpleTextureMode {
    PARTICLES_TEXTUREMODE_UNIT = 0, /// One Same Texture Image for each Particle (taken from texture)
    PARTICLES_TEXTUREMODE_PATCH = 1, /// Same Texture Image Divided In Different Fragments for each Particle (taken from texture, divided in width*height)
    PARTICLES_TEXTUREMODE_MANY = 2, /// Many Different Textures Image for each Particle ( taken from texturefolder )
    PARTICLES_TEXTUREMODE_MANY2PATCH = 3 /// Many textures/particle to construct a patched texture one ( taken from texturefolder, build the one defined on texture parameter, or from a folder, call to Shot(source) then ReInit to build... )
};

///Creation Method
enum moParticlesCreationMethod {
    PARTICLES_CREATIONMETHOD_LINEAR=0, ///particles are been created along the array order
    PARTICLES_CREATIONMETHOD_PLANAR=1, ///particles are been created randomly over the surface of the emitter geometry
    PARTICLES_CREATIONMETHOD_VOLUMETRIC=2, ///particles are been created randomly inside the volume defined by the emitter geometry
    PARTICLES_CREATIONMETHOD_CENTER=3 /// particles are created on the center of the emitter
};

///Random Method
enum moParticlesRandomMethod {
    PARTICLES_RANDOMMETHOD_NOISY=0,
    PARTICLES_RANDOMMETHOD_COLINEAR=1,
    PARTICLES_RANDOMMETHOD_PERPENDICULAR=2
};

///Orientation Method
enum moParticlesOrientationMode {
    PARTICLES_ORIENTATIONMODE_FIXED=0,
    PARTICLES_ORIENTATIONMODE_CAMERA=1,
    PARTICLES_ORIENTATIONMODE_MOTION=2,
    PARTICLES_ORIENTATIONMODE_ACCELERATION=3
};


enum moParticlesSimpleParamIndex {
	PARTICLES_ALPHA,
	PARTICLES_COLOR,
	PARTICLES_PARTICLECOLOR,
	PARTICLES_SYNC,
	PARTICLES_PHASE,
	PARTICLES_FONT,
	PARTICLES_TEXT,
	PARTICLES_ORTHO,

	PARTICLES_TEXTURE,
	PARTICLES_FOLDERS,
	PARTICLES_TEXTUREMODE,
	PARTICLES_BLENDING,
	PARTICLES_TIMETOREVELATION,
	PARTICLES_TIMEOFREVELATION,
	PARTICLES_TIMETORESTORATION,
	PARTICLES_TIMEOFRESTORATION,
	PARTICLES_DRAWINGFEATURES,

	PARTICLES_WIDTH,
	PARTICLES_HEIGHT,
	PARTICLES_SIZEX,
	PARTICLES_SIZEY,
	PARTICLES_SIZEZ,

	PARTICLES_GRAVITY,
	PARTICLES_VISCOSITY,

	PARTICLES_MAXAGE,
	PARTICLES_RANDOMMETHOD,
	PARTICLES_CREATIONMETHOD,
	PARTICLES_ORIENTATIONMODE,
	PARTICLES_EMITIONPERIOD,
	PARTICLES_EMITIONRATE,
	PARTICLES_DEATHPERIOD,
	PARTICLES_SCRIPT,

	PARTICLES_FADEIN,
	PARTICLES_FADEOUT,
	PARTICLES_SIZEIN,
	PARTICLES_SIZEOUT,

	PARTICLES_RANDOMPOSITION,
	PARTICLES_RANDOMPOSITION_X,
	PARTICLES_RANDOMPOSITION_Y,
	PARTICLES_RANDOMPOSITION_Z,

	PARTICLES_RANDOMVELOCITY,
	PARTICLES_RANDOMVELOCITY_X,
	PARTICLES_RANDOMVELOCITY_Y,
	PARTICLES_RANDOMVELOCITY_Z,

	PARTICLES_RANDOMMOTION,
	PARTICLES_RANDOMMOTION_X,
	PARTICLES_RANDOMMOTION_Y,
	PARTICLES_RANDOMMOTION_Z,


	PARTICLES_EMITTERTYPE,
	PARTICLES_EMITTERVECTOR_X,
	PARTICLES_EMITTERVECTOR_Y,
	PARTICLES_EMITTERVECTOR_Z,

	PARTICLES_ATTRACTORTYPE,
	PARTICLES_ATTRACTORMODE,
	PARTICLES_ATTRACTORVECTOR_X,
	PARTICLES_ATTRACTORVECTOR_Y,
	PARTICLES_ATTRACTORVECTOR_Z,

    PARTICLES_SCALEX_PARTICLE,
	PARTICLES_SCALEY_PARTICLE,
	PARTICLES_SCALEZ_PARTICLE,
	PARTICLES_ROTATEX_PARTICLE,
	PARTICLES_ROTATEY_PARTICLE,
	PARTICLES_ROTATEZ_PARTICLE,
	PARTICLES_TRANSLATEX,
	PARTICLES_TRANSLATEY,
	PARTICLES_TRANSLATEZ,
    PARTICLES_SCALEX,
	PARTICLES_SCALEY,
	PARTICLES_SCALEZ,
	PARTICLES_ROTATEX,
	PARTICLES_ROTATEY,
	PARTICLES_ROTATEZ,
	PARTICLES_EYEX,
	PARTICLES_EYEY,
	PARTICLES_EYEZ,
	PARTICLES_VIEWX,
	PARTICLES_VIEWY,
	PARTICLES_VIEWZ,
	PARTICLES_LIGHTX,
	PARTICLES_LIGHTY,
	PARTICLES_LIGHTZ,
    PARTICLES_INLET,
	PARTICLES_OUTLET
};

class moParticlesSimplePhysics : public moAbstract {

  public:

    moVector3f      m_GravityCenter;//x,y,z,intensity
    moVector3f      m_GravityVector;//x,y,z,intensity
    moVector3f      m_GravityDisc;//normal x, normal y, normal z,intensity

    moVector3f      m_Plane;

    long            m_MaxAge;
    long            m_EmitionPeriod;
    long            m_EmitionRate;
    long            m_DeathPeriod;

    moParticlesCreationMethod   m_CreationMethod;
    moParticlesRandomMethod     m_RandomMethod;
    moParticlesOrientationMode  m_OrientationMode;


    double          m_FadeIn;
    double            m_FadeOut;
    double            m_SizeIn;
    double            m_SizeOut;

    moParticlesSimpleEmitterType    m_EmitterType;
    moVector3f      m_EmitterSize;

    moParticlesSimpleAttractorType    m_AttractorType;
    moParticlesSimpleAttractorMode    m_AttractorMode;

    moVector3f      m_EyeVector;

    moVector3f      m_EmitterVector;
    moVector3f      m_AttractorVector;

    double          m_RandomVelocity;
    moVector3f      m_VelocityVector;

    double           m_RandomPosition;
    moVector3f      m_PositionVector;

    double          m_RandomMotion;
    moVector3f      m_MotionVector;

    double gravitational;
    double viscousdrag;

    moTimer EmitionTimer;

    moText  m_ParticleScript;




};

class moParticlesSimple : public moAbstract {

  public:

    ///Position absolute
    moVector3f  Pos3d;

    ///Destination
    moVector3f  Destination;

    ///Speed or Velocity vector
    moVector3f  Velocity;

    ///Self force (propulsion) of the particle
    moVector3f  Force;

    ///Particles Vectors

    ///position relative to grid...
    /**
        Works for indexation on a grid relative geometry ( bidimentional array )
        X > i [0..width]
        Y > j [0..height]
    */
    moVector2f  Pos;//relativo a una grilla

    ///texture coordinate
    moVector2f  TCoord;
    moVector2f  TCoord2;

    ///particle size
    /**
        This size is dependent on particles number and Emitter size
        Size.X > EmitterSize.X() / m_cols
        Size.Y > EmitterSize.Y() / m_rows
    */
    moVector2f  Size;

    ///particle texture size
    moVector2f  TSize;
    moVector2f  TSize2;

    ///particle orientation
    /**
        Dependant on Orientation Mode

    */
    moVector3f  U,V,W;

    ///Differentials of position (speed) and velocity (acceleration)
    moVector3f  dpdt;
    moVector3f  dvdt;

    ///Mass in g of the particle
    double      Mass;

    ///No motion
    bool        Fixed;

    ///Exists but is invisible
    bool        Visible;

    ///Captured
    /**
        Captured by the general script, out of normal flow
    */
    bool        Captured;


    ///Transparency of the particle
    double      Alpha;

    ///Scale of the particle
    double      Scale;

    ///Angle of the particle
    moVector3f      Rotation;

    ///Unique ID of OpenGL Texture
    MOint       GLId;///para asociar la textura al momento de la creación
    MOint       GLId2;
    moTextureMemory *pTextureMemory;

    ///Texture image proportion Width / Height ....
    float       ImageProportion;

    moVector3f  Color;

    ///Age of the particle
    moTimer     Age;
};


enum enumRevelationStatus {

    PARTICLES_FULLRESTORED = 0,
    PARTICLES_REVEALING = 1,
    PARTICLES_REVEALINGALL = 2,
    PARTICLES_FULLREVEALED = 3,
    PARTICLES_RESTORING = 4,
    PARTICLES_RESTORINGALL = 5
};



moDeclareDynamicArray( moParticlesSimple*, moParticlesSimpleArray )

class moEffectParticlesSimple : public moEffect
{
    public:


        moEffectParticlesSimple();
        virtual ~moEffectParticlesSimple();

        MOboolean Init();
        void Draw(moTempo*, moEffectState* parentstate = NULL);
        MOboolean Finish();

        void ReInit();
        void Shot( moText source = moText(""), int shot_type = 0, int shot_file=0  );

        void Interaction( moIODeviceManager * );
        void Update( moEventList * p_eventlist);
        moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

        int ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber);

        void TrackParticle( int partid );


    private:

        void UpdateParameters();

        void ResetTimers();
        void RevealingAll();
        void RestoringAll();


        void InitParticlesSimple( int p_cols, int p_rows , bool p_forced = true  );

        void CalculateForces( bool tmparray = false );
        void CalculateDerivatives( bool tmparray = false, double dt = 0.0 );

        void UpdateParticles( double dt, int method );
        void Regenerate();
        double Solver1D( double h, double y0, int method, double (*fcn)(double) );

        void SetParticlePosition( moParticlesSimple* pParticle );


        void ParticlesSimpleInfluence( float posx, float posy, float velx, float vely, float veln );
        void ParticlesSimpleAnimation( moTempo* tempogral, moEffectState* parentstate );
        void DrawParticlesSimple( moTempo* tempogral, moEffectState* parentstate );
        void DrawTracker();

        /// Script functions.
        void RegisterFunctions();

        int luaGetParticle(moLuaVirtualMachine& vm);
        int luaGetParticlePosition(moLuaVirtualMachine& vm);
        int luaGetParticleSize(moLuaVirtualMachine& vm);
        int luaGetParticleScale(moLuaVirtualMachine& vm);
        int luaGetParticleVelocity(moLuaVirtualMachine& vm);
        int luaGetParticleRotation(moLuaVirtualMachine& vm);

        int luaUpdateParticle( moLuaVirtualMachine& vm );
        int luaUpdateParticlePosition( moLuaVirtualMachine& vm );
        int luaUpdateParticleSize( moLuaVirtualMachine& vm );
        int luaUpdateParticleScale( moLuaVirtualMachine& vm );
        int luaUpdateParticleVelocity( moLuaVirtualMachine& vm );
        int luaUpdateParticleRotation( moLuaVirtualMachine& vm );

        int luaUpdateForce( moLuaVirtualMachine& vm );

        int luaShot(moLuaVirtualMachine& vm);
        int luaReInit(moLuaVirtualMachine& vm);

        int luaDrawPoint(moLuaVirtualMachine& vm);
        int luaGetParticleIntersection(moLuaVirtualMachine& vm);

        ///end script functions


        moParticlesSimpleArray    m_ParticlesSimpleArray;
        moParticlesSimpleArray    m_ParticlesSimpleArrayTmp;
        moParticlesSimplePhysics    m_Physics;

        bool                    m_bTrackerInit;

        moTrackerSystemData*    m_pTrackerData;
        moTUIOSystemData*       m_pTUIOData;

        MOint                   m_InletTuioSystemIndex;
        MOint                   m_InletTrackerSystemIndex;

        moVector2f              m_TrackerBarycenter;


        void setUpLighting();

        int m_rows,m_cols;
        float normalf; ///width of full floor usually 100.0f

        long time_tofull_revelation;
        long time_tofull_restoration;
        long time_of_revelation;
        long time_of_restoration;
        long drawing_features; /// 0: nothing 1: motion  2: all
        long texture_mode;

        bool ortho;

    ///internal
        moTimer MotionTimer;

        moTimer TimerFullRevelation; ///begins on first motion activity!!!!
        moTimer TimerFullRestoration;///begins on full revelation finished....
        moTimer TimerOfRevelation; ///begins on revealing all
        moTimer TimerOfRestoration;///begins on restoring all

        moTimer FeatureActivity;///start on first feature activity, ends on
        moTimer MotionActivity;///start on first motion activity, ends on no motion
        moTimer NoMotionActivity;///start on no motion, ends on first motion activity

        enumRevelationStatus revelation_status; /// 5: full revealed 0: full hidden

        long    m_Rate;

        long    last_tick;

        //special for script
        moTexture* pTextureDest;
        moTexture* pSubSample;
        MObyte* samplebuffer;

        int glidori;
        int glid;

        int original_width;
        int original_height;
        float original_proportion;

        float emiper;
        long emiperi;

        float midi_red, midi_green, midi_blue;
        float midi_maxage; //in millis
        float midi_emitionperiod;//in millisec
        float midi_emitionrate; // n per emitionperiod
        float midi_randomvelocity; //inicial vel
        float midi_randommotion; //motion dynamic

        float tx,ty,tz;
        float sx,sy,sz;
        float rx,ry,rz;

};

class moEffectParticlesSimpleFactory : public moEffectFactory
{
public:
    moEffectParticlesSimpleFactory() {}
    virtual ~moEffectParticlesSimpleFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
