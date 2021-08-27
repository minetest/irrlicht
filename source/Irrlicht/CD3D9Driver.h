// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_VIDEO_DIRECTX_9_H_INCLUDED
#define IRR_C_VIDEO_DIRECTX_9_H_INCLUDED

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_

#ifdef _IRR_WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "CNullDriver.h"
#include "SIrrCreationParameters.h"
#include "IMaterialRendererServices.h"
#if defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#include "irrMath.h"    // needed by borland for sqrtf define
#endif
#include <d3d9.h>

namespace irr
{
namespace video
{
	class CD3D9CallBridge;
	class CD3D9RenderTarget;
	class CD3D9Texture;

	class CD3D9Driver : public CNullDriver, IMaterialRendererServices
	{
	public:

		friend class CD3D9CallBridge;
		friend class CD3D9RenderTarget;
		friend class CD3D9Texture;

		//! constructor
		CD3D9Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io);

		//! destructor
		virtual ~CD3D9Driver();

		virtual bool beginScene(u16 clearFlag, SColor clearColor = SColor(255,0,0,0), f32 clearDepth = 1.f, u8 clearStencil = 0,
			const SExposedVideoData& videoData = SExposedVideoData(), core::rect<s32>* sourceRect = 0) IRR_OVERRIDE;

		virtual bool endScene() IRR_OVERRIDE;

		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const IRR_OVERRIDE;

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat) IRR_OVERRIDE;

		//! sets a material
		virtual void setMaterial(const SMaterial& material) IRR_OVERRIDE;

