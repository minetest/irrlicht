// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceWayland.h"

#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
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
		IVideoDriver* createWaylandOpenGLDriver(const SIrrlichtCreationParameters& params,
				io::IFileSystem* io, CIrrDeviceLinux* device);
	}

} // end namespace irr

namespace irr
{

const struct wl_registry_listener CIrrDeviceWayland::waylandRegistryListener = {
	.global = CIrrDeviceWayland::waylandRegistry,
	.global_remove = CIrrDeviceWayland::waylandRegistryRemove
};

// TODO: do something with those
struct pointer_data {
    struct wl_surface *surface;
    struct wl_buffer *buffer;
    int32_t hot_spot_x;
    int32_t hot_spot_y;
    struct wl_surface *target_surface;
};

static void pointer_enter(void *data,
    wl_pointer *wl_pointer,
    uint32_t serial, struct wl_surface *surface,
    wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    //pointer_data *pd = (pointer_data *) wl_pointer_get_user_data(wl_pointer);
    //pd->target_surface = surface;
    //wl_surface_attach(pd->surface,
    //    pd->buffer, 0, 0);
    //wl_surface_commit(pd->surface);
    //wl_pointer_set_cursor(wl_pointer, serial,
    //    pd->surface, pd->hot_spot_x,
    //    pd->hot_spot_y);
}

static void pointer_leave(void *data,
    struct wl_pointer *wl_pointer, uint32_t serial,
    struct wl_surface *wl_surface) { }

static void pointer_motion(void *data,
    struct wl_pointer *wl_pointer, uint32_t time,
    wl_fixed_t surface_x, wl_fixed_t surface_y) { }

static void pointer_button(void *data,
    struct wl_pointer *wl_pointer, uint32_t serial,
    uint32_t time, uint32_t button, uint32_t state)
{
    //pointer_data *pd = (pointer_data *) wl_pointer_get_user_data(wl_pointer);
    //void (*callback)(uint32_t) = dynamic_cast<void(uint32_t)>(wl_surface_get_user_data(
    //    pd->target_surface));
    //if (callback != NULL)
    //    callback(button);
}

static void pointer_axis(void *data,
    struct wl_pointer *wl_pointer, uint32_t time,
    uint32_t axis, wl_fixed_t value) { }

// END TODO

const struct wl_pointer_listener CIrrDeviceWayland::waylandPointerListener = {
    .enter = pointer_enter,
    .leave = pointer_leave,
    .motion = pointer_motion,
    .button = pointer_button,
    .axis = pointer_axis
};


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
		if (!createWindow()) {
			return;
		}
	}

	// create driver
	createDriver();

	if (!VideoDriver) {
		return;
	}

	// create cursor control
	CursorControl = new CCursorControl(this, CreationParams.DriverType == video::EDT_NULL);

	createGUIAndScene();
}


