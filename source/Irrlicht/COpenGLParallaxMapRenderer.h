// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_OPENGL_PARALLAX_MAP_RENDERER_H_INCLUDED
#define IRR_C_OPENGL_PARALLAX_MAP_RENDERER_H_INCLUDED

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "IShaderConstantSetCallBack.h"

#include "COpenGLShaderMaterialRenderer.h"

namespace irr
{
namespace video
{

//! Class for rendering normal maps with OpenGL
class COpenGLParallaxMapRenderer : public COpenGLShaderMaterialRenderer, public IShaderConstantSetCallBack
{
public:

	//! Constructor
	COpenGLParallaxMapRenderer(video::COpenGLDriver* driver,
		s32& outMaterialTypeNr, E_MATERIAL_TYPE baseMaterial);

	//! Destructor
	~COpenGLParallaxMapRenderer();

	//! Called by the engine when the vertex and/or pixel shader constants for an
	//! material renderer should be set.
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData) IRR_OVERRIDE;

	//! Returns the render capability of the material.
	virtual s32 getRenderCapability() const IRR_OVERRIDE;

	virtual void OnSetMaterial(const SMaterial& material) IRR_OVERRIDE { }
	virtual void OnSetMaterial(const video::SMaterial& material,
		const video::SMaterial& lastMaterial,
		bool resetAllRenderstates, video::IMaterialRendererServices* services) IRR_OVERRIDE;

protected:

	bool CompiledShaders;
	f32 CurrentScale;
};


} // end namespace video
} // end namespace irr

#endif
#endif
