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

namespace irr
{

	class CIrrDeviceiOS : public CIrrDeviceStub
	{
	public:
		CIrrDeviceiOS(const SIrrlichtCreationParameters& params);
		virtual ~CIrrDeviceiOS();

		bool run() override;
		void yield() override;
		void sleep(u32 timeMs, bool pauseTimer) override;

		void setWindowCaption(const wchar_t* text) override;

		bool isWindowActive() const override;
		bool isWindowFocused() const override;
		bool isWindowMinimized() const override;

		void closeDevice() override;

		void setResizable(bool resize = false) override;

		void minimizeWindow() override;
		void maximizeWindow() override;
		void restoreWindow() override;

		core::position2di getWindowPosition() override;

        bool activateAccelerometer(float updateInterval = 0.016666f) override;
        bool deactivateAccelerometer() override;
        bool isAccelerometerActive() override;
        bool isAccelerometerAvailable() override;
        bool activateGyroscope(float updateInterval = 0.016666f) override;
        bool deactivateGyroscope() override;
        bool isGyroscopeActive() override;
        bool isGyroscopeAvailable() override;
        bool activateDeviceMotion(float updateInterval = 0.016666f) override;
        bool deactivateDeviceMotion() override;
        bool isDeviceMotionActive() override;
        bool isDeviceMotionAvailable() override;

        E_DEVICE_TYPE getType() const override;

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
