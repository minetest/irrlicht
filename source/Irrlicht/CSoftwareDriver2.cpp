// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "CSoftwareDriver2.h"

#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_

#include "SoftwareDriver2_helper.h"
#include "CSoftwareTexture.h"
#include "CSoftwareTexture2.h"
#include "CSoftware2MaterialRenderer.h"
#include "S3DVertex.h"
#include "S4DVertex.h"
#include "CBlit.h"


// Matrix now here

template <class T>
bool mat44_transposed_inverse(irr::core::CMatrix4<T>& out, const irr::core::CMatrix4<T>& M)
{
	const T* burning_restrict m = M.pointer();

	double d =
		(m[0] * m[5] - m[1] * m[4]) * (m[10] * m[15] - m[11] * m[14]) -
		(m[0] * m[6] - m[2] * m[4]) * (m[9] * m[15] - m[11] * m[13]) +
		(m[0] * m[7] - m[3] * m[4]) * (m[9] * m[14] - m[10] * m[13]) +
		(m[1] * m[6] - m[2] * m[5]) * (m[8] * m[15] - m[11] * m[12]) -
		(m[1] * m[7] - m[3] * m[5]) * (m[8] * m[14] - m[10] * m[12]) +
		(m[2] * m[7] - m[3] * m[6]) * (m[8] * m[13] - m[9] * m[12]);

	if (fabs(d) < DBL_MIN)
	{
		out.makeIdentity();
		return false;
	}

	d = 1.0 / d;
	T* burning_restrict o = out.pointer();
	o[0] = (T)(d*(m[5] * (m[10] * m[15] - m[11] * m[14]) + m[6] * (m[11] * m[13] - m[9] * m[15]) + m[7] * (m[9] * m[14] - m[10] * m[13])));
	o[4] = (T)(d*(m[9] * (m[2] * m[15] - m[3] * m[14]) + m[10] * (m[3] * m[13] - m[1] * m[15]) + m[11] * (m[1] * m[14] - m[2] * m[13])));
	o[8] = (T)(d*(m[13] * (m[2] * m[7] - m[3] * m[6]) + m[14] * (m[3] * m[5] - m[1] * m[7]) + m[15] * (m[1] * m[6] - m[2] * m[5])));
	o[12] = (T)(d*(m[1] * (m[7] * m[10] - m[6] * m[11]) + m[2] * (m[5] * m[11] - m[7] * m[9]) + m[3] * (m[6] * m[9] - m[5] * m[10])));

	o[1] = (T)(d*(m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[10] * m[12] - m[8] * m[14]) + m[4] * (m[11] * m[14] - m[10] * m[15])));
	o[5] = (T)(d*(m[10] * (m[0] * m[15] - m[3] * m[12]) + m[11] * (m[2] * m[12] - m[0] * m[14]) + m[8] * (m[3] * m[14] - m[2] * m[15])));
	o[9] = (T)(d*(m[14] * (m[0] * m[7] - m[3] * m[4]) + m[15] * (m[2] * m[4] - m[0] * m[6]) + m[12] * (m[3] * m[6] - m[2] * m[7])));
	o[13] = (T)(d*(m[2] * (m[7] * m[8] - m[4] * m[11]) + m[3] * (m[4] * m[10] - m[6] * m[8]) + m[0] * (m[6] * m[11] - m[7] * m[10])));

	o[2] = (T)(d*(m[7] * (m[8] * m[13] - m[9] * m[12]) + m[4] * (m[9] * m[15] - m[11] * m[13]) + m[5] * (m[11] * m[12] - m[8] * m[15])));
	o[6] = (T)(d*(m[11] * (m[0] * m[13] - m[1] * m[12]) + m[8] * (m[1] * m[15] - m[3] * m[13]) + m[9] * (m[3] * m[12] - m[0] * m[15])));
	o[10] = (T)(d*(m[15] * (m[0] * m[5] - m[1] * m[4]) + m[12] * (m[1] * m[7] - m[3] * m[5]) + m[13] * (m[3] * m[4] - m[0] * m[7])));
	o[14] = (T)(d*(m[3] * (m[5] * m[8] - m[4] * m[9]) + m[0] * (m[7] * m[9] - m[5] * m[11]) + m[1] * (m[4] * m[11] - m[7] * m[8])));

	o[3] = (T)(d*(m[4] * (m[10] * m[13] - m[9] * m[14]) + m[5] * (m[8] * m[14] - m[10] * m[12]) + m[6] * (m[9] * m[12] - m[8] * m[13])));
	o[7] = (T)(d*(m[8] * (m[2] * m[13] - m[1] * m[14]) + m[9] * (m[0] * m[14] - m[2] * m[12]) + m[10] * (m[1] * m[12] - m[0] * m[13])));
	o[11] = (T)(d*(m[12] * (m[2] * m[5] - m[1] * m[6]) + m[13] * (m[0] * m[6] - m[2] * m[4]) + m[14] * (m[1] * m[4] - m[0] * m[5])));
	o[15] = (T)(d*(m[0] * (m[5] * m[10] - m[6] * m[9]) + m[1] * (m[6] * m[8] - m[4] * m[10]) + m[2] * (m[4] * m[9] - m[5] * m[8])));

	return true;
}

#if 0
// difference to CMatrix4<T>::getInverse . higher precision in determinant. return identity on failure
template <class T>
bool mat44_inverse(CMatrix4<T>& out, const CMatrix4<T>& M)
{
	const T* m = M.pointer();

	double d =
		(m[0] * m[5] - m[1] * m[4]) * (m[10] * m[15] - m[11] * m[14]) -
		(m[0] * m[6] - m[2] * m[4]) * (m[9] * m[15] - m[11] * m[13]) +
		(m[0] * m[7] - m[3] * m[4]) * (m[9] * m[14] - m[10] * m[13]) +
		(m[1] * m[6] - m[2] * m[5]) * (m[8] * m[15] - m[11] * m[12]) -
		(m[1] * m[7] - m[3] * m[5]) * (m[8] * m[14] - m[10] * m[12]) +
		(m[2] * m[7] - m[3] * m[6]) * (m[8] * m[13] - m[9] * m[12]);

	if (fabs(d) < DBL_MIN)
	{
		out.makeIdentity();
		return false;
	}

	d = 1.0 / d;
	T* o = out.pointer();
	o[0] = (T)(d*(m[5] * (m[10] * m[15] - m[11] * m[14]) + m[6] * (m[11] * m[13] - m[9] * m[15]) + m[7] * (m[9] * m[14] - m[10] * m[13])));
	o[1] = (T)(d*(m[9] * (m[2] * m[15] - m[3] * m[14]) + m[10] * (m[3] * m[13] - m[1] * m[15]) + m[11] * (m[1] * m[14] - m[2] * m[13])));
	o[2] = (T)(d*(m[13] * (m[2] * m[7] - m[3] * m[6]) + m[14] * (m[3] * m[5] - m[1] * m[7]) + m[15] * (m[1] * m[6] - m[2] * m[5])));
	o[3] = (T)(d*(m[1] * (m[7] * m[10] - m[6] * m[11]) + m[2] * (m[5] * m[11] - m[7] * m[9]) + m[3] * (m[6] * m[9] - m[5] * m[10])));

	o[4] = (T)(d*(m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[10] * m[12] - m[8] * m[14]) + m[4] * (m[11] * m[14] - m[10] * m[15])));
	o[5] = (T)(d*(m[10] * (m[0] * m[15] - m[3] * m[12]) + m[11] * (m[2] * m[12] - m[0] * m[14]) + m[8] * (m[3] * m[14] - m[2] * m[15])));
	o[6] = (T)(d*(m[14] * (m[0] * m[7] - m[3] * m[4]) + m[15] * (m[2] * m[4] - m[0] * m[6]) + m[12] * (m[3] * m[6] - m[2] * m[7])));
	o[7] = (T)(d*(m[2] * (m[7] * m[8] - m[4] * m[11]) + m[3] * (m[4] * m[10] - m[6] * m[8]) + m[0] * (m[6] * m[11] - m[7] * m[10])));

	o[8] = (T)(d*(m[7] * (m[8] * m[13] - m[9] * m[12]) + m[4] * (m[9] * m[15] - m[11] * m[13]) + m[5] * (m[11] * m[12] - m[8] * m[15])));
	o[9] = (T)(d*(m[11] * (m[0] * m[13] - m[1] * m[12]) + m[8] * (m[1] * m[15] - m[3] * m[13]) + m[9] * (m[3] * m[12] - m[0] * m[15])));
	o[10] = (T)(d*(m[15] * (m[0] * m[5] - m[1] * m[4]) + m[12] * (m[1] * m[7] - m[3] * m[5]) + m[13] * (m[3] * m[4] - m[0] * m[7])));
	o[11] = (T)(d*(m[3] * (m[5] * m[8] - m[4] * m[9]) + m[0] * (m[7] * m[9] - m[5] * m[11]) + m[1] * (m[4] * m[11] - m[7] * m[8])));

	o[12] = (T)(d*(m[4] * (m[10] * m[13] - m[9] * m[14]) + m[5] * (m[8] * m[14] - m[10] * m[12]) + m[6] * (m[9] * m[12] - m[8] * m[13])));
	o[13] = (T)(d*(m[8] * (m[2] * m[13] - m[1] * m[14]) + m[9] * (m[0] * m[14] - m[2] * m[12]) + m[10] * (m[1] * m[12] - m[0] * m[13])));
	o[14] = (T)(d*(m[12] * (m[2] * m[5] - m[1] * m[6]) + m[13] * (m[0] * m[6] - m[2] * m[4]) + m[14] * (m[1] * m[4] - m[0] * m[5])));
	o[15] = (T)(d*(m[0] * (m[5] * m[10] - m[6] * m[9]) + m[1] * (m[6] * m[8] - m[4] * m[10]) + m[2] * (m[4] * m[9] - m[5] * m[8])));

	return true;
}
#endif

// void CMatrix4<T>::transformVec4(T *out, const T * in) const
template <class T>
inline void transformVec4Vec4(const irr::core::CMatrix4<T>& m, T* burning_restrict out, const T* burning_restrict in)
{
	const T* burning_restrict M = m.pointer();

	out[0] = in[0] * M[0] + in[1] * M[4] + in[2] * M[8]  + in[3] * M[12];
	out[1] = in[0] * M[1] + in[1] * M[5] + in[2] * M[9]  + in[3] * M[13];
	out[2] = in[0] * M[2] + in[1] * M[6] + in[2] * M[10] + in[3] * M[14];
	out[3] = in[0] * M[3] + in[1] * M[7] + in[2] * M[11] + in[3] * M[15];
}

#if 0
// void CMatrix4<T>::transformVect(T *out, const core::vector3df &in) const
template <class T>
inline void transformVec3Vec4(const irr::core::CMatrix4<T>& m,T* burning_restrict out, const core::vector3df &in)
{
	const T* burning_restrict M = m.pointer();
	out[0] = in.X*M[0] + in.Y*M[4] + in.Z*M[8]  + M[12];
	out[1] = in.X*M[1] + in.Y*M[5] + in.Z*M[9]  + M[13];
	out[2] = in.X*M[2] + in.Y*M[6] + in.Z*M[10] + M[14];
	out[3] = in.X*M[3] + in.Y*M[7] + in.Z*M[11] + M[15];
}
#endif

template <class T>
inline void rotateVec3Vec4(const irr::core::CMatrix4<T>& m, T* burning_restrict out, const T* burning_restrict in)
{
	const T* burning_restrict M = m.pointer();

	out[0] = in[0] * M[0] + in[1] * M[4] + in[2] * M[8];
	out[1] = in[0] * M[1] + in[1] * M[5] + in[2] * M[9];
	out[2] = in[0] * M[2] + in[1] * M[6] + in[2] * M[10];
	out[3] = 0.f;
}

//https://github.com/ekmett/approximate/blob/master/cbits/fast.c powf_fast_precise
static inline float powf_limit(const float a, const float b)
{
	if (a <= 0.0000001f) return 0.f;
	else if (a >= 1.f) return 1.f;

	/* calculate approximation with fraction of the exponent */
	int e = (int)b;
	union { float f; int x; } u = { a };
	u.x = (int)((b - e) * (u.x - 1065353216) + 1065353216);

	float r = 1.0f;
	float ua = a;
	while (e) {
		if (e & 1) {
			r *= ua;
		}
		if (ua < 0.00000001f)
			break;
		ua *= ua;
		e >>= 1;
	}

	r *= u.f;
	return r;
}


#if defined(Tweak_Burning)

// run time parameter
struct tweakBurning
{
	tweakBurning()
	{
		current = 11;
		step = 0.0001f;

		ndc_shrink_x = -0.75f;
		ndc_scale_x = 0.5f;
		ndc_trans_x = -0.5f;

		ndc_shrink_y = -0.75f;
		ndc_scale_y = -0.5f;
		ndc_trans_y = -0.5f;

		tex_w_add = 0.f;
		tex_h_add = 0.f;
		tex_cx_add = 0.f;
		tex_cy_add = 0.f;

		AreaMinDrawSize = 0.001f;
	}
	int current;

	union
	{
		struct {
			f32 step;

			f32 ndc_shrink_x;
			f32 ndc_scale_x;
			f32 ndc_trans_x;

			f32 ndc_shrink_y;
			f32 ndc_scale_y;
			f32 ndc_trans_y;

			f32 tex_w_add;
			f32 tex_cx_add;
			f32 tex_h_add;
			f32 tex_cy_add;

			f32 AreaMinDrawSize; //! minimal visible covered area for primitive
		};
		f32 val[16];
	};
	static const char* const name[16];
	void postEventFromUser(const SEvent& e);
};

const char* const tweakBurning::name[] = { "step",
	"ndc_shrink_x","ndc_scale_x","ndc_trans_x",
	"ndc_shrink_y","ndc_scale_y","ndc_trans_y",
	"tex_w_add","tex_cx_add","tex_h_add","tex_cy_add",
	"dc_area",0 };

void tweakBurning::postEventFromUser(const SEvent& e)
{
	int show = 0;
	if (e.EventType == EET_KEY_INPUT_EVENT)
	{
		switch (e.KeyInput.Key)
		{
		case KEY_KEY_1: step *= 0.9f; if (step < 0.00001f) step = 0.0001f; show = 2; break;
		case KEY_KEY_2: step *= 1.1f; show = 2; break;

		case KEY_KEY_3: if (!e.KeyInput.PressedDown) { current -= 1; if (current < 1) current = 11; show = 1; } break;
		case KEY_KEY_4: if (!e.KeyInput.PressedDown) { current += 1; if (current > 11) current = 1; show = 1; } break;

		case KEY_KEY_5: val[current] -= e.KeyInput.Shift ? step * 100.f : step; show = 1; break;
		case KEY_KEY_6: val[current] += e.KeyInput.Shift ? step * 100.f : step; show = 1; break;
		default:
			break;
		}
	}
	if (show)
	{
		if (step < 0.0001f) step = 0.0001f;
		char buf[256];
		if (show == 2) sprintf(buf, "%s %f\n", name[0], val[0]);
		else sprintf(buf, "%s %f\n", name[current], val[current]);
		os::Printer::print(buf);
	}
}

void CBurningVideoDriver::postEventFromUser(const void* sevent)
{
	if (sevent) Tweak.postEventFromUser(*(const SEvent*)sevent);
}

tweakBurning Tweak;
#endif //defined(Tweak_Burning)



namespace irr
{
namespace video
{

//! constructor
CBurningVideoDriver::CBurningVideoDriver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, video::IImagePresenter* presenter)
: CNullDriver(io, params.WindowSize), BackBuffer(0), Presenter(presenter),
	WindowId(0), SceneSourceRect(0),
	RenderTargetTexture(0), RenderTargetSurface(0), CurrentShader(0),
	DepthBuffer(0), StencilBuffer ( 0 )
{
	//enable fpu exception
	//unsigned int fp_control_state = _controlfp(_EM_INEXACT, _MCW_EM);

	#ifdef _DEBUG
	setDebugName("CBurningVideoDriver");
	#endif

	VertexCache_map_source_format();

	// create backbuffer
	BackBuffer = new CImage(BURNINGSHADER_COLOR_FORMAT, params.WindowSize);
	if (BackBuffer)
	{
		BackBuffer->fill(SColor(0));

		// create z buffer
		if ( params.ZBufferBits )
			DepthBuffer = video::createDepthBuffer(BackBuffer->getDimension());

		// create stencil buffer
		if ( params.Stencilbuffer )
			StencilBuffer = video::createStencilBuffer(BackBuffer->getDimension(),8);
	}

	DriverAttributes->setAttribute("MaxIndices", 1<<16);
	DriverAttributes->setAttribute("MaxTextures", BURNING_MATERIAL_MAX_TEXTURES);
	DriverAttributes->setAttribute("MaxTextureSize", SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE ? SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE : 1<<20);
	DriverAttributes->setAttribute("MaxLights", 1024 ); //glsl::gl_MaxLights);
	DriverAttributes->setAttribute("MaxTextureLODBias", 16.f);
	DriverAttributes->setAttribute("Version", 50);

	// create triangle renderers

	memset( BurningShader, 0, sizeof ( BurningShader ) );
	//BurningShader[ETR_FLAT] = createTRFlat2(DepthBuffer);
	//BurningShader[ETR_FLAT_WIRE] = createTRFlatWire2(DepthBuffer);
	BurningShader[ETR_GOURAUD] = createTriangleRendererGouraud2(this);
	BurningShader[ETR_GOURAUD_NOZ] = createTriangleRendererGouraudNoZ2(this);
	//BurningShader[ETR_GOURAUD_ALPHA] = createTriangleRendererGouraudAlpha2(this );
	BurningShader[ETR_GOURAUD_ALPHA_NOZ] = createTRGouraudAlphaNoZ2(this ); // 2D
	//BurningShader[ETR_GOURAUD_WIRE] = createTriangleRendererGouraudWire2(DepthBuffer);
	//BurningShader[ETR_TEXTURE_FLAT] = createTriangleRendererTextureFlat2(DepthBuffer);
	//BurningShader[ETR_TEXTURE_FLAT_WIRE] = createTriangleRendererTextureFlatWire2(DepthBuffer);
	BurningShader[ETR_TEXTURE_GOURAUD] = createTriangleRendererTextureGouraud2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_LIGHTMAP_M1] = createTriangleRendererTextureLightMap2_M1(this);
	BurningShader[ETR_TEXTURE_GOURAUD_LIGHTMAP_M2] = createTriangleRendererTextureLightMap2_M2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_LIGHTMAP_M4] = createTriangleRendererGTextureLightMap2_M4(this);
	BurningShader[ETR_TEXTURE_LIGHTMAP_M4] = createTriangleRendererTextureLightMap2_M4(this);
	BurningShader[ETR_TEXTURE_GOURAUD_LIGHTMAP_ADD] = createTriangleRendererTextureLightMap2_Add(this);
	BurningShader[ETR_TEXTURE_GOURAUD_DETAIL_MAP] = createTriangleRendererTextureDetailMap2(this);

	BurningShader[ETR_TEXTURE_GOURAUD_WIRE] = createTriangleRendererTextureGouraudWire2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_NOZ] = createTRTextureGouraudNoZ2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_ADD] = createTRTextureGouraudAdd2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_ADD_NO_Z] = createTRTextureGouraudAddNoZ2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_VERTEX_ALPHA] = createTriangleRendererTextureVertexAlpha2 ( this );

	BurningShader[ETR_TEXTURE_GOURAUD_ALPHA] = createTRTextureGouraudAlpha(this );
	BurningShader[ETR_TEXTURE_GOURAUD_ALPHA_NOZ] = createTRTextureGouraudAlphaNoZ( this );

	BurningShader[ETR_NORMAL_MAP_SOLID] = createTRNormalMap ( this );
	BurningShader[ETR_STENCIL_SHADOW] = createTRStencilShadow ( this );
	BurningShader[ETR_TEXTURE_BLEND] = createTRTextureBlend( this );

	BurningShader[ETR_TRANSPARENT_REFLECTION_2_LAYER] = createTriangleRendererTexture_transparent_reflection_2_layer(this);
	//BurningShader[ETR_REFERENCE] = createTriangleRendererReference ( this );

	BurningShader[ETR_COLOR] = create_burning_shader_color(this);

	// add the same renderer for all solid types
	CSoftware2MaterialRenderer_SOLID* smr = new CSoftware2MaterialRenderer_SOLID( this);
	CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR* tmr = new CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR( this);
	//CSoftware2MaterialRenderer_UNSUPPORTED * umr = new CSoftware2MaterialRenderer_UNSUPPORTED ( this );

	//!TODO: addMaterialRenderer depends on pushing order....
	addMaterialRenderer ( smr ); // EMT_SOLID
	addMaterialRenderer ( smr ); // EMT_SOLID_2_LAYER,
	addMaterialRenderer ( smr ); // EMT_LIGHTMAP,
	addMaterialRenderer ( tmr ); // EMT_LIGHTMAP_ADD,
	addMaterialRenderer ( smr ); // EMT_LIGHTMAP_M2,
	addMaterialRenderer ( smr ); // EMT_LIGHTMAP_M4,
	addMaterialRenderer ( smr ); // EMT_LIGHTMAP_LIGHTING,
	addMaterialRenderer ( smr ); // EMT_LIGHTMAP_LIGHTING_M2,
	addMaterialRenderer ( smr ); // EMT_LIGHTMAP_LIGHTING_M4,
	addMaterialRenderer ( smr ); // EMT_DETAIL_MAP,
	addMaterialRenderer ( smr ); // EMT_SPHERE_MAP,
	addMaterialRenderer ( smr ); // EMT_REFLECTION_2_LAYER,
	addMaterialRenderer ( tmr ); // EMT_TRANSPARENT_ADD_COLOR,
	addMaterialRenderer ( tmr ); // EMT_TRANSPARENT_ALPHA_CHANNEL,
	addMaterialRenderer ( tmr ); // EMT_TRANSPARENT_ALPHA_CHANNEL_REF,
	addMaterialRenderer ( tmr ); // EMT_TRANSPARENT_VERTEX_ALPHA,
	addMaterialRenderer ( smr ); // EMT_TRANSPARENT_REFLECTION_2_LAYER,
	addMaterialRenderer ( smr ); // EMT_NORMAL_MAP_SOLID,
	addMaterialRenderer ( tmr ); // EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR,
	addMaterialRenderer ( tmr ); // EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA,
	addMaterialRenderer ( smr ); // EMT_PARALLAX_MAP_SOLID,
	addMaterialRenderer ( tmr ); // EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR,
	addMaterialRenderer ( tmr ); // EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA,
	addMaterialRenderer ( tmr ); // EMT_ONETEXTURE_BLEND

	smr->drop ();
	tmr->drop ();
	//umr->drop ();

	// select render target
	setRenderTargetImage(BackBuffer);

	//reset Lightspace
	EyeSpace.reset();
	EyeSpace.resetFog();

	// select the right renderer
	setMaterial(Material.org);
}


