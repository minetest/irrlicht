// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_MATERIAL_FLAGS_H_INCLUDED__
#define __E_MATERIAL_FLAGS_H_INCLUDED__

namespace irr
{
namespace video
{

	//! Material flags
	enum E_MATERIAL_FLAG
	{
		//! Corresponds to SMaterial::Wireframe.
		EMF_WIREFRAME = 0x1,

		//! Corresponds to SMaterial::PointCloud.
		EMF_POINTCLOUD = 0x2,

		//! Corresponds to SMaterial::GouraudShading.
		EMF_GOURAUD_SHADING = 0x4,

		//! Corresponds to SMaterial::Lighting.
		EMF_LIGHTING = 0x8,

		//! Corresponds to SMaterial::ZBuffer.
		EMF_ZBUFFER = 0x10,

		//! Corresponds to SMaterial::ZWriteEnable.
		EMF_ZWRITE_ENABLE = 0x20,

		//! Corresponds to SMaterial::BackfaceCulling.
		EMF_BACK_FACE_CULLING = 0x40,

		//! Corresponds to SMaterial::FrontfaceCulling.
		EMF_FRONT_FACE_CULLING = 0x80,

		//! Corresponds to SMaterialLayer::BilinearFilter.
		EMF_BILINEAR_FILTER = 0x100,

		//! Corresponds to SMaterialLayer::TrilinearFilter.
		EMF_TRILINEAR_FILTER = 0x200,

		//! Corresponds to SMaterialLayer::AnisotropicFilter.
		EMF_ANISOTROPIC_FILTER = 0x400,

		//! Corresponds to SMaterial::FogEnable.
		EMF_FOG_ENABLE = 0x800,

		//! Corresponds to SMaterial::NormalizeNormals.
		EMF_NORMALIZE_NORMALS = 0x1000,

		//! Corresponds to SMaterialLayer::TextureWrapU, TextureWrapV and
		//! TextureWrapW.
		EMF_TEXTURE_WRAP = 0x2000,

		//! Corresponds to SMaterial::AntiAliasing.
		EMF_ANTI_ALIASING = 0x4000,

		//! Corresponds to SMaterial::ColorMask.
		EMF_COLOR_MASK = 0x8000,

		//! Corresponds to SMaterial::ColorMaterial.
		EMF_COLOR_MATERIAL = 0x10000,

		//! Corresponds to SMaterial::UseMipMaps.
		EMF_USE_MIP_MAPS = 0x20000,

		//! Corresponds to SMaterial::BlendOperation.
		EMF_BLEND_OPERATION = 0x40000,

		//! Corresponds to SMaterial::PolygonOffsetFactor, PolygonOffsetDirection,
		//! PolygonOffsetDepthBias and PolygonOffsetSlopeScale.
		EMF_POLYGON_OFFSET = 0x80000,

		//! Corresponds to SMaterial::BlendFactor.
		EMF_BLEND_FACTOR = 0x160000
	};

} // end namespace video
} // end namespace irr


#endif // __E_MATERIAL_FLAGS_H_INCLUDED__

