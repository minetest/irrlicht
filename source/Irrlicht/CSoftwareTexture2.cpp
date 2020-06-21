// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_

#include "SoftwareDriver2_compile_config.h"
#include "SoftwareDriver2_helper.h"
#include "CSoftwareTexture2.h"
#include "CSoftwareDriver2.h"
#include "CBlit.h"
#include "os.h"

namespace irr
{
namespace video
{

//! stretches srcRect src to dstRect dst, applying a sliding window box filter in linear color space (sRGB->linear->sRGB)
void Resample_subSampling(eBlitter op, video::IImage* dst, const core::rect<s32>* dstRect, const video::IImage* src, const core::rect<s32>* srcRect, size_t flags);

//! constructor
CSoftwareTexture2::CSoftwareTexture2(IImage* image, const io::path& name, u32 flags, CBurningVideoDriver* driver)
	: ITexture(name
#if !defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
		, ETT_2D
#endif
	)
	,MipMapLOD(0), Flags(flags), Driver(driver)
{
	#ifdef _DEBUG
	setDebugName("CSoftwareTexture2");
	#endif

#ifndef SOFTWARE_DRIVER_2_MIPMAPPING
	Flags &= ~(GEN_MIPMAP| GEN_MIPMAP_AUTO);
#endif
	//set baseclass properties
	DriverType = EDT_BURNINGSVIDEO;
	ColorFormat = BURNINGSHADER_COLOR_FORMAT;
	IsRenderTarget = (Flags & IS_RENDERTARGET) != 0;
	HasMipMaps = (Flags & GEN_MIPMAP) != 0;
	MipMap0_Area[0] = 1;
	MipMap0_Area[1] = 1;
	LodBIAS = 0.75f;
	for ( size_t i = 0; i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i ) MipMap[i] = 0;
	if (!image) return;

	OriginalSize = image->getDimension();
	OriginalColorFormat = image->getColorFormat();


#if defined(IRRLICHT_sRGB)
	if ( Flags & IMAGE_IS_LINEAR ) image->set_sRGB(0);
#else
	//guessing linear image
	if (name.find("light") >= 0 ||
		name.find("bump") >= 0 ||
		name.find("height") >= 0
		)
	{
		Flags |= TEXTURE_IS_LINEAR | IMAGE_IS_LINEAR;
	}
#endif

	bool isCompressed = IImage::isCompressedFormat(OriginalColorFormat);
	if (isCompressed)
	{
		os::Printer::log("Texture compression not available.", ELL_ERROR);
	}

	//visual studio code warning
	u32 maxTexSize = SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE;
	core::dimension2d<u32> optSize(	OriginalSize.getOptimalSize(
			(Flags & ALLOW_NPOT) ? 0 : 1, // requirePowerOfTwo
			false, // requireSquare
			(Flags & ALLOW_NPOT) ? 1 : maxTexSize == 0, // larger
			(Flags & ALLOW_NPOT) ? 0 : maxTexSize // maxValue
		)
	);

	if (OriginalSize == optSize)
	{
		MipMap[0] = new CImage(BURNINGSHADER_COLOR_FORMAT, image->getDimension());
#if defined(IRRLICHT_sRGB)
		MipMap[0]->set_sRGB( (Flags & TEXTURE_IS_LINEAR ) ? 0 : image->get_sRGB()  );
#endif
		if (!isCompressed)
			image->copyTo(MipMap[0]);
	}
	else
	{
		char buf[256];
		core::stringw showName ( name );
		snprintf_irr ( buf, sizeof(buf), "Burningvideo: Warning Texture %ls reformat %ux%u,%d -> %ux%u,%d",
						showName.c_str(),
						OriginalSize.Width, OriginalSize.Height, OriginalColorFormat,
						optSize.Width, optSize.Height,BURNINGSHADER_COLOR_FORMAT
					);

		os::Printer::log ( buf, ELL_WARNING );
		MipMap[0] = new CImage(BURNINGSHADER_COLOR_FORMAT, optSize);
#if defined(IRRLICHT_sRGB)
		MipMap[0]->set_sRGB( (Flags & TEXTURE_IS_LINEAR ) ? 0 : image->get_sRGB()  );
#endif
		if (!isCompressed)
		{
			//image->copyToScalingBoxFilter ( MipMap[0],0, false );
			Resample_subSampling(BLITTER_TEXTURE,MipMap[0],0,image,0, Flags);
		}
		// if Original Size is used for calculation ( 2D position, font) it will be wrong
		//OriginalSize = optSize;
	}

	//select highest mipmap 0
	regenerateMipMapLevels(image->getMipMapsData());
}


//! destructor
CSoftwareTexture2::~CSoftwareTexture2()
{
	for ( size_t i = 0; i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i )
	{
		if ( MipMap[i] )
		{
			MipMap[i]->drop();
			MipMap[i] = 0;
		}
	}
}



//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
#if !defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
void CSoftwareTexture2::regenerateMipMapLevels(void* data, u32 layer)
#else
void CSoftwareTexture2::regenerateMipMapLevels(void* data)
#endif
{
	int i;

	// release
	for ( i = 1; i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i )
	{
		if ( MipMap[i] )
		{
			MipMap[i]->drop();
			MipMap[i] = 0;
		}
	}

	core::dimension2d<u32> newSize;

	if (HasMipMaps && ( (Flags & GEN_MIPMAP_AUTO) || 0 == data ) )
	{
		//need memory also if autogen mipmap disabled
		for (i = 1; i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i)
		{
			const core::dimension2du& upperDim = MipMap[i - 1]->getDimension();
			//isotropic
			newSize.Width = core::s32_max(SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE, upperDim.Width >> 1);
			newSize.Height = core::s32_max(SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE, upperDim.Height >> 1);
			if (upperDim == newSize)
				break;

			MipMap[i] = new CImage(BURNINGSHADER_COLOR_FORMAT, newSize);
#if defined(IRRLICHT_sRGB)
			MipMap[i]->set_sRGB(MipMap[i - 1]->get_sRGB());
#endif
			//MipMap[i]->fill ( 0xFFFF4040 );
			//MipMap[i-1]->copyToScalingBoxFilter( MipMap[i], 0, false );
			Resample_subSampling(BLITTER_TEXTURE, MipMap[i], 0, MipMap[0], 0, Flags);
		}
	}
	else if (HasMipMaps && data)
	{
		//deactivated outside mipdata until TA knows how to handle this.

		//query mipmap dimension
		u8* mip_current = (u8*)data;
		const u8* mip_end = (u8*)data;

		core::dimension2d<u32> origSize = OriginalSize;
		i = 1;
		do
		{
			if (origSize.Width > 1) origSize.Width >>= 1;
			if (origSize.Height > 1) origSize.Height >>= 1;
			mip_end += IImage::getDataSizeFromFormat(OriginalColorFormat, origSize.Width, origSize.Height);
			i += 1;
		} while ((origSize.Width != 1 || origSize.Height != 1) && i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX);

		//TODO: this is not true
		LodBIAS = i*0.75f;

		origSize = OriginalSize;
		for (i = 1; i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX && mip_current < mip_end; ++i)
		{
			const core::dimension2du& upperDim = MipMap[i - 1]->getDimension();
			//isotropic
			newSize.Width = core::s32_max(SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE, upperDim.Width >> 1);
			newSize.Height = core::s32_max(SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE, upperDim.Height >> 1);
			if (upperDim == newSize)
				break;

			if (origSize.Width > 1) origSize.Width >>= 1;
			if (origSize.Height > 1) origSize.Height >>= 1;

			if (OriginalColorFormat != BURNINGSHADER_COLOR_FORMAT)
			{
				IImage* tmpImage = new CImage(OriginalColorFormat, origSize, mip_current, true, false);
				MipMap[i] = new CImage(BURNINGSHADER_COLOR_FORMAT, newSize);
				if (origSize == newSize)
					tmpImage->copyTo(MipMap[i]);
				else
					tmpImage->copyToScalingBoxFilter(MipMap[i]);
				tmpImage->drop();
			}
			else
			{
				if (origSize == newSize)
					MipMap[i] = new CImage(BURNINGSHADER_COLOR_FORMAT, newSize, mip_current, false);
				else
				{
					MipMap[i] = new CImage(BURNINGSHADER_COLOR_FORMAT, newSize);
					IImage* tmpImage = new CImage(BURNINGSHADER_COLOR_FORMAT, origSize, mip_current, true, false);
					tmpImage->copyToScalingBoxFilter(MipMap[i]);
					tmpImage->drop();
				}
			}
			mip_current += IImage::getDataSizeFromFormat(OriginalColorFormat, origSize.Width, origSize.Height);
		}
	}


	//visualize mipmap
	for (i=1; i < 0 && i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i)
	{
/*
		static u32 color[] = {
			0x30bf7f00,0x3040bf00,0x30bf00bf,0x3000bf00,
			0x300080bf,0x30bf4000,0x300040bf,0x307f00bf,
			0x30bf0000,0x3000bfbf,0x304000bf,0x307fbf00,
			0x8000bf7f,0x80bf0040,0x80bfbf00,0x800000bf
		};
*/
		static u32 color[] = {
			0xFFFFFFFF,0xFFFF0000,0xFF00FF00,0xFF0000FF,
			0xFFFFFF00,0xFF00FFFF,0xFFFF00FF,0xFF0000FF,
			0xFF0000FF,0xFF0000FF,0xFF0000FF,0xFF0000FF,
			0xFF0000FF,0xFF0000FF,0xFF0000FF,0xFFFF00FF
		};

		if ( MipMap[i] )
		{
			core::rect<s32> p (core::position2di(0,0),MipMap[i]->getDimension());
			SColor c((color[i & 15] & 0x00FFFFFF) | 0xFF000000);
			Blit(BLITTER_TEXTURE_ALPHA_COLOR_BLEND, MipMap[i], 0, 0, MipMap[i], &p, c.color);
		}
	}

	//save mipmap chain
	if ( 0 )
	{
		char buf[256];
		const char* name = getName().getPath().c_str();
		int filename = 0;
		//int ext = -1;
		i = 0;
		while (name[i])
		{
			if (name[i] == '/' || name[i] == '\\') filename = i + 1;
			//if (name[i] == '.') ext = i;
			i += 1;
		}
		for (i = 0; i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i)
		{
			if (MipMap[i])
			{
				snprintf_irr(buf, sizeof(buf),"mip/%s_%02d.png", name + filename,i);
				Driver->writeImageToFile(MipMap[i], buf);
			}
		}
	}

	calcDerivative();
}

void CSoftwareTexture2::calcDerivative()
{
	//reset current MipMap
	MipMapLOD = 0;
	if (MipMap[0])
	{
		const core::dimension2du& dim = MipMap[0]->getDimension();
		MipMap0_Area[0] = dim.Width;
		MipMap0_Area[1] = dim.Height; // screensize of a triangle

		Size = dim; // MipMap[MipMapLOD]->getDimension();
		Pitch = MipMap[MipMapLOD]->getPitch();
	}

	//preCalc mipmap texel center boundaries
	for ( s32 i = 0; i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i )
	{
		CSoftwareTexture2_Bound& b = TexBound[i];
		if (MipMap[i])
		{
			const core::dimension2du& dim = MipMap[i]->getDimension();
			//f32 u = 1.f / dim.Width;
			//f32 v = 1.f / dim.Height;

			b.w = dim.Width - 1.f;
			b.h = dim.Height - 1.f;
			b.cx = 0.f; //u*0.005f;
			b.cy = 0.f; //v*0.005f;
		}
		else
		{
			b.w = 0.f;
			b.h = 0.f;
			b.cx = 0.f;
			b.cy = 0.f;
		}
	}

}


/* Software Render Target 2 */

CSoftwareRenderTarget2::CSoftwareRenderTarget2(CBurningVideoDriver* driver) : Driver(driver)
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
, IRenderTarget(0)
#endif
{
	DriverType = EDT_BURNINGSVIDEO;

	Texture.set_used(1);
	Texture[0] = 0;
}

CSoftwareRenderTarget2::~CSoftwareRenderTarget2()
{
	if (Texture[0])
		Texture[0]->drop();
}

void CSoftwareRenderTarget2::setTexture(const core::array<ITexture*>& texture, ITexture* depthStencil, const core::array<E_CUBE_SURFACE>& cubeSurfaces)
{
	if (Texture != texture)
	{
		ITexture* prevTexture = Texture[0];

		bool textureDetected = false;

		for (u32 i = 0; i < texture.size(); ++i)
		{
			if (texture[i] && texture[i]->getDriverType() == EDT_BURNINGSVIDEO)
			{
				Texture[0] = texture[i];
				Texture[0]->grab();
				textureDetected = true;

				break;
			}
		}

		if (prevTexture)
			prevTexture->drop();

		if (!textureDetected)
			Texture[0] = 0;
	}
}

ITexture* CSoftwareRenderTarget2::getTexture() const
{
	return Texture[0];
}



static const float srgb_8bit_to_linear_float[1 << 8] = {
	0.0f, 3.03527e-4f, 6.07054e-4f, 9.10581e-4f,
	0.001214108f, 0.001517635f, 0.001821162f, 0.0021246888f,
	0.002428216f, 0.002731743f, 0.00303527f, 0.0033465358f,
	0.0036765074f, 0.004024717f, 0.004391442f, 0.0047769537f,
	0.005181517f, 0.005605392f, 0.0060488335f, 0.006512091f,
	0.0069954107f, 0.007499032f, 0.008023193f, 0.008568126f,
	0.009134059f, 0.009721218f, 0.010329823f, 0.010960095f,
	0.011612245f, 0.012286489f, 0.0129830325f, 0.013702083f,
	0.014443845f, 0.015208516f, 0.015996294f, 0.016807377f,
	0.017641956f, 0.018500222f, 0.019382363f, 0.020288564f,
	0.021219011f, 0.022173885f, 0.023153368f, 0.024157634f,
	0.025186861f, 0.026241222f, 0.027320893f, 0.02842604f,
	0.029556835f, 0.030713445f, 0.031896032f, 0.033104766f,
	0.034339808f, 0.035601314f, 0.036889452f, 0.038204372f,
	0.039546236f, 0.0409152f, 0.04231141f, 0.04373503f,
	0.045186203f, 0.046665087f, 0.048171826f, 0.049706567f,
	0.051269464f, 0.05286065f, 0.05448028f, 0.056128494f,
	0.057805438f, 0.059511244f, 0.06124606f, 0.06301002f,
	0.06480327f, 0.066625945f, 0.068478175f, 0.0703601f,
	0.07227185f, 0.07421357f, 0.07618539f, 0.07818743f,
	0.08021983f, 0.082282715f, 0.084376216f, 0.086500466f,
	0.08865559f, 0.09084172f, 0.093058966f, 0.09530747f,
	0.097587354f, 0.09989873f, 0.10224174f, 0.10461649f,
	0.107023105f, 0.10946172f, 0.111932434f, 0.11443538f,
	0.11697067f, 0.119538434f, 0.122138776f, 0.12477182f,
	0.12743768f, 0.13013647f, 0.13286832f, 0.13563333f,
	0.13843162f, 0.14126329f, 0.14412847f, 0.14702727f,
	0.14995979f, 0.15292616f, 0.15592647f, 0.15896083f,
	0.16202939f, 0.1651322f, 0.1682694f, 0.17144111f,
	0.1746474f, 0.17788842f, 0.18116425f, 0.18447499f,
	0.18782078f, 0.19120169f, 0.19461784f, 0.19806932f,
	0.20155625f, 0.20507874f, 0.20863687f, 0.21223076f,
	0.21586053f, 0.21952623f, 0.22322798f, 0.2269659f,
	0.23074007f, 0.23455061f, 0.2383976f, 0.24228115f,
	0.24620135f, 0.2501583f, 0.25415212f, 0.25818288f,
	0.2622507f, 0.26635563f, 0.27049783f, 0.27467734f,
	0.2788943f, 0.28314877f, 0.28744087f, 0.29177067f,
	0.2961383f, 0.3005438f, 0.30498734f, 0.30946895f,
	0.31398875f, 0.3185468f, 0.32314324f, 0.32777813f,
	0.33245155f, 0.33716366f, 0.34191445f, 0.3467041f,
	0.35153264f, 0.35640016f, 0.36130682f, 0.36625263f,
	0.3712377f, 0.37626216f, 0.38132605f, 0.38642946f,
	0.3915725f, 0.39675525f, 0.4019778f, 0.40724024f,
	0.41254264f, 0.4178851f, 0.4232677f, 0.42869052f,
	0.43415368f, 0.4396572f, 0.44520122f, 0.45078582f,
	0.45641103f, 0.46207702f, 0.4677838f, 0.4735315f,
	0.4793202f, 0.48514995f, 0.4910209f, 0.496933f,
	0.5028865f, 0.50888133f, 0.5149177f, 0.5209956f,
	0.52711517f, 0.53327644f, 0.5394795f, 0.5457245f,
	0.55201143f, 0.55834043f, 0.5647115f, 0.57112485f,
	0.57758045f, 0.58407843f, 0.59061885f, 0.5972018f,
	0.60382736f, 0.61049557f, 0.6172066f, 0.62396044f,
	0.63075715f, 0.6375969f, 0.6444797f, 0.65140563f,
	0.65837485f, 0.66538733f, 0.67244315f, 0.6795425f,
	0.6866853f, 0.6938718f, 0.7011019f, 0.7083758f,
	0.71569353f, 0.7230551f, 0.73046076f, 0.73791045f,
	0.74540424f, 0.7529422f, 0.7605245f, 0.76815116f,
	0.7758222f, 0.7835378f, 0.791298f, 0.7991027f,
	0.8069523f, 0.8148466f, 0.82278574f, 0.8307699f,
	0.838799f, 0.8468732f, 0.8549926f, 0.8631572f,
	0.8713671f, 0.8796224f, 0.8879231f, 0.8962694f,
	0.9046612f, 0.91309863f, 0.92158186f, 0.9301109f,
	0.9386857f, 0.9473065f, 0.9559733f, 0.9646863f,
	0.9734453f, 0.9822506f, 0.9911021f, 1.0f,
};
/*
int linear_to_srgb_8bit(const float x) {
	if (x <= 0.f) return 0;
	if (x >= 1.f) return 255;
	const float *table = SRGB_8BIT_TO_LINEAR_FLOAT;
	int y = 0;
	for (int i = 128; i != 0; i >>= 1) {
		if (table[y + i] <= x)
			y += i;
	}
	if (x - table[y] <= table[y + 1] - x)
		return y;
	else
		return y + 1;
}
*/


u32 linear_to_srgb_8bit(const float v)
{
	ieee754 c;
	c.f = v;
	const size_t x = c.u;
	const u32 *table = (u32*)srgb_8bit_to_linear_float;
	u32 y = 0;
	y += table[y + 128] <= x ? 128 : 0;
	y += table[y + 64] <= x ? 64 : 0;
	y += table[y + 32] <= x ? 32 : 0;
	y += table[y + 16] <= x ? 16 : 0;
	y += table[y + 8] <= x ? 8 : 0;
	y += table[y + 4] <= x ? 4 : 0;
	y += table[y + 2] <= x ? 2 : 0;
	y += table[y + 1] <= x ? 1 : 0;

	return y;
}

// 2D Region half open [x0;x1[
struct absrect2
{
	s32 x0;
	s32 y0;
	s32 x1;
	s32 y1;
};

static inline int clipTest(absrect2 &o, const core::rect<s32>* a, const absrect2& b)
{
	if (a == 0)
	{
		o.x0 = b.x0;
		o.y0 = b.y0;
		o.x1 = b.x1;
		o.y1 = b.y1;
	}
	else
	{
		o.x0 = core::s32_max(a->UpperLeftCorner.X, b.x0);
		o.x1 = core::s32_min(a->LowerRightCorner.X, b.x1);
		o.y0 = core::s32_max(a->UpperLeftCorner.Y, b.y0);
		o.y1 = core::s32_min(a->LowerRightCorner.Y, b.y1);
	}
	int clipTest = 0;
	clipTest |= o.x0 >= o.x1 ? 1 : 0;
	clipTest |= o.y0 >= o.y1 ? 2 : 0;
	return clipTest;
}

//! stretches srcRect src to dstRect dst, applying a sliding window box filter in linear color space (sRGB->linear->sRGB)
// todo: texture jumps (mip selection problem)
void Resample_subSampling(eBlitter op, video::IImage* dst, const core::rect<s32>* dstRect,
	const video::IImage* src, const core::rect<s32>* srcRect,size_t flags)
{
	const absrect2 dst_clip = { 0,0,(s32)dst->getDimension().Width,(s32)dst->getDimension().Height };
	absrect2 dc;
	if (clipTest(dc, dstRect, dst_clip)) return;
	const video::ECOLOR_FORMAT dstFormat = dst->getColorFormat();
	u8* dstData = (u8*)dst->getData();

	const absrect2 src_clip = { 0,0,(s32)src->getDimension().Width,(s32)src->getDimension().Height };
	absrect2 sc;
	if (clipTest(sc, srcRect, src_clip)) return;
	const video::ECOLOR_FORMAT srcFormat = src->getColorFormat();
	const u8* srcData = (u8*)src->getData();

#if defined(IRRLICHT_sRGB)
	const int dst_sRGB = dst->get_sRGB();
	const int src_sRGB = src->get_sRGB();
#else
	const int dst_sRGB = (flags & CSoftwareTexture2::TEXTURE_IS_LINEAR) ? 0 : 1;
	const int src_sRGB = (flags & CSoftwareTexture2::IMAGE_IS_LINEAR) ? 0 : 1;
#endif



	float scale[2];
	scale[0] = (float)(sc.x1 - sc.x0) / (float)(dc.x1 - dc.x0);
	scale[1] = (float)(sc.y1 - sc.y0) / (float)(dc.y1 - dc.y0);
	const float rs = 1.f / (scale[0] * scale[1]);

	float sum[4];
	u32 sbgra = 0;

	float f[4];
	int fi[4];
	f[3] = (float)sc.y0;
	for (int dy = dc.y0; dy < dc.y1; ++dy)
	{
		f[1] = f[3];
		f[3] = sc.y0 + (dy + 1 - dc.y0)*scale[1];
		if (f[3] >= sc.y1) f[3] = sc.y1 - 0.001f; //todo:1.f/dim should be enough

		f[2] = (float)sc.x0;
		for (int dx = dc.x0; dx < dc.x1; ++dx)
		{
			f[0] = f[2];
			f[2] = sc.x0 + (dx + 1 - dc.x0)*scale[0];
			if (f[2] >= sc.x1) f[2] = sc.x1 - 0.001f;

			//accumulate linear color
			sum[0] = 0.f;
			sum[1] = 0.f;
			sum[2] = 0.f;
			sum[3] = 0.f;

			//sample border
			fi[0] = (int)(f[0]);
			fi[1] = (int)(f[1]);
			fi[2] = (int)(f[2]);
			fi[3] = (int)(f[3]);

			float w[2];
			for (int fy = fi[1]; fy <= fi[3]; ++fy)
			{
				w[1] = 1.f;
				if (fy == fi[1]) w[1] -= f[1] - fy;
				if (fy == fi[3]) w[1] -= fy + 1 - f[3];

				for (int fx = fi[0]; fx <= fi[2]; ++fx)
				{
					w[0] = 1.f;
					if (fx == fi[0]) w[0] -= f[0] - fx;
					if (fx == fi[2]) w[0] -= fx + 1 - f[2];

					const float ws = w[1] * w[0] * rs;

					switch (srcFormat)
					{
					case video::ECF_A1R5G5B5: sbgra = video::A1R5G5B5toA8R8G8B8(*(u16*)(srcData + (fy*src_clip.x1) * 2 + (fx * 2))); break;
					case video::ECF_R5G6B5: sbgra = video::R5G6B5toA8R8G8B8(*(u16*)(srcData + (fy*src_clip.x1) * 2 + (fx * 2))); break;
					case video::ECF_A8R8G8B8: sbgra = *(u32*)(srcData + (fy*src_clip.x1) * 4 + (fx * 4)); break;
					case video::ECF_R8G8B8:
					{
						const u8* p = srcData + (fy*src_clip.x1) * 3 + (fx * 3);
						sbgra = 0xFF000000 | p[0] << 16 | p[1] << 8 | p[2];
					} break;
					default: break;
					}
					if (src_sRGB)
					{
						sum[0] += srgb_8bit_to_linear_float[(sbgra) & 0xFF] * ws;
						sum[1] += srgb_8bit_to_linear_float[(sbgra >> 8) & 0xFF] * ws;
						sum[2] += srgb_8bit_to_linear_float[(sbgra >> 16) & 0xFF] * ws;
						sum[3] += ((sbgra >> 24) & 0xFF) * ws;
					}
					else
					{
						sum[0] += ((sbgra) & 0xFF) * ws;
						sum[1] += ((sbgra >> 8) & 0xFF) * ws;
						sum[2] += ((sbgra >> 16) & 0xFF) * ws;
						sum[3] += ((sbgra >> 24) & 0xFF) * ws;
					}

				}
			}
			switch (op)
			{
			case BLITTER_TEXTURE_ALPHA_BLEND:
			case BLITTER_TEXTURE_ALPHA_COLOR_BLEND:
				break;
			default:
				break;
			}
			if (dst_sRGB)
			{
				sbgra = linear_to_srgb_8bit(sum[0]) |
					linear_to_srgb_8bit(sum[1]) << 8 |
					linear_to_srgb_8bit(sum[2]) << 16 |
					(u32)(sum[3]) << 24;
			}
			else
			{
				sbgra = (u32)(sum[0]) |
					(u32)(sum[1]) << 8 |
					(u32)(sum[2]) << 16 |
					(u32)(sum[3]) << 24;
			}
			switch (dstFormat)
			{
			case video::ECF_A8R8G8B8: *(u32*)(dstData + (dy*dst_clip.x1) * 4 + (dx * 4)) = sbgra; break;
			case video::ECF_R8G8B8:
			{
				u8* p = dstData + (dy*dst_clip.x1) * 3 + (dx * 3);
				p[2] = (sbgra) & 0xFF;
				p[1] = (sbgra >> 8) & 0xFF;
				p[0] = (sbgra >> 16) & 0xFF;
			} break;
			case video::ECF_A1R5G5B5: *(u16*)(dstData + (dy*dst_clip.x1) * 2 + (dx * 2)) = video::A8R8G8B8toA1R5G5B5(sbgra); break;
			case video::ECF_R5G6B5:   *(u16*)(dstData + (dy*dst_clip.x1) * 2 + (dx * 2)) = video::A8R8G8B8toR5G6B5(sbgra); break;
			default:
				break;
			}
		}
	}

}

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_
