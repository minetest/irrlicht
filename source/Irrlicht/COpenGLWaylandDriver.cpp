#include "COpenGLWaylandDriver.h"

#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_

namespace irr
{
namespace video
{

bool COpenGLWaylandDriver::initDriver(CIrrDeviceWayland *device)
{
	mContext = device->getCurrentContext();
	mDisplay = device->getCurrentDisplay();
	ExposedData.OpenGLLinux.X11Window = (unsigned long)Params.WindowId;
	mDrawable = eglGetCurrentSurface(EGL_DRAW);

	genericDriverInit();

	// set vsync
	eglSwapInterval(mDisplay, Params.Vsync ? 1 : 0);
	return true;
}

bool COpenGLWaylandDriver::genericDriverInit()
{
		return COpenGLDriver::genericDriverInit();
}
}
}
#endif