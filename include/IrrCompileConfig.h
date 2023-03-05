// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_COMPILE_CONFIG_H_INCLUDED__
#define __IRR_COMPILE_CONFIG_H_INCLUDED__

//! Identifies the IrrlichtMt fork customized for the Minetest engine
#define IRRLICHT_VERSION_MT_REVISION 10
#define IRRLICHT_VERSION_MT "mt10"

//! Irrlicht SDK Version
#define IRRLICHT_VERSION_MAJOR 1
#define IRRLICHT_VERSION_MINOR 9
#define IRRLICHT_VERSION_REVISION 0
// This flag will be defined only in SVN, the official release code will have
// it undefined
#define IRRLICHT_VERSION_SVN alpha
#define IRRLICHT_SDK_VERSION "1.9.0" IRRLICHT_VERSION_MT

#include <stdio.h> // TODO: Although included elsewhere this is required at least for mingw

//! The defines for different operating system are:
//! _IRR_WINDOWS_ for all irrlicht supported Windows versions
//! _IRR_WINDOWS_API_ for Windows or XBox
//! _IRR_LINUX_PLATFORM_ for Linux (it is defined here if no other os is defined)
//! _IRR_SOLARIS_PLATFORM_ for Solaris
//! _IRR_OSX_PLATFORM_ for Apple systems running OSX
//! _IRR_IOS_PLATFORM_ for Apple devices running iOS
//! _IRR_ANDROID_PLATFORM_ for devices running Android
//! _IRR_POSIX_API_ for Posix compatible systems
//! Note: PLATFORM defines the OS specific layer, API can group several platforms

//! DEVICE is the windowing system used, several PLATFORMs support more than one DEVICE
//! Irrlicht can be compiled with more than one device
//! _IRR_COMPILE_WITH_WINDOWS_DEVICE_ for Windows API based device
//! _IRR_COMPILE_WITH_OSX_DEVICE_ for Cocoa native windowing on OSX
//! _IRR_COMPILE_WITH_X11_DEVICE_ for Linux X11 based device
//! _IRR_COMPILE_WITH_SDL_DEVICE_ for platform independent SDL framework

//! Passing defines to the compiler which have NO in front of the _IRR definename is an alternative
//! way which can be used to disable defines (instead of outcommenting them in this header).
//! So defines can be controlled from Makefiles or Projectfiles which allows building
//! different library versions without having to change the sources.
//! Example: NO_IRR_COMPILE_WITH_X11_ would disable X11

//! Uncomment this line to compile with the SDL device
//#define _IRR_COMPILE_WITH_SDL_DEVICE_
#ifdef NO_IRR_COMPILE_WITH_SDL_DEVICE_
#undef _IRR_COMPILE_WITH_SDL_DEVICE_
#endif

//! WIN32 for Windows32
//! WIN64 for Windows64
// The windows platform and API support SDL and WINDOW device
#if defined(_WIN32) || defined(_WIN64)
#define _IRR_WINDOWS_
#define _IRR_WINDOWS_API_
#ifndef _IRR_COMPILE_WITH_SDL_DEVICE_
#define _IRR_COMPILE_WITH_WINDOWS_DEVICE_
#endif
#endif

#if defined(__APPLE__)
#if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#define _IRR_IOS_PLATFORM_
#define _IRR_COMPILE_WITH_IOS_DEVICE_
#define NO_IRR_COMPILE_WITH_OPENGL_
// The application state events and following methods: IrrlichtDevice::isWindowActive, IrrlichtDevice::isWindowFocused,
// IrrlichtDevice::isWindowMinimized works out of box only if you'll use built-in CIrrDelegateiOS,
// so _IRR_COMPILE_WITH_IOS_BUILTIN_MAIN_ must be enabled in this case. If you need a custom UIApplicationDelegate
// you must disable _IRR_COMPILE_WITH_IOS_BUILTIN_MAIN_ definition and handle all application events yourself.
#define _IRR_COMPILE_WITH_IOS_BUILTIN_MAIN_
#else
#define _IRR_OSX_PLATFORM_
#ifndef _IRR_COMPILE_WITH_SDL_DEVICE_
#define _IRR_COMPILE_WITH_OSX_DEVICE_
#endif
#define NO_IRR_COMPILE_WITH_OGLES1_
#define NO_IRR_COMPILE_WITH_OGLES2_
#define NO_IRR_COMPILE_WITH_WEBGL1_
#endif
#endif

