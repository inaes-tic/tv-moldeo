#include "moDirectorIODeviceManager.h"

BEGIN_EVENT_TABLE( moDirectorIODeviceManager, wxEvtHandler)
	EVT_MOUSE_EVENTS( moDirectorIODeviceManager::OnMouseEvent )
	EVT_KEY_DOWN( moDirectorIODeviceManager::OnKeyDown )
	EVT_KEY_UP( moDirectorIODeviceManager::OnKeyUp )
END_EVENT_TABLE()

/*
WXK_BACK    = 8
    WXK_TAB     = 9
    WXK_RETURN  = 13
    WXK_ESCAPE  = 27
    WXK_SPACE   = 32
    WXK_DELETE  = 127

    WXK_START   = 300
    WXK_LBUTTON
    WXK_RBUTTON
    WXK_CANCEL
    WXK_MBUTTON
    WXK_CLEAR
    WXK_SHIFT
    WXK_CONTROL
    WXK_MENU
    WXK_PAUSE
    WXK_CAPITAL
    WXK_PRIOR
    WXK_NEXT
    WXK_END
    WXK_HOME
    WXK_LEFT
    WXK_UP
    WXK_RIGHT
    WXK_DOWN
    WXK_SELECT
    WXK_PRINT
    WXK_EXECUTE
    WXK_SNAPSHOT
    WXK_INSERT
    WXK_HELP
    WXK_NUMPAD0
    WXK_NUMPAD1

    WXK_NUMPAD2
    WXK_NUMPAD3
    WXK_NUMPAD4
    WXK_NUMPAD5
    WXK_NUMPAD6
    WXK_NUMPAD7
    WXK_NUMPAD8
    WXK_NUMPAD9
    WXK_MULTIPLY
    WXK_ADD
    WXK_SEPARATOR
    WXK_SUBTRACT
    WXK_DECIMAL
    WXK_DIVIDE
    WXK_F1
    WXK_F2
    WXK_F3
    WXK_F4
    WXK_F5
    WXK_F6
    WXK_F7
    WXK_F8
    WXK_F9
    WXK_F10
    WXK_F11
    WXK_F12
    WXK_F13
    WXK_F14
    WXK_F15
    WXK_F16
    WXK_F17
    WXK_F18
    WXK_F19
    WXK_F20
    WXK_F21
    WXK_F22
    WXK_F23
    WXK_F24
    WXK_NUMLOCK

    WXK_SCROLL
    WXK_PAGEUP,
    WXK_PAGEDOWN,

    WXK_NUMPAD_SPACE,
    WXK_NUMPAD_TAB,
    WXK_NUMPAD_ENTER,
    WXK_NUMPAD_F1,
    WXK_NUMPAD_F2,
    WXK_NUMPAD_F3,
    WXK_NUMPAD_F4,
    WXK_NUMPAD_HOME,
    WXK_NUMPAD_LEFT,
    WXK_NUMPAD_UP,
    WXK_NUMPAD_RIGHT,
    WXK_NUMPAD_DOWN,
    WXK_NUMPAD_PRIOR,
    WXK_NUMPAD_PAGEUP,
    WXK_NUMPAD_NEXT,
    WXK_NUMPAD_PAGEDOWN,
    WXK_NUMPAD_END,
    WXK_NUMPAD_BEGIN,
    WXK_NUMPAD_INSERT,
    WXK_NUMPAD_DELETE,
    WXK_NUMPAD_EQUAL,
    WXK_NUMPAD_MULTIPLY,

    WXK_NUMPAD_ADD,
    WXK_NUMPAD_SEPARATOR,
    WXK_NUMPAD_SUBTRACT,
    WXK_NUMPAD_DECIMAL,
    WXK_NUMPAD_DIVIDE,

    // the following key codes are only generated under Windows currently
    WXK_WINDOWS_LEFT,
    WXK_WINDOWS_RIGHT,
    WXK_WINDOWS_MENU,
    WXK_COMMAND,

    // Hardware-specific buttons
    WXK_SPECIAL1 = 193,
    WXK_SPECIAL2,
    WXK_SPECIAL3,
    WXK_SPECIAL4,
    WXK_SPECIAL5,
    WXK_SPECIAL6,
    WXK_SPECIAL7,
    WXK_SPECIAL8,
    WXK_SPECIAL9,
    WXK_SPECIAL10,
    WXK_SPECIAL11,
    WXK_SPECIAL12,
    WXK_SPECIAL13,

    WXK_SPECIAL14,
    WXK_SPECIAL15,
    WXK_SPECIAL16,
    WXK_SPECIAL17,
    WXK_SPECIAL18,
    WXK_SPECIAL19,
    WXK_SPECIAL20
*/
typedef enum {
       SDL_NOEVENT = 0,			/* Unused (do not remove) */
       SDL_ACTIVEEVENT,			/* Application loses/gains visibility */
       SDL_KEYDOWN,			/* Keys pressed */
       SDL_KEYUP,			/* Keys released */
       SDL_MOUSEMOTION,			/* Mouse moved */
       SDL_MOUSEBUTTONDOWN,		/* Mouse button pressed */
       SDL_MOUSEBUTTONUP,		/* Mouse button released */
       SDL_JOYAXISMOTION,		/* Joystick axis motion */
       SDL_JOYBALLMOTION,		/* Joystick trackball motion */
       SDL_JOYHATMOTION,		/* Joystick hat position change */
       SDL_JOYBUTTONDOWN,		/* Joystick button pressed */
       SDL_JOYBUTTONUP,			/* Joystick button released */
       SDL_QUIT,			/* User-requested quit */
       SDL_SYSWMEVENT,			/* System specific event */
       SDL_EVENT_RESERVEDA,		/* Reserved for future use.. */
       SDL_EVENT_RESERVEDB,		/* Reserved for future use.. */
       SDL_VIDEORESIZE,			/* User resized video mode */
       SDL_VIDEOEXPOSE,			/* Screen needs to be redrawn */
       SDL_EVENT_RESERVED2,		/* Reserved for future use.. */
       SDL_EVENT_RESERVED3,		/* Reserved for future use.. */
       SDL_EVENT_RESERVED4,		/* Reserved for future use.. */
       SDL_EVENT_RESERVED5,		/* Reserved for future use.. */
       SDL_EVENT_RESERVED6,		/* Reserved for future use.. */
       SDL_EVENT_RESERVED7,		/* Reserved for future use.. */
       /* Events SDL_USEREVENT through SDL_MAXEVENTS-1 are for your use */
       SDL_USEREVENT = 24,
       /* This last event is only for bounding internal arrays
	  It is the number of bits in the event mask datatype -- Uint32
        */
       SDL_NUMEVENTS = 32
} SDL_EventType;


