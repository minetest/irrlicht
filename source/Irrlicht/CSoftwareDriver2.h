// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VIDEO_2_SOFTWARE_H_INCLUDED__
#define __C_VIDEO_2_SOFTWARE_H_INCLUDED__

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
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const _IRR_OVERRIDE_;

		//! Create render target.
		virtual IRenderTarget* addRenderTarget() _IRR_OVERRIDE_;

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat) _IRR_OVERRIDE_;

		//! sets a material
		virtual void setMaterial(const SMaterial& material) _IRR_OVERRIDE_;

		virtual bool setRenderTargetEx(IRenderTarget* target, u16 clearFlag, SColor clearColor,
			f32 clearDepth, u8 clearStencil) _IRR_OVERRIDE_;

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area) _IRR_OVERRIDE_;
		virtual void setScissor(int x, int y, int width, int height);

		virtual bool beginScene(u16 clearFlag, SColor clearColor, f32 clearDepth, u8 clearStencil,
			const SExposedVideoData& videoData, core::rect<s32>* sourceRect) _IRR_OVERRIDE_;

		virtual bool endScene() _IRR_OVERRIDE_;

		//! Only used by the internal engine. Used to notify the driver that
		//! the window was resized.
		virtual void OnResize(const core::dimension2d<u32>& size) _IRR_OVERRIDE_;

		//! returns size of the current render target
		virtual const core::dimension2d<u32>& getCurrentRenderTargetSize() const _IRR_OVERRIDE_;

		//! deletes all dynamic lights there are
		virtual void deleteAllDynamicLights() _IRR_OVERRIDE_;

		//! adds a dynamic light, returning an index to the light
		//! \param light: the light data to use to create the light
		//! \return An index to the light, or -1 if an error occurs
		virtual s32 addDynamicLight(const SLight& light) _IRR_OVERRIDE_;

		//! Turns a dynamic light on or off
		//! \param lightIndex: the index returned by addDynamicLight
		//! \param turnOn: true to turn the light on, false to turn it off
		virtual void turnLightOn(s32 lightIndex, bool turnOn) _IRR_OVERRIDE_;

		//! returns the maximal amount of dynamic lights the device can handle
		virtual u32 getMaximalDynamicLightAmount() const _IRR_OVERRIDE_;

		//! Sets the dynamic ambient light color. The default color is
		//! (0,0,0,0) which means it is dark.
		//! \param color: New color of the ambient light.
		virtual void setAmbientLight(const SColorf& color) _IRR_OVERRIDE_;

		//! draws a vertex primitive list
		virtual void drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
				const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType) _IRR_OVERRIDE_;

		//! draws a vertex primitive list in 2d
		virtual void draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount,
			const void* indexList, u32 primitiveCount,
			E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType) _IRR_OVERRIDE_;


		//! draws an 2d image
		//virtual void draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos, bool useAlphaChannelOfTexture) _IRR_OVERRIDE_;

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
			SColor color=SColor(255,255,255,255), bool useAlphaChannelOfTexture=false) _IRR_OVERRIDE_;

		//! Draws a part of the texture into the rectangle.
		virtual void draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
				const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
				const video::SColor* const colors=0, bool useAlphaChannelOfTexture=false) _IRR_OVERRIDE_;

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
			const core::vector3df& end, SColor color_start) _IRR_OVERRIDE_;

		//! draw an 2d rectangle
		//virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos,
		//	const core::rect<s32>* clip = 0) _IRR_OVERRIDE_;

		/* NullDriver calls
		draw2DRectangle(pos, color, color, color, color, clip);
		*/

		//!Draws an 2d rectangle with a gradient.
		virtual void draw2DRectangle(const core::rect<s32>& pos,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32>* clip = 0) _IRR_OVERRIDE_;

		//! Draws a 2d line.
		virtual void draw2DLine(const core::position2d<s32>& start,
					const core::position2d<s32>& end,
					SColor color=SColor(255,255,255,255)) _IRR_OVERRIDE_;

		//! Draws a single pixel
		virtual void drawPixel(u32 x, u32 y, const SColor & color) _IRR_OVERRIDE_;

		//! \return Returns the name of the video driver. Example: In case of the DirectX8
		//! driver, it would return "Direct3D8.1".
		virtual const wchar_t* getName() const _IRR_OVERRIDE_;

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType() const _IRR_OVERRIDE_;

		//! get color format of the current color buffer
		virtual ECOLOR_FORMAT getColorFormat() const _IRR_OVERRIDE_;

		//! Returns the transformation set by setTransform
		virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const _IRR_OVERRIDE_;

		//! Creates a render target texture.
		virtual ITexture* addRenderTargetTexture(const core::dimension2d<u32>& size,
			const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN) _IRR_OVERRIDE_;

		virtual void clearBuffers(u16 flag, SColor color, f32 depth, u8 stencil) _IRR_OVERRIDE_;

		//! Returns an image created from the last rendered frame.
		virtual IImage* createScreenShot(video::ECOLOR_FORMAT format=video::ECF_UNKNOWN, video::E_RENDER_TARGET target=video::ERT_FRAME_BUFFER) _IRR_OVERRIDE_;

		//! Returns the maximum amount of primitives (mostly vertices) which
		//! the device is able to render with one drawIndexedTriangleList
		//! call.
		virtual u32 getMaximalPrimitiveCount() const _IRR_OVERRIDE_;

		//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
		//! this: First, draw all geometry. Then use this method, to draw the shadow
		//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
		virtual void drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail=true, u32 debugDataVisible=0) _IRR_OVERRIDE_;

		//! Fills the stencil shadow with color. After the shadow volume has been drawn
		//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
		//! to draw the color of the shadow.
		virtual void drawStencilShadow(bool clearStencilBuffer=false,
			video::SColor leftUpEdge = video::SColor(0,0,0,0),
			video::SColor rightUpEdge = video::SColor(0,0,0,0),
			video::SColor leftDownEdge = video::SColor(0,0,0,0),
			video::SColor rightDownEdge = video::SColor(0,0,0,0)) _IRR_OVERRIDE_;

		//! Enable the 2d override material
		virtual void enableMaterial2D(bool enable = true) _IRR_OVERRIDE_;

		//! Returns the graphics card vendor name.
		virtual core::stringc getVendorInfo() _IRR_OVERRIDE_;

		//! Returns the maximum texture size supported.
		virtual core::dimension2du getMaxTextureSize() const _IRR_OVERRIDE_;

		//! Check if the driver supports creating textures with the given color format
		virtual bool queryTextureFormat(ECOLOR_FORMAT format) const _IRR_OVERRIDE_;

		//! Used by some SceneNodes to check if a material should be rendered in the transparent render pass
		virtual bool needsTransparentRenderPass(const irr::video::SMaterial& material) const _IRR_OVERRIDE_;

		IDepthBuffer * getDepthBuffer () { return DepthBuffer; }
		IStencilBuffer * getStencilBuffer () { return StencilBuffer; }

		//#define Tweak_Burning
