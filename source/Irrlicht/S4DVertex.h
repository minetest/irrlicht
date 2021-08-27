// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef S_4D_VERTEX_H_INCLUDED
#define S_4D_VERTEX_H_INCLUDED

#include "SoftwareDriver2_compile_config.h"
#include "SoftwareDriver2_helper.h"
#include "irrAllocator.h"
#include "EPrimitiveTypes.h"

namespace irr
{

namespace video
{

//! sVec2 used in BurningShader texture coordinates
struct sVec2
{
	f32 x;
	f32 y;

	sVec2 () {}

	sVec2 ( f32 s) : x ( s ), y ( s ) {}
	sVec2 ( f32 _x, f32 _y )
		: x ( _x ), y ( _y ) {}

	void set ( f32 _x, f32 _y )
	{
		x = _x;
		y = _y;
	}

	// f = a * t + b * ( 1 - t )
	void interpolate(const sVec2& burning_restrict a, const sVec2& burning_restrict b, const ipoltype t)
	{
		x = (f32)(b.x + ( ( a.x - b.x ) * t ));
		y = (f32)(b.y + ( ( a.y - b.y ) * t ));
	}

	sVec2 operator-(const sVec2& other) const
	{
		return sVec2(x - other.x, y - other.y);
	}

	sVec2 operator+(const sVec2& other) const
	{
		return sVec2(x + other.x, y + other.y);
	}

	void operator+=(const sVec2& other)
	{
		x += other.x;
		y += other.y;
	}

	sVec2 operator*(const f32 s) const
	{
		return sVec2(x * s , y * s);
	}

	void operator*=( const f32 s)
	{
		x *= s;
		y *= s;
	}

	void operator=(const sVec2& other)
	{
		x = other.x;
		y = other.y;
	}

};

#include "irrpack.h"

//! sVec2Pack is Irrlicht S3DVertex,S3DVertex2TCoords,S3DVertexTangents Texutre Coordinates.
// Start address is not 4 byte aligned
struct sVec2Pack
{
	f32 x, y;
};

//! sVec3Pack used in BurningShader, packed direction
struct sVec3Pack
{
	f32 x, y, z;
	//f32 _can_pack;

	sVec3Pack() {}
	sVec3Pack(f32 _x, f32 _y, f32 _z)
		: x(_x), y(_y), z(_z) {}

	// f = a * t + b * ( 1 - t )
	void interpolate(const sVec3Pack& burning_restrict v0, const sVec3Pack& burning_restrict v1, const ipoltype t)
	{
		x = (f32)(v1.x + ((v0.x - v1.x) * t));
		y = (f32)(v1.y + ((v0.y - v1.y) * t));
		z = (f32)(v1.z + ((v0.z - v1.z) * t));
	}

	sVec3Pack operator-(const sVec3Pack& other) const
	{
		return sVec3Pack(x - other.x, y - other.y, z - other.z);
	}

	sVec3Pack operator+(const sVec3Pack& other) const
	{
		return sVec3Pack(x + other.x, y + other.y, z + other.z);
	}

	sVec3Pack operator*(const f32 s) const
	{
		return sVec3Pack(x * s, y * s, z * s);
	}

	void operator+=(const sVec3Pack& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}

	void operator=(const sVec3Pack& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

}  PACK_STRUCT;

#include "irrunpack.h"

//! sVec4 used in Driver,BurningShader, direction/color
struct sVec4
{
	union
	{
		struct { f32 x, y, z, w; };
		struct { f32 a, r, g, b; };
	};

	sVec4 () {}
	sVec4 ( f32 _x, f32 _y, f32 _z, f32 _w )
		: x ( _x ), y ( _y ), z( _z ), w ( _w ){}

