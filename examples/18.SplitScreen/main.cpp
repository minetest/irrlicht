/** Example 018 Splitscreen

A tutorial by Max Winkel.

In this tutorial we'll learn how to use split screen (e.g. for racing-games)
with Irrlicht. We'll create a viewport divided into 4 parts, with 3 fixed 
cameras and one user-controlled.

OK, let's start with the headers (I think there's
nothing to say about it)
*/

#include <irrlicht.h>
#include "driverChoice.h"
#include "exampleHelper.h"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

//Namespaces for the engine
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

/*
Now we'll define the resolution in a constant for use in
initializing the device and setting up the viewport. 
*/

//Resolution
const int ResX=800;
const int ResY=600;

//Use SplitScreen?
bool SplitScreen=true;

// We allow quitting the with the ESC key
bool Quit = false;

/*
We need four pointers to our cameras which are created later:
*/
ICameraSceneNode *Camera[4]={0,0,0,0};

/*
In our event-receiver we switch the SplitScreen-variable,
whenever the user press the S-key. 
We also allow quitting the application with ESC. 
*/
class MyEventReceiver : public IEventReceiver
{
	public:
		virtual bool OnEvent(const SEvent& event)
		{
			//Key S enables/disables SplitScreen
			if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown )
			{
				if ( event.KeyInput.Key == KEY_KEY_S )
				{
					SplitScreen = !SplitScreen;
					return true;
				}
				if (event.KeyInput.Key == KEY_ESCAPE )
				{
					Quit = true;
					return true;
				}
			}

			return false;
		}
};

/*
The main-function:
First, we initialize the device, get some useful variables, 
load an animated mesh from .md2 and a map from .pk3. 
Because that's old stuff, I won't explain every step.
Just take care of the maps position.
*/
int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	//Instance of the EventReceiver
	MyEventReceiver receiver;

	//Initialize the engine
	const bool fullScreen=false;
	IrrlichtDevice *device = createDevice(driverType,
			dimension2du(ResX,ResY), 32, fullScreen,
			false, false, &receiver);
	if (!device)
		return 1;

	ISceneManager *smgr = device->getSceneManager();
	IVideoDriver *driver = device->getVideoDriver();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	const io::path mediaPath = getExampleMediaPath();

	//Load model
	IAnimatedMesh *model = smgr->getMesh(mediaPath + "sydney.md2");
	if (!model)
		return 1;
	IAnimatedMeshSceneNode *model_node = smgr->addAnimatedMeshSceneNode(model);
	//Load texture
	if (model_node)
	{
		ITexture *texture = driver->getTexture(mediaPath + "sydney.bmp");
		model_node->setMaterialTexture(0,texture);
		model_node->setMD2Animation(scene::EMAT_RUN);
		//Disable lighting (we've got no light)
		model_node->setMaterialFlag(EMF_LIGHTING,false);
	}

	//Load map
	device->getFileSystem()->addFileArchive(mediaPath + "map-20kdm2.pk3");
	IAnimatedMesh *map = smgr->getMesh("20kdm2.bsp");
	if (map)
	{
		ISceneNode *map_node = smgr->addOctreeSceneNode(map->getMesh(0));
		//Set position
		map_node->setPosition(vector3df(-850,-220,-850));
	}

	// Add some static text gui element, to give users information and how to show how using the UI works.
	guienv->addStaticText(L"<S> to switch split screen.\n<ESC> to quit", recti(5,5, 100, 30), false, true, 0, -1, true);

/*
Now we create our four cameras. One is looking at the model
from the front, one from the top and one from the side. In
addition there's a FPS-camera which can be controlled by the
user.
*/
	// Create 3 fixed and one user-controlled cameras
	//Front
	Camera[0] = smgr->addCameraSceneNode(0, vector3df(50,0,0), vector3df(0,0,0));
	//Top
	Camera[1] = smgr->addCameraSceneNode(0, vector3df(0,50,0), vector3df(0,0,0));
	//Left
	Camera[2] = smgr->addCameraSceneNode(0, vector3df(0,0,50), vector3df(0,0,0));
	//User-controlled
	Camera[3] = smgr->addCameraSceneNodeFPS();
	// don't start at Sydney's position
	if (Camera[3])
		Camera[3]->setPosition(core::vector3df(-50,0,-50));