#if defined(Tweak_Burning)
		virtual void postEventFromUser(const void* sevent)  _IRR_OVERRIDE_;
#endif

		//! Adds a new material renderer to the VideoDriver, using pixel and/or
		//! vertex shaders to render geometry.
		virtual s32 addShaderMaterial(const c8* vertexShaderProgram,
			const c8* pixelShaderProgram,
			IShaderConstantSetCallBack* callback,
			E_MATERIAL_TYPE baseMaterial,
			s32 userData) _IRR_OVERRIDE_;

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
			s32 userData = 0) _IRR_OVERRIDE_;

		//IMaterialRendererService

		virtual void setBasicRenderStates(const SMaterial& material,
			const SMaterial& lastMaterial,
			bool resetAllRenderstates) _IRR_OVERRIDE_;

		//pass BaseMaterialID
		void setFallback_Material(E_MATERIAL_TYPE fallback_MaterialType);

		//! Return an index constant for the vertex shader based on a name.
		virtual s32 getVertexShaderConstantID(const c8* name) _IRR_OVERRIDE_;
		virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;
		virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;
		virtual bool setVertexShaderConstant(s32 index, const u32* ints, int count) _IRR_OVERRIDE_;
		virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount) _IRR_OVERRIDE_;

		//! Return an index constant for the pixel shader based on a name.
		virtual s32 getPixelShaderConstantID(const c8* name) _IRR_OVERRIDE_;
		virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;
		virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;
		virtual bool setPixelShaderConstant(s32 index, const u32* ints, int count) _IRR_OVERRIDE_;
		
		virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount) _IRR_OVERRIDE_;

		//! Get pointer to the IVideoDriver interface
		/** \return Pointer to the IVideoDriver interface */
		virtual IVideoDriver* getVideoDriver() _IRR_OVERRIDE_;

	protected:

		void saveBuffer();

		//! sets a render target
		void setRenderTargetImage(video::CImage* image);

		//! sets the current Texture
		//bool setTexture(u32 stage, video::ITexture* texture);

		virtual ITexture* createDeviceDependentTexture(const io::path& name, IImage* image) _IRR_OVERRIDE_;
		virtual ITexture* createDeviceDependentTextureCubemap(const io::path& name, const core::array<IImage*>& image) _IRR_OVERRIDE_;

		video::CImage* BackBuffer;
		video::IImagePresenter* Presenter;

		void* WindowId;
		core::rect<s32>* SceneSourceRect;

		video::ITexture* RenderTargetTexture;
		video::IImage* RenderTargetSurface;
		core::dimension2d<u32> RenderTargetSize;

		IBurningShader* CurrentShader;
		IBurningShader* BurningShader[ETR2_COUNT];

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
			ETS_PROJ_MODEL_VIEW,
			ETS_MODEL_VIEW,
			ETS_NORMAL, //3x3 ModelView Tansposed Inverse

			ETS_COUNT_BURNING = 16
		};

		enum E_TRANSFORMATION_FLAG
		{
			ETF_VALID = 1,
			ETF_IDENTITY = 2,
			ETF_TEXGEN_CAMERA_SPHERE = 4,
			ETF_TEXGEN_CAMERA_REFLECTION = 8,
			ETF_TEXGEN_WRAP = 16,
			ETF_TEXGEN_MASK = ETF_TEXGEN_CAMERA_SPHERE | ETF_TEXGEN_CAMERA_REFLECTION | ETF_TEXGEN_WRAP
		};
		core::matrix4 Transformation[2][ETS_COUNT_BURNING];
		size_t TransformationFlag[2][ETS_COUNT_BURNING]; // E_TRANSFORMATION_FLAG

		size_t TransformationStack; // 0 .. 3D , 1 .. 2D

		void setRenderStates2DMode(const video::SColor& color,video::ITexture* texture,bool useAlphaChannelOfTexture);
		void setRenderStates3DMode();

		//ETS_CLIPSCALE, // moved outside to stay at 16 matrices
		f32 Transformation_ETS_CLIPSCALE[2][4];
		void transform_calc(E_TRANSFORMATION_STATE_BURNING_VIDEO state);

		//core::recti ViewPort;
		AbsRectangle Scissor;

		// Vertex Cache
		SVertexCache VertexCache;

		int VertexCache_reset (const void* vertices, u32 vertexCount,
					const void* indices, u32 indexCount,
					E_VERTEX_TYPE vType,scene::E_PRIMITIVE_TYPE pType,
					E_INDEX_TYPE iType);
		void VertexCache_get (s4DVertexPair* face[4] );

		void VertexCache_map_source_format();
		void VertexCache_fill ( const u32 sourceIndex,const u32 destIndex );
		s4DVertexPair* VertexCache_getVertex ( const u32 sourceIndex ) const;


		// culling & clipping
		//size_t inline clipToHyperPlane (s4DVertexPair* burning_restrict dest, const s4DVertexPair* burning_restrict source, const size_t inCount, const sVec4 &plane );
		//size_t inline clipToFrustumTest ( const s4DVertex * v  ) const;
		public:
		size_t clipToFrustum( const size_t vIn /*, const size_t clipmask_for_face*/ );
		protected:

		// holds transformed, clipped vertices for a triangle. triangle expands on clipping
		// Buffer is in in pairs of 4DVertex (0 ... ndc, 1 .. dc and projected)
		SAligned4DVertex Clipper;
		SAligned4DVertex Clipper_temp;


#ifdef SOFTWARE_DRIVER_2_LIGHTING
		void lightVertex_eye ( s4DVertex *dest, u32 vertexargb );
#endif

		//! Sets the fog mode.
		virtual void setFog(SColor color, E_FOG_TYPE fogType, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog) _IRR_OVERRIDE_;


		void ndc_2_dc_and_project (s4DVertexPair* dest,const s4DVertexPair* source, const size_t vIn ) const;

		//const is misleading. **v is const that true, but not *v..
		f32 screenarea_inside (const s4DVertexPair* burning_restrict const face[] ) const;
		s32 lodFactor_inside ( const s4DVertexPair* burning_restrict const face[], const size_t tex, f32 dc_area, f32 lod_bias ) const;
		void select_polygon_mipmap_inside ( s4DVertex* burning_restrict face[], const size_t tex, const CSoftwareTexture2_Bound& b ) const;

		void getCameraPosWorldSpace();
		SBurningShaderEyeSpace EyeSpace;
		SBurningShaderMaterial Material;

		static const sVec4 NDCPlane[6+2];

		//! Built-in 2D quad for 2D rendering.
		S3DVertex Quad2DVertices[4];

	};

} // end namespace video
} // end namespace irr


#endif

