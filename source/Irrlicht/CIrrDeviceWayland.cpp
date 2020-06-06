// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceWayland.h"

#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <sys/mman.h>
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

#define IRR_KEY_PRESS_INTERVAL 5

namespace irr
{
	namespace video
	{
		IVideoDriver* createWaylandOpenGLDriver(const SIrrlichtCreationParameters& params,
				io::IFileSystem* io, CIrrDeviceWayland* device);
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

void CIrrDeviceWayland::waylandSeatCapabilities(void *data, struct wl_seat *wl_seat, uint32_t capabilities)
{
       CIrrDeviceWayland *self = (CIrrDeviceWayland *)data;

       bool have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;

       if (have_pointer && self->mSeatPointer == NULL) {
               self->mSeatPointer = wl_seat_get_pointer(self->mSeat);
			   printf("register pointer listener\n");
               wl_pointer_add_listener(self->mSeatPointer,
                               &CIrrDeviceWayland::waylandPointerListener, self);
       } else if (!have_pointer && self->mSeatPointer != nullptr) {
               wl_pointer_release(self->mSeatPointer);
               self->mSeatPointer = nullptr;
       }

	   bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

	   if (have_keyboard && self->mSeatKeyboard == nullptr) {
		   printf("register keyboard listener\n");
			self->mSeatKeyboard = wl_seat_get_keyboard(self->mSeat);
               wl_keyboard_add_listener(self->mSeatKeyboard,
                               &CIrrDeviceWayland::waylandKeyboardListener, self);
       } else if (!have_keyboard && self->mSeatKeyboard != nullptr) {
               wl_keyboard_release(self->mSeatKeyboard);
			self->mSeatKeyboard =nullptr;
       }
}

static void wl_seat_name(void *data, struct wl_seat *wl_seat, const char *name)
{
       fprintf(stderr, "seat name: %s\n", name);
}

const struct wl_seat_listener CIrrDeviceWayland::waylandSeatListener = {
    .capabilities = CIrrDeviceWayland::waylandSeatCapabilities,
	.name = wl_seat_name,
};

const struct wl_pointer_listener CIrrDeviceWayland::waylandPointerListener = {
    .enter = pointer_enter,
    .leave = pointer_leave,
    .motion = pointer_motion,
    .button = pointer_button,
    .axis = pointer_axis
};

void CIrrDeviceWayland::wl_keyboard_keymap(void *data, wl_keyboard *wl_keyboard,
               uint32_t format, int32_t fd, uint32_t size)
{
	printf("wl_keyboard_keymap\n");
       CIrrDeviceWayland *self = (CIrrDeviceWayland *)data;
       //assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

       char *map_shm = (char *) mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
       //assert(map_shm != MAP_FAILED);

       xkb_keymap *xkb_keymap = xkb_keymap_new_from_string(
                       self->mKBContext, map_shm,
                       XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
       munmap(map_shm, size);
       close(fd);

       xkb_state *xkb_state = xkb_state_new(xkb_keymap);
       xkb_keymap_unref(self->mKBKeymap);
       xkb_state_unref(self->mKBState);
       self->mKBKeymap = xkb_keymap;
       self->mKBState = xkb_state;
}

void CIrrDeviceWayland::wl_keyboard_enter(void *data, wl_keyboard *wl_keyboard,
               uint32_t serial, struct wl_surface *surface,
               wl_array *keys)
{
	CIrrDeviceWayland *self = (CIrrDeviceWayland *)data;
	for (uint32_t *key = (uint32_t *)keys->data;
			(const char *) key < ((const char *) keys->data + keys->size); key++){
		wl_keyboard_key(data, wl_keyboard, serial, 0, *key, WL_KEYBOARD_KEY_STATE_PRESSED);
	}
}

void CIrrDeviceWayland::wl_keyboard_key(void *data, wl_keyboard *wl_keyboard,
               uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
		CIrrDeviceWayland *self = (CIrrDeviceWayland *)data;
		SEvent irrevent;

		char buf[128];
		uint32_t keycode = key + 8;
		SKeyMap mp;

		mp.XKBKey = xkb_state_key_get_one_sym(
						self->mKBState, keycode);
		xkb_keysym_get_name(mp.XKBKey, buf, sizeof(buf));

		// Handle CTRL & SHIFT specifically
		switch (mp.XKBKey) {
			case XKB_KEY_Shift_L:
			case XKB_KEY_Shift_R:
				self->mShiftPressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
				return;
			case XKB_KEY_Control_L:
			case XKB_KEY_Control_R:
				self->mCTRLPressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
				return;
			default: break;
		}

		self->mKeyPressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);

		const char *action =
				state == WL_KEYBOARD_KEY_STATE_PRESSED ? "press" : "release";
		fprintf(stderr, "key %s: sym: %-12s (%d), ", action, buf, mp.XKBKey);
		xkb_state_key_get_utf8(self->mKBState, keycode, buf, sizeof(buf));
		fprintf(stderr, "utf8: '%s'\n", buf);

		irrevent.EventType = irr::EET_KEY_INPUT_EVENT;
		irrevent.KeyInput.PressedDown = (state == WL_KEYBOARD_KEY_STATE_PRESSED);

		irrevent.KeyInput.Char = ((wchar_t*)(buf))[0];
		irrevent.KeyInput.Control = self->mCTRLPressed;
		irrevent.KeyInput.Shift = self->mShiftPressed;

		const s32 idx = self->KeyMap.binary_search(mp);
		if (idx != -1)
		{
			irrevent.KeyInput.Key = (EKEY_CODE)self->KeyMap[idx].Win32Key;
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
				os::Printer::log("Could not find EKEY_CODE, using orig. XKB keycode instead", core::stringc(mp.XKBKey).c_str(), ELL_INFORMATION);
			}
			else
			{
				os::Printer::log("EKEY_CODE is 0, using orig. XKB keycode instead", core::stringc(mp.XKBKey).c_str(), ELL_INFORMATION);
			}
			// Any value is better than none, that allows at least using the keys.
			// Worst case is that some keys will be identical, still better than _all_
			// unknown keys being identical.
			irrevent.KeyInput.Key = (EKEY_CODE)mp.XKBKey;
		}

