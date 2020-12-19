// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_

#include "SoftwareDriver2_compile_config.h"
#include "IBurningShader.h"
#include "CSoftwareDriver2.h"
#include "IShaderConstantSetCallBack.h"

namespace irr
{
namespace video
{

const tFixPointu IBurningShader::dithermask[] =
{
	0x00,0x80,0x20,0xa0,
	0xc0,0x40,0xe0,0x60,
	0x30,0xb0,0x10,0x90,
	0xf0,0x70,0xd0,0x50
};

void IBurningShader::constructor_IBurningShader(CBurningVideoDriver* driver)
{
#ifdef _DEBUG
	setDebugName("IBurningShader");
#endif

	if (((unsigned long long)&scan & 15) || ((unsigned long long)&line & 15))
	{
		os::Printer::log("BurningVideo Shader not 16 byte aligned", ELL_ERROR);
		_IRR_DEBUG_BREAK_IF(1);
	}

	Interlaced.enable = 0;
	Interlaced.bypass = 1;
	Interlaced.nr = 0;

	EdgeTestPass = edge_test_pass;
	EdgeTestPass_stack = edge_test_pass;

	for (u32 i = 0; i < BURNING_MATERIAL_MAX_TEXTURES; ++i)
	{
		IT[i].Texture = 0;
	}

	Driver = driver;
	CallBack = 0;

	RenderTarget = 0;
	ColorMask = COLOR_BRIGHT_WHITE;
	DepthBuffer = (CDepthBuffer*)driver->getDepthBuffer();
	if (DepthBuffer)
		DepthBuffer->grab();

	Stencil = (CStencilBuffer*)driver->getStencilBuffer();
	if (Stencil)
		Stencil->grab();

	stencilOp[0] = StencilOp_KEEP;
	stencilOp[1] = StencilOp_KEEP;
	stencilOp[2] = StencilOp_KEEP;
	AlphaRef = 0;
	RenderPass_ShaderIsTransparent = 0;
	PrimitiveColor = COLOR_BRIGHT_WHITE;
	TL_Flag = 0;
}

IBurningShader::IBurningShader(CBurningVideoDriver* driver)
{
	constructor_IBurningShader(driver);
}

//! Constructor
IBurningShader::IBurningShader(
	CBurningVideoDriver* driver,
	s32& outMaterialTypeNr,
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	const c8* geometryShaderProgram,
	const c8* geometryShaderEntryPointName,
	E_GEOMETRY_SHADER_TYPE gsCompileTarget,
	scene::E_PRIMITIVE_TYPE inType,
	scene::E_PRIMITIVE_TYPE outType,
	u32 verticesOut,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData)
{
	constructor_IBurningShader(driver);
	BaseMaterial = baseMaterial;
	CallBack = callback;
	if (CallBack)
		CallBack->grab();

	// register myself as new material
	outMaterialTypeNr = Driver->addMaterialRenderer(this);
}


//! destructor
IBurningShader::~IBurningShader()
{
	if (RenderTarget)
		RenderTarget->drop();

	if (DepthBuffer)
		DepthBuffer->drop();

	if (Stencil)
		Stencil->drop();

	for (u32 i = 0; i != BURNING_MATERIAL_MAX_TEXTURES; ++i)
	{
		if (IT[i].Texture)
			IT[i].Texture->drop();
	}

	if (CallBack)
		CallBack->drop();

}

//! sets a render target
void IBurningShader::setRenderTarget(video::IImage* surface, const core::rect<s32>& viewPort, const interlaced_control interlaced)
{
	Interlaced = interlaced;

	if (RenderTarget)
		RenderTarget->drop();

	RenderTarget = (video::CImage*) surface;

	if (RenderTarget)
	{
		RenderTarget->grab();

		//(fp24*) DepthBuffer->lock() = DepthBuffer->lock();
	}
}


//! sets the Texture
void IBurningShader::setTextureParam(const size_t stage, video::CSoftwareTexture2* texture, s32 lodFactor)
{
	sInternalTexture* it = &IT[stage];

	if (it->Texture)
		it->Texture->drop();

	it->Texture = texture;

	if (it->Texture)
	{
		it->Texture->grab();

		// select mignify and magnify
		it->lodFactor = lodFactor;

		//only mipmap chain (means positive lodFactor)
		u32 existing_level = it->Texture->getMipmapLevel(lodFactor);
#if !defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
		it->data = (tVideoSample*)it->Texture->lock(ETLM_READ_ONLY, existing_level, 0);
#else
		it->data = (tVideoSample*)it->Texture->lock(ETLM_READ_ONLY, existing_level);
#endif

		// prepare for optimal fixpoint
		it->pitchlog2 = s32_log2_s32(it->Texture->getPitch());

		const core::dimension2d<u32>& dim = it->Texture->getSize();
		it->textureXMask = s32_to_fixPoint(dim.Width - 1) & FIX_POINT_UNSIGNED_MASK;
		it->textureYMask = s32_to_fixPoint(dim.Height - 1) & FIX_POINT_UNSIGNED_MASK;
	}
}

//emulate a line with degenerate triangle and special shader mode (not perfect...)
void IBurningShader::drawLine(const s4DVertex* a, const s4DVertex* b)
{
	sVec2 d;
	d.x = b->Pos.x - a->Pos.x;	d.x *= d.x;
	d.y = b->Pos.y - a->Pos.y;	d.y *= d.y;
	//if ( d.x * d.y < 0.001f ) return;

	if (a->Pos.x > b->Pos.x) swapVertexPointer(&a, &b);

	s4DVertex c = *a;

	const f32 w = (f32)RenderTarget->getDimension().Width - 1;
	const f32 h = (f32)RenderTarget->getDimension().Height - 1;

	if (d.x < 2.f) { c.Pos.x = b->Pos.x + 1.f + d.y; if (c.Pos.x > w) c.Pos.x = w; }
	else c.Pos.x = b->Pos.x;
	if (d.y < 2.f) { c.Pos.y = b->Pos.y + 1.f; if (c.Pos.y > h) c.Pos.y = h; EdgeTestPass |= edge_test_first_line; }

	drawTriangle(a, b, &c);
	EdgeTestPass &= ~edge_test_first_line;

}

void IBurningShader::drawPoint(const s4DVertex* a)
{
}

void IBurningShader::drawWireFrameTriangle(const s4DVertex* a, const s4DVertex* b, const s4DVertex* c)
{
	if (EdgeTestPass & edge_test_pass) drawTriangle(a, b, c);
	else if (EdgeTestPass & edge_test_point)
	{
		drawPoint(a);
		drawPoint(b);
		drawPoint(c);
	}
	else
	{
		drawLine(a, b);
		drawLine(b, c);
		drawLine(a, c);
	}
}


void IBurningShader::OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
	bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (Driver)
		Driver->setFallback_Material(BaseMaterial);
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
	if (CallBack)
		CallBack->OnSetMaterial(material);

}

void IBurningShader::OnUnsetMaterial()
{
}

bool IBurningShader::OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
{
	// call callback to set shader constants
	if (CallBack)
		CallBack->OnSetConstants(this, UserData);
	return true;
}


//! Returns if the material is transparent.
bool IBurningShader::isTransparent() const
{
	return RenderPass_ShaderIsTransparent != 0;
}

//! Access the callback provided by the users when creating shader materials
IShaderConstantSetCallBack* IBurningShader::getShaderConstantSetCallBack() const
{
	return CallBack;
}

// implementations for the render services
void IBurningShader::setBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates)
{
	// forward
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

s32 IBurningShader::getShaderConstantID(EBurningUniformFlags flags, const c8* name)
{
	if (!name || !name[0])
		return -1;

	BurningUniform add;
	tiny_strcpy(add.name, name);
	add.type = flags;

	s32 index = UniformInfo.linear_search(add);
	if (index < 0)
	{
		UniformInfo.push_back(add);
		index = UniformInfo.size() - 1;
	}

	return index;
}

const char* tiny_itoa(s32 value, int base)
{
	static char b[32];
	int p = 31;

	//int sign = 0;
	//if (value < 0) { sign = 1; value = -value; }

	b[p] = '\0';
	do {
		b[--p] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[value % base];
		value /= base;
	} while (value && p > 0);

	//if (sign && p > 0) { b[--p] = '-'; }

	return b + p;
}

bool IBurningShader::setShaderConstantID(EBurningUniformFlags flags, s32 index, const void* data, size_t u32_count)
{
	if ((u32)index >= UniformInfo.size())
		return false;
#if 0
	BurningUniform add;
	while ((u32)index >= UniformInfo.size())
	{
		tiny_strcpy(add.name, tiny_itoa(UniformInfo.size(), 10));
		add.type = flags;
		UniformInfo.push_back(add);
	}
#endif

	BurningUniform& use = UniformInfo[index];
	use.type = flags;

	const u32* s = (u32*)data;
	u32* d = (u32*)use.data;

	if (!s) u32_count = 0;
	if (u32_count > array_size(use.data)) u32_count = array_size(use.data);
	for (size_t i = 0; i < u32_count; ++i)
	{
		d[i] = s[i];
	}

	return true;
}


s32 IBurningShader::getVertexShaderConstantID(const c8* name)
{
	return getShaderConstantID(BL_VERTEX_PROGRAM, name);
}

s32 IBurningShader::getPixelShaderConstantID(const c8* name)
{
	return getShaderConstantID(BL_FRAGMENT_PROGRAM, name);
}

void IBurningShader::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	c8 name[BL_ACTIVE_UNIFORM_MAX_LENGTH];
	tiny_strcpy(name, tiny_itoa(startRegister, 10));

	setShaderConstantID(BL_VERTEX_FLOAT, getShaderConstantID(BL_VERTEX_PROGRAM, name), data, constantAmount);
}

