// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_VIDEO_2_SOFTWARE_H_INCLUDED
#define IRR_C_VIDEO_2_SOFTWARE_H_INCLUDED

#include "SoftwareDriver2_compile_config.h"
#include "IBurningShader.h"
#include "CNullDriver.h"
#include "CImage.h"
#include "os.h"
#include "irrString.h"
#include "SIrrCreationParameters.h"


namespace irr
{
namespace video
{
	class CBurningVideoDriver : public CNullDriver, public IMaterialRendererServices
	{
	public:

		//! constructor
		CBurningVideoDriver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, video::IImagePresenter* presenter);

		//! destructor
		virtual ~CBurningVideoDriver();

		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const IRR_OVERRIDE;

		//! Create render target.
		virtual IRenderTarget* addRenderTarget() IRR_OVERRIDE;

		//! Run occlusion query. Draws mesh stored in query.
		/** If the mesh shall not be rendered visible, use
		overrideMaterial to disable the color and depth buffer. */
		virtual void runOcclusionQuery(scene::ISceneNode* node, bool visible = false) IRR_OVERRIDE;

		//! Update occlusion query. Retrieves results from GPU.
		/** If the query shall not block, set the flag to false.
		Update might not occur in this case, though */
		virtual void updateOcclusionQuery(scene::ISceneNode* node, bool block = true) IRR_OVERRIDE;

		//! Return query result.
		/** Return value is the number of visible pixels/fragments.
		The value is a safe approximation, i.e. can be larger then the
		actual value of pixels. */
		virtual u32 getOcclusionQueryResult(scene::ISceneNode* node) const IRR_OVERRIDE;

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat) IRR_OVERRIDE;

		//! sets a material
		virtual void setMaterial(const SMaterial& material) IRR_OVERRIDE;

		virtual bool setRenderTargetEx(IRenderTarget* target, u16 clearFlag, SColor clearColor,
			f32 clearDepth, u8 clearStencil) IRR_OVERRIDE;

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area) IRR_OVERRIDE;
		virtual void setScissor(int x, int y, int width, int height);

		virtual bool beginScene(u16 clearFlag, SColor clearColor, f32 clearDepth, u8 clearStencil,
			const SExposedVideoData& videoData, core::rect<s32>* sourceRect) IRR_OVERRIDE;

#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
		virtual bool beginScene(bool backBuffer, bool zBuffer, SColor color,
			const SExposedVideoData& videoData, core::rect<s32>* sourceRect)
		{
			u16 flag = 0;
			if (backBuffer)	flag |= ECBF_COLOR;
			if (zBuffer) flag |= ECBF_DEPTH;
			return beginScene(flag, color, 1.f, 0, videoData, sourceRect);
		}
		virtual bool setRenderTarget(video::ITexture* texture, bool clearBackBuffer, bool clearZBuffer, SColor color);
