// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_SOFTWARE2_MATERIAL_RENDERER_H_INCLUDED
#define IRR_C_SOFTWARE2_MATERIAL_RENDERER_H_INCLUDED

#include "SoftwareDriver2_compile_config.h"

#include "IMaterialRenderer.h"
#include "CSoftwareDriver2.h"

namespace irr
{
namespace video
{

//! Base class for all internal Software2 material renderers
class CSoftware2MaterialRenderer : public IMaterialRenderer
{
public:

	//! Constructor
	CSoftware2MaterialRenderer(video::CBurningVideoDriver* driver)
		: Driver(driver)
	{
	}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services) IRR_OVERRIDE
	{
		if (Driver)
			Driver->setFallback_Material(material.MaterialType, BVT_Fix);
	}

protected:

	video::CBurningVideoDriver* Driver;
};

//! solid material renderer
class CSoftware2MaterialRenderer_SOLID : public CSoftware2MaterialRenderer
{
public:
	CSoftware2MaterialRenderer_SOLID ( video::CBurningVideoDriver* driver )
		:CSoftware2MaterialRenderer ( driver ) {}

	//! Returns if the material is transparent.
	virtual bool isTransparent() const IRR_OVERRIDE
	{
		return false;
	}

};


//! Transparent material renderer
class CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR : public CSoftware2MaterialRenderer
{
public:
	CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR ( video::CBurningVideoDriver* driver )
		: CSoftware2MaterialRenderer ( driver ) {}


	//! Returns if the material is transparent.
	virtual bool isTransparent() const IRR_OVERRIDE
	{
		return true;
	}

};

//! unsupported material renderer
class CSoftware2MaterialRenderer_UNSUPPORTED : public CSoftware2MaterialRenderer
{
public:
	CSoftware2MaterialRenderer_UNSUPPORTED ( video::CBurningVideoDriver* driver )
		: CSoftware2MaterialRenderer ( driver ) {}

	virtual s32 getRenderCapability() const IRR_OVERRIDE { return 1; }

};



} // end namespace video
} // end namespace irr

#endif
