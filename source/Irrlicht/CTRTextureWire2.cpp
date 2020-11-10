// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "IBurningShader.h"

#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_

// compile flag for this file
#undef USE_ZBUFFER
#undef IPOL_Z
#undef CMP_Z
#undef WRITE_Z

#undef IPOL_W
#undef CMP_W
#undef WRITE_W

#undef SUBTEXEL
#undef INVERSE_W

#undef IPOL_C0
#undef IPOL_T0
#undef IPOL_T1

// define render case
#define SUBTEXEL
#define INVERSE_W

//#define USE_ZBUFFER
#define IPOL_W
//#define CMP_W
//#define WRITE_W


#define IPOL_C0
//#define IPOL_T0
//#define IPOL_T1

// apply global override
#ifndef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
	#undef INVERSE_W
#endif

#ifndef SOFTWARE_DRIVER_2_SUBTEXEL
	#undef SUBTEXEL
#endif

#if BURNING_MATERIAL_MAX_COLORS < 1
	#undef IPOL_C0
#endif

#if !defined ( SOFTWARE_DRIVER_2_USE_WBUFFER ) && defined ( USE_ZBUFFER )
	#ifndef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
		#undef IPOL_W
	#endif
	#define IPOL_Z

	#ifdef CMP_W
		#undef CMP_W
		#define CMP_Z
	#endif

	#ifdef WRITE_W
		#undef WRITE_W
		#define WRITE_Z
	#endif

#endif


namespace irr
{

namespace video
{

class CTRTextureWire2 : public IBurningShader
{
public:

	//! constructor
	CTRTextureWire2(CBurningVideoDriver* driver);

	//! draws an indexed triangle list
	virtual void drawTriangle(const s4DVertex* burning_restrict a, const s4DVertex* burning_restrict b, const s4DVertex* burning_restrict c) _IRR_OVERRIDE_;
	virtual void drawLine ( const s4DVertex *a,const s4DVertex *b) _IRR_OVERRIDE_;
	virtual void drawPoint( const s4DVertex *a) _IRR_OVERRIDE_;
	virtual bool canWireFrame () _IRR_OVERRIDE_  { return true; }
	virtual bool canPointCloud() _IRR_OVERRIDE_  { return true; }

protected:
	virtual void fragmentShader();