#define SDL_BUTTON(X)		(1 << ((X)-1))
#define SDL_BUTTON_LEFT		1
#define SDL_BUTTON_MIDDLE	2
#define SDL_BUTTON_RIGHT	3
#define SDL_BUTTON_WHEELUP	4
#define SDL_BUTTON_WHEELDOWN	5
#define SDL_BUTTON_X1		6
#define SDL_BUTTON_X2		7
#define SDL_BUTTON_LMASK	SDL_BUTTON(SDL_BUTTON_LEFT)
#define SDL_BUTTON_MMASK	SDL_BUTTON(SDL_BUTTON_MIDDLE)
#define SDL_BUTTON_RMASK	SDL_BUTTON(SDL_BUTTON_RIGHT)
#define SDL_BUTTON_X1MASK	SDL_BUTTON(SDL_BUTTON_X1)
#define SDL_BUTTON_X2MASK	SDL_BUTTON(SDL_BUTTON_X2)


typedef enum {
	/* The keyboard syms have been cleverly chosen to map to ASCII */
	SDLK_UNKNOWN		= 0,
	SDLK_FIRST		= 0,
	SDLK_BACKSPACE		= 8,
	SDLK_TAB		= 9,
	SDLK_CLEAR		= 12,
	SDLK_RETURN		= 13,
	SDLK_PAUSE		= 19,
	SDLK_ESCAPE		= 27,
	SDLK_SPACE		= 32,
	SDLK_EXCLAIM		= 33,
	SDLK_QUOTEDBL		= 34,
	SDLK_HASH		= 35,
	SDLK_DOLLAR		= 36,
	SDLK_AMPERSAND		= 38,
	SDLK_QUOTE		= 39,
	SDLK_LEFTPAREN		= 40,
	SDLK_RIGHTPAREN		= 41,
	SDLK_ASTERISK		= 42,
	SDLK_PLUS		= 43,
	SDLK_COMMA		= 44,
	SDLK_MINUS		= 45,
	SDLK_PERIOD		= 46,
	SDLK_SLASH		= 47,
	SDLK_0			= 48,
	SDLK_1			= 49,
	SDLK_2			= 50,
	SDLK_3			= 51,
	SDLK_4			= 52,
	SDLK_5			= 53,
	SDLK_6			= 54,
	SDLK_7			= 55,
	SDLK_8			= 56,
	SDLK_9			= 57,
	SDLK_COLON		= 58,
	SDLK_SEMICOLON		= 59,
	SDLK_LESS		= 60,
	SDLK_EQUALS		= 61,
	SDLK_GREATER		= 62,
	SDLK_QUESTION		= 63,
	SDLK_AT			= 64,
	/*
	   Skip uppercase letters
	 */
	SDLK_LEFTBRACKET	= 91,
	SDLK_BACKSLASH		= 92,
	SDLK_RIGHTBRACKET	= 93,
	SDLK_CARET		= 94,
	SDLK_UNDERSCORE		= 95,
	SDLK_BACKQUOTE		= 96,
	SDLK_a			= 97,
	SDLK_b			= 98,
	SDLK_c			= 99,
	SDLK_d			= 100,
	SDLK_e			= 101,
	SDLK_f			= 102,
	SDLK_g			= 103,
	SDLK_h			= 104,
	SDLK_i			= 105,
	SDLK_j			= 106,
	SDLK_k			= 107,
	SDLK_l			= 108,
	SDLK_m			= 109,
	SDLK_n			= 110,
	SDLK_o			= 111,
	SDLK_p			= 112,
	SDLK_q			= 113,
	SDLK_r			= 114,
	SDLK_s			= 115,
	SDLK_t			= 116,
	SDLK_u			= 117,
	SDLK_v			= 118,
	SDLK_w			= 119,
	SDLK_x			= 120,
	SDLK_y			= 121,
	SDLK_z			= 122,
	SDLK_DELETE		= 127,
	/* End of ASCII mapped keysyms */

	/* International keyboard syms */
	SDLK_WORLD_0		= 160,		/* 0xA0 */
	SDLK_WORLD_1		= 161,
	SDLK_WORLD_2		= 162,
	SDLK_WORLD_3		= 163,
	SDLK_WORLD_4		= 164,
	SDLK_WORLD_5		= 165,
	SDLK_WORLD_6		= 166,
	SDLK_WORLD_7		= 167,
	SDLK_WORLD_8		= 168,
	SDLK_WORLD_9		= 169,
	SDLK_WORLD_10		= 170,
	SDLK_WORLD_11		= 171,
	SDLK_WORLD_12		= 172,
	SDLK_WORLD_13		= 173,
	SDLK_WORLD_14		= 174,
	SDLK_WORLD_15		= 175,
	SDLK_WORLD_16		= 176,
	SDLK_WORLD_17		= 177,
	SDLK_WORLD_18		= 178,
	SDLK_WORLD_19		= 179,
	SDLK_WORLD_20		= 180,
	SDLK_WORLD_21		= 181,
	SDLK_WORLD_22		= 182,
	SDLK_WORLD_23		= 183,
	SDLK_WORLD_24		= 184,
	SDLK_WORLD_25		= 185,
	SDLK_WORLD_26		= 186,
	SDLK_WORLD_27		= 187,
	SDLK_WORLD_28		= 188,
	SDLK_WORLD_29		= 189,
	SDLK_WORLD_30		= 190,
	SDLK_WORLD_31		= 191,
	SDLK_WORLD_32		= 192,
	SDLK_WORLD_33		= 193,
	SDLK_WORLD_34		= 194,
	SDLK_WORLD_35		= 195,
	SDLK_WORLD_36		= 196,
	SDLK_WORLD_37		= 197,
	SDLK_WORLD_38		= 198,
	SDLK_WORLD_39		= 199,
	SDLK_WORLD_40		= 200,
	SDLK_WORLD_41		= 201,
	SDLK_WORLD_42		= 202,
	SDLK_WORLD_43		= 203,
	SDLK_WORLD_44		= 204,
	SDLK_WORLD_45		= 205,
	SDLK_WORLD_46		= 206,
	SDLK_WORLD_47		= 207,
	SDLK_WORLD_48		= 208,
	SDLK_WORLD_49		= 209,
	SDLK_WORLD_50		= 210,
	SDLK_WORLD_51		= 211,
	SDLK_WORLD_52		= 212,
	SDLK_WORLD_53		= 213,
	SDLK_WORLD_54		= 214,
	SDLK_WORLD_55		= 215,
	SDLK_WORLD_56		= 216,
	SDLK_WORLD_57		= 217,
	SDLK_WORLD_58		= 218,
	SDLK_WORLD_59		= 219,
	SDLK_WORLD_60		= 220,
	SDLK_WORLD_61		= 221,
	SDLK_WORLD_62		= 222,
	SDLK_WORLD_63		= 223,
	SDLK_WORLD_64		= 224,
	SDLK_WORLD_65		= 225,
	SDLK_WORLD_66		= 226,
	SDLK_WORLD_67		= 227,
	SDLK_WORLD_68		= 228,
	SDLK_WORLD_69		= 229,
	SDLK_WORLD_70		= 230,
	SDLK_WORLD_71		= 231,
	SDLK_WORLD_72		= 232,
	SDLK_WORLD_73		= 233,
	SDLK_WORLD_74		= 234,
	SDLK_WORLD_75		= 235,
	SDLK_WORLD_76		= 236,
	SDLK_WORLD_77		= 237,
	SDLK_WORLD_78		= 238,
	SDLK_WORLD_79		= 239,
	SDLK_WORLD_80		= 240,
	SDLK_WORLD_81		= 241,
	SDLK_WORLD_82		= 242,
	SDLK_WORLD_83		= 243,
	SDLK_WORLD_84		= 244,
	SDLK_WORLD_85		= 245,
	SDLK_WORLD_86		= 246,
	SDLK_WORLD_87		= 247,
	SDLK_WORLD_88		= 248,
	SDLK_WORLD_89		= 249,
	SDLK_WORLD_90		= 250,
	SDLK_WORLD_91		= 251,
	SDLK_WORLD_92		= 252,
	SDLK_WORLD_93		= 253,
	SDLK_WORLD_94		= 254,
	SDLK_WORLD_95		= 255,		/* 0xFF */

	/* Numeric keypad */
	SDLK_KP0		= 256,
	SDLK_KP1		= 257,
	SDLK_KP2		= 258,
	SDLK_KP3		= 259,
	SDLK_KP4		= 260,
	SDLK_KP5		= 261,
	SDLK_KP6		= 262,
	SDLK_KP7		= 263,
	SDLK_KP8		= 264,
	SDLK_KP9		= 265,
	SDLK_KP_PERIOD		= 266,
	SDLK_KP_DIVIDE		= 267,
	SDLK_KP_MULTIPLY	= 268,
	SDLK_KP_MINUS		= 269,
	SDLK_KP_PLUS		= 270,
	SDLK_KP_ENTER		= 271,
	SDLK_KP_EQUALS		= 272,

	/* Arrows + Home/End pad */
	SDLK_UP			= 273,
	SDLK_DOWN		= 274,
	SDLK_RIGHT		= 275,
	SDLK_LEFT		= 276,
	SDLK_INSERT		= 277,
	SDLK_HOME		= 278,
	SDLK_END		= 279,
	SDLK_PAGEUP		= 280,
	SDLK_PAGEDOWN		= 281,

	/* Function keys */
	SDLK_F1			= 282,
	SDLK_F2			= 283,
	SDLK_F3			= 284,
	SDLK_F4			= 285,
	SDLK_F5			= 286,
	SDLK_F6			= 287,
	SDLK_F7			= 288,
	SDLK_F8			= 289,
	SDLK_F9			= 290,
	SDLK_F10		= 291,
	SDLK_F11		= 292,
	SDLK_F12		= 293,
	SDLK_F13		= 294,
	SDLK_F14		= 295,
	SDLK_F15		= 296,

	/* Key state modifier keys */
	SDLK_NUMLOCK		= 300,
	SDLK_CAPSLOCK		= 301,
	SDLK_SCROLLOCK		= 302,
	SDLK_RSHIFT		= 303,
	SDLK_LSHIFT		= 304,
	SDLK_RCTRL		= 305,
	SDLK_LCTRL		= 306,
	SDLK_RALT		= 307,
	SDLK_LALT		= 308,
	SDLK_RMETA		= 309,
	SDLK_LMETA		= 310,
	SDLK_LSUPER		= 311,		/* Left "Windows" key */
	SDLK_RSUPER		= 312,		/* Right "Windows" key */
	SDLK_MODE		= 313,		/* "Alt Gr" key */
	SDLK_COMPOSE		= 314,		/* Multi-key compose key */

	/* Miscellaneous function keys */
	SDLK_HELP		= 315,
	SDLK_PRINT		= 316,
	SDLK_SYSREQ		= 317,
	SDLK_BREAK		= 318,
	SDLK_MENU		= 319,
	SDLK_POWER		= 320,		/* Power Macintosh power key */
	SDLK_EURO		= 321,		/* Some european keyboards */
	SDLK_UNDO		= 322,		/* Atari keyboard has Undo */

	/* Add any other keys here */

	SDLK_LAST
} SDLKey;

