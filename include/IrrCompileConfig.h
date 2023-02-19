// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_COMPILE_CONFIG_H_INCLUDED__
#define __IRR_COMPILE_CONFIG_H_INCLUDED__

//! Identifies the IrrlichtMt fork customized for the Minetest engine
#define IRRLICHT_VERSION_MT_REVISION 9
#define IRRLICHT_VERSION_MT "mt9"

//! Irrlicht SDK Version
#define IRRLICHT_VERSION_MAJOR 1
#define IRRLICHT_VERSION_MINOR 9
#define IRRLICHT_VERSION_REVISION 0
// This flag will be defined only in SVN, the official release code will have
// it undefined
#define IRRLICHT_VERSION_SVN alpha
#define IRRLICHT_SDK_VERSION "1.9.0" IRRLICHT_VERSION_MT

#include <stdio.h> // TODO: Although included elsewhere this is required at least for mingw


//! Maximum number of texture an SMaterial can have, up to 8 are supported by Irrlicht.
#define _IRR_MATERIAL_MAX_TEXTURES_ 4


#ifdef _WIN32
#define IRRCALLCONV __stdcall
#else
#define IRRCALLCONV
#endif

#ifndef IRRLICHT_API
#define IRRLICHT_API
#endif


#define _IRR_COMPILE_WITH_GUI_
#define _IRR_COMPILE_WITH_LIBJPEG_
#define _IRR_COMPILE_WITH_LIBPNG_
#define _IRR_COMPILE_WITH_BILLBOARD_SCENENODE_
#define _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_
#define _IRR_COMPILE_WITH_B3D_LOADER_
#define _IRR_COMPILE_WITH_X_LOADER_
#define _IRR_COMPILE_WITH_OBJ_LOADER_
#define _IRR_COMPILE_WITH_BMP_LOADER_
#define _IRR_COMPILE_WITH_JPG_LOADER_
#define _IRR_COMPILE_WITH_PNG_LOADER_
#define _IRR_COMPILE_WITH_TGA_LOADER_
#define _IRR_COMPILE_WITH_JPG_WRITER_
#define _IRR_COMPILE_WITH_PNG_WRITER_
#define __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#define _IRR_COMPILE_WITH_ZLIB_
#define __IRR_HAS_S64


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
