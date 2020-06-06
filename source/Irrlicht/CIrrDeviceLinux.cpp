// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceLinux.h"

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
CIrrDeviceLinux::CIrrDeviceLinux(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param),
#ifdef _IRR_COMPILE_WITH_X11_
	display(0), visual(0), screennr(0), window(0), StdHints(0), SoftwareImage(0),
#endif
	Width(param.WindowSize.Width), Height(param.WindowSize.Height),
	WindowHasFocus(false), WindowMinimized(false),
	UseXVidMode(false), UseXRandR(false), UseGLXWindow(false),
	ExternalWindow(false), AutorepeatSupport(0)
{
	#ifdef _DEBUG
	setDebugName("CIrrDeviceLinux");
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
}


//! destructor
CIrrDeviceLinux::~CIrrDeviceLinux()
{
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

bool CIrrDeviceLinux::switchToFullscreen(bool reset)
{
	if (!CreationParams.Fullscreen)
		return true;
	if (reset)
	{
#ifdef _IRR_LINUX_X11_VIDMODE_
		if (UseXVidMode && CreationParams.Fullscreen)
		{
			XF86VidModeSwitchToMode(display, screennr, &oldVideoMode);
			XF86VidModeSetViewPort(display, screennr, 0, 0);
		}
		#endif
		#ifdef _IRR_LINUX_X11_RANDR_
		if (UseXRandR && CreationParams.Fullscreen)
		{
			XRRScreenConfiguration *config=XRRGetScreenInfo(display,DefaultRootWindow(display));
			XRRSetScreenConfig(display,config,DefaultRootWindow(display),oldRandrMode,oldRandrRotation,CurrentTime);
			XRRFreeScreenConfigInfo(config);
		}
		#endif
		return true;
	}

	getVideoModeList();
	#if defined(_IRR_LINUX_X11_VIDMODE_) || defined(_IRR_LINUX_X11_RANDR_)
	s32 eventbase, errorbase;
	s32 bestMode = -1;
	#endif

	#ifdef _IRR_LINUX_X11_VIDMODE_
	if (XF86VidModeQueryExtension(display, &eventbase, &errorbase))
	{
		// enumerate video modes
		s32 modeCount;
		XF86VidModeModeInfo** modes;

		XF86VidModeGetAllModeLines(display, screennr, &modeCount, &modes);

		// find fitting mode
		for (s32 i = 0; i<modeCount; ++i)
		{
			if (bestMode==-1 && modes[i]->hdisplay >= Width && modes[i]->vdisplay >= Height)
				bestMode = i;
			else if (bestMode!=-1 &&
					modes[i]->hdisplay >= Width &&
					modes[i]->vdisplay >= Height &&
					modes[i]->hdisplay <= modes[bestMode]->hdisplay &&
					modes[i]->vdisplay <= modes[bestMode]->vdisplay)
				bestMode = i;
		}
		if (bestMode != -1)
		{
			os::Printer::log("Starting vidmode fullscreen mode...", ELL_INFORMATION);
			os::Printer::log("hdisplay: ", core::stringc(modes[bestMode]->hdisplay).c_str(), ELL_INFORMATION);
			os::Printer::log("vdisplay: ", core::stringc(modes[bestMode]->vdisplay).c_str(), ELL_INFORMATION);

			XF86VidModeSwitchToMode(display, screennr, modes[bestMode]);
			XF86VidModeSetViewPort(display, screennr, 0, 0);
			UseXVidMode=true;
		}
		else
		{
			os::Printer::log("Could not find specified video mode, running windowed.", ELL_WARNING);
			CreationParams.Fullscreen = false;
		}

		XFree(modes);
	}
	else
	#endif
	#ifdef _IRR_LINUX_X11_RANDR_
	if (XRRQueryExtension(display, &eventbase, &errorbase))
	{
		s32 modeCount;
		XRRScreenConfiguration *config=XRRGetScreenInfo(display,DefaultRootWindow(display));
		XRRScreenSize *modes=XRRConfigSizes(config,&modeCount);
		for (s32 i = 0; i<modeCount; ++i)
		{
			if (bestMode==-1 && (u32)modes[i].width >= Width && (u32)modes[i].height >= Height)
				bestMode = i;
			else if (bestMode!=-1 &&
					(u32)modes[i].width >= Width &&
					(u32)modes[i].height >= Height &&
					modes[i].width <= modes[bestMode].width &&
					modes[i].height <= modes[bestMode].height)
				bestMode = i;
		}
		if (bestMode != -1)
		{
			os::Printer::log("Starting randr fullscreen mode...", ELL_INFORMATION);
			os::Printer::log("width: ", core::stringc(modes[bestMode].width).c_str(), ELL_INFORMATION);
			os::Printer::log("height: ", core::stringc(modes[bestMode].height).c_str(), ELL_INFORMATION);

			XRRSetScreenConfig(display,config,DefaultRootWindow(display),bestMode,oldRandrRotation,CurrentTime);
			UseXRandR=true;
		}
		XRRFreeScreenConfigInfo(config);
	}
	else
	#endif
	{
		os::Printer::log("VidMode or RandR extension must be installed to allow Irrlicht "
		"to switch to fullscreen mode. Running in windowed mode instead.", ELL_WARNING);
		CreationParams.Fullscreen = false;
	}
	return CreationParams.Fullscreen;
}


bool CIrrDeviceLinux::createWindow()
{
	return true;
}


//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceLinux::run()
{
	os::Timer::tick();

	if (!Close)
		pollJoysticks();

	return !Close;
}


//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDeviceLinux::yield()
{
	struct timespec ts = {0,1};
	nanosleep(&ts, NULL);
}


//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceLinux::sleep(u32 timeMs, bool pauseTimer=false)
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
void CIrrDeviceLinux::setWindowCaption(const wchar_t* text)
{
#ifdef _IRR_COMPILE_WITH_X11_
	if (CreationParams.DriverType == video::EDT_NULL)
		return;

	XTextProperty txt;
	if (Success==XwcTextListToTextProperty(display, const_cast<wchar_t**>(&text),
				1, XStdICCTextStyle, &txt))
	{
		XSetWMName(display, window, &txt);
		XSetWMIconName(display, window, &txt);
		XFree(txt.value);
	}
#endif
}


//! presents a surface in the client area
bool CIrrDeviceLinux::present(video::IImage* image, void* windowId, core::rect<s32>* srcRect)
{
#ifdef _IRR_COMPILE_WITH_X11_
	// this is only necessary for software drivers.
	if (!SoftwareImage)
		return true;

	// thx to Nadav, who send me some clues of how to display the image
	// to the X Server.

	const u32 destwidth = SoftwareImage->width;
	const u32 minWidth = core::min_(image->getDimension().Width, destwidth);
	const u32 destPitch = SoftwareImage->bytes_per_line;

	video::ECOLOR_FORMAT destColor;
	switch (SoftwareImage->bits_per_pixel)
	{
		case 16:
			if (SoftwareImage->depth==16)
				destColor = video::ECF_R5G6B5;
			else
				destColor = video::ECF_A1R5G5B5;
		break;
		case 24: destColor = video::ECF_R8G8B8; break;
		case 32: destColor = video::ECF_A8R8G8B8; break;
		default:
			os::Printer::log("Unsupported screen depth.");
			return false;
	}

	u8* srcdata = reinterpret_cast<u8*>(image->lock());
	u8* destData = reinterpret_cast<u8*>(SoftwareImage->data);

	const u32 destheight = SoftwareImage->height;
	const u32 srcheight = core::min_(image->getDimension().Height, destheight);
	const u32 srcPitch = image->getPitch();
	for (u32 y=0; y!=srcheight; ++y)
	{
		video::CColorConverter::convert_viaFormat(srcdata,image->getColorFormat(), minWidth, destData, destColor);
		srcdata+=srcPitch;
		destData+=destPitch;
	}
	image->unlock();

	GC gc = DefaultGC(display, DefaultScreen(display));
	Window myWindow=window;
	if (windowId)
		myWindow = reinterpret_cast<Window>(windowId);
	XPutImage(display, myWindow, gc, SoftwareImage, 0, 0, 0, 0, destwidth, destheight);
#endif
	return true;
}


//! notifies the device that it should close itself
void CIrrDeviceLinux::closeDevice()
{
	Close = true;
}


//! returns if window is active. if not, nothing need to be drawn
bool CIrrDeviceLinux::isWindowActive() const
{
	return (WindowHasFocus && !WindowMinimized);
}


//! returns if window has focus.
bool CIrrDeviceLinux::isWindowFocused() const
{
	return WindowHasFocus;
}


//! returns if window is minimized.
bool CIrrDeviceLinux::isWindowMinimized() const
{
	return WindowMinimized;
}


//! returns color format of the window.
video::ECOLOR_FORMAT CIrrDeviceLinux::getColorFormat() const
{
		return video::ECF_R5G6B5;
}


//! Sets if the window should be resizable in windowed mode.
void CIrrDeviceLinux::setResizable(bool resize)
{
#ifdef _IRR_COMPILE_WITH_X11_
	if (CreationParams.DriverType == video::EDT_NULL || CreationParams.Fullscreen )
		return;

	XUnmapWindow(display, window);
	if ( !resize )
	{
		// Must be heap memory because data size depends on X Server
		XSizeHints *hints = XAllocSizeHints();
		hints->flags=PSize|PMinSize|PMaxSize;
		hints->min_width=hints->max_width=hints->base_width=Width;
		hints->min_height=hints->max_height=hints->base_height=Height;
		XSetWMNormalHints(display, window, hints);
		XFree(hints);
	}
	else
	{
		XSetWMNormalHints(display, window, StdHints);
	}
	XMapWindow(display, window);
	XFlush(display);
#endif // #ifdef _IRR_COMPILE_WITH_X11_
}


//! Return pointer to a list with all video modes supported by the gfx adapter.
video::IVideoModeList* CIrrDeviceLinux::getVideoModeList()
{
#ifdef _IRR_COMPILE_WITH_X11_
	if (!VideoModeList->getVideoModeCount())
	{
		bool temporaryDisplay = false;

		if (!display)
		{
			display = XOpenDisplay(0);
			temporaryDisplay=true;
		}
		if (display)
		{
			#if defined(_IRR_LINUX_X11_VIDMODE_) || defined(_IRR_LINUX_X11_RANDR_)
			s32 eventbase, errorbase;
			s32 defaultDepth=DefaultDepth(display,screennr);
			#endif

			#ifdef _IRR_LINUX_X11_VIDMODE_
			if (XF86VidModeQueryExtension(display, &eventbase, &errorbase))
			{
				// enumerate video modes
				int modeCount;
				XF86VidModeModeInfo** modes;

				XF86VidModeGetAllModeLines(display, screennr, &modeCount, &modes);

				// save current video mode
				oldVideoMode = *modes[0];

				// find fitting mode

				VideoModeList->setDesktop(defaultDepth, core::dimension2d<u32>(
					modes[0]->hdisplay, modes[0]->vdisplay));
				for (int i = 0; i<modeCount; ++i)
				{
					VideoModeList->addMode(core::dimension2d<u32>(
						modes[i]->hdisplay, modes[i]->vdisplay), defaultDepth);
				}
				XFree(modes);
			}
			else
			#endif
			#ifdef _IRR_LINUX_X11_RANDR_
			if (XRRQueryExtension(display, &eventbase, &errorbase))
			{
				int modeCount;
				XRRScreenConfiguration *config=XRRGetScreenInfo(display,DefaultRootWindow(display));
				oldRandrMode=XRRConfigCurrentConfiguration(config,&oldRandrRotation);
				XRRScreenSize *modes=XRRConfigSizes(config,&modeCount);
				VideoModeList->setDesktop(defaultDepth, core::dimension2d<u32>(
					modes[oldRandrMode].width, modes[oldRandrMode].height));
				for (int i = 0; i<modeCount; ++i)
				{
					VideoModeList->addMode(core::dimension2d<u32>(
						modes[i].width, modes[i].height), defaultDepth);
				}
				XRRFreeScreenConfigInfo(config);
			}
			else
			#endif
			{
				os::Printer::log("VidMode or RandR X11 extension requireed for VideoModeList." , ELL_WARNING);
			}
		}
		if (display && temporaryDisplay)
		{
			XCloseDisplay(display);
			display=0;
		}
	}
#endif

	return VideoModeList;
}


//! Minimize window
void CIrrDeviceLinux::minimizeWindow()
{
}


//! Maximize window
void CIrrDeviceLinux::maximizeWindow()
{
}


//! Restore original window size
void CIrrDeviceLinux::restoreWindow()
{
}

bool CIrrDeviceLinux::activateJoysticks(core::array<SJoystickInfo> & joystickInfo)
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


void CIrrDeviceLinux::pollJoysticks()
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
bool CIrrDeviceLinux::setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast )
{
	#if defined(_IRR_LINUX_X11_VIDMODE_) || defined(_IRR_LINUX_X11_RANDR_)
	s32 eventbase, errorbase;
	#ifdef _IRR_LINUX_X11_VIDMODE_
	if (XF86VidModeQueryExtension(display, &eventbase, &errorbase))
	{
		XF86VidModeGamma gamma;
		gamma.red=red;
		gamma.green=green;
		gamma.blue=blue;
		XF86VidModeSetGamma(display, screennr, &gamma);
		return true;
	}
	#endif
	#if defined(_IRR_LINUX_X11_VIDMODE_) && defined(_IRR_LINUX_X11_RANDR_)
	else
	#endif
	#ifdef _IRR_LINUX_X11_RANDR_
	if (XRRQueryExtension(display, &eventbase, &errorbase))
	{
		XRRQueryVersion(display, &eventbase, &errorbase); // major, minor
		if (eventbase>=1 && errorbase>1)
		{
		#if (RANDR_MAJOR>1 || RANDR_MINOR>1)
			XRRCrtcGamma *gamma = XRRGetCrtcGamma(display, screennr);
			if (gamma)
			{
				*gamma->red=(u16)red;
				*gamma->green=(u16)green;
				*gamma->blue=(u16)blue;
				XRRSetCrtcGamma(display, screennr, gamma);
				XRRFreeGamma(gamma);
				return true;
			}
		#endif
		}
	}
	#endif
	#endif
	return false;
}


