// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h


#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_

#include "CIrrDeviceSDL.h"
#include "IEventReceiver.h"
#include "IGUIElement.h"
#include "IGUIEnvironment.h"
#include "IImageLoader.h"
#include "IFileSystem.h"
#include "os.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include "COSOperator.h"
#include <stdio.h>
#include <stdlib.h>
#include "SIrrCreationParameters.h"
#include <SDL_video.h>

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
#include <emscripten.h>
#endif

#include "CSDLManager.h"

static int SDLDeviceInstances = 0;

namespace irr
{
	namespace video
	{
		#ifdef _IRR_COMPILE_WITH_OPENGL_
		IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
		#else
		static IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
		{
			os::Printer::log("No OpenGL support compiled in.", ELL_ERROR);
			return nullptr;
		}
		#endif

		#ifdef ENABLE_OPENGL3
		IVideoDriver* createOpenGL3Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
		#else
		static IVideoDriver* createOpenGL3Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
		{
			os::Printer::log("No OpenGL 3 support compiled in.", ELL_ERROR);
			return nullptr;
		}
		#endif

		#ifdef _IRR_COMPILE_WITH_OGLES2_
		IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
		#else
		static IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
		{
			os::Printer::log("No OpenGL ES 2 support compiled in.", ELL_ERROR);
			return nullptr;
		}
		#endif

		#ifdef _IRR_COMPILE_WITH_WEBGL1_
		IVideoDriver* createWebGL1Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
		#else
		static IVideoDriver* createWebGL1Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
		{
			os::Printer::log("No WebGL 1 support compiled in.", ELL_ERROR);
			return nullptr;
		}
		#endif
	} // end namespace video

} // end namespace irr


namespace irr
{
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
EM_BOOL CIrrDeviceSDL::MouseUpDownCallback(int eventType, const EmscriptenMouseEvent * event, void* userData)
{
	// We need this callback so far only because otherwise "emscripten_request_pointerlock" calls will
	// fail as their request are infinitely deferred.
	// Not exactly certain why, maybe SDL does catch those mouse-events otherwise and not pass them on.
	return EM_FALSE;
}

EM_BOOL CIrrDeviceSDL::MouseEnterCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
	CIrrDeviceSDL * This = static_cast<CIrrDeviceSDL*>(userData);

	SEvent irrevent;

	irrevent.EventType = irr::EET_MOUSE_INPUT_EVENT;
	irrevent.MouseInput.Event = irr::EMIE_MOUSE_ENTER_CANVAS;
	This->MouseX = irrevent.MouseInput.X = mouseEvent->canvasX;
	This->MouseY = irrevent.MouseInput.Y = mouseEvent->canvasY;
	This->MouseXRel = mouseEvent->movementX; // should be 0 I guess? Or can it enter while pointer is locked()?
	This->MouseYRel = mouseEvent->movementY;
	irrevent.MouseInput.ButtonStates = This->MouseButtonStates;	// TODO: not correct, but couldn't figure out the bitset of mouseEvent->buttons yet.
	irrevent.MouseInput.Shift = mouseEvent->shiftKey;
	irrevent.MouseInput.Control = mouseEvent->ctrlKey;

	This->postEventFromUser(irrevent);

	return EM_FALSE;
}

EM_BOOL CIrrDeviceSDL::MouseLeaveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
	CIrrDeviceSDL * This = static_cast<CIrrDeviceSDL*>(userData);

	SEvent irrevent;

	irrevent.EventType = irr::EET_MOUSE_INPUT_EVENT;
	irrevent.MouseInput.Event = irr::EMIE_MOUSE_LEAVE_CANVAS;
	This->MouseX = irrevent.MouseInput.X = mouseEvent->canvasX;
	This->MouseY = irrevent.MouseInput.Y = mouseEvent->canvasY;
	This->MouseXRel = mouseEvent->movementX; // should be 0 I guess? Or can it enter while pointer is locked()?
	This->MouseYRel = mouseEvent->movementY;
	irrevent.MouseInput.ButtonStates = This->MouseButtonStates;	// TODO: not correct, but couldn't figure out the bitset of mouseEvent->buttons yet.
	irrevent.MouseInput.Shift = mouseEvent->shiftKey;
	irrevent.MouseInput.Control = mouseEvent->ctrlKey;

	This->postEventFromUser(irrevent);

	return EM_FALSE;
}
#endif


bool CIrrDeviceSDL::keyIsKnownSpecial(EKEY_CODE key)
{
	switch ( key )
	{
		// keys which are known to have safe special character interpretation
		// could need changes over time (removals and additions!)
		case KEY_RETURN:
		case KEY_PAUSE:
		case KEY_ESCAPE:
		case KEY_PRIOR:
		case KEY_NEXT:
		case KEY_HOME:
		case KEY_END:
		case KEY_LEFT:
		case KEY_UP:
		case KEY_RIGHT:
		case KEY_DOWN:
		case KEY_TAB:
		case KEY_PRINT:
		case KEY_SNAPSHOT:
		case KEY_INSERT:
		case KEY_BACK:
		case KEY_DELETE:
		case KEY_HELP:
		case KEY_APPS:
		case KEY_SLEEP:
		case KEY_F1:
		case KEY_F2:
		case KEY_F3:
		case KEY_F4:
		case KEY_F5:
		case KEY_F6:
		case KEY_F7:
		case KEY_F8:
		case KEY_F9:
		case KEY_F10:
		case KEY_F11:
		case KEY_F12:
		case KEY_F13:
		case KEY_F14:
		case KEY_F15:
		case KEY_F16:
		case KEY_F17:
		case KEY_F18:
		case KEY_F19:
		case KEY_F20:
		case KEY_F21:
		case KEY_F22:
		case KEY_F23:
		case KEY_F24:
		case KEY_NUMLOCK:
		case KEY_SCROLL:
		case KEY_LCONTROL:
		case KEY_RCONTROL:
			return true;

		default:
			return false;
	}
}

