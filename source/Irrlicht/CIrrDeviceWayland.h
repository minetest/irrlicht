// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_WAYLAND_H_INCLUDED__
#define __C_IRR_DEVICE_WAYLAND_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <xkbcommon/xkbcommon.h>
#include "CIrrDeviceLinux.h"
#include "xdg-shell-client-protocol.h"

namespace irr
{
	class CIrrDeviceWayland : public CIrrDeviceLinux
	{
	public:

		//! constructor
		CIrrDeviceWayland(const SIrrlichtCreationParameters& param);

		//! destructor
		virtual ~CIrrDeviceWayland();

		//! runs the device. Returns false if device wants to be deleted
		virtual bool run();

		//! sets the caption of the window
		virtual void setWindowCaption(const wchar_t* text);

		//! returns if window is active. if not, nothing need to be drawn
		virtual bool isWindowActive() const;

		//! returns if window has focus.
		virtual bool isWindowFocused() const;

		//! returns if window is minimized.
		virtual bool isWindowMinimized() const;

		//! returns color format of the window.
		virtual video::ECOLOR_FORMAT getColorFormat() const;

		//! presents a surface in the client area
		virtual bool present(video::IImage* surface, void* windowId=0, core::rect<s32>* src=0 );

		//! notifies the device that it should close itself
		virtual void closeDevice();

		//! \return Returns a pointer to a list with all video modes
		//! supported by the gfx adapter.
		video::IVideoModeList* getVideoModeList();

		//! Sets if the window should be resizable in windowed mode.
		virtual void setResizable(bool resize=false);

		//! Minimizes the window.
		virtual void minimizeWindow();

		//! Maximizes the window.
		virtual void maximizeWindow();

		//! Restores the window size.
		virtual void restoreWindow();

		//! Set the current Gamma Value for the Display
		virtual bool setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast );

