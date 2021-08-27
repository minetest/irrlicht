// Copyright (C) 2002-2007 Nikolaus Gebhardt
// Copyright (C) 2007-2012 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_IRR_DEVICE_FB_H_INCLUDED
#define IRR_C_IRR_DEVICE_FB_H_INCLUDED

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_FB_DEVICE_

#include "CIrrDeviceStub.h"
#include "SIrrCreationParameters.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"
#include "ICursorControl.h"

#define KeySym s32
#include <linux/fb.h>
#include <linux/kd.h>

namespace irr
{
	class CIrrDeviceFB : public CIrrDeviceStub, public video::IImagePresenter
	{
	public:

		//! constructor
		CIrrDeviceFB(const SIrrlichtCreationParameters& params);

		//! destructor
		virtual ~CIrrDeviceFB();

		//! runs the device. Returns false if device wants to be deleted
		virtual bool run() IRR_OVERRIDE;

		//! Cause the device to temporarily pause execution and let other processes to run
		// This should bring down processor usage without major performance loss for Irrlicht
		virtual void yield() IRR_OVERRIDE;

		//! Pause execution and let other processes to run for a specified amount of time.
		virtual void sleep(u32 timeMs, bool pauseTimer) IRR_OVERRIDE;

		//! sets the caption of the window
		virtual void setWindowCaption(const wchar_t* text) IRR_OVERRIDE;

		//! returns if window is active. if not, nothing need to be drawn
		virtual bool isWindowActive() const IRR_OVERRIDE;

		//! returns if window has focus
		virtual bool isWindowFocused() const IRR_OVERRIDE;

		//! returns if window is minimized
		virtual bool isWindowMinimized() const IRR_OVERRIDE;

		//! Minimizes window
		virtual void minimizeWindow() IRR_OVERRIDE;

		//! Maximizes window
		virtual void maximizeWindow() IRR_OVERRIDE;

		//! Restores original window size
		virtual void restoreWindow() IRR_OVERRIDE;

		//! returns current window position (not supported for this device)
		virtual core::position2di getWindowPosition() IRR_OVERRIDE
		{
			return core::position2di(-1, -1);
		}

		//! presents a surface in the client area
		virtual bool present(video::IImage* surface, void* windowId = 0, core::rect<s32>* src=0 ) IRR_OVERRIDE;

		//! notifies the device that it should close itself
		virtual void closeDevice() IRR_OVERRIDE;

		//! Sets if the window should be resizeable in windowed mode.
		virtual void setResizable(bool resize=false) IRR_OVERRIDE;

		//! Returns the type of this device
		virtual E_DEVICE_TYPE getType() const IRR_OVERRIDE;

	private:

		//! create the driver
		void createDriver();

		bool createWindow(const core::dimension2d<u32>& windowSize, u32 bits);

		//! Implementation of the cursor control
		class CCursorControl : public gui::ICursorControl
		{
		public:

			CCursorControl(CIrrDeviceFB* dev, bool null)
				: Device(dev), IsVisible(true), Null(null)
			{
				Device->grab();
			}

			~CCursorControl()
			{
				Device->drop();
			}

			//! Changes the visible state of the mouse cursor.
			virtual void setVisible(bool visible) IRR_OVERRIDE
			{
				IsVisible = visible;
			}

			//! Returns if the cursor is currently visible.
			virtual bool isVisible() const IRR_OVERRIDE
			{
				return IsVisible;
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<f32> &pos) IRR_OVERRIDE
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(f32 x, f32 y) IRR_OVERRIDE
			{
				setPosition((s32)(x*Device->CreationParams.WindowSize.Width), (s32)(y*Device->CreationParams.WindowSize.Height));
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<s32> &pos) IRR_OVERRIDE
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(s32 x, s32 y) IRR_OVERRIDE
			{
			}

			//! Returns the current position of the mouse cursor.
			virtual const core::position2d<s32>& getPosition(bool updateCursor) IRR_OVERRIDE
			{
				if ( updateCursor )
					updateCursorPos();
				return CursorPos;
			}

			//! Returns the current position of the mouse cursor.
			virtual core::position2d<f32> getRelativePosition(bool updateCursor) IRR_OVERRIDE
			{
				if ( updateCursor)
					updateCursorPos();
				return core::position2d<f32>(CursorPos.X / (f32)Device->CreationParams.WindowSize.Width,
					CursorPos.Y / (f32)Device->CreationParams.WindowSize.Height);
			}

			virtual void setReferenceRect(core::rect<s32>* rect=0) IRR_OVERRIDE
			{
			}

		private:

			void updateCursorPos()
			{
			}

			core::position2d<s32> CursorPos;
			CIrrDeviceFB* Device;
			bool IsVisible;
			bool Null;
		};

		friend class CCursorControl;

		int Framebuffer;
		int EventDevice;
		int KeyboardDevice;
		struct fb_fix_screeninfo fbfixscreeninfo;
		struct fb_var_screeninfo fbscreeninfo;
		struct fb_var_screeninfo oldscreeninfo;
		long KeyboardMode;
		u8* SoftwareImage;

		u32 Pitch;
		video::ECOLOR_FORMAT FBColorFormat;
		bool Close;

		struct SKeyMap
		{
			SKeyMap() {}
			SKeyMap(s32 x11, s32 win32)
				: X11Key(x11), Win32Key(win32)
			{
			}

			KeySym X11Key;
			s32 Win32Key;

			bool operator<(const SKeyMap& o) const
			{
				return X11Key<o.X11Key;
			}
		};

		core::array<SKeyMap> KeyMap;
	};


} // end namespace irr

#endif // _IRR_USE_FB_DEVICE_
#endif // IRR_C_IRR_DEVICE_FB_H_INCLUDED
