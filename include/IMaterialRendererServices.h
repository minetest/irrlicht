// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MATERIAL_RENDERER_SERVICES_H_INCLUDED__
#define __I_MATERIAL_RENDERER_SERVICES_H_INCLUDED__

#include "SMaterial.h"
#include "S3DVertex.h"

namespace irr
{
namespace video
{

class IVideoDriver;


//! Interface providing some methods for changing advanced, internal states of a IVideoDriver.
class IMaterialRendererServices
{
public:

	//! Destructor
	virtual ~IMaterialRendererServices() {}

	//! Can be called by an IMaterialRenderer to make its work easier.
	/** Sets all basic renderstates if needed.
	Basic render states are diffuse, ambient, specular, and emissive color,
	specular power, bilinear and trilinear filtering, wireframe mode,
	grouraudshading, lighting, zbuffer, zwriteenable, backfaceculling and
	fog enabling.
	\param material The new material to be used.
	\param lastMaterial The material used until now.
	\param resetAllRenderstates Set to true if all renderstates should be
	set, regardless of their current state. */
	virtual void setBasicRenderStates(const SMaterial& material,
		const SMaterial& lastMaterial,
		bool resetAllRenderstates) = 0;

	//! Return an index constant for the vertex shader based on a name.
	virtual s32 getVertexShaderConstantID(const c8* name) = 0;

	//! Sets a constant for the vertex shader based on a name.
	/** This can be used if you used a high level shader language like GLSL
	or HLSL to create a shader. Example: If you created a shader which has
	variables named 'mWorldViewProj' (containing the WorldViewProjection
	matrix) and another one named 'fTime' containing one float, you can set
	them in your IShaderConstantSetCallBack derived class like this:
	\code
	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		f32 time = (f32)os::Timer::getTime()/100000.0f;
		services->setVertexShaderConstant("fTime", &time, 1);

		core::matrix4 worldViewProj(driver->getTransform(video::ETS_PROJECTION));
		worldViewProj *= driver->getTransform(video::ETS_VIEW);
		worldViewProj *= driver->getTransform(video::ETS_WORLD);
		services->setVertexShaderConstant("mWorldViewProj", worldViewProj.M, 16);
	}
	\endcode
	\param index Index of the variable
	\param floats Pointer to array of floats
	\param count Amount of floats in array.
	\return True if successful.
	*/
	virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count) = 0;

	//! Int interface for the above.
	virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count) = 0;

	//! Uint interface for the above.
	/* NOTE: UINT only works with GLSL, not supported for other shaders.
	         Also GLES drivers in Irrlicht do not support it as this needs at least GLES 3.0.
	*/
	virtual bool setVertexShaderConstant(s32 index, const u32* ints, int count) = 0;

	//! Sets a vertex shader constant.
	/** Can be used if you created a shader using pixel/vertex shader
	assembler or ARB_fragment_program or ARB_vertex_program.
	\param data: Data to be set in the constants
	\param startRegister: First register to be set
	\param constantAmount: Amount of registers to be set. One register consists of 4 floats. */
	virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1) = 0;

	//! Return an index constant for the pixel shader based on a name.
	virtual s32 getPixelShaderConstantID(const c8* name) = 0;

	//! Sets a constant for the pixel shader based on a name.
	/** This can be used if you used a high level shader language like GLSL
	or HLSL to create a shader. See setVertexShaderConstant() for an
	example on how to use this.
	\param index Index of the variable
	\param floats Pointer to array of floats
	\param count Amount of floats in array.
	\return True if successful. */
	virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count) = 0;

	//! Int interface for the above.
	virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count) = 0;

	//! Uint interface for the above.
	/* NOTE: UINT only works with GLSL, not supported for other shaders.
	         Also GLES drivers in Irrlicht do not support it as this needs at least GLES 3.0.
	*/
	virtual bool setPixelShaderConstant(s32 index, const u32* ints, int count) = 0;

	//! Sets a pixel shader constant.
	/** Can be used if you created a shader using pixel/vertex shader
	assembler or ARB_fragment_program or ARB_vertex_program.
	\param data Data to be set in the constants
	\param startRegister First register to be set.
	\param constantAmount Amount of registers to be set. One register consists of 4 floats. */
	virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1) = 0;

	//! \deprecated. This method may be removed by Irrlicht 2.0
	_IRR_DEPRECATED_ bool setVertexShaderConstant(const c8* name, const f32* floats, int count)
	{
		return setVertexShaderConstant(getVertexShaderConstantID(name), floats, count);
	}

	//! \deprecated. This method may be removed by Irrlicht 2.0
	_IRR_DEPRECATED_ bool setVertexShaderConstant(const c8* name, const s32* ints, int count)
	{
		return setVertexShaderConstant(getVertexShaderConstantID(name), ints, count);
	}

	//! \deprecated. This method may be removed by Irrlicht 2.0
	_IRR_DEPRECATED_ bool setPixelShaderConstant(const c8* name, const f32* floats, int count)
	{
		return setPixelShaderConstant(getPixelShaderConstantID(name), floats, count);
	}

	//! \deprecated. This method may be removed by Irrlicht 2.0
	_IRR_DEPRECATED_ bool setPixelShaderConstant(const c8* name, const s32* ints, int count)
	{
		return setPixelShaderConstant(getPixelShaderConstantID(name), ints, count);
	}

	//! Get pointer to the IVideoDriver interface
	/** \return Pointer to the IVideoDriver interface */
	virtual IVideoDriver* getVideoDriver() = 0;
};

} // end namespace video
} // end namespace irr

#endif