/* Enumeration of valid key mods (possibly OR'd together) */
typedef enum {
	KMOD_NONE  = 0x0000,
	KMOD_LSHIFT= 0x0001,
	KMOD_RSHIFT= 0x0002,
	KMOD_LCTRL = 0x0040,
	KMOD_RCTRL = 0x0080,
	KMOD_LALT  = 0x0100,
	KMOD_RALT  = 0x0200,
	KMOD_LMETA = 0x0400,
	KMOD_RMETA = 0x0800,
	KMOD_NUM   = 0x1000,
	KMOD_CAPS  = 0x2000,
	KMOD_MODE  = 0x4000,
	KMOD_RESERVED = 0x8000
} SDLMod;

#define KMOD_CTRL	(KMOD_LCTRL|KMOD_RCTRL)
#define KMOD_SHIFT	(KMOD_LSHIFT|KMOD_RSHIFT)
#define KMOD_ALT	(KMOD_LALT|KMOD_RALT)
#define KMOD_META	(KMOD_LMETA|KMOD_RMETA)


moDirectorIODeviceManager::moDirectorIODeviceManager() : moIODeviceManager() {

}

moDirectorIODeviceManager::~moDirectorIODeviceManager() {

}

/*
type 	SDL_MOUSEBUTTONDOWN or SDL_MOUSEBUTTONUP
button 	The mouse button index(SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT, SDL_BUTTON_WHEELUP, SDL_BUTTON_WHEELDOWN)
state 	SDL_PRESSED or SDL_RELEASED
x, y 	The X/Y coordinates of the mouse at press/release time
*/
void
moDirectorIODeviceManager::OnMouseEvent( wxMouseEvent &event) {

	//translates wxMouseEvents to SDL mouse events type(internal events of Moldeo are based on SDL )
	//accumulate mouse events( to pass to event list later )


	//SDL_MOUSEMOTION

	//MOTION
	if( event.Moving() || event.Dragging() ) {
		GetEvents()->Add( MO_IODEVICE_MOUSE, SDL_MOUSEMOTION, event.m_x - m_MouseX/*rel*/, event.m_y - m_MouseY/*rel*/, event.m_x, event.m_y);
	}

	m_MouseX = event.m_x;
	m_MouseY = event.m_y;

	//MOUSEBUTTONDOWN
	if( event.LeftDown() ) {
		GetEvents()->Add(MO_IODEVICE_MOUSE,SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT , event.m_x, event.m_y);
	}
	if( event.MiddleDown() ) {
		GetEvents()->Add(MO_IODEVICE_MOUSE,SDL_MOUSEBUTTONDOWN, SDL_BUTTON_MIDDLE , event.m_x, event.m_y);
	}
	if( event.RightDown() ) {
		GetEvents()->Add(MO_IODEVICE_MOUSE,SDL_MOUSEBUTTONDOWN, SDL_BUTTON_RIGHT , event.m_x, event.m_y);
	}
	//MOUSEBUTTONUP
	if( event.LeftUp() ) {
		GetEvents()->Add(MO_IODEVICE_MOUSE,SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT , event.m_x, event.m_y);
	}
	if( event.MiddleUp() ) {
		GetEvents()->Add(MO_IODEVICE_MOUSE,SDL_MOUSEBUTTONUP, SDL_BUTTON_MIDDLE , event.m_x, event.m_y);
	}
	if( event.RightUp() ) {
		GetEvents()->Add(MO_IODEVICE_MOUSE,SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT , event.m_x, event.m_y);
	}
}
/*
type 	SDL_KEYDOWN or SDL_KEYUP
state 	SDL_PRESSED or SDL_RELEASED
keysym 	Contains key press information in a SDL_keysym structure

		scancode 	Hardware specific scancode
		sym 	SDL virtual keysym
		mod 	Current key modifiers
		unicode 	Translated character
mod
KMOD_NONE 	No modifiers applicable
KMOD_LSHIFT 	Left Shift is down
KMOD_RSHIFT 	Right Shift is down
KMOD_LCTRL 	Left Control is down
KMOD_RCTRL 	Right Control is down
KMOD_LALT 	Left Alt is down
KMOD_RALT 	Right Alt is down
KMOD_LMETA 	Left Meta is down
KMOD_RMETA 	Right Meta is down
KMOD_NUM 	Numlock is down
KMOD_CAPS 	Capslock is down
KMOD_MODE 	Mode is down
KMOD_CTRL 	A Control key is down
KMOD_SHIFT 	A Shift key is down
KMOD_ALT 	An Alt key is down
KMOD_META 	A Meta key is down
*/
void moDirectorIODeviceManager::OnKeyDown( wxKeyEvent &event ) {

	SDLKey sym = SDLK_UNKNOWN;
	SDLMod mod = KMOD_NONE;

	if(event.ShiftDown())
		mod =(SDLMod)((int)mod | KMOD_SHIFT );
	if(event.ControlDown())
		mod =(SDLMod)((int)mod | KMOD_CTRL );

	//event.key.keysym.sym
	if( event.GetKeyCode() == WXK_SHIFT ) {
		sym = SDLK_LSHIFT;
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
	} else
	if( event.GetKeyCode() == WXK_CONTROL ) {
		sym = SDLK_RCTRL;
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
	} else
	if((int)(SDLK_BACKSPACE)  <= event.GetKeyCode() && event.GetKeyCode() <=(int)(SDLK_DELETE) ) {
		if( 65<=event.GetKeyCode() && event.GetKeyCode()<=90) {
			sym = SDLKey((int)event.GetKeyCode() + 32);
		} else {
			sym =(SDLKey)(int)event.GetKeyCode();
		}
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
	} else
	if(WXK_F1  <= event.GetKeyCode() && event.GetKeyCode() <= WXK_F15 ) {
		sym = SDLKey((int)SDLK_F1 +((int)event.GetKeyCode() -(int)WXK_F1 ));
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
	} else
	if( WXK_NUMPAD0  <= event.GetKeyCode() && event.GetKeyCode() <= WXK_NUMPAD9 ) {
		sym = SDLKey((int)SDLK_KP0 +((int)event.GetKeyCode() -(int)WXK_NUMPAD0 ));
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
	} else
	switch(event.GetKeyCode()) {
		/*NUMPAD*/
		case WXK_NUMPAD_MULTIPLY:
			sym = SDLK_KP_MULTIPLY;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_DIVIDE:
			sym = SDLK_KP_DIVIDE;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_SUBTRACT:
			sym = SDLK_KP_MINUS;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_DECIMAL:
			sym = SDLK_KP_PERIOD;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_ADD:
			sym = SDLK_KP_PLUS;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_ENTER:
			sym = SDLK_KP_ENTER;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		/*SPECIALS*/
		case WXK_PAGEUP:
		case WXK_NUMPAD_PAGEUP:
			sym = SDLK_PAGEUP;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_PAGEDOWN:
		case WXK_NUMPAD_PAGEDOWN:
			sym = SDLK_PAGEDOWN;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_LEFT:
			sym = SDLK_LEFT;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_UP:
			sym = SDLK_UP;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_RIGHT:
			sym = SDLK_RIGHT;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_DOWN:
			sym = SDLK_DOWN;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		case WXK_NUMLOCK:
			sym = SDLK_NUMLOCK;
			mod = KMOD_NUM;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYDOWN, sym , mod ,0,0);
			break;
		default:
			break;
	}
}

