// Copyright (C) 2002-2008 Nikolaus Gebhardt
// Copyright (C) 2008 Redshift Software, Inc.
// Copyright (C) 2012-2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_IOS_H_INCLUDED__
#define __C_IRR_DEVICE_IOS_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_IOS_DEVICE_

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"

namespace irr
{

	class CIrrDeviceiOS : public CIrrDeviceStub, public video::IImagePresenter
	{
	public:
		CIrrDeviceiOS(const SIrrlichtCreationParameters& params);
		virtual ~CIrrDeviceiOS();

		virtual bool run() IRR_OVERRIDE;
		virtual void yield() IRR_OVERRIDE;
		virtual void sleep(u32 timeMs, bool pauseTimer) IRR_OVERRIDE;

		virtual void setWindowCaption(const wchar_t* text) IRR_OVERRIDE;

		virtual bool isWindowActive() const IRR_OVERRIDE;
		virtual bool isWindowFocused() const IRR_OVERRIDE;
		virtual bool isWindowMinimized() const IRR_OVERRIDE;

		virtual bool present(video::IImage* surface, void * windowId = 0, core::rect<s32>* src = 0) IRR_OVERRIDE;

		virtual void closeDevice() IRR_OVERRIDE;

		virtual void setResizable(bool resize = false) IRR_OVERRIDE;

		virtual void minimizeWindow() IRR_OVERRIDE;
		virtual void maximizeWindow() IRR_OVERRIDE;
		virtual void restoreWindow() IRR_OVERRIDE;

		virtual core::position2di getWindowPosition() IRR_OVERRIDE;

        virtual bool activateAccelerometer(float updateInterval = 0.016666f) IRR_OVERRIDE;
        virtual bool deactivateAccelerometer() IRR_OVERRIDE;
        virtual bool isAccelerometerActive() IRR_OVERRIDE;
        virtual bool isAccelerometerAvailable() IRR_OVERRIDE;
        virtual bool activateGyroscope(float updateInterval = 0.016666f) IRR_OVERRIDE;
        virtual bool deactivateGyroscope() IRR_OVERRIDE;
        virtual bool isGyroscopeActive() IRR_OVERRIDE;
        virtual bool isGyroscopeAvailable() IRR_OVERRIDE;
        virtual bool activateDeviceMotion(float updateInterval = 0.016666f) IRR_OVERRIDE;
        virtual bool deactivateDeviceMotion() IRR_OVERRIDE;
        virtual bool isDeviceMotionActive() IRR_OVERRIDE;
        virtual bool isDeviceMotionAvailable() IRR_OVERRIDE;

        virtual E_DEVICE_TYPE getType() const IRR_OVERRIDE;

	private:
        void createWindow();
        void createViewAndDriver();

        void* DataStorage;

		bool Close;
	};

}

#ifdef _IRR_COMPILE_WITH_IOS_BUILTIN_MAIN_
extern void irrlicht_main();
#endif

#endif
#endif