//! destructor
CIrrDeviceWayland::~CIrrDeviceWayland()
{
	if (mDisplay)
	{
		// wayland cleanup
		eglDestroyContext(mDisplay, mContext);
		wl_pointer_destroy(mSeatPointer);
		wl_seat_destroy(mSeat);
		wl_shell_destroy(mShell);
		wl_shm_destroy(mShm);
		wl_compositor_destroy(mCompositor);
		wl_display_disconnect(mDisplay);

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


void CIrrDeviceWayland::waylandRegistry(void *data,
	wl_registry *registry, uint32_t name,
	const char *interface, uint32_t version)
{
	CIrrDeviceWayland *self = (CIrrDeviceWayland *)data;

	if (strcmp(interface, wl_compositor_interface.name) == 0)
	{
		self->mCompositor = (wl_compositor *) wl_registry_bind(registry, name,
			&wl_compositor_interface, std::min(version, (uint32_t)4));
	}
	else if (strcmp(interface, wl_shm_interface.name) == 0)
	{
		self->mShm = (wl_shm *) wl_registry_bind(registry, name,
			&wl_shm_interface, std::min(version, (uint32_t)1));
	}
	else if (strcmp(interface, wl_shell_interface.name) == 0)
	{
		self->mShell = (wl_shell *)wl_registry_bind(registry, name,
			&wl_shell_interface, std::min(version, (uint32_t)1));
	}
	else if (strcmp(interface, wl_seat_interface.name) == 0)
	{
		self->mSeat = (wl_seat *)wl_registry_bind(registry, name,
			&wl_seat_interface, std::min(version, (uint32_t)2));
		self->mSeatPointer = wl_seat_get_pointer(self->mSeat);
		wl_pointer_add_listener(self->mSeatPointer, &CIrrDeviceWayland::waylandPointerListener,
			NULL);
	}
	else
	{
		printf("Got a unknown wayland registry event for %s id %d\n", interface, name);
	}
}

void CIrrDeviceWayland::waylandRegistryRemove(void *a,
	struct wl_registry *b, uint32_t c)
{
}

bool CIrrDeviceWayland::createWindow()
{
#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_
#ifdef _DEBUG
	os::Printer::log("Creating Wayland window...", ELL_INFORMATION);
#endif

	mDisplay = wl_display_connect(nullptr);
    if (!mDisplay)
	{
		os::Printer::log("Unable to connect to wayland subsystem", ELL_WARNING);
		return false;
    }

    wl_registry *registry = wl_display_get_registry(mDisplay);
    wl_registry_add_listener(registry, &waylandRegistryListener, this);
	wl_display_dispatch(mDisplay);
    wl_display_roundtrip(mDisplay);
    wl_registry_destroy(registry);

	wl_surface *surface = wl_compositor_create_surface(mCompositor);
    if (surface == NULL)
	{
		os::Printer::log("Failed to create wayland surface", ELL_ERROR);
		return false;
    }

    wl_shell_surface *shell_surface = wl_shell_get_shell_surface(mShell, surface);
    if (shell_surface == NULL)
	{
		os::Printer::log("Failed to create wayland shell surface", ELL_ERROR);
		return false;
    }

    wl_shell_surface_set_toplevel(shell_surface);

	mRegion = wl_compositor_create_region(mCompositor);

	wl_region_add(mRegion, 0, 0, CreationParams.WindowSize.Width, CreationParams.WindowSize.Height);
	wl_surface_set_opaque_region(surface, mRegion);

	mEGLWindow =
		wl_egl_window_create(surface, CreationParams.WindowSize.Width, CreationParams.WindowSize.Height);

	if (mEGLWindow == EGL_NO_SURFACE)
	{
		os::Printer::log("No wayland window found ?!", ELL_ERROR);
		return false;
	}

	if (!initEGL())
		return false;

	switchToFullscreen();
#endif
	return true;
}


bool CIrrDeviceWayland::initEGL()
{
	EGLint numConfigs;
	EGLConfig config;
	EGLint fbAttribs[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE,        8,
		EGL_GREEN_SIZE,      8,
		EGL_BLUE_SIZE,       8,
		EGL_NONE
	};
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
	EGLDisplay display = eglGetDisplay(mDisplay);
	if (display == EGL_NO_DISPLAY)
	{
		os::Printer::log("Failed to init EGL: no display found.", ELL_ERROR);
		return false;
	}

	mEGLDisplay = display;

	EGLint majorVersion;
	EGLint minorVersion;

   // Initialize EGL
   if (!eglInitialize(display, &majorVersion, &minorVersion))
   {
      os::Printer::log("Failed to init EGL: init failure.", ELL_ERROR);
      return false;
   }

   // Get configs
   if ((eglGetConfigs(display, NULL, 0, &numConfigs) != EGL_TRUE) || (numConfigs == 0))
   {
      os::Printer::log("Failed to init EGL: failed to retrieve configs.", ELL_ERROR);
      return false;
   }

   if ((eglChooseConfig(display, fbAttribs, &config, 1, &numConfigs) != EGL_TRUE) || (numConfigs != 1))
   {
      os::Printer::log("Failed to init EGL: failed to choose config.", ELL_ERROR);
      return EGL_FALSE;
   }

   EGLSurface surface = eglCreateWindowSurface(display, config, mEGLWindow, NULL);
   if (surface == EGL_NO_SURFACE)
   {
      os::Printer::log("Failed to init EGL: failed to create surface.", ELL_ERROR);
      return EGL_FALSE;
   }

   mEGLSurface = surface;

   // Create a GL context
   EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
   if (context == EGL_NO_CONTEXT)
   {
      os::Printer::log("Failed to init EGL: failed to create context.", ELL_ERROR);
      return EGL_FALSE;
   }

   mContext = context;

   // Make the context current
   if (!eglMakeCurrent(display, mEGLSurface, mEGLSurface, mContext))
   {
	  os::Printer::log("Failed to init EGL: Could not make the current window current.", ELL_ERROR);
      return EGL_FALSE;
   }

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
		if (mContext)
			VideoDriver = video::createWaylandOpenGLDriver(CreationParams, FileSystem, this);
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

	wl_display_dispatch_pending(mDisplay);

	glClearColor(0.5, 0.3, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

	eglSwapBuffers(mEGLDisplay, mEGLSurface);
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