	// f = a * t + b * ( 1 - t )
	void interpolate(const sVec4& burning_restrict a, const sVec4& burning_restrict b, const ipoltype t)
	{
		x = (f32)(b.x + ( ( a.x - b.x ) * t ));
		y = (f32)(b.y + ( ( a.y - b.y ) * t ));
		z = (f32)(b.z + ( ( a.z - b.z ) * t ));
		w = (f32)(b.w + ( ( a.w - b.w ) * t ));
	}

	sVec4 operator-(const sVec4& other) const
	{
		return sVec4(x - other.x, y - other.y, z - other.z,w - other.w);
	}

	sVec4 operator+(const sVec4& other) const
	{
		return sVec4(x + other.x, y + other.y, z + other.z,w + other.w);
	}

	void operator+=(const sVec4& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
	}

	sVec4 operator*(const f32 s) const
	{
		return sVec4(x * s , y * s, z * s,w * s);
	}

	sVec4 operator*(const sVec4 &other) const
	{
		return sVec4(x * other.x , y * other.y, z * other.z,w * other.w);
	}

	void operator*=(const sVec4 &other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
	}

	void operator=(const sVec4& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
	}

	//outside shader
	void set(f32 _x, f32 _y, f32 _z, f32 _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	void setA8R8G8B8(const u32 argb)
	{
		a = ((argb & 0xFF000000) >> 24) * (1.f / 255.f);
		r = ((argb & 0x00FF0000) >> 16) * (1.f / 255.f);
		g = ((argb & 0x0000FF00) >> 8 ) * (1.f / 255.f);
		b = ((argb & 0x000000FF)      ) * (1.f / 255.f);
	}

	REALINLINE ipoltype dot_xyzw(const sVec4& other) const
	{
		return (ipoltype)x * other.x + (ipoltype)y * other.y + (ipoltype)z * other.z + (ipoltype)w * other.w;
	}

	REALINLINE f32 dot_xyz(const sVec4& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	REALINLINE f32 dot_minus_xyz(const sVec4& other) const
	{
		return -x * other.x + -y * other.y + -z * other.z;
	}

	void mul_xyz(const f32 s)
	{
		x *= s;
		y *= s;
		z *= s;
	}

	f32 length_xyz() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	void normalize_dir_xyz()
	{
		//const f32 l = core::reciprocal_squareroot(x * x + y * y + z * z);
		f32 l = x * x + y * y + z * z;
		l = l > 0.0000001f ? 1.f / sqrtf(l) : 1.f;
		x *= l;
		y *= l;
		z *= l;
	}


	//unpack sVec3 to aligned during runtime
	sVec4(const sVec3Pack& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = 0.f;
	}

	void normalize_pack_xyz(sVec3Pack& out, const f32 len, const f32 ofs) const
	{
		//const f32 l = len * core::reciprocal_squareroot ( r * r + g * g + b * b );
		f32 l = x * x + y * y + z * z;

		l = l > 0.0000001f ? len / sqrtf(l) : 0.f;
		out.x = (x*l) + ofs;
		out.y = (y*l) + ofs;
		out.z = (z*l) + ofs;
	}

};

//!during runtime sVec3Pack
typedef sVec4 sVec3Pack_unpack;

//!sVec4 is argb. sVec3Color is rgba
struct sVec3Color
{
	f32 r, g, b,a;

	void set(const f32 s)
	{
		r = s;
		g = s;
		b = s;
		a = s;
	}

	void setA8R8G8B8(const u32 argb)
	{
		r = ((argb & 0x00FF0000) >> 16) * (1.f / 255.f);
		g = ((argb & 0x0000FF00) >> 8 ) * (1.f / 255.f);
		b = ((argb & 0x000000FF)      ) * (1.f / 255.f);
		a = ((argb & 0xFF000000) >> 24) * (1.f / 255.f);
	}

	void setColorf(const video::SColorf & color)
	{
		r = color.r;
		g = color.g;
		b = color.b;
		a = color.a;
	}

	void add_rgb(const sVec3Color& other)
	{
		r += other.r;
		g += other.g;
		b += other.b;
	}

	void mad_rgb(const sVec3Color& other, const f32 v)
	{
		r += other.r * v;
		g += other.g * v;
		b += other.b * v;
	}

	void mad_rgbv(const sVec3Color& v0, const sVec3Color& v1)
	{
		r += v0.r * v1.r;
		g += v0.g * v1.g;
		b += v0.b * v1.b;
	}

	//sVec4 is a,r,g,b, alpha pass
	void sat(sVec4 &dest, const u32 argb) const
	{
		dest.a = ((argb & 0xFF000000) >> 24) * (1.f / 255.f);
		dest.r = r <= 1.f ? r : 1.f;
		dest.g = g <= 1.f ? g : 1.f;
		dest.b = b <= 1.f ? b : 1.f;
	}

	void sat_xyz(sVec3Pack &dest, const sVec3Color& v1) const
	{
		f32 v;
		v = r * v1.r;	dest.x = v < 1.f ? v : 1.f;
		v = g * v1.g;	dest.y = v < 1.f ? v : 1.f;
		v = b * v1.b;	dest.z = v < 1.f ? v : 1.f;
	}

	void sat_xyz(sVec4 &dest, const sVec3Color& v1) const
	{
		f32 v;
		dest.a = 1.f;
		v = r * v1.r;	dest.r = v < 1.f ? v : 1.f;
		v = g * v1.g;	dest.g = v < 1.f ? v : 1.f;
		v = b * v1.b;	dest.b = v < 1.f ? v : 1.f;
	}


};

//internal BurningShaderFlag for a Vertex
enum e4DVertexFlag
{
	VERTEX4D_CLIPMASK				= 0x0000003F,
	VERTEX4D_CLIP_NEAR				= 0x00000001,
	VERTEX4D_CLIP_FAR				= 0x00000002,
	VERTEX4D_CLIP_LEFT				= 0x00000004,
	VERTEX4D_CLIP_RIGHT				= 0x00000008,
	VERTEX4D_CLIP_BOTTOM			= 0x00000010,
	VERTEX4D_CLIP_TOP				= 0x00000020,
	VERTEX4D_INSIDE					= 0x0000003F,

	VERTEX4D_PROJECTED				= 0x00000100,
	VERTEX4D_VAL_ZERO				= 0x00000200,
	VERTEX4D_VAL_ONE				= 0x00000400,

	VERTEX4D_FORMAT_MASK			= 0xFFFF0000,

	VERTEX4D_FORMAT_MASK_TEXTURE	= 0x000F0000,
	VERTEX4D_FORMAT_TEXTURE_1		= 0x00010000,
	VERTEX4D_FORMAT_TEXTURE_2		= 0x00020000,
	VERTEX4D_FORMAT_TEXTURE_3		= 0x00030000,
	VERTEX4D_FORMAT_TEXTURE_4		= 0x00040000,

	VERTEX4D_FORMAT_MASK_COLOR		= 0x00F00000,
	VERTEX4D_FORMAT_COLOR_1			= 0x00100000,
	VERTEX4D_FORMAT_COLOR_2_FOG		= 0x00200000,
	VERTEX4D_FORMAT_COLOR_3			= 0x00300000,
	VERTEX4D_FORMAT_COLOR_4			= 0x00400000,

	VERTEX4D_FORMAT_MASK_LIGHT		= 0x0F000000,
	VERTEX4D_FORMAT_LIGHT_1			= 0x01000000,
	VERTEX4D_FORMAT_LIGHT_2			= 0x02000000,

	VERTEX4D_FORMAT_MASK_TANGENT	= 0xF0000000,
	VERTEX4D_FORMAT_BUMP_DOT3		= 0x10000000,
	VERTEX4D_FORMAT_SPECULAR		= 0x20000000,

};

//! vertex layout
enum e4DVertexType
{
	E4VT_STANDARD = 0,			// EVT_STANDARD, video::S3DVertex.
	E4VT_2TCOORDS = 1,			// EVT_2TCOORDS, video::S3DVertex2TCoords.
	E4VT_TANGENTS = 2,			// EVT_TANGENTS, video::S3DVertexTangents
	E4VT_REFLECTION_MAP = 3,
	E4VT_SHADOW = 4,			// float * 3
	E4VT_NO_TEXTURE = 5,		// runtime if texture missing
	E4VT_LINE = 6,

	E4VT_COUNT
};

enum e4DIndexType
{
	E4IT_16BIT = 1, // EIT_16BIT,
	E4IT_32BIT = 2, // EIT_32BIT,
	E4IT_NONE  = 4, //
};

#ifdef BURNINGVIDEO_RENDERER_BEAUTIFUL
	#define BURNING_MATERIAL_MAX_TEXTURES 4
	#define BURNING_MATERIAL_MAX_COLORS 4
	#define BURNING_MATERIAL_MAX_LIGHT_TANGENT 1

	//ensure handcrafted sizeof(s4DVertex)
	#define sizeof_s4DVertex	128

#else
	#define BURNING_MATERIAL_MAX_TEXTURES 2
	#ifdef SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
		#define BURNING_MATERIAL_MAX_COLORS 1
	#else
		#define BURNING_MATERIAL_MAX_COLORS 0
	#endif
	#define BURNING_MATERIAL_MAX_LIGHT_TANGENT 1

	//ensure handcrafted sizeof(s4DVertex)
	#define sizeof_s4DVertex	64
#endif

// dummy Vertex. used for calculation vertex memory size
struct s4DVertex_proxy
{
	sVec4 Pos;
#if BURNING_MATERIAL_MAX_TEXTURES > 0
	sVec2 Tex[BURNING_MATERIAL_MAX_TEXTURES];
#endif
#if BURNING_MATERIAL_MAX_COLORS > 0
	sVec4 Color[BURNING_MATERIAL_MAX_COLORS];
#endif
#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
	sVec3Pack LightTangent[BURNING_MATERIAL_MAX_LIGHT_TANGENT];
#endif
	u32 flag; // e4DVertexFlag

};


/*!
	Internal BurningVideo Vertex
*/
struct s4DVertex
{
	sVec4 Pos;
#if BURNING_MATERIAL_MAX_TEXTURES > 0
	sVec2 Tex[ BURNING_MATERIAL_MAX_TEXTURES ];
#endif
#if BURNING_MATERIAL_MAX_COLORS > 0
	sVec4 Color[ BURNING_MATERIAL_MAX_COLORS ];
#endif
#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
	sVec3Pack LightTangent[BURNING_MATERIAL_MAX_LIGHT_TANGENT];
#endif

	u32 flag; // e4DVertexFlag


#if BURNING_MATERIAL_MAX_COLORS < 1 || BURNING_MATERIAL_MAX_LIGHT_TANGENT < 1
	u8 __align [sizeof_s4DVertex - sizeof (s4DVertex_proxy) ];
#endif

	// f = a * t + b * ( 1 - t )
	void interpolate(const s4DVertex& burning_restrict b, const s4DVertex& burning_restrict a, const ipoltype t)
	{
		Pos.interpolate ( a.Pos, b.Pos, t );
#if 0
		Tex[0].interpolate(a.Tex[0], b.Tex[0], t);
		Tex[1].interpolate(a.Tex[1], b.Tex[1], t);
		Color[0].interpolate(a.Color[0], b.Color[0], t);
		LightTangent[0].interpolate(a.LightTangent[0], b.LightTangent[0], t);
#endif

		size_t i;
		size_t size;

#if BURNING_MATERIAL_MAX_TEXTURES > 0
		size = (flag & VERTEX4D_FORMAT_MASK_TEXTURE) >> 16;
		for ( i = 0; i!= size; ++i )
		{
			Tex[i].interpolate ( a.Tex[i], b.Tex[i], t );
		}
#endif

#if BURNING_MATERIAL_MAX_COLORS > 0
		size = (flag & VERTEX4D_FORMAT_MASK_COLOR) >> 20;
		for ( i = 0; i!= size; ++i )
		{
			Color[i].interpolate ( a.Color[i], b.Color[i], t );
		}
#endif

#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
		size = (flag & VERTEX4D_FORMAT_MASK_LIGHT) >> 24;
		for ( i = 0; i!= size; ++i )
		{
			LightTangent[i].interpolate ( a.LightTangent[i], b.LightTangent[i], t );
		}
#endif

	}
};

// ----------------- Vertex Cache ---------------------------

// Buffer is used as pairs of S4DVertex (0 ... ndc, 1 .. dc and projected)
typedef s4DVertex s4DVertexPair;
#define sizeof_s4DVertexPairRel 2
#define s4DVertex_ofs(index)  ((index)*sizeof_s4DVertexPairRel)
#define s4DVertex_proj(index) ((index)*sizeof_s4DVertexPairRel) + 1

struct SAligned4DVertex
{
	SAligned4DVertex()
		:data(0),ElementSize(0),mem(0)	{}

	virtual ~SAligned4DVertex ()
	{
		if (mem)
		{
			delete[] mem;
			mem = 0;
		}
	}

	void resize(size_t element)
	{
		if (element > ElementSize)
		{
			if (mem) delete[] mem;
			size_t byteSize = align_next(element * sizeof_s4DVertex, 4096);
			mem = new u8[byteSize];
		}
		ElementSize = element;
		data = (s4DVertex*)mem;
	}

	s4DVertex* data;	//align to 16 byte
	size_t ElementSize;

private:

	u8* mem;
};

//#define memcpy_s4DVertexPair(dst,src) memcpy(dst,src,sizeof_s4DVertex * 2)
static REALINLINE void memcpy_s4DVertexPair(void* burning_restrict dst, const void* burning_restrict src)
{
	//test alignment -> if already in aligned data
#if 0
	if (((size_t)dst & 0xC) | ((size_t)src & 0xC))
	{
		int g = 1;
	}
#endif

#if defined(ENV64BIT) && (sizeof_s4DVertex * sizeof_s4DVertexPairRel == 128)
	u64* burning_restrict dst64 = (u64*)dst;
	const u64* burning_restrict src64 = (const u64*)src;

	dst64[0]  = src64[0];
	dst64[1]  = src64[1];
	dst64[2]  = src64[2];
	dst64[3]  = src64[3];
	dst64[4]  = src64[4];
	dst64[5]  = src64[5];
	dst64[6]  = src64[6];
	dst64[7]  = src64[7];

	dst64[8]  = src64[8];
	dst64[9]  = src64[9];
	dst64[10] = src64[10];
	dst64[11] = src64[11];
	dst64[12] = src64[12];
	dst64[13] = src64[13];
	dst64[14] = src64[14];
	dst64[15] = src64[15];

#elif defined(ENV64BIT) && (sizeof_s4DVertex * sizeof_s4DVertexPairRel == 256)
	u64* burning_restrict dst64 = (u64*)dst;
	const u64* burning_restrict src64 = (const u64*)src;

	dst64[0]  = src64[0];
	dst64[1]  = src64[1];
	dst64[2]  = src64[2];
	dst64[3]  = src64[3];
	dst64[4]  = src64[4];
	dst64[5]  = src64[5];
	dst64[6]  = src64[6];
	dst64[7]  = src64[7];

	dst64[8]  = src64[8];
	dst64[9]  = src64[9];
	dst64[10] = src64[10];
	dst64[11] = src64[11];
	dst64[12] = src64[12];
	dst64[13] = src64[13];
	dst64[14] = src64[14];
	dst64[15] = src64[15];

	dst64[16] = src64[16];
	dst64[17] = src64[17];
	dst64[18] = src64[18];
	dst64[19] = src64[19];
	dst64[20] = src64[20];
	dst64[21] = src64[21];
	dst64[22] = src64[22];
	dst64[23] = src64[23];

	dst64[24] = src64[24];
	dst64[25] = src64[25];
	dst64[26] = src64[26];
	dst64[27] = src64[27];
	dst64[28] = src64[28];
	dst64[29] = src64[29];
	dst64[30] = src64[30];
	dst64[31] = src64[31];

#else
	u32* dst32 = (u32*)dst;
	const u32* src32 = (const u32*)src;

	size_t len = sizeof_s4DVertex * sizeof_s4DVertexPairRel;
	while (len >= 32)
	{
		*dst32++ = *src32++;
		*dst32++ = *src32++;
		*dst32++ = *src32++;
		*dst32++ = *src32++;
		*dst32++ = *src32++;
		*dst32++ = *src32++;
		*dst32++ = *src32++;
		*dst32++ = *src32++;
		len -= 32;
	}
/*
	while (len >= 4)
	{
		*dst32++ = *src32++;
		len -= 4;
	}
*/
#endif
}


//! hold info for different Vertex Types
struct SVSize
{
	size_t Format;		// e4DVertexFlag VERTEX4D_FORMAT_MASK_TEXTURE
	size_t Pitch;		// sizeof Vertex
	size_t TexSize;	// amount Textures
	size_t TexCooSize;	// sizeof TextureCoordinates
};


// a cache info
struct SCacheInfo
{
	u32 index;
	u32 hit;
};

//must at least hold all possible (clipped) vertices of primitive.
#define VERTEXCACHE_ELEMENT	16			
#define VERTEXCACHE_MISS 0xFFFFFFFF
struct SVertexCache
{
	SVertexCache () {}
	~SVertexCache() {}

	//VertexType
	SVSize vSize[E4VT_COUNT];

	SCacheInfo info[VERTEXCACHE_ELEMENT];
	SCacheInfo info_temp[VERTEXCACHE_ELEMENT];


	// Transformed and lite, clipping state
	// + Clipped, Projected
	SAligned4DVertex mem;

	// source
	const void* vertices;
	u32 vertexCount;

	const void* indices;
	u32 indexCount;
	u32 indicesIndex;
	u32 indicesRun;
	u32 indicesPitch;

	// primitives consist of x vertices
	size_t primitiveHasVertex;

	e4DVertexType vType;		//E_VERTEX_TYPE
	scene::E_PRIMITIVE_TYPE pType;		//scene::E_PRIMITIVE_TYPE
	e4DIndexType iType;		//E_INDEX_TYPE iType

};


// swap 2 pointer
REALINLINE void swapVertexPointer(const s4DVertex** v1, const s4DVertex** v2)
{
	const s4DVertex* b = *v1;
	*v1 = *v2;
	*v2 = b;
}


// ------------------------ Internal Scanline Rasterizer -----------------------------



// internal scan convert
struct sScanConvertData
{
	u32 left;			// major edge left/right
	u32 right;			// !left
	u8 _unused_pack[8];

	f32 invDeltaY[4];	// inverse edge delta for screen space sorted triangle 

	f32 x[2];			// x coordinate
	f32 slopeX[2];		// x slope along edges

#if defined ( SOFTWARE_DRIVER_2_USE_WBUFFER ) || defined ( SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT )
	f32 w[2];			// w coordinate
	fp24 slopeW[2];		// w slope along edges
#else
	f32 z[2];			// z coordinate
	f32 slopeZ[2];		// z slope along edges
#endif

#if BURNING_MATERIAL_MAX_COLORS > 0
	sVec4 c[BURNING_MATERIAL_MAX_COLORS][2];		// color
	sVec4 slopeC[BURNING_MATERIAL_MAX_COLORS][2];	// color slope along edges
#endif

#if BURNING_MATERIAL_MAX_TEXTURES > 0
	sVec2 t[BURNING_MATERIAL_MAX_TEXTURES][2];		// texture
	sVec2 slopeT[BURNING_MATERIAL_MAX_TEXTURES][2];	// texture slope along edges
#endif

#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
	sVec3Pack_unpack l[BURNING_MATERIAL_MAX_LIGHT_TANGENT][2];		// Light Tangent
	sVec3Pack_unpack slopeL[BURNING_MATERIAL_MAX_LIGHT_TANGENT][2];	// tangent slope along edges
#endif
};

// passed to scan Line
struct sScanLineData
{
	s32 y;				// y position of scanline
	u8 _unused_pack[4];
	f32 x[2];			// x start, x end of scanline

#if defined ( SOFTWARE_DRIVER_2_USE_WBUFFER ) || defined ( SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT )
	f32 w[2];			// w start, w end of scanline
#else
	f32 z[2];			// z start, z end of scanline
#endif

	s32 x_edgetest;		// slope x
	u8 _unused_pack_1[4];

#if BURNING_MATERIAL_MAX_COLORS > 0
	sVec4 c[BURNING_MATERIAL_MAX_COLORS][2];			// color start, color end of scanline
#endif

#if BURNING_MATERIAL_MAX_TEXTURES > 0
	sVec2 t[BURNING_MATERIAL_MAX_TEXTURES][2];		// texture start, texture end of scanline
#endif

#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
	sVec3Pack_unpack l[BURNING_MATERIAL_MAX_LIGHT_TANGENT][2];		// Light Tangent start, end
#endif
};

// passed to pixel Shader
struct sPixelShaderData
{
	tVideoSample *dst;
	fp24 *z;

	s32 xStart;
	s32 xEnd;
	s32 dx;
	s32 i;
};

/*
	load a color value
*/
REALINLINE void getTexel_plain2 (	tFixPoint &r, tFixPoint &g, tFixPoint &b,const sVec4 &v	)
{
	r = tofix(v.r, FIX_POINT_F32_MUL);
	g = tofix(v.g, FIX_POINT_F32_MUL);
	b = tofix(v.b, FIX_POINT_F32_MUL);
}

#if 0
/*
	load a color value
*/
REALINLINE void getSample_color (	tFixPoint &a, tFixPoint &r, tFixPoint &g, tFixPoint &b,	const sVec4 &v )
{
	a = tofix ( v.a, FIX_POINT_F32_MUL);
	r = tofix ( v.r, COLOR_MAX * FIX_POINT_F32_MUL);
	g = tofix ( v.g, COLOR_MAX * FIX_POINT_F32_MUL);
	b = tofix ( v.b, COLOR_MAX * FIX_POINT_F32_MUL);
}

/*
	load a color value
*/
REALINLINE void getSample_color ( tFixPoint &r, tFixPoint &g, tFixPoint &b,const sVec4 &v )
{
	r = tofix ( v.r, COLOR_MAX * FIX_POINT_F32_MUL);
	g = tofix ( v.g, COLOR_MAX * FIX_POINT_F32_MUL);
	b = tofix ( v.b, COLOR_MAX * FIX_POINT_F32_MUL);
}
#endif

/*
	load a color value. mulby controls [0;1] or [0;ColorMax]
	aka getSample_color
*/
REALINLINE void vec4_to_fix(tFixPoint &r, tFixPoint &g, tFixPoint &b,const sVec4 &v, const f32 mulby )
{
	r = tofix(v.r, mulby);
	g = tofix(v.g, mulby);
	b = tofix(v.b, mulby);
}

REALINLINE void vec4_to_fix(tFixPoint &a,tFixPoint &r, tFixPoint &g, tFixPoint &b,const sVec4 &v, const f32 mulby)
{
	a = tofix(v.a, mulby);
	r = tofix(v.r, mulby);
	g = tofix(v.g, mulby);
	b = tofix(v.b, mulby);
}


}

}

#endif
