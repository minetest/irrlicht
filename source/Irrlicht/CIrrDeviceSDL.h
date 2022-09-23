// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// This device code is based on the original SDL device implementation
// contributed by Shane Parker (sirshane).

#ifndef __C_IRR_DEVICE_SDL_H_INCLUDED__
#define __C_IRR_DEVICE_SDL_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_

#include "IrrlichtDevice.h"
#include "CIrrDeviceStub.h"
#include "IImagePresenter.h"
#include "ICursorControl.h"

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
#include <emscripten/html5.h>
#endif

#include <SDL.h>
#include <SDL_syswm.h>

#include <memory>

namespace irr
{

	class CIrrDeviceSDL : public CIrrDeviceStub, video::IImagePresenter
	{
	public:

		//! constructor
		CIrrDeviceSDL(const SIrrlichtCreationParameters& param);

		//! destructor
		virtual ~CIrrDeviceSDL();

		//! runs the device. Returns false if device wants to be deleted
		bool run() override;

		//! pause execution temporarily
		void yield() override;

		//! pause execution for a specified time
		void sleep(u32 timeMs, bool pauseTimer) override;

		//! sets the caption of the window
		void setWindowCaption(const wchar_t* text) override;

		//! returns if window is active. if not, nothing need to be drawn
		bool isWindowActive() const override;

		//! returns if window has focus.
		bool isWindowFocused() const override;

		//! returns if window is minimized.
		bool isWindowMinimized() const override;

		//! returns color format of the window.
		video::ECOLOR_FORMAT getColorFormat() const override;

		//! presents a surface in the client area
		bool present(video::IImage* surface, void* windowId=0, core::rect<s32>* src=0) override;

		//! notifies the device that it should close itself
		void closeDevice() override;

		//! Sets if the window should be resizable in windowed mode.
		void setResizable(bool resize=false) override;

		//! Minimizes the window.
		void minimizeWindow() override;

		//! Maximizes the window.
		void maximizeWindow() override;

		//! Restores the window size.
		void restoreWindow() override;

		//! Checks if the Irrlicht window is running in fullscreen mode
		/** \return True if window is fullscreen. */
		bool isFullscreen() const override;

		//! Get the position of this window on screen
		core::position2di getWindowPosition() override;

		//! Activate any joysticks, and generate events for them.
		bool activateJoysticks(core::array<SJoystickInfo> & joystickInfo) override;

		//! Get the device type
		E_DEVICE_TYPE getType() const override
		{
			return EIDT_SDL;
		}

		void SwapWindow();

		//! Implementation of the linux cursor control
		class CCursorControl : public gui::ICursorControl
		{
		public:

			CCursorControl(CIrrDeviceSDL* dev)
				: Device(dev), IsVisible(true)
			{
				initCursors();
			}

			//! Changes the visible state of the mouse cursor.
			void setVisible(bool visible) override
			{
				IsVisible = visible;
				if ( visible )
					SDL_ShowCursor( SDL_ENABLE );
				else
				{
					SDL_ShowCursor( SDL_DISABLE );
				}
			}

			//! Returns if the cursor is currently visible.
			bool isVisible() const override
			{
				return IsVisible;
			}

			//! Sets the new position of the cursor.
			void setPosition(const core::position2d<f32> &pos) override
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			void setPosition(f32 x, f32 y) override
			{
				setPosition((s32)(x*Device->Width), (s32)(y*Device->Height));
			}

			//! Sets the new position of the cursor.
			void setPosition(const core::position2d<s32> &pos) override
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			void setPosition(s32 x, s32 y) override
			{
				SDL_WarpMouseInWindow(Device->Window, x, y);

				if (SDL_GetRelativeMouseMode()) {
					// There won't be an event for this warp (details on libsdl-org/SDL/issues/6034)
					Device->MouseX = x;
					Device->MouseY = y;
				}
			}

			//! Returns the current position of the mouse cursor.
			const core::position2d<s32>& getPosition(bool updateCursor) override
			{
				if ( updateCursor )
					updateCursorPos();
				return CursorPos;
			}