//! destructor
CBurningVideoDriver::~CBurningVideoDriver()
{
	// delete Backbuffer
	if (BackBuffer)
	{
		BackBuffer->drop();
		BackBuffer = 0;
	}

	// delete triangle renderers

	for (s32 i=0; i<ETR2_COUNT; ++i)
	{
		if (BurningShader[i])
		{
			BurningShader[i]->drop();
			BurningShader[i] = 0;
		}
	}

	// delete Additional buffer
	if (StencilBuffer)
	{
		StencilBuffer->drop();
		StencilBuffer = 0;
	}

	if (DepthBuffer)
	{
		DepthBuffer->drop();
		DepthBuffer = 0;
	}

	if (RenderTargetTexture)
	{
		RenderTargetTexture->drop();
		RenderTargetTexture = 0;
	}

	if (RenderTargetSurface)
	{
		RenderTargetSurface->drop();
		RenderTargetSurface = 0;
	}
}



//! queries the features of the driver, returns true if feature is available
bool CBurningVideoDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
{
	int on = 0;
	switch (feature)
	{
#ifdef SOFTWARE_DRIVER_2_BILINEAR
	case EVDF_BILINEAR_FILTER:
		on = 1;
		break;
#endif
#ifdef SOFTWARE_DRIVER_2_MIPMAPPING
	case EVDF_MIP_MAP:
		on = 1;
		break;
#endif
	case EVDF_STENCIL_BUFFER:
		on = StencilBuffer != 0;
		break;

	case EVDF_RENDER_TO_TARGET:
	case EVDF_MULTITEXTURE:
	case EVDF_HARDWARE_TL:
	case EVDF_TEXTURE_NSQUARE: 
	case EVDF_TEXTURE_MATRIX:
		on = 1;
		break;

	case EVDF_DEPTH_CLAMP: // shadow
		on = 1;
		break;

	case EVDF_TEXTURE_NPOT: // for 2D
		on = 0;
		break;

	case EVDF_ARB_FRAGMENT_PROGRAM_1:
	case EVDF_ARB_VERTEX_PROGRAM_1:
		on = 1;
		break;
#if defined(PATCH_SUPERTUX_8_0_1)
	case EVDF_TEXTURE_NPOT:
	case EVDF_ARB_GLSL:
		on = 1;
		break;
#endif

#if defined(SOFTWARE_DRIVER_2_2D_AS_3D)
#if defined(IRRLICHT_FREE_CANVAS)
	case EVDF_VIEWPORT_SCALE_GUI:
		on = 1;
		break;
#endif
#endif
	default:
		on = 0;
		break;
	}

	return on && FeatureEnabled[feature];
}



//! Create render target.
IRenderTarget* CBurningVideoDriver::addRenderTarget()
{
	CSoftwareRenderTarget2* renderTarget = new CSoftwareRenderTarget2(this);
	RenderTargets.push_back(renderTarget);

	return renderTarget;
}


//matrix multiplication
void CBurningVideoDriver::transform_calc(E_TRANSFORMATION_STATE_BURNING_VIDEO state)
{
	size_t* flag = TransformationFlag[TransformationStack];
	if (flag[state] & ETF_VALID ) return;

	//check
	int ok = 0;
	switch ( state )
	{
		case ETS_PROJ_MODEL_VIEW:
			if ( 0 == (flag[ETS_VIEW_PROJECTION] & ETF_VALID) ) transform_calc (ETS_VIEW_PROJECTION);
			ok = flag[ETS_WORLD] & flag[ETS_VIEW] & flag[ETS_PROJECTION] & flag[ETS_VIEW_PROJECTION] & ETF_VALID;
			break;
		case ETS_VIEW_PROJECTION:
			ok = flag[ETS_VIEW] & flag[ETS_PROJECTION] & ETF_VALID;
			break;
		//case ETS_VIEW_INVERSE:
		//	ok = TransformationFlag[ETS_VIEW] & ETF_VALID;
		//	break;
		case ETS_MODEL_VIEW:
			ok = flag[ETS_WORLD] & flag[ETS_VIEW] & ETF_VALID;
			break;
		case ETS_NORMAL:
			ok = flag[ETS_MODEL_VIEW] & ETF_VALID;
			break;
		default:
			break;
	}

	if ( !ok )
	{
		char buf[256];
		sprintf(buf,"transform_calc not valid for %d\n",state);
		os::Printer::log(buf, ELL_WARNING);
	}

	core::matrix4* matrix = Transformation[TransformationStack];

	switch ( state )
	{
		case ETS_PROJ_MODEL_VIEW:
			if (flag[ETS_WORLD] & ETF_IDENTITY )
			{
				matrix[state] = matrix[ETS_VIEW_PROJECTION];
			}
			else
			{
				matrix[state].setbyproduct_nocheck(matrix[ETS_VIEW_PROJECTION], matrix[ETS_WORLD]);
			}
			break;

		//case ETS_VIEW_INVERSE:
		//	mat44_inverse(matrix[state],matrix[ETS_VIEW]);
		//	break;

		case ETS_VIEW_PROJECTION:
			matrix[state].setbyproduct_nocheck (matrix[ETS_PROJECTION], matrix[ETS_VIEW]);
			break;
		case ETS_MODEL_VIEW:
			if ( flag[ETS_WORLD] & ETF_IDENTITY )
			{
				matrix[state] = matrix[ETS_VIEW];
			}
			else
			{
				matrix[state].setbyproduct_nocheck(matrix[ETS_VIEW], matrix[ETS_WORLD]);
			}
			break;
		case ETS_NORMAL:
			mat44_transposed_inverse(matrix[state], matrix[ETS_MODEL_VIEW]);
			break;

		default:
			break;
	}
	flag[state] |= ETF_VALID;
}


//! sets transformation
void CBurningVideoDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	size_t* flag = TransformationFlag[TransformationStack];
	core::matrix4* matrix = Transformation[TransformationStack];

#if 0
	int changed = 1;
	if (flag[state] & ETF_VALID)
	{
		changed = memcmp(mat.pointer(), matrix[state].pointer(), sizeof(mat));
	}
	if (changed)
#endif
	{
		matrix[state] = mat;
		flag[state] |= ETF_VALID;
	}

	//maybe identity (mostly for texturematrix to avoid costly multiplication)
#if defined ( USE_MATRIX_TEST )
	burning_setbit( TransformationFlag[state], mat.getDefinitelyIdentityMatrix(), ETF_IDENTITY );
#else
	burning_setbit(flag[state],
		!memcmp(mat.pointer(), core::IdentityMatrix.pointer(),sizeof(mat)),ETF_IDENTITY
	);
#endif

#if 0
	if ( changed )
#endif
	switch ( state )
	{
		case ETS_PROJECTION:
			flag[ETS_PROJ_MODEL_VIEW] &= ~ETF_VALID;
			flag[ETS_VIEW_PROJECTION] &= ~ETF_VALID;
			break;
		case ETS_VIEW:
			flag[ETS_PROJ_MODEL_VIEW] &= ~ETF_VALID;
			flag[ETS_VIEW_PROJECTION] &= ~ETF_VALID;
			//flag[ETS_VIEW_INVERSE] &= ~ETF_VALID;
			flag[ETS_MODEL_VIEW] &= ~ETF_VALID;
			flag[ETS_NORMAL] &= ~ETF_VALID;
			break;
		case ETS_WORLD:
			flag[ETS_PROJ_MODEL_VIEW] &= ~ETF_VALID;
			flag[ETS_MODEL_VIEW] &= ~ETF_VALID;
			flag[ETS_NORMAL] &= ~ETF_VALID;
			break;
		case ETS_TEXTURE_0:
		case ETS_TEXTURE_1:
		case ETS_TEXTURE_2:
		case ETS_TEXTURE_3:
#if _IRR_MATERIAL_MAX_TEXTURES_>4
		case ETS_TEXTURE_4:
#endif
#if _IRR_MATERIAL_MAX_TEXTURES_>5
		case ETS_TEXTURE_5:
#endif
#if _IRR_MATERIAL_MAX_TEXTURES_>6
		case ETS_TEXTURE_6:
#endif
#if _IRR_MATERIAL_MAX_TEXTURES_>7
		case ETS_TEXTURE_7:
#endif
			if ( 0 == (flag[state] & ETF_IDENTITY ) )
			{
				EyeSpace.TL_Flag |= TL_TEXTURE_TRANSFORM;
			}
			break;
		default:
			break;
	}

}

//! Returns the transformation set by setTransform
const core::matrix4& CBurningVideoDriver::getTransform(E_TRANSFORMATION_STATE state) const
{
	return Transformation[TransformationStack][state];
}


bool CBurningVideoDriver::beginScene(u16 clearFlag, SColor clearColor, f32 clearDepth, u8 clearStencil, const SExposedVideoData& videoData, core::rect<s32>* sourceRect)
{
	CNullDriver::beginScene(clearFlag, clearColor, clearDepth, clearStencil, videoData, sourceRect);
	WindowId = videoData.D3D9.HWnd;
	SceneSourceRect = sourceRect;

	clearBuffers(clearFlag, clearColor, clearDepth, clearStencil);

	//memset ( TransformationFlag, 0, sizeof ( TransformationFlag ) );
	return true;
}

bool CBurningVideoDriver::endScene()
{
	CNullDriver::endScene();

	return Presenter->present(BackBuffer, WindowId, SceneSourceRect);
}

bool CBurningVideoDriver::setRenderTargetEx(IRenderTarget* target, u16 clearFlag, SColor clearColor, f32 clearDepth, u8 clearStencil)
{
	if (target && target->getDriverType() != EDT_BURNINGSVIDEO)
	{
		os::Printer::log("Fatal Error: Tried to set a render target not owned by this driver.", ELL_ERROR);
		return false;
	}

	if (RenderTargetTexture)
		RenderTargetTexture->drop();

	CSoftwareRenderTarget2* renderTarget = static_cast<CSoftwareRenderTarget2*>(target);
	RenderTargetTexture = (renderTarget) ? renderTarget->getTexture() : 0;

	if (RenderTargetTexture)
	{
		RenderTargetTexture->grab();
		setRenderTargetImage(((CSoftwareTexture2*)RenderTargetTexture)->getTexture());
	}
	else
	{
		setRenderTargetImage(BackBuffer);
	}

	clearBuffers(clearFlag, clearColor, clearDepth, clearStencil);

	return true;
}

//! sets a render target
void CBurningVideoDriver::setRenderTargetImage(video::CImage* image)
{
	if (RenderTargetSurface)
		RenderTargetSurface->drop();

	core::dimension2d<u32> current = RenderTargetSize;
	RenderTargetSurface = image;
	RenderTargetSize.Width = 0;
	RenderTargetSize.Height = 0;

	if (RenderTargetSurface)
	{
		RenderTargetSurface->grab();
		RenderTargetSize = RenderTargetSurface->getDimension();
	}

	int not_changed = current == RenderTargetSize;
	burning_setbit(TransformationFlag[0][ETS_PROJECTION], not_changed, ETF_VALID);
	burning_setbit(TransformationFlag[1][ETS_PROJECTION], not_changed, ETF_VALID);

	setViewPort(core::recti(RenderTargetSize));

	if (DepthBuffer)
		DepthBuffer->setSize(RenderTargetSize);

	if (StencilBuffer)
		StencilBuffer->setSize(RenderTargetSize);
}


//--------- Transform from NDC to DC, transform TexCoo ----------------------------------------------

// controls subtexel and fill convention
#if defined(SOFTWARE_DRIVER_2_SUBTEXEL)
#define SOFTWARE_DRIVER_2_PIXEL_CENTER -0.5f
#else
#define SOFTWARE_DRIVER_2_PIXEL_CENTER -0.5f
#endif

#if 1

// used to scale <-1,-1><1,1> to viewport
void buildNDCToDCMatrix(f32* m, const core::rect<s32>& viewport, f32 tx)
{
	m[0] = (viewport.getWidth() + tx) * 0.5f;
	m[1] = SOFTWARE_DRIVER_2_PIXEL_CENTER + ((viewport.UpperLeftCorner.X + viewport.LowerRightCorner.X) * 0.5f);

	m[2] = (viewport.getHeight() + tx) * -0.5f;
	m[3] = SOFTWARE_DRIVER_2_PIXEL_CENTER + ((viewport.UpperLeftCorner.Y + viewport.LowerRightCorner.Y) * 0.5f);
}


#else
// used to scale <-1,-1><1,1> to viewport
void buildNDCToDCMatrix( core::matrix4& out, const core::rect<s32>& viewport)
{
	//guard band to stay in screen bounds.(empirical). get holes left side otherwise or out of screen
	//TODO: match openGL or D3D.
	//assumption pixel center, top-left rule and rounding error projection deny exact match without additional clipping
	//or triangle render scanline doesn't step on correct texel center
	//or sampler is not on texel center

	f32* m = out.pointer();
#if defined(Tweak_Burning) && 0
	m[0]  = (viewport.getWidth() + Tweak.ndc_shrink_x ) * Tweak.ndc_scale_x;
	m[5]  = (viewport.getHeight() + Tweak.ndc_shrink_y ) * Tweak.ndc_scale_y;
	m[12] = Tweak.ndc_trans_x + ( (viewport.UpperLeftCorner.X + viewport.LowerRightCorner.X ) * 0.5f );
	m[13] = Tweak.ndc_trans_y + ( (viewport.UpperLeftCorner.Y + viewport.LowerRightCorner.Y ) * 0.5f );
#endif

	m[0]  = (viewport.getWidth() - 0.75f ) * 0.5f;
	m[1]  = 0.f;
	m[2]  = 0.f;
	m[3]  = 0.f;
	m[4]  = 0.f;
	m[5]  = (viewport.getHeight() - 0.75f ) * -0.5f;
	m[6]  = 0.f;
	m[7]  = 0.f;
	m[8]  = 0.f;
	m[9]  = 0.f;
	m[10] = 1.f;
	m[11] = 0.f;
	m[12] = SOFTWARE_DRIVER_2_PIXEL_CENTER + ( (viewport.UpperLeftCorner.X + viewport.LowerRightCorner.X ) * 0.5f );
	m[13] = SOFTWARE_DRIVER_2_PIXEL_CENTER + ( (viewport.UpperLeftCorner.Y + viewport.LowerRightCorner.Y ) * 0.5f );
	m[14] = 0.f;
	m[15] = 1.f;
}
#endif


//--------- Transform from NCD to DC ----------------------------------------------

//! sets a viewport
void CBurningVideoDriver::setViewPort(const core::rect<s32>& area)
{
	ViewPort = area;

	core::rect<s32> rendert(0,0,RenderTargetSize.Width,RenderTargetSize.Height);
	ViewPort.clipAgainst(rendert);

	buildNDCToDCMatrix(Transformation_ETS_CLIPSCALE[0], ViewPort,-0.375f);
	buildNDCToDCMatrix(Transformation_ETS_CLIPSCALE[1], ViewPort, 0.f); // OverrideMaterial2DEnabled ? -0.375f : 0.f);

	if (CurrentShader)
		CurrentShader->setRenderTarget(RenderTargetSurface, ViewPort);
}

void CBurningVideoDriver::setScissor(int x, int y, int width, int height)
{
	AbsRectangle v0;
	v0.x0 = x;
	v0.y0 = y;
	v0.x1 = x+width;
	v0.y1 = y+width;

	const core::dimension2d<u32>& rt = getCurrentRenderTargetSize();
	AbsRectangle v1;
	v1.x0 = 0;
	v1.y0 = 0;
	v1.x1 = rt.Width;
	v1.y1 = rt.Height;

	intersect(Scissor, v0, v1);
}

/*
	generic plane clipping in homogenous coordinates
	special case ndc frustum <-w,w>,<-w,w>,<-w,w>
	can be rewritten with compares e.q near plane, a.z < -a.w and b.z < -b.w

	cam is (0,0,-1)
*/