#if defined(__EMSCRIPTEN__)
#define _IRR_EMSCRIPTEN_PLATFORM_
#define NO_IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#define NO_IRR_COMPILE_WITH_OPENGL_
#define NO_IRR_COMPILE_WITH_OGLES1_
#define _IRR_COMPILE_WITH_OGLES2_
#define _IRR_COMPILE_WITH_WEBGL1_
#define _IRR_COMPILE_WITH_EGL_MANAGER_
#define _IRR_COMPILE_WITH_SDL_DEVICE_
#define NO_IRR_COMPILE_WITH_X11_DEVICE_
#define _IRR_LINUX_PLATFORM_	// emscripten basically working like a unix
#endif // __EMSCRIPTEN__

#if defined(__ANDROID__)
#define _IRR_ANDROID_PLATFORM_
#define _IRR_COMPILE_WITH_ANDROID_DEVICE_
#define _IRR_COMPILE_ANDROID_ASSET_READER_
#define NO_IRR_COMPILE_WITH_OPENGL_
#endif

#if defined(__SVR4) && defined(__sun)
#define _IRR_SOLARIS_PLATFORM_
#if defined(__sparc)
	#define __BIG_ENDIAN__
#endif
#endif

#if !defined(_IRR_WINDOWS_API_) && !defined(_IRR_OSX_PLATFORM_) && !defined(_IRR_IOS_PLATFORM_) && !defined(_IRR_ANDROID_PLATFORM_) && !defined(_IRR_EMSCRIPTEN_PLATFORM_)
#ifndef _IRR_SOLARIS_PLATFORM_
#define _IRR_LINUX_PLATFORM_
#endif
#define _IRR_POSIX_API_
#ifndef _IRR_COMPILE_WITH_SDL_DEVICE_
#define _IRR_COMPILE_WITH_X11_DEVICE_
#endif
#endif


//! Define _IRR_COMPILE_WITH_JOYSTICK_SUPPORT_ if you want joystick events.
#if !(defined(__FreeBSD__) || defined(__OpenBSD__))
#define _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#endif
#ifdef NO_IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#undef _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#endif


//! Maximum number of texture an SMaterial can have, up to 8 are supported by Irrlicht.
#define _IRR_MATERIAL_MAX_TEXTURES_ 4

//! Add a leak-hunter to Irrlicht which helps finding unreleased reference counted objects.
//! NOTE: This is slow and should only be used for debugging
//#define _IRR_COMPILE_WITH_LEAK_HUNTER_
#ifdef NO_IRR_COMPILE_WITH_LEAK_HUNTER_
#undef _IRR_COMPILE_WITH_LEAK_HUNTER_
#endif

//! Enable profiling information in the engine
/** NOTE: The profiler itself always exists and can be used by applications.
This define is about the engine creating profile data
while it runs and enabling it will slow down the engine. */
//#define _IRR_COMPILE_WITH_PROFILING_
#ifdef NO_IRR_COMPILE_WITH_PROFILING_
#undef _IRR_COMPILE_WITH_PROFILING_
#endif

//! Define _IRR_COMPILE_WITH_DIRECT3D_9_ to compile the Irrlicht engine with DIRECT3D9.
/** If you only want to use the software device or opengl you can disable those defines.
This switch is mostly disabled because people do not get the g++ compiler compile
directX header files, and directX is only available on Windows platforms. If you
are using Dev-Cpp, and want to compile this using a DX dev pack, you can define
_IRR_COMPILE_WITH_DX9_DEV_PACK_. So you simply need to add something like this
to the compiler settings: -DIRR_COMPILE_WITH_DX9_DEV_PACK
and this to the linker settings: -ld3dx9
*/
#if defined(_IRR_WINDOWS_API_) && defined(IRR_COMPILE_WITH_DX9_DEV_PACK)

//! Define _IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_ if you want to use DirectInput for joystick handling.
/** This only applies to Windows devices, currently only supported under Win32 device.
If not defined, Windows Multimedia library is used, which offers also broad support for joystick devices. */
#define _IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_
#ifdef NO_IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_
#undef _IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_
#endif

#endif

//! Define _IRR_COMPILE_WITH_OPENGL_ to compile the Irrlicht engine with OpenGL.
/** If you do not wish the engine to be compiled with OpenGL, comment this
define out. */
#define _IRR_COMPILE_WITH_OPENGL_
#ifdef NO_IRR_COMPILE_WITH_OPENGL_
#undef _IRR_COMPILE_WITH_OPENGL_
#endif