int CIrrDeviceSDL::findCharToPassToIrrlicht(int assumedChar, EKEY_CODE key) {
	// SDL in-place ORs values with no character representation with 1<<30
	// https://wiki.libsdl.org/SDL2/SDLKeycodeLookup
	if (assumedChar & (1<<30))
		return 0;

	switch (key) {
		case KEY_PRIOR:
		case KEY_NEXT:
		case KEY_HOME:
		case KEY_END:
		case KEY_LEFT:
		case KEY_UP:
		case KEY_RIGHT:
		case KEY_DOWN:
		case KEY_NUMLOCK:
			return 0;
		default:
			return assumedChar;
	}
}

void CIrrDeviceSDL::resetReceiveTextInputEvents() {
	gui::IGUIElement *elem = GUIEnvironment->getFocus();
	if (elem && elem->acceptsIME())
		SDL_StartTextInput();
	else
		SDL_StopTextInput();
}

//! constructor
CIrrDeviceSDL::CIrrDeviceSDL(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param),
	Window((SDL_Window*)param.WindowId), SDL_Flags(0),
	MouseX(0), MouseY(0), MouseXRel(0), MouseYRel(0), MouseButtonStates(0),
	Width(param.WindowSize.Width), Height(param.WindowSize.Height),
	Resizable(param.WindowResizable == 1 ? true : false)
{
	#ifdef _DEBUG
	setDebugName("CIrrDeviceSDL");
	#endif

	if ( ++SDLDeviceInstances == 1 )
	{
		u32 flags = SDL_INIT_TIMER | SDL_INIT_EVENTS;
		if (CreationParams.DriverType != video::EDT_NULL)
			flags |= SDL_INIT_VIDEO;
#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
		flags |= SDL_INIT_JOYSTICK;
#endif
		if (SDL_Init(flags) < 0)
		{
			os::Printer::log("Unable to initialize SDL", SDL_GetError(), ELL_ERROR);
			Close = true;
		}
		else
		{
			os::Printer::log("SDL initialized", ELL_INFORMATION);
		}
	}

	// create keymap
	createKeyMap();

	// create window
	if (CreationParams.DriverType != video::EDT_NULL)
	{
		// create the window, only if we do not use the null device
		createWindow();
	}


	SDL_VERSION(&Info.version);

#ifndef _IRR_EMSCRIPTEN_PLATFORM_
	SDL_GetWindowWMInfo(Window,&Info);
#endif //_IRR_EMSCRIPTEN_PLATFORM_
	core::stringc sdlversion = "SDL Version ";
	sdlversion += Info.version.major;
	sdlversion += ".";
	sdlversion += Info.version.minor;
	sdlversion += ".";
	sdlversion += Info.version.patch;

	Operator = new COSOperator(sdlversion);
	if (SDLDeviceInstances == 1) {
		os::Printer::log(sdlversion.c_str(), ELL_INFORMATION);
	}

	// create cursor control
	CursorControl = new CCursorControl(this);

	// create driver
	createDriver();

	if (VideoDriver) {
		createGUIAndScene();
		VideoDriver->OnResize(core::dimension2d<u32>(Width, Height));
	}
}


//! destructor
CIrrDeviceSDL::~CIrrDeviceSDL()
{
	if ( --SDLDeviceInstances == 0 )
	{
#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
		const u32 numJoysticks = Joysticks.size();
		for (u32 i=0; i<numJoysticks; ++i)
			SDL_JoystickClose(Joysticks[i]);
#endif
		if (Window)
		{
			SDL_GL_MakeCurrent(Window, NULL);
			SDL_GL_DeleteContext(Context);
			SDL_DestroyWindow(Window);
		}
		SDL_Quit();

		os::Printer::log("Quit SDL", ELL_INFORMATION);
	}
}

void CIrrDeviceSDL::logAttributes()
{
	core::stringc sdl_attr("SDL attribs:");
	int value = 0;
	if ( SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &value ) == 0 )
		sdl_attr += core::stringc(" r:") + core::stringc(value);
	if ( SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &value ) == 0 )
		sdl_attr += core::stringc(" g:") + core::stringc(value);
	if ( SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &value ) == 0 )
		sdl_attr += core::stringc(" b:") + core::stringc(value);
	if ( SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &value ) == 0 )
		sdl_attr += core::stringc(" a:") + core::stringc(value);

	if ( SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &value) == 0 )
		sdl_attr += core::stringc(" depth:") + core::stringc(value);
	if ( SDL_GL_GetAttribute( SDL_GL_STENCIL_SIZE, &value ) == 0 )
		sdl_attr += core::stringc(" stencil:") + core::stringc(value);
	if ( SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &value ) == 0 )
		sdl_attr += core::stringc(" doublebuf:") + core::stringc(value);
	if ( SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &value ) == 0 )
		sdl_attr += core::stringc(" aa:") + core::stringc(value);
	if ( SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &value ) == 0 )
		sdl_attr += core::stringc(" aa-samples:") + core::stringc(value);

	os::Printer::log(sdl_attr.c_str());
}

