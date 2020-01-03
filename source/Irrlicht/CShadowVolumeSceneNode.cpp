// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_SHADOW_VOLUME_SCENENODE_

#include "CShadowVolumeSceneNode.h"
#include "ISceneManager.h"
#include "IMesh.h"
#include "IVideoDriver.h"
#include "ICameraSceneNode.h"
#include "SViewFrustum.h"
#include "SLight.h"
#include "os.h"

namespace irr
{
namespace scene
{


//! constructor
CShadowVolumeSceneNode::CShadowVolumeSceneNode(const IMesh* shadowMesh, ISceneNode* parent,
		ISceneManager* mgr, s32 id, bool zfailmethod, f32 infinity)
: IShadowVolumeSceneNode(parent, mgr, id),
	AdjacencyDirtyFlag(true),
	ShadowMesh(0), IndexCount(0), VertexCount(0), ShadowVolumesUsed(0),
	Infinity(infinity), UseZFailMethod(zfailmethod), Optimization(ESV_SILHOUETTE_BY_POS)
{
	#ifdef _DEBUG
	setDebugName("CShadowVolumeSceneNode");
	#endif
	setShadowMesh(shadowMesh);
	setAutomaticCulling(scene::EAC_OFF);
}


//! destructor
CShadowVolumeSceneNode::~CShadowVolumeSceneNode()
{
	if (ShadowMesh)
		ShadowMesh->drop();
}


void CShadowVolumeSceneNode::createShadowVolume(const core::vector3df& light, bool isDirectional)
{
	SShadowVolume* svp = 0;
	core::aabbox3d<f32>* bb = 0;

	// builds the shadow volume and adds it to the shadow volume list.

	if (ShadowVolumes.size() > ShadowVolumesUsed)
	{
		// get the next unused buffer

		svp = &ShadowVolumes[ShadowVolumesUsed];
		svp->set_used(0);

		bb = &ShadowBBox[ShadowVolumesUsed];
	}
	else
	{
		ShadowVolumes.push_back(SShadowVolume());
		svp = &ShadowVolumes.getLast();

		ShadowBBox.push_back(core::aabbox3d<f32>());
		bb = &ShadowBBox.getLast();
	}
	svp->reallocate(IndexCount*5);
	++ShadowVolumesUsed;

	// We use triangle lists
	Edges.set_used(IndexCount*2);
	u32 numEdges = 0;

	numEdges=createEdgesAndCaps(light, isDirectional, svp, bb);

	// for all edges add the near->far quads
	core::vector3df lightDir1(light*Infinity);
	core::vector3df lightDir2(light*Infinity);
	for (u32 i=0; i<numEdges; ++i)
	{
		const core::vector3df &v1 = Vertices[Edges[2*i+0]];
		const core::vector3df &v2 = Vertices[Edges[2*i+1]];
		if ( !isDirectional )
		{
			lightDir1 = (v1 - light).normalize()*Infinity;
			lightDir2 = (v2 - light).normalize()*Infinity;
		}
		const core::vector3df v3(v1+lightDir1);
		const core::vector3df v4(v2+lightDir2);

		// Add a quad (two triangles) to the vertex list
#ifdef _DEBUG
		if (svp->size() >= svp->allocated_size()-5)
			os::Printer::log("Allocation too small.", ELL_DEBUG);
#endif
		svp->push_back(v1);
		svp->push_back(v2);
		svp->push_back(v3);

		svp->push_back(v2);
		svp->push_back(v4);
		svp->push_back(v3);
	}
}

// TODO.
// Not sure what's going on. Either FaceData should mean the opposite and true should mean facing away from light
// or I'm missing something else. Anyway - when not setting this then Shadows will look wrong on Burnings driver
// while they seem to look OK on first view either way on other drivers. Only tested with z-fail so far.
// Maybe errors only show up close to near/far plane on other drivers as otherwise the stencil-buffer-count 
// is probably ending up with same value anyway 
#define IRR_USE_REVERSE_EXTRUDED

u32 CShadowVolumeSceneNode::createEdgesAndCaps(const core::vector3df& light, bool isDirectional,
					SShadowVolume* svp, core::aabbox3d<f32>* bb)
{
	u32 numEdges=0;
	const u32 faceCount = IndexCount / 3;

	if(faceCount >= 1)
		bb->reset(Vertices[Indices[0]]);
	else
		bb->reset(0,0,0);

	// Check every face if it is front or back facing the light.
	core::vector3df lightDir0(light);
	core::vector3df lightDir1(light);
	core::vector3df lightDir2(light);
	for (u32 i=0; i<faceCount; ++i)
	{
		const core::vector3df v0 = Vertices[Indices[3*i+0]];
		const core::vector3df v1 = Vertices[Indices[3*i+1]];
		const core::vector3df v2 = Vertices[Indices[3*i+2]];

		if ( !isDirectional )
		{
			lightDir0 = (v0-light).normalize();
		}
#ifdef IRR_USE_REVERSE_EXTRUDED
		FaceData[i]=core::triangle3df(v2,v1,v0).isFrontFacing(lightDir0);	// actually the back-facing polygons
#else
		FaceData[i]=core::triangle3df(v0,v1,v2).isFrontFacing(lightDir0);
#endif

#if 0	// Useful for internal debugging & testing. Show all the faces in the light.
		if ( FaceData[i] )
		{
			video::SMaterial m;
			m.Lighting = false;
			SceneManager->getVideoDriver()->setMaterial(m);
#ifdef IRR_USE_REVERSE_EXTRUDED
			SceneManager->getVideoDriver()->draw3DTriangle(core::triangle3df(v0+lightDir0,v1+lightDir0,v2+lightDir0), irr::video::SColor(255,255, 0, 0));
#else
			SceneManager->getVideoDriver()->draw3DTriangle(core::triangle3df(v0-lightDir0,v1-lightDir0,v2-lightDir0), irr::video::SColor(255,255, 0, 0));
#endif
		}
#endif

		if (UseZFailMethod && FaceData[i])
		{
#ifdef _DEBUG
			if (svp->size() >= svp->allocated_size()-5)
				os::Printer::log("Allocation too small.", ELL_DEBUG);
#endif
			// add front cap from light-facing faces
			svp->push_back(v2);
			svp->push_back(v1);
			svp->push_back(v0);

			// add back cap
			if ( !isDirectional )
			{
				lightDir1 = (v1-light).normalize();
				lightDir2 = (v2-light).normalize();
			}
			const core::vector3df i0 = v0+lightDir0*Infinity;
			const core::vector3df i1 = v1+lightDir1*Infinity;
			const core::vector3df i2 = v2+lightDir2*Infinity;

			svp->push_back(i0);
			svp->push_back(i1);
			svp->push_back(i2);

			bb->addInternalPoint(i0);
			bb->addInternalPoint(i1);
			bb->addInternalPoint(i2);
		}
	}

	// Create edges
	for (u32 i=0; i<faceCount; ++i)
	{
		// check all front facing faces
		if (FaceData[i] == true)
		{
			const u16 wFace0 = Indices[3*i+0];
			const u16 wFace1 = Indices[3*i+1];
			const u16 wFace2 = Indices[3*i+2];

			if ( Optimization == ESV_NONE )
			{
				// add edge v0-v1
				Edges[2*numEdges+0] = wFace0;
				Edges[2*numEdges+1] = wFace1;
				++numEdges;

				// add edge v1-v2
				Edges[2*numEdges+0] = wFace1;
				Edges[2*numEdges+1] = wFace2;
				++numEdges;

				// add edge v2-v0
				Edges[2*numEdges+0] = wFace2;
				Edges[2*numEdges+1] = wFace0;
				++numEdges;
			}
			else
			{
				const u16 adj0 = Adjacency[3*i+0];
				const u16 adj1 = Adjacency[3*i+1];
				const u16 adj2 = Adjacency[3*i+2];

				// add edges if face is adjacent to back-facing face
				// or if no adjacent face was found
				if (adj0 == i || FaceData[adj0] == false)
				{
					// add edge v0-v1
					Edges[2*numEdges+0] = wFace0;
					Edges[2*numEdges+1] = wFace1;
					++numEdges;
				}

				if (adj1 == i || FaceData[adj1] == false)
				{
					// add edge v1-v2
					Edges[2*numEdges+0] = wFace1;
					Edges[2*numEdges+1] = wFace2;
					++numEdges;
				}

				if (adj2 == i || FaceData[adj2] == false)
				{
					// add edge v2-v0
					Edges[2*numEdges+0] = wFace2;
					Edges[2*numEdges+1] = wFace0;
					++numEdges;
				}
			}
		}
	}
	return numEdges;
}


void CShadowVolumeSceneNode::setShadowMesh(const IMesh* mesh)
{
	if (ShadowMesh == mesh)
		return;
	if (ShadowMesh)
		ShadowMesh->drop();
	ShadowMesh = mesh;
	if (ShadowMesh)
	{
		ShadowMesh->grab();
		Box = ShadowMesh->getBoundingBox();
	}
}


void CShadowVolumeSceneNode::updateShadowVolumes()
{
	const u32 oldIndexCount = IndexCount;
	const u32 oldVertexCount = VertexCount;

	VertexCount = 0;
	IndexCount = 0;
	ShadowVolumesUsed = 0;

	const IMesh* const mesh = ShadowMesh;
	if (!mesh)
		return;

	// create as much shadow volumes as there are lights but
	// do not ignore the max light settings.
	const u32 lightCount = SceneManager->getVideoDriver()->getDynamicLightCount();
	if (!lightCount)
		return;

	// calculate total amount of vertices and indices

	u32 i;
	u32 totalVertices = 0;
	u32 totalIndices = 0;
	const u32 bufcnt = mesh->getMeshBufferCount();

	for (i=0; i<bufcnt; ++i)
	{
		const IMeshBuffer* buf = mesh->getMeshBuffer(i);
		if (	buf->getIndexType() == video::EIT_16BIT 
			&& buf->getPrimitiveType() == scene::EPT_TRIANGLES )
		{
			totalIndices += buf->getIndexCount();
			totalVertices += buf->getVertexCount();
		}
		else
		{
			os::Printer::log("ShadowVolumeSceneNode only supports meshbuffers with 16 bit indices and triangles", ELL_WARNING);
			return;
		}
	}
	if ( totalIndices != (u32)(u16)totalIndices)
	{
		// We could switch to 32-bit indices, not much work and just bit of extra memory (< 192k) per shadow volume.
		// If anyone ever complains and really needs that just switch it. But huge shadows are usually a bad idea as they will be slow.
		os::Printer::log("ShadowVolumeSceneNode does not yet support shadowvolumes which need more than 16 bit indices", ELL_WARNING);
		return;
	}

	// allocate memory if necessary

	Vertices.set_used(totalVertices);
	Indices.set_used(totalIndices);
	FaceData.set_used(totalIndices / 3);

	// copy mesh 
	// (could speed this up for static meshes by adding some user flag to prevents copying)
	for (i=0; i<bufcnt; ++i)
	{
		const IMeshBuffer* buf = mesh->getMeshBuffer(i);

		const u16* idxp = buf->getIndices();
		const u16* idxpend = idxp + buf->getIndexCount();
		for (; idxp!=idxpend; ++idxp)
			Indices[IndexCount++] = *idxp + VertexCount;

		const u32 vtxcnt = buf->getVertexCount();
		for (u32 j=0; j<vtxcnt; ++j)
			Vertices[VertexCount++] = buf->getPosition(j);
	}

	// recalculate adjacency if necessary
	if (oldVertexCount != VertexCount || oldIndexCount != IndexCount || AdjacencyDirtyFlag)
		calculateAdjacency();

	core::matrix4 matInv(Parent->getAbsoluteTransformation());
	matInv.makeInverse();
	core::matrix4 matTransp(Parent->getAbsoluteTransformation(), core::matrix4::EM4CONST_TRANSPOSED);
	const core::vector3df parentpos = Parent->getAbsolutePosition();

	for (i=0; i<lightCount; ++i)
	{
		const video::SLight& dl = SceneManager->getVideoDriver()->getDynamicLight(i);

		if ( dl.Type == video::ELT_DIRECTIONAL )
		{
			core::vector3df ldir(dl.Direction);
			matTransp.transformVect(ldir);
			createShadowVolume(ldir, true);
		}
		else
		{
			core::vector3df lpos(dl.Position);
			if (dl.CastShadows &&
				fabs((lpos - parentpos).getLengthSQ()) <= (dl.Radius*dl.Radius*4.0f))
			{
				matInv.transformVect(lpos);
				createShadowVolume(lpos, false);
			}
		}
	}
}

void CShadowVolumeSceneNode::setOptimization(ESHADOWVOLUME_OPTIMIZATION optimization)
{
	if ( Optimization != optimization )
	{
		Optimization = optimization;
		AdjacencyDirtyFlag = true;
	}
}

//! pre render method
void CShadowVolumeSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this, scene::ESNRP_SHADOW);
		ISceneNode::OnRegisterSceneNode();
	}
}