	void renderAlphaLine ( const s4DVertex *a,const s4DVertex *b ) const;
	void renderLine ( const s4DVertex *a,const s4DVertex *b, int renderZero = 0 ) const;

};

//! constructor
CTRTextureWire2::CTRTextureWire2(CBurningVideoDriver* driver)
: IBurningShader(driver)
{
	#ifdef _DEBUG
	setDebugName("CTRTextureWire2");
	#endif
}



/*!
	2d line
*/
void CTRTextureWire2::renderLine ( const s4DVertex *a,const s4DVertex *b, int renderZero) const
{
	int pitch0 = RenderTarget->getDimension().Width << SOFTWARE_DRIVER_2_RENDERTARGET_GRANULARITY;
#ifdef USE_ZBUFFER
	int pitch1 = RenderTarget->getDimension().Width << 2;
#endif

	//todo:!
	int aposx = fill_convention_none(a->Pos.x);
	int aposy = fill_convention_none(a->Pos.y);
	int bposx = fill_convention_none(b->Pos.x);
	int bposy = fill_convention_none(b->Pos.y);

	int dx = bposx - aposx;
	int dy = bposy - aposy;

	int c;
	int m;
	int d = 0;
	int run;

	tVideoSample *dst;
#ifdef USE_ZBUFFER
	fp24 *z;
#endif

	int xInc0 = 1 << SOFTWARE_DRIVER_2_RENDERTARGET_GRANULARITY;
	int yInc0 = pitch0;

#ifdef USE_ZBUFFER
	int xInc1 = 4;
	int yInc1 = pitch1;
#endif

	if ( dx < 0 )
	{
		xInc0 = - ( 1 << SOFTWARE_DRIVER_2_RENDERTARGET_GRANULARITY);
#ifdef USE_ZBUFFER
		xInc1 = -4;
#endif
		dx = -dx;
	}

	if ( dy > dx )
	{
		//swap
		s32 t;
		t = dx;dx=dy;dy=t;
		t = xInc0;xInc0=yInc0;yInc0=t;
#ifdef USE_ZBUFFER
		t = xInc1;xInc1=yInc1;yInc1=t;
#endif
	}

	if (0 == dx)
	{
		if (!renderZero) return;
		dx = 1;
	}

	SOFTWARE_DRIVER_2_CLIPCHECK_WIRE;
	dst = (tVideoSample*) ( (u8*) RenderTarget->getData() + ( aposy * pitch0 ) + (aposx* (1<< SOFTWARE_DRIVER_2_RENDERTARGET_GRANULARITY) ) );
#ifdef USE_ZBUFFER
	z = (fp24*) ( (u8*) (fp24*) DepthBuffer->lock() + ( aposy * pitch1 ) + (aposx << 2 ) );
#endif

	c = dx << 1;
	m = dy << 1;

	// slopes
	const f32 invDeltaX = fill_step_x( (f32)dx );

#ifdef IPOL_Z
	f32 slopeZ = (b->Pos.z - a->Pos.z) * invDeltaX;
	f32 dataZ = a->Pos.z;
#endif

#ifdef IPOL_W
	fp24 slopeW = (b->Pos.w - a->Pos.w) * invDeltaX;
	fp24 dataW = a->Pos.w;
#endif

	f32 inversew = FIX_POINT_F32_MUL;

	tVideoSample color;
#if BURNING_MATERIAL_MAX_COLORS > 0
	tFixPoint r0, g0, b0;
#ifdef IPOL_C0
	sVec4 slopeC;
	sVec4 C;
	slopeC = (b->Color[0] - a->Color[0]) * invDeltaX;
	C = a->Color[0];
#endif

#ifdef INVERSE_W
	inversew = fix_inverse32_color(dataW);
#endif

	vec4_to_fix( r0, g0, b0, a->Color[0], inversew);
	color = fix_to_sample( r0, g0, b0 );

#else
	color = (tVideoSample) 0xFFFFFFFF;
#endif


	run = dx;
	while ( run )
	{
#ifdef CMP_Z
		if ( *z >= dataZ )
#endif
#ifdef CMP_W
		if ( dataW >= *z )
#endif
		{
#ifdef WRITE_Z
			*z = dataZ;
#endif
#ifdef WRITE_W
			*z = dataW;
#endif

#ifdef IPOL_C0
#ifdef INVERSE_W
			inversew = fix_inverse32_color(dataW);
#endif
			vec4_to_fix(r0, g0, b0, C,inversew);
			color = fix_to_sample( r0, g0, b0 );
#endif
			*dst = color;

		}

		dst = (tVideoSample*) ( (u8*) dst + xInc0 );	// x += xInc
#ifdef CMP_Z
		z = (fp24*) ( (u8*) z + xInc1 );
#endif
#ifdef CMP_W
		z = (fp24*) ( (u8*) z + xInc1 );
#endif

		d += m;
		if ( d > dx )
		{
			dst = (tVideoSample*) ( (u8*) dst + yInc0 );	// y += yInc
#ifdef CMP_Z
			z = (fp24*) ( (u8*) z + yInc1 );
#endif
#ifdef CMP_W
			z = (fp24*) ( (u8*) z + yInc1 );
#endif

			d -= c;
		}
		run -= 1;
#ifdef IPOL_Z
		dataZ += slopeZ;
#endif
#ifdef IPOL_W
		dataW += slopeW;
#endif
#ifdef IPOL_C0
		C += slopeC;
#endif


	}

}

void CTRTextureWire2::fragmentShader()
{
}

void CTRTextureWire2::drawTriangle(const s4DVertex* burning_restrict a, const s4DVertex* burning_restrict b, const s4DVertex* burning_restrict c)
{
	// sort on height, y
	if ( F32_A_GREATER_B ( a->Pos.y , b->Pos.y ) ) swapVertexPointer(&a, &b);
	if ( F32_A_GREATER_B ( b->Pos.y , c->Pos.y ) ) swapVertexPointer(&b, &c);
	if ( F32_A_GREATER_B ( a->Pos.y , b->Pos.y ) ) swapVertexPointer(&a, &b);

	renderLine ( a, b );
	renderLine ( b, c );
	renderLine ( a, c );

}


void CTRTextureWire2::drawLine ( const s4DVertex *a,const s4DVertex *b)
{
	// query access to TexMaps

	// sort on height, y
	if (F32_A_GREATER_B(a->Pos.y,b->Pos.y )) swapVertexPointer(&a, &b);

	renderLine ( a, b );
}

void CTRTextureWire2::drawPoint(const s4DVertex *a)
{
	if ( (a->flag & VERTEX4D_CLIPMASK ) == VERTEX4D_INSIDE ) renderLine(a, a,1);
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_

namespace irr
{
namespace video
{


//! creates a flat triangle renderer
IBurningShader* createTriangleRendererTextureGouraudWire2(CBurningVideoDriver* driver)
{
	//ETR_TEXTURE_GOURAUD_WIRE
	#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_
	return new CTRTextureWire2(driver);
	#else
	return 0;
	#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_
}


} // end namespace video
} // end namespace irr


