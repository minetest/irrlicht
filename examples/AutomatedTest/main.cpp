#include <irrlicht.h>
#include "exampleHelper.h"

using namespace irr;

static video::E_DRIVER_TYPE chooseDriver(const char *arg_)
{
	if (core::stringc(arg_) == "null")
		return video::EDT_NULL;

	if (IrrlichtDevice::isDriverSupported(video::EDT_OGLES1))
		return video::EDT_OGLES1;
	if (IrrlichtDevice::isDriverSupported(video::EDT_OGLES2))
		return video::EDT_OGLES2;
	return video::EDT_OPENGL;
}

int main(int argc, char *argv[])
{
	SIrrlichtCreationParameters p;
	p.DriverType = chooseDriver(argc > 1 ? argv[1] : "");
	p.WindowSize = core::dimension2du(640, 480);
	p.Vsync = true;
	p.LoggingLevel = ELL_DEBUG;

	IrrlichtDevice *device = createDeviceEx(p);
	if (!device)
		return 1;

	device->setWindowCaption(L"Hello World!");
	device->setResizable(true);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	guienv->addStaticText(L"sample text", core::rect<s32>(10,10,110,22), false);

	gui::IGUIButton* button = guienv->addButton(
		core::rect<s32>(10,30,110,30 + 32), 0, -1, L"sample button",
		L"sample tooltip");

	gui::IGUIEditBox* editbox = guienv->addEditBox(L"",
		core::rect<s32>(10,70,60,70 + 16));

	const io::path mediaPath = getExampleMediaPath();

	scene::IAnimatedMesh* mesh = smgr->getMesh(mediaPath + "coolguy_opt.x");
	if (!mesh)
		return 1;
	scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
	if (node)
	{
		node->setMaterialFlag(video::EMF_LIGHTING, false);
		node->setFrameLoop(0, 29);
		node->setAnimationSpeed(30);
		node->setMaterialTexture(0, driver->getTexture(mediaPath + "cooltexture.png"));
	}

	smgr->addCameraSceneNode(0, core::vector3df(0,30,-40), core::vector3df(0,5,0));

	s32 n = 0;
	SEvent event;
	device->getTimer()->start();

	while (device->run())
	{
		if (device->getTimer()->getTime() >= 1300)
		{
			device->getTimer()->setTime(0);
			++n;
			if (n == 1) // Tooltip display
			{
				bzero(&event, sizeof(SEvent));
				event.EventType = irr::EET_MOUSE_INPUT_EVENT;
				event.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
				event.MouseInput.X = button->getAbsolutePosition().getCenter().X;
				event.MouseInput.Y = button->getAbsolutePosition().getCenter().Y;
				device->postEventFromUser(event);
			}
			else if (n == 2) // Text input focus
				guienv->setFocus(editbox);
			else if (n == 3) // Keypress for Text input
			{
				bzero(&event, sizeof(SEvent));
				event.EventType = irr::EET_KEY_INPUT_EVENT;
				event.KeyInput.Char = L'a';
				event.KeyInput.Key = KEY_KEY_A;
				event.KeyInput.PressedDown = true;
				device->postEventFromUser(event);
				event.KeyInput.PressedDown = false;
				device->postEventFromUser(event);
			}
			else
				device->closeDevice();
		}

		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH,
			video::SColor(255,100,100,150));
		smgr->drawAll();
		guienv->drawAll();
		driver->endScene();
	}

	if (core::stringw(L"a") != editbox->getText()) {
		device->getLogger()->log("EditBox text mismatch", ELL_INFORMATION);
		return 1;
	}

	device->getLogger()->log("Done.", ELL_INFORMATION);
	device->drop();
	return 0;
}