//! renders the node.
void CShadowVolumeSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!ShadowVolumesUsed || !driver)
		return;

	driver->setTransform(video::ETS_WORLD, Parent->getAbsoluteTransformation());

	bool checkFarPlaneClipping = UseZFailMethod && !driver->queryFeature(video::EVDF_DEPTH_CLAMP);

	// get camera frustum converted to local coordinates when we have to check for far plane clipping
	SViewFrustum frust;
	if ( checkFarPlaneClipping )
	{
		const irr::scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();
		if ( camera )
		{
			frust = *camera->getViewFrustum();
			core::matrix4 invTrans(Parent->getAbsoluteTransformation(), core::matrix4::EM4CONST_INVERSE);
			frust.transform(invTrans);
		}
		else
			checkFarPlaneClipping = false;
	}

	for (u32 i=0; i<ShadowVolumesUsed; ++i)
	{
		bool drawShadow = true;

		if (checkFarPlaneClipping)
		{
			// Disable shadows drawing, when back cap is behind of ZFar plane.
			// TODO: Using infinite projection matrices instead is said to work better
			//       as then we wouldn't fail when the shadow clip the far plane.
			//       I couldn't get it working (and neither anyone before me it seems).
			//       Anyone who can figure it out is welcome to provide a patch.

			core::vector3df edges[8];
			ShadowBBox[i].getEdges(edges);

			for(int j = 0; j < 8; ++j)
			{
				if (frust.planes[scene::SViewFrustum::VF_FAR_PLANE].classifyPointRelation(edges[j]) == core::ISREL3D_FRONT)
				{
					drawShadow = false;
					break;
				}
			}
		}

		if(drawShadow)
			driver->drawStencilShadowVolume(ShadowVolumes[i], UseZFailMethod, DebugDataVisible);
		else
		{
			// TODO: For some reason (not yet further investigated), Direct3D needs a call to drawStencilShadowVolume
			//       even if we have nothing to draw here to set the renderstate into a StencilShadowMode.
			//       If that's not done it has effect on further render calls.
			core::array<core::vector3df> triangles;
			driver->drawStencilShadowVolume(triangles, UseZFailMethod, DebugDataVisible);
		}
	}
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CShadowVolumeSceneNode::getBoundingBox() const
{
	return Box;
}


