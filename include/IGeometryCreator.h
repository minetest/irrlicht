// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_I_GEOMETRY_CREATOR_H_INCLUDED
#define IRR_I_GEOMETRY_CREATOR_H_INCLUDED

#include "IReferenceCounted.h"
#include "IMesh.h"
#include "IImage.h"

namespace irr
{
namespace video
{
	class IVideoDriver;
	class SMaterial;
}

namespace scene
{

	enum ECUBE_MESH_TYPE
	{
		//! Single buffer with 12 different vertices, normals are average of adjacent planes
		//! Order for outgoing (front-face) normals of planes would be: NEG_Z, POS_X, POS_Z, NEG_X, POS_Y, NEG_Y
		//! This was the only available type before Irrlicht 1.9, so it's still the default in some functions.
		//! It has the least vertices, but is pretty much unusable if you have dynamic light
		ECMT_1BUF_12VTX_NA,

		//! One buffer per side, each with 4 vertices, normals are perpendicular to sides
		//! You can use this one if you need different texture on each cube side
		ECMT_6BUF_4VTX_NP,

		//! Single buffer with 24 different vertices, normals are perpendicular to sides
		ECMT_1BUF_24VTX_NP,

		//! not used, counts the number of enumerated types
		ECMT_COUNT
	};

	//! Names for ECUBE_MESH_TYPE
	const c8* const CubeMeshTypeNames[ECMT_COUNT+1] =
	{
		"1BUF_12VTX_NA",
		"ECMT_6BUF_4VTX_NP",
		"1BUF_24VTX_NP",
		0
	};


//! Helper class for creating geometry on the fly.
/** You can get an instance of this class through ISceneManager::getGeometryCreator() */
class IGeometryCreator : public IReferenceCounted
{
public:

	//! Creates a simple cube mesh.
	/**
	\param size Dimensions of the cube.
	\param type One of ECUBE_MESH_TYPE. So you can chose between cubes with single material or independent materials per side.
	\return Generated mesh.
	Note: UV's go always from 0 to 1. Which can produce wrong colors at edges with texture filtering.
	      Fixing UV's depends on texture-size (have to be moved half a pixel towards the inside, so 0.5f/texure_size as then the pixel center is exactly on the border)
		  Easier solution is usually to set TextureWrapU and TextureWrapV to ETC_CLAMP_TO_EDGE in the Material.
	*/
	virtual IMesh* createCubeMesh(const core::vector3df& size=core::vector3df(5.f,5.f,5.f), ECUBE_MESH_TYPE type = ECMT_1BUF_12VTX_NA) const =0;

	//! Create a pseudo-random mesh representing a hilly terrain.
	/**
	\param tileSize The size of each tile.
	\param tileCount The number of tiles in each dimension.
	\param material The material to apply to the mesh.
	\param hillHeight The maximum height of the hills.
	\param countHills The number of hills along each dimension.
	\param textureRepeatCount The number of times to repeat the material texture along each dimension.
	\return Generated mesh.
	*/
	virtual IMesh* createHillPlaneMesh(
			const core::dimension2d<f32>& tileSize,
			const core::dimension2d<u32>& tileCount,
			video::SMaterial* material, f32 hillHeight,
			const core::dimension2d<f32>& countHills,
			const core::dimension2d<f32>& textureRepeatCount) const =0;

	//! Create a simple rectangular textured plane mesh.
	/**
	\param tileSize The size of each tile.
	\param tileCount The number of tiles in each dimension.
	\param material The material to apply to the mesh.
	\param textureRepeatCount The number of times to repeat the material texture along each dimension.
	\return Generated mesh.
	*/
	IMesh* createPlaneMesh(
			const core::dimension2d<f32>& tileSize,
			const core::dimension2d<u32>& tileCount=core::dimension2du(1,1),
			video::SMaterial* material=0,
			const core::dimension2df& textureRepeatCount=core::dimension2df(1.f,1.f)) const
	{
		return createHillPlaneMesh(tileSize, tileCount, material, 0.f, core::dimension2df(), textureRepeatCount);
	}

	//! Create a geoplane.
	/**
	\param radius Radius of the plane
	\param rows How many rows to place
	\param columns How many columns to place
	\return Generated mesh.
	*/
	virtual IMesh* createGeoplaneMesh(f32 radius = 5.f,
			u32 rows = 16, u32 columns = 16) const =0;

	//! Create a terrain mesh from an image representing a heightfield.
	/**
	\param texture The texture to apply to the terrain.
	\param heightmap An image that will be interpreted as a heightmap. The
	brightness (average color) of each pixel is interpreted as a height,
	with a 255 brightness pixel producing the maximum height.
	\param stretchSize The size that each pixel will produce, i.e. a
	512x512 heightmap
	and a stretchSize of (10.f, 20.f) will produce a mesh of size
	5120.f x 10240.f
	\param maxHeight The maximum height of the terrain.
	\param driver The current video driver.
	\param defaultVertexBlockSize (to be documented)
	\param debugBorders (to be documented)
	\return Generated mesh.
	*/
	virtual IMesh* createTerrainMesh(video::IImage* texture,
			video::IImage* heightmap,
			const core::dimension2d<f32>& stretchSize,
			f32 maxHeight, video::IVideoDriver* driver,
			const core::dimension2d<u32>& defaultVertexBlockSize,
			bool debugBorders=false) const =0;