void moDirectorIODeviceManager::OnKeyUp( wxKeyEvent &event ) {

	SDLKey sym = SDLK_UNKNOWN;
	SDLMod mod = KMOD_NONE;

	if(event.ShiftDown())
		mod =(SDLMod)((int)mod | KMOD_SHIFT );
	if(event.ControlDown())
		mod =(SDLMod)((int)mod | KMOD_CTRL );

	//event.key.keysym.sym
	if( event.GetKeyCode() == WXK_SHIFT ) {
		sym = SDLK_LSHIFT;
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
	} else
	if( event.GetKeyCode() == WXK_CONTROL ) {
		sym = SDLK_RCTRL;
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
	} else
	if((int)(SDLK_BACKSPACE)  <= event.GetKeyCode() && event.GetKeyCode() <=(int)(SDLK_DELETE) ) {
		if( 65<=event.GetKeyCode() && event.GetKeyCode()<=90) {
			sym = SDLKey((int)event.GetKeyCode() + 32);
		} else {
			sym =(SDLKey)(int)event.GetKeyCode();
		}
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
	} else
	if( WXK_F1  <= event.GetKeyCode() && event.GetKeyCode() <= WXK_F15 ) {
		sym = SDLKey((int)SDLK_F1 +((int)event.GetKeyCode() -(int)WXK_F1 ));
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
	} else
	if( WXK_NUMPAD0  <= event.GetKeyCode() && event.GetKeyCode() <= WXK_NUMPAD9 ) {
		sym = SDLKey((int)SDLK_KP0 +((int)event.GetKeyCode() -(int)WXK_NUMPAD0 ));
		GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
	} else
	switch(event.GetKeyCode()) {
		/*NUMPAD*/
		case WXK_NUMPAD_MULTIPLY:
			sym = SDLK_KP_MULTIPLY;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_DIVIDE:
			sym = SDLK_KP_DIVIDE;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_SUBTRACT:
			sym = SDLK_KP_MINUS;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_DECIMAL:
			sym = SDLK_KP_PERIOD;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_ADD:
			sym = SDLK_KP_PLUS;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_NUMPAD_ENTER:
			sym = SDLK_KP_ENTER;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		/*SPECIALS*/
		case WXK_PAGEUP:
		case WXK_NUMPAD_PAGEUP:
			sym = SDLK_PAGEUP;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_PAGEDOWN:
		case WXK_NUMPAD_PAGEDOWN:
			sym = SDLK_PAGEDOWN;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_LEFT:
			sym = SDLK_LEFT;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_UP:
			sym = SDLK_UP;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_RIGHT:
			sym = SDLK_RIGHT;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_DOWN:
			sym = SDLK_DOWN;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		case WXK_NUMLOCK:
			sym = SDLK_NUMLOCK;
			mod = KMOD_NUM;
			GetEvents()->Add( MO_IODEVICE_KEYBOARD, SDL_KEYUP, sym , mod ,0,0);
			break;
		default:
			break;
	}


}