void IBurningShader::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	c8 name[BL_ACTIVE_UNIFORM_MAX_LENGTH];
	tiny_strcpy(name, tiny_itoa(startRegister, 10));

	setShaderConstantID(BL_FRAGMENT_FLOAT, getShaderConstantID(BL_FRAGMENT_PROGRAM, name), data, constantAmount);
}

bool IBurningShader::setVertexShaderConstant(s32 index, const f32* floats, int count)
{
	return setShaderConstantID(BL_VERTEX_FLOAT, index, floats, count);
}

bool IBurningShader::setVertexShaderConstant(s32 index, const s32* ints, int count)
{
	return setShaderConstantID(BL_VERTEX_INT, index, ints, count);
}

bool IBurningShader::setVertexShaderConstant(s32 index, const u32* ints, int count)
{
	return setShaderConstantID(BL_VERTEX_UINT, index, ints, count);
}

bool IBurningShader::setPixelShaderConstant(s32 index, const f32* floats, int count)
{
	return setShaderConstantID(BL_FRAGMENT_FLOAT, index, floats, count);
}

bool IBurningShader::setPixelShaderConstant(s32 index, const s32* ints, int count)
{
	return setShaderConstantID(BL_FRAGMENT_INT, index, ints, count);
}

bool IBurningShader::setPixelShaderConstant(s32 index, const u32* ints, int count)
{
	return setShaderConstantID(BL_FRAGMENT_UINT, index, ints, count);
}

void IBurningShader::setStencilOp(eBurningStencilOp sfail, eBurningStencilOp dpfail, eBurningStencilOp dppass)
{
	stencilOp[0] = sfail;
	stencilOp[1] = dpfail;
	stencilOp[2] = dppass;
}


IVideoDriver* IBurningShader::getVideoDriver()
{
	return Driver;
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_