//! Get the current Gamma Value for the Display
bool CIrrDeviceLinux::getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast )
{
	brightness = 0.f;
	contrast = 0.f;
	#if defined(_IRR_LINUX_X11_VIDMODE_) || defined(_IRR_LINUX_X11_RANDR_)
	s32 eventbase, errorbase;
	#ifdef _IRR_LINUX_X11_VIDMODE_
	if (XF86VidModeQueryExtension(display, &eventbase, &errorbase))
	{
		XF86VidModeGamma gamma;
		XF86VidModeGetGamma(display, screennr, &gamma);
		red = gamma.red;
		green = gamma.green;
		blue = gamma.blue;
		return true;
	}
	#endif
	#if defined(_IRR_LINUX_X11_VIDMODE_) && defined(_IRR_LINUX_X11_RANDR_)
	else
	#endif
	#ifdef _IRR_LINUX_X11_RANDR_
	if (XRRQueryExtension(display, &eventbase, &errorbase))
	{
		XRRQueryVersion(display, &eventbase, &errorbase); // major, minor
		if (eventbase>=1 && errorbase>1)
		{
		#if (RANDR_MAJOR>1 || RANDR_MINOR>1)
			XRRCrtcGamma *gamma = XRRGetCrtcGamma(display, screennr);
			if (gamma)
			{
				red = *gamma->red;
				green = *gamma->green;
				blue= *gamma->blue;
				XRRFreeGamma(gamma);
				return true;
			}
		#endif
		}
	}
	#endif
	#endif
	return false;
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
const c8* CIrrDeviceLinux::getTextFromClipboard() const
{
	return 0;
}

//! copies text to the clipboard
void CIrrDeviceLinux::copyToClipboard(const c8* text) const
{
}

//! Remove all messages pending in the system message loop
void CIrrDeviceLinux::clearSystemMessages()
{
}

void CIrrDeviceLinux::initXAtoms()
{
}


} // end namespace

#endif // _IRR_COMPILE_WITH_X11_DEVICE_

