// Copyright (C) 2008-2012 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;

// Tests mesh loading features and the mesh cache.
/** This won't test render results. Currently, not all mesh loaders are tested. */
bool meshLoaders(void)
{
	bool passed = true;
	IrrlichtDevice *device = createDevice(video::EDT_NULL, core::dimension2d<u32>(160, 120), 32);
	if (!assertLog(device))
		return false;

	scene::ISceneManager * smgr = device->getSceneManager();
	scene::IAnimatedMesh* mesh = smgr->getMesh("../media/ninja.b3d");
	passed &= assertLog(mesh);
	if (passed)
		passed &= assertLog(
			mesh == smgr->getMesh("../media/ninja.b3d"));

	device->closeDevice();
	device->run();
	device->drop();

	return passed;
}

int main()
{
	return runTest(meshLoaders, "testMeshLoaders");
}
