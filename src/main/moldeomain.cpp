/* Simple program:  Fill a colormap with gray and stripe it down the screen */

#include "moConsole.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*OSX*/
#ifdef MO_MACOSX
  #include "SDL.h"
  #include "SDL_thread.h"
  #include "SDL_mutex.h"
  #include "SDL_net.h"
  #include "SDL_image.h"
#endif

/*LINUX*/
#ifdef MO_LINUX
	#include <SDL/SDL.h>
	#include <SDL/SDL_thread.h>
	#include <SDL/SDL_mutex.h>
	#include <SDL/SDL_net.h>
	#include <SDL/SDL_image.h>
	#include <SDL/SDL_syswm.h>
#endif

/*WINDOWS*/
#ifdef MO_WIN32
	#include <SDL.h>
	#include <SDL_thread.h>
	#include <SDL_mutex.h>
	#include <SDL_syswm.h>
#endif


#ifdef MO_WIN32

#include "../../Build/vcres/resource.h"

static MO_HANDLE g_hwnd;
static MO_DISPLAY g_hdisp;

LRESULT CALLBACK graywinWindowProc(
  HWND hwnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam) {

   SDL_Event event;

   switch(uMsg) {
      case WM_COMMAND:
         if(LOWORD(wParam) == ID_QUIT) {
            event.type = SDL_QUIT;
            SDL_PushEvent(&event);
         }
         break;

      default:
         return DefWindowProc(hwnd, uMsg, wParam, lParam);
         break;
   }

   return 1;
}


int init_win32_window(HINSTANCE hInst, int width, int height) {

   char windowid[100];
   WNDCLASSEX wcex;
   RECT rect;

   ZeroMemory(&wcex, sizeof(wcex));
   wcex.cbSize = sizeof(wcex);
   wcex.style = CS_BYTEALIGNCLIENT;
   wcex.lpfnWndProc = graywinWindowProc;
   wcex.hInstance = hInst;
   wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
   wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON));
   //wcex.hbrBackground =(HBRUSH)GetStockObject(GRAY_BRUSH);
   wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
   wcex.lpszClassName = "Mainframe";

   if(!RegisterClassEx(&wcex)) {
      MessageBox(NULL, "could not register class", "Error", MB_ICONERROR);
      return 0;
   }

   ZeroMemory(&rect, sizeof(RECT));
   rect.right = width;
   rect.bottom = height;
   AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, TRUE, 0);

   g_hwnd = CreateWindowEx(
      0,
      "Mainframe",
      "Moldeo",
      WS_OVERLAPPEDWINDOW,
      0,
      0,
      abs((int)rect.left) + rect.right,
      abs((int)rect.top) + rect.bottom,
      NULL,
      LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU)),
      hInst,
      NULL);

   if(!g_hwnd) {
      MessageBox(NULL, "could not create window", "Error", MB_ICONERROR);
      return 0;
   }

   //ShowWindow(g_hwnd, SW_NORMAL);
   //UpdateWindow(g_hwnd);

   strcpy(windowid, "SDL_WINDOWID=");
   _ltoa((long)g_hwnd, windowid+13, 10);
   _putenv(windowid);

   return 1;
}
#endif /* WIN32 */

#ifdef MO_LINUX

static MO_HANDLE g_hwnd;
static MO_DISPLAY g_hdisp;

#endif