bool CIrrDeviceSDL::createWindow()
{
	if (CreationParams.Fullscreen) {
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
		SDL_Flags |= SDL_WINDOW_FULLSCREEN;
#else
		SDL_Flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif
	}
	if (Resizable)
		SDL_Flags |= SDL_WINDOW_RESIZABLE;
	if (CreationParams.WindowMaximized)
		SDL_Flags |= SDL_WINDOW_MAXIMIZED;
	SDL_Flags |= SDL_WINDOW_OPENGL;

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	if ( Width != 0 || Height != 0 )
		emscripten_set_canvas_size( Width, Height);
	else
	{
		int w, h, fs;
		emscripten_get_canvas_size(&w, &h, &fs);
		Width = w;
		Height = h;
	}

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, CreationParams.WithAlphaChannel?8:0 );

	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, CreationParams.ZBufferBits);
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, CreationParams.Stencilbuffer ? 8 : 0);
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, CreationParams.Doublebuffer ? 1 : 0);

	if (CreationParams.AntiAlias>1)
	{
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, CreationParams.AntiAlias );
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	}

	SDL_CreateWindowAndRenderer(0, 0, SDL_Flags, &Window, &Renderer); // 0,0 will use the canvas size

	logAttributes();

	// "#canvas" is for the opengl context
	emscripten_set_mousedown_callback("#canvas", (void*)this, true, MouseUpDownCallback);
    emscripten_set_mouseup_callback("#canvas", (void*)this, true, MouseUpDownCallback);
    emscripten_set_mouseenter_callback("#canvas", (void*)this, false, MouseEnterCallback);
    emscripten_set_mouseleave_callback("#canvas", (void*)this, false, MouseLeaveCallback);

	return true;
#else // !_IRR_EMSCRIPTEN_PLATFORM_
	if ( Close )
		return false;

	switch (CreationParams.DriverType) {
		case video::EDT_OPENGL:
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
			break;
		case video::EDT_OPENGL3:
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
			break;
		case video::EDT_OGLES1:
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			break;
		case video::EDT_OGLES2:
		case video::EDT_WEBGL1:
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			break;
		default:;
	}

#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG);
#endif

	if (CreationParams.Bits == 16) {
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, CreationParams.WithAlphaChannel ? 1 : 0);
	} else {
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, CreationParams.WithAlphaChannel ? 8 : 0);
	}
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, CreationParams.ZBufferBits);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, CreationParams.Doublebuffer);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, CreationParams.Stencilbuffer ? 8 : 0);
	SDL_GL_SetAttribute(SDL_GL_STEREO, CreationParams.Stereobuffer);
	if (CreationParams.AntiAlias > 1) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, CreationParams.AntiAlias);
	}
	if (!Window)
		Window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_Flags);
	if (!Window) {
		os::Printer::log("Could not create window...", SDL_GetError(), ELL_WARNING);
	}
	if (!Window && CreationParams.AntiAlias > 1) {
		while (--CreationParams.AntiAlias > 1) {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, CreationParams.AntiAlias);
			Window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_Flags);
			if (Window)
				break;
		}
		if (!Window) {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
			Window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_Flags);
			if (Window)
				os::Printer::log("AntiAliasing disabled due to lack of support!", ELL_WARNING);
		}
	}

	if ( !Window && CreationParams.Doublebuffer)
	{
		// Try single buffer
		if (CreationParams.DriverType == video::EDT_OPENGL)
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		Window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_Flags);
	}
	if ( !Window )
	{
		os::Printer::log("Could not initialize display", SDL_GetError(), ELL_ERROR);
		return false;
	}

	Context = SDL_GL_CreateContext(Window);
	if (!Context) {
		os::Printer::log("Could not initialize context", SDL_GetError(), ELL_ERROR);
		SDL_DestroyWindow(Window);
		return false;
	}

	return true;
#endif // !_IRR_EMSCRIPTEN_PLATFORM_
}


//! create the driver
void CIrrDeviceSDL::createDriver()
{
	if (CreationParams.DriverType == video::EDT_NULL) {
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		return;
	}

	ContextManager = new video::CSDLManager(this);
	switch(CreationParams.DriverType)
	{
	case video::EDT_OPENGL: VideoDriver = video::createOpenGLDriver(CreationParams, FileSystem, ContextManager); break;
	case video::EDT_OPENGL3: VideoDriver = video::createOpenGL3Driver(CreationParams, FileSystem, ContextManager); break;
	case video::EDT_OGLES2: VideoDriver = video::createOGLES2Driver(CreationParams, FileSystem, ContextManager); break;
	case video::EDT_WEBGL1: VideoDriver = video::createWebGL1Driver(CreationParams, FileSystem, ContextManager); break;
	default:;
	}
	if (!VideoDriver)
		os::Printer::log("Could not create video driver", ELL_ERROR);
}


