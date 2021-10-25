#include "testUtils.h"
#include <iostream>

using namespace irr;
using namespace core;

// Test the functionality of the Irrlicht timer
bool testTimer()
{
	bool success = true;

	IrrlichtDevice* device = createDevice(video::EDT_NULL);
	if (!assertLog(device))
		return false;

	std::cerr << "Testing virtual timer.\n";
	ITimer *timer = device->getTimer();

	// must be running at start
	success &= assertLog(!timer->isStopped());

	// starting more often should not stop the timer
	timer->start();
	success &= assertLog(!timer->isStopped());

	// one stop should not stop the timer because it's started twice now
	timer->stop();
	success &= assertLog(!timer->isStopped());

	// another stop should really stop it
	timer->stop();
	success &= assertLog(timer->isStopped());

	// third stop - timer should still be stopped
	timer->stop();
	success &= assertLog(timer->isStopped());

	// should not start yet
	timer->start();
	success &= assertLog(timer->isStopped());

	// start again
	timer->start();
	success &= assertLog(!timer->isStopped());

	std::cerr << "Testing real timer.\n";
	const u32 startVirtual = timer->getTime();
	const u32 startReal = timer->getRealTime();
	device->sleep(2);
	if (startReal != timer->getRealTime())
		std::cerr << "Warning: Real timer did not progress. "\
		"Maybe the time slices are too coarse to see.\n";
	if (startVirtual != timer->getTime())
		std::cerr << "Warning: Virtual timer did not progress. "\
		"Maybe the time slices are too coarse to see.\n";

	irr::ITimer::RealTimeDate date = timer->getRealTimeAndDate();
	std::cerr << "Real time and date. "
		<< date.Day << ' ' << date.Month << ' ' << date.Year << ' '
		<< date.Hour << ' ' <<  date.Minute << ' ' <<  date.Second
		<< '\n'
		<< "This is day " << date.Yearday << ' '
		<< "of the year and weekday " << date.Weekday << ". "
		<< "The current time zone has daylight saving "
		<< (date.IsDST ? "enabled" : "disabled") << ".\n";

	device->closeDevice();
	device->run();
	device->drop();

	return success;
}

int main()
{
	return runTest(testTimer, "testTimer");
}
