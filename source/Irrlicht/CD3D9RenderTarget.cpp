// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CD3D9RenderTarget.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_

#include "IImage.h"
#include "irrMath.h"
#include "irrString.h"

#include "CD3D9Driver.h"
#include "CD3D9Texture.h"

namespace irr
{
	namespace video
	{

		CD3D9RenderTarget::CD3D9RenderTarget(CD3D9Driver* driver) : DepthStencilSurface(0), Driver(driver)
		{
#ifdef _DEBUG
			setDebugName("CD3D9RenderTarget");
#endif

			DriverType = EDT_DIRECT3D9;
			Size = Driver->getScreenSize();
		}

		CD3D9RenderTarget::~CD3D9RenderTarget()
		{
			for (u32 i = 0; i < Surfaces.size(); ++i)
			{
				if (Surfaces[i])
					Surfaces[i]->Release();
			}

			if (DepthStencilSurface)
				DepthStencilSurface->Release();

			for (u32 i = 0; i < Textures.size(); ++i)
			{
				if (Textures[i])
					Textures[i]->drop();
			}

			if (DepthStencil)
				DepthStencil->drop();
		}

		void CD3D9RenderTarget::setTextures(ITexture* const * textures, u32 numTextures, ITexture* depthStencil, const E_CUBE_SURFACE* cubeSurfaces, u32 numCubeSurfaces)
		{
			bool needSizeUpdate = false;

			// Set color attachments.
			if (!Textures.equals(textures, numTextures) || !CubeSurfaces.equals(cubeSurfaces, numCubeSurfaces))
			{
				needSizeUpdate = true;
				CubeSurfaces.set_data(cubeSurfaces, numCubeSurfaces);	// TODO: we can probably avoid some memory allocating/de-allocating if _only_ CubeSurfaces change.

				if (numTextures > Driver->ActiveRenderTarget.size())
				{
					core::stringc message = "This GPU supports up to ";
					message += Driver->ActiveRenderTarget.size();
					message += " textures per render target.";

					os::Printer::log(message.c_str(), ELL_WARNING);
				}

				const u32 size = core::min_(numTextures, static_cast<u32>(Driver->ActiveRenderTarget.size()));

				for (u32 i = 0; i < Surfaces.size(); ++i)
				{
					if (Surfaces[i])
						Surfaces[i]->Release();
				}

				Surfaces.set_used(size);

				core::array<ITexture*> prevTextures(Textures);

				Textures.set_used(size);

				for (u32 i = 0; i < size; ++i)
				{
					CD3D9Texture* currentTexture = (textures[i] && textures[i]->getDriverType() == DriverType) ? static_cast<CD3D9Texture*>(textures[i]) : 0;

					IDirect3DTexture9* textureID = 0;
					IDirect3DCubeTexture9* cubeTextureId = 0;
					UINT level = 0;	// no support for rendering to to other mip-levels so far

					if (currentTexture)
					{
						if (currentTexture->getType() == ETT_2D)
							textureID = currentTexture->getDX9Texture();
						else if ( currentTexture->getType() == ETT_CUBEMAP )
							cubeTextureId = currentTexture->getDX9CubeTexture();
					}

					if (textureID)
					{
						Textures[i] = textures[i];
						Textures[i]->grab();

						IDirect3DSurface9* currentSurface = 0;
						textureID->GetSurfaceLevel(level, &currentSurface);

						Surfaces[i] = currentSurface;
					}
					else if ( cubeTextureId )
					{
						Textures[i] = textures[i];
						Textures[i]->grab();

						IDirect3DSurface9* currentSurface = 0;
						D3DCUBEMAP_FACES face = (D3DCUBEMAP_FACES)CubeSurfaces[i];	// we use same numbering
						cubeTextureId->GetCubeMapSurface(face, level, &currentSurface);

						Surfaces[i] = currentSurface;
					}
					else
					{
						Surfaces[i] = 0;
						Textures[i] = 0;
					}
				}

				for (u32 i = 0; i < prevTextures.size(); ++i)
				{
					if (prevTextures[i])
						prevTextures[i]->drop();
				}
			}

			// Set depth and stencil attachments.

			if (DepthStencil != depthStencil)
			{
				if (DepthStencilSurface)
				{
					DepthStencilSurface->Release();
					DepthStencilSurface = 0;
				}

				if (DepthStencil)
				{
					DepthStencil->drop();
					DepthStencil = 0;

					DepthStencilSurface = 0;
				}

				needSizeUpdate = true;
				CD3D9Texture* currentTexture = (depthStencil && depthStencil->getDriverType() == DriverType) ? static_cast<CD3D9Texture*>(depthStencil) : 0;

				if (currentTexture)
				{
					if (currentTexture->getType() == ETT_2D)
					{
						IDirect3DTexture9* textureID = currentTexture->getDX9Texture();
						if (textureID)
						{
							const ECOLOR_FORMAT textureFormat = depthStencil->getColorFormat();
							if (IImage::isDepthFormat(textureFormat))
							{
								DepthStencil = depthStencil;
								DepthStencil->grab();

								IDirect3DSurface9* currentSurface = 0;
								textureID->GetSurfaceLevel(0, &currentSurface);

								DepthStencilSurface = currentSurface;
							}
							else
							{
								os::Printer::log("Ignoring depth/stencil texture without depth color format.", ELL_WARNING);
							}
						}
					}
					else
						os::Printer::log("This driver doesn't support depth/stencil to cubemaps.", ELL_WARNING);
				}
			}

			if (needSizeUpdate)
			{
				// Set size required for a viewport.

				bool sizeDetected = false;

				for (u32 i = 0; i < Textures.size(); ++i)
				{
					if (Textures[i])
					{
						Size = Textures[i]->getSize();
						sizeDetected = true;

						break;
					}
				}

				if (!sizeDetected)
				{
					if (DepthStencil)
						Size = DepthStencil->getSize();
					else
						Size = Driver->getScreenSize();
				}
			}
		}