//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceSDL::run()
{
	os::Timer::tick();

	SEvent irrevent;
	SDL_Event SDL_event;

	while ( !Close && SDL_PollEvent( &SDL_event ) )
	{
		// os::Printer::log("event: ", core::stringc((int)SDL_event.type).c_str(),   ELL_INFORMATION);	// just for debugging

		switch ( SDL_event.type )
		{
		case SDL_MOUSEMOTION: {
			SDL_Keymod keymod = SDL_GetModState();

			irrevent.EventType = irr::EET_MOUSE_INPUT_EVENT;
			irrevent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
			MouseX = irrevent.MouseInput.X = SDL_event.motion.x;
			MouseY = irrevent.MouseInput.Y = SDL_event.motion.y;
			MouseXRel = SDL_event.motion.xrel;
			MouseYRel = SDL_event.motion.yrel;
			irrevent.MouseInput.ButtonStates = MouseButtonStates;
			irrevent.MouseInput.Shift = (keymod & KMOD_SHIFT) != 0;
			irrevent.MouseInput.Control = (keymod & KMOD_CTRL) != 0;

			postEventFromUser(irrevent);
			break;
		}
		case SDL_MOUSEWHEEL: {
			SDL_Keymod keymod = SDL_GetModState();

			irrevent.EventType = irr::EET_MOUSE_INPUT_EVENT;
			irrevent.MouseInput.Event = irr::EMIE_MOUSE_WHEEL;
			irrevent.MouseInput.Wheel = SDL_event.wheel.preciseY;
			irrevent.MouseInput.Shift = (keymod & KMOD_SHIFT) != 0;
			irrevent.MouseInput.Control = (keymod & KMOD_CTRL) != 0;
			irrevent.MouseInput.X = MouseX;
			irrevent.MouseInput.Y = MouseY;

			postEventFromUser(irrevent);
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP: {
			SDL_Keymod keymod = SDL_GetModState();

			irrevent.EventType = irr::EET_MOUSE_INPUT_EVENT;
			irrevent.MouseInput.X = SDL_event.button.x;
			irrevent.MouseInput.Y = SDL_event.button.y;
			irrevent.MouseInput.Shift = (keymod & KMOD_SHIFT) != 0;
			irrevent.MouseInput.Control = (keymod & KMOD_CTRL) != 0;

			irrevent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;


#ifdef _IRR_EMSCRIPTEN_PLATFORM_
			// Handle mouselocking in emscripten in Windowed mode.
			// In fullscreen SDL will handle it.
			// The behavior we want windowed is - when the canvas was clicked then
			// we will lock the mouse-pointer if it should be invisible.
			// For security reasons this will be delayed until the next mouse-up event.
			// We do not pass on this event as we don't want the activation click to do anything.
			if ( SDL_event.type == SDL_MOUSEBUTTONDOWN && !isFullscreen() )
			{
				EmscriptenPointerlockChangeEvent pointerlockStatus; // let's hope that test is not expensive ...
				if ( emscripten_get_pointerlock_status(&pointerlockStatus) == EMSCRIPTEN_RESULT_SUCCESS )
				{
					if ( CursorControl->isVisible() && pointerlockStatus.isActive )
					{
						emscripten_exit_pointerlock();
						return !Close;
					}
					else if ( !CursorControl->isVisible() && !pointerlockStatus.isActive )
					{
						emscripten_request_pointerlock(0, true);
						return !Close;
					}
				}
			}
#endif

			switch(SDL_event.button.button)
			{
			case SDL_BUTTON_LEFT:
				if (SDL_event.type == SDL_MOUSEBUTTONDOWN)
				{
					irrevent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
					MouseButtonStates |= irr::EMBSM_LEFT;
				}
				else
				{
					irrevent.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
					MouseButtonStates &= ~irr::EMBSM_LEFT;
				}
				break;

			case SDL_BUTTON_RIGHT:
				if (SDL_event.type == SDL_MOUSEBUTTONDOWN)
				{
					irrevent.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
					MouseButtonStates |= irr::EMBSM_RIGHT;
				}
				else
				{
					irrevent.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
					MouseButtonStates &= ~irr::EMBSM_RIGHT;
				}
				break;

			case SDL_BUTTON_MIDDLE:
				if (SDL_event.type == SDL_MOUSEBUTTONDOWN)
				{
					irrevent.MouseInput.Event = irr::EMIE_MMOUSE_PRESSED_DOWN;
					MouseButtonStates |= irr::EMBSM_MIDDLE;
				}
				else
				{
					irrevent.MouseInput.Event = irr::EMIE_MMOUSE_LEFT_UP;
					MouseButtonStates &= ~irr::EMBSM_MIDDLE;
				}
				break;
			}

			irrevent.MouseInput.ButtonStates = MouseButtonStates;

			if (irrevent.MouseInput.Event != irr::EMIE_MOUSE_MOVED)
			{
				postEventFromUser(irrevent);

				if ( irrevent.MouseInput.Event >= EMIE_LMOUSE_PRESSED_DOWN && irrevent.MouseInput.Event <= EMIE_MMOUSE_PRESSED_DOWN )
				{
					u32 clicks = checkSuccessiveClicks(irrevent.MouseInput.X, irrevent.MouseInput.Y, irrevent.MouseInput.Event);
					if ( clicks == 2 )
					{
						irrevent.MouseInput.Event = (EMOUSE_INPUT_EVENT)(EMIE_LMOUSE_DOUBLE_CLICK + irrevent.MouseInput.Event-EMIE_LMOUSE_PRESSED_DOWN);
						postEventFromUser(irrevent);
					}
					else if ( clicks == 3 )
					{
						irrevent.MouseInput.Event = (EMOUSE_INPUT_EVENT)(EMIE_LMOUSE_TRIPLE_CLICK + irrevent.MouseInput.Event-EMIE_LMOUSE_PRESSED_DOWN);
						postEventFromUser(irrevent);
					}
				}
			}
			break;
		}

		case SDL_TEXTINPUT:
			{
				irrevent.EventType = irr::EET_STRING_INPUT_EVENT;
				irrevent.StringInput.Str = new core::stringw();
				irr::core::utf8ToWString(*irrevent.StringInput.Str, SDL_event.text.text);
				postEventFromUser(irrevent);
				delete irrevent.StringInput.Str;
				irrevent.StringInput.Str = NULL;
			}
			break;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			{
				SKeyMap mp;
				mp.SDLKey = SDL_event.key.keysym.sym;
				s32 idx = KeyMap.binary_search(mp);

				EKEY_CODE key;
				if (idx == -1)
					key = (EKEY_CODE)0;
				else
					key = (EKEY_CODE)KeyMap[idx].Win32Key;

				// Make sure to only input special characters if something is in focus, as SDL_TEXTINPUT handles normal unicode already
				if (SDL_IsTextInputActive() && !keyIsKnownSpecial(key) && (SDL_event.key.keysym.mod & KMOD_CTRL) == 0)
					break;

#ifdef _IRR_WINDOWS_API_
				// handle alt+f4 in Windows, because SDL seems not to
				if ( (SDL_event.key.keysym.mod & KMOD_LALT) && key == KEY_F4)
				{
					Close = true;
					break;
				}
#endif
				irrevent.EventType = irr::EET_KEY_INPUT_EVENT;
				irrevent.KeyInput.Key = key;
				irrevent.KeyInput.PressedDown = (SDL_event.type == SDL_KEYDOWN);
				irrevent.KeyInput.Shift = (SDL_event.key.keysym.mod & KMOD_SHIFT) != 0;
				irrevent.KeyInput.Control = (SDL_event.key.keysym.mod & KMOD_CTRL ) != 0;
				irrevent.KeyInput.Char = findCharToPassToIrrlicht(mp.SDLKey, key);
				postEventFromUser(irrevent);
			}
			break;

		case SDL_QUIT:
			Close = true;
			break;

		case SDL_WINDOWEVENT:
			switch (SDL_event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				if ((SDL_event.window.data1 != (int)Width) || (SDL_event.window.data2 != (int)Height))
				{
					Width = SDL_event.window.data1;
					Height = SDL_event.window.data2;
					if (VideoDriver)
						VideoDriver->OnResize(core::dimension2d<u32>(Width, Height));
				}
				break;
			}

		case SDL_USEREVENT:
			irrevent.EventType = irr::EET_USER_EVENT;
			irrevent.UserEvent.UserData1 = reinterpret_cast<uintptr_t>(SDL_event.user.data1);
			irrevent.UserEvent.UserData2 = reinterpret_cast<uintptr_t>(SDL_event.user.data2);

			postEventFromUser(irrevent);
			break;

		default:
			break;
		} // end switch
	resetReceiveTextInputEvents();
	} // end while

#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	// TODO: Check if the multiple open/close calls are too expensive, then
	// open/close in the constructor/destructor instead

	// update joystick states manually
	SDL_JoystickUpdate();
	// we'll always send joystick input events...
	SEvent joyevent;
	joyevent.EventType = EET_JOYSTICK_INPUT_EVENT;
	for (u32 i=0; i<Joysticks.size(); ++i)
	{
		SDL_Joystick* joystick = Joysticks[i];
		if (joystick)
		{
			int j;
			// query all buttons
			const int numButtons = core::min_(SDL_JoystickNumButtons(joystick), 32);
			joyevent.JoystickEvent.ButtonStates=0;
			for (j=0; j<numButtons; ++j)
				joyevent.JoystickEvent.ButtonStates |= (SDL_JoystickGetButton(joystick, j)<<j);

			// query all axes, already in correct range
			const int numAxes = core::min_(SDL_JoystickNumAxes(joystick), (int)SEvent::SJoystickEvent::NUMBER_OF_AXES);
			joyevent.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_X]=0;
			joyevent.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Y]=0;
			joyevent.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_Z]=0;
			joyevent.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_R]=0;
			joyevent.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_U]=0;
			joyevent.JoystickEvent.Axis[SEvent::SJoystickEvent::AXIS_V]=0;
			for (j=0; j<numAxes; ++j)
				joyevent.JoystickEvent.Axis[j] = SDL_JoystickGetAxis(joystick, j);

			// we can only query one hat, SDL only supports 8 directions
			if (SDL_JoystickNumHats(joystick)>0)
			{
				switch (SDL_JoystickGetHat(joystick, 0))
				{
					case SDL_HAT_UP:
						joyevent.JoystickEvent.POV=0;
						break;
					case SDL_HAT_RIGHTUP:
						joyevent.JoystickEvent.POV=4500;
						break;
					case SDL_HAT_RIGHT:
						joyevent.JoystickEvent.POV=9000;
						break;
					case SDL_HAT_RIGHTDOWN:
						joyevent.JoystickEvent.POV=13500;
						break;
					case SDL_HAT_DOWN:
						joyevent.JoystickEvent.POV=18000;
						break;
					case SDL_HAT_LEFTDOWN:
						joyevent.JoystickEvent.POV=22500;
						break;
					case SDL_HAT_LEFT:
						joyevent.JoystickEvent.POV=27000;
						break;
					case SDL_HAT_LEFTUP:
						joyevent.JoystickEvent.POV=31500;
						break;
					case SDL_HAT_CENTERED:
					default:
						joyevent.JoystickEvent.POV=65535;
						break;
				}
			}
			else
			{
				joyevent.JoystickEvent.POV=65535;
			}

			// we map the number directly
			joyevent.JoystickEvent.Joystick=static_cast<u8>(i);
			// now post the event
			postEventFromUser(joyevent);
			// and close the joystick
		}
	}