#endif

		virtual bool endScene() IRR_OVERRIDE;

		//! Only used by the internal engine. Used to notify the driver that
		//! the window was resized.
		virtual void OnResize(const core::dimension2d<u32>& size) IRR_OVERRIDE;

		//! returns size of the current render target
		virtual const core::dimension2d<u32>& getCurrentRenderTargetSize() const IRR_OVERRIDE;

		//! deletes all dynamic lights there are
		virtual void deleteAllDynamicLights() IRR_OVERRIDE;

		//! adds a dynamic light, returning an index to the light
		//! \param light: the light data to use to create the light
		//! \return An index to the light, or -1 if an error occurs
		virtual s32 addDynamicLight(const SLight& light) IRR_OVERRIDE;

		//! Turns a dynamic light on or off
		//! \param lightIndex: the index returned by addDynamicLight
		//! \param turnOn: true to turn the light on, false to turn it off
		virtual void turnLightOn(s32 lightIndex, bool turnOn) IRR_OVERRIDE;

		//! returns the maximal amount of dynamic lights the device can handle
		virtual u32 getMaximalDynamicLightAmount() const IRR_OVERRIDE;

		//! Sets the dynamic ambient light color. The default color is
		//! (0,0,0,0) which means it is dark.
		//! \param color: New color of the ambient light.
		virtual void setAmbientLight(const SColorf& color) IRR_OVERRIDE;

		//! draws a vertex primitive list
		virtual void drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
				const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType) IRR_OVERRIDE;

		//! draws a vertex primitive list in 2d
		virtual void draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount,
			const void* indexList, u32 primitiveCount,
			E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType) IRR_OVERRIDE;


		//! draws an 2d image
		//virtual void draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos, bool useAlphaChannelOfTexture) IRR_OVERRIDE;

		/* NullDriver calls
		draw2DImage(texture, destPos,
			core::rect<s32>(core::position2d<s32>(0, 0), core::dimension2di(texture->getOriginalSize())),
			0,
			SColor(255, 255, 255, 255),
			useAlphaChannelOfTexture
		*/

		//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
		virtual void draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
			SColor color=SColor(255,255,255,255), bool useAlphaChannelOfTexture=false) IRR_OVERRIDE;

		//! Draws a part of the texture into the rectangle.
		virtual void draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
				const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
				const video::SColor* const colors=0, bool useAlphaChannelOfTexture=false) IRR_OVERRIDE;

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
			const core::vector3df& end, SColor color_start) IRR_OVERRIDE;

		//! draw an 2d rectangle
		//virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos,
		//	const core::rect<s32>* clip = 0) IRR_OVERRIDE;

		/* NullDriver calls
		draw2DRectangle(pos, color, color, color, color, clip);
		*/

		//!Draws an 2d rectangle with a gradient.
		virtual void draw2DRectangle(const core::rect<s32>& pos,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32>* clip = 0) IRR_OVERRIDE;

		//! Draws a 2d line.
		virtual void draw2DLine(const core::position2d<s32>& start,
					const core::position2d<s32>& end,
					SColor color=SColor(255,255,255,255)) IRR_OVERRIDE;

		//! Draws a single pixel
		virtual void drawPixel(u32 x, u32 y, const SColor & color) IRR_OVERRIDE;

		//! \return Returns the name of the video driver. Example: In case of the DirectX8
		//! driver, it would return "Direct3D8.1".
		virtual const wchar_t* getName() const IRR_OVERRIDE;

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType() const IRR_OVERRIDE;

		//! get color format of the current color buffer
		virtual ECOLOR_FORMAT getColorFormat() const IRR_OVERRIDE;

		//! Returns the transformation set by setTransform
		virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const IRR_OVERRIDE;

		//! Creates a render target texture.
		virtual ITexture* addRenderTargetTexture(const core::dimension2d<u32>& size,
			const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
			, const bool useStencil = false
#endif
		) IRR_OVERRIDE;


		virtual void clearBuffers(u16 flag, SColor color, f32 depth, u8 stencil) IRR_OVERRIDE;

		//! Returns an image created from the last rendered frame.
		virtual IImage* createScreenShot(video::ECOLOR_FORMAT format=video::ECF_UNKNOWN, video::E_RENDER_TARGET target=video::ERT_FRAME_BUFFER) IRR_OVERRIDE;

		//! Returns the maximum amount of primitives (mostly vertices) which
		//! the device is able to render with one drawIndexedTriangleList
		//! call.
		virtual u32 getMaximalPrimitiveCount() const IRR_OVERRIDE;

		//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
		//! this: First, draw all geometry. Then use this method, to draw the shadow
		//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
		virtual void drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail=true, u32 debugDataVisible=0) IRR_OVERRIDE;

		//! Fills the stencil shadow with color. After the shadow volume has been drawn
		//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
		//! to draw the color of the shadow.
		virtual void drawStencilShadow(bool clearStencilBuffer=false,
			video::SColor leftUpEdge = video::SColor(0,0,0,0),
			video::SColor rightUpEdge = video::SColor(0,0,0,0),
			video::SColor leftDownEdge = video::SColor(0,0,0,0),
			video::SColor rightDownEdge = video::SColor(0,0,0,0)) IRR_OVERRIDE;

		//! Enable the 2d override material
		virtual void enableMaterial2D(bool enable = true) IRR_OVERRIDE;

		//! Returns the graphics card vendor name.
		virtual core::stringc getVendorInfo() IRR_OVERRIDE;

		//! Returns the maximum texture size supported.
		virtual core::dimension2du getMaxTextureSize() const IRR_OVERRIDE;

		//! Check if the driver supports creating textures with the given color format
		virtual bool queryTextureFormat(ECOLOR_FORMAT format) const IRR_OVERRIDE;

#if !defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
		//! Used by some SceneNodes to check if a material should be rendered in the transparent render pass
		virtual bool needsTransparentRenderPass(const irr::video::SMaterial& material) const IRR_OVERRIDE;