		const core::dimension2d<u32>& CD3D9RenderTarget::getSize() const
		{
			return Size;
		}

		IDirect3DSurface9* CD3D9RenderTarget::getSurface(u32 id) const
		{
			return (id < Surfaces.size()) ? Surfaces[id] : 0;
		}

		u32 CD3D9RenderTarget::getSurfaceCount() const
		{
			return Surfaces.size();
		}

		IDirect3DSurface9* CD3D9RenderTarget::getDepthStencilSurface() const
		{
			return DepthStencilSurface;
		}

		void CD3D9RenderTarget::releaseSurfaces()
		{
			for (u32 i = 0; i < Surfaces.size(); ++i)
			{
				if (Surfaces[i])
				{
					Surfaces[i]->Release();
					Surfaces[i] = 0;
				}
			}

			if (DepthStencilSurface)
			{
				DepthStencilSurface->Release();
				DepthStencilSurface = 0;
			}
		}

		void CD3D9RenderTarget::generateSurfaces()
		{
			for (u32 i = 0; i < Surfaces.size(); ++i)
			{
				if (!Surfaces[i] && Textures[i])
				{
					IDirect3DTexture9* currentTexture = static_cast<CD3D9Texture*>(Textures[i])->getDX9Texture();
					if ( currentTexture )
					{
						IDirect3DSurface9* currentSurface = 0;
						currentTexture->GetSurfaceLevel(0, &currentSurface);
						Surfaces[i] = currentSurface;
					}
				}
			}

			if (!DepthStencilSurface && DepthStencil)
			{
				IDirect3DTexture9* currentTexture = static_cast<CD3D9Texture*>(DepthStencil)->getDX9Texture();
				if ( currentTexture )
				{
					IDirect3DSurface9* currentSurface = 0;
					currentTexture->GetSurfaceLevel(0, &currentSurface);
					DepthStencilSurface = currentSurface;
				}
			}
		}
	}
}

#endif
