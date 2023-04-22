// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GEOMETRY_CREATOR_H_INCLUDED
#define IRR_C_GEOMETRY_CREATOR_H_INCLUDED

#include "IGeometryCreator.h"
#include "SMeshBuffer.h"

namespace irr
{

namespace scene
{

//! class for creating geometry on the fly
class CGeometryCreator : public IGeometryCreator
{
	void addToBuffer(const video::S3DVertex& v, SMeshBuffer* Buffer) const;
public:
	virtual IMesh* createCubeMesh(const core::vector3df& size, ECUBE_MESH_TYPE type) const IRR_OVERRIDE;

	virtual IMesh* createHillPlaneMesh(
		const core::dimension2d<f32>& tileSize, const core::dimension2d<u32>& tileCount,
		video::SMaterial* material, f32 hillHeight, const core::dimension2d<f32>& countHills,
		const core::dimension2d<f32>& textureRepeatCount) const IRR_OVERRIDE;

	virtual IMesh* createGeoplaneMesh(f32 radius, u32 rows, u32 columns) const IRR_OVERRIDE;

	virtual IMesh* createTerrainMesh(video::IImage* texture,
		video::IImage* heightmap, const core::dimension2d<f32>& stretchSize,
		f32 maxHeight, video::IVideoDriver* driver,
		const core::dimension2d<u32>& defaultVertexBlockSize,
		bool debugBorders=false) const IRR_OVERRIDE;

	virtual IMesh* createArrowMesh(const u32 tessellationCylinder,
			const u32 tessellationCone, const f32 height,
			const f32 cylinderHeight, const f32 width0,
			const f32 width1, const video::SColor vtxColor0,
			const video::SColor vtxColor1) const IRR_OVERRIDE;

	virtual IMesh* createSphereMesh(f32 radius, u32 polyCountX, u32 polyCountY) const IRR_OVERRIDE;

	virtual IMesh* createCylinderMesh(f32 radius, f32 length, u32 tessellation,
				const video::SColor& color=0xffffffff,
				bool closeTop=true, f32 oblique=0.f, u32 normalType=0) const IRR_OVERRIDE;

	virtual IMesh* createConeMesh(f32 radius, f32 length, u32 tessellation,
				const video::SColor& colorTop=0xffffffff,
				const video::SColor& colorBottom=0xffffffff,
				f32 oblique=0.f) const IRR_OVERRIDE;

	virtual IMesh* createTorusMesh(f32 majorRadius, f32 minorRadius, 
				u32 majorSegments, u32 minorSegments,
				f32 angleStart, f32 angleEnd, int capEnds) const IRR_OVERRIDE;

	virtual IMesh* createVolumeLightMesh(
			const u32 subdivideU=32, const u32 subdivideV=32,
			const video::SColor footColor=0xffffffff,
			const video::SColor tailColor=0xffffffff,
			const f32 lpDistance = 8.f,
			const core::vector3df& lightDim = core::vector3df(1.f,1.2f,1.f)) const IRR_OVERRIDE;
};


} // end namespace scene
} // end namespace irr

#endif