const sVec4 CBurningVideoDriver::NDCPlane[6+2] =
{
	sVec4(  0.f,  0.f, -1.f, -1.f ),	// near
	sVec4(  0.f,  0.f,  1.f, -1.f ),	// far
	sVec4(  1.f,  0.f,  0.f, -1.f ),	// left
	sVec4( -1.f,  0.f,  0.f, -1.f ),	// right
	sVec4(  0.f,  1.f,  0.f, -1.f ),	// bottom
	sVec4(  0.f, -1.f,  0.f, -1.f )		// top
};


/*
	test a vertex if it's inside the standard frustum

	this is the generic one..

	f32 dotPlane;
	for ( u32 i = 0; i!= 6; ++i )
	{
		dotPlane = v->Pos.dotProduct ( NDCPlane[i] );
		burning_setbit32( flag, dotPlane <= 0.f, 1 << i );
	}

	// this is the base for ndc frustum <-w,w>,<-w,w>,<-w,w>
	burning_setbit32( flag, ( v->Pos.z - v->Pos.w ) <= 0.f, 1 );
	burning_setbit32( flag, (-v->Pos.z - v->Pos.w ) <= 0.f, 2 );
	burning_setbit32( flag, ( v->Pos.x - v->Pos.w ) <= 0.f, 4 );
	burning_setbit32( flag, (-v->Pos.x - v->Pos.w ) <= 0.f, 8 );
	burning_setbit32( flag, ( v->Pos.y - v->Pos.w ) <= 0.f, 16 );
	burning_setbit32( flag, (-v->Pos.y - v->Pos.w ) <= 0.f, 32 );

*/
#ifdef IRRLICHT_FAST_MATH

REALINLINE size_t CBurningVideoDriver::clipToFrustumTest ( const s4DVertex* v  ) const
{
	register size_t flag;
	f32 test[8];
	const f32 w = - v->Pos.w;

	// a conditional move is needed....FCOMI ( but we don't have it )
	// so let the fpu calculate and write it back.
	// cpu makes the compare, interleaving

	test[0] =  v->Pos.z + w;
	test[1] = -v->Pos.z + w;
	test[2] =  v->Pos.x + w;
	test[3] = -v->Pos.x + w;
	test[4] =  v->Pos.y + w;
	test[5] = -v->Pos.y + w;
	
	const u32* a = F32_AS_U32_POINTER(test);
	flag =  (a[0]             ) >> 31;
	flag |= (a[1] & 0x80000000) >> 30;
	flag |= (a[2] & 0x80000000) >> 29;
	flag |= (a[3] & 0x80000000) >> 28;
	flag |= (a[4] & 0x80000000) >> 27;
	flag |= (a[5] & 0x80000000) >> 26;

/*
	flag  = (IR ( test[0] )              ) >> 31;
	flag |= (IR ( test[1] ) & 0x80000000 ) >> 30;
	flag |= (IR ( test[2] ) & 0x80000000 ) >> 29;
	flag |= (IR ( test[3] ) & 0x80000000 ) >> 28;
	flag |= (IR ( test[4] ) & 0x80000000 ) >> 27;
	flag |= (IR ( test[5] ) & 0x80000000 ) >> 26;
*/
/*
	flag  = F32_LOWER_EQUAL_0 ( test[0] );
	flag |= F32_LOWER_EQUAL_0 ( test[1] ) << 1;
	flag |= F32_LOWER_EQUAL_0 ( test[2] ) << 2;
	flag |= F32_LOWER_EQUAL_0 ( test[3] ) << 3;
	flag |= F32_LOWER_EQUAL_0 ( test[4] ) << 4;
	flag |= F32_LOWER_EQUAL_0 ( test[5] ) << 5;
*/
	return flag;
}

#else


REALINLINE size_t clipToFrustumTest ( const s4DVertex* v  )
{
	size_t flag = 0;

	flag |=  v->Pos.z <= v->Pos.w ? VERTEX4D_CLIP_NEAR	: 0;
	flag |= -v->Pos.z <= v->Pos.w ? VERTEX4D_CLIP_FAR	: 0;

	flag |=  v->Pos.x <= v->Pos.w ? VERTEX4D_CLIP_LEFT	: 0;
	flag |= -v->Pos.x <= v->Pos.w ? VERTEX4D_CLIP_RIGHT	: 0;
	
	flag |=  v->Pos.y <= v->Pos.w ? VERTEX4D_CLIP_BOTTOM	: 0;
	flag |= -v->Pos.y <= v->Pos.w ? VERTEX4D_CLIP_TOP	: 0;


/*
	for ( u32 i = 0; i <= 6; ++i )
	{
		if (v->Pos.dot_xyzw(NDCPlane[i]) <= 0.f) flag |= ((size_t)1) << i;
	}
*/
	return flag;
}

#endif // _MSC_VER


size_t clipToHyperPlane (
	s4DVertexPair* burning_restrict dest,
	const s4DVertexPair* burning_restrict source,
	const size_t inCount,
	const sVec4& plane
)
{
	size_t outCount = 0;
	s4DVertexPair* out = dest;

	const s4DVertex* a;
	const s4DVertex* b = source;

	ipoltype bDotPlane;
	bDotPlane = b->Pos.dot_xyzw( plane );

/*
	for( u32 i = 1; i < inCount + 1; ++i)
	{
#if 0
		a = source + (i%inCount)*2;
#else
		const s32 condition = i - inCount;
		const s32 index = (( ( condition >> 31 ) & ( i ^ condition ) ) ^ condition ) << 1;
		a = source + index;
#endif
*/
	//Sutherland–Hodgman
	for(size_t i = 0; i < inCount; ++i)
	{
		a = source + (i == inCount-1 ? 0 : s4DVertex_ofs(i+1));

		// current point inside
		if (ipol_lower_equal_0(a->Pos.dot_xyzw( plane )) )
		{
			// last point outside
			if (ipol_greater_0( bDotPlane ) )
			{
				// intersect line segment with plane
				out->interpolate(*b, *a, bDotPlane / (b->Pos - a->Pos).dot_xyzw(plane) );
				out += sizeof_s4DVertexPairRel;
				outCount += 1;
			}

			// copy current to out
			//*out = *a;
			memcpy_s4DVertexPair( out, a);
			b = out;

			out += sizeof_s4DVertexPairRel;
			outCount += 1;
		}
		else
		{
			// current point outside
			if (ipol_lower_equal_0(  bDotPlane ) )
			{
				// previous was inside
				// intersect line segment with plane
				out->interpolate(*b, *a, bDotPlane / (b->Pos - a->Pos).dot_xyzw(plane) );
				out += sizeof_s4DVertexPairRel;
				outCount += 1;
			}
			// pointer
			b = a;
		}

		bDotPlane = b->Pos.dot_xyzw( plane );
	}

	return outCount;
}


/*
	Clip on all planes. Clipper.data
	clipmask per face
*/
size_t CBurningVideoDriver::clipToFrustum(const size_t vIn /*, const size_t clipmask_for_face*/ )
{
	s4DVertexPair* v0 = Clipper.data;
	s4DVertexPair* v1 = Clipper_temp.data;
	size_t vOut = vIn;

	//clear all clipping & projected flags
	const u32 flag = v0[0].flag & VERTEX4D_FORMAT_MASK;
	for (size_t g = 0; g != Clipper.ElementSize; ++g)
	{
		v0[g].flag = flag;
		v1[g].flag = flag;
	}

#if 0
	for (size_t i = 0; i < 6; ++i)
	{
		v0 = i & 1 ? Clipper_temp.data : Clipper.data;
		v1 = i & 1 ? Clipper.data : Clipper_temp.data;

		//clipMask checked outside - always clip all planes
#if 0
		if (0 == (clipMask & ((size_t)1<<i)))
		{
			vOut = vIn;
			memcpy_s4DVertexPair(v1, v0);
		}
		else
#endif
		{
			vOut = clipToHyperPlane(v1, v0, vOut, NDCPlane[i]);
			if (vOut < vIn) return vOut;
		}
	}
#endif


	vOut = clipToHyperPlane( v1, v0, vOut, NDCPlane[0] ); if ( vOut < vIn) return  vOut;
	vOut = clipToHyperPlane( v0, v1, vOut, NDCPlane[1] ); if ( vOut < vIn ) return vOut;
	vOut = clipToHyperPlane( v1, v0, vOut, NDCPlane[2] ); if ( vOut < vIn ) return vOut;
	vOut = clipToHyperPlane( v0, v1, vOut, NDCPlane[3] ); if ( vOut < vIn ) return vOut;
	vOut = clipToHyperPlane( v1, v0, vOut, NDCPlane[4] ); if ( vOut < vIn ) return vOut;
	vOut = clipToHyperPlane( v0, v1, vOut, NDCPlane[5] );

	return vOut;
}

/*!
 Part I:
	apply Clip Scale matrix
	From Normalized Device Coordiante ( NDC ) Space to Device Coordinate ( DC ) Space 

 Part II:
	Project homogeneous vector
	homogeneous to non-homogenous coordinates ( dividebyW )

	Incoming: ( xw, yw, zw, w, u, v, 1, R, G, B, A )
	Outgoing: ( xw/w, yw/w, zw/w, w/w, u/w, v/w, 1/w, R/w, G/w, B/w, A/w )

	replace w/w by 1/w
*/
//aliasing problems! [dest = source + 1]
inline void CBurningVideoDriver::ndc_2_dc_and_project (s4DVertexPair* dest,const s4DVertexPair *source, const size_t vIn ) const
{
	const f32* dc = Transformation_ETS_CLIPSCALE[TransformationStack];

	for ( size_t g = 0; g != vIn; g += sizeof_s4DVertexPairRel)
	{
		if ( dest[g].flag & VERTEX4D_PROJECTED )
			continue;

		dest[g].flag = source[g].flag | VERTEX4D_PROJECTED;

		const f32 w = source[g].Pos.w;
		const f32 iw = reciprocal_zero ( w );

		// to device coordinates
		dest[g].Pos.x = iw * ( source[g].Pos.x * dc[0] + w * dc[1] );
		dest[g].Pos.y = iw * ( source[g].Pos.y * dc[2] + w * dc[3] );

		//burning uses direct Z. for OpenGL it should be -Z and texture flip
#if !defined(SOFTWARE_DRIVER_2_USE_WBUFFER) || 1
		dest[g].Pos.z = iw * source[g].Pos.z;
#endif
		dest[g].Pos.w = iw;

// Texture Coordinates will be projected after mipmap selection
// satisfy write-combiner
#if 1
#if BURNING_MATERIAL_MAX_TEXTURES > 0
		dest[g].Tex[0] = source[g].Tex[0];
#endif
#if BURNING_MATERIAL_MAX_TEXTURES > 1
		dest[g].Tex[1] = source[g].Tex[1];
#endif
#if BURNING_MATERIAL_MAX_TEXTURES > 2
		dest[g].Tex[2] = source[g].Tex[2];
#endif
#if BURNING_MATERIAL_MAX_TEXTURES > 3
		dest[g].Tex[3] = source[g].Tex[3];
#endif

#endif

#if BURNING_MATERIAL_MAX_COLORS > 0
		#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
			dest[g].Color[0] = source[g].Color[0] * iw; // alpha?
		#else
			dest[g].Color[0] = source[g].Color[0];
		#endif
#endif

#if BURNING_MATERIAL_MAX_COLORS > 1
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
			dest[g].Color[1] = source[g].Color[1] * iw; // alpha?
#else
			dest[g].Color[1] = source[g].Color[1];
#endif
#endif

#if BURNING_MATERIAL_MAX_COLORS > 2
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
			dest[g].Color[2] = source[g].Color[2] * iw; // alpha?
#else
			dest[g].Color[2] = source[g].Color[2];
#endif
#endif

#if BURNING_MATERIAL_MAX_COLORS > 3
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
			dest[g].Color[3] = source[g].Color[3] * iw; // alpha?
#else
			dest[g].Color[3] = source[g].Color[3];
#endif
#endif

#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
		dest[g].LightTangent[0] = source[g].LightTangent[0] * iw;
#endif

	}
}



/*!
	crossproduct in projected 2D, face
*/
REALINLINE f32 CBurningVideoDriver::screenarea_inside(const s4DVertexPair* burning_restrict const face[] ) const
{
	return	( ((face[1]+1)->Pos.x - (face[0]+1)->Pos.x) * ((face[2]+1)->Pos.y - (face[0]+1)->Pos.y) ) -
			( ((face[2]+1)->Pos.x - (face[0]+1)->Pos.x) * ((face[1]+1)->Pos.y - (face[0]+1)->Pos.y) );
/*
	float signedArea = 0;
	for (int k = 1; k < output->count; k++) {
		signedArea += (output->vertices[k - 1].values[0] * output->vertices[k - 0].values[1]);
		signedArea -= (output->vertices[k - 0].values[0] * output->vertices[k - 1].values[1]);
	}
*/
}

#if 0
static inline f32 dot(const sVec2& a,const sVec2& b) { return a.x * b.x + a.y * b.y; }
sVec2 dFdx(const sVec2& v) { return v; }
sVec2 dFdy(const sVec2& v) { return v; }

f32 MipmapLevel(const sVec2& uv, const sVec2& textureSize)
{
	sVec2 dx = dFdx(uv * textureSize.x);
	sVec2 dy = dFdy(uv * textureSize.y);
	f32 d = core::max_(dot(dx, dx), dot(dy, dy));
	return log2f(sqrtf(d));
}
#endif

#define MAT_TEXTURE(tex) ( (video::CSoftwareTexture2*) Material.org.getTexture ( (u32)tex ) )

/*!
	calculate from unprojected.
	attribute need not to follow winding rule from position.
	Edge-walking problem
	Texture Wrapping problem
*/
REALINLINE s32 CBurningVideoDriver::lodFactor_inside(const s4DVertexPair* burning_restrict const face[],
	const size_t m, f32 dc_area, f32 lod_bias) const
{
	/*
		sVec2 a(v[1]->Tex[tex].x - v[0]->Tex[tex].x,v[1]->Tex[tex].y - v[0]->Tex[tex].y);
		sVec2 b(v[2]->Tex[tex].x - v[0]->Tex[tex].x,v[2]->Tex[tex].y - v[0]->Tex[tex].y);
		f32 area = a.x * b.y - b.x * a.y;
	*/


	/*
		degenerate(A, B, C, minarea) = ((B - A).cross(C - A)).lengthSquared() < (4.0f * minarea * minarea);
		check for collapsed or "long thin triangles"
	*/
	ieee754 signedArea;

	ieee754 t[4];
	t[0].f = face[1]->Tex[m].x - face[0]->Tex[m].x;
	t[1].f = face[1]->Tex[m].y - face[0]->Tex[m].y;

	t[2].f = face[2]->Tex[m].x - face[0]->Tex[m].x;
	t[3].f = face[2]->Tex[m].y - face[0]->Tex[m].y;

	//crossproduct in projected 2D -> screen area triangle
	signedArea.f = t[0].f * t[3].f - t[2].f * t[1].f;

	//signedArea =
	//	  ((face[1]->Tex[m].x - face[0]->Tex[m].x) * (face[2]->Tex[m].y - face[0]->Tex[m].y))
	//	- ((face[2]->Tex[m].x - face[0]->Tex[m].x) * (face[1]->Tex[m].y - face[0]->Tex[m].y));

	//if (signedArea*signedArea <= 0.00000000001f)
	if (signedArea.fields.exp == 0 )
	{
		ieee754 _max;
		_max.u = t[0].abs.frac_exp;
		if (t[1].abs.frac_exp > _max.u) _max.u = t[1].abs.frac_exp;
		if (t[2].abs.frac_exp > _max.u) _max.u = t[2].abs.frac_exp;
		if (t[3].abs.frac_exp > _max.u) _max.u = t[3].abs.frac_exp;

		signedArea.u = _max.fields.exp ? _max.u : ieee754_one;

/*
		//dot,length
		ieee754 v[2];
		v[0].f = t[0] * t[2];
		v[1].f = t[1] * t[3];
		
		//signedArea.f = t[4] > t[5] ? t[4] : t[5];
		signedArea.u = v[0].fields.frac > v[1].fields.frac ? v[0].u : v[1].u;
		if (signedArea.fields.exp == 0)
		{
			return -1;
		}
*/
	}

	//only guessing: take more detail (lower mipmap) in light+bump textures
	//assume transparent add is ~50% transparent -> more detail
	const u32* d = MAT_TEXTURE(m)->getMipMap0_Area();
	f32 texelspace = d[0] * d[1] * lod_bias; //(m ? 0.5f : 0.5f);

	ieee754 ratio;
	ratio.f = (signedArea.f * texelspace) * dc_area;
	ratio.fields.sign = 0;

	//log2(0)==denormal [ use high lod] [ only if dc_area == 0 checked outside ]
#if 0
	if (ratio.fields.exp == 0)
	{
		int g = 1;
	}
#endif
	//log2
	return (ratio.fields.exp & 0x80) ? ratio.fields.exp - 127 : 0; /*denormal very high lod*/

	//return (ratio.f <= 1.f) ? 0 : 1;
	//f32 texArea = MAT_TEXTURE(m)->getLODFactor(signedArea); // texelarea_inside(face, m);
	//s32 lodFactor = s32_log2_f32(texArea * dc_area); /* avoid denorm */

	//return MAT_TEXTURE(m)->getLODFactor(signedArea);
}


/*!
	texcoo in current mipmap dimension (face, already clipped)
	-> want to help fixpoint
*/
inline void CBurningVideoDriver::select_polygon_mipmap_inside(s4DVertex* burning_restrict face[], const size_t tex, const CSoftwareTexture2_Bound& b) const
{
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
#if defined(Tweak_Burning)
	(face[0] + 1)->Tex[tex].x = face[0]->Tex[tex].x * (face[0] + 1)->Pos.w * (b.w + Tweak.tex_w_add) + (b.cx + Tweak.tex_cx_add);
	(face[0] + 1)->Tex[tex].y = face[0]->Tex[tex].y * (face[0] + 1)->Pos.w * (b.h + Tweak.tex_h_add) + (b.cy + Tweak.tex_cy_add);

	(face[1] + 1)->Tex[tex].x = face[1]->Tex[tex].x * (face[1] + 1)->Pos.w * (b.w + Tweak.tex_w_add) + (b.cx + Tweak.tex_cx_add);
	(face[1] + 1)->Tex[tex].y = face[1]->Tex[tex].y * (face[1] + 1)->Pos.w * (b.h + Tweak.tex_h_add) + (b.cy + Tweak.tex_cy_add);

	(face[2] + 1)->Tex[tex].x = face[2]->Tex[tex].x * (face[2] + 1)->Pos.w * (b.w + Tweak.tex_w_add) + (b.cx + Tweak.tex_cx_add);
	(face[2] + 1)->Tex[tex].y = face[2]->Tex[tex].y * (face[2] + 1)->Pos.w * (b.h + Tweak.tex_h_add) + (b.cy + Tweak.tex_cy_add);
#else
	(face[0] + 1)->Tex[tex].x = face[0]->Tex[tex].x * (face[0] + 1)->Pos.w * b.w + b.cx;
	(face[0] + 1)->Tex[tex].y = face[0]->Tex[tex].y * (face[0] + 1)->Pos.w * b.h + b.cy;

	(face[1] + 1)->Tex[tex].x = face[1]->Tex[tex].x * (face[1] + 1)->Pos.w * b.w + b.cx;
	(face[1] + 1)->Tex[tex].y = face[1]->Tex[tex].y * (face[1] + 1)->Pos.w * b.h + b.cy;

	(face[2] + 1)->Tex[tex].x = face[2]->Tex[tex].x * (face[2] + 1)->Pos.w * b.w + b.cx;
	(face[2] + 1)->Tex[tex].y = face[2]->Tex[tex].y * (face[2] + 1)->Pos.w * b.h + b.cy;
#endif
#else
	(face[0] + 1)->Tex[tex].x = face[0]->Tex[tex].x * b.w;
	(face[0] + 1)->Tex[tex].y = face[0]->Tex[tex].y * b.h;

	(face[1] + 1)->Tex[tex].x = face[1]->Tex[tex].x * b.w;
	(face[1] + 1)->Tex[tex].y = face[1]->Tex[tex].y * b.h;

	(face[2] + 1)->Tex[tex].x = face[2]->Tex[tex].x * b.w;
	(face[2] + 1)->Tex[tex].y = face[2]->Tex[tex].y * b.h;
#endif

}