//! Define required options for OpenGL drivers.
#if defined(_IRR_COMPILE_WITH_OPENGL_)
	#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
		#define _IRR_COMPILE_WITH_WGL_MANAGER_
	#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
		#define _IRR_COMPILE_WITH_GLX_MANAGER_
	#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
		#define _IRR_COMPILE_WITH_NSOGL_MANAGER_
	#elif defined(_IRR_SOLARIS_PLATFORM_)
		#define _IRR_COMPILE_WITH_GLX_MANAGER_
	#elif defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
	#endif
#endif

//! Define _IRR_COMPILE_WITH_OGLES1_ to compile the Irrlicht engine with OpenGL ES 1.1.
/** If you do not wish the engine to be compiled with OpenGL ES 1.1, comment this
define out.
Depending on platform you may have to enable _IRR_OGLES1_USE_KHRONOS_API_HEADERS_ as well when using it.
*/
#if defined(_IRR_ANDROID_PLATFORM_) || defined(_IRR_IOS_PLATFORM_)
#define _IRR_COMPILE_WITH_OGLES1_
#endif
#ifdef NO_IRR_COMPILE_WITH_OGLES1_
#undef _IRR_COMPILE_WITH_OGLES1_
#endif

#ifdef _IRR_COMPILE_WITH_OGLES1_
//! Define _IRR_OGLES1_USE_KHRONOS_API_HEADERS_ to use the OpenGL ES headers from the Debian Khronos-api package
//#define _IRR_OGLES1_USE_KHRONOS_API_HEADERS_
#endif

//! Define required options for OpenGL ES 1.1 drivers.
#if defined(_IRR_COMPILE_WITH_OGLES1_)
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_) || defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
#define _IRR_OGLES1_USE_EXTPOINTER_
#ifndef _IRR_COMPILE_WITH_EGL_MANAGER_
#define _IRR_COMPILE_WITH_EGL_MANAGER_
#endif
#elif defined(_IRR_COMPILE_WITH_IOS_DEVICE_)
#ifndef _IRR_COMPILE_WITH_EAGL_MANAGER_
#define _IRR_COMPILE_WITH_EAGL_MANAGER_
#endif
#endif
#endif

//! Define _IRR_COMPILE_WITH_OGLES2_ to compile the Irrlicht engine with OpenGL ES 2.0.
/** If you do not wish the engine to be compiled with OpenGL ES 2.0, comment this
define out. */
#if defined(_IRR_ANDROID_PLATFORM_) || defined(_IRR_IOS_PLATFORM_)
#define _IRR_COMPILE_WITH_OGLES2_
#else
//#define _IRR_COMPILE_WITH_OGLES2_
#endif
#ifdef NO_IRR_COMPILE_WITH_OGLES2_
#undef _IRR_COMPILE_WITH_OGLES2_
#endif

//! Define _IRR_COMPILE_WITH_WEBGL1_ to compile Irrlicht engine with a WebGL friendly
//! subset of the OpenGL ES 2.0 driver.
//#define _IRR_COMPILE_WITH_WEBGL1_
#ifdef NO_IRR_COMPILE_WITH_WEBGL1_
#undef _IRR_COMPILE_WITH_WEBGL1_
#endif
#ifdef _IRR_COMPILE_WITH_WEBGL1_
#define _IRR_COMPILE_WITH_OGLES2_ //  it's a subset of OGL ES2, so always needed when using WebGL
#endif

//! Define required options for OpenGL ES 2.0 drivers.
#if defined(_IRR_COMPILE_WITH_OGLES2_)
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_) || defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_) || defined(__EMSCRIPTEN__)
#define _IRR_OGLES2_USE_EXTPOINTER_
#ifndef _IRR_COMPILE_WITH_EGL_MANAGER_
#define _IRR_COMPILE_WITH_EGL_MANAGER_
#endif
#elif defined(_IRR_COMPILE_WITH_IOS_DEVICE_)
#ifndef _IRR_COMPILE_WITH_EAGL_MANAGER_
#define _IRR_COMPILE_WITH_EAGL_MANAGER_
#endif
#endif
#endif



//! Define _IRR_COMPILE_WITH_X11_ to compile the Irrlicht engine with X11 support.
/** If you do not wish the engine to be compiled with X11, comment this
define out. */
// Only used in LinuxDevice.
#define _IRR_COMPILE_WITH_X11_
#ifdef NO_IRR_COMPILE_WITH_X11_
#undef _IRR_COMPILE_WITH_X11_
#endif