#endif
	return !Close;
}

//! Activate any joysticks, and generate events for them.
bool CIrrDeviceSDL::activateJoysticks(core::array<SJoystickInfo> & joystickInfo)
{
#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	joystickInfo.clear();

	// we can name up to 256 different joysticks
	const int numJoysticks = core::min_(SDL_NumJoysticks(), 256);
	Joysticks.reallocate(numJoysticks);
	joystickInfo.reallocate(numJoysticks);

	int joystick = 0;
	for (; joystick<numJoysticks; ++joystick)
	{
		Joysticks.push_back( SDL_JoystickOpen(joystick));
		SJoystickInfo info;

		info.Joystick = joystick;
		info.Axes = SDL_JoystickNumAxes(Joysticks[joystick]);
		info.Buttons = SDL_JoystickNumButtons(Joysticks[joystick]);
		info.Name = SDL_JoystickName(Joysticks[joystick]);
		info.PovHat = (SDL_JoystickNumHats(Joysticks[joystick]) > 0)
						? SJoystickInfo::POV_HAT_PRESENT : SJoystickInfo::POV_HAT_ABSENT;

		joystickInfo.push_back(info);
	}

	for(joystick = 0; joystick < (int)joystickInfo.size(); ++joystick)
	{
		char logString[256];
		snprintf_irr(logString, sizeof(logString), "Found joystick %d, %d axes, %d buttons '%s'",
			joystick, joystickInfo[joystick].Axes,
			joystickInfo[joystick].Buttons, joystickInfo[joystick].Name.c_str());
		os::Printer::log(logString, ELL_INFORMATION);
	}

	return true;

#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_

	return false;
}


