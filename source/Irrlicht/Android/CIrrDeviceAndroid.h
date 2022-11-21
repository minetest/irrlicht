// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_ANDROID_H_INCLUDED__
#define __C_IRR_DEVICE_ANDROID_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_ANDROID_DEVICE_

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"
#include "ICursorControl.h"

#include <android/sensor.h>
#include <android_native_app_glue.h>

namespace irr
{
	class CIrrDeviceAndroid : public CIrrDeviceStub, video::IImagePresenter
	{
	public:
		CIrrDeviceAndroid(const SIrrlichtCreationParameters& param);

		virtual ~CIrrDeviceAndroid();

		virtual bool run();

		virtual void yield();

		virtual void sleep(u32 timeMs, bool pauseTimer = false);

		virtual void setWindowCaption(const wchar_t* text);

		virtual bool present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip);

		virtual bool isWindowActive() const;

		virtual bool isWindowFocused() const;

		virtual bool isWindowMinimized() const;

		virtual void closeDevice();

		virtual void setResizable(bool resize = false);

		virtual void minimizeWindow();

		virtual void maximizeWindow();

		virtual void restoreWindow();

		virtual core::position2di getWindowPosition();

		virtual E_DEVICE_TYPE getType() const;

		virtual bool activateAccelerometer(float updateInterval);

		virtual bool deactivateAccelerometer();

		virtual bool isAccelerometerActive();

		virtual bool isAccelerometerAvailable();

		virtual bool activateGyroscope(float updateInterval);

		virtual bool deactivateGyroscope();

		virtual bool isGyroscopeActive();

		virtual bool isGyroscopeAvailable();

	private:

		static void handleAndroidCommand(android_app* app, int32_t cmd);

		static s32 handleInput(android_app* app, AInputEvent* event);

		void createDriver();

		void createKeyMap();

		video::SExposedVideoData& getExposedVideoData();

		android_app* Android;
		ASensorManager* SensorManager;
		ASensorEventQueue* SensorEventQueue;
		const ASensor* Accelerometer;
		const ASensor* Gyroscope;

		bool Focused;
		bool Initialized;
		bool Paused;

		JNIEnv* JNIEnvAttachedToVM;

		video::SExposedVideoData ExposedVideoData;

		core::array<EKEY_CODE> KeyMap;
	};

} // end namespace irr

#endif // _IRR_COMPILE_WITH_ANDROID_DEVICE_
#endif // __C_IRR_DEVICE_ANDROID_H_INCLUDED__
