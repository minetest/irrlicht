// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceSDL2.h"

#ifdef _IRR_COMPILE_WITH_X11_DEVICE_

#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <time.h>
#include "IEventReceiver.h"
#include "ISceneManager.h"
#include "IGUIEnvironment.h"
#include "os.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include "COSOperator.h"
#include "CColorConverter.h"
#include "SIrrCreationParameters.h"
#include "IGUISpriteBank.h"

#if defined _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#include <fcntl.h>
#include <unistd.h>

#ifdef __FreeBSD__
#include <sys/joystick.h>
#else

// linux/joystick.h includes linux/input.h, which #defines values for various KEY_FOO keys.
// These override the irr::KEY_FOO equivalents, which stops key handling from working.
// As a workaround, defining _INPUT_H stops linux/input.h from being included; it
// doesn't actually seem to be necessary except to pull in sys/ioctl.h.
#define _INPUT_H
#include <sys/ioctl.h> // Would normally be included in linux/input.h
#include <linux/joystick.h>
#undef _INPUT_H
#endif

#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_

namespace irr
{
	namespace video
	{
		IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& params,
				io::IFileSystem* io, CIrrDeviceSDL2* device);
	}
} // end namespace irr

namespace irr
{

const char* wmDeleteWindow = "WM_DELETE_WINDOW";

//! constructor
CIrrDeviceSDL2::CIrrDeviceSDL2(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param),
#ifdef _IRR_COMPILE_WITH_X11_
#ifdef _IRR_COMPILE_WITH_OPENGL_
	window(0),
	Context(0),
#endif
#endif
	Width(param.WindowSize.Width), Height(param.WindowSize.Height),
	WindowHasFocus(false), WindowMinimized(false),
	UseXVidMode(false), UseXRandR(false), UseGLXWindow(false),
	ExternalWindow(false), AutorepeatSupport(0)
{
	#ifdef _DEBUG
	setDebugName("CIrrDeviceSDL2");
	#endif

	// print version, distribution etc.
	// thx to LynxLuna for pointing me to the uname function
	core::stringc linuxversion;
	struct utsname LinuxInfo;
	uname(&LinuxInfo);

	linuxversion += LinuxInfo.sysname;
	linuxversion += " ";
	linuxversion += LinuxInfo.release;
	linuxversion += " ";
	linuxversion += LinuxInfo.version;
	linuxversion += " ";
	linuxversion += LinuxInfo.machine;

	Operator = new COSOperator(linuxversion, this);
	os::Printer::log(linuxversion.c_str(), ELL_INFORMATION);

	// create keymap
	createKeyMap();

	// create window
	if (CreationParams.DriverType != video::EDT_NULL)
	{
		SDL_Init(SDL_INIT_VIDEO);

		// create the window, only if we do not use the null device
		if (!createWindow())
			return;
	}

	// create cursor control
	CursorControl = new CCursorControl(this, CreationParams.DriverType == video::EDT_NULL);

	// create driver
	createDriver();

	if (!VideoDriver)
		return;

	createGUIAndScene();
}


//! destructor
CIrrDeviceSDL2::~CIrrDeviceSDL2()
{
#ifdef _IRR_COMPILE_WITH_X11_
	// Disable cursor (it is drop'ed in stub)
	if (CursorControl)
	{
		CursorControl->setVisible(false);
		static_cast<CCursorControl*>(CursorControl)->clearCursors();
	}

	// Must free OpenGL textures etc before destroying context, so can't wait for stub destructor
	if ( GUIEnvironment )
	{
		GUIEnvironment->drop();
		GUIEnvironment = NULL;
	}
	if ( SceneManager )
	{
		SceneManager->drop();
		SceneManager = NULL;
	}
	if ( VideoDriver )
	{
		VideoDriver->drop();
		VideoDriver = NULL;
	}

	#ifdef _IRR_COMPILE_WITH_OPENGL_
	if (Context)
	{
		if (!SDL_GL_MakeCurrent(window, Context)) {
			os::Printer::log("Could not release GL context.", ELL_WARNING);
		}

		SDL_GL_DeleteContext(Context);
	}
	#endif // #ifdef _IRR_COMPILE_WITH_OPENGL_

	// Reset fullscreen resolution change
	switchToFullscreen(true);

	if (!ExternalWindow)
	{
		SDL_DestroyWindow(window);
	}

#endif // #ifdef _IRR_COMPILE_WITH_X11_

#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	for (u32 joystick = 0; joystick < ActiveJoysticks.size(); ++joystick)
	{
		if (ActiveJoysticks[joystick].fd >= 0)
		{
			close(ActiveJoysticks[joystick].fd);
		}
	}
#endif
}

bool CIrrDeviceSDL2::switchToFullscreen(bool reset)
{
	if (!CreationParams.Fullscreen)
		return true;
	if (reset)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		return true;
	}

	getVideoModeList();

	return CreationParams.Fullscreen;
}


#if defined(_IRR_COMPILE_WITH_X11_)
void IrrPrintXGrabError(int grabResult, const c8 * grabCommand )
{
	if ( grabResult == GrabSuccess )
	{
//		os::Printer::log(grabCommand, ": GrabSuccess", ELL_INFORMATION);
		return;
	}

	switch ( grabResult )
	{
		case AlreadyGrabbed:
			os::Printer::log(grabCommand, ": AlreadyGrabbed", ELL_WARNING);
			break;
		case GrabNotViewable:
			os::Printer::log(grabCommand, ": GrabNotViewable", ELL_WARNING);
			break;
		case GrabFrozen:
			os::Printer::log(grabCommand, ": GrabFrozen", ELL_WARNING);
			break;
		case GrabInvalidTime:
			os::Printer::log(grabCommand, ": GrabInvalidTime", ELL_WARNING);
			break;
		default:
			os::Printer::log(grabCommand, ": grab failed with unknown problem", ELL_WARNING);
			break;
	}
}
#endif


bool CIrrDeviceSDL2::createWindow()
{
#ifdef _IRR_COMPILE_WITH_X11_

	switchToFullscreen();

	Uint32 windowFlags = 0;
	if (CreationParams.Fullscreen) {
		windowFlags = SDL_WINDOW_FULLSCREEN;
	}

	window = SDL_CreateWindow(
    "Irrlicht (title not set)",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		CreationParams.WindowSize.Width,
		CreationParams.WindowSize.Height,
		windowFlags | SDL_WINDOW_OPENGL
	);

	// create color map
	WindowMinimized=false;

	SDL_GL_LoadLibrary(NULL);
	Context = SDL_GL_CreateContext(window);

	unsigned int bits;

	CreationParams.Bits = bits;
	CreationParams.WindowSize.Width = Width;
	CreationParams.WindowSize.Height = Height;

	// create an XImage for the software renderer
	//(thx to Nadav for some clues on how to do that!)

#endif // #ifdef _IRR_COMPILE_WITH_X11_
	return true;
}


