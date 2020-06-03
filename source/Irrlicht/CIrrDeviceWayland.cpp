// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceWayland.h"

#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_

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
#include <X11/XKBlib.h>
#include <X11/Xatom.h>

#ifdef _IRR_LINUX_XCURSOR_
#include <X11/Xcursor/Xcursor.h>
#endif

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
				io::IFileSystem* io, CIrrDeviceLinux* device);
	}
} // end namespace irr

namespace irr
{
//! constructor
CIrrDeviceWayland::CIrrDeviceWayland(const SIrrlichtCreationParameters& param)
	: CIrrDeviceLinux(param),
	Width(param.WindowSize.Width), Height(param.WindowSize.Height),
	WindowHasFocus(false), WindowMinimized(false),
	UseGLXWindow(false),
	ExternalWindow(false), AutorepeatSupport(0)
{
	#ifdef _DEBUG
	setDebugName("CIrrDeviceWayland");
	#endif

	// create keymap
	createKeyMap();

	// create window
	if (CreationParams.DriverType != video::EDT_NULL)
	{
		// create the window, only if we do not use the null device
		if (!createWindow())
			return;
	}

	// create driver
	createDriver();

	if (!VideoDriver)
		return;

	createGUIAndScene();
}


//! destructor
CIrrDeviceWayland::~CIrrDeviceWayland()
{
	if (display)
	{
		#ifdef _IRR_COMPILE_WITH_OPENGL_
		if (Context)
		{
			if (glxWin)
			{
				if (!glXMakeContextCurrent(display, None, None, NULL))
					os::Printer::log("Could not release glx context.", ELL_WARNING);
			}
			else
			{
				if (!glXMakeCurrent(display, None, NULL))
					os::Printer::log("Could not release glx context.", ELL_WARNING);
			}
			glXDestroyContext(display, Context);
			if (glxWin)
				glXDestroyWindow(display, glxWin);
		}
		#endif // #ifdef _IRR_COMPILE_WITH_OPENGL_

		// Reset fullscreen resolution change
		switchToFullscreen(true);

		if (!ExternalWindow)
		{
			// @TODO ?
		}
	}
}


bool CIrrDeviceWayland::switchToFullscreen(bool reset)
{
	if (!CreationParams.Fullscreen)
		return true;
	if (reset)
	{
		return true;
	}

	getVideoModeList();

	{
		os::Printer::log("VidMode or RandR extension must be installed to allow Irrlicht "
		"to switch to fullscreen mode. Running in windowed mode instead.", ELL_WARNING);
		CreationParams.Fullscreen = false;
	}
	return CreationParams.Fullscreen;
}


bool CIrrDeviceWayland::createWindow()
{
#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_
	switchToFullscreen();
#endif
	return true;
}


//! create the driver
void CIrrDeviceWayland::createDriver()
{
	switch(CreationParams.DriverType)
	{
#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_

	case video::EDT_SOFTWARE:
		#ifdef _IRR_COMPILE_WITH_SOFTWARE_
		VideoDriver = video::createSoftwareDriver(CreationParams.WindowSize, CreationParams.Fullscreen, FileSystem, this);
		#else
		os::Printer::log("No Software driver support compiled in.", ELL_ERROR);
		#endif
		break;

	case video::EDT_BURNINGSVIDEO:
		#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_
		VideoDriver = video::createBurningVideoDriver(CreationParams, FileSystem, this);
		#else
		os::Printer::log("Burning's video driver was not compiled in.", ELL_ERROR);
		#endif
		break;

	case video::EDT_OPENGL:
		#ifdef _IRR_COMPILE_WITH_OPENGL_
		if (Context)
			VideoDriver = video::createOpenGLDriver(CreationParams, FileSystem, this);
		#else
		os::Printer::log("No OpenGL support compiled in.", ELL_ERROR);
		#endif
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
		os::Printer::log("No Wayland support compiled in. Only Null driver available.", ELL_ERROR);
		break;
#endif
	}
}


//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceWayland::run()
{
	os::Timer::tick();

	if (!Close)
		pollJoysticks();

	return !Close;
}


//! sets the caption of the window
void CIrrDeviceWayland::setWindowCaption(const wchar_t* text)
{
}


//! presents a surface in the client area
bool CIrrDeviceWayland::present(video::IImage* image, void* windowId, core::rect<s32>* srcRect)
{
	return true;
}

//! notifies the device that it should close itself
void CIrrDeviceWayland::closeDevice()
{
	Close = true;
}


//! returns if window is active. if not, nothing need to be drawn
bool CIrrDeviceWayland::isWindowActive() const
{
	return (WindowHasFocus && !WindowMinimized);
}


//! returns if window has focus.
bool CIrrDeviceWayland::isWindowFocused() const
{
	return WindowHasFocus;
}


//! returns if window is minimized.
bool CIrrDeviceWayland::isWindowMinimized() const
{
	return WindowMinimized;
}


//! returns color format of the window.
video::ECOLOR_FORMAT CIrrDeviceWayland::getColorFormat() const
{
		return video::ECF_R5G6B5;
}


//! Sets if the window should be resizable in windowed mode.
void CIrrDeviceWayland::setResizable(bool resize)
{
}


//! Return pointer to a list with all video modes supported by the gfx adapter.
video::IVideoModeList* CIrrDeviceWayland::getVideoModeList()
{
	return VideoModeList;
}


//! Minimize window
void CIrrDeviceWayland::minimizeWindow()
{
}


//! Maximize window
void CIrrDeviceWayland::maximizeWindow()
{
}


//! Restore original window size
void CIrrDeviceWayland::restoreWindow()
{
}


void CIrrDeviceWayland::createKeyMap()
{
}


//! Set the current Gamma Value for the Display
bool CIrrDeviceWayland::setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast )
{
	return false;
}


//! Get the current Gamma Value for the Display
bool CIrrDeviceWayland::getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast )
{
	brightness = 0.f;
	contrast = 0.f;
	return false;
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
const c8* CIrrDeviceWayland::getTextFromClipboard() const
{
	return 0;
}

//! copies text to the clipboard
void CIrrDeviceWayland::copyToClipboard(const c8* text) const
{
}

//! Remove all messages pending in the system message loop
void CIrrDeviceWayland::clearSystemMessages()
{
}

} // end namespace

#endif // _IRR_COMPILE_WITH_WAYLAND_DEVICE_