// Vertex Cache

//! setup Vertex Format
void CBurningVideoDriver::VertexCache_map_source_format()
{
	u32 s0 = sizeof(s4DVertex);
	u32 s1 = sizeof(s4DVertex_proxy);

	if ( s1 <= sizeof_s4DVertex /2 )
	{
		os::Printer::log ( "BurningVideo vertex format unnecessary to large", ELL_WARNING );
	}

	//memcpy_vertex
	if ( s0 != sizeof_s4DVertex || ((sizeof_s4DVertex * sizeof_s4DVertexPairRel)&31))
	{
		os::Printer::log ( "BurningVideo vertex format compile problem", ELL_ERROR );
		_IRR_DEBUG_BREAK_IF(1);
	}

#if defined(ENV64BIT)
	if (sizeof(void*) != 8)
	{
		os::Printer::log("BurningVideo pointer should be 8 bytes", ELL_ERROR);
		_IRR_DEBUG_BREAK_IF(1);
	}
#endif

	SVSize* vSize = VertexCache.vSize;
	//vSize[E4VT_STANDARD].Format = VERTEX4D_FORMAT_TEXTURE_1 | VERTEX4D_FORMAT_COLOR_1 | VERTEX4D_FORMAT_LIGHT_1 | VERTEX4D_FORMAT_SPECULAR;
	vSize[E4VT_STANDARD].Format = VERTEX4D_FORMAT_TEXTURE_1 | VERTEX4D_FORMAT_COLOR_2_FOG;
	vSize[E4VT_STANDARD].Pitch = sizeof(S3DVertex);
	vSize[E4VT_STANDARD].TexSize = 1;
	vSize[E4VT_STANDARD].TexCooSize = 1;

	vSize[E4VT_2TCOORDS].Format = VERTEX4D_FORMAT_TEXTURE_2 | VERTEX4D_FORMAT_COLOR_1;
	vSize[E4VT_2TCOORDS].Pitch = sizeof(S3DVertex2TCoords);
	vSize[E4VT_2TCOORDS].TexSize = 2;
	vSize[E4VT_2TCOORDS].TexCooSize = 2;

	//vSize[E4VT_TANGENTS].Format = VERTEX4D_FORMAT_TEXTURE_2 | VERTEX4D_FORMAT_COLOR_1 | VERTEX4D_FORMAT_LIGHT_1 | VERTEX4D_FORMAT_BUMP_DOT3;
	vSize[E4VT_TANGENTS].Format = VERTEX4D_FORMAT_TEXTURE_2 | VERTEX4D_FORMAT_COLOR_2_FOG | VERTEX4D_FORMAT_LIGHT_1 | VERTEX4D_FORMAT_BUMP_DOT3;
	vSize[E4VT_TANGENTS].Pitch = sizeof(S3DVertexTangents);
	vSize[E4VT_TANGENTS].TexSize = 2;
	vSize[E4VT_TANGENTS].TexCooSize = 2;

	// reflection map
	vSize[E4VT_REFLECTION_MAP].Format = VERTEX4D_FORMAT_TEXTURE_2 | VERTEX4D_FORMAT_COLOR_1;
	vSize[E4VT_REFLECTION_MAP].Pitch = sizeof(S3DVertex);
	vSize[E4VT_REFLECTION_MAP].TexSize = 2;
	vSize[E4VT_REFLECTION_MAP].TexCooSize = 1; //TexCoo2 generated

	// shadow
	vSize[E4VT_SHADOW].Format = 0;
	vSize[E4VT_SHADOW].Pitch = sizeof(f32) * 3; // core::vector3df*
	vSize[E4VT_SHADOW].TexSize = 0;
	vSize[E4VT_SHADOW].TexCooSize = 0;

	// color shading only (no texture)
	vSize[E4VT_NO_TEXTURE].Format = VERTEX4D_FORMAT_COLOR_1 | VERTEX4D_FORMAT_LIGHT_1 | VERTEX4D_FORMAT_SPECULAR;
	vSize[E4VT_NO_TEXTURE].Pitch = sizeof(S3DVertex);
	vSize[E4VT_NO_TEXTURE].TexSize = 0;
	vSize[E4VT_NO_TEXTURE].TexCooSize = 0;

	//Line
	vSize[E4VT_LINE].Format = VERTEX4D_FORMAT_COLOR_1;
	vSize[E4VT_LINE].Pitch = sizeof(S3DVertex);
	vSize[E4VT_LINE].TexSize = 0;
	vSize[E4VT_LINE].TexCooSize = 0;

	size_t size;
	for ( size_t i = 0; i < E4VT_COUNT; ++i )
	{
		size_t& flag = vSize[i].Format;

#if !defined(SOFTWARE_DRIVER_2_USE_SEPARATE_SPECULAR_COLOR)
		flag &= ~VERTEX4D_FORMAT_SPECULAR;
#endif
		if ( vSize[i].TexSize > BURNING_MATERIAL_MAX_TEXTURES )
			vSize[i].TexSize = BURNING_MATERIAL_MAX_TEXTURES;

		size = (flag & VERTEX4D_FORMAT_MASK_TEXTURE) >> 16;
		if ( size > BURNING_MATERIAL_MAX_TEXTURES )
		{
			flag = (flag & ~VERTEX4D_FORMAT_MASK_TEXTURE) | (BURNING_MATERIAL_MAX_TEXTURES << 16);
		}

		size = (flag & VERTEX4D_FORMAT_MASK_COLOR) >> 20;
		if ( size > BURNING_MATERIAL_MAX_COLORS )
		{
			flag = (flag & ~VERTEX4D_FORMAT_MASK_COLOR) | (BURNING_MATERIAL_MAX_COLORS << 20);
		}

		size = (flag & VERTEX4D_FORMAT_MASK_LIGHT) >> 24;
		if ( size > BURNING_MATERIAL_MAX_LIGHT_TANGENT)
		{
			flag = (flag & ~VERTEX4D_FORMAT_MASK_LIGHT) | (BURNING_MATERIAL_MAX_LIGHT_TANGENT << 24);
		}
	}

	VertexCache.mem.resize(VERTEXCACHE_ELEMENT * 2);
	VertexCache.vType = E4VT_STANDARD;

	Clipper.resize(VERTEXCACHE_ELEMENT * 2);
	Clipper_temp.resize(VERTEXCACHE_ELEMENT * 2);
	
	TransformationStack = 0;
	memset(TransformationFlag, 0, sizeof(TransformationFlag));
	memset(Transformation_ETS_CLIPSCALE, 0, sizeof(Transformation_ETS_CLIPSCALE));

	Material.resetRenderStates = true;
	Material.Fallback_MaterialType = EMT_SOLID;


}



/*!
	fill a cache line with transformed, light and clip test triangles
	overhead - if primitive is outside or culled, vertexLighting and TextureTransform is still done
*/
void CBurningVideoDriver::VertexCache_fill(const u32 sourceIndex, const u32 destIndex)
{
	u8* burning_restrict source;
	s4DVertex* burning_restrict dest;

	source = (u8*) VertexCache.vertices + ( sourceIndex * VertexCache.vSize[VertexCache.vType].Pitch );

	// it's a look ahead so we never hit it..
	// but give priority...
	//VertexCache.info[ destIndex ].hit = hitCount;

	// store info
	VertexCache.info[ destIndex ].index = sourceIndex;
	VertexCache.info[ destIndex ].hit = 0;

	// destination Vertex
	dest = VertexCache.mem.data + s4DVertex_ofs(destIndex);

	//Irrlicht S3DVertex,S3DVertex2TCoords,S3DVertexTangents
	const S3DVertex* base = ((S3DVertex*)source);

	// transform Model * World * Camera * Projection * NDCSpace matrix
	const core::matrix4* matrix = Transformation[TransformationStack];
	matrix[ETS_PROJ_MODEL_VIEW].transformVect(&dest->Pos.x, base->Pos);

	//mhm ... maybe no goto
	if (VertexCache.vType == E4VT_SHADOW)
	{
		//core::vector3df i = base->Pos;
		//i.Z -= 0.5f;
		//matrix[ETS_PROJ_MODEL_VIEW].transformVect(&dest->Pos.x, i);

		//GL_DEPTH_CLAMP,EVDF_DEPTH_CLAMP
		//if ( dest->Pos.z < dest->Pos.w)
		//	dest->Pos.z = dest->Pos.w*0.99f;

		//glPolygonOffset // self shadow wanted or not?
		dest->Pos.w *= 1.005f;

		//flag |= v->Pos.z <= v->Pos.w ? VERTEX4D_CLIP_NEAR : 0;
		//flag |= -v->Pos.z <= v->Pos.w ? VERTEX4D_CLIP_FAR : 0;

		goto clipandproject;
	}


#if defined (SOFTWARE_DRIVER_2_LIGHTING) || defined ( SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM )

	// vertex, normal in light(eye) space
	if ( Material.org.Lighting || (EyeSpace.TL_Flag & (TL_TEXTURE_TRANSFORM|TL_FOG)) )
	{
		sVec4 vertex4; //eye coordinate position of vertex
		matrix[ETS_MODEL_VIEW].transformVect ( &vertex4.x, base->Pos );

		f32 iw = reciprocal_zero(vertex4.w);
		EyeSpace.vertex.x = vertex4.x * iw;
		EyeSpace.vertex.y = vertex4.y * iw;
		EyeSpace.vertex.z = vertex4.z * iw;
		EyeSpace.vertex.w = iw;

		//EyeSpace.cam_distance = EyeSpace.vertex.length_xyz();
		EyeSpace.cam_dir = EyeSpace.vertex;
		EyeSpace.cam_dir.normalize_dir_xyz();

		matrix[ETS_NORMAL].rotateVect(&EyeSpace.normal.x, base->Normal);
		if (EyeSpace.TL_Flag & TL_NORMALIZE_NORMALS)
			EyeSpace.normal.normalize_dir_xyz();

	}

#endif

#if BURNING_MATERIAL_MAX_COLORS > 1
	dest->Color[1].a = 1.f;
	dest->Color[1].r = 0.f;
	dest->Color[1].g = 0.f;
	dest->Color[1].b = 0.f;
#endif

#if BURNING_MATERIAL_MAX_COLORS > 2
	dest->Color[2].a = 1.f;
	dest->Color[2].r = 0.f;
	dest->Color[2].g = 0.f;
	dest->Color[2].b = 0.f;
#endif

#if BURNING_MATERIAL_MAX_COLORS > 3
	dest->Color[3].a = 1.f;
	dest->Color[3].r = 0.f;
	dest->Color[3].g = 0.f;
	dest->Color[3].b = 0.f;
#endif

#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
	dest->LightTangent[0].x = 0.f;
	dest->LightTangent[0].y = 0.f;
	dest->LightTangent[0].z = 0.f;
#endif


#if BURNING_MATERIAL_MAX_COLORS > 0
// apply lighting model
#if defined (SOFTWARE_DRIVER_2_LIGHTING)
	if ( Material.org.Lighting )
	{
		lightVertex_eye ( dest, base->Color.color );
	}
	else
	{
		dest->Color[0].setA8R8G8B8 ( base->Color.color );
	}
#else
	dest->Color[0].setA8R8G8B8 ( base->Color.color );
#endif
#endif

	//vertex fog
	if (EyeSpace.TL_Flag & TL_FOG ) //Material.org.FogEnable
	{
		f32 fog_factor = 1.f;

		switch (FogType)
		{
		case EFT_FOG_LINEAR:
			fog_factor = (FogEnd - EyeSpace.vertex.z) * EyeSpace.fog_scale;
			break;
		case EFT_FOG_EXP:
			fog_factor = expf(-FogDensity * EyeSpace.vertex.z);
			break;
		case EFT_FOG_EXP2:
			fog_factor = expf(-FogDensity * FogDensity * EyeSpace.vertex.z * EyeSpace.vertex.z);
			break;
		}

		sVec4* a = dest->Color + ((VertexCache.vSize[VertexCache.vType].Format & VERTEX4D_FORMAT_COLOR_2_FOG) ? 1 : 0);
		a->a = core::clamp(fog_factor, 0.f, 1.f);
	}

	// Texture Transform
#if defined ( SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM )

	if ( 0 == (EyeSpace.TL_Flag & TL_TEXTURE_TRANSFORM) )
#endif // SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
	{
		// Irrlicht TCoords and TCoords2 must be contiguous memory. baseTCoord has no 4 byte aligned start address!
		const f32* baseTCoord = &base->TCoords.X;

		switch (VertexCache.vSize[VertexCache.vType].TexCooSize)
		{
#if BURNING_MATERIAL_MAX_TEXTURES == 4
		case 0:
			dest->Tex[0].x = 0.f;
			dest->Tex[0].y = 0.f;
			dest->Tex[1].x = 0.f;
			dest->Tex[1].y = 0.f;
			dest->Tex[2].x = 0.f;
			dest->Tex[2].y = 0.f;
			dest->Tex[3].x = 0.f;
			dest->Tex[3].y = 0.f;
			break;
		case 1:
			dest->Tex[0].x = baseTCoord[0];
			dest->Tex[0].y = baseTCoord[1];
			dest->Tex[1].x = 0.f;
			dest->Tex[1].y = 0.f;
			dest->Tex[2].x = 0.f;
			dest->Tex[2].y = 0.f;
			dest->Tex[3].x = 0.f;
			dest->Tex[3].y = 0.f;
			break;
		case 2:
			dest->Tex[0].x = baseTCoord[0];
			dest->Tex[0].y = baseTCoord[1];
			dest->Tex[1].x = baseTCoord[2];
			dest->Tex[1].y = baseTCoord[3];
			dest->Tex[2].x = 0.f;
			dest->Tex[2].y = 0.f;
			dest->Tex[3].x = 0.f;
			dest->Tex[3].y = 0.f;
			break;
		case 3:
			dest->Tex[0].x = baseTCoord[0];
			dest->Tex[0].y = baseTCoord[1];
			dest->Tex[1].x = baseTCoord[2];
			dest->Tex[1].y = baseTCoord[3];
			dest->Tex[2].x = baseTCoord[4];
			dest->Tex[2].y = baseTCoord[5];
			dest->Tex[3].x = 0.f;
			dest->Tex[3].y = 0.f;
			break;
		case 4:
			dest->Tex[0].x = baseTCoord[0];
			dest->Tex[0].y = baseTCoord[1];
			dest->Tex[1].x = baseTCoord[2];
			dest->Tex[1].y = baseTCoord[3];
			dest->Tex[2].x = baseTCoord[4];
			dest->Tex[2].y = baseTCoord[5];
			dest->Tex[3].x = baseTCoord[6];
			dest->Tex[3].y = baseTCoord[7];
			break;
#endif

#if BURNING_MATERIAL_MAX_TEXTURES == 2
		case 0:
			dest->Tex[0].x = 0.f;
			dest->Tex[0].y = 0.f;
			dest->Tex[1].x = 0.f;
			dest->Tex[1].y = 0.f;
			break;

		case 1:
			dest->Tex[0].x = baseTCoord[0];
			dest->Tex[0].y = baseTCoord[1];
			dest->Tex[1].x = 0.f;
			dest->Tex[1].y = 0.f;
			break;
		case 2:
			dest->Tex[0].x = baseTCoord[0];
			dest->Tex[0].y = baseTCoord[1];
			dest->Tex[1].x = baseTCoord[2];
			dest->Tex[1].y = baseTCoord[3];
			break;
#endif
#if BURNING_MATERIAL_MAX_TEXTURES == 1
		case 0:
			dest->Tex[0].x = 0.f;
			dest->Tex[0].y = 0.f;
			break;
		case 1:
			dest->Tex[0].x = baseTCoord[0];
			dest->Tex[0].y = baseTCoord[1];
			break;
#endif
		default:
			break;
		}
	}
#if defined ( SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM )
	else
	{
	/*
			Generate texture coordinates as linear functions so that:
				u = Ux*x + Uy*y + Uz*z + Uw
				v = Vx*x + Vy*y + Vz*z + Vw
			The matrix M for this case is:
				Ux  Vx  0  0
				Uy  Vy  0  0
				Uz  Vz  0  0
				Uw  Vw  0  0
	*/

		const sVec4& u = EyeSpace.cam_dir;
		const sVec4& n = EyeSpace.normal;
		sVec4 r;

		const size_t* flag = TransformationFlag[TransformationStack];
		for ( u32 t = 0; t != VertexCache.vSize[VertexCache.vType].TexSize; ++t )
		{
			// texgen
			if (flag[ETS_TEXTURE_0+t] & ETF_TEXGEN_CAMERA_SPHERE )
			{
				//u.x = EyeSpace.vertex.x;
				//u.y = EyeSpace.vertex.y;
				//u.z = EyeSpace.vertex.z;
				//u.normalize_dir_xyz();

				//reflect(u,N) u - 2.0 * dot(N, u) * N
				// cam is (0,0,-1), tex flipped
				f32 dot = -2.f * n.dot_xyz(u);
				r.x = u.x + dot * n.x;
				r.y = u.y + dot * n.y;
				r.z = u.z + dot * n.z;

				//openGL
				f32 m = 2.f * sqrtf(r.x*r.x+r.y*r.y+(r.z+1.f)*(r.z+1.f));
				dest[0].Tex[t].x = r.x / m + 0.5f;
				dest[0].Tex[t].y = -r.y / m + 0.5f;

/*
				//~d3d with spheremap scale
				f32 m = 0.25f / (0.00001f + sqrtf(r.x*r.x+r.y*r.y+r.z*r.z));
				dest[0].Tex[t].x =  r.x * m + 0.5f;
				dest[0].Tex[t].y = -r.y * m + 0.5f;
*/
			}
			else if (flag[ETS_TEXTURE_0+t] & ETF_TEXGEN_CAMERA_REFLECTION )
			{
				//u.x = EyeSpace.vertex.x;
				//u.y = EyeSpace.vertex.y;
				//u.z = EyeSpace.vertex.z;
				//u.normalize_dir_xyz();

				//reflect(u,N) u - 2.0 * dot(N, u) * N
				// cam is (0,0,-1), tex flipped
				f32 dot = -2.f * n.dot_xyz(u);
				r.x = u.x + dot * n.x;
				r.y = u.y + dot * n.y;
				r.z = u.z + dot * n.z;

				//openGL
				dest[0].Tex[t].x = r.x;
				dest[0].Tex[t].y = -r.y;
/*
				//~d3d with spheremap scale
				dest[0].Tex[t].x = r.x;
				dest[0].Tex[t].y = r.y;
*/
			}
			else if (VertexCache.vSize[VertexCache.vType].TexCooSize > t)
			{
				const f32* M = matrix[ETS_TEXTURE_0 + t].pointer();

				// Irrlicht TCoords and TCoords2 must be contiguous memory. baseTCoord has no 4 byte aligned start address!
				const f32* baseTCoord = &base->TCoords.X;

				sVec4 srcT;
				srcT.x = baseTCoord[(t * 2) + 0];
				srcT.y = baseTCoord[(t * 2) + 1];

				switch ( Material.org.TextureLayer[t].TextureWrapU )
				{
					case ETC_CLAMP:
					case ETC_CLAMP_TO_EDGE:
					case ETC_CLAMP_TO_BORDER:
						dest->Tex[t].x = core::clamp ( (f32) ( M[0] * srcT.x + M[4] * srcT.y + M[8] ), 0.f, 1.f );
						break;
					case ETC_MIRROR:
						dest->Tex[t].x = M[0] * srcT.x + M[4] * srcT.y + M[8];
						if (core::fract(dest->Tex[t].x)>0.5f)
							dest->Tex[t].x=1.f-dest->Tex[t].x;
					break;
					case ETC_MIRROR_CLAMP:
					case ETC_MIRROR_CLAMP_TO_EDGE:
					case ETC_MIRROR_CLAMP_TO_BORDER:
						dest->Tex[t].x = core::clamp ( (f32) ( M[0] * srcT.x + M[4] * srcT.y + M[8] ), 0.f, 1.f );
						if (core::fract(dest->Tex[t].x)>0.5f)
							dest->Tex[t].x=1.f-dest->Tex[t].x;
					break;
					case ETC_REPEAT:
					default:
						dest->Tex[t].x = M[0] * srcT.x + M[4] * srcT.y + M[8];
						break;
				}
				switch ( Material.org.TextureLayer[t].TextureWrapV )
				{
					case ETC_CLAMP:
					case ETC_CLAMP_TO_EDGE:
					case ETC_CLAMP_TO_BORDER:
						dest->Tex[t].y = core::clamp ( (f32) ( M[1] * srcT.x + M[5] * srcT.y + M[9] ), 0.f, 1.f );
						break;
					case ETC_MIRROR:
						dest->Tex[t].y = M[1] * srcT.x + M[5] * srcT.y + M[9];
						if (core::fract(dest->Tex[t].y)>0.5f)
							dest->Tex[t].y=1.f-dest->Tex[t].y;
					break;
					case ETC_MIRROR_CLAMP:
					case ETC_MIRROR_CLAMP_TO_EDGE:
					case ETC_MIRROR_CLAMP_TO_BORDER:
						dest->Tex[t].y = core::clamp ( (f32) ( M[1] * srcT.x + M[5] * srcT.y + M[9] ), 0.f, 1.f );
						if (core::fract(dest->Tex[t].y)>0.5f)
							dest->Tex[t].y=1.f-dest->Tex[t].y;
					break;
					case ETC_REPEAT:
					default:
						dest->Tex[t].y = M[1] * srcT.x + M[5] * srcT.y + M[9];
						break;
				}
			}
		}
	}


#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
	if ((EyeSpace.TL_Flag & TL_LIGHT0_IS_NORMAL_MAP) &&
		((VertexCache.vSize[VertexCache.vType].Format & VERTEX4D_FORMAT_MASK_TANGENT) == VERTEX4D_FORMAT_BUMP_DOT3)
		)
	{
		const S3DVertexTangents *tangent = ((S3DVertexTangents*) source );

		sVec4 vp;
		sVec4 light_accu;
		light_accu.x = 0.f;
		light_accu.y = 0.f;
		light_accu.z = 0.f;
		light_accu.w = 0.f;
		for ( u32 i = 0; i < 2 && i < EyeSpace.Light.size (); ++i )
		{
			const SBurningShaderLight &light = EyeSpace.Light[i];

			if ( !light.LightIsOn )
				continue;

			vp.x = light.pos4.x - EyeSpace.vertex.x;
			vp.y = light.pos4.y - EyeSpace.vertex.y;
			vp.z = light.pos4.z - EyeSpace.vertex.z;
	
			//vp.normalize_dir_xyz();


			// transform by tangent matrix
			sVec4 l;
	#if 0
			l.x = (vp.x * tangent->Tangent.X + vp.y * tangent->Tangent.Y + vp.z * tangent->Tangent.Z );
			l.y = (vp.x * tangent->Binormal.X + vp.y * tangent->Binormal.Y + vp.z * tangent->Binormal.Z );
			l.z = (vp.x * tangent->Normal.X + vp.y * tangent->Normal.Y + vp.z * tangent->Normal.Z );
	#else
			l.x = (vp.x * tangent->Tangent.X + vp.y * tangent->Binormal.X + vp.z * tangent->Normal.X );
			l.y = (vp.x * tangent->Tangent.Y + vp.y * tangent->Binormal.Y + vp.z * tangent->Normal.Y );
			l.z = (vp.x * tangent->Tangent.Z + vp.y * tangent->Binormal.Z + vp.z * tangent->Normal.Z );
	#endif


	/*
			f32 scale = 1.f / 128.f;
			if ( dest->LightTangent[0].y != 0.f )
				scale /= dest->LightTangent[0].y;

			// emboss, shift coordinates
			dest->Tex[1].x = dest->Tex[0].x + l.r * scale;
			dest->Tex[1].y = dest->Tex[0].y + l.g * scale;
	*/
			dest->Tex[1].x = dest->Tex[0].x;
			dest->Tex[1].y = dest->Tex[0].y;

			// scale bias
			light_accu.x += l.x;
			light_accu.y += l.y;
			light_accu.z += l.z;
		}
		//normalize [-1,+1] to [0,1]
		light_accu.normalize_pack_xyz(dest->LightTangent[0],0.5f, 0.5f);
	}
	else if (Material.org.Lighting)
	{
		//dest->LightTangent[0].x = 0.f;
		//dest->LightTangent[0].y = 0.f;
		//dest->LightTangent[0].z = 0.f;
	}
#endif //if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0

#endif // SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM

clipandproject:

	// test vertex visible
	dest[0].flag = (u32) (clipToFrustumTest(dest) | VertexCache.vSize[VertexCache.vType].Format);
	dest[1].flag = dest[0].flag;

	// to DC Space, project homogenous vertex
	if ( (dest[0].flag & VERTEX4D_CLIPMASK ) == VERTEX4D_INSIDE )
	{
		ndc_2_dc_and_project ( dest+1, dest, s4DVertex_ofs(1));
	}

}