//! create the driver
void CIrrDeviceSDL2::createDriver()
{
	switch(CreationParams.DriverType)
	{
#ifdef _IRR_COMPILE_WITH_X11_

	case video::EDT_OPENGL:
		if (Context)
			VideoDriver = video::createOpenGLDriver(CreationParams, FileSystem, this);
		break;

	case video::EDT_DIRECT3D8:
	case video::EDT_DIRECT3D9:
		os::Printer::log("This driver is not available in Linux. Try OpenGL or Software renderer.",
			ELL_ERROR);
		break;

	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		break;

	default:
		os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
		break;
#else
	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		break;
	default:
		os::Printer::log("No X11 support compiled in. Only Null driver available.", ELL_ERROR);
		break;
#endif
	}
}


//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceSDL2::run()
{
	os::Timer::tick();

#ifdef _IRR_COMPILE_WITH_X11_

	if ( CursorControl )
		static_cast<CCursorControl*>(CursorControl)->update();

	if ((CreationParams.DriverType != video::EDT_NULL))
	{
		SEvent irrevent;
		irrevent.MouseInput.ButtonStates = 0xffffffff;
		while (/*XPending(display) > 0 &&*/ !Close)
		{
#if 0
			XEvent event;
			XNextEvent(display, &event);

			switch (event.type)
			{
			case ConfigureNotify:
				// check for changed window size
				if ((event.xconfigure.width != (int) Width) ||
					(event.xconfigure.height != (int) Height))
				{
					Width = event.xconfigure.width;
					Height = event.xconfigure.height;

					// resize image data
					if (SoftwareImage)
					{
						XDestroyImage(SoftwareImage);

						SoftwareImage = XCreateImage(display,
							visual->visual, visual->depth,
							ZPixmap, 0, 0, Width, Height,
							BitmapPad(display), 0);

						// use malloc because X will free it later on
						if (SoftwareImage)
							SoftwareImage->data = (char*) malloc(SoftwareImage->bytes_per_line * SoftwareImage->height * sizeof(char));
					}

					if (VideoDriver)
						VideoDriver->OnResize(core::dimension2d<u32>(Width, Height));
				}
				break;

			case MapNotify:
				WindowMinimized=false;
				break;

			case UnmapNotify:
				WindowMinimized=true;
				break;

			case FocusIn:
				WindowHasFocus=true;
				break;

			case FocusOut:
				WindowHasFocus=false;
				break;

			case MotionNotify:
				irrevent.EventType = irr::EET_MOUSE_INPUT_EVENT;
				irrevent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
				irrevent.MouseInput.X = event.xbutton.x;
				irrevent.MouseInput.Y = event.xbutton.y;
				irrevent.MouseInput.Control = (event.xkey.state & ControlMask) != 0;
				irrevent.MouseInput.Shift = (event.xkey.state & ShiftMask) != 0;

				// mouse button states
				irrevent.MouseInput.ButtonStates = (event.xbutton.state & Button1Mask) ? irr::EMBSM_LEFT : 0;
				irrevent.MouseInput.ButtonStates |= (event.xbutton.state & Button3Mask) ? irr::EMBSM_RIGHT : 0;
				irrevent.MouseInput.ButtonStates |= (event.xbutton.state & Button2Mask) ? irr::EMBSM_MIDDLE : 0;

				postEventFromUser(irrevent);
				break;

			case ButtonPress:
			case ButtonRelease:

				irrevent.EventType = irr::EET_MOUSE_INPUT_EVENT;
				irrevent.MouseInput.X = event.xbutton.x;
				irrevent.MouseInput.Y = event.xbutton.y;
				irrevent.MouseInput.Control = (event.xkey.state & ControlMask) != 0;
				irrevent.MouseInput.Shift = (event.xkey.state & ShiftMask) != 0;

				// mouse button states
				// This sets the state which the buttons had _prior_ to the event.
				// So unlike on Windows the button which just got changed has still the old state here.
				// We handle that below by flipping the corresponding bit later.
				irrevent.MouseInput.ButtonStates = (event.xbutton.state & Button1Mask) ? irr::EMBSM_LEFT : 0;
				irrevent.MouseInput.ButtonStates |= (event.xbutton.state & Button3Mask) ? irr::EMBSM_RIGHT : 0;
				irrevent.MouseInput.ButtonStates |= (event.xbutton.state & Button2Mask) ? irr::EMBSM_MIDDLE : 0;

				irrevent.MouseInput.Event = irr::EMIE_COUNT;

				switch(event.xbutton.button)
				{
				case  Button1:
					irrevent.MouseInput.Event =
						(event.type == ButtonPress) ? irr::EMIE_LMOUSE_PRESSED_DOWN : irr::EMIE_LMOUSE_LEFT_UP;
					irrevent.MouseInput.ButtonStates ^= irr::EMBSM_LEFT;
					break;

				case  Button3:
					irrevent.MouseInput.Event =
						(event.type == ButtonPress) ? irr::EMIE_RMOUSE_PRESSED_DOWN : irr::EMIE_RMOUSE_LEFT_UP;
					irrevent.MouseInput.ButtonStates ^= irr::EMBSM_RIGHT;
					break;

				case  Button2:
					irrevent.MouseInput.Event =
						(event.type == ButtonPress) ? irr::EMIE_MMOUSE_PRESSED_DOWN : irr::EMIE_MMOUSE_LEFT_UP;
					irrevent.MouseInput.ButtonStates ^= irr::EMBSM_MIDDLE;
					break;

				case  Button4:
					if (event.type == ButtonPress)
					{
						irrevent.MouseInput.Event = EMIE_MOUSE_WHEEL;
						irrevent.MouseInput.Wheel = 1.0f;
					}
					break;

				case  Button5:
					if (event.type == ButtonPress)
					{
						irrevent.MouseInput.Event = EMIE_MOUSE_WHEEL;
						irrevent.MouseInput.Wheel = -1.0f;
					}
					break;
				}

				if (irrevent.MouseInput.Event != irr::EMIE_COUNT)
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

			case MappingNotify:
				XRefreshKeyboardMapping (&event.xmapping) ;
				break;

			case KeyRelease:
				if (0 == AutorepeatSupport && (XPending( display ) > 0) )
				{
					// check for Autorepeat manually
					// We'll do the same as Windows does: Only send KeyPressed
					// So every KeyRelease is a real release
					XEvent next_event;
					XPeekEvent (event.xkey.display, &next_event);
					if ((next_event.type == KeyPress) &&
						(next_event.xkey.keycode == event.xkey.keycode) &&
						(next_event.xkey.time - event.xkey.time) < 2)	// usually same time, but on some systems a difference of 1 is possible
					{
						/* Ignore the key release event */
						break;
					}
				}
				// fall-through in case the release should be handled
			case KeyPress:
				{
					SKeyMap mp;
					char buf[8]={0};
					XLookupString(&event.xkey, buf, sizeof(buf), &mp.X11Key, NULL);

					irrevent.EventType = irr::EET_KEY_INPUT_EVENT;
					irrevent.KeyInput.PressedDown = (event.type == KeyPress);
//					mbtowc(&irrevent.KeyInput.Char, buf, sizeof(buf));
					irrevent.KeyInput.Char = ((wchar_t*)(buf))[0];
					irrevent.KeyInput.Control = (event.xkey.state & ControlMask) != 0;
					irrevent.KeyInput.Shift = (event.xkey.state & ShiftMask) != 0;

					event.xkey.state = 0; // ignore shift-ctrl states for figuring out the key
					XLookupString(&event.xkey, buf, sizeof(buf), &mp.X11Key, NULL);
					const s32 idx = KeyMap.binary_search(mp);
					if (idx != -1)
					{
						irrevent.KeyInput.Key = (EKEY_CODE)KeyMap[idx].Win32Key;
					}
					else
					{
						irrevent.KeyInput.Key = (EKEY_CODE)0;
					}
					if (irrevent.KeyInput.Key == 0)
					{
						// 1:1 mapping to windows-keys would require testing for keyboard type (us, ger, ...)
						// So unless we do that we will have some unknown keys here.
						if (idx == -1)
						{
							os::Printer::log("Could not find EKEY_CODE, using orig. X11 keycode instead", core::stringc(event.xkey.keycode).c_str(), ELL_INFORMATION);
						}
						else
						{
							os::Printer::log("EKEY_CODE is 0, using orig. X11 keycode instead", core::stringc(event.xkey.keycode).c_str(), ELL_INFORMATION);
						}
						// Any value is better than none, that allows at least using the keys.
						// Worst case is that some keys will be identical, still better than _all_
						// unknown keys being identical.
						irrevent.KeyInput.Key = (EKEY_CODE)event.xkey.keycode;
					}

					postEventFromUser(irrevent);
				}
				break;

			case ClientMessage:
				{
					char *atom = XGetAtomName(display, event.xclient.message_type);
					if (*atom == *wmDeleteWindow)
					{
						os::Printer::log("Quit message received.", ELL_INFORMATION);
						Close = true;
					}
					else
					{
						// we assume it's a user message
						irrevent.EventType = irr::EET_USER_EVENT;
						irrevent.UserEvent.UserData1 = (s32)event.xclient.data.l[0];
						irrevent.UserEvent.UserData2 = (s32)event.xclient.data.l[1];
						postEventFromUser(irrevent);
					}
					XFree(atom);
				}
				break;

			case SelectionRequest:
				{
					XEvent respond;
					XSelectionRequestEvent *req = &(event.xselectionrequest);
					if (  req->target == XA_STRING)
					{
						XChangeProperty (display,
								req->requestor,
								req->property, req->target,
								8, // format
								PropModeReplace,
								(unsigned char*) Clipboard.c_str(),
								Clipboard.size());
						respond.xselection.property = req->property;
					}
					else if ( req->target == X_ATOM_TARGETS )
					{
						long data[2];

						data[0] = X_ATOM_TEXT;
						data[1] = XA_STRING;

						XChangeProperty (display, req->requestor,
								req->property, req->target,
								8, PropModeReplace,
								(unsigned char *) &data,
								sizeof (data));
						respond.xselection.property = req->property;
					}
					else
					{
						respond.xselection.property= None;
					}
					respond.xselection.type= SelectionNotify;
					respond.xselection.display= req->display;
					respond.xselection.requestor= req->requestor;
					respond.xselection.selection=req->selection;
					respond.xselection.target= req->target;
					respond.xselection.time = req->time;
					XSendEvent (display, req->requestor,0,0,&respond);
					XFlush (display);
				}
				break;

			default:
				break;
			} // end switch
#endif
		} // end while
	}
#endif //_IRR_COMPILE_WITH_X11_

	if (!Close)
		pollJoysticks();

	return !Close;
}


