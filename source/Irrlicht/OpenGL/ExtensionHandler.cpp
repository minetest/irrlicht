// Copyright (C) 2015 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// 2017 modified by Michael Zeilfelder (unifying extension handlers)
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "ExtensionHandler.h"

#include "irrString.h"
#include "SMaterial.h"
#include "fast_atof.h"
#include <mt_opengl.h>

namespace irr
{
namespace video
{
	void COpenGL3ExtensionHandler::initExtensionsOld()
	{
		auto extensions_string = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
		const char *pos = extensions_string;
		while (const char *next = strchr(pos, ' ')) {
			std::string name{pos, next};
			addExtension(name.c_str());
			pos = next + 1;
		}
		addExtension(pos);
	}

	void COpenGL3ExtensionHandler::initExtensionsNew()
	{
		GLint ext_count = 0;
		GL.GetIntegerv(GL_NUM_EXTENSIONS, &ext_count);
		for (int k = 0; k < ext_count; k++)
			addExtension(reinterpret_cast<const char *>(GL.GetStringi(GL_EXTENSIONS, k)));
	}

	void COpenGL3ExtensionHandler::addExtension(const char *name) {
		for (size_t j=0; j<IRR_OGLES_Feature_Count; ++j) {
			if (!strcmp(getFeatureString(j), name)) {
				FeatureAvailable[j] = true;
				break;
			}
		}
	}
} // end namespace video
} // end namespace irr