			//! Returns the current position of the mouse cursor.
			core::position2d<f32> getRelativePosition(bool updateCursor) override
			{
				if ( updateCursor )
					updateCursorPos();
				return core::position2d<f32>(CursorPos.X / (f32)Device->Width,
					CursorPos.Y / (f32)Device->Height);
			}

			void setReferenceRect(core::rect<s32>* rect=0) override
			{
			}

			virtual void setRelativeMode(bool relative) _IRR_OVERRIDE_
			{
				// Only change it when necessary, as it flushes mouse motion when enabled
				if ( relative != SDL_GetRelativeMouseMode()) {
					if ( relative )
						SDL_SetRelativeMouseMode( SDL_TRUE );
					else
						SDL_SetRelativeMouseMode( SDL_FALSE );
				}
			}

			void setActiveIcon(gui::ECURSOR_ICON iconId) override
			{
				ActiveIcon = iconId;
				if (iconId > Cursors.size() || !Cursors[iconId]) {
					iconId = gui::ECI_NORMAL;
					if (iconId > Cursors.size() || !Cursors[iconId])
						return;
				}
				SDL_SetCursor(Cursors[iconId].get());
			}

			gui::ECURSOR_ICON getActiveIcon() const override
			{
				return ActiveIcon;
			}

		private:

			void updateCursorPos()
			{
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
				EmscriptenPointerlockChangeEvent pointerlockStatus; // let's hope that test is not expensive ...
				if ( emscripten_get_pointerlock_status(&pointerlockStatus) == EMSCRIPTEN_RESULT_SUCCESS )
				{
					if ( pointerlockStatus.isActive )
					{
						CursorPos.X += Device->MouseXRel;
						CursorPos.Y += Device->MouseYRel;
						Device->MouseXRel = 0;
						Device->MouseYRel = 0;
					}
					else
					{
						CursorPos.X = Device->MouseX;
						CursorPos.Y = Device->MouseY;
					}
				}
#else
				CursorPos.X = Device->MouseX;
				CursorPos.Y = Device->MouseY;

				if (CursorPos.X < 0)
					CursorPos.X = 0;
				if (CursorPos.X > (s32)Device->Width)
					CursorPos.X = Device->Width;
				if (CursorPos.Y < 0)
					CursorPos.Y = 0;
				if (CursorPos.Y > (s32)Device->Height)
					CursorPos.Y = Device->Height;
#endif
			}

			void initCursors();

			CIrrDeviceSDL* Device;
			core::position2d<s32> CursorPos;
			bool IsVisible;

			struct CursorDeleter {
				void operator()(SDL_Cursor *ptr) {
					if (ptr)
						SDL_FreeCursor(ptr);
				}
			};
			std::vector<std::unique_ptr<SDL_Cursor, CursorDeleter>> Cursors;
			gui::ECURSOR_ICON ActiveIcon;
		};

	private:

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	static EM_BOOL MouseUpDownCallback(int eventType, const EmscriptenMouseEvent * event, void* userData);
	static EM_BOOL MouseEnterCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
	static EM_BOOL MouseLeaveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);

#endif
		//! create the driver
		void createDriver();

		bool createWindow();

		void createKeyMap();

		void logAttributes();
		SDL_GLContext Context;
		SDL_Window *Window;
		int SDL_Flags;
#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
		core::array<SDL_Joystick*> Joysticks;
#endif

		s32 MouseX, MouseY;
		s32 MouseXRel, MouseYRel;
		u32 MouseButtonStates;

		u32 Width, Height;

		bool Resizable;
		bool WindowMinimized;

		struct SKeyMap
		{
			SKeyMap() {}
			SKeyMap(s32 x11, s32 win32)
				: SDLKey(x11), Win32Key(win32)
			{
			}

			s32 SDLKey;
			s32 Win32Key;

			bool operator<(const SKeyMap& o) const
			{
				return SDLKey<o.SDLKey;
			}
		};

		core::array<SKeyMap> KeyMap;
		SDL_SysWMinfo Info;
	};

} // end namespace irr

#endif // _IRR_COMPILE_WITH_SDL_DEVICE_
#endif // __C_IRR_DEVICE_SDL_H_INCLUDED__

