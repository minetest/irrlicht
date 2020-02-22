// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_BURNING_SHADER_H_INCLUDED__
#define __I_BURNING_SHADER_H_INCLUDED__

#include "SoftwareDriver2_compile_config.h"
#include "IReferenceCounted.h"
#include "irrMath.h"
#include "irrMathFastCompat.h"
#include "IImage.h"
#include "S2DVertex.h"
#include "rect.h"
#include "CDepthBuffer.h"
#include "S4DVertex.h"
#include "irrArray.h"
#include "SLight.h"
#include "SMaterial.h"
#include "os.h"
#include "IMaterialRenderer.h"
#include "IMaterialRendererServices.h"
#include "IGPUProgrammingServices.h"

namespace irr
{

namespace video
{

	struct SBurningShaderLight
	{
		//SLight org;

		sVec4 pos;	//light position input
		sVec4 pos4; //light position Model*View (Identity*View)

		E_LIGHT_TYPE Type;
		f32 linearAttenuation;
		f32 constantAttenuation;
		f32 quadraticAttenuation;

		sVec4 spotDirection;
		sVec4 spotDirection4;
		f32 spotCosCutoff;
		f32 spotCosInnerCutoff;
		f32 spotExponent;
		bool LightIsOn;

		sVec3Color AmbientColor;
		sVec3Color DiffuseColor;
		sVec3Color SpecularColor;
	};

	enum eTransformLightFlags
	{
		//ENABLED		= 0x01,
		TL_SCISSOR				= 0x02,
		TL_LIGHT				= 0x04,
		TL_SPECULAR				= 0x08,
		TL_FOG					= 0x10,
		TL_NORMALIZE_NORMALS	= 0x20,
		TL_TEXTURE_TRANSFORM	= 0x40,
		TL_LIGHT_LOCAL_VIEWER	= 0x80,
		TL_LIGHT0_IS_NORMAL_MAP	= 0x100		//sVec4 Light Vector is used as normal or specular
	};

	struct SBurningShaderEyeSpace
	{
		SBurningShaderEyeSpace() {}
		virtual ~SBurningShaderEyeSpace() {}
		void reset ()
		{
			Light.set_used ( 0 );
			Global_AmbientLight.set ( 0.f );

			TL_Flag = TL_LIGHT_LOCAL_VIEWER;
		}
		void resetFog()
		{
			fog_scale = 0.f;
			//cam_distance = 0.f;
		}

		core::array<SBurningShaderLight> Light;
		sVec3Color Global_AmbientLight;

		//sVec4 cam_eye_pos; //Camera Position in eye Space (0,0,-1)
		//sVec4 cam_world_pos; //Camera Position in world Space
		//sVec4 vertex4; //eye coordinate position of vertex
		sVec4 normal; //transformed normal
		sVec4 vertex; //eye coordinate position of vertex projected

		//derivative of vertex
		//f32 cam_distance; // vertex.length();
		sVec4 cam_dir; //vertex.normalize();

		f32 fog_scale; // 1 / (fog.end-fog.start)

		size_t TL_Flag; // eTransformLightFlags
	};

	enum eBurningCullFlag
	{
		CULL_FRONT = 1,
		CULL_BACK = 2,
		CULL_INVISIBLE = 4,	//primitive smaller than a pixel (AreaMinDrawSize)
		CULL_FRONT_AND_BACK = 8,
	};

	enum eBurningStencilOp
	{
		StencilOp_KEEP = 0x1E00,
		StencilOp_INCR = 0x1E02,
		StencilOp_DECR = 0x1E03
	};

	struct SBurningShaderMaterial
	{
		SMaterial org;
		SMaterial lastMaterial;
		bool resetRenderStates;

		E_MATERIAL_TYPE Fallback_MaterialType;

		SMaterial mat2D;
		//SMaterial save3D;

		size_t CullFlag; //eCullFlag
		u32 depth_write;
		u32 depth_test;

		sVec3Color AmbientColor;
		sVec3Color DiffuseColor;
		sVec3Color SpecularColor;
		sVec3Color EmissiveColor;

	};

	enum EBurningFFShader
	{
		ETR_FLAT = 0,
		ETR_FLAT_WIRE,
		ETR_GOURAUD,
		ETR_GOURAUD_WIRE,
		ETR_TEXTURE_FLAT,
		ETR_TEXTURE_FLAT_WIRE,
		ETR_TEXTURE_GOURAUD,
		ETR_TEXTURE_GOURAUD_WIRE,
		ETR_TEXTURE_GOURAUD_NOZ,
		ETR_TEXTURE_GOURAUD_ADD,
		ETR_TEXTURE_GOURAUD_ADD_NO_Z,

		ETR_TEXTURE_GOURAUD_VERTEX_ALPHA,

		ETR_TEXTURE_GOURAUD_LIGHTMAP_M1,
		ETR_TEXTURE_GOURAUD_LIGHTMAP_M2,
		ETR_TEXTURE_GOURAUD_LIGHTMAP_M4,
		ETR_TEXTURE_LIGHTMAP_M4,

		ETR_TEXTURE_GOURAUD_DETAIL_MAP,
		ETR_TEXTURE_GOURAUD_LIGHTMAP_ADD,