//! Get the display density in dots per inch.
float CIrrDeviceSDL::getDisplayDensity() const
{
	if (!Window)
		return 0.0f;

	int window_w;
	int window_h;
	SDL_GetWindowSize(Window, &window_w, &window_h);

	int drawable_w;
	int drawable_h;
	SDL_GL_GetDrawableSize(Window, &drawable_w, &drawable_h);

	// assume 96 dpi
	float dpi_w = (float)drawable_w / (float)window_w * 96.0f;
	float dpi_h = (float)drawable_h / (float)window_h * 96.0f;

	return std::max(dpi_w, dpi_h);
}


void CIrrDeviceSDL::SwapWindow()
{
	SDL_GL_SwapWindow(Window);
}



//! pause execution temporarily
void CIrrDeviceSDL::yield()
{
	SDL_Delay(0);
}


//! pause execution for a specified time
void CIrrDeviceSDL::sleep(u32 timeMs, bool pauseTimer)
{
	const bool wasStopped = Timer ? Timer->isStopped() : true;
	if (pauseTimer && !wasStopped)
		Timer->stop();

	SDL_Delay(timeMs);

	if (pauseTimer && !wasStopped)
		Timer->start();
}


//! sets the caption of the window
void CIrrDeviceSDL::setWindowCaption(const wchar_t* text)
{
	core::stringc textc;
	core::wStringToUTF8(textc, text);
	SDL_SetWindowTitle(Window, textc.c_str());
}


//! Sets the window icon.
bool CIrrDeviceSDL::setWindowIcon(const video::IImage *img)
{
	if (!Window)
		return false;

	u32 height = img->getDimension().Height;
	u32 width = img->getDimension().Width;

	SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32,
			0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	if (!surface) {
		os::Printer::log("Failed to create SDL suface", ELL_ERROR);
		return false;
	}

	SDL_LockSurface(surface);
	bool succ = img->copyToNoScaling(surface->pixels, width, height, video::ECF_A8R8G8B8, surface->pitch);
	SDL_UnlockSurface(surface);

	if (!succ) {
		os::Printer::log("Could not copy icon image. Is the format not ECF_A8R8G8B8?", ELL_ERROR);
		SDL_FreeSurface(surface);
		return false;
	}

	SDL_SetWindowIcon(Window, surface);

	SDL_FreeSurface(surface);

	return true;
}


//! notifies the device that it should close itself
void CIrrDeviceSDL::closeDevice()
{
	Close = true;
}


//! Sets if the window should be resizable in windowed mode.
void CIrrDeviceSDL::setResizable(bool resize)
{
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	os::Printer::log("Resizable not available on the web." , ELL_WARNING);
	return;
#else // !_IRR_EMSCRIPTEN_PLATFORM_
	if (resize != Resizable) {
		if (resize)
			SDL_Flags |= SDL_WINDOW_RESIZABLE;
		else
			SDL_Flags &= ~SDL_WINDOW_RESIZABLE;

		if (Window) {
			SDL_SetWindowResizable(Window, (SDL_bool)resize);
		}
		Resizable = resize;
	}
#endif // !_IRR_EMSCRIPTEN_PLATFORM_
}


//! Minimizes window if possible
void CIrrDeviceSDL::minimizeWindow()
{
	if (Window)
		SDL_MinimizeWindow(Window);
}


//! Maximize window
void CIrrDeviceSDL::maximizeWindow()
{
	if (Window)
		SDL_MaximizeWindow(Window);
}

//! Get the position of this window on screen
core::position2di CIrrDeviceSDL::getWindowPosition()
{
    return core::position2di(-1, -1);
}


//! Restore original window size
void CIrrDeviceSDL::restoreWindow()
{
	if (Window)
		SDL_RestoreWindow(Window);
}

bool CIrrDeviceSDL::isWindowMaximized() const
{
	return Window && (SDL_GetWindowFlags(Window) & SDL_WINDOW_MAXIMIZED) != 0;
}

bool CIrrDeviceSDL::isFullscreen() const
{
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	return SDL_GetWindowFlags(0) == SDL_WINDOW_FULLSCREEN;
#else

	return CIrrDeviceStub::isFullscreen();
#endif
}


//! returns if window is active. if not, nothing need to be drawn
bool CIrrDeviceSDL::isWindowActive() const
{
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	// Hidden test only does something in some browsers (when tab in background or window is minimized)
	// In other browsers code automatically doesn't seem to be called anymore.
	EmscriptenVisibilityChangeEvent emVisibility;
	if ( emscripten_get_visibility_status(&emVisibility) == EMSCRIPTEN_RESULT_SUCCESS)
	{
		if ( emVisibility.hidden )
			return false;
	}
#endif
	const u32 windowFlags = SDL_GetWindowFlags(Window);
	return windowFlags & SDL_WINDOW_SHOWN && windowFlags & SDL_WINDOW_INPUT_FOCUS;
}


