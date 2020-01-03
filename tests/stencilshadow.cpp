// Copyright (C) 2008-2012 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;

//
static bool stencilShadow(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice (driverType, core::dimension2d<u32>(160,120), 16, false, true);
	if (!device)
		return true; // No error if device does not exist

	stabilizeScreenBackground(device->getVideoDriver());

	scene::ICameraSceneNode* cam = device->getSceneManager()->addCameraSceneNodeFPS();
	cam->setPosition(core::vector3df(-15,60,40));
	cam->setTarget(core::vector3df(+25,-5,-25));

	device->getSceneManager()->setAmbientLight(video::SColorf(.5f,.5f,.5f));
	device->getSceneManager()->setShadowColor( video::SColor(255, 50, 0, 50));
	device->getSceneManager()->addCubeSceneNode(100, 0, -1, core::vector3df(0,50,0), core::vector3df(), core::vector3df(-1,-1,-1));

	scene::IAnimatedMeshSceneNode* node = device->getSceneManager()->addAnimatedMeshSceneNode(device->getSceneManager()->getMesh("../media/ninja.b3d"), 0, -1, core::vector3df(), core::vector3df(0.f, 230.f, 0.f),core::vector3df(5,5,5));
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->addShadowVolumeSceneNode(0, -1, true, 200.f);
	node->setAnimationSpeed(0.f);

	scene::IMeshSceneNode* cube2 = device->getSceneManager()->addCubeSceneNode(10, 0, -1, core::vector3df(40,0,0), core::vector3df(), core::vector3df(1,1,2.5f));
	cube2->getMaterial(0).DiffuseColor = video::SColor(220, 0, 100, 100);
	cube2->addShadowVolumeSceneNode(0, -1, false, 200.f);

	scene::ILightSceneNode* light = device->getSceneManager()->addLightSceneNode(0, core::vector3df(-40,10,20));
	light->setLightType(video::ELT_POINT);
	light->setRadius(500.f);
	light->getLightData().DiffuseColor.set(1,1,1);

	device->getVideoDriver()->beginScene(video::ECBF_ALL, video::SColor(0,0,0,0));
	device->getSceneManager()->drawAll();
	device->getVideoDriver()->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(device->getVideoDriver(), "-stencilShadow.png", 99.91f);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

// test self-shadowing
static bool selfShadowing(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice (driverType, core::dimension2d<u32>(160,120), 16, false, true);
	if (!device)
		return true; // No error if device does not exist

	stabilizeScreenBackground(device->getVideoDriver());

	device->getSceneManager()->setAmbientLight(video::SColorf(.5f,.5f,.5f));
	device->getSceneManager()->setShadowColor( video::SColor(220, 50, 0, 0));

	scene::IAnimatedMeshSceneNode* node = device->getSceneManager()->addAnimatedMeshSceneNode(device->getSceneManager()->getMesh("../media/dwarf.x"));
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->addShadowVolumeSceneNode();
	node->setAnimationSpeed(0.f);

	scene::ICameraSceneNode* cam = device->getSceneManager()->addCameraSceneNodeFPS();
	cam->setPosition(core::vector3df(0,55,-30));
	cam->setTarget(core::vector3df(60,45,150));

	scene::ILightSceneNode* light = device->getSceneManager()->addLightSceneNode(0, core::vector3df(100,100,-20));
	light->setLightType(video::ELT_POINT);
	light->setRadius(500.f);
	light->getLightData().DiffuseColor.set(0,1,1);

	device->getVideoDriver()->beginScene(video::ECBF_ALL, video::SColor(0,0,0,0));
	device->getSceneManager()->drawAll();
	device->getVideoDriver()->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(device->getVideoDriver(), "-stencilSelfShadow.png", 99.41f);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

bool stencilShadow(void)
{
	bool passed = true;

	passed &= stencilShadow(video::EDT_OPENGL);
	passed &= stencilShadow(video::EDT_DIRECT3D9);
	// no shadows in software renderer
//	passed &= stencilShadow(video::EDT_SOFTWARE);
	passed &= stencilShadow(video::EDT_BURNINGSVIDEO);	// Note: cube has wrong color, if that gets ever changed just update the test-image.

	passed &= selfShadowing(video::EDT_OPENGL);
	passed &= selfShadowing(video::EDT_DIRECT3D9);
	// no shadows in software renderer
//	passed &= selfShadowing(video::EDT_SOFTWARE);
	passed &= selfShadowing(video::EDT_BURNINGSVIDEO);

	return passed;
}
