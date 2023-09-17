#include <irrlicht.h>
#include <iostream>

using namespace irr;

int main(int argc, char *argv[])
{
	if (argc < 2)
		return 1;

	SIrrlichtCreationParameters p;
	p.DriverType = video::EDT_NULL;
	p.WindowSize = core::dimension2du(640, 480);
	p.LoggingLevel = ELL_DEBUG;

	auto *device = createDeviceEx(p);
	if (!device)
		return 1;
	auto *driver = device->getVideoDriver();

	while (__AFL_LOOP(10000)) {
		auto *tex = driver->getTexture(argv[1]);
		if (tex)
			driver->removeTexture(tex);
	}

	device->drop();
	return 0;
}