		virtual bool setRenderTargetEx(IRenderTarget* target, u16 clearFlag, SColor clearColor = SColor(255,0,0,0),
			f32 clearDepth = 1.f, u8 clearStencil = 0) IRR_OVERRIDE;

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area) IRR_OVERRIDE;

		struct SHWBufferLink_d3d9 : public SHWBufferLink
		{
			SHWBufferLink_d3d9(const scene::IMeshBuffer *_MeshBuffer):
				SHWBufferLink(_MeshBuffer),
					vertexBuffer(0), indexBuffer(0),
					vertexBufferSize(0), indexBufferSize(0) {}

			IDirect3DVertexBuffer9* vertexBuffer;
			IDirect3DIndexBuffer9* indexBuffer;

			u32 vertexBufferSize;
			u32 indexBufferSize;
		};

		bool updateVertexHardwareBuffer(SHWBufferLink_d3d9 *HWBuffer);
		bool updateIndexHardwareBuffer(SHWBufferLink_d3d9 *HWBuffer);

		//! updates hardware buffer if needed
		virtual bool updateHardwareBuffer(SHWBufferLink *HWBuffer) IRR_OVERRIDE;

		//! Create hardware buffer from mesh
		virtual SHWBufferLink *createHardwareBuffer(const scene::IMeshBuffer* mb) IRR_OVERRIDE;

		//! Delete hardware buffer (only some drivers can)
		virtual void deleteHardwareBuffer(SHWBufferLink *HWBuffer) IRR_OVERRIDE;

		//! Draw hardware buffer
		virtual void drawHardwareBuffer(SHWBufferLink *HWBuffer) IRR_OVERRIDE;

		//! Create occlusion query.
		/** Use node for identification and mesh for occlusion test. */
		virtual void addOcclusionQuery(scene::ISceneNode* node,
				const scene::IMesh* mesh=0) IRR_OVERRIDE;

		//! Remove occlusion query.
		virtual void removeOcclusionQuery(scene::ISceneNode* node) IRR_OVERRIDE;

		//! Run occlusion query. Draws mesh stored in query.
		/** If the mesh shall not be rendered visible, use
		overrideMaterial to disable the color and depth buffer. */
		virtual void runOcclusionQuery(scene::ISceneNode* node, bool visible=false) IRR_OVERRIDE;

		//! Update occlusion query. Retrieves results from GPU.
		/** If the query shall not block, set the flag to false.
		Update might not occur in this case, though */
		virtual void updateOcclusionQuery(scene::ISceneNode* node, bool block=true) IRR_OVERRIDE;

		//! Return query result.
		/** Return value is the number of visible pixels/fragments.
		The value is a safe approximation, i.e. can be larger then the
		actual value of pixels. */
		virtual u32 getOcclusionQueryResult(scene::ISceneNode* node) const IRR_OVERRIDE;

		//! Create render target.
		virtual IRenderTarget* addRenderTarget() IRR_OVERRIDE;

		//! draws a vertex primitive list
		virtual void drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
				const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
				E_INDEX_TYPE iType) IRR_OVERRIDE;

		//! draws a vertex primitive list in 2d
		virtual void draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount,
				const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
				E_INDEX_TYPE iType) IRR_OVERRIDE;

		//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
		virtual void draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
			SColor color=SColor(255,255,255,255), bool useAlphaChannelOfTexture=false) IRR_OVERRIDE;

		//! Draws a part of the texture into the rectangle.
		virtual void draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
			const video::SColor* const colors=0, bool useAlphaChannelOfTexture=false) IRR_OVERRIDE;

		//! Draws a set of 2d images, using a color and the alpha channel of the texture.
		virtual void draw2DImageBatch(const video::ITexture* texture,
				const core::array<core::position2d<s32> >& positions,
				const core::array<core::rect<s32> >& sourceRects,
				const core::rect<s32>* clipRect=0,
				SColor color=SColor(255,255,255,255),
				bool useAlphaChannelOfTexture=false) IRR_OVERRIDE;

		//!Draws an 2d rectangle with a gradient.
		virtual void draw2DRectangle(const core::rect<s32>& pos,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32>* clip) IRR_OVERRIDE;

		//! Draws a 2d line.
		virtual void draw2DLine(const core::position2d<s32>& start,
					const core::position2d<s32>& end,
					SColor color=SColor(255,255,255,255)) IRR_OVERRIDE;

		//! Draws a pixel.
		virtual void drawPixel(u32 x, u32 y, const SColor & color) IRR_OVERRIDE;

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
			const core::vector3df& end, SColor color = SColor(255,255,255,255)) IRR_OVERRIDE;

		//! Draws a 3d box.
		virtual void draw3DBox( const core::aabbox3d<f32>& box, SColor color = SColor(255,255,255,255 ) )  IRR_OVERRIDE;

		//! initialises the Direct3D API
		bool initDriver(HWND hwnd, bool pureSoftware);

		//! \return Returns the name of the video driver. Example: In case of the DIRECT3D8
		//! driver, it would return "Direct3D8.1".
		virtual const wchar_t* getName() const IRR_OVERRIDE;

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

		//! Draws a shadow volume into the stencil buffer.
		virtual void drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail=true, u32 debugDataVisible=0) IRR_OVERRIDE;

		//! Fills the stencil shadow with color.
		virtual void drawStencilShadow(bool clearStencilBuffer=false,
			video::SColor leftUpEdge = video::SColor(0,0,0,0),
			video::SColor rightUpEdge = video::SColor(0,0,0,0),
			video::SColor leftDownEdge = video::SColor(0,0,0,0),
			video::SColor rightDownEdge = video::SColor(0,0,0,0)) IRR_OVERRIDE;

		//! Returns the maximum amount of primitives (mostly vertices) which
		//! the device is able to render with one drawIndexedTriangleList
		//! call.
		virtual u32 getMaximalPrimitiveCount() const IRR_OVERRIDE;

		//! Sets the fog mode.
		virtual void setFog(SColor color, E_FOG_TYPE fogType, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog) IRR_OVERRIDE;

		//! Only used by the internal engine. Used to notify the driver that
		//! the window was resized.
		virtual void OnResize(const core::dimension2d<u32>& size) IRR_OVERRIDE;

		//! Can be called by an IMaterialRenderer to make its work easier.
		virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial,
			bool resetAllRenderstates) IRR_OVERRIDE;

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType() const IRR_OVERRIDE;

		//! Returns the transformation set by setTransform
		virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const IRR_OVERRIDE;

		//! Get a vertex shader constant index.
		virtual s32 getVertexShaderConstantID(const c8* name) IRR_OVERRIDE;

		//! Get a pixel shader constant index.
		virtual s32 getPixelShaderConstantID(const c8* name) IRR_OVERRIDE;

		//! Sets a vertex shader constant.
		virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1) IRR_OVERRIDE;

		//! Sets a pixel shader constant.
		virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1) IRR_OVERRIDE;

		//! Sets a constant for the vertex shader based on an index.
		virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count) IRR_OVERRIDE;

		//! Int interface for the above.
		virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count) IRR_OVERRIDE;

		//! Uint interface for the above.
		virtual bool setVertexShaderConstant(s32 index, const u32* ints, int count) IRR_OVERRIDE;

		//! Sets a constant for the pixel shader based on an index.
		virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count) IRR_OVERRIDE;

		//! Int interface for the above.
		virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count) IRR_OVERRIDE;

		//! Uint interface for the above.
		virtual bool setPixelShaderConstant(s32 index, const u32* ints, int count) IRR_OVERRIDE;

		//! Returns a pointer to the IVideoDriver interface. (Implementation for
		//! IMaterialRendererServices)
		virtual IVideoDriver* getVideoDriver() IRR_OVERRIDE;

		//! Creates a render target texture.
		virtual ITexture* addRenderTargetTexture(const core::dimension2d<u32>& size,
				const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN) IRR_OVERRIDE;

		//! Creates a render target texture for a cubemap
		ITexture* addRenderTargetTextureCubemap(const irr::u32 sideLen,
				const io::path& name, const ECOLOR_FORMAT format) IRR_OVERRIDE;

		virtual void clearBuffers(u16 flag, SColor color = SColor(255,0,0,0), f32 depth = 1.f, u8 stencil = 0) IRR_OVERRIDE;

		//! Returns an image created from the last rendered frame.
		virtual IImage* createScreenShot(video::ECOLOR_FORMAT format=video::ECF_UNKNOWN, video::E_RENDER_TARGET target=video::ERT_FRAME_BUFFER) IRR_OVERRIDE;

		//! Set/unset a clipping plane.
		virtual bool setClipPlane(u32 index, const core::plane3df& plane, bool enable=false) IRR_OVERRIDE;

		//! Enable/disable a clipping plane.
		virtual void enableClipPlane(u32 index, bool enable) IRR_OVERRIDE;

		//! Returns the graphics card vendor name.
		virtual core::stringc getVendorInfo() IRR_OVERRIDE {return VendorName;}

		//! Enable the 2d override material
		virtual void enableMaterial2D(bool enable=true) IRR_OVERRIDE;

		//! Check if the driver was recently reset.
		virtual bool checkDriverReset() IRR_OVERRIDE {return DriverWasReset;}

		//! Get the current color format of the color buffer
		/** \return Color format of the color buffer. */
		virtual ECOLOR_FORMAT getColorFormat() const IRR_OVERRIDE;

		//! Returns the maximum texture size supported.
		virtual core::dimension2du getMaxTextureSize() const IRR_OVERRIDE;

		//! Check if the driver supports creating textures with the given color format
		virtual bool queryTextureFormat(ECOLOR_FORMAT format) const IRR_OVERRIDE;

		//! Used by some SceneNodes to check if a material should be rendered in the transparent render pass
		virtual bool needsTransparentRenderPass(const irr::video::SMaterial& material) const IRR_OVERRIDE;

		//! Get the current color format of the color buffer
		/** \return Color format of the color buffer as D3D color value. */
		D3DFORMAT getD3DColorFormat() const;

		//! Get D3D color format from Irrlicht color format.
		D3DFORMAT getD3DFormatFromColorFormat(ECOLOR_FORMAT format) const;

		//! Get Irrlicht color format from D3D color format.
		ECOLOR_FORMAT getColorFormatFromD3DFormat(D3DFORMAT format) const;

		//! Get D3D blending factor.
		u32 getD3DBlend(E_BLEND_FACTOR factor) const;

		//! Get D3D modulate.
		u32 getD3DModulate(E_MODULATE_FUNC func) const;

		//! Get bridge calls.
		CD3D9CallBridge* getBridgeCalls() const;

	private:

		//! enumeration for rendering modes such as 2d and 3d for minimizing the switching of renderStates.
		enum E_RENDER_MODE
		{
			ERM_NONE = 0,	// no render state has been set yet.
			ERM_2D,			// 2d drawing rendermode
			ERM_3D,			// 3d rendering mode
			ERM_STENCIL_FILL, // stencil fill mode
			ERM_SHADOW_VOLUME_ZFAIL, // stencil volume draw mode
			ERM_SHADOW_VOLUME_ZPASS // stencil volume draw mode
		};

		//! sets right vertex shader
		void setVertexShader(video::E_VERTEX_TYPE newType);

		//! sets the needed renderstates
		bool setRenderStates3DMode();

		//! sets the needed renderstates
		void setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel);

		//! sets the needed renderstates
		void setRenderStatesStencilFillMode(bool alpha);

		//! sets the needed renderstates
		void setRenderStatesStencilShadowMode(bool zfail, u32 debugDataVisible);

		//! sets the current Texture
		bool setActiveTexture(u32 stage, const video::ITexture* texture);

		//! resets the device
		bool reset();

		//! Try to get back a lost device
		bool retrieveDevice(int numTries, int msSleepBetweenTries=100);

		virtual ITexture* createDeviceDependentTexture(const io::path& name, IImage* image) IRR_OVERRIDE;

		virtual ITexture* createDeviceDependentTextureCubemap(const io::path& name, const core::array<IImage*>& image) IRR_OVERRIDE;

		//! Adds a new material renderer to the VideoDriver, using pixel and/or
		//! vertex shaders to render geometry.
		s32 addShaderMaterial(const c8* vertexShaderProgram, const c8* pixelShaderProgram,
			IShaderConstantSetCallBack* callback,
			E_MATERIAL_TYPE baseMaterial, s32 userData) IRR_OVERRIDE;

		//! Adds a new material renderer to the VideoDriver, based on a high level shading
		//! language.
		virtual s32 addHighLevelShaderMaterial(
			const c8* vertexShaderProgram,
			const c8* vertexShaderEntryPointName,
			E_VERTEX_SHADER_TYPE vsCompileTarget,
			const c8* pixelShaderProgram,
			const c8* pixelShaderEntryPointName,
			E_PIXEL_SHADER_TYPE psCompileTarget,
			const c8* geometryShaderProgram,
			const c8* geometryShaderEntryPointName = "main",
			E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
			scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
			scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData = 0) IRR_OVERRIDE;

		void createMaterialRenderers();

		void draw2D3DVertexPrimitiveList(const void* vertices,
				u32 vertexCount, const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
				E_INDEX_TYPE iType, bool is3D);

		D3DTEXTUREADDRESS getTextureWrapMode(const u8 clamp);

		inline D3DCOLORVALUE colorToD3D(const SColor& col)
		{
			const f32 f = 1.0f / 255.0f;
			D3DCOLORVALUE v;
			v.r = col.getRed() * f;
			v.g = col.getGreen() * f;
			v.b = col.getBlue() * f;
			v.a = col.getAlpha() * f;
			return v;
		}

		CD3D9CallBridge* BridgeCalls;

		E_RENDER_MODE CurrentRenderMode;
		D3DPRESENT_PARAMETERS present;

		SMaterial Material, LastMaterial;
		bool ResetRenderStates; // bool to make all renderstates be reseted if set.
		bool Transformation3DChanged;
		const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
		bool LastTextureMipMapsAvailable[MATERIAL_MAX_TEXTURES];
		core::matrix4 Matrices[ETS_COUNT]; // matrices of the 3d mode we need to restore when we switch back from the 2d mode.

		HINSTANCE D3DLibrary;
		IDirect3D9* pID3D;
		IDirect3DDevice9* pID3DDevice;

		IDirect3DSurface9* BackBufferSurface;
		IDirect3DSurface9* DepthStencilSurface;

		core::array<bool> ActiveRenderTarget;

		HWND WindowId;
		core::rect<s32>* SceneSourceRect;

		D3DCAPS9 Caps;

		SIrrlichtCreationParameters Params;

		E_VERTEX_TYPE LastVertexType;

		core::stringc VendorName;
		u16 VendorID;

		u32 MaxTextureUnits;
		u32 MaxFixedPipelineTextureUnits;
		u32 MaxUserClipPlanes;
		f32 MaxLightDistance;
		s32 LastSetLight;

		enum E_CACHE_2D_ATTRIBUTES
		{
			EC2D_ALPHA = 0x1,
			EC2D_TEXTURE = 0x2,
			EC2D_ALPHA_CHANNEL = 0x4
		};

		ECOLOR_FORMAT ColorFormat;
		D3DFORMAT D3DColorFormat;
		bool DeviceLost;
		bool DriverWasReset;
		bool OcclusionQuerySupport;
		bool AlphaToCoverageSupport;
	};

	//! This bridge between Irrlicht pseudo D3D9 calls
	//! and true D3D9 calls.

	class CD3D9CallBridge
	{
	public:
		CD3D9CallBridge(IDirect3DDevice9* p, CD3D9Driver* driver);

		// Reset to default state.

		void reset();

		// Blending calls.

		void setBlendOperation(DWORD mode);

		void setBlendFunc(DWORD source, DWORD destination);

		void setBlendFuncSeparate(DWORD sourceRGB, DWORD destinationRGB, DWORD sourceAlpha, DWORD destinationAlpha);

		void setBlend(bool enable);

	private:
		IDirect3DDevice9* pID3DDevice;

		DWORD BlendOperation;
		DWORD BlendSourceRGB;
		DWORD BlendDestinationRGB;
		DWORD BlendSourceAlpha;
		DWORD BlendDestinationAlpha;
		bool Blend;
		bool BlendSeparate;

		bool FeatureBlendSeparate;
	};

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_DIRECT3D_9_
#endif // IRR_C_VIDEO_DIRECTX_9_H_INCLUDED