//! Generates adjacency information based on mesh indices.
void CShadowVolumeSceneNode::calculateAdjacency()
{
	AdjacencyDirtyFlag = false;

	if ( Optimization == ESV_NONE )
	{
		Adjacency.clear();
	}
	else if ( Optimization == ESV_SILHOUETTE_BY_POS )
	{
		Adjacency.set_used(IndexCount);

		// go through all faces and fetch their three neighbours
		for (u32 f=0; f<IndexCount; f+=3)
		{
			for (u32 edge = 0; edge<3; ++edge)
			{
				const core::vector3df& v1 = Vertices[Indices[f+edge]];
				const core::vector3df& v2 = Vertices[Indices[f+((edge+1)%3)]];

				// now we search an_O_ther _F_ace with these two
				// vertices, which is not the current face.
				u32 of;

				for (of=0; of<IndexCount; of+=3)
				{
					// only other faces
					if (of != f)
					{
						bool cnt1 = false;
						bool cnt2 = false;

						for (s32 e=0; e<3; ++e)
						{
							if (v1.equals(Vertices[Indices[of+e]]))
								cnt1=true;

							if (v2.equals(Vertices[Indices[of+e]]))
								cnt2=true;
						}
						// one match for each vertex, i.e. edge is the same
						if (cnt1 && cnt2)
							break;
					}
				}

				// no adjacent edges -> store face number, else store adjacent face
				if (of >= IndexCount)
					Adjacency[f + edge] = f/3;
				else
					Adjacency[f + edge] = of/3;
			}
		}
	}
}


} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_SHADOW_VOLUME_SCENENODE_