//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDeviceSDL2::yield()
{
	struct timespec ts = {0,1};
	nanosleep(&ts, NULL);
}


//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceSDL2::sleep(u32 timeMs, bool pauseTimer=false)
{
	const bool wasStopped = Timer ? Timer->isStopped() : true;

	struct timespec ts;
	ts.tv_sec = (time_t) (timeMs / 1000);
	ts.tv_nsec = (long) (timeMs % 1000) * 1000000;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	nanosleep(&ts, NULL);

	if (pauseTimer && !wasStopped)
		Timer->start();
}


//! sets the caption of the window
void CIrrDeviceSDL2::setWindowCaption(const wchar_t* text)
{
#ifdef _IRR_COMPILE_WITH_X11_
	if (CreationParams.DriverType == video::EDT_NULL)
		return;

	core::stringc textc = text;
	SDL_SetWindowTitle(window, textc.c_str());
#endif
}


//! presents a surface in the client area
bool CIrrDeviceSDL2::present(video::IImage* image, void* windowId, core::rect<s32>* srcRect)
{
	return true;
}


//! notifies the device that it should close itself
void CIrrDeviceSDL2::closeDevice()
{
	Close = true;
}


//! returns if window is active. if not, nothing need to be drawn
bool CIrrDeviceSDL2::isWindowActive() const
{
	return (WindowHasFocus && !WindowMinimized);
}


//! returns if window has focus.
bool CIrrDeviceSDL2::isWindowFocused() const
{
	return WindowHasFocus;
}


//! returns if window is minimized.
bool CIrrDeviceSDL2::isWindowMinimized() const
{
	return WindowMinimized;
}


//! returns color format of the window.
video::ECOLOR_FORMAT CIrrDeviceSDL2::getColorFormat() const
{
		return video::ECF_R5G6B5;
}


//! Sets if the window should be resizable in windowed mode.
void CIrrDeviceSDL2::setResizable(bool resize)
{
#ifdef _IRR_COMPILE_WITH_X11_
	if (CreationParams.DriverType == video::EDT_NULL || CreationParams.Fullscreen )
		return;

	SDL_SetWindowResizable(window, resize ? SDL_TRUE : SDL_FALSE);
#endif // #ifdef _IRR_COMPILE_WITH_X11_
}