//todo: this should return only index
s4DVertexPair* CBurningVideoDriver::VertexCache_getVertex ( const u32 sourceIndex ) const
{
	for ( size_t i = 0; i < VERTEXCACHE_ELEMENT; ++i )
	{
		if ( VertexCache.info[ i ].index == sourceIndex )
		{
			return VertexCache.mem.data + s4DVertex_ofs(i);
		}
	}
	return VertexCache.mem.data; //error
}


/*
	Cache based on linear walk indices
	fill blockwise on the next 16(Cache_Size) unique vertices in indexlist
	merge the next 16 vertices with the current
*/
void CBurningVideoDriver::VertexCache_get(s4DVertexPair* face[4])
{
	// next primitive must be complete in cache
	if (	VertexCache.indicesIndex - VertexCache.indicesRun < VertexCache.primitiveHasVertex &&
			VertexCache.indicesIndex < VertexCache.indexCount
		)
	{

		size_t i;
		//memset(info, VERTEXCACHE_MISS, sizeof(info));
		for (i = 0; i != VERTEXCACHE_ELEMENT; ++i)
		{
			VertexCache.info_temp[i].hit = VERTEXCACHE_MISS;
			VertexCache.info_temp[i].index = VERTEXCACHE_MISS;
		}

		// rewind to start of primitive
		VertexCache.indicesIndex = VertexCache.indicesRun;


		// get the next unique vertices cache line
		u32 fillIndex = 0;
		u32 dIndex = 0;
		u32 sourceIndex = 0;

		while ( VertexCache.indicesIndex < VertexCache.indexCount &&
				fillIndex < VERTEXCACHE_ELEMENT
				)
		{
			switch ( VertexCache.iType )
			{
				case E4IT_16BIT:
					sourceIndex =  ((u16*)VertexCache.indices) [ VertexCache.indicesIndex ];
					break;
				case E4IT_32BIT:
					sourceIndex =  ((u32*)VertexCache.indices) [ VertexCache.indicesIndex ];
					break;
				default:
				case E4IT_NONE:
					sourceIndex = VertexCache.indicesIndex;
					break;
			}

			VertexCache.indicesIndex += 1;

			// if not exist, push back
			s32 exist = 0;
			for ( dIndex = 0;  dIndex < fillIndex; ++dIndex )
			{
				if (VertexCache.info_temp[ dIndex ].index == sourceIndex )
				{
					exist = 1;
					break;
				}
			}

			if ( 0 == exist )
			{
				VertexCache.info_temp[fillIndex++].index = sourceIndex;
			}
		}

		// clear marks
		for ( i = 0; i!= VERTEXCACHE_ELEMENT; ++i )
		{
			VertexCache.info[i].hit = 0;
		}

		// mark all existing
		for ( i = 0; i!= fillIndex; ++i )
		{
			for ( dIndex = 0;  dIndex < VERTEXCACHE_ELEMENT; ++dIndex )
			{
				if ( VertexCache.info[ dIndex ].index == VertexCache.info_temp[i].index )
				{
					VertexCache.info_temp[i].hit = dIndex;
					VertexCache.info[ dIndex ].hit = 1;
					break;
				}
			}
		}

		// fill new
		for ( i = 0; i!= fillIndex; ++i )
		{
			if (VertexCache.info_temp[i].hit != VERTEXCACHE_MISS )
				continue;

			for ( dIndex = 0;  dIndex < VERTEXCACHE_ELEMENT; ++dIndex )
			{
				if ( 0 == VertexCache.info[dIndex].hit )
				{
					VertexCache_fill (VertexCache.info_temp[i].index, dIndex );
					VertexCache.info[dIndex].hit += 1;
					VertexCache.info_temp[i].hit = dIndex;
					break;
				}
			}
		}
	}

	//const u32 i0 = core::if_c_a_else_0 ( VertexCache.pType != scene::EPT_TRIANGLE_FAN, VertexCache.indicesRun );
	const u32 i0 = VertexCache.pType != scene::EPT_TRIANGLE_FAN ? VertexCache.indicesRun : 0;

	switch ( VertexCache.iType )
	{
		case E4IT_16BIT:
		{
			const u16* p = (const u16*) VertexCache.indices;
			face[0] = VertexCache_getVertex ( p[ i0    ] );
			face[1] = VertexCache_getVertex ( p[ VertexCache.indicesRun + 1] );
			face[2] = VertexCache_getVertex ( p[ VertexCache.indicesRun + 2] );
		}
		break;

		case E4IT_32BIT:
		{
			const u32* p = (const u32*) VertexCache.indices;
			face[0] = VertexCache_getVertex ( p[ i0    ] );
			face[1] = VertexCache_getVertex ( p[ VertexCache.indicesRun + 1] );
			face[2] = VertexCache_getVertex ( p[ VertexCache.indicesRun + 2] );
		}
		break;

		case E4IT_NONE:
			face[0] = VertexCache_getVertex ( VertexCache.indicesRun + 0 );
			face[1] = VertexCache_getVertex ( VertexCache.indicesRun + 1 );
			face[2] = VertexCache_getVertex ( VertexCache.indicesRun + 2 );
		break;
		default:
			face[0] = face[1] = face[2] = VertexCache_getVertex(VertexCache.indicesRun + 0);
		break;
	}
	face[3] = face[0]; // quad unsupported
	VertexCache.indicesRun += VertexCache.indicesPitch;
}


/*!
*/
int CBurningVideoDriver::VertexCache_reset ( const void* vertices, u32 vertexCount,
											const void* indices, u32 primitiveCount,
											E_VERTEX_TYPE vType,
											scene::E_PRIMITIVE_TYPE pType,
											E_INDEX_TYPE iType)
{

	// These calls would lead to crashes due to wrong index usage.
	// The vertex cache needs to be rewritten for these primitives.
	if (0 == CurrentShader ||
		pType == scene::EPT_POINTS || pType == scene::EPT_LINE_STRIP ||
		pType == scene::EPT_LINE_LOOP || pType == scene::EPT_LINES ||
		pType == scene::EPT_POLYGON ||
		pType == scene::EPT_POINT_SPRITES)
	{
		return 1;
	}

	VertexCache.vertices = vertices;
	VertexCache.vertexCount = vertexCount;

	switch (Material.org.MaterialType) // (Material.Fallback_MaterialType)
	{
		case EMT_REFLECTION_2_LAYER:
		case EMT_TRANSPARENT_REFLECTION_2_LAYER:
			VertexCache.vType = E4VT_REFLECTION_MAP;
			break;
		default:
			VertexCache.vType = (e4DVertexType)vType;
			break;
	}

	//check material
	SVSize* vSize = VertexCache.vSize;
	for (int m = (int)vSize[VertexCache.vType].TexSize-1; m >= 0 ; --m)
	{
		ITexture* tex = MAT_TEXTURE(m);
		if (!tex)
		{
			vSize[E4VT_NO_TEXTURE] = vSize[VertexCache.vType];
			vSize[E4VT_NO_TEXTURE].TexSize = m;
			vSize[E4VT_NO_TEXTURE].TexCooSize = m;
			VertexCache.vType = E4VT_NO_TEXTURE;
			//flags downconvert?
		}
	}

	VertexCache.indices = indices;
	VertexCache.indicesIndex = 0;
	VertexCache.indicesRun = 0;

	switch ( iType )
	{
		case EIT_16BIT: VertexCache.iType = E4IT_16BIT; break;
		case EIT_32BIT: VertexCache.iType = E4IT_32BIT; break;
		default:
			VertexCache.iType = (e4DIndexType)iType; break;
	}
	if (!VertexCache.indices)
		VertexCache.iType = E4IT_NONE;

	VertexCache.pType = pType;
	VertexCache.primitiveHasVertex = 3;
	VertexCache.indicesPitch = 1;
	switch ( VertexCache.pType )
	{
		// most types here will not work as expected, only triangles/triangle_fan
		// is known to work.
		case scene::EPT_POINTS:
			VertexCache.indexCount = primitiveCount;
			VertexCache.indicesPitch = 1;
			VertexCache.primitiveHasVertex = 1;
			break;
		case scene::EPT_LINE_STRIP:
			VertexCache.indexCount = primitiveCount+1;
			VertexCache.indicesPitch = 1;
			VertexCache.primitiveHasVertex = 2;
			break;
		case scene::EPT_LINE_LOOP:
			VertexCache.indexCount = primitiveCount+1;
			VertexCache.indicesPitch = 1;
			VertexCache.primitiveHasVertex = 2;
			break;
		case scene::EPT_LINES:
			VertexCache.indexCount = 2*primitiveCount;
			VertexCache.indicesPitch = 2;
			VertexCache.primitiveHasVertex = 2;
			break;
		case scene::EPT_TRIANGLE_STRIP:
			VertexCache.indexCount = primitiveCount+2;
			VertexCache.indicesPitch = 1;
			VertexCache.primitiveHasVertex = 3;
			break;
		case scene::EPT_TRIANGLES:
			VertexCache.indexCount = primitiveCount + primitiveCount + primitiveCount;
			VertexCache.indicesPitch = 3;
			VertexCache.primitiveHasVertex = 3;
			break;
		case scene::EPT_TRIANGLE_FAN:
			VertexCache.indexCount = primitiveCount + 2;
			VertexCache.indicesPitch = 1;
			VertexCache.primitiveHasVertex = 3;
			break;
		case scene::EPT_QUAD_STRIP:
			VertexCache.indexCount = 2*primitiveCount + 2;
			VertexCache.indicesPitch = 2;
			VertexCache.primitiveHasVertex = 4;
			break;
		case scene::EPT_QUADS:
			VertexCache.indexCount = 4*primitiveCount;
			VertexCache.indicesPitch = 4;
			VertexCache.primitiveHasVertex = 4;
			break;
		case scene::EPT_POLYGON:
			VertexCache.indexCount = primitiveCount+1;
			VertexCache.indicesPitch = 1;
			VertexCache.primitiveHasVertex = primitiveCount;
			break;
		case scene::EPT_POINT_SPRITES:
			VertexCache.indexCount = primitiveCount;
			VertexCache.indicesPitch = 1;
			VertexCache.primitiveHasVertex = 1;
			break;
	}

	//memset( VertexCache.info, VERTEXCACHE_MISS, sizeof ( VertexCache.info ) );
	for (size_t i = 0; i != VERTEXCACHE_ELEMENT; ++i)
	{
		VertexCache.info[i].hit = VERTEXCACHE_MISS;
		VertexCache.info[i].index = VERTEXCACHE_MISS;
	}
	return 0;
}


//! draws a vertex primitive list
void CBurningVideoDriver::drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
				const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)