//! returns if window has focus.
bool CIrrDeviceSDL::isWindowFocused() const
{
	return Window && (SDL_GetWindowFlags(Window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}


//! returns if window is minimized.
bool CIrrDeviceSDL::isWindowMinimized() const
{
	return Window && (SDL_GetWindowFlags(Window) & SDL_WINDOW_MINIMIZED) != 0;
}


//! returns color format of the window.
video::ECOLOR_FORMAT CIrrDeviceSDL::getColorFormat() const
{
	if (Window)
	{
		SDL_Surface *surface = SDL_GetWindowSurface(Window);
		if (surface->format->BitsPerPixel == 16)
		{
			if (surface->format->Amask != 0)
				return video::ECF_A1R5G5B5;
			else
				return video::ECF_R5G6B5;
		}
		else
		{
			if (surface->format->Amask != 0)
				return video::ECF_A8R8G8B8;
			else
				return video::ECF_R8G8B8;
		}
	}
	else
		return CIrrDeviceStub::getColorFormat();
}


void CIrrDeviceSDL::createKeyMap()
{
	// I don't know if this is the best method  to create
	// the lookuptable, but I'll leave it like that until
	// I find a better version.

	KeyMap.reallocate(105);

	// buttons missing

	KeyMap.push_back(SKeyMap(SDLK_BACKSPACE, KEY_BACK));
	KeyMap.push_back(SKeyMap(SDLK_TAB, KEY_TAB));
	KeyMap.push_back(SKeyMap(SDLK_CLEAR, KEY_CLEAR));
	KeyMap.push_back(SKeyMap(SDLK_RETURN, KEY_RETURN));

	// combined modifiers missing

	KeyMap.push_back(SKeyMap(SDLK_PAUSE, KEY_PAUSE));
	KeyMap.push_back(SKeyMap(SDLK_CAPSLOCK, KEY_CAPITAL));

	// asian letter keys missing

	KeyMap.push_back(SKeyMap(SDLK_ESCAPE, KEY_ESCAPE));

	// asian letter keys missing

	KeyMap.push_back(SKeyMap(SDLK_SPACE, KEY_SPACE));
	KeyMap.push_back(SKeyMap(SDLK_PAGEUP, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(SDLK_PAGEDOWN, KEY_NEXT));
	KeyMap.push_back(SKeyMap(SDLK_END, KEY_END));
	KeyMap.push_back(SKeyMap(SDLK_HOME, KEY_HOME));
	KeyMap.push_back(SKeyMap(SDLK_LEFT, KEY_LEFT));
	KeyMap.push_back(SKeyMap(SDLK_UP, KEY_UP));
	KeyMap.push_back(SKeyMap(SDLK_RIGHT, KEY_RIGHT));
	KeyMap.push_back(SKeyMap(SDLK_DOWN, KEY_DOWN));

	// select missing
	KeyMap.push_back(SKeyMap(SDLK_PRINTSCREEN, KEY_PRINT));
	// execute missing
	KeyMap.push_back(SKeyMap(SDLK_PRINTSCREEN, KEY_SNAPSHOT));

	KeyMap.push_back(SKeyMap(SDLK_INSERT, KEY_INSERT));
	KeyMap.push_back(SKeyMap(SDLK_DELETE, KEY_DELETE));
	KeyMap.push_back(SKeyMap(SDLK_HELP, KEY_HELP));

	KeyMap.push_back(SKeyMap(SDLK_0, KEY_KEY_0));
	KeyMap.push_back(SKeyMap(SDLK_1, KEY_KEY_1));
	KeyMap.push_back(SKeyMap(SDLK_2, KEY_KEY_2));
	KeyMap.push_back(SKeyMap(SDLK_3, KEY_KEY_3));
	KeyMap.push_back(SKeyMap(SDLK_4, KEY_KEY_4));
	KeyMap.push_back(SKeyMap(SDLK_5, KEY_KEY_5));
	KeyMap.push_back(SKeyMap(SDLK_6, KEY_KEY_6));
	KeyMap.push_back(SKeyMap(SDLK_7, KEY_KEY_7));
	KeyMap.push_back(SKeyMap(SDLK_8, KEY_KEY_8));
	KeyMap.push_back(SKeyMap(SDLK_9, KEY_KEY_9));

	KeyMap.push_back(SKeyMap(SDLK_a, KEY_KEY_A));
	KeyMap.push_back(SKeyMap(SDLK_b, KEY_KEY_B));
	KeyMap.push_back(SKeyMap(SDLK_c, KEY_KEY_C));
	KeyMap.push_back(SKeyMap(SDLK_d, KEY_KEY_D));
	KeyMap.push_back(SKeyMap(SDLK_e, KEY_KEY_E));
	KeyMap.push_back(SKeyMap(SDLK_f, KEY_KEY_F));
	KeyMap.push_back(SKeyMap(SDLK_g, KEY_KEY_G));
	KeyMap.push_back(SKeyMap(SDLK_h, KEY_KEY_H));
	KeyMap.push_back(SKeyMap(SDLK_i, KEY_KEY_I));
	KeyMap.push_back(SKeyMap(SDLK_j, KEY_KEY_J));
	KeyMap.push_back(SKeyMap(SDLK_k, KEY_KEY_K));
	KeyMap.push_back(SKeyMap(SDLK_l, KEY_KEY_L));
	KeyMap.push_back(SKeyMap(SDLK_m, KEY_KEY_M));
	KeyMap.push_back(SKeyMap(SDLK_n, KEY_KEY_N));
	KeyMap.push_back(SKeyMap(SDLK_o, KEY_KEY_O));
	KeyMap.push_back(SKeyMap(SDLK_p, KEY_KEY_P));
	KeyMap.push_back(SKeyMap(SDLK_q, KEY_KEY_Q));
	KeyMap.push_back(SKeyMap(SDLK_r, KEY_KEY_R));
	KeyMap.push_back(SKeyMap(SDLK_s, KEY_KEY_S));
	KeyMap.push_back(SKeyMap(SDLK_t, KEY_KEY_T));
	KeyMap.push_back(SKeyMap(SDLK_u, KEY_KEY_U));
	KeyMap.push_back(SKeyMap(SDLK_v, KEY_KEY_V));
	KeyMap.push_back(SKeyMap(SDLK_w, KEY_KEY_W));
	KeyMap.push_back(SKeyMap(SDLK_x, KEY_KEY_X));
	KeyMap.push_back(SKeyMap(SDLK_y, KEY_KEY_Y));
	KeyMap.push_back(SKeyMap(SDLK_z, KEY_KEY_Z));

	KeyMap.push_back(SKeyMap(SDLK_LGUI, KEY_LWIN));
	KeyMap.push_back(SKeyMap(SDLK_RGUI, KEY_RWIN));
	// apps missing
	KeyMap.push_back(SKeyMap(SDLK_POWER, KEY_SLEEP)); //??

	KeyMap.push_back(SKeyMap(SDLK_KP_0, KEY_NUMPAD0));
	KeyMap.push_back(SKeyMap(SDLK_KP_1, KEY_NUMPAD1));
	KeyMap.push_back(SKeyMap(SDLK_KP_2, KEY_NUMPAD2));
	KeyMap.push_back(SKeyMap(SDLK_KP_3, KEY_NUMPAD3));
	KeyMap.push_back(SKeyMap(SDLK_KP_4, KEY_NUMPAD4));
	KeyMap.push_back(SKeyMap(SDLK_KP_5, KEY_NUMPAD5));
	KeyMap.push_back(SKeyMap(SDLK_KP_6, KEY_NUMPAD6));
	KeyMap.push_back(SKeyMap(SDLK_KP_7, KEY_NUMPAD7));
	KeyMap.push_back(SKeyMap(SDLK_KP_8, KEY_NUMPAD8));
	KeyMap.push_back(SKeyMap(SDLK_KP_9, KEY_NUMPAD9));
	KeyMap.push_back(SKeyMap(SDLK_KP_MULTIPLY, KEY_MULTIPLY));
	KeyMap.push_back(SKeyMap(SDLK_KP_PLUS, KEY_ADD));
//	KeyMap.push_back(SKeyMap(SDLK_KP_, KEY_SEPARATOR));
	KeyMap.push_back(SKeyMap(SDLK_KP_MINUS, KEY_SUBTRACT));
	KeyMap.push_back(SKeyMap(SDLK_KP_PERIOD, KEY_DECIMAL));
	KeyMap.push_back(SKeyMap(SDLK_KP_DIVIDE, KEY_DIVIDE));

	KeyMap.push_back(SKeyMap(SDLK_F1,  KEY_F1));
	KeyMap.push_back(SKeyMap(SDLK_F2,  KEY_F2));
	KeyMap.push_back(SKeyMap(SDLK_F3,  KEY_F3));
	KeyMap.push_back(SKeyMap(SDLK_F4,  KEY_F4));
	KeyMap.push_back(SKeyMap(SDLK_F5,  KEY_F5));
	KeyMap.push_back(SKeyMap(SDLK_F6,  KEY_F6));
	KeyMap.push_back(SKeyMap(SDLK_F7,  KEY_F7));
	KeyMap.push_back(SKeyMap(SDLK_F8,  KEY_F8));
	KeyMap.push_back(SKeyMap(SDLK_F9,  KEY_F9));
	KeyMap.push_back(SKeyMap(SDLK_F10, KEY_F10));
	KeyMap.push_back(SKeyMap(SDLK_F11, KEY_F11));
	KeyMap.push_back(SKeyMap(SDLK_F12, KEY_F12));
	KeyMap.push_back(SKeyMap(SDLK_F13, KEY_F13));
	KeyMap.push_back(SKeyMap(SDLK_F14, KEY_F14));
	KeyMap.push_back(SKeyMap(SDLK_F15, KEY_F15));
	// no higher F-keys

	KeyMap.push_back(SKeyMap(SDLK_NUMLOCKCLEAR, KEY_NUMLOCK));
	KeyMap.push_back(SKeyMap(SDLK_SCROLLLOCK, KEY_SCROLL));
	KeyMap.push_back(SKeyMap(SDLK_LSHIFT, KEY_LSHIFT));
	KeyMap.push_back(SKeyMap(SDLK_RSHIFT, KEY_RSHIFT));
	KeyMap.push_back(SKeyMap(SDLK_LCTRL,  KEY_LCONTROL));
	KeyMap.push_back(SKeyMap(SDLK_RCTRL,  KEY_RCONTROL));
	KeyMap.push_back(SKeyMap(SDLK_LALT,  KEY_LMENU));
	KeyMap.push_back(SKeyMap(SDLK_RALT,  KEY_RMENU));

	KeyMap.push_back(SKeyMap(SDLK_PLUS,   KEY_PLUS));
	KeyMap.push_back(SKeyMap(SDLK_COMMA,  KEY_COMMA));
	KeyMap.push_back(SKeyMap(SDLK_MINUS,  KEY_MINUS));
	KeyMap.push_back(SKeyMap(SDLK_PERIOD, KEY_PERIOD));

	// some special keys missing

	KeyMap.sort();
}

void CIrrDeviceSDL::CCursorControl::initCursors()
{
	Cursors.reserve(gui::ECI_COUNT);

	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));     // ECI_NORMAL
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR)); // ECI_CROSS
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));      // ECI_HAND
	Cursors.emplace_back(nullptr);                                             // ECI_HELP
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM));     // ECI_IBEAM
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO));        // ECI_NO
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT));      // ECI_WAIT
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL));   // ECI_SIZEALL
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW));  // ECI_SIZENESW
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE));  // ECI_SIZENWSE
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS));    // ECI_SIZENS
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE));    // ECI_SIZEWE
	Cursors.emplace_back(nullptr);                                             // ECI_UP
}

} // end namespace irr

#endif // _IRR_COMPILE_WITH_SDL_DEVICE_