/*
Hide the mouse and create a variable for counting the fps:
*/
	//Hide mouse
	device->getCursorControl()->setVisible(false);
	//We want to count the fps
	int lastFPS = -1;

/*
There wasn't much new stuff - till now!
The game won't be split the screen just by defining four cameras.
To do this several steps are needed:
  - Set the viewport to the whole screen
  - Begin a new scene (Clear screen)

  - The following 3 steps are repeated for every viewport in the split screen
    - Set the viewport to the area you wish
    - Activate the camera which should be "linked" with the viewport
    - Render all objects

  - If you have a GUI:
    - Set the viewport the whole screen
    - Display the GUI
  - End scene

Sounds a little complicated, but you'll see it isn't:
*/

	while(!Quit && device->run())
	{
		// Don't reset mouse cursor when window is not active
		Camera[3]->setInputReceiverEnabled(device->isWindowActive());

		//Set the viewpoint to the whole screen and begin scene
		driver->setViewPort(rect<s32>(0,0,ResX,ResY));
		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, SColor(255,100,100,100));
		//If split screen is used
		if (SplitScreen)
		{
			//Activate camera1
			smgr->setActiveCamera(Camera[0]);
			//Set viewpoint to the first quarter (left top)
			driver->setViewPort(rect<s32>(0,0,ResX/2,ResY/2));
			//Draw scene
			smgr->drawAll();
			//Activate camera2
			smgr->setActiveCamera(Camera[1]);
			//Set viewpoint to the second quarter (right top)
			driver->setViewPort(rect<s32>(ResX/2,0,ResX,ResY/2));
			//Draw scene
			smgr->drawAll();
			//Activate camera3
			smgr->setActiveCamera(Camera[2]);
			//Set viewpoint to the third quarter (left bottom)
			driver->setViewPort(rect<s32>(0,ResY/2,ResX/2,ResY));
			//Draw scene
			smgr->drawAll();
			//Set viewport the last quarter (right bottom)
			driver->setViewPort(rect<s32>(ResX/2,ResY/2,ResX,ResY));
		}
		//Activate camera4
		smgr->setActiveCamera(Camera[3]);
		//Draw scene
		smgr->drawAll();

		// Back to whole screen for the UI
		if (SplitScreen)
			driver->setViewPort(rect<s32>(0,0,ResX,ResY));
		guienv->drawAll();

		driver->endScene();

		/*
		As you can probably see, the image is rendered for every
		viewport separately. That means, that you'll loose much performance.
		OK, if you're asking "How do I have to set the viewport
		to get this or that screen?", don't panic. It's really
		easy: In the rect-function you define 4 coordinates:
		- X-coordinate of the corner left top
		- Y-coordinate of the corner left top
		- X-coordinate of the corner right bottom
		- Y-coordinate of the corner right bottom

		That means, if you want to split the screen into 2 viewports
		you would give the following coordinates:
		- 1st viewport: 0,0,ResX/2,ResY
		- 2nd viewport: ResX/2,0,ResX,ResY

		If you didn't fully understand, just play around with the example
		to check out what happens.

		Last we show the current fps.
		*/
		if (driver->getFPS() != lastFPS)
		{
			lastFPS = driver->getFPS();
			core::stringw tmp = L"Irrlicht SplitScreen-Example (FPS: ";
			tmp += lastFPS;
			tmp += ")";
			device->setWindowCaption(tmp.c_str());
		}
	}
	//Delete device
	device->drop();
	return 0;
}
/*
That's it! Just compile and play around with the program.
**/
