
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_ 1

#include "irrlicht.h"
#include <assert.h>

#include <iostream>
#include <string>

// Small hack. Some newer X11 systems can't handle switching drivers too fast (causing BadWindow errors in X_ChangeWindowAttributes).
// Could be they don't like when Windows with different Visuals are created very quickly (it always happened after creating a new Window with different Visual to previous one).
// timeMs value set by try&error
#ifdef _IRR_POSIX_API_
	#include <time.h>
	#define SLOW_SWITCH \
	do { \
		struct timespec ts; \
		const int timeMs = 250; \
		ts.tv_sec = (time_t) (timeMs / 1000); \
		ts.tv_nsec = (long) (timeMs % 1000) * 1000000; \
		nanosleep(&ts, NULL);\
	} while (false)
#else
	#define SLOW_SWITCH
#endif

#define TestWithAllDrivers(X) \
	logTestString("Running test " #X "\n"); \
	for (u32 i=1; i<video::EDT_COUNT; ++i) \
		if (video::E_DRIVER_TYPE(i) != video::DEPRECATED_EDT_DIRECT3D8_NO_LONGER_EXISTS && irr::IrrlichtDevice::isDriverSupported((irr::video::E_DRIVER_TYPE)i)) \
		{\
			SLOW_SWITCH; \
			result &= X(video::E_DRIVER_TYPE(i));\
		}

#define TestWithAllHWDrivers(X) \
	SLOW_SWITCH; \
	logTestString("Running test " #X "\n"); \
	for (u32 i=video::EDT_DIRECT3D9; i<video::EDT_COUNT; ++i) \
		if (irr::IrrlichtDevice::isDriverSupported((irr::video::E_DRIVER_TYPE)i)) \
		{\
			SLOW_SWITCH; \
			result &= X(video::E_DRIVER_TYPE(i));\
		}

//! Replacement for assert which logs the line number.
/** \param cmp Boolean value to check for truthiness.
	\return Same as value of cmp. */
inline bool assertLog(bool cmp)
{
	if (!cmp)
		std::cerr << "ASSERT FAILURE: " << __FILE__
		<< " line " << __LINE__ <<'\n';
	return cmp;
}

//! Compare two files
/** \param fileName1 The first file for comparison.
	\param fileName2 The second file for comparison.
	\return true if binaries match, false on any error or difference. */
bool binaryCompareFiles(const char * fileName1, const char * fileName2);

//! Compare two images, returning the degree to which they match.
/** \param driver The Irrlicht video driver.
	\param fileName1 The first image to compare.
	\param fileName2 The second image to compare.
	\return The match, from 0.f to 100.f */
float fuzzyCompareImages(irr::video::IVideoDriver * driver,
		const char * fileName1, const char * fileName2);

//! Stabilize the screen background eg. eliminate problems like an aero transparency effects etc.
/** \param driver The Irrlicht video driver.
	\return true if required color is the same as a window background color. */
void stabilizeScreenBackground(irr::video::IVideoDriver * driver,
													irr::video::SColor color = irr::video::SColor(255, 255, 255, 255));

//! Return a drivername for the driver which is useable in filenames
irr::core::stringc shortDriverName(irr::video::IVideoDriver * driver);


//! Run one unit test.
/** \param testFunction The test function to run.
	\param testName Name of the test as logged to terminal.
	\return 0 for success, 1 for failure */
int runTest(bool (*testFunction)(), const std::string &testName);
#endif // _TEST_UTILS_H_