#if defined(_IRR_LINUX_PLATFORM_) && defined(_IRR_COMPILE_WITH_X11_)
//! XInput2 (library called Xi) is currently only used to support touch-input.
#define _IRR_LINUX_X11_XINPUT2_
#ifdef NO_IRR_LINUX_X11_XINPUT2_
#undef _IRR_LINUX_X11_XINPUT2_
#endif

//! X11 has by default only monochrome cursors, but using the Xcursor library we can also get color cursor support.
//! If you have the need for custom color cursors on X11 then enable this and make sure you also link
//! to the Xcursor library in your Makefile/Projectfile.
//#define _IRR_LINUX_XCURSOR_
#ifdef NO_IRR_LINUX_XCURSOR_
#undef _IRR_LINUX_XCURSOR_
#endif

#endif

//! Define _IRR_COMPILE_WITH_GUI_ to compile the engine with the built-in GUI
/** Disable this if you are using an external library to draw the GUI. If you disable this then
you will not be able to use anything provided by the GUI Environment, including loading fonts. */
#define _IRR_COMPILE_WITH_GUI_
#ifdef NO_IRR_COMPILE_WITH_GUI_
#undef _IRR_COMPILE_WITH_GUI_
#endif

//! Define _IRR_COMPILE_WITH_LIBJPEG_ to enable compiling the engine using libjpeg.
/** This enables the engine to read jpeg images. If you comment this out,
the engine will no longer read .jpeg images. */
#define _IRR_COMPILE_WITH_LIBJPEG_
#ifdef NO_IRR_COMPILE_WITH_LIBJPEG_
#undef _IRR_COMPILE_WITH_LIBJPEG_
#endif

//! Define _IRR_COMPILE_WITH_LIBPNG_ to enable compiling the engine using libpng.
/** This enables the engine to read png images. If you comment this out,
the engine will no longer read .png images. */
#define _IRR_COMPILE_WITH_LIBPNG_
#ifdef NO_IRR_COMPILE_WITH_LIBPNG_
#undef _IRR_COMPILE_WITH_LIBPNG_
#endif

//! Uncomment the following line if you want to ignore the deprecated warnings
//#define IGNORE_DEPRECATED_WARNING

//! Define _IRR_COMPILE_WITH_BILLBOARD_SCENENODE_ to support BillboardSceneNodes
#define _IRR_COMPILE_WITH_BILLBOARD_SCENENODE_
#ifdef NO_IRR_COMPILE_WITH_BILLBOARD_SCENENODE_
#undef _IRR_COMPILE_WITH_BILLBOARD_SCENENODE_
#endif

//! Define _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_ if you want to use bone based
/** animated meshes. If you compile without this, you will be unable to load
B3D, MS3D or X meshes */
#define _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_
#ifdef NO_IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_
#undef _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_
#endif

#ifdef _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_
//! Define _IRR_COMPILE_WITH_B3D_LOADER_ if you want to use Blitz3D files
#define _IRR_COMPILE_WITH_B3D_LOADER_
#ifdef NO_IRR_COMPILE_WITH_B3D_LOADER_
#undef _IRR_COMPILE_WITH_B3D_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_X_LOADER_ if you want to use Microsoft X files
#define _IRR_COMPILE_WITH_X_LOADER_
#ifdef NO_IRR_COMPILE_WITH_X_LOADER_
#undef _IRR_COMPILE_WITH_X_LOADER_
#endif
#endif // _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_

//! Define _IRR_COMPILE_WITH_OBJ_LOADER_ if you want to load Wavefront OBJ files
#define _IRR_COMPILE_WITH_OBJ_LOADER_
#ifdef NO_IRR_COMPILE_WITH_OBJ_LOADER_
#undef _IRR_COMPILE_WITH_OBJ_LOADER_
#endif