		//! Get the current Gamma Value for the Display
		virtual bool getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast );

		//! gets text from the clipboard
		//! \return Returns 0 if no string is in there.
		virtual const c8* getTextFromClipboard() const;

		//! copies text to the clipboard
		//! This sets the clipboard selection and _not_ the primary selection which you have on X on the middle mouse button.
		virtual void copyToClipboard(const c8* text) const;

		//! Remove all messages pending in the system message loop
		virtual void clearSystemMessages();

		//! Get the device type
		virtual E_DEVICE_TYPE getType() const
		{
				return EIDT_X11;
		}

		EGLContext getCurrentContext() const
		{
			return mContext;
		}

		EGLDisplay getCurrentDisplay() const
		{
			return mEGLDisplay;
		}

		EGLSurface getCurrentSurface() const
		{
			return mEGLSurface;
		}

	private:

		//! create the driver
		virtual void createDriver();

		bool createWindow();
		bool initEGL();

		void createKeyMap();

		bool switchToFullscreen(bool reset=false);

		friend class CCursorControl;

		u32 Width, Height;
		bool WindowHasFocus;
		bool WindowMinimized;
		bool UseGLXWindow;
		bool ExternalWindow;
		int AutorepeatSupport;

		// Wayland stuff
		static void waylandRegistry(void *data, struct wl_registry *registry, uint32_t name,
			const char *interface, uint32_t version);
		static void waylandRegistryRemove(void *a, struct wl_registry *b, uint32_t c);
		static void waylandSeatCapabilities(void *data, struct wl_seat *wl_seat, uint32_t capabilities);

		static void wl_keyboard_keymap(void *data, wl_keyboard *wl_keyboard,
               uint32_t format, int32_t fd, uint32_t size);
		static void wl_keyboard_enter(void *data, struct wl_keyboard *wl_keyboard,
               uint32_t serial, struct wl_surface *surface,
               struct wl_array *keys);
		static void wl_keyboard_key(void *data, wl_keyboard *wl_keyboard,
               uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
		static void wl_keyboard_leave(void *data, wl_keyboard *wl_keyboard,
               uint32_t serial, wl_surface *surface);
		static void wl_keyboard_modifiers(void *data, wl_keyboard *wl_keyboard,
               uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
               uint32_t group);
		static void wl_keyboard_repeat_info(void *data, wl_keyboard *wl_keyboard,
               int32_t rate, int32_t delay);
		static void xdg_surface_configure(void *data, xdg_surface *xdg_surface, uint32_t serial);

		static const wl_registry_listener waylandRegistryListener;
		static const wl_seat_listener waylandSeatListener;
		static const wl_pointer_listener waylandPointerListener;
		static const wl_keyboard_listener waylandKeyboardListener;
		static const struct xdg_surface_listener xdg_surface_listener;

		xkb_context *mKBContext = nullptr;
		xkb_state *mKBState = nullptr;
		xkb_keymap *mKBKeymap = nullptr;
		bool mShiftPressed = false;
		bool mCTRLPressed = false;

		wl_display *mDisplay = nullptr;
		wl_compositor *mCompositor = nullptr;
		wl_surface *mSurface = nullptr;
		wl_seat *mSeat = nullptr;
		xdg_wm_base *mXDGWMBase = nullptr;
		wl_shm *mShm = nullptr;
		wl_pointer *mSeatPointer = nullptr;
		wl_keyboard *mSeatKeyboard = nullptr;
		wl_touch *mSeatTouch = nullptr;
		wl_region *mRegion = nullptr;
		wl_egl_window *mEGLWindow = nullptr;
		EGLDisplay mEGLDisplay = nullptr;
		EGLContext mContext = nullptr;
		EGLSurface mEGLSurface = nullptr;

		// Cursor control
		//! Implementation of the linux cursor control
		class CCursorControl : public gui::ICursorControl
		{
		public:

			CCursorControl(CIrrDeviceWayland* dev, bool null) : Device(dev) {}

			~CCursorControl() {}

			//! Changes the visible state of the mouse cursor.
			virtual void setVisible(bool visible)
			{
				if (visible==IsVisible)
					return;
				IsVisible = visible;
			}

			//! Returns if the cursor is currently visible.
			virtual bool isVisible() const
			{
				return IsVisible;
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<f32> &pos)
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(f32 x, f32 y)
			{
				setPosition((s32)(x*Device->Width), (s32)(y*Device->Height));
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<s32> &pos)
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(s32 x, s32 y)
			{
				CursorPos.X = x;
				CursorPos.Y = y;
			}

			//! Returns the current position of the mouse cursor.
			virtual const core::position2d<s32>& getPosition()
			{
				return CursorPos;
			}

			//! Returns the current position of the mouse cursor.
			virtual core::position2d<f32> getRelativePosition()
			{
				if (!UseReferenceRect)
				{
					return core::position2d<f32>(CursorPos.X / (f32)Device->Width,
						CursorPos.Y / (f32)Device->Height);
				}

				return core::position2d<f32>(CursorPos.X / (f32)ReferenceRect.getWidth(),
						CursorPos.Y / (f32)ReferenceRect.getHeight());
			}

			virtual void setReferenceRect(core::rect<s32>* rect=0)
			{
				if (rect)
				{
					ReferenceRect = *rect;
					UseReferenceRect = true;

					// prevent division through zero and uneven sizes

					if (!ReferenceRect.getHeight() || ReferenceRect.getHeight()%2)
						ReferenceRect.LowerRightCorner.Y += 1;

					if (!ReferenceRect.getWidth() || ReferenceRect.getWidth()%2)
						ReferenceRect.LowerRightCorner.X += 1;
				}
				else
					UseReferenceRect = false;
			}
		private:
			CIrrDeviceWayland* Device;
			core::position2d<s32> CursorPos;
			core::rect<s32> ReferenceRect;
			bool UseReferenceRect = false;
			bool IsVisible = true;
		};

		struct SKeyMap
		{
			SKeyMap() {}
			SKeyMap(s32 k, s32 win32)
				: XKBKey(k), Win32Key(win32)
			{
			}

			xkb_keysym_t XKBKey;
			s32 Win32Key;

			bool operator<(const SKeyMap& o) const
			{
				return XKBKey<o.XKBKey;
			}
		};

		core::array<SKeyMap> KeyMap;
	};


} // end namespace irr

#endif // _IRR_COMPILE_WITH_WAYLAND_DEVICE_
#endif // __C_IRR_DEVICE_WAYLAND_H_INCLUDED__

