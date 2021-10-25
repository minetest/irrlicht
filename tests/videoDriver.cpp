// Copyright (C) 2008-2012 Colin MacDonald, Christian Stehno
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include <iostream>

using namespace irr;
using namespace core;

/** Test various things in video drivers. */
bool testVideoDriver(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device =
		createDevice(driverType, dimension2d<u32>(160, 120));

	if (!device)
		return true;

	video::IVideoDriver* driver = device->getVideoDriver();
	std::cerr << "Testing driver " << driver->getName() << '\n'
		<< "MaxTextures:"
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxTextures") << '\n'
		<< "MaxSupportedTextures: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxSupportedTextures") << '\n'
		<< "MaxLights: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxLights") << '\n'
		<< "MaxAnisotropy: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxAnisotropy") << '\n'
		<< "MaxUserClipPlanes: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxUserClipPlanes") << '\n'
		<< "MaxAuxBuffers: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxAuxBuffers") << '\n'
		<< "MaxMultipleRenderTargets: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxMultipleRenderTargets") << '\n'
		<< "MaxIndices: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxIndices") << '\n'
		<< "MaxTextureSize: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxTextureSize") << '\n'
		<< "MaxGeometryVerticesOut: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"MaxGeometryVerticesOut") << '\n'
		<< "Version: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"Version") << '\n'
		<< "ShaderLanguageVersion: "
		<< driver->getDriverAttributes().getAttributeAsInt(
			"ShaderLanguageVersion") << '\n';

	device->closeDevice();
	device->run();
	device->drop();
	return true;
}

bool videoDriver()
{
	bool result = true;
	TestWithAllDrivers(testVideoDriver);
	return result;
}

int main()
{
	return runTest(videoDriver, "testVideoDriver");
}