#endif

		IDepthBuffer * getDepthBuffer () { return DepthBuffer; }
		IStencilBuffer * getStencilBuffer () { return StencilBuffer; }

		//! Adds a new material renderer to the VideoDriver, using pixel and/or
		//! vertex shaders to render geometry.
		virtual s32 addShaderMaterial(const c8* vertexShaderProgram,
			const c8* pixelShaderProgram,
			IShaderConstantSetCallBack* callback,
			E_MATERIAL_TYPE baseMaterial,
			s32 userData) IRR_OVERRIDE;

		//! Adds a new material renderer to the VideoDriver, based on a high level shading
		//! language. Currently only HLSL in D3D9 is supported.
		virtual s32 addHighLevelShaderMaterial(
			const c8* vertexShaderProgram,
			const c8* vertexShaderEntryPointName = 0,
			E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
			const c8* pixelShaderProgram = 0,
			const c8* pixelShaderEntryPointName = 0,
			E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
			const c8* geometryShaderProgram = 0,
			const c8* geometryShaderEntryPointName = "main",
			E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
			scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
			scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData = 0
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
			, E_GPU_SHADING_LANGUAGE shadingLang = EGSL_DEFAULT
#endif
		) IRR_OVERRIDE;

		//IMaterialRendererService

		virtual void setBasicRenderStates(const SMaterial& material,
			const SMaterial& lastMaterial,
			bool resetAllRenderstates) IRR_OVERRIDE;

		//pass BaseMaterialID
		void setFallback_Material(E_MATERIAL_TYPE fallback_MaterialType
			, eBurningVertexShader vertexShader);

		//! Return an index constant for the vertex shader based on a name.
		virtual s32 getVertexShaderConstantID(const c8* name) IRR_OVERRIDE;
		virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count) IRR_OVERRIDE;
		virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count) IRR_OVERRIDE;
		virtual bool setVertexShaderConstant(s32 index, const u32* ints, int count) IRR_OVERRIDE;
		virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount) IRR_OVERRIDE;

		//! Return an index constant for the pixel shader based on a name.
		virtual s32 getPixelShaderConstantID(const c8* name) IRR_OVERRIDE;
		virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count) IRR_OVERRIDE;
		virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count) IRR_OVERRIDE;
		virtual bool setPixelShaderConstant(s32 index, const u32* ints, int count) IRR_OVERRIDE;
		
		virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount) IRR_OVERRIDE;

#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
		virtual bool setVertexShaderConstant(const c8* name, const f32* floats, int count)
		{
			return setVertexShaderConstant(getVertexShaderConstantID(name), floats, count);
		}
		virtual bool setVertexShaderConstant(const c8* name, const bool* bools, int count)
		{
			return setVertexShaderConstant(getVertexShaderConstantID(name), (const s32*)bools, count);
		}
		virtual bool setVertexShaderConstant(const c8* name, const s32* ints, int count)
		{
			return setVertexShaderConstant(getVertexShaderConstantID(name), ints, count);
		}

		virtual bool setPixelShaderConstant(const c8* name, const f32* floats, int count)
		{
			return setPixelShaderConstant(getPixelShaderConstantID(name), floats, count);
		}
		virtual bool setPixelShaderConstant(const c8* name, const bool* bools, int count)
		{
			return setPixelShaderConstant(getPixelShaderConstantID(name), (const s32*)bools, count);
		}
		virtual bool setPixelShaderConstant(const c8* name, const s32* ints, int count)
		{
			return setPixelShaderConstant(getPixelShaderConstantID(name), ints, count);
		}