//! Define _IRR_COMPILE_WITH_BMP_LOADER_ if you want to load .bmp files
//! Disabling this loader will also disable the built-in font
#define _IRR_COMPILE_WITH_BMP_LOADER_
#ifdef NO_IRR_COMPILE_WITH_BMP_LOADER_
#undef _IRR_COMPILE_WITH_BMP_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_JPG_LOADER_ if you want to load .jpg files
#define _IRR_COMPILE_WITH_JPG_LOADER_
#ifdef NO_IRR_COMPILE_WITH_JPG_LOADER_
#undef _IRR_COMPILE_WITH_JPG_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_PNG_LOADER_ if you want to load .png files
#define _IRR_COMPILE_WITH_PNG_LOADER_
#ifdef NO_IRR_COMPILE_WITH_PNG_LOADER_
#undef _IRR_COMPILE_WITH_PNG_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_TGA_LOADER_ if you want to load .tga files
#define _IRR_COMPILE_WITH_TGA_LOADER_
#ifdef NO_IRR_COMPILE_WITH_TGA_LOADER_
#undef _IRR_COMPILE_WITH_TGA_LOADER_
#endif

//! Define _IRR_COMPILE_WITH_JPG_WRITER_ if you want to write .jpg files
#define _IRR_COMPILE_WITH_JPG_WRITER_
#ifdef NO_IRR_COMPILE_WITH_JPG_WRITER_
#undef _IRR_COMPILE_WITH_JPG_WRITER_
#endif
//! Define _IRR_COMPILE_WITH_PNG_WRITER_ if you want to write .png files
#define _IRR_COMPILE_WITH_PNG_WRITER_
#ifdef NO_IRR_COMPILE_WITH_PNG_WRITER_
#undef _IRR_COMPILE_WITH_PNG_WRITER_
#endif

//! Define __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_ if you want to open ZIP and GZIP archives
/** ZIP reading has several more options below to configure. */
#define __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#endif
#ifdef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
//! Define _IRR_COMPILE_WITH_ZLIB_ to enable compiling the engine using zlib.
/** This enables the engine to read from compressed .zip archives. If you
disable this feature, the engine can still read archives, but only uncompressed
ones. */
#define _IRR_COMPILE_WITH_ZLIB_
#ifdef NO_IRR_COMPILE_WITH_ZLIB_
#undef _IRR_COMPILE_WITH_ZLIB_
#endif
#endif

//! Define __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_ if you want to mount folders as archives
#define __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#endif

// Some cleanup and standard stuff

#ifdef _IRR_WINDOWS_API_

// To build Irrlicht as a static library, you must define _IRR_STATIC_LIB_ in both the
// Irrlicht build, *and* in the user application, before #including <irrlicht.h>
#ifndef _IRR_STATIC_LIB_
#ifdef IRRLICHT_EXPORTS
#define IRRLICHT_API __declspec(dllexport)
#else
#define IRRLICHT_API __declspec(dllimport)
#endif // IRRLICHT_EXPORT
#else
#define IRRLICHT_API
#endif // _IRR_STATIC_LIB_

// Declare the calling convention.
#if defined(_STDCALL_SUPPORTED)
#define IRRCALLCONV __stdcall
#else
#define IRRCALLCONV __cdecl
#endif // STDCALL_SUPPORTED

#else // _IRR_WINDOWS_API_

// Force symbol export in shared libraries built with gcc.
#if defined(__GNUC__) && !defined(_IRR_STATIC_LIB_) && defined(IRRLICHT_EXPORTS)
#define IRRLICHT_API __attribute__ ((visibility("default")))
#else
#define IRRLICHT_API
#endif

#define IRRCALLCONV

#endif // _IRR_WINDOWS_API_

#if defined(_IRR_SOLARIS_PLATFORM_)
	#undef _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#endif

//! Define __IRR_HAS_S64 if the irr::s64 type should be enable (needs long long, available on most platforms, but not part of ISO C++ 98)
#define __IRR_HAS_S64
#ifdef NO__IRR_HAS_S64
#undef __IRR_HAS_S64
#endif

#ifndef __has_feature
  #define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif

#ifdef _DEBUG
	//! A few attributes are written in CSceneManager when _IRR_SCENEMANAGER_DEBUG is enabled
	// NOTE: Those attributes were used always until 1.8.0 and became a global define for 1.8.1
	// which is only enabled in debug because it had a large (sometimes >5%) impact on speed.
	// A better solution in the long run is to break the interface and remove _all_ attribute
	// access in functions like CSceneManager::drawAll and instead put that information in some
	// own struct/class or in CSceneManager.
	// See http://irrlicht.sourceforge.net/forum/viewtopic.php?f=2&t=48211 for the discussion.
	#define _IRR_SCENEMANAGER_DEBUG
	#ifdef NO_IRR_SCENEMANAGER_DEBUG
		#undef _IRR_SCENEMANAGER_DEBUG
	#endif
#endif

#endif // __IRR_COMPILE_CONFIG_H_INCLUDED__