//! Return pointer to a list with all video modes supported by the gfx adapter.
video::IVideoModeList* CIrrDeviceSDL2::getVideoModeList()
{
	s32 displayModeCount = SDL_GetNumDisplayModes(0);
	for (s32 i = 0; i < displayModeCount; i++) {
		SDL_DisplayMode mode;
		SDL_GetDesktopDisplayMode(i, &mode);

		if (i == 0) {
			VideoModeList->setDesktop(1, core::dimension2d<u32>(
					mode.w, mode.h));
		}

		VideoModeList->addMode(core::dimension2d<u32>(
			mode.w, mode.h), 1);
	}

	return VideoModeList;
}


//! Minimize window
void CIrrDeviceSDL2::minimizeWindow()
{
#ifdef _IRR_COMPILE_WITH_X11_
	SDL_MinimizeWindow(window);
#endif
}


//! Maximize window
void CIrrDeviceSDL2::maximizeWindow()
{
#ifdef _IRR_COMPILE_WITH_X11_
	SDL_MaximizeWindow(window);
#endif
}


//! Restore original window size
void CIrrDeviceSDL2::restoreWindow()
{
#ifdef _IRR_COMPILE_WITH_X11_
	SDL_RestoreWindow(window);
#endif
}


void CIrrDeviceSDL2::createKeyMap()
{
	// I don't know if this is the best method  to create
	// the lookuptable, but I'll leave it like that until
	// I find a better version.

#ifdef _IRR_COMPILE_WITH_X11_
	KeyMap.reallocate(84);
	KeyMap.push_back(SKeyMap(XK_BackSpace, KEY_BACK));
	KeyMap.push_back(SKeyMap(XK_Tab, KEY_TAB));
	KeyMap.push_back(SKeyMap(XK_ISO_Left_Tab, KEY_TAB));
	KeyMap.push_back(SKeyMap(XK_Linefeed, 0)); // ???
	KeyMap.push_back(SKeyMap(XK_Clear, KEY_CLEAR));
	KeyMap.push_back(SKeyMap(XK_Return, KEY_RETURN));
	KeyMap.push_back(SKeyMap(XK_Pause, KEY_PAUSE));
	KeyMap.push_back(SKeyMap(XK_Scroll_Lock, KEY_SCROLL));
	KeyMap.push_back(SKeyMap(XK_Sys_Req, 0)); // ???
	KeyMap.push_back(SKeyMap(XK_Escape, KEY_ESCAPE));
	KeyMap.push_back(SKeyMap(XK_Insert, KEY_INSERT));
	KeyMap.push_back(SKeyMap(XK_Delete, KEY_DELETE));
	KeyMap.push_back(SKeyMap(XK_Home, KEY_HOME));
	KeyMap.push_back(SKeyMap(XK_Left, KEY_LEFT));
	KeyMap.push_back(SKeyMap(XK_Up, KEY_UP));
	KeyMap.push_back(SKeyMap(XK_Right, KEY_RIGHT));
	KeyMap.push_back(SKeyMap(XK_Down, KEY_DOWN));
	KeyMap.push_back(SKeyMap(XK_Prior, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(XK_Page_Up, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(XK_Next, KEY_NEXT));
	KeyMap.push_back(SKeyMap(XK_Page_Down, KEY_NEXT));
	KeyMap.push_back(SKeyMap(XK_End, KEY_END));
	KeyMap.push_back(SKeyMap(XK_Begin, KEY_HOME));
	KeyMap.push_back(SKeyMap(XK_Num_Lock, KEY_NUMLOCK));
	KeyMap.push_back(SKeyMap(XK_KP_Space, KEY_SPACE));
	KeyMap.push_back(SKeyMap(XK_KP_Tab, KEY_TAB));
	KeyMap.push_back(SKeyMap(XK_KP_Enter, KEY_RETURN));
	KeyMap.push_back(SKeyMap(XK_KP_F1, KEY_F1));
	KeyMap.push_back(SKeyMap(XK_KP_F2, KEY_F2));
	KeyMap.push_back(SKeyMap(XK_KP_F3, KEY_F3));
	KeyMap.push_back(SKeyMap(XK_KP_F4, KEY_F4));
	KeyMap.push_back(SKeyMap(XK_KP_Home, KEY_HOME));
	KeyMap.push_back(SKeyMap(XK_KP_Left, KEY_LEFT));
	KeyMap.push_back(SKeyMap(XK_KP_Up, KEY_UP));
	KeyMap.push_back(SKeyMap(XK_KP_Right, KEY_RIGHT));
	KeyMap.push_back(SKeyMap(XK_KP_Down, KEY_DOWN));
	KeyMap.push_back(SKeyMap(XK_Print, KEY_PRINT));
	KeyMap.push_back(SKeyMap(XK_KP_Prior, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(XK_KP_Page_Up, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(XK_KP_Next, KEY_NEXT));
	KeyMap.push_back(SKeyMap(XK_KP_Page_Down, KEY_NEXT));
	KeyMap.push_back(SKeyMap(XK_KP_End, KEY_END));
	KeyMap.push_back(SKeyMap(XK_KP_Begin, KEY_HOME));
	KeyMap.push_back(SKeyMap(XK_KP_Insert, KEY_INSERT));
	KeyMap.push_back(SKeyMap(XK_KP_Delete, KEY_DELETE));
	KeyMap.push_back(SKeyMap(XK_KP_Equal, 0)); // ???
	KeyMap.push_back(SKeyMap(XK_KP_Multiply, KEY_MULTIPLY));
	KeyMap.push_back(SKeyMap(XK_KP_Add, KEY_ADD));
	KeyMap.push_back(SKeyMap(XK_KP_Separator, KEY_SEPARATOR));
	KeyMap.push_back(SKeyMap(XK_KP_Subtract, KEY_SUBTRACT));
	KeyMap.push_back(SKeyMap(XK_KP_Decimal, KEY_DECIMAL));
	KeyMap.push_back(SKeyMap(XK_KP_Divide, KEY_DIVIDE));
	KeyMap.push_back(SKeyMap(XK_KP_0, KEY_KEY_0));
	KeyMap.push_back(SKeyMap(XK_KP_1, KEY_KEY_1));
	KeyMap.push_back(SKeyMap(XK_KP_2, KEY_KEY_2));
	KeyMap.push_back(SKeyMap(XK_KP_3, KEY_KEY_3));
	KeyMap.push_back(SKeyMap(XK_KP_4, KEY_KEY_4));
	KeyMap.push_back(SKeyMap(XK_KP_5, KEY_KEY_5));
	KeyMap.push_back(SKeyMap(XK_KP_6, KEY_KEY_6));
	KeyMap.push_back(SKeyMap(XK_KP_7, KEY_KEY_7));
	KeyMap.push_back(SKeyMap(XK_KP_8, KEY_KEY_8));
	KeyMap.push_back(SKeyMap(XK_KP_9, KEY_KEY_9));
	KeyMap.push_back(SKeyMap(XK_F1, KEY_F1));
	KeyMap.push_back(SKeyMap(XK_F2, KEY_F2));
	KeyMap.push_back(SKeyMap(XK_F3, KEY_F3));
	KeyMap.push_back(SKeyMap(XK_F4, KEY_F4));
	KeyMap.push_back(SKeyMap(XK_F5, KEY_F5));
	KeyMap.push_back(SKeyMap(XK_F6, KEY_F6));
	KeyMap.push_back(SKeyMap(XK_F7, KEY_F7));
	KeyMap.push_back(SKeyMap(XK_F8, KEY_F8));
	KeyMap.push_back(SKeyMap(XK_F9, KEY_F9));
	KeyMap.push_back(SKeyMap(XK_F10, KEY_F10));
	KeyMap.push_back(SKeyMap(XK_F11, KEY_F11));
	KeyMap.push_back(SKeyMap(XK_F12, KEY_F12));
	KeyMap.push_back(SKeyMap(XK_Shift_L, KEY_LSHIFT));
	KeyMap.push_back(SKeyMap(XK_Shift_R, KEY_RSHIFT));
	KeyMap.push_back(SKeyMap(XK_Control_L, KEY_LCONTROL));
	KeyMap.push_back(SKeyMap(XK_Control_R, KEY_RCONTROL));
	KeyMap.push_back(SKeyMap(XK_Caps_Lock, KEY_CAPITAL));
	KeyMap.push_back(SKeyMap(XK_Shift_Lock, KEY_CAPITAL));
	KeyMap.push_back(SKeyMap(XK_Meta_L, KEY_LWIN));
	KeyMap.push_back(SKeyMap(XK_Meta_R, KEY_RWIN));
	KeyMap.push_back(SKeyMap(XK_Alt_L, KEY_LMENU));
	KeyMap.push_back(SKeyMap(XK_Alt_R, KEY_RMENU));
	KeyMap.push_back(SKeyMap(XK_ISO_Level3_Shift, KEY_RMENU));
	KeyMap.push_back(SKeyMap(XK_Menu, KEY_MENU));
	KeyMap.push_back(SKeyMap(XK_space, KEY_SPACE));
	KeyMap.push_back(SKeyMap(XK_exclam, 0)); //?
	KeyMap.push_back(SKeyMap(XK_quotedbl, 0)); //?
	KeyMap.push_back(SKeyMap(XK_section, 0)); //?
	KeyMap.push_back(SKeyMap(XK_numbersign, KEY_OEM_2));
	KeyMap.push_back(SKeyMap(XK_dollar, 0)); //?
	KeyMap.push_back(SKeyMap(XK_percent, 0)); //?
	KeyMap.push_back(SKeyMap(XK_ampersand, 0)); //?
	KeyMap.push_back(SKeyMap(XK_apostrophe, KEY_OEM_7));
	KeyMap.push_back(SKeyMap(XK_parenleft, 0)); //?
	KeyMap.push_back(SKeyMap(XK_parenright, 0)); //?
	KeyMap.push_back(SKeyMap(XK_asterisk, 0)); //?
	KeyMap.push_back(SKeyMap(XK_plus, KEY_PLUS)); //?
	KeyMap.push_back(SKeyMap(XK_comma, KEY_COMMA)); //?
	KeyMap.push_back(SKeyMap(XK_minus, KEY_MINUS)); //?
	KeyMap.push_back(SKeyMap(XK_period, KEY_PERIOD)); //?
	KeyMap.push_back(SKeyMap(XK_slash, KEY_OEM_2)); //?
	KeyMap.push_back(SKeyMap(XK_0, KEY_KEY_0));
	KeyMap.push_back(SKeyMap(XK_1, KEY_KEY_1));
	KeyMap.push_back(SKeyMap(XK_2, KEY_KEY_2));
	KeyMap.push_back(SKeyMap(XK_3, KEY_KEY_3));
	KeyMap.push_back(SKeyMap(XK_4, KEY_KEY_4));
	KeyMap.push_back(SKeyMap(XK_5, KEY_KEY_5));
	KeyMap.push_back(SKeyMap(XK_6, KEY_KEY_6));
	KeyMap.push_back(SKeyMap(XK_7, KEY_KEY_7));
	KeyMap.push_back(SKeyMap(XK_8, KEY_KEY_8));
	KeyMap.push_back(SKeyMap(XK_9, KEY_KEY_9));
	KeyMap.push_back(SKeyMap(XK_colon, 0)); //?
	KeyMap.push_back(SKeyMap(XK_semicolon, KEY_OEM_1));
	KeyMap.push_back(SKeyMap(XK_less, KEY_OEM_102));
	KeyMap.push_back(SKeyMap(XK_equal, KEY_PLUS));
	KeyMap.push_back(SKeyMap(XK_greater, 0)); //?
	KeyMap.push_back(SKeyMap(XK_question, 0)); //?
	KeyMap.push_back(SKeyMap(XK_at, KEY_KEY_2)); //?
	KeyMap.push_back(SKeyMap(XK_mu, 0)); //?
	KeyMap.push_back(SKeyMap(XK_EuroSign, 0)); //?
	KeyMap.push_back(SKeyMap(XK_A, KEY_KEY_A));
	KeyMap.push_back(SKeyMap(XK_B, KEY_KEY_B));
	KeyMap.push_back(SKeyMap(XK_C, KEY_KEY_C));
	KeyMap.push_back(SKeyMap(XK_D, KEY_KEY_D));
	KeyMap.push_back(SKeyMap(XK_E, KEY_KEY_E));
	KeyMap.push_back(SKeyMap(XK_F, KEY_KEY_F));
	KeyMap.push_back(SKeyMap(XK_G, KEY_KEY_G));
	KeyMap.push_back(SKeyMap(XK_H, KEY_KEY_H));
	KeyMap.push_back(SKeyMap(XK_I, KEY_KEY_I));
	KeyMap.push_back(SKeyMap(XK_J, KEY_KEY_J));
	KeyMap.push_back(SKeyMap(XK_K, KEY_KEY_K));
	KeyMap.push_back(SKeyMap(XK_L, KEY_KEY_L));
	KeyMap.push_back(SKeyMap(XK_M, KEY_KEY_M));
	KeyMap.push_back(SKeyMap(XK_N, KEY_KEY_N));
	KeyMap.push_back(SKeyMap(XK_O, KEY_KEY_O));
	KeyMap.push_back(SKeyMap(XK_P, KEY_KEY_P));
	KeyMap.push_back(SKeyMap(XK_Q, KEY_KEY_Q));
	KeyMap.push_back(SKeyMap(XK_R, KEY_KEY_R));
	KeyMap.push_back(SKeyMap(XK_S, KEY_KEY_S));
	KeyMap.push_back(SKeyMap(XK_T, KEY_KEY_T));
	KeyMap.push_back(SKeyMap(XK_U, KEY_KEY_U));
	KeyMap.push_back(SKeyMap(XK_V, KEY_KEY_V));
	KeyMap.push_back(SKeyMap(XK_W, KEY_KEY_W));
	KeyMap.push_back(SKeyMap(XK_X, KEY_KEY_X));
	KeyMap.push_back(SKeyMap(XK_Y, KEY_KEY_Y));
	KeyMap.push_back(SKeyMap(XK_Z, KEY_KEY_Z));
	KeyMap.push_back(SKeyMap(XK_bracketleft, KEY_OEM_4));
	KeyMap.push_back(SKeyMap(XK_backslash, KEY_OEM_5));
	KeyMap.push_back(SKeyMap(XK_bracketright, KEY_OEM_6));
	KeyMap.push_back(SKeyMap(XK_asciicircum, KEY_OEM_5));
	KeyMap.push_back(SKeyMap(XK_degree, 0)); //?
	KeyMap.push_back(SKeyMap(XK_underscore, KEY_MINUS)); //?
	KeyMap.push_back(SKeyMap(XK_grave, KEY_OEM_3));
	KeyMap.push_back(SKeyMap(XK_acute, KEY_OEM_6));
	KeyMap.push_back(SKeyMap(XK_a, KEY_KEY_A));
	KeyMap.push_back(SKeyMap(XK_b, KEY_KEY_B));
	KeyMap.push_back(SKeyMap(XK_c, KEY_KEY_C));
	KeyMap.push_back(SKeyMap(XK_d, KEY_KEY_D));
	KeyMap.push_back(SKeyMap(XK_e, KEY_KEY_E));
	KeyMap.push_back(SKeyMap(XK_f, KEY_KEY_F));
	KeyMap.push_back(SKeyMap(XK_g, KEY_KEY_G));
	KeyMap.push_back(SKeyMap(XK_h, KEY_KEY_H));
	KeyMap.push_back(SKeyMap(XK_i, KEY_KEY_I));
	KeyMap.push_back(SKeyMap(XK_j, KEY_KEY_J));
	KeyMap.push_back(SKeyMap(XK_k, KEY_KEY_K));
	KeyMap.push_back(SKeyMap(XK_l, KEY_KEY_L));
	KeyMap.push_back(SKeyMap(XK_m, KEY_KEY_M));
	KeyMap.push_back(SKeyMap(XK_n, KEY_KEY_N));
	KeyMap.push_back(SKeyMap(XK_o, KEY_KEY_O));
	KeyMap.push_back(SKeyMap(XK_p, KEY_KEY_P));
	KeyMap.push_back(SKeyMap(XK_q, KEY_KEY_Q));
	KeyMap.push_back(SKeyMap(XK_r, KEY_KEY_R));
	KeyMap.push_back(SKeyMap(XK_s, KEY_KEY_S));
	KeyMap.push_back(SKeyMap(XK_t, KEY_KEY_T));
	KeyMap.push_back(SKeyMap(XK_u, KEY_KEY_U));
	KeyMap.push_back(SKeyMap(XK_v, KEY_KEY_V));
	KeyMap.push_back(SKeyMap(XK_w, KEY_KEY_W));
	KeyMap.push_back(SKeyMap(XK_x, KEY_KEY_X));
	KeyMap.push_back(SKeyMap(XK_y, KEY_KEY_Y));
	KeyMap.push_back(SKeyMap(XK_z, KEY_KEY_Z));
	KeyMap.push_back(SKeyMap(XK_ssharp, KEY_OEM_4));
	KeyMap.push_back(SKeyMap(XK_adiaeresis, KEY_OEM_7));
	KeyMap.push_back(SKeyMap(XK_odiaeresis, KEY_OEM_3));
	KeyMap.push_back(SKeyMap(XK_udiaeresis, KEY_OEM_1));
	KeyMap.push_back(SKeyMap(XK_Super_L, KEY_LWIN));
	KeyMap.push_back(SKeyMap(XK_Super_R, KEY_RWIN));

	KeyMap.sort();
#endif
}

bool CIrrDeviceSDL2::activateJoysticks(core::array<SJoystickInfo> & joystickInfo)
{
#if defined (_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)

	joystickInfo.clear();

	u32 joystick;
	for (joystick = 0; joystick < 32; ++joystick)
	{
		// The joystick device could be here...
		core::stringc devName = "/dev/js";
		devName += joystick;

		SJoystickInfo returnInfo;
		JoystickInfo info;

		info.fd = open(devName.c_str(), O_RDONLY);
		if (-1 == info.fd)
		{
			// ...but Ubuntu and possibly other distros
			// create the devices in /dev/input
			devName = "/dev/input/js";
			devName += joystick;
			info.fd = open(devName.c_str(), O_RDONLY);
			if (-1 == info.fd)
			{
				// and BSD here
				devName = "/dev/joy";
				devName += joystick;
				info.fd = open(devName.c_str(), O_RDONLY);
			}
		}

		if (-1 == info.fd)
			continue;

#ifdef __FreeBSD__
		info.axes=2;
		info.buttons=2;
#else
		ioctl( info.fd, JSIOCGAXES, &(info.axes) );
		ioctl( info.fd, JSIOCGBUTTONS, &(info.buttons) );
		fcntl( info.fd, F_SETFL, O_NONBLOCK );
#endif

		(void)memset(&info.persistentData, 0, sizeof(info.persistentData));
		info.persistentData.EventType = irr::EET_JOYSTICK_INPUT_EVENT;
		info.persistentData.JoystickEvent.Joystick = ActiveJoysticks.size();

		// There's no obvious way to determine which (if any) axes represent a POV
		// hat, so we'll just set it to "not used" and forget about it.
		info.persistentData.JoystickEvent.POV = 65535;

		ActiveJoysticks.push_back(info);

		returnInfo.Joystick = joystick;
		returnInfo.PovHat = SJoystickInfo::POV_HAT_UNKNOWN;
		returnInfo.Axes = info.axes;
		returnInfo.Buttons = info.buttons;

#ifndef __FreeBSD__
		char name[80];
		ioctl( info.fd, JSIOCGNAME(80), name);
		returnInfo.Name = name;
#endif

		joystickInfo.push_back(returnInfo);
	}

	for (joystick = 0; joystick < joystickInfo.size(); ++joystick)
	{
		char logString[256];
		(void)sprintf(logString, "Found joystick %u, %u axes, %u buttons '%s'",
			joystick, joystickInfo[joystick].Axes,
			joystickInfo[joystick].Buttons, joystickInfo[joystick].Name.c_str());
		os::Printer::log(logString, ELL_INFORMATION);
	}

	return true;
#else
	return false;
#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
}


void CIrrDeviceSDL2::pollJoysticks()
{
#if defined (_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	if (0 == ActiveJoysticks.size())
		return;

	for (u32 j= 0; j< ActiveJoysticks.size(); ++j)
	{
		JoystickInfo & info =  ActiveJoysticks[j];

#ifdef __FreeBSD__
		struct joystick js;
		if (read(info.fd, &js, sizeof(js)) == sizeof(js))
		{
			info.persistentData.JoystickEvent.ButtonStates = js.b1 | (js.b2 << 1); /* should be a two-bit field */
			info.persistentData.JoystickEvent.Axis[0] = js.x; /* X axis */
			info.persistentData.JoystickEvent.Axis[1] = js.y; /* Y axis */
		}
#else
		struct js_event event;
		while (sizeof(event) == read(info.fd, &event, sizeof(event)))
		{
			switch(event.type & ~JS_EVENT_INIT)
			{
			case JS_EVENT_BUTTON:
				if (event.value)
						info.persistentData.JoystickEvent.ButtonStates |= (1 << event.number);
				else
						info.persistentData.JoystickEvent.ButtonStates &= ~(1 << event.number);
				break;

			case JS_EVENT_AXIS:
				if (event.number < SEvent::SJoystickEvent::NUMBER_OF_AXES)
					info.persistentData.JoystickEvent.Axis[event.number] = event.value;
				break;

			default:
				break;
			}
		}
#endif

		// Send an irrlicht joystick event once per ::run() even if no new data were received.
		(void)postEventFromUser(info.persistentData);
	}
#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
}


//! Set the current Gamma Value for the Display
bool CIrrDeviceSDL2::setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast )
{
	// TODO
	//SDL_SetWindowGammaRamp(window, red, green, blue);
	return false;
}


//! Get the current Gamma Value for the Display
bool CIrrDeviceSDL2::getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast )
{
	brightness = 0.f;
	contrast = 0.f;
	// TODO
	//u16 red_[256], green_[256], blue_[256];
	//SDL_GetWindowGammaRamp(window, red_, green_, blue_);
	return false;
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
const c8* CIrrDeviceSDL2::getTextFromClipboard() const
{
#if defined(_IRR_COMPILE_WITH_X11_)
	char *clipboardText = SDL_GetClipboardText();

	Clipboard = clipboardText;

	SDL_free(clipboardText);

	return Clipboard.c_str();

#else
	return 0;
#endif
}

//! copies text to the clipboard
void CIrrDeviceSDL2::copyToClipboard(const c8* text) const
{
#if defined(_IRR_COMPILE_WITH_X11_)
	// Actually there is no clipboard on X but applications just say they own the clipboard and return text when asked.
	// Which btw. also means that on X you lose clipboard content when closing applications.
	Clipboard = text;
	SDL_SetClipboardText(text);
#endif
}

#ifdef _IRR_COMPILE_WITH_X11_
// return true if the passed event has the type passed in parameter arg
Bool PredicateIsEventType(Display *display, XEvent *event, XPointer arg)
{
	if ( event && event->type == *(int*)arg )
	{
//		os::Printer::log("remove event:", core::stringc((int)arg).c_str(), ELL_INFORMATION);
		return True;
	}
	return False;
}
#endif //_IRR_COMPILE_WITH_X11_

//! Remove all messages pending in the system message loop
void CIrrDeviceSDL2::clearSystemMessages()
{
}


#ifdef _IRR_COMPILE_WITH_X11_

SDL_Cursor *CIrrDeviceSDL2::TextureToMonochromeCursor(irr::video::ITexture * tex, const core::rect<s32>& sourceRect, const core::position2d<s32> &hotspot)
{
	// write texture into XImage
	video::ECOLOR_FORMAT format = tex->getColorFormat();
	u32 bytesPerPixel = video::IImage::getBitsPerPixelFromFormat(format) / 8;
	u32 bytesLeftGap = sourceRect.UpperLeftCorner.X * bytesPerPixel;
	u32 bytesRightGap = tex->getPitch() - sourceRect.LowerRightCorner.X * bytesPerPixel;
	const u8* data = (const u8*)tex->lock(video::ETLM_READ_ONLY, 0);
	data += sourceRect.UpperLeftCorner.Y*tex->getPitch();
	for ( s32 y = 0; y < sourceRect.getHeight(); ++y )
	{
		data += bytesLeftGap;
		for ( s32 x = 0; x < sourceRect.getWidth(); ++x )
		{
			data += bytesPerPixel;
		}
		data += bytesRightGap;
	}

	SDL_Cursor *cursorResult = SDL_CreateCursor(data, nullptr, sourceRect.getWidth(), sourceRect.getHeight(), 0 ,0);
	tex->unlock();

	return cursorResult;
}

#ifdef _IRR_LINUX_XCURSOR_
Cursor CIrrDeviceSDL2::TextureToARGBCursor(irr::video::ITexture * tex, const core::rect<s32>& sourceRect, const core::position2d<s32> &hotspot)
{
	XcursorImage * image = XcursorImageCreate (sourceRect.getWidth(), sourceRect.getHeight());
	image->xhot = hotspot.X;
	image->yhot = hotspot.Y;

	// write texture into XcursorImage
	video::ECOLOR_FORMAT format = tex->getColorFormat();
	u32 bytesPerPixel = video::IImage::getBitsPerPixelFromFormat(format) / 8;
	u32 bytesLeftGap = sourceRect.UpperLeftCorner.X * bytesPerPixel;
	u32 bytesRightGap = tex->getPitch() - sourceRect.LowerRightCorner.X * bytesPerPixel;
	XcursorPixel* target = image->pixels;
	const u8* data = (const u8*)tex->lock(video::ETLM_READ_ONLY, 0);
	data += sourceRect.UpperLeftCorner.Y*tex->getPitch();
	for ( s32 y = 0; y < sourceRect.getHeight(); ++y )
	{
		data += bytesLeftGap;
		for ( s32 x = 0; x < sourceRect.getWidth(); ++x )
		{
			video::SColor pixelCol;
			pixelCol.setData((const void*)data, format);
			data += bytesPerPixel;

			*target = (XcursorPixel)pixelCol.color;
			++target;
		}
		data += bytesRightGap;
	}
	tex->unlock();

	Cursor cursorResult=XcursorImageLoadCursor(display, image);

	XcursorImageDestroy(image);


	return cursorResult;
}
#endif // #ifdef _IRR_LINUX_XCURSOR_

SDL_Cursor *CIrrDeviceSDL2::TextureToCursor(irr::video::ITexture * tex, const core::rect<s32>& sourceRect, const core::position2d<s32> &hotspot)
{
#ifdef _IRR_LINUX_XCURSOR_
	return TextureToARGBCursor( tex, sourceRect, hotspot );
#else
	return TextureToMonochromeCursor( tex, sourceRect, hotspot );
#endif
}
#endif	// _IRR_COMPILE_WITH_X11_


CIrrDeviceSDL2::CCursorControl::CCursorControl(CIrrDeviceSDL2* dev, bool null)
	: Device(dev)
#ifdef _IRR_COMPILE_WITH_X11_
	, PlatformBehavior(gui::ECPB_NONE), lastQuery(0)
#endif
	, IsVisible(true), Null(null), UseReferenceRect(false)
	, ActiveIcon(gui::ECI_NORMAL), ActiveIconStartTime(0)
{
#ifdef _IRR_COMPILE_WITH_X11_
	if (!Null)
	{
		SDL_ShowCursor(0);
		initCursors();
	}
#endif
}

CIrrDeviceSDL2::CCursorControl::~CCursorControl()
{
	// Do not clearCursors here as the display is already closed
	// TODO (cutealien): droping cursorcontrol earlier might work, not sure about reason why that's done in stub currently.
}

#ifdef _IRR_COMPILE_WITH_X11_
void CIrrDeviceSDL2::CCursorControl::clearCursors()
{
	if (!Null)
	for ( u32 i=0; i < Cursors.size(); ++i )
	{
		for ( u32 f=0; f < Cursors[i].Frames.size(); ++f )
		{
			SDL_FreeCursor(Cursors[i].Frames[f].IconHW);
		}
	}
}

void CIrrDeviceSDL2::CCursorControl::initCursors()
{
	/*Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_top_left_arrow)) ); //  (or XC_arrow?)
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_crosshair)) );
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_hand2)) ); // (or XC_hand1? )
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_question_arrow)) );
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_xterm)) );
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_X_cursor)) );	//  (or XC_pirate?)
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_watch)) );	// (or XC_clock?)
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_fleur)) );
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_top_right_corner)) );	// NESW not available in X11
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_top_left_corner)) );	// NWSE not available in X11
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_sb_v_double_arrow)) );
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_sb_h_double_arrow)) );
	Cursors.push_back( CursorX11(XCreateFontCursor(Device->display, XC_sb_up_arrow)) );	// (or XC_center_ptr?)
	*/
}

void CIrrDeviceSDL2::CCursorControl::update()
{
	if ( (u32)ActiveIcon < Cursors.size() && !Cursors[ActiveIcon].Frames.empty() && Cursors[ActiveIcon].FrameTime )
	{
		// update animated cursors. This could also be done by X11 in case someone wants to figure that out (this way was just easier to implement)
		u32 now = Device->getTimer()->getRealTime();
		u32 frame = ((now - ActiveIconStartTime) / Cursors[ActiveIcon].FrameTime) % Cursors[ActiveIcon].Frames.size();
		SDL_SetCursor(Cursors[ActiveIcon].Frames[frame].IconHW);
	}
}
#endif

//! Sets the active cursor icon
void CIrrDeviceSDL2::CCursorControl::setActiveIcon(gui::ECURSOR_ICON iconId)
{
#ifdef _IRR_COMPILE_WITH_X11_
	if ( iconId >= (s32)Cursors.size() )
		return;

	if ( Cursors[iconId].Frames.size() )
		SDL_SetCursor(Cursors[iconId].Frames[0].IconHW);

	ActiveIconStartTime = Device->getTimer()->getRealTime();
	ActiveIcon = iconId;
#endif
}


//! Add a custom sprite as cursor icon.
gui::ECURSOR_ICON CIrrDeviceSDL2::CCursorControl::addIcon(const gui::SCursorSprite& icon)
{
#ifdef _IRR_COMPILE_WITH_X11_
	if ( icon.SpriteId >= 0 )
	{
		CursorX11 cX11;
		cX11.FrameTime = icon.SpriteBank->getSprites()[icon.SpriteId].frameTime;
		for ( u32 i=0; i < icon.SpriteBank->getSprites()[icon.SpriteId].Frames.size(); ++i )
		{
			irr::u32 texId = icon.SpriteBank->getSprites()[icon.SpriteId].Frames[i].textureNumber;
			irr::u32 rectId = icon.SpriteBank->getSprites()[icon.SpriteId].Frames[i].rectNumber;
			irr::core::rect<s32> rectIcon = icon.SpriteBank->getPositions()[rectId];
			SDL_Cursor *cursor = Device->TextureToCursor(icon.SpriteBank->getTexture(texId), rectIcon, icon.HotSpot);
			cX11.Frames.push_back( CursorFrameX11(cursor) );
		}

		Cursors.push_back( cX11 );

		return (gui::ECURSOR_ICON)(Cursors.size() - 1);
	}
#endif
	return gui::ECI_NORMAL;
}

//! replace the given cursor icon.
void CIrrDeviceSDL2::CCursorControl::changeIcon(gui::ECURSOR_ICON iconId, const gui::SCursorSprite& icon)
{
#ifdef _IRR_COMPILE_WITH_X11_
	if ( iconId >= (s32)Cursors.size() )
		return;

	for ( u32 i=0; i < Cursors[iconId].Frames.size(); ++i )
		SDL_FreeCursor(Cursors[iconId].Frames[i].IconHW);

	if ( icon.SpriteId >= 0 )
	{
		CursorX11 cX11;
		cX11.FrameTime = icon.SpriteBank->getSprites()[icon.SpriteId].frameTime;
		for ( u32 i=0; i < icon.SpriteBank->getSprites()[icon.SpriteId].Frames.size(); ++i )
		{
			irr::u32 texId = icon.SpriteBank->getSprites()[icon.SpriteId].Frames[i].textureNumber;
			irr::u32 rectId = icon.SpriteBank->getSprites()[icon.SpriteId].Frames[i].rectNumber;
			irr::core::rect<s32> rectIcon = icon.SpriteBank->getPositions()[rectId];
			SDL_Cursor *cursor = Device->TextureToCursor(icon.SpriteBank->getTexture(texId), rectIcon, icon.HotSpot);
			cX11.Frames.push_back( CursorFrameX11(cursor) );
		}

		Cursors[iconId] = cX11;
	}
#endif
}

irr::core::dimension2di CIrrDeviceSDL2::CCursorControl::getSupportedIconSize() const
{
	// this returns the closest match that is smaller or same size, so we just pass a value which should be large enough for cursors
	unsigned int width=0, height=0;
#ifdef _IRR_COMPILE_WITH_X11_
	//XQueryBestCursor(Device->display, Device->window, 64, 64, &width, &height);
#endif
	return core::dimension2di(width, height);
}

} // end namespace

#endif // _IRR_COMPILE_WITH_X11_DEVICE_