#endif
		//! Get pointer to the IVideoDriver interface
		/** \return Pointer to the IVideoDriver interface */
		virtual IVideoDriver* getVideoDriver() IRR_OVERRIDE;

	protected:

		void saveBuffer();

		//! sets a render target
		void setRenderTargetImage2(video::IImage* color, video::IImage* depth=0, video::IImage* stencil=0);

		//! sets the current Texture
		//bool setTexture(u32 stage, video::ITexture* texture);

		virtual ITexture* createDeviceDependentTexture(const io::path& name, IImage* image) IRR_OVERRIDE;
		virtual ITexture* createDeviceDependentTextureCubemap(const io::path& name, const core::array<IImage*>& image) IRR_OVERRIDE;

		video::CImage* BackBuffer;
		video::IImagePresenter* Presenter;

		void* WindowId;
		core::rect<s32>* SceneSourceRect;

		video::ITexture* RenderTargetTexture;
		video::IImage* RenderTargetSurface;
		core::dimension2d<u32> RenderTargetSize;
		sVec4 RatioRenderTargetScreen; // Smaller Render Target

		IBurningShader* CurrentShader;
		IBurningShader* BurningShader[ETR2_COUNT];

		PushShaderData PushShader;
		void pushShader(scene::E_PRIMITIVE_TYPE pType, int testCurrent);

		IDepthBuffer* DepthBuffer;
		IStencilBuffer* StencilBuffer;


		/*
			extend Matrix Stack
			-> combined CameraProjection
			-> combined CameraProjectionWorld
			-> ClipScale from NDC to DC Space
		*/
		enum E_TRANSFORMATION_STATE_BURNING_VIDEO
		{
			ETS_VIEW_PROJECTION = ETS_COUNT,
			ETS_MODEL_VIEW_PROJ,
			ETS_MODEL_VIEW,
			ETS_NORMAL, //3x3 ModelView Transposed Inverse

			ETS_MODEL_INVERSE,	//normal,parallax

			ETS_COUNT_BURNING = 16
		};

		// align manually to 16 byte start address
		//u8 _pack_0[8];
		enum E_TRANSFORMATION_FLAG
		{
			ETF_VALID = 1,
			ETF_IDENTITY = 2,
			ETF_TEXGEN_MATRIX = 4,	// or !ETF_IDENTITY
			ETF_TEXGEN_CAMERA_SPHERE = 8,
			ETF_TEXGEN_CAMERA_REFLECTION = 16,
			ETF_TEXGEN_MASK = ETF_TEXGEN_CAMERA_SPHERE | ETF_TEXGEN_CAMERA_REFLECTION | ETF_TEXGEN_MATRIX
		};
		enum E_TRANSFORMATION_STACK
		{
			ETF_STACK_3D = 0,
			ETF_STACK_2D = 1,
		};

		size_t TransformationStack; // 0 .. 3D , 1 .. 2D, 2.. Geometric Clipper
		core::matrix4 ALIGN(16) Transformation[2][ETS_COUNT_BURNING];
		size_t TransformationFlag[2][ETS_COUNT_BURNING]; // E_TRANSFORMATION_FLAG
		

		void setRenderStates2DMode(const video::SColor& color,const video::ITexture* texture,bool useAlphaChannelOfTexture);
		void setRenderStates3DMode();

		//ETS_CLIPSCALE, // moved outside to stay at 16 matrices
		f32 Transformation_ETS_CLIPSCALE[2][4];
		void transform_calc(E_TRANSFORMATION_STATE_BURNING_VIDEO state);

		//core::recti ViewPort;
		AbsRectangle Scissor;

		// Vertex Cache
		SVertexShader VertexShader;

		int VertexCache_reset (const void* vertices, u32 vertexCount,
					const void* indices, u32 indexCount,
					E_VERTEX_TYPE vType,scene::E_PRIMITIVE_TYPE pType,
					E_INDEX_TYPE iType);
		//void VertexCache_get (s4DVertexPair* face[4] );

		void VertexCache_map_source_format();
		//s4DVertexPair* VertexCache_getVertex ( const u32 sourceIndex ) const;

		// culling & clipping
		//size_t inline clipToHyperPlane (s4DVertexPair* burning_restrict dest, const s4DVertexPair* burning_restrict source, const size_t inCount, const sVec4 &plane );
		//size_t inline clipToFrustumTest ( const s4DVertex * v  ) const;
		public:
		void VertexCache_fill(const u32 sourceIndex, const u32 destIndex);
		u32 clipToFrustum( const u32 vIn /*, const size_t clipmask_for_face*/ );
		protected:

		// holds transformed, clipped vertices for a triangle. triangle expands on clipping
		// Buffer is in in pairs of 4DVertex (0 ... ndc, 1 .. dc and projected)
		SAligned4DVertex Clipper;
		SAligned4DVertex Clipper_disjoint; // __restrict helper


#ifdef SOFTWARE_DRIVER_2_LIGHTING
		void lightVertex_eye ( s4DVertex *dest, const u32 vertexargb );
#endif

		//! Sets the fog mode.
		virtual void setFog(SColor color, E_FOG_TYPE fogType, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog) IRR_OVERRIDE;


		//void ndc_2_dc_and_project (s4DVertexPair* dest,const s4DVertexPair* source, const size_t vIn ) const;

		//const is misleading. **v is const that true, but not *v..
		//f32 screenarea_inside (const s4DVertexPair* burning_restrict const face[] ) const;
		//s32 lodFactor_inside ( const s4DVertexPair* burning_restrict const face[], const size_t tex, const f32 dc_area, const f32 lod_bias ) const;
		//void select_polygon_mipmap_inside (s4DVertexPair* burning_restrict face[], const size_t tex, const CSoftwareTexture2_Bound& b ) const;

		//void getCameraPosWorldSpace();
		void assignHardwareLight(SBurningShaderLight& l, const SLight& dl);
		SBurningShaderEyeSpace EyeSpace;
		SBurningShaderMaterial Material;

		//static const sVec4 NDCPlane[6+2];

		//! Built-in 2D quad for 2D rendering.
		S3DVertex Quad2DVertices[4];
		interlaced_control Interlaced;
		f32 TexBias[2];
public:
		const interlaced_control& getInterlace() { return Interlaced; }
protected:

		u32 samples_passed;

#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
		core::array<IRenderTarget*> RenderTargets;

		inline bool getWriteZBuffer(const SMaterial& material) const
		{
			return material.ZWriteEnable && (AllowZWriteOnTransparent || !material.isTransparent());
		}
		virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData = 0)
		{
			return createDeviceDependentTexture(name, surface);
		}
#endif

	};

} // end namespace video
} // end namespace irr


#endif