		self->postEventFromUser(irrevent);

		// register key press time & event
		self->mKeyPressLastTime = os::Timer::getTime();
		self->mLastKeyPressEvent.key = key;
		self->mLastKeyPressEvent.serial = serial;
}

void CIrrDeviceWayland::wl_keyboard_leave(void *data, wl_keyboard *wl_keyboard,
               uint32_t serial, wl_surface *surface)
{
       fprintf(stderr, "keyboard leave\n");
}

void CIrrDeviceWayland::wl_keyboard_modifiers(void *data, wl_keyboard *wl_keyboard,
               uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
               uint32_t group)
{
       CIrrDeviceWayland *self = (CIrrDeviceWayland *)data;
       xkb_state_update_mask(self->mKBState,
               mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

void CIrrDeviceWayland::wl_keyboard_repeat_info(void *data, wl_keyboard *wl_keyboard,
               int32_t rate, int32_t delay)
{
       CIrrDeviceWayland *self = (CIrrDeviceWayland *)data;
	   fprintf(stderr, "repeat!\n");
	   // TODO ?
}

const struct wl_keyboard_listener CIrrDeviceWayland::waylandKeyboardListener = {
    .keymap = CIrrDeviceWayland::wl_keyboard_keymap,
    .enter = CIrrDeviceWayland::wl_keyboard_enter,
    .leave = CIrrDeviceWayland::wl_keyboard_leave,
    .key = CIrrDeviceWayland::wl_keyboard_key,
    .modifiers = CIrrDeviceWayland::wl_keyboard_modifiers,
    .repeat_info = CIrrDeviceWayland::wl_keyboard_repeat_info,
};

void CIrrDeviceWayland::xdg_surface_configure(void *data,
        xdg_surface *xdg_surface, uint32_t serial)
{
    CIrrDeviceWayland *self = (CIrrDeviceWayland *)data;
    xdg_surface_ack_configure(xdg_surface, serial);

    wl_surface_commit(self->mSurface);
}

const struct xdg_surface_listener CIrrDeviceWayland::xdg_surface_listener = {
    .configure = xdg_surface_configure,
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
		wl_seat_destroy(mSeat);
		xdg_wm_base_destroy(mXDGWMBase);
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
	else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
	{
		self->mXDGWMBase = (xdg_wm_base *) wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
	}
	else if (strcmp(interface, wl_seat_interface.name) == 0)
	{
		self->mSeat = (wl_seat *)wl_registry_bind(registry, name,
			&wl_seat_interface, std::min(version, (uint32_t)2));

		 wl_seat_add_listener(self->mSeat, &CIrrDeviceWayland::waylandSeatListener, self);
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

	mKBContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
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

	mSurface = surface;

	xdg_surface *xdg_surface = xdg_wm_base_get_xdg_surface(mXDGWMBase, mSurface);
	xdg_surface_add_listener(xdg_surface, &CIrrDeviceWayland::xdg_surface_listener, this);

	xdg_toplevel *xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
	xdg_toplevel_set_title(xdg_toplevel, "Irrlicht window");

	wl_surface_commit(mSurface);

	mRegion = wl_compositor_create_region(mCompositor);

	wl_region_add(mRegion, 0, 0, CreationParams.WindowSize.Width, CreationParams.WindowSize.Height);
	wl_surface_set_opaque_region(mSurface, mRegion);

	mEGLWindow =
		wl_egl_window_create(mSurface, CreationParams.WindowSize.Width, CreationParams.WindowSize.Height);

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
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
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

   if (!eglBindAPI(EGL_OPENGL_API)) {
        os::Printer::log("Failed to init EGL: fail to bind openGL API\n", ELL_ERROR);
        return -1;
    }

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

	if ((CreationParams.DriverType != video::EDT_NULL) && mDisplay)
	{
		wl_display_dispatch_pending(mDisplay);
	}

	// Handle key repeat feature
	if (mKeyPressed && mKeyPressLastTime + IRR_KEY_PRESS_INTERVAL <= os::Timer::getTime())
	{
			CIrrDeviceWayland::wl_keyboard_key(this, mSeatKeyboard, mLastKeyPressEvent.serial,
				os::Timer::getRealTime(), mLastKeyPressEvent.key, WL_KEYBOARD_KEY_STATE_PRESSED);
	}

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
	KeyMap.reallocate(84);
	KeyMap.push_back(SKeyMap(XKB_KEY_BackSpace, KEY_BACK));
	KeyMap.push_back(SKeyMap(XKB_KEY_Tab, KEY_TAB));
	KeyMap.push_back(SKeyMap(XKB_KEY_ISO_Left_Tab, KEY_TAB));
	KeyMap.push_back(SKeyMap(XKB_KEY_Linefeed, 0)); // ???
	KeyMap.push_back(SKeyMap(XKB_KEY_Clear, KEY_CLEAR));
	KeyMap.push_back(SKeyMap(XKB_KEY_Return, KEY_RETURN));
	KeyMap.push_back(SKeyMap(XKB_KEY_Pause, KEY_PAUSE));
	KeyMap.push_back(SKeyMap(XKB_KEY_Scroll_Lock, KEY_SCROLL));
	KeyMap.push_back(SKeyMap(XKB_KEY_Sys_Req, 0)); // ???
	KeyMap.push_back(SKeyMap(XKB_KEY_Escape, KEY_ESCAPE));
	KeyMap.push_back(SKeyMap(XKB_KEY_Insert, KEY_INSERT));
	KeyMap.push_back(SKeyMap(XKB_KEY_Delete, KEY_DELETE));
	KeyMap.push_back(SKeyMap(XKB_KEY_Home, KEY_HOME));
	KeyMap.push_back(SKeyMap(XKB_KEY_Left, KEY_LEFT));
	KeyMap.push_back(SKeyMap(XKB_KEY_Up, KEY_UP));
	KeyMap.push_back(SKeyMap(XKB_KEY_Right, KEY_RIGHT));
	KeyMap.push_back(SKeyMap(XKB_KEY_Down, KEY_DOWN));
	KeyMap.push_back(SKeyMap(XKB_KEY_Prior, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(XKB_KEY_Page_Up, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(XKB_KEY_Next, KEY_NEXT));
	KeyMap.push_back(SKeyMap(XKB_KEY_Page_Down, KEY_NEXT));
	KeyMap.push_back(SKeyMap(XKB_KEY_End, KEY_END));
	KeyMap.push_back(SKeyMap(XKB_KEY_Begin, KEY_HOME));
	KeyMap.push_back(SKeyMap(XKB_KEY_Num_Lock, KEY_NUMLOCK));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Space, KEY_SPACE));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Tab, KEY_TAB));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Enter, KEY_RETURN));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_F1, KEY_F1));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_F2, KEY_F2));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_F3, KEY_F3));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_F4, KEY_F4));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Home, KEY_HOME));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Left, KEY_LEFT));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Up, KEY_UP));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Right, KEY_RIGHT));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Down, KEY_DOWN));
	KeyMap.push_back(SKeyMap(XKB_KEY_Print, KEY_PRINT));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Prior, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Page_Up, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Next, KEY_NEXT));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Page_Down, KEY_NEXT));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_End, KEY_END));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Begin, KEY_HOME));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Insert, KEY_INSERT));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Delete, KEY_DELETE));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Equal, 0)); // ???
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Multiply, KEY_MULTIPLY));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Add, KEY_ADD));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Separator, KEY_SEPARATOR));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Subtract, KEY_SUBTRACT));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Decimal, KEY_DECIMAL));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_Divide, KEY_DIVIDE));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_0, KEY_KEY_0));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_1, KEY_KEY_1));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_2, KEY_KEY_2));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_3, KEY_KEY_3));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_4, KEY_KEY_4));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_5, KEY_KEY_5));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_6, KEY_KEY_6));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_7, KEY_KEY_7));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_8, KEY_KEY_8));
	KeyMap.push_back(SKeyMap(XKB_KEY_KP_9, KEY_KEY_9));
	KeyMap.push_back(SKeyMap(XKB_KEY_F1, KEY_F1));
	KeyMap.push_back(SKeyMap(XKB_KEY_F2, KEY_F2));
	KeyMap.push_back(SKeyMap(XKB_KEY_F3, KEY_F3));
	KeyMap.push_back(SKeyMap(XKB_KEY_F4, KEY_F4));
	KeyMap.push_back(SKeyMap(XKB_KEY_F5, KEY_F5));
	KeyMap.push_back(SKeyMap(XKB_KEY_F6, KEY_F6));
	KeyMap.push_back(SKeyMap(XKB_KEY_F7, KEY_F7));
	KeyMap.push_back(SKeyMap(XKB_KEY_F8, KEY_F8));
	KeyMap.push_back(SKeyMap(XKB_KEY_F9, KEY_F9));
	KeyMap.push_back(SKeyMap(XKB_KEY_F10, KEY_F10));
	KeyMap.push_back(SKeyMap(XKB_KEY_F11, KEY_F11));
	KeyMap.push_back(SKeyMap(XKB_KEY_F12, KEY_F12));
	KeyMap.push_back(SKeyMap(XKB_KEY_Shift_L, KEY_LSHIFT));
	KeyMap.push_back(SKeyMap(XKB_KEY_Shift_R, KEY_RSHIFT));
	KeyMap.push_back(SKeyMap(XKB_KEY_Control_L, KEY_LCONTROL));
	KeyMap.push_back(SKeyMap(XKB_KEY_Control_R, KEY_RCONTROL));
	KeyMap.push_back(SKeyMap(XKB_KEY_Caps_Lock, KEY_CAPITAL));
	KeyMap.push_back(SKeyMap(XKB_KEY_Shift_Lock, KEY_CAPITAL));
	KeyMap.push_back(SKeyMap(XKB_KEY_Meta_L, KEY_LWIN));
	KeyMap.push_back(SKeyMap(XKB_KEY_Meta_R, KEY_RWIN));
	KeyMap.push_back(SKeyMap(XKB_KEY_Alt_L, KEY_LMENU));
	KeyMap.push_back(SKeyMap(XKB_KEY_Alt_R, KEY_RMENU));
	KeyMap.push_back(SKeyMap(XKB_KEY_ISO_Level3_Shift, KEY_RMENU));
	KeyMap.push_back(SKeyMap(XKB_KEY_Menu, KEY_MENU));
	KeyMap.push_back(SKeyMap(XKB_KEY_space, KEY_SPACE));
	KeyMap.push_back(SKeyMap(XKB_KEY_exclam, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_quotedbl, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_section, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_numbersign, KEY_OEM_2));
	KeyMap.push_back(SKeyMap(XKB_KEY_dollar, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_percent, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_ampersand, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_apostrophe, KEY_OEM_7));
	KeyMap.push_back(SKeyMap(XKB_KEY_parenleft, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_parenright, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_asterisk, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_plus, KEY_PLUS)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_comma, KEY_COMMA)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_minus, KEY_MINUS)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_period, KEY_PERIOD)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_slash, KEY_OEM_2)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_0, KEY_KEY_0));
	KeyMap.push_back(SKeyMap(XKB_KEY_1, KEY_KEY_1));
	KeyMap.push_back(SKeyMap(XKB_KEY_2, KEY_KEY_2));
	KeyMap.push_back(SKeyMap(XKB_KEY_3, KEY_KEY_3));
	KeyMap.push_back(SKeyMap(XKB_KEY_4, KEY_KEY_4));
	KeyMap.push_back(SKeyMap(XKB_KEY_5, KEY_KEY_5));
	KeyMap.push_back(SKeyMap(XKB_KEY_6, KEY_KEY_6));
	KeyMap.push_back(SKeyMap(XKB_KEY_7, KEY_KEY_7));
	KeyMap.push_back(SKeyMap(XKB_KEY_8, KEY_KEY_8));
	KeyMap.push_back(SKeyMap(XKB_KEY_9, KEY_KEY_9));
	KeyMap.push_back(SKeyMap(XKB_KEY_colon, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_semicolon, KEY_OEM_1));
	KeyMap.push_back(SKeyMap(XKB_KEY_less, KEY_OEM_102));
	KeyMap.push_back(SKeyMap(XKB_KEY_equal, KEY_PLUS));
	KeyMap.push_back(SKeyMap(XKB_KEY_greater, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_question, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_at, KEY_KEY_2)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_mu, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_EuroSign, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_A, KEY_KEY_A));
	KeyMap.push_back(SKeyMap(XKB_KEY_B, KEY_KEY_B));
	KeyMap.push_back(SKeyMap(XKB_KEY_C, KEY_KEY_C));
	KeyMap.push_back(SKeyMap(XKB_KEY_D, KEY_KEY_D));
	KeyMap.push_back(SKeyMap(XKB_KEY_E, KEY_KEY_E));
	KeyMap.push_back(SKeyMap(XKB_KEY_F, KEY_KEY_F));
	KeyMap.push_back(SKeyMap(XKB_KEY_G, KEY_KEY_G));
	KeyMap.push_back(SKeyMap(XKB_KEY_H, KEY_KEY_H));
	KeyMap.push_back(SKeyMap(XKB_KEY_I, KEY_KEY_I));
	KeyMap.push_back(SKeyMap(XKB_KEY_J, KEY_KEY_J));
	KeyMap.push_back(SKeyMap(XKB_KEY_K, KEY_KEY_K));
	KeyMap.push_back(SKeyMap(XKB_KEY_L, KEY_KEY_L));
	KeyMap.push_back(SKeyMap(XKB_KEY_M, KEY_KEY_M));
	KeyMap.push_back(SKeyMap(XKB_KEY_N, KEY_KEY_N));
	KeyMap.push_back(SKeyMap(XKB_KEY_O, KEY_KEY_O));
	KeyMap.push_back(SKeyMap(XKB_KEY_P, KEY_KEY_P));
	KeyMap.push_back(SKeyMap(XKB_KEY_Q, KEY_KEY_Q));
	KeyMap.push_back(SKeyMap(XKB_KEY_R, KEY_KEY_R));
	KeyMap.push_back(SKeyMap(XKB_KEY_S, KEY_KEY_S));
	KeyMap.push_back(SKeyMap(XKB_KEY_T, KEY_KEY_T));
	KeyMap.push_back(SKeyMap(XKB_KEY_U, KEY_KEY_U));
	KeyMap.push_back(SKeyMap(XKB_KEY_V, KEY_KEY_V));
	KeyMap.push_back(SKeyMap(XKB_KEY_W, KEY_KEY_W));
	KeyMap.push_back(SKeyMap(XKB_KEY_X, KEY_KEY_X));
	KeyMap.push_back(SKeyMap(XKB_KEY_Y, KEY_KEY_Y));
	KeyMap.push_back(SKeyMap(XKB_KEY_Z, KEY_KEY_Z));
	KeyMap.push_back(SKeyMap(XKB_KEY_bracketleft, KEY_OEM_4));
	KeyMap.push_back(SKeyMap(XKB_KEY_backslash, KEY_OEM_5));
	KeyMap.push_back(SKeyMap(XKB_KEY_bracketright, KEY_OEM_6));
	KeyMap.push_back(SKeyMap(XKB_KEY_asciicircum, KEY_OEM_5));
	KeyMap.push_back(SKeyMap(XKB_KEY_degree, 0)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_underscore, KEY_MINUS)); //?
	KeyMap.push_back(SKeyMap(XKB_KEY_grave, KEY_OEM_3));
	KeyMap.push_back(SKeyMap(XKB_KEY_acute, KEY_OEM_6));
	KeyMap.push_back(SKeyMap(XKB_KEY_a, KEY_KEY_A));
	KeyMap.push_back(SKeyMap(XKB_KEY_b, KEY_KEY_B));
	KeyMap.push_back(SKeyMap(XKB_KEY_c, KEY_KEY_C));
	KeyMap.push_back(SKeyMap(XKB_KEY_d, KEY_KEY_D));
	KeyMap.push_back(SKeyMap(XKB_KEY_e, KEY_KEY_E));
	KeyMap.push_back(SKeyMap(XKB_KEY_f, KEY_KEY_F));
	KeyMap.push_back(SKeyMap(XKB_KEY_g, KEY_KEY_G));
	KeyMap.push_back(SKeyMap(XKB_KEY_h, KEY_KEY_H));
	KeyMap.push_back(SKeyMap(XKB_KEY_i, KEY_KEY_I));
	KeyMap.push_back(SKeyMap(XKB_KEY_j, KEY_KEY_J));
	KeyMap.push_back(SKeyMap(XKB_KEY_k, KEY_KEY_K));
	KeyMap.push_back(SKeyMap(XKB_KEY_l, KEY_KEY_L));
	KeyMap.push_back(SKeyMap(XKB_KEY_m, KEY_KEY_M));
	KeyMap.push_back(SKeyMap(XKB_KEY_n, KEY_KEY_N));
	KeyMap.push_back(SKeyMap(XKB_KEY_o, KEY_KEY_O));
	KeyMap.push_back(SKeyMap(XKB_KEY_p, KEY_KEY_P));
	KeyMap.push_back(SKeyMap(XKB_KEY_q, KEY_KEY_Q));
	KeyMap.push_back(SKeyMap(XKB_KEY_r, KEY_KEY_R));
	KeyMap.push_back(SKeyMap(XKB_KEY_s, KEY_KEY_S));
	KeyMap.push_back(SKeyMap(XKB_KEY_t, KEY_KEY_T));
	KeyMap.push_back(SKeyMap(XKB_KEY_u, KEY_KEY_U));
	KeyMap.push_back(SKeyMap(XKB_KEY_v, KEY_KEY_V));
	KeyMap.push_back(SKeyMap(XKB_KEY_w, KEY_KEY_W));
	KeyMap.push_back(SKeyMap(XKB_KEY_x, KEY_KEY_X));
	KeyMap.push_back(SKeyMap(XKB_KEY_y, KEY_KEY_Y));
	KeyMap.push_back(SKeyMap(XKB_KEY_z, KEY_KEY_Z));
	KeyMap.push_back(SKeyMap(XKB_KEY_ssharp, KEY_OEM_4));
	KeyMap.push_back(SKeyMap(XKB_KEY_adiaeresis, KEY_OEM_7));
	KeyMap.push_back(SKeyMap(XKB_KEY_odiaeresis, KEY_OEM_3));
	KeyMap.push_back(SKeyMap(XKB_KEY_udiaeresis, KEY_OEM_1));
	KeyMap.push_back(SKeyMap(XKB_KEY_Super_L, KEY_LWIN));
	KeyMap.push_back(SKeyMap(XKB_KEY_Super_R, KEY_RWIN));
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