int main(int argc, char *argv[])
{
	//SDL_Surfac/home/andres/devel/moldeo/svn/trunk/0.7/data/test/tablet.cfge *screen;
	SDL_SysWMinfo info;
	Uint32 videoflags;
	Uint16 screen_width, screen_height;
	Uint16 render_width, render_height;
	Uint16 render_to_fbo;
	Uint16 i;
    Uint8 bpp;
	moConsole *MoldeoSession;
	moText data;
	moText config;

	data = moText("../../data/test");
	config = moText("../../data/test/test.mol");



	// Default parameters.
	screen_width = MO_DEF_SCREEN_WIDTH;
	screen_height = MO_DEF_SCREEN_HEIGHT;
	render_width = MO_DEF_RENDER_WIDTH;
	render_height = MO_DEF_RENDER_HEIGHT;
	render_to_fbo = 0;
	bpp = MO_DEF_SCREEN_DEPTH;
	videoflags = SDL_OPENGL | SDL_INIT_TIMER;

	while( argc > 1 ) {
		--argc;
		if( argv[argc-1] &&(strcmp(argv[argc-1], "-swidth") == 0) ) {
			screen_width = atoi(argv[argc]);
			--argc;
		} else
		if( argv[argc-1] &&(strcmp(argv[argc-1], "-sheight") == 0) ) {
			screen_height = atoi(argv[argc]);
			--argc;
		} else
		if( argv[argc-1] &&(strcmp(argv[argc-1], "-rwidth") == 0) ) {
			render_width = atoi(argv[argc]);
			--argc;
		} else
		if( argv[argc-1] &&(strcmp(argv[argc-1], "-rheight") == 0) ) {
			render_height = atoi(argv[argc]);
			--argc;
		} else
		if( argv[argc-1] &&(strcmp(argv[argc-1], "-bpp") == 0) ) {
			bpp = atoi(argv[argc]);
			--argc;
		} else
		if( argv[argc] &&(strcmp(argv[argc], "-fullscreen") == 0) ) {
			videoflags ^= SDL_FULLSCREEN;
		} else
		if( argv[argc] &&(strcmp(argv[argc], "-render2tex") == 0) ) {
			render_to_fbo = 1;
		} else
		if( argv[argc-1] &&(strcmp(argv[argc-1], "-data") == 0) ) {
			data = argv[argc];
			--argc;
		} else if( argv[argc-1] &&(strcmp(argv[argc-1], "-mol") == 0) ) {
			config = argv[argc];
			--argc;
		} else {
			fprintf(stderr, "Usage: %s  [-data] [-mol] [-windowed] [-swidth] [-sheight] [-rwidth] [-rheight] [-render2tex]\n", argv[0]);
			exit(1);
		}
	}

#  ifdef MO_USING_VC
   init_win32_window(GetModuleHandle(NULL), screen_width, screen_height);
#  endif

   /* Initialize SDL */
   /*
   SDL_GL_SetAttribute
typedef enum {
    SDL_GL_RED_SIZE,
    SDL_GL_GREEN_SIZE,
    SDL_GL_BLUE_SIZE,
    SDL_GL_ALPHA_SIZE,
    SDL_GL_BUFFER_SIZE,
    SDL_GL_DOUBLEBUFFER,
    SDL_GL_DEPTH_SIZE,
    SDL_GL_STENCIL_SIZE,
    SDL_GL_ACCUM_RED_SIZE,
    SDL_GL_ACCUM_GREEN_SIZE,
    SDL_GL_ACCUM_BLUE_SIZE,
    SDL_GL_ACCUM_ALPHA_SIZE,
    SDL_GL_STEREO,
    SDL_GL_MULTISAMPLEBUFFERS,
    SDL_GL_MULTISAMPLESAMPLES
} SDL_GLattr;
*/
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}

	/* Set a video mode */
	if ( SDL_SetVideoMode( screen_width, screen_height, bpp, videoflags ) == 0 ) {
        /*
         * This could happen for a variety of reasons,
         * including DISPLAY not being set, the specified
         * resolution not being available, etc.
         */
        fprintf( stderr, "Video mode set failed: %s\n",
             SDL_GetError( ) );
        exit(2);
    }

	printf("%i joysticks were found.\n\n", SDL_NumJoysticks() );
        if (0 < SDL_NumJoysticks())
        {
            printf("The names of the joysticks are:\n");
            for( i=0; i < SDL_NumJoysticks(); i++ ) printf("    %s\n", SDL_JoystickName(i));
        }

	/*Empieza la Lola!!*/
	SDL_ShowCursor(0);
	SDL_WM_GrabInput(SDL_GRAB_ON);

    #ifdef MO_DIRECTSHOW
	    CoInitialize(NULL);
    #endif

    if (SDL_GetWMInfo(&info)) {

        #ifdef MO_WIN32
        g_hwnd = info.window;
        g_hdisp = NULL;
        #else
        g_hwnd = NULL;
        g_hdisp = NULL;


        if ( info.subsystem == SDL_SYSWM_X11 ) {
            //Window u1; int u2; unsigned int u3;
            Display* display;
            Window window;

            info.info.x11.lock_func();
            display = info.info.x11.display;
            window = info.info.x11.window;

/*
            XGetGeometry(display, window, &u1, &u2, &u2,
                         (unsigned int *)width,
                         (unsigned int *)height, &u3, &u3);
*/
            info.info.x11.unlock_func();

            g_hwnd = window;
            g_hdisp = display;

        }
        #endif

    }

	MoldeoSession = new moConsole();

	if ( MoldeoSession->Init( data, config, NULL, NULL,
		                render_to_fbo,
		                screen_width, screen_height,
						render_width, render_height,
						(MO_HANDLE)g_hwnd , (MO_DISPLAY)g_hdisp )) {

		while(!MoldeoSession->Interaction()) {
			MoldeoSession->Update();
			MoldeoSession->Draw();
		}
	}

	MoldeoSession->Finish();

	delete MoldeoSession;

    #ifdef MO_DIRECTSHOW
        CoUninitialize();
    #endif

	SDL_Quit();
	return(0);
}