	//! Create an arrow mesh, composed of a cylinder and a cone.
	/**
	\param tesselationCylinder Number of quads composing the cylinder.
	\param tesselationCone Number of triangles composing the cone's roof.
	\param height Total height of the arrow
	\param cylinderHeight Total height of the cylinder, should be lesser
	than total height
	\param widthCylinder Diameter of the cylinder
	\param widthCone Diameter of the cone's base, should be not smaller
	than the cylinder's diameter
	\param colorCylinder color of the cylinder
	\param colorCone color of the cone
	\return Generated mesh.
	*/
	virtual IMesh* createArrowMesh(const u32 tesselationCylinder = 4,
			const u32 tesselationCone = 8, const f32 height = 1.f,
			const f32 cylinderHeight = 0.6f, const f32 widthCylinder = 0.05f,
			const f32 widthCone = 0.3f, const video::SColor colorCylinder = 0xFFFFFFFF,
			const video::SColor colorCone = 0xFFFFFFFF) const =0;


	//! Create a sphere mesh.
	/**
	\param radius Radius of the sphere
	\param polyCountX Number of quads used for the horizontal tiling
	\param polyCountY Number of quads used for the vertical tiling
	\return Generated mesh.
	*/
	virtual IMesh* createSphereMesh(f32 radius = 5.f,
			u32 polyCountX = 16, u32 polyCountY = 16) const =0;

	//! Create a cylinder mesh.
	/**
	\param radius Radius of the cylinder.
	\param length Length of the cylinder.
	\param tesselation Number of quads around the circumference of the cylinder.
	\param color The color of the cylinder.
	\param closeTop If true, close the ends of the cylinder, otherwise leave them open.
	\param oblique X-offset (shear) of top compared to bottom. 
	\param normalType When 0 side normals are radial from origin. Note that origin is at the bottom.
	                  When 1 side normals are flat along top/bottom polygons.
			NOTE: To get normals which are perpendicular to the side of an oblique 
			cylinder, don't use the oblique parameter. Instead set normalType to 1 
			and create a cylinder with oblique set to 0. Then use 
			IMeshManipulator::transform with a shear matrix on the returned mesh.
			You get a shear matrix for an identical effect of this oblique parameter when you 
			set the 4th element of an identity matrix to (oblique/length).
	\return Generated mesh.
	*/
	virtual IMesh* createCylinderMesh(f32 radius, f32 length,
			u32 tesselation,
			const video::SColor& color=video::SColor(0xffffffff),
			bool closeTop=true, f32 oblique=0.f, u32 normalType=0) const =0;

	//! Create a cone mesh.
	/**
	\param radius Radius of the cone.
	\param length Length of the cone.
	\param tesselation Number of quads around the circumference of the cone.
	\param colorTop The color of the top of the cone.
	\param colorBottom The color of the bottom of the cone.
	\param oblique (to be documented)
	\return Generated mesh.
	*/
	virtual IMesh* createConeMesh(f32 radius, f32 length, u32 tesselation,
			const video::SColor& colorTop=video::SColor(0xffffffff),
			const video::SColor& colorBottom=video::SColor(0xffffffff),
			f32 oblique=0.f) const =0;

	//! Create a torus mesh
	/** Note: Segments might get reduced to ensure it fits into 16-bit meshbuffer.
	With 255 segments for minor and major circle you'll hit the maximum.
	When using caps 2 more vertices are added.
	Note: UV's for caps are probably not useful
	\param majorRadius Starting from mesh center
	\param minorRadius Starting from a circle at majorRadius distance around center
	\param majorSegments Segments for major circle. Will use at least 3 segments. 
	\param minorSegments Segments for minor circle. Will use at least 3 segments.
	\param angleStart Start major circle between 0 and 360° and < angleEnd
	\param angleEnd End major circle between 0 and 360° and > angleStart
	\param capEnds When you don't create a full major circle you might want caps
	               0 = no caps (default)
				   Bit 1: add cap at angleStart
	               Bit 2: add cap at angleEnd
	*/
	virtual IMesh* createTorusMesh(f32 majorRadius, f32 minorRadius, 
		u32 majorSegments = 32, u32 minorSegments = 16, 
		f32 angleStart=0.f, f32 angleEnd=360.f, int capEnds=0) const = 0;

	//! Create a volume light mesh.
	/**
	\param subdivideU Horizontal patch count.
	\param subdivideV Vertical patch count.
	\param footColor Color at the bottom of the light.
	\param tailColor Color at the mid of the light.
	\param lpDistance Virtual distance of the light point for normals.
	\param lightDim Dimensions of the light.
	\return Generated mesh.
	*/
	virtual IMesh* createVolumeLightMesh(
			const u32 subdivideU=32, const u32 subdivideV=32,
			const video::SColor footColor = 0xffffffff,
			const video::SColor tailColor = 0xffffffff,
			const f32 lpDistance = 8.f,
			const core::vector3df& lightDim = core::vector3df(1.f,1.2f,1.f)) const =0;
};


} // end namespace scene
} // end namespace irr

#endif // IRR_I_GEOMETRY_CREATOR_H_INCLUDED
