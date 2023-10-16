// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_IRR_C_TIMER_H_INCLUDED
#define IRR_C_IRR_C_TIMER_H_INCLUDED

#include "ITimer.h"
#include "os.h"

namespace irr
{
	//! Device independent implementation of the timer
	class CTimer : public ITimer
	{
	public:

		CTimer(bool usePerformanceTimer=true)
		{
			os::Timer::initTimer(usePerformanceTimer);
		}

		//! Returns current real time in milliseconds of the system.
		/** This value does not start with 0 when the application starts.
		For example in one implementation the value returned could be the
		amount of milliseconds which have elapsed since the system was started. */
		virtual u32 getRealTime() const IRR_OVERRIDE
		{
			return os::Timer::getRealTime();
		}

		//! Get current time and date in calendar form
		virtual RealTimeDate getRealTimeAndDate() const IRR_OVERRIDE
		{
			return os::Timer::getRealTimeAndDate();
		}

		//! Returns current virtual time in milliseconds.
		/** This value starts with 0 and can be manipulated using setTime(), stopTimer(),
		startTimer(), etc. This value depends on the set speed of the timer if the timer
		is stopped, etc. If you need the system time, use getRealTime() */
		virtual u32 getTime() const IRR_OVERRIDE
		{
			return os::Timer::getTime();
		}

		//! sets current virtual time
		virtual void setTime(u32 time) IRR_OVERRIDE
		{
			os::Timer::setTime(time);
		}

		//! Stops the game timer.
		/** The timer is reference counted, which means everything which calls
		stopTimer() will also have to call startTimer(), otherwise the timer may not start/stop
		correctly again. */
		virtual void stop() IRR_OVERRIDE
		{
			os::Timer::stopTimer();
		}

		//! Starts the game timer.
		/** The timer is reference counted, which means everything which calls
		stopTimer() will also have to call startTimer(), otherwise the timer may not start/stop
		correctly again. */
		virtual void start() IRR_OVERRIDE
		{
			os::Timer::startTimer();
		}

		//! Sets the speed of the timer
		/** The speed is the factor with which the time is running faster or slower then the
		real system time. */
		virtual void setSpeed(f32 speed = 1.0f) IRR_OVERRIDE
		{
			os::Timer::setSpeed(speed);
		}

		//! Returns current speed of the timer
		/** The speed is the factor with which the time is running faster or slower then the
		real system time. */
		virtual f32 getSpeed() const IRR_OVERRIDE
		{
			return os::Timer::getSpeed();
		}

		//! Returns if game timer is currently stopped
		virtual bool isStopped() const IRR_OVERRIDE
		{
			bool ret = os::Timer::isStopped();
			return ret;
		}

		//! Advances the virtual time
		/** Makes the virtual timer update the time value based on the real time. This is
		called automatically when calling IrrlichtDevice::run(), but you can call it manually
		if you don't use this method. */
		virtual void tick() IRR_OVERRIDE
		{
			os::Timer::tick();
		}
	};

} // end namespace

#endif