		ETR_GOURAUD_NOZ,
		//ETR_GOURAUD_ALPHA,
		ETR_GOURAUD_ALPHA_NOZ,

		ETR_TEXTURE_GOURAUD_ALPHA,
		ETR_TEXTURE_GOURAUD_ALPHA_NOZ,
		ETR_TEXTURE_GOURAUD_ALPHA_NOZ_NOPERSPECTIVE_CORRECT,

		ETR_NORMAL_MAP_SOLID,
		ETR_STENCIL_SHADOW,

		ETR_TEXTURE_BLEND,
		ETR_TRANSPARENT_REFLECTION_2_LAYER,

		ETR_COLOR,

		//ETR_REFERENCE,
		ETR_INVALID,

		ETR2_COUNT
	};

	typedef enum
	{
		BL_VERTEX_PROGRAM = 1,
		BL_FRAGMENT_PROGRAM = 2,
		BL_TYPE_FLOAT = 4,
		BL_TYPE_INT = 8,
		BL_TYPE_UINT = 16,

		BL_VERTEX_FLOAT = (BL_VERTEX_PROGRAM | BL_TYPE_FLOAT),
		BL_VERTEX_INT = (BL_VERTEX_PROGRAM | BL_TYPE_INT),
		BL_VERTEX_UINT = (BL_VERTEX_PROGRAM | BL_TYPE_UINT),
		BL_FRAGMENT_FLOAT = (BL_FRAGMENT_PROGRAM | BL_TYPE_FLOAT),
		BL_FRAGMENT_INT = (BL_FRAGMENT_PROGRAM | BL_TYPE_INT),
		BL_FRAGMENT_UINT = (BL_FRAGMENT_PROGRAM | BL_TYPE_UINT),

		BL_ACTIVE_UNIFORM_MAX_LENGTH = 28
	} EBurningUniformFlags;

	struct BurningUniform
	{
		c8 name[BL_ACTIVE_UNIFORM_MAX_LENGTH];
		u32 type; //EBurningUniformFlags
		//int location; // UniformLocation is index
		f32 data[16];	// simple LocalParameter

		bool operator==(const BurningUniform& other) const
		{
			return tiny_istoken(name, other.name);
		}

	};


	class CBurningVideoDriver;
	class IBurningShader : public IMaterialRenderer, public IMaterialRendererServices
	{
	public:
		//! Constructor
		IBurningShader(CBurningVideoDriver* driver);

		//! Constructor
		IBurningShader(
			CBurningVideoDriver* driver,
			s32& outMaterialTypeNr,
			const c8* vertexShaderProgram = 0,
			const c8* vertexShaderEntryPointName = 0,
			E_VERTEX_SHADER_TYPE vsCompileTarget = video::EVST_VS_1_1,
			const c8* pixelShaderProgram = 0,
			const c8* pixelShaderEntryPointName = 0,
			E_PIXEL_SHADER_TYPE psCompileTarget = video::EPST_PS_1_1,
			const c8* geometryShaderProgram = 0,
			const c8* geometryShaderEntryPointName = "main",
			E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
			scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
			scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = EMT_SOLID,
			s32 userData = 0);

		//! destructor
		virtual ~IBurningShader();

		//! sets a render target
		virtual void setRenderTarget(video::IImage* surface, const core::rect<s32>& viewPort);

		//! sets the Texture
		virtual void setTextureParam( const size_t stage, video::CSoftwareTexture2* texture, s32 lodFactor);
		virtual void drawTriangle(const s4DVertex* burning_restrict a, const s4DVertex* burning_restrict b, const s4DVertex* burning_restrict c) {};
		virtual void drawLine ( const s4DVertex *a,const s4DVertex *b);
		virtual void drawPoint(const s4DVertex *a);

		void drawWireFrameTriangle ( const s4DVertex *a,const s4DVertex *b,const s4DVertex *c );

		virtual void OnSetMaterial( const SBurningShaderMaterial& material ) {};

		void pushEdgeTest(const int wireFrame,const int point,int save)
		{
			if ( save ) EdgeTestPass_stack = EdgeTestPass;
			EdgeTestPass = point ? edge_test_point : wireFrame ? edge_test_left : edge_test_pass;
		}
		void popEdgeTest() { EdgeTestPass = EdgeTestPass_stack; }
		virtual bool canWireFrame () { return false; }
		virtual bool canPointCloud() { return false; }

		void setStencilOp(eBurningStencilOp sfail, eBurningStencilOp dpfail, eBurningStencilOp dppass);

		//IMaterialRenderer

		virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
			bool resetAllRenderstates, IMaterialRendererServices* services) _IRR_OVERRIDE_;

		virtual bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype) _IRR_OVERRIDE_;

		virtual void OnUnsetMaterial() _IRR_OVERRIDE_;

		//! Returns if the material is transparent.
		virtual bool isTransparent() const _IRR_OVERRIDE_;

		//! Access the callback provided by the users when creating shader materials
		virtual IShaderConstantSetCallBack* getShaderConstantSetCallBack() const _IRR_OVERRIDE_;

