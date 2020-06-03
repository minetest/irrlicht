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
#include "CIrrDeviceLinux.h"

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

		static const wl_registry_listener waylandRegistryListener;
		static const wl_pointer_listener waylandPointerListener;

		wl_display *mDisplay = nullptr;
		wl_compositor *mCompositor = nullptr;
		wl_seat *mSeat = nullptr;
		wl_shell *mShell = nullptr;
		wl_shm *mShm = nullptr;
		wl_pointer *mSeatPointer = nullptr;
		wl_region *mRegion = nullptr;
		wl_egl_window *mEGLWindow = nullptr;
		EGLDisplay mEGLDisplay = nullptr;
		EGLContext mContext = nullptr;
		EGLSurface mEGLSurface = nullptr;
	};


} // end namespace irr

#endif // _IRR_COMPILE_WITH_WAYLAND_DEVICE_
#endif // __C_IRR_DEVICE_WAYLAND_H_INCLUDED__