{
	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::drawVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType);

	if (VertexCache_reset(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType))
		return;

	if ((u32)Material.org.MaterialType < MaterialRenderers.size())
	{
		MaterialRenderers[Material.org.MaterialType].Renderer->OnRender(this, vType);
	}

	//Matrices needed for this primitive
	transform_calc(ETS_PROJ_MODEL_VIEW);
	if ( Material.org.Lighting || (EyeSpace.TL_Flag & (TL_TEXTURE_TRANSFORM | TL_FOG)) )
	{
		transform_calc(ETS_MODEL_VIEW);
		transform_calc(ETS_NORMAL);
	}


	s4DVertexPair* face[4];

	size_t vOut;
	size_t vertex_from_clipper; // from VertexCache or CurrentOut
	size_t has_vertex_run;

	for ( size_t primitive_run = 0; primitive_run < primitiveCount; ++primitive_run)
	{
		//collect pointer to face vertices
		VertexCache_get(face);

		register size_t clipMask_i;
		register size_t clipMask_o;

		clipMask_i = face[0]->flag;
		clipMask_o = face[0]->flag;

		for (has_vertex_run = 1; has_vertex_run < VertexCache.primitiveHasVertex; ++has_vertex_run)
		{
			clipMask_i |= face[has_vertex_run]->flag; // if fully outside or outside on same side
			clipMask_o &= face[has_vertex_run]->flag; // if fully inside
		}

		clipMask_i &= VERTEX4D_CLIPMASK;
		clipMask_o &= VERTEX4D_CLIPMASK;

		if (clipMask_i != VERTEX4D_INSIDE )
		{
			// if primitive fully outside or outside on same side
			continue;
			vOut = 0;
			vertex_from_clipper = 0;
		}
		else if (clipMask_o == VERTEX4D_INSIDE )
		{
			// if primitive fully inside
			vOut = VertexCache.primitiveHasVertex;
			vertex_from_clipper = 0;
		}
		else
#if defined(SOFTWARE_DRIVER_2_CLIPPING)
		{
			// else if not complete inside clipping necessary
			// check: clipping should reuse vertexcache (try to minimize clipping)
			for (has_vertex_run = 0; has_vertex_run < VertexCache.primitiveHasVertex; ++has_vertex_run)
			{
				memcpy_s4DVertexPair(Clipper.data + s4DVertex_ofs(has_vertex_run), face[has_vertex_run]);
			}

			vOut = clipToFrustum(VertexCache.primitiveHasVertex);
			vertex_from_clipper = 1;

			// to DC Space, project homogenous vertex
			ndc_2_dc_and_project(Clipper.data + s4DVertex_proj(0), Clipper.data, s4DVertex_ofs(vOut));
		}
#else
		{
			continue;
			vOut = 0;
			vertex_from_clipper = 0;
		}
#endif
		// re-tesselate ( triangle-fan, 0-1-2,0-2-3.. )
		for (has_vertex_run = 0; (has_vertex_run + VertexCache.primitiveHasVertex) <= vOut; has_vertex_run += 1)
		{
			// set from clipped geometry
			if (vertex_from_clipper)
			{
				face[0] = Clipper.data + s4DVertex_ofs(0);
				face[1] = Clipper.data + s4DVertex_ofs(has_vertex_run + 1);
				face[2] = Clipper.data + s4DVertex_ofs(has_vertex_run + 2);
			}

			//area of primitive in device space
			f32 dc_area = screenarea_inside(face);

			//geometric clipping has problem with invisible or very small Triangles
			//size_t sign = dc_area < 0.001f ? CULL_BACK : dc_area > 0.001f ? CULL_FRONT : CULL_INVISIBLE;
			ieee754 t;
			t.f = dc_area;
			size_t sign = t.fields.sign ? CULL_BACK : CULL_FRONT;
			sign |= t.abs.frac_exp < 981668463 /*0.01f*/ ? CULL_INVISIBLE : 0;
			if (Material.CullFlag & sign)
				break; //continue;

			//select mipmap ratio between drawing space and texture space
			dc_area = reciprocal_zero(dc_area);

			// select mipmap
			for (size_t m = 0; m < VertexCache.vSize[VertexCache.vType].TexSize; ++m)
			{
				//only guessing: take more detail (lower mipmap) in light+bump textures
				//assume transparent add is ~50% transparent -> more detail
				f32 lod_bias = Material.org.MaterialType == EMT_TRANSPARENT_ADD_COLOR ? 0.1f : 0.33f;
				s32 lodFactor = lodFactor_inside(face, m, dc_area, lod_bias);
				

				video::CSoftwareTexture2* tex = MAT_TEXTURE(m);
				CurrentShader->setTextureParam(m, tex, lodFactor);
				//currently shader receives texture coordinate as Pixelcoo of 1 Texture
				select_polygon_mipmap_inside(face, m, tex->getTexBound());
			}

			CurrentShader->drawWireFrameTriangle(face[0] + 1, face[1] + 1, face[2] + 1);
			vertex_from_clipper = 1;
		}

	}

	//release texture
	for (size_t m = 0; m < VertexCache.vSize[VertexCache.vType].TexSize; ++m)
	{
		CurrentShader->setTextureParam(m, 0, 0);
	}

}


//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void CBurningVideoDriver::setAmbientLight(const SColorf& color)
{
	EyeSpace.Global_AmbientLight.setColorf ( color );
}


//! adds a dynamic light
s32 CBurningVideoDriver::addDynamicLight(const SLight& dl)
{
	(void) CNullDriver::addDynamicLight( dl );

	SBurningShaderLight l;
//	l.org = dl;
	l.Type = dl.Type;
	l.LightIsOn = true;

	l.AmbientColor.setColorf ( dl.AmbientColor );
	l.DiffuseColor.setColorf ( dl.DiffuseColor );
	l.SpecularColor.setColorf ( dl.SpecularColor );

	//should always be valid?
	sVec4 nDirection;
	nDirection.x = dl.Direction.X;
	nDirection.y = dl.Direction.Y;
	nDirection.z = dl.Direction.Z;
	nDirection.normalize_dir_xyz();


	switch ( dl.Type )
	{
		case ELT_DIRECTIONAL:
			l.pos.x = dl.Position.X;
			l.pos.y = dl.Position.Y;
			l.pos.z = dl.Position.Z;
			l.pos.w = 0.f;

			l.constantAttenuation = 1.f;
			l.linearAttenuation = 0.f;
			l.quadraticAttenuation = 0.f;

			l.spotDirection.x = -nDirection.x;
			l.spotDirection.y = -nDirection.y;
			l.spotDirection.z = -nDirection.z;
			l.spotDirection.w = 0.f;
			l.spotCosCutoff = -1.f;
			l.spotCosInnerCutoff = 1.f;
			l.spotExponent = 0.f;
			break;

		case ELT_POINT:
			l.pos.x = dl.Position.X;
			l.pos.y = dl.Position.Y;
			l.pos.z = dl.Position.Z;
			l.pos.w = 1.f;

			l.constantAttenuation = dl.Attenuation.X;
			l.linearAttenuation = dl.Attenuation.Y;
			l.quadraticAttenuation = dl.Attenuation.Z;

			l.spotDirection.x = -nDirection.x;
			l.spotDirection.y = -nDirection.y;
			l.spotDirection.z = -nDirection.z;
			l.spotDirection.w = 0.f;
			l.spotCosCutoff = -1.f;
			l.spotCosInnerCutoff = 1.f;
			l.spotExponent = 0.f;
			break;

		case ELT_SPOT:
			l.pos.x = dl.Position.X;
			l.pos.y = dl.Position.Y;
			l.pos.z = dl.Position.Z;
			l.pos.w = 1.f;

			l.constantAttenuation = dl.Attenuation.X;
			l.linearAttenuation = dl.Attenuation.Y;
			l.quadraticAttenuation = dl.Attenuation.Z;

			l.spotDirection.x = nDirection.x;
			l.spotDirection.y = nDirection.y;
			l.spotDirection.z = nDirection.z;
			l.spotDirection.w = 0.0f;
			l.spotCosCutoff = cosf(dl.OuterCone * 2.0f * core::DEGTORAD * 0.5f);
			l.spotCosInnerCutoff = cosf(dl.InnerCone * 2.0f * core::DEGTORAD * 0.5f);
			l.spotExponent = dl.Falloff;
			break;
		default:
			break;
	}

	//which means ETS_VIEW
	setTransform(ETS_WORLD,irr::core::IdentityMatrix);
	transform_calc(ETS_MODEL_VIEW);

	const core::matrix4* matrix = Transformation[TransformationStack];
	transformVec4Vec4(matrix[ETS_MODEL_VIEW], &l.pos4.x, &l.pos.x );
	rotateVec3Vec4(matrix[ETS_MODEL_VIEW], &l.spotDirection4.x, &l.spotDirection.x );

	EyeSpace.Light.push_back ( l );
	return EyeSpace.Light.size() - 1;
}

#if 0
/*!
	Camera Position in World Space
*/
void CBurningVideoDriver::getCameraPosWorldSpace()
{
	transform_calc(ETS_VIEW_INVERSE);

	const f32 *M = Transformation[ETS_VIEW_INVERSE].pointer();

	/*	The  viewpoint is at (0., 0., 0.) in eye space.
		Turning this into a vector [0 0 0 1] and multiply it by
		the inverse of the view matrix, the resulting vector is the
		object space location of the camera.
	*/

	EyeSpace.camworldpos.x = M[12];
	EyeSpace.camworldpos.y = M[13];
	EyeSpace.camworldpos.z = M[14];
	EyeSpace.camworldpos.w = 1.f;
}
#endif

//! Turns a dynamic light on or off
void CBurningVideoDriver::turnLightOn(s32 lightIndex, bool turnOn)
{
	if((u32)lightIndex < EyeSpace.Light.size())
	{
		EyeSpace.Light[lightIndex].LightIsOn = turnOn;
	}
}

//! deletes all dynamic lights there are
void CBurningVideoDriver::deleteAllDynamicLights()
{
	EyeSpace.reset ();
	CNullDriver::deleteAllDynamicLights();

}

//! returns the maximal amount of dynamic lights the device can handle
u32 CBurningVideoDriver::getMaximalDynamicLightAmount() const
{
	return 8; //no limit 8 only for convenience
}


//! sets a material
void CBurningVideoDriver::setMaterial(const SMaterial& material)
{
// ---------- Override
	Material.org = material;
	OverrideMaterial.apply(Material.org);

	const SMaterial& in = Material.org;

// ---------- Notify Shader
	// unset old material
	u32 mi;
	mi = (u32)Material.lastMaterial.MaterialType;
	if (mi != material.MaterialType && mi < MaterialRenderers.size())
		MaterialRenderers[mi].Renderer->OnUnsetMaterial();

	// set new material.
	mi = (u32)in.MaterialType;
	if (mi < MaterialRenderers.size())
		MaterialRenderers[mi].Renderer->OnSetMaterial(
			in, Material.lastMaterial, Material.resetRenderStates, this);

	Material.lastMaterial = in;
	Material.resetRenderStates = false;

	//CSoftware2MaterialRenderer sets Material.Fallback_MaterialType

	//Material.Fallback_MaterialType = material.MaterialType;

//-----------------
	
	//Material.org = material;
	Material.CullFlag = CULL_INVISIBLE | (in.BackfaceCulling ? CULL_BACK : 0) | (in.FrontfaceCulling ? CULL_FRONT : 0);

	size_t* flag = TransformationFlag[TransformationStack];

#ifdef SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
	for (u32 m = 0; m < BURNING_MATERIAL_MAX_TEXTURES /*&& m < vSize[VertexCache.vType].TexSize*/; ++m)
	{
		setTransform((E_TRANSFORMATION_STATE) (ETS_TEXTURE_0 + m),in.getTextureMatrix(m));
		flag[ETS_TEXTURE_0+m] &= ~ETF_TEXGEN_MASK;
	}
#endif

#ifdef SOFTWARE_DRIVER_2_LIGHTING

	Material.AmbientColor.setA8R8G8B8( in.AmbientColor.color );
	Material.DiffuseColor.setA8R8G8B8( in.ColorMaterial ? 0xFFFFFFFF : in.DiffuseColor.color );
	Material.EmissiveColor.setA8R8G8B8(in.EmissiveColor.color );
	Material.SpecularColor.setA8R8G8B8( in.SpecularColor.color );

	burning_setbit(EyeSpace.TL_Flag, in.Lighting, TL_LIGHT);
	burning_setbit( EyeSpace.TL_Flag, (in.Shininess != 0.f) & (in.SpecularColor.color & 0x00ffffff), TL_SPECULAR );
	burning_setbit( EyeSpace.TL_Flag, in.FogEnable, TL_FOG );
	burning_setbit( EyeSpace.TL_Flag, in.NormalizeNormals, TL_NORMALIZE_NORMALS );
	//if (EyeSpace.Flags & SPECULAR ) EyeSpace.Flags |= NORMALIZE_NORMALS;

#endif

	//--------------- setCurrentShader
	
	ITexture* texture0 = in.getTexture(0);
	ITexture* texture1 = in.getTexture(1);
	//ITexture* texture2 = in.getTexture(2);
	//ITexture* texture3 = in.getTexture(3);

	//visual studio code analysis
	u32 maxTex = BURNING_MATERIAL_MAX_TEXTURES;
	if (maxTex < 1) texture0 = 0;
	if (maxTex < 2) texture1 = 0;
	//if (maxTex < 3) texture2 = 0;
	//if (maxTex < 4) texture3 = 0;

	EyeSpace.TL_Flag &= ~(TL_TEXTURE_TRANSFORM | TL_LIGHT0_IS_NORMAL_MAP);

	//todo: seperate depth test from depth write
	Material.depth_write = getWriteZBuffer(in);
	Material.depth_test = in.ZBuffer != ECFN_DISABLED && Material.depth_write;

	EBurningFFShader shader = Material.depth_test ? ETR_TEXTURE_GOURAUD : ETR_TEXTURE_GOURAUD_NOZ;

	switch (Material.Fallback_MaterialType) //(Material.org.MaterialType)
	{
	case EMT_ONETEXTURE_BLEND:
		shader = ETR_TEXTURE_BLEND;
		break;

	case EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
		Material.org.MaterialTypeParam = 0.5f;
		//fallthrough

	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		if (texture0 && texture0->hasAlpha())
		{
			shader = Material.depth_test ? ETR_TEXTURE_GOURAUD_ALPHA : ETR_TEXTURE_GOURAUD_ALPHA_NOZ;
		}
		else
		{
			//fall back to EMT_TRANSPARENT_VERTEX_ALPHA
			shader = ETR_TEXTURE_GOURAUD_VERTEX_ALPHA;
		}
		break;

	case EMT_TRANSPARENT_ADD_COLOR:
		shader = Material.depth_test ? ETR_TEXTURE_GOURAUD_ADD : ETR_TEXTURE_GOURAUD_ADD_NO_Z;
		break;

	case EMT_TRANSPARENT_VERTEX_ALPHA:
		shader = ETR_TEXTURE_GOURAUD_VERTEX_ALPHA;
		break;

	case EMT_LIGHTMAP:
	case EMT_LIGHTMAP_LIGHTING:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_LIGHTMAP_M1;
		break;

	case EMT_LIGHTMAP_M2:
	case EMT_LIGHTMAP_LIGHTING_M2:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_LIGHTMAP_M2;
		break;

	case EMT_LIGHTMAP_LIGHTING_M4:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_LIGHTMAP_M4;
		break;
	case EMT_LIGHTMAP_M4:
		if (texture1)
			shader = ETR_TEXTURE_LIGHTMAP_M4;
		break;

	case EMT_LIGHTMAP_ADD:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_LIGHTMAP_ADD;
		break;

	case EMT_DETAIL_MAP:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_DETAIL_MAP;
		break;

	case EMT_SPHERE_MAP:
		flag[ETS_TEXTURE_0] |= ETF_TEXGEN_CAMERA_SPHERE;
		EyeSpace.TL_Flag |= TL_TEXTURE_TRANSFORM;
		break;
	case EMT_REFLECTION_2_LAYER:
	case EMT_TRANSPARENT_REFLECTION_2_LAYER:
		if (texture1)
		{
			shader = ETR_TRANSPARENT_REFLECTION_2_LAYER;
			flag[ETS_TEXTURE_1] |= ETF_TEXGEN_CAMERA_REFLECTION;
			EyeSpace.TL_Flag |= TL_TEXTURE_TRANSFORM;
		}
		break;

	case EMT_NORMAL_MAP_SOLID:
	case EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR:
	case EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA:
		if (texture1)
		{
			shader = ETR_NORMAL_MAP_SOLID;
			EyeSpace.TL_Flag |= TL_TEXTURE_TRANSFORM | TL_LIGHT0_IS_NORMAL_MAP;
		}
		break;
	case EMT_PARALLAX_MAP_SOLID:
	case EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
	case EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
		if (texture1)
		{
			shader = ETR_NORMAL_MAP_SOLID;
			EyeSpace.TL_Flag |= TL_TEXTURE_TRANSFORM | TL_LIGHT0_IS_NORMAL_MAP;
		}
		break;

	default:
		break;

	}

	if (!texture0)
	{
		shader = Material.depth_test ? ETR_GOURAUD :
			shader == ETR_TEXTURE_GOURAUD_VERTEX_ALPHA ?
			ETR_GOURAUD_ALPHA_NOZ: // 2D Gradient
			ETR_GOURAUD_NOZ;

		shader = ETR_COLOR;
	}

	if (in.Wireframe)
	{
		IBurningShader* candidate = BurningShader[shader];
		if (!candidate || (candidate && !candidate->canWireFrame()))
		{
			shader = ETR_TEXTURE_GOURAUD_WIRE;
		}
	}

	if (in.PointCloud)
	{
		IBurningShader* candidate = BurningShader[shader];
		if (!candidate || (candidate && !candidate->canPointCloud()))
		{
			shader = ETR_TEXTURE_GOURAUD_WIRE;
		}
	}

	//shader = ETR_REFERENCE;

	// switchToTriangleRenderer
	CurrentShader = BurningShader[shader];
	if (CurrentShader)
	{
		CurrentShader->setTLFlag(EyeSpace.TL_Flag);
		if (EyeSpace.TL_Flag & TL_FOG) CurrentShader->setFog(FogColor);
		if (EyeSpace.TL_Flag & TL_SCISSOR) CurrentShader->setScissor(Scissor);
		CurrentShader->setRenderTarget(RenderTargetSurface, ViewPort);
		CurrentShader->OnSetMaterial(Material);
		CurrentShader->pushEdgeTest(in.Wireframe, in.PointCloud, 0);
	}


/*
	mi = (u32)Material.org.MaterialType;
	if (mi < MaterialRenderers.size())
		MaterialRenderers[mi].Renderer->OnRender(this, (video::E_VERTEX_TYPE)VertexCache.vType);
*/
}




//! Sets the fog mode.
void CBurningVideoDriver::setFog(SColor color, E_FOG_TYPE fogType, f32 start,
	f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	CNullDriver::setFog(color, fogType, start, end, density, pixelFog, rangeFog);

	EyeSpace.fog_scale = reciprocal_zero2(FogEnd - FogStart);
}



#if defined(SOFTWARE_DRIVER_2_LIGHTING) && BURNING_MATERIAL_MAX_COLORS > 0


