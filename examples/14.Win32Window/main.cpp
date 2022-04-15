/** Example 014 Win32 Window

This example runs only under MS Windows and demonstrates how Irrlicht can
render inside a win32 window. MFC and .NET Windows.Forms windows are possible,
too.*/


#include <irrlicht.h>
#ifndef _IRR_WINDOWS_
#error Windows only example
#else
#include <windows.h> // this example only runs with Windows
#include <iostream>
#include "driverChoice.h"
#include "exampleHelper.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "irrlicht.lib")
#endif

HWND hOKButton = 0;

/*
	Windows message handler
*/
static LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			HWND hwndCtl = (HWND)lParam;
			int code = HIWORD(wParam);

			if (hwndCtl == hOKButton)
			{
				DestroyWindow(hWnd);
				PostQuitMessage(0);
				return 0;
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int main()
{
	/*
		Ask user for driver
	*/
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	printf("Select the render window (some dead window may exist too):\n"\
		" (a) Window with button (via CreationParam)\n"\
		" (b) Window with button (via beginScene)\n"\
		" (c) Own Irrlicht window (default behavior)\n"\
		" (otherKey) exit\n\n");

	char key;
	std::cin >> key;
	if (key != 'a' && key != 'b' && key != 'c')
		return 1;

	/*
	   Create the Windows specific window using the Windows API. 
	   Not further explained here, please see the MSDN or a Windows book
	   for details about doing that.
	*/

	HINSTANCE hInstance = 0;
	const fschar_t* Win32ClassName = __TEXT("CIrrlichtWindowsTestDialog");

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)CustomWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= DLGWINDOWEXTRA;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= Win32ClassName;
	wcex.hIconSm		= 0;

	RegisterClassEx(&wcex);

	DWORD style = WS_SYSMENU | WS_BORDER | WS_CAPTION |
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;

	int windowWidth = 440;
	int windowHeight = 380;

	HWND hWnd = CreateWindow( Win32ClassName, __TEXT("Irrlicht Win32 window example"),
		style, 100, 100, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL);

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	windowWidth = clientRect.right;
	windowHeight = clientRect.bottom;

	// Create OK button
	hOKButton = CreateWindow(__TEXT("BUTTON"), __TEXT("OK - Close"), WS_CHILD | WS_VISIBLE | BS_TEXT,
		windowWidth - 160, windowHeight - 40, 150, 30, hWnd, NULL, hInstance, NULL);

	// Create some text
	CreateWindow(__TEXT("STATIC"), __TEXT("This is Irrlicht running inside a standard Win32 window.\n")\
		__TEXT("Also mixing with MFC and .NET Windows.Forms is possible."),
		WS_CHILD | WS_VISIBLE, 20, 20, 400, 40, hWnd, NULL, hInstance, NULL);

	// Create a window to put Irrlicht in
	HWND hIrrlichtWindow = CreateWindow(__TEXT("BUTTON"), __TEXT(""),
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			50, 80, 320, 220, hWnd, NULL, hInstance, NULL);

	/*
	So now that we have some Windows window, we can use it with Irrlicht.
	There's several options.
	*/
	
	irr::SIrrlichtCreationParameters param;
	param.DriverType = driverType;

	/* First option: We create an Irrlicht device inside of the Windows window. 
	We use Irrlicht createEx() function for this. We do need the 
	handle (HWND) for that window, set it as windowsID parameter
	and start up the engine as usual. That's it.
	*/

	if (key=='a')
		param.WindowId = reinterpret_cast<void*>(hIrrlichtWindow);

	irr::IrrlichtDevice* device = irr::createDeviceEx(param);
	irr::scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver* driver = device->getVideoDriver();

	/*
		Second option: We create a typical Irrlicht device, but render to the Window window.
		For rendering into another Window than the one used for creating the Irrlicht device
		we have to pass some changed SExposedVideoData to beginScene which contains the 
		HWND of the Windows window.
	*/
	video::SExposedVideoData videodata((key=='b')?hIrrlichtWindow:0);

	/*
		OpenGL needs a bit more setup.
		Also not yet working as well (haven't figured out yet how to render into the Irrlicht window as well)
	*/
	if (key == 'b' && driverType==video::EDT_OPENGL)
	{
		HDC HDc=GetDC(hIrrlichtWindow);
		PIXELFORMATDESCRIPTOR pfd={0};
		pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
		int pf = GetPixelFormat(HDc);
		DescribePixelFormat(HDc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		pfd.dwFlags |= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.cDepthBits=16;
		pf = ChoosePixelFormat(HDc, &pfd);
		SetPixelFormat(HDc, pf, &pfd);
		videodata.OpenGLWin32.HDc = HDc;
		videodata.OpenGLWin32.HRc=wglCreateContext(HDc);
		wglShareLists((HGLRC)driver->getExposedVideoData().OpenGLWin32.HRc, (HGLRC)videodata.OpenGLWin32.HRc);
	}

	/* 
		Setup a simple 3d scene
	*/
	scene::ICameraSceneNode* cam = smgr->addCameraSceneNode();
	cam->setTarget(core::vector3df(0,0,0));

	scene::ISceneNodeAnimator* anim =
		smgr->createFlyCircleAnimator(core::vector3df(0,15,0), 30.0f);
	cam->addAnimator(anim);
	anim->drop();

	scene::ISceneNode* cube = smgr->addCubeSceneNode(20);

	const io::path mediaPath = getExampleMediaPath();

	cube->setMaterialTexture(0, driver->getTexture(mediaPath + "wall.bmp"));
	cube->setMaterialTexture(1, driver->getTexture(mediaPath + "water.jpg"));
	cube->setMaterialFlag( video::EMF_LIGHTING, false );
	cube->setMaterialType( video::EMT_REFLECTION_2_LAYER );

	smgr->addSkyBoxSceneNode(
	driver->getTexture(mediaPath + "irrlicht2_up.jpg"),
	driver->getTexture(mediaPath + "irrlicht2_dn.jpg"),
	driver->getTexture(mediaPath + "irrlicht2_lf.jpg"),
	driver->getTexture(mediaPath + "irrlicht2_rt.jpg"),
	driver->getTexture(mediaPath + "irrlicht2_ft.jpg"),
	driver->getTexture(mediaPath + "irrlicht2_bk.jpg"));

	// This shows that we can render to multiple windows within one application
	// TODO: Currently not working with OpenGL
	device->getGUIEnvironment()->addStaticText(core::stringw("Second screen render").c_str(),core::recti(0,0,200,200));

	// show and execute the Windows dialog
	ShowWindow(hWnd , SW_SHOW);
	UpdateWindow(hWnd);

	// do message queue

#if 1	// Irrlicht does the message handling with device->run()

	/*
	Now the only thing missing is the drawing loop using
	IrrlichtDevice::run(). We do this as usual. But instead of this, there
	is another possibility: You can also  use your own message loop
	using GetMessage, DispatchMessage and whatever. Calling
	device->run() will cause Irrlicht to dispatch messages internally too.
	You need not call device->run() if you want to do your own message
	dispatching loop, but Irrlicht will not be able to fetch user input
	then and you have to do it on your own using the window messages,
	DirectInput, or whatever.
	*/
	while (device->run())
	{
		// draw 3d scene
		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0), 1.f, 0, videodata);
		smgr->drawAll();
		driver->endScene();

		// draw gui into second window
		if (key=='b')
		{
			driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0xbbbbbbbb));
			device->getGUIEnvironment()->drawAll();
			driver->endScene();
		}
	}

#else	// Windows API does the message handling
	/*
	The alternative, own message dispatching loop without Device->run()
	would look like this:
	*/

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		// advance virtual time
		device->getTimer()->tick();

		// draw 3d scene
		driver->beginScene(true, true, 0, videodata);
		smgr->drawAll();
		driver->endScene();

		// draw gui into second window
		if (key=='b')
		{
			driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0xbbbbbbbb));
			device->getGUIEnvironment()->drawAll();
			driver->endScene();
		}
	}
#endif

	device->closeDevice();
	device->drop();

	return 0;
}
#endif // if windows

/*
That's it, Irrlicht now uses a Windows window.
**/