void
moDirectorIODeviceManager::PollEvents() {

	//m_AuxEvents.First();

	//SDL_Event event;

	   //SDL_KEYDOWN,			// Keys pressed
       //SDL_KEYUP,				// Keys released
       //SDL_MOUSEMOTION,		// Mouse moved
       //SDL_MOUSEBUTTONDOWN,	// Mouse button pressed
       //SDL_MOUSEBUTTONUP,		// Mouse button released
/*
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
				case SDL_MOUSEMOTION:
					GetEvents()->Add(MO_IODEVICE_MOUSE,SDL_MOUSEMOTION, event.motion.xrel, event.motion.yrel, event.motion.x, event.motion.y);
					break;
				case SDL_MOUSEBUTTONDOWN:
					GetEvents()->Add(MO_IODEVICE_MOUSE,SDL_MOUSEBUTTONDOWN, event.button.button, event.button.x, event.button.y);
					break;
				case SDL_MOUSEBUTTONUP:
					GetEvents()->Add(MO_IODEVICE_MOUSE,SDL_MOUSEBUTTONUP, event.button.button, event.button.x, event.button.y);
					break;
				case SDL_KEYDOWN:
					// Ignore ALT-TAB for windows
					if((event.key.keysym.sym == SDLK_LALT) ||
						(event.key.keysym.sym == SDLK_RALT) ||
						(event.key.keysym.sym == SDLK_TAB)) {
						break;
					}
					GetEvents()->Add(MO_IODEVICE_KEYBOARD,SDL_KEYDOWN, event.key.keysym.sym, event.key.keysym.mod,0,0);
					break;

				case SDL_KEYUP:
					if((event.key.keysym.sym == SDLK_LALT) ||
						(event.key.keysym.sym == SDLK_RALT) ||
						(event.key.keysym.sym == SDLK_TAB)) {
						break;
					}
					GetEvents()->Add(MO_IODEVICE_KEYBOARD,SDL_KEYUP, event.key.keysym.sym, event.key.keysym.mod,0,0);
					break;
				default:
					break;
		}
	}
*/
}