/*!
	applies lighting model
*/
void CBurningVideoDriver::lightVertex_eye(s4DVertex *dest, u32 vertexargb)
{
	//gl_FrontLightModelProduct.sceneColor = gl_FrontMaterial.emission + gl_FrontMaterial.ambient * gl_LightModel.ambient

	sVec3Color ambient;
	sVec3Color diffuse;
	sVec3Color specular;


	// the universe started in darkness..
	ambient = EyeSpace.Global_AmbientLight;
	diffuse.set(0.f);
	specular.set(0.f);


	u32 i;
	f32 dot;
	f32 distance;
	f32 attenuation;
	sVec4 vp;			// unit vector vertex to light
	sVec4 lightHalf;	// blinn-phong reflection

	f32 spotDot;			// cos of angle between spotlight and point on surface

	for (i = 0; i < EyeSpace.Light.size(); ++i)
	{
		const SBurningShaderLight &light = EyeSpace.Light[i];

		if (!light.LightIsOn)
			continue;

		switch (light.Type)
		{
		case ELT_DIRECTIONAL:

			//angle between normal and light vector
			dot = EyeSpace.normal.dot_xyz(light.spotDirection4);

			// accumulate ambient
			ambient.add_rgb(light.AmbientColor);

			// diffuse component
			if (dot > 0.f)
				diffuse.mad_rgb(light.DiffuseColor, dot);
			break;

		case ELT_POINT:
			// surface to light
			vp.x = light.pos4.x - EyeSpace.vertex.x;
			vp.y = light.pos4.y - EyeSpace.vertex.y;
			vp.z = light.pos4.z - EyeSpace.vertex.z;

			distance = vp.length_xyz();

			attenuation = light.constantAttenuation
				+ light.linearAttenuation * distance
				+ light.quadraticAttenuation * (distance * distance);
			attenuation = reciprocal_one(attenuation);

			//att = clamp(1.0 - dist/radius, 0.0, 1.0); att *= att

			// accumulate ambient
			ambient.mad_rgb(light.AmbientColor, attenuation);

			// build diffuse reflection

			//angle between normal and light vector
			vp.mul_xyz(reciprocal_zero(distance)); //normalize
			dot = EyeSpace.normal.dot_xyz(vp);
			if (dot <= 0.f) continue;

			// diffuse component
			diffuse.mad_rgb(light.DiffuseColor, dot * attenuation);

			if (!(EyeSpace.TL_Flag & TL_SPECULAR))
				continue;

			lightHalf.x = vp.x + 0.f; // EyeSpace.cam_eye_pos.x;
			lightHalf.y = vp.y + 0.f; // EyeSpace.cam_eye_pos.y;
			lightHalf.z = vp.z - 1.f; // EyeSpace.cam_eye_pos.z;
			lightHalf.normalize_dir_xyz();

			//specular += light.SpecularColor * pow(max(dot(Eyespace.normal,lighthalf),0,Material.org.Shininess)*attenuation
			specular.mad_rgb(light.SpecularColor,
				powf_limit(EyeSpace.normal.dot_xyz(lightHalf), Material.org.Shininess)*attenuation
			);
			break;

		case ELT_SPOT:
			// surface to light
			vp.x = light.pos4.x - EyeSpace.vertex.x;
			vp.y = light.pos4.y - EyeSpace.vertex.y;
			vp.z = light.pos4.z - EyeSpace.vertex.z;

			distance = vp.length_xyz();

			//normalize
			vp.mul_xyz(reciprocal_zero(distance));

			// point on surface inside cone of illumination
			spotDot = vp.dot_minus_xyz(light.spotDirection4);
			if (spotDot < light.spotCosCutoff)
				continue;

			attenuation = light.constantAttenuation
				+ light.linearAttenuation * distance
				+ light.quadraticAttenuation * distance * distance;
			attenuation = reciprocal_one(attenuation);
			attenuation *= powf_limit(spotDot, light.spotExponent);

			// accumulate ambient
			ambient.mad_rgb(light.AmbientColor, attenuation);


			// build diffuse reflection
			//angle between normal and light vector
			dot = EyeSpace.normal.dot_xyz(vp);
			if (dot < 0.f) continue;

			// diffuse component
			diffuse.mad_rgb(light.DiffuseColor, dot * attenuation);

			if (!(EyeSpace.TL_Flag & TL_SPECULAR))
				continue;

			lightHalf.x = vp.x + 0.f; // EyeSpace.cam_eye_pos.x;
			lightHalf.y = vp.y + 0.f; // EyeSpace.cam_eye_pos.y;
			lightHalf.z = vp.z - 1.f; // EyeSpace.cam_eye_pos.z;
			lightHalf.normalize_dir_xyz();

			//specular += light.SpecularColor * pow(max(dot(Eyespace.normal,lighthalf),0,Material.org.Shininess)*attenuation
			specular.mad_rgb(light.SpecularColor,
				powf_limit(EyeSpace.normal.dot_xyz(lightHalf), Material.org.Shininess)*attenuation
			);
			break;

		default:
			break;
		}

	}

	// sum up lights
	sVec3Color dColor;
	dColor.set(0.f);
	dColor.mad_rgbv(diffuse, Material.DiffuseColor);

	//diffuse * vertex color.
	//has to move to shader (for vertex color only this will fit [except clamping])

	sVec3Color c;
	c.setA8R8G8B8(vertexargb);
	dColor.r *= c.r;
	dColor.g *= c.g;
	dColor.b *= c.b;

	//separate specular
#if defined(SOFTWARE_DRIVER_2_USE_SEPARATE_SPECULAR_COLOR)
	if ((VertexCache.vSize[VertexCache.vType].Format & VERTEX4D_FORMAT_COLOR_2_FOG))
	{
		specular.sat_xyz(dest->Color[1], Material.SpecularColor);
	}
	else
	if ( !(EyeSpace.TL_Flag & TL_LIGHT0_IS_NORMAL_MAP) &&
		 (VertexCache.vSize[VertexCache.vType].Format & VERTEX4D_FORMAT_MASK_LIGHT)
		)
	{
		specular.sat_xyz(dest->LightTangent[0], Material.SpecularColor);
	}
	else
#endif
	{
		dColor.mad_rgbv(specular, Material.SpecularColor);
	}


	dColor.mad_rgbv(ambient, Material.AmbientColor);
	dColor.add_rgb(Material.EmissiveColor);


	dColor.sat(dest->Color[0], vertexargb);
}

#endif

CImage* getImage(const video::ITexture* texture)
{
	if (!texture) return 0;

	CImage* img = 0;
	switch (texture->getDriverType())
	{
	case EDT_BURNINGSVIDEO:
		img = ((CSoftwareTexture2*)texture)->getImage();
		break;
	case EDT_SOFTWARE:
		img = ((CSoftwareTexture*)texture)->getImage();
		break;
	default:
		os::Printer::log("Fatal Error: Tried to copy from a surface not owned by this driver.", ELL_ERROR);
		break;
	}
	return img;
}

/*
	draw2DImage with single color scales into destination quad & cliprect(more like viewport)
	draw2DImage with 4 color scales on destination and cliprect is scissor
*/

static const u16 quad_triangle_indexList[6] = { 0,1,2,0,2,3 };


#if defined(SOFTWARE_DRIVER_2_2D_AS_2D)

//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void CBurningVideoDriver::draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
	const core::rect<s32>& sourceRect,
	const core::rect<s32>* clipRect, SColor color,
	bool useAlphaChannelOfTexture)
{
	if (texture)
	{
		if (texture->getOriginalSize() != texture->getSize())
		{
			core::rect<s32> destRect(destPos, sourceRect.getSize());
			SColor c4[4] = { color,color,color,color };
			draw2DImage(texture, destRect, sourceRect, clipRect, c4, useAlphaChannelOfTexture);
			return;
		}

		if (texture->getDriverType() != EDT_BURNINGSVIDEO)
		{
			os::Printer::log("Fatal Error: Tried to copy from a surface not owned by this driver.", ELL_ERROR);
			return;
		}

		if (useAlphaChannelOfTexture)
			((CSoftwareTexture2*)texture)->getImage()->copyToWithAlpha(
				RenderTargetSurface, destPos, sourceRect, color, clipRect);
		else
			((CSoftwareTexture2*)texture)->getImage()->copyTo(
				RenderTargetSurface, destPos, sourceRect, clipRect);
	}
}


//! Draws a part of the texture into the rectangle.
void CBurningVideoDriver::draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
	const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
	const video::SColor* const colors, bool useAlphaChannelOfTexture)
{
	if (texture)
	{
		if (texture->getDriverType() != EDT_BURNINGSVIDEO)
		{
			os::Printer::log("Fatal Error: Tried to copy from a surface not owned by this driver.", ELL_ERROR);
			return;
		}

		u32 argb = (colors ? colors[0].color : 0xFFFFFFFF);
		eBlitter op = useAlphaChannelOfTexture ?
			(argb == 0xFFFFFFFF ? BLITTER_TEXTURE_ALPHA_BLEND : BLITTER_TEXTURE_ALPHA_COLOR_BLEND) : BLITTER_TEXTURE;

		StretchBlit(op, RenderTargetSurface, clipRect, &destRect,
			((CSoftwareTexture2*)texture)->getImage(), &sourceRect, &texture->getOriginalSize(), argb);

	}
}

//!Draws an 2d rectangle with a gradient.
void CBurningVideoDriver::draw2DRectangle(const core::rect<s32>& position,
	SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
	const core::rect<s32>* clip)
{
	core::rect<s32> p(position);
	if (clip) p.clipAgainst(*clip);
	if (p.isValid()) drawRectangle(RenderTargetSurface, p, colorLeftUp);
}

#endif //defined(SOFTWARE_DRIVER_2_2D_AS_2D)





#if 0
void transform_for_BlitJob2D( SBlitJob& out,
	const S3DVertex quad2DVertices[4], const core::dimension2d<u32>& renderTargetSize,
	CBurningVideoDriver* driver
)
{
	//assume. z = 0.f, w = 1.f
	//MVP 2D
	core::matrix4 m;
	m.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0f, 1.0f);
	m.setTranslation(core::vector3df(-1.f, 1.f, 0));

	//setTransform(ETS_WORLD, m);
	//m.setTranslation(core::vector3df(0.375f, 0.375f, 0.0f));
	//setTransform(ETS_VIEW, m);

	s4DVertexPair v[4*sizeof_s4DVertexPairRel];
	for (int i = 0; i < 4; ++i)
	{
		m.transformVect(&v[s4DVertex_ofs(i)].Pos.x, quad2DVertices[i].Pos);
		v[s4DVertex_ofs(i)].Tex[0].x = quad2DVertices[i].TCoords.X;
		v[s4DVertex_ofs(i)].Tex[0].y = quad2DVertices[i].TCoords.Y;
		v[s4DVertex_ofs(i)].Color[0].setA8R8G8B8(quad2DVertices[i].Color.color);
		v[s4DVertex_ofs(i)].flag = VERTEX4D_FORMAT_TEXTURE_1 | VERTEX4D_FORMAT_COLOR_1;
		v[s4DVertex_ofs(i)].flag |= clipToFrustumTest(v + i);
	}

	size_t vOut = driver->clipToFrustum(4);

	struct s2DVertex
	{
		union
		{
			struct {
				f32 x, y;
				f32 u, v;
				f32 a, r, g, b;
			};
			f32 attr[8];
		};

		// f = a * t + b * ( 1 - t )
		void interpolate(const s2DVertex& b, const s2DVertex& a, const f32 t)
		{
			for (int i = 0; i < 8; ++i)
			{
				attr[i] = b.attr[i] + ((a.attr[i] - b.attr[i]) * t);
			}
		}

	};

	// 0 5
	f32 m2[2];
	m2[0] = renderTargetSize.Width ? 2.f / renderTargetSize.Width : 0.f;
	m2[1] = renderTargetSize.Height ? -2.f / renderTargetSize.Height : 0.f;

	s2DVertex p[4];
	for (int i = 0; i < 4; ++i)
	{
		p[i].x = quad2DVertices[i].Pos.X*m2[0] - 1.f;
		p[i].y = quad2DVertices[i].Pos.Y*m2[1] + 1.f;
		p[i].u = quad2DVertices[i].TCoords.X;
		p[i].v = quad2DVertices[i].TCoords.Y;

		p[i].a = quad2DVertices[i].Color.getAlpha() * (1.f / 255.f);
		p[i].r = quad2DVertices[i].Color.getRed() * (1.f / 255.f);
		p[i].g = quad2DVertices[i].Color.getBlue() * (1.f / 255.f);
		p[i].b = quad2DVertices[i].Color.getGreen() * (1.f / 255.f);

	}

}
#endif


//! Enable the 2d override material
void CBurningVideoDriver::enableMaterial2D(bool enable)
{
	CNullDriver::enableMaterial2D(enable);
	burning_setbit(TransformationFlag[1][ETS_PROJECTION], 0, ETF_VALID);
}

size_t compare_2d_material(const SMaterial& a, const SMaterial& b)
{
	size_t flag = 0;
	flag |= a.MaterialType == b.MaterialType ? 0 : 1;
	flag |= a.ZBuffer == b.ZBuffer ? 0 : 2;
	flag |= a.TextureLayer[0].Texture == b.TextureLayer[0].Texture ? 0 : 4;
	flag |= a.TextureLayer[0].BilinearFilter == b.TextureLayer[0].BilinearFilter ? 0 : 8;
	flag |= a.MaterialTypeParam == b.MaterialTypeParam ? 0 : 16;
	if (flag) return flag;

	flag |= a.TextureLayer[1].Texture == b.TextureLayer[1].Texture ? 0 : 32;
	flag |= a.ZWriteEnable == b.ZWriteEnable ? 0 : 64;

	return flag;
}

void CBurningVideoDriver::setRenderStates2DMode(const video::SColor& color, video::ITexture* texture, bool useAlphaChannelOfTexture)
{
	//save current 3D Material
	//Material.save3D = Material.org;

	//build new 2D Material

	bool vertexAlpha = color.getAlpha() < 255;

	//2D uses textureAlpa*vertexAlpha 3D not..
	if (useAlphaChannelOfTexture && texture && texture->hasAlpha())
	{
		Material.mat2D.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
	}
	else if (vertexAlpha)
	{
		Material.mat2D.MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA;
	}
	else
	{
		Material.mat2D.MaterialType = EMT_SOLID;
	}

	Material.mat2D.ZBuffer = ECFN_DISABLED;
	Material.mat2D.ZWriteEnable = EZW_OFF;
	Material.mat2D.Lighting = false;

	Material.mat2D.setTexture(0, (video::ITexture*)texture);

	//used for text. so stay as sharp as possible (like HW Driver)
	bool mip = false;

	const SMaterial& currentMaterial = (!OverrideMaterial2DEnabled) ? InitMaterial2D : OverrideMaterial2D;
	mip = currentMaterial.TextureLayer[0].BilinearFilter;

	Material.mat2D.setFlag(video::EMF_BILINEAR_FILTER, mip);


	//switch to 2D Matrix Stack [ Material set Texture Matrix ]
	TransformationStack = 1;

	//2D GUI Matrix
	if (!(TransformationFlag[TransformationStack][ETS_PROJECTION] & ETF_VALID))
	{
		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
		core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
		m.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0f, 1.0f);
		m.setTranslation(core::vector3df(-1.f, 1.f, 0));
		setTransform(ETS_PROJECTION, m);

		m.makeIdentity();
		setTransform(ETS_WORLD, m);

		if ( mip ) m.setTranslation(core::vector3df(0.375f, 0.375f, 0.0f));

		setTransform(ETS_VIEW, m);

		buildNDCToDCMatrix(Transformation_ETS_CLIPSCALE[TransformationStack], ViewPort, mip ? -0.187f : 0.f);

	}

	//compare
	if (compare_2d_material(Material.org,Material.mat2D))
	{
		setMaterial(Material.mat2D);
	}
	if (CurrentShader)
	{
		CurrentShader->setPrimitiveColor(color.color);
	}
	
}

void CBurningVideoDriver::setRenderStates3DMode()
{
	//restoreRenderStates3DMode

	//setMaterial(Material.save3D);
	//switch to 3D Matrix Stack
	TransformationStack = 0;
}

//! draws a vertex primitive list in 2d
void CBurningVideoDriver::draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount,
	const void* indexList, u32 primitiveCount,
	E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::draw2DVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType);

	bool useAlphaChannelOfTexture = false;
	video::SColor color(0xFFFFFFFF);
	switch (Material.org.MaterialType)
	{
	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		useAlphaChannelOfTexture = true;
		break;
	case EMT_TRANSPARENT_VERTEX_ALPHA:
		color.setAlpha(127);
		break;
	default:
		break;
	}
	setRenderStates2DMode(color, Material.org.getTexture(0), useAlphaChannelOfTexture);

	drawVertexPrimitiveList(vertices, vertexCount,
		indexList, primitiveCount,
		vType, pType, iType);

	setRenderStates3DMode();

}

//setup a quad
#if defined(SOFTWARE_DRIVER_2_2D_AS_3D)

//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void CBurningVideoDriver::draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
	const core::rect<s32>& sourceRect,
	const core::rect<s32>* clipRect, SColor color,
	bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	if (!sourceRect.isValid())
		return;

	// clip these coordinates
	core::rect<s32> targetRect(destPos, sourceRect.getSize());
	if (clipRect)
	{
		targetRect.clipAgainst(*clipRect);
		if (targetRect.getWidth() < 0 || targetRect.getHeight() < 0)
			return;
	}

	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
	targetRect.clipAgainst(core::rect<s32>(0, 0, (s32)renderTargetSize.Width, (s32)renderTargetSize.Height));
	if (targetRect.getWidth() < 0 || targetRect.getHeight() < 0)
		return;

	// ok, we've clipped everything.
	// now draw it.
	const core::dimension2d<s32> sourceSize(targetRect.getSize());
	const core::position2d<s32> sourcePos(sourceRect.UpperLeftCorner + (targetRect.UpperLeftCorner - destPos));

	const core::dimension2d<u32>& tex_orgsize = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(tex_orgsize.Width);
	const f32 invH = 1.f / static_cast<f32>(tex_orgsize.Height);
	const core::rect<f32> tcoords(
		sourcePos.X * invW,
		sourcePos.Y * invH,
		(sourcePos.X + sourceSize.Width) * invW,
		(sourcePos.Y + sourceSize.Height) * invH);


	Quad2DVertices[0].Color = color;
	Quad2DVertices[1].Color = color;
	Quad2DVertices[2].Color = color;
	Quad2DVertices[3].Color = color;

	Quad2DVertices[0].Pos = core::vector3df((f32)targetRect.UpperLeftCorner.X, (f32)targetRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[1].Pos = core::vector3df((f32)targetRect.LowerRightCorner.X, (f32)targetRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[2].Pos = core::vector3df((f32)targetRect.LowerRightCorner.X, (f32)targetRect.LowerRightCorner.Y, 0.0f);
	Quad2DVertices[3].Pos = core::vector3df((f32)targetRect.UpperLeftCorner.X, (f32)targetRect.LowerRightCorner.Y, 0.0f);

	Quad2DVertices[0].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[1].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[2].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	Quad2DVertices[3].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

	//SBlitJob job;
	//transform_for_BlitJob2D(job, Quad2DVertices, renderTargetSize,this);

	setRenderStates2DMode(color, (video::ITexture*) texture, useAlphaChannelOfTexture);

	drawVertexPrimitiveList(Quad2DVertices, 4,
		quad_triangle_indexList, 2,
		EVT_STANDARD, scene::EPT_TRIANGLES, EIT_16BIT);

	setRenderStates3DMode();

}


//! Draws a part of the texture into the rectangle.
void CBurningVideoDriver::draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
	const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
	const video::SColor* const colors, bool useAlphaChannelOfTexture)
{
	const core::dimension2d<u32>& st = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(st.Width);
	const f32 invH = 1.f / static_cast<f32>(st.Height);
	const core::rect<f32> tcoords(
		sourceRect.UpperLeftCorner.X * invW,
		sourceRect.UpperLeftCorner.Y * invH,
		sourceRect.LowerRightCorner.X * invW,
		sourceRect.LowerRightCorner.Y *invH);

	const video::SColor temp[4] =
	{
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF
	};

	const video::SColor* const useColor = colors ? colors : temp;


	Quad2DVertices[0].Color = useColor[0];
	Quad2DVertices[1].Color = useColor[3];
	Quad2DVertices[2].Color = useColor[2];
	Quad2DVertices[3].Color = useColor[1];

	Quad2DVertices[0].Pos = core::vector3df((f32)destRect.UpperLeftCorner.X, (f32)destRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[1].Pos = core::vector3df((f32)destRect.LowerRightCorner.X, (f32)destRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[2].Pos = core::vector3df((f32)destRect.LowerRightCorner.X, (f32)destRect.LowerRightCorner.Y, 0.0f);
	Quad2DVertices[3].Pos = core::vector3df((f32)destRect.UpperLeftCorner.X, (f32)destRect.LowerRightCorner.Y, 0.0f);

	Quad2DVertices[0].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[1].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[2].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	Quad2DVertices[3].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);


	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

	if (clipRect)
	{
		if (!clipRect->isValid())
			return;

		//glEnable(GL_SCISSOR_TEST);
		EyeSpace.TL_Flag |= TL_SCISSOR;
		setScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height - clipRect->LowerRightCorner.Y,
			clipRect->getWidth(), clipRect->getHeight());
	}

	video::SColor alphaTest;
	alphaTest.color = useColor[0].color & useColor[0].color & useColor[0].color & useColor[0].color;

	//SBlitJob job;
	//transform_for_BlitJob2D(job, Quad2DVertices, renderTargetSize, this);

	setRenderStates2DMode(alphaTest, (video::ITexture*) texture, useAlphaChannelOfTexture);

	drawVertexPrimitiveList(Quad2DVertices, 4,
		quad_triangle_indexList, 2,
		EVT_STANDARD, scene::EPT_TRIANGLES, EIT_16BIT);

	
	if (clipRect)
		EyeSpace.TL_Flag &= ~TL_SCISSOR;

	setRenderStates3DMode();


}