		// implementations for the render services
		virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates) _IRR_OVERRIDE_;
		virtual s32 getVertexShaderConstantID(const c8* name) _IRR_OVERRIDE_;
		virtual s32 getPixelShaderConstantID(const c8* name) _IRR_OVERRIDE_;
		virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1) _IRR_OVERRIDE_;
		virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1) _IRR_OVERRIDE_;
		virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;
		virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;
		virtual bool setVertexShaderConstant(s32 index, const u32* ints, int count) _IRR_OVERRIDE_;
		virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;
		virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;
		virtual bool setPixelShaderConstant(s32 index, const u32* ints, int count)  _IRR_OVERRIDE_;
		virtual IVideoDriver* getVideoDriver() _IRR_OVERRIDE_;

		//used if no color interpolation is defined
		void setPrimitiveColor(const video::SColor& color)
		{
			#if BURNINGSHADER_COLOR_FORMAT == ECF_A8R8G8B8
				PrimitiveColor = color.color;
			#else
				PrimitiveColor = color.toA1R5G5B5();
			#endif
		}
		void setTLFlag(size_t in /*eTransformLightFlags*/)
		{
			TL_Flag = in;
		}
		void setFog(SColor color_fog)
		{
#if BURNINGSHADER_COLOR_FORMAT == ECF_A8R8G8B8
			fog_color_sample = color_fog.color;
#else
			fog_color_sample = color_fog.toA1R5G5B5();
#endif
			color_to_fix(fog_color, fog_color_sample);
		}
		void setScissor(const AbsRectangle& scissor)
		{
			Scissor = scissor;
		}

	protected:

		void constructor_IBurningShader(CBurningVideoDriver* driver);

		CBurningVideoDriver *Driver;
		IShaderConstantSetCallBack* CallBack;
		E_MATERIAL_TYPE BaseMaterial;
		s32 UserData;

		core::array<BurningUniform> UniformInfo;
		s32 getShaderConstantID(EBurningUniformFlags program, const c8* name);
		bool setShaderConstantID(EBurningUniformFlags flags, s32 index, const void* data, size_t u32_count);

		video::CImage* RenderTarget;
		CDepthBuffer* DepthBuffer;
		CStencilBuffer* Stencil;
		tVideoSample ColorMask;

		sInternalTexture IT[ BURNING_MATERIAL_MAX_TEXTURES ];

		static const tFixPointu dithermask[ 4 * 4];

		//draw degenerate triangle as line (left edge) drawTriangle -> holes,drawLine dda/bresenham
		int EdgeTestPass; //edge_test_flag
		int EdgeTestPass_stack;

		eBurningStencilOp stencilOp[4];
		tFixPoint AlphaRef;
		int RenderPass_ShaderIsTransparent;

		sScanConvertData scan;
		sScanLineData line;
		tVideoSample PrimitiveColor; //used if no color interpolation is defined

		size_t /*eTransformLightFlags*/ TL_Flag;
		tFixPoint fog_color[4];
		tVideoSample fog_color_sample;

		AbsRectangle Scissor;
	};


	IBurningShader* createTriangleRendererTextureGouraud2(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererTextureLightMap2_M1(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererTextureLightMap2_M2(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererTextureLightMap2_M4(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererGTextureLightMap2_M4(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererTextureLightMap2_Add(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererTextureDetailMap2(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererTextureVertexAlpha2(CBurningVideoDriver* driver);


	IBurningShader* createTriangleRendererTextureGouraudWire2(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererGouraud2(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererGouraudNoZ2(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererGouraudAlpha2(CBurningVideoDriver* driver);
	IBurningShader* createTRGouraudAlphaNoZ2(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererGouraudWire2(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererTextureFlat2(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererTextureFlatWire2(CBurningVideoDriver* driver);
	IBurningShader* createTRFlat2(CBurningVideoDriver* driver);
	IBurningShader* createTRFlatWire2(CBurningVideoDriver* driver);
	IBurningShader* createTRTextureGouraudNoZ2(CBurningVideoDriver* driver);
	IBurningShader* createTRTextureGouraudAdd2(CBurningVideoDriver* driver);
	IBurningShader* createTRTextureGouraudAddNoZ2(CBurningVideoDriver* driver);

	IBurningShader* createTRTextureGouraudAlpha(CBurningVideoDriver* driver);
	IBurningShader* createTRTextureGouraudAlphaNoZ(CBurningVideoDriver* driver);
	IBurningShader* createTRTextureBlend(CBurningVideoDriver* driver);
	IBurningShader* createTRTextureInverseAlphaBlend(CBurningVideoDriver* driver);

	IBurningShader* createTRNormalMap(CBurningVideoDriver* driver);
	IBurningShader* createTRStencilShadow(CBurningVideoDriver* driver);

	IBurningShader* createTriangleRendererReference(CBurningVideoDriver* driver);
	IBurningShader* createTriangleRendererTexture_transparent_reflection_2_layer(CBurningVideoDriver* driver);

	IBurningShader* create_burning_shader_color(CBurningVideoDriver* driver);

} // end namespace video
} // end namespace irr

#endif