//!Draws an 2d rectangle with a gradient.
void CBurningVideoDriver::draw2DRectangle(const core::rect<s32>& position,
	SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
	const core::rect<s32>* clip)
{
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	Quad2DVertices[0].Color = colorLeftUp;
	Quad2DVertices[1].Color = colorRightUp;
	Quad2DVertices[2].Color = colorRightDown;
	Quad2DVertices[3].Color = colorLeftDown;

	Quad2DVertices[0].Pos = core::vector3df((f32)pos.UpperLeftCorner.X, (f32)pos.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[1].Pos = core::vector3df((f32)pos.LowerRightCorner.X, (f32)pos.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[2].Pos = core::vector3df((f32)pos.LowerRightCorner.X, (f32)pos.LowerRightCorner.Y, 0.0f);
	Quad2DVertices[3].Pos = core::vector3df((f32)pos.UpperLeftCorner.X, (f32)pos.LowerRightCorner.Y, 0.0f);

	Quad2DVertices[0].TCoords.X = 0.f;
	Quad2DVertices[0].TCoords.Y = 0.f;
	Quad2DVertices[1].TCoords.X = 0.f;
	Quad2DVertices[1].TCoords.Y = 0.f;
	Quad2DVertices[2].TCoords.X = 0.f;
	Quad2DVertices[3].TCoords.Y = 0.f;
	Quad2DVertices[3].TCoords.X = 0.f;
	Quad2DVertices[3].TCoords.Y = 0.f;


	video::SColor alphaTest;
	alphaTest.color = colorLeftUp.color & colorRightUp.color & colorRightDown.color & colorLeftDown.color;
	setRenderStates2DMode(alphaTest, 0, 0);

	drawVertexPrimitiveList(Quad2DVertices, 4,
		quad_triangle_indexList, 2,
		EVT_STANDARD, scene::EPT_TRIANGLES, EIT_16BIT);

	setRenderStates3DMode();

}


#endif // SOFTWARE_DRIVER_2_2D_AS_3D





//! Draws a 2d line.
void CBurningVideoDriver::draw2DLine(const core::position2d<s32>& start,
					const core::position2d<s32>& end,
					SColor color)
{
	drawLine(RenderTargetSurface, start, end, color );
}


//! Draws a pixel
void CBurningVideoDriver::drawPixel(u32 x, u32 y, const SColor & color)
{
	RenderTargetSurface->setPixel(x, y, color, true);
}


//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void CBurningVideoDriver::OnResize(const core::dimension2d<u32>& size)
{
	// make sure width and height are multiples of 2
	core::dimension2d<u32> realSize(size);
/*
	if (realSize.Width % 2)
		realSize.Width += 1;

	if (realSize.Height % 2)
		realSize.Height += 1;
*/
	if (ScreenSize != realSize)
	{
		if (ViewPort.getWidth() == (s32)ScreenSize.Width &&
			ViewPort.getHeight() == (s32)ScreenSize.Height)
		{
			ViewPort.UpperLeftCorner.X = 0;
			ViewPort.UpperLeftCorner.Y = 0;
			ViewPort.LowerRightCorner.X = realSize.Width;
			ViewPort.LowerRightCorner.X = realSize.Height;
		}

		ScreenSize = realSize;

		bool resetRT = (RenderTargetSurface == BackBuffer);

		if (BackBuffer)
			BackBuffer->drop();
		BackBuffer = new CImage(BURNINGSHADER_COLOR_FORMAT, realSize);

		if (resetRT)
			setRenderTargetImage(BackBuffer);
	}
}


//! returns the current render target size
const core::dimension2d<u32>& CBurningVideoDriver::getCurrentRenderTargetSize() const
{
	return RenderTargetSize;
}




//! Draws a 3d line.
void CBurningVideoDriver::draw3DLine(const core::vector3df& start,
	const core::vector3df& end, SColor color_start)
{
	SColor color_end = color_start;
	
	VertexCache.primitiveHasVertex = 2;
	VertexCache.vType = E4VT_LINE;

	s4DVertex* v = Clipper.data;

	transform_calc(ETS_PROJ_MODEL_VIEW);
	const core::matrix4* matrix = Transformation[TransformationStack];
	matrix[ETS_PROJ_MODEL_VIEW].transformVect ( &v[0].Pos.x, start );
	matrix[ETS_PROJ_MODEL_VIEW].transformVect ( &v[2].Pos.x, end );

#if BURNING_MATERIAL_MAX_COLORS > 0
	v[0].Color[0].setA8R8G8B8 ( color_start.color );
	v[2].Color[0].setA8R8G8B8 ( color_end.color );
#endif

	u32 i;
	for (i = 0; i < 4; i += sizeof_s4DVertexPairRel)
	{
		v[i + 0].flag = (u32)(VertexCache.vSize[VertexCache.vType].Format);
		v[i + 1].flag = v[i + 0].flag;
	}


	size_t g;
	size_t vOut;

	// vertices count per line
	vOut = clipToFrustum (VertexCache.primitiveHasVertex);
	if ( vOut < VertexCache.primitiveHasVertex)
		return;

	vOut *= sizeof_s4DVertexPairRel;

	// to DC Space, project homogenous vertex
	ndc_2_dc_and_project ( v + 1, v, vOut );

	// unproject vertex color
#if 0
#if BURNING_MATERIAL_MAX_COLORS > 0
	for ( g = 0; g != vOut; g+= 2 )
	{
		v[ g + 1].Color[0].setA8R8G8B8 ( color.color );
	}
#endif
#endif

	IBurningShader * shader = 0;
	if ( CurrentShader && CurrentShader->canWireFrame() ) shader = CurrentShader;
	else shader = BurningShader [ ETR_TEXTURE_GOURAUD_WIRE ];
	shader = BurningShader [ ETR_TEXTURE_GOURAUD_WIRE ];

	shader->pushEdgeTest(1,0,1);
	shader->setRenderTarget(RenderTargetSurface, ViewPort);

	for ( g = 0; g <= vOut - 4; g += sizeof_s4DVertexPairRel)
	{
		shader->drawLine ( v + 1 + g, v + 1 + g + sizeof_s4DVertexPairRel);
	}

	shader->popEdgeTest();

}


//! \return Returns the name of the video driver. Example: In case of the DirectX8
//! driver, it would return "Direct3D8.1".
const wchar_t* CBurningVideoDriver::getName() const
{
#ifdef BURNINGVIDEO_RENDERER_BEAUTIFUL
	return L"Burning's Video 0.50 beautiful";
#elif defined ( BURNINGVIDEO_RENDERER_ULTRA_FAST )
	return L"Burning's Video 0.50 ultra fast";
#elif defined ( BURNINGVIDEO_RENDERER_FAST )
	return L"Burning's Video 0.50 fast";
#elif defined ( BURNINGVIDEO_RENDERER_CE )
	return L"Burning's Video 0.50 CE";
#else
	return L"Burning's Video 0.50";
#endif
}

//! Returns the graphics card vendor name.
core::stringc CBurningVideoDriver::getVendorInfo()
{
	return "Burning's Video: Ing. Thomas Alten (c) 2006-2020";
}


//! Returns type of video driver
E_DRIVER_TYPE CBurningVideoDriver::getDriverType() const
{
	return EDT_BURNINGSVIDEO;
}


//! returns color format
ECOLOR_FORMAT CBurningVideoDriver::getColorFormat() const
{
	return BURNINGSHADER_COLOR_FORMAT;
}


//! Creates a render target texture.
ITexture* CBurningVideoDriver::addRenderTargetTexture(const core::dimension2d<u32>& size,
		const io::path& name, const ECOLOR_FORMAT format)
{
	IImage* img = createImage(BURNINGSHADER_COLOR_FORMAT, size);
	ITexture* tex = new CSoftwareTexture2(img, name, CSoftwareTexture2::IS_RENDERTARGET,this);
	img->drop();
	addTexture(tex);
	tex->drop();
	return tex;
}

void CBurningVideoDriver::clearBuffers(u16 flag, SColor color, f32 depth, u8 stencil)
{
	if ((flag & ECBF_COLOR) && RenderTargetSurface)
		RenderTargetSurface->fill(color);

	if ((flag & ECBF_DEPTH) && DepthBuffer)
		DepthBuffer->clear(depth);

	if ((flag & ECBF_STENCIL) && StencilBuffer)
		StencilBuffer->clear(stencil);
}

#if 0
void CBurningVideoDriver::saveBuffer()
{
	static int shotCount = 0;
	char buf[256];
	if (BackBuffer)
	{
		sprintf(buf, "shot/%04d_b.png", shotCount);
		writeImageToFile(BackBuffer, buf);
	}
	if (StencilBuffer)
	{
		CImage stencil(ECF_A8R8G8B8, StencilBuffer->getSize(), StencilBuffer->lock(), true, false);
		sprintf(buf, "shot/%04d_s.ppm", shotCount);
		writeImageToFile(&stencil, buf);
	}
	shotCount += 1;
}
#endif

//! Returns an image created from the last rendered frame.
IImage* CBurningVideoDriver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
{
	if (target != video::ERT_FRAME_BUFFER)
		return 0;

	if (BackBuffer)
	{
		IImage* tmp = createImage(BackBuffer->getColorFormat(), BackBuffer->getDimension());
		BackBuffer->copyTo(tmp);
		return tmp;
	}
	else
		return 0;
}

ITexture* CBurningVideoDriver::createDeviceDependentTexture(const io::path& name, IImage* image)
{
	CSoftwareTexture2* texture = new CSoftwareTexture2(image, name,
		(getTextureCreationFlag(ETCF_CREATE_MIP_MAPS) ? CSoftwareTexture2::GEN_MIPMAP : 0) |
		(getTextureCreationFlag(ETCF_ALLOW_NON_POWER_2) ? CSoftwareTexture2::ALLOW_NPOT : 0)
#if defined(IRRLICHT_sRGB)
		| (getTextureCreationFlag(ETCF_IMAGE_IS_LINEAR) ? CSoftwareTexture2::IMAGE_IS_LINEAR : 0)
		| (getTextureCreationFlag(ETCF_TEXTURE_IS_LINEAR) ? CSoftwareTexture2::TEXTURE_IS_LINEAR : 0)
#endif
		,this
		);

	return texture;
}

ITexture* CBurningVideoDriver::createDeviceDependentTextureCubemap(const io::path& name, const core::array<IImage*>& image)
{
	return 0;
}

//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
u32 CBurningVideoDriver::getMaximalPrimitiveCount() const
{
	return 0x7FFFFFFF;
}


//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: First, draw all geometry. Then use this method, to draw the shadow
//! volume. Next use IVideoDriver::drawStencilShadow() to visualize the shadow.
void CBurningVideoDriver::drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail, u32 debugDataVisible)
{
	const u32 count = triangles.size();
	if (!StencilBuffer || !count)
		return;

	Material.org.MaterialType = video::EMT_SOLID;
	Material.org.Lighting = false;
	Material.org.ZWriteEnable = video::EZW_OFF;
	Material.org.ZBuffer = ECFN_LESS;

	CurrentShader = BurningShader[ETR_STENCIL_SHADOW];

	CurrentShader->setRenderTarget(RenderTargetSurface, ViewPort);
	CurrentShader->pushEdgeTest(Material.org.Wireframe, 0,0);

	//setMaterial
	EyeSpace.TL_Flag &= ~(TL_TEXTURE_TRANSFORM | TL_LIGHT0_IS_NORMAL_MAP);
	CurrentShader->setTLFlag(EyeSpace.TL_Flag);
	//glStencilMask(~0);
	//glStencilFunc(GL_ALWAYS, 0, ~0);

	//glEnable(GL_DEPTH_CLAMP);

	if (zfail)
	{
		Material.org.BackfaceCulling = false;
		Material.org.FrontfaceCulling = true;
		Material.CullFlag = CULL_FRONT | CULL_INVISIBLE;

		CurrentShader->setStencilOp( StencilOp_KEEP, StencilOp_INCR, StencilOp_KEEP);
		drawVertexPrimitiveList (triangles.const_pointer(), count, 0, count/3, (video::E_VERTEX_TYPE) E4VT_SHADOW, scene::EPT_TRIANGLES, (video::E_INDEX_TYPE) E4IT_NONE);

		Material.org.BackfaceCulling = true;
		Material.org.FrontfaceCulling = false;
		Material.CullFlag = CULL_BACK | CULL_INVISIBLE;

		CurrentShader->setStencilOp( StencilOp_KEEP, StencilOp_DECR, StencilOp_KEEP);
		drawVertexPrimitiveList (triangles.const_pointer(), count, 0, count/3, (video::E_VERTEX_TYPE) E4VT_SHADOW, scene::EPT_TRIANGLES, (video::E_INDEX_TYPE) E4IT_NONE);
	}
	else // zpass
	{
		Material.org.BackfaceCulling = true;
		Material.org.FrontfaceCulling = false;
		Material.CullFlag = CULL_BACK | CULL_INVISIBLE;

		CurrentShader->setStencilOp( StencilOp_KEEP, StencilOp_KEEP, StencilOp_INCR);
		drawVertexPrimitiveList(triangles.const_pointer(), count, 0, count / 3, (video::E_VERTEX_TYPE) E4VT_SHADOW, scene::EPT_TRIANGLES, (video::E_INDEX_TYPE) E4IT_NONE);

		Material.org.BackfaceCulling = false;
		Material.org.FrontfaceCulling = true;
		Material.CullFlag = CULL_FRONT | CULL_INVISIBLE;

		CurrentShader->setStencilOp(StencilOp_KEEP, StencilOp_KEEP, StencilOp_DECR);
		drawVertexPrimitiveList(triangles.const_pointer(), count, 0, count / 3, (video::E_VERTEX_TYPE) E4VT_SHADOW, scene::EPT_TRIANGLES, (video::E_INDEX_TYPE) E4IT_NONE);
	}
	//glDisable(GL_DEPTH_CLAMP);

}

//! Fills the stencil shadow with color. After the shadow volume has been drawn
//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
//! to draw the color of the shadow.
void CBurningVideoDriver::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
	video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;
	// draw a shadow rectangle covering the entire screen using stencil buffer
	const u32 h = RenderTargetSurface->getDimension().Height;
	const u32 w = RenderTargetSurface->getDimension().Width;
	tVideoSample *dst;
	const tStencilSample* stencil;

#if defined(SOFTWARE_DRIVER_2_32BIT)
	const u32 alpha = extractAlpha(leftUpEdge.color);
	const u32 src = leftUpEdge.color;
#else
	const u16 alpha = extractAlpha( leftUpEdge.color ) >> 3;
	const u32 src = video::A8R8G8B8toA1R5G5B5( leftUpEdge.color );
#endif


	for ( u32 y = 0; y < h; ++y )
	{
		dst = (tVideoSample*)RenderTargetSurface->getData() + ( y * w );
		stencil = (tStencilSample*)StencilBuffer->lock() + (y * w);

		for ( u32 x = 0; x < w; ++x )
		{
			if ( stencil[x] )
			{
#if defined(SOFTWARE_DRIVER_2_32BIT)
				dst[x] = PixelBlend32 ( dst[x], src,alpha );
#else
				dst[x] = PixelBlend16( dst[x], src, alpha );
#endif
			}
		}
	}

	if ( clearStencilBuffer )
		StencilBuffer->clear(0);
}


core::dimension2du CBurningVideoDriver::getMaxTextureSize() const
{
	return core::dimension2du(SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE ? SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE : 1 << 20,
		SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE ? SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE : 1 << 20);
}

bool CBurningVideoDriver::queryTextureFormat(ECOLOR_FORMAT format) const
{
	return format == BURNINGSHADER_COLOR_FORMAT;
}

bool CBurningVideoDriver::needsTransparentRenderPass(const irr::video::SMaterial& material) const
{
	return CNullDriver::needsTransparentRenderPass(material) || material.isAlphaBlendOperation(); // || material.isTransparent();
}

s32 CBurningVideoDriver::addShaderMaterial(const c8* vertexShaderProgram,
	const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData)
{
	s32 materialID = -1;

	IBurningShader* shader = new IBurningShader(
		this, materialID,
		vertexShaderProgram, 0, video::EVST_VS_1_1,
		pixelShaderProgram, 0, video::EPST_PS_1_1,
		0, 0, EGST_GS_4_0,
		scene::EPT_TRIANGLES, scene::EPT_TRIANGLE_STRIP, 0,
		callback, baseMaterial, userData);

	shader->drop();

	return materialID;
}

//! Adds a new material renderer to the VideoDriver, based on a high level shading language.
s32 CBurningVideoDriver::addHighLevelShaderMaterial(
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
	s32 userData
	)
{
	s32 materialID = -1;

	IBurningShader* shader = new IBurningShader(
		this, materialID,
		vertexShaderProgram, vertexShaderEntryPointName, vsCompileTarget,
		pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
		geometryShaderProgram, geometryShaderEntryPointName, gsCompileTarget,
		inType, outType, verticesOut,
		callback, baseMaterial, userData);

	shader->drop();

	return materialID;
}


void CBurningVideoDriver::setFallback_Material(E_MATERIAL_TYPE fallback_MaterialType)
{
	//this should be in material....
	Material.Fallback_MaterialType = fallback_MaterialType;
}

void CBurningVideoDriver::setBasicRenderStates(const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates)
{
	
}

//! Return an index constant for the vertex shader based on a name.
s32 CBurningVideoDriver::getVertexShaderConstantID(const c8* name)
{
	return -1;
}

bool CBurningVideoDriver::setVertexShaderConstant(s32 index, const f32* floats, int count)
{
	return true;
}

bool CBurningVideoDriver::setVertexShaderConstant(s32 index, const s32* ints, int count)
{
	return true;
}

bool CBurningVideoDriver::setVertexShaderConstant(s32 index, const u32* ints, int count)
{
	return true;
}

void CBurningVideoDriver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
}

//! Return an index constant for the pixel shader based on a name.
s32 CBurningVideoDriver::getPixelShaderConstantID(const c8* name)
{
	return -1;
}

bool CBurningVideoDriver::setPixelShaderConstant(s32 index, const f32* floats, int count)
{
	return false;
}

bool CBurningVideoDriver::setPixelShaderConstant(s32 index, const s32* ints, int count)
{
	return false;
}

bool CBurningVideoDriver::setPixelShaderConstant(s32 index, const u32* ints, int count)
{
	return false;
}

void CBurningVideoDriver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1)
{
}

//! Get pointer to the IVideoDriver interface
/** \return Pointer to the IVideoDriver interface */
IVideoDriver* CBurningVideoDriver::getVideoDriver()
{
	return this;
}

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_


namespace irr
{
namespace video
{

//! creates a video driver
IVideoDriver* createBurningVideoDriver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, video::IImagePresenter* presenter)
{
	#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_
		return new CBurningVideoDriver(params, io, presenter);
	#else
		return 0;
	#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_
}



} // end namespace video
} // end namespace irr

