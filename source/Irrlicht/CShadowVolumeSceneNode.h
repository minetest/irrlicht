// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_SHADOW_VOLUME_SCENE_NODE_H_INCLUDED
#define IRR_C_SHADOW_VOLUME_SCENE_NODE_H_INCLUDED

#include "IShadowVolumeSceneNode.h"

namespace irr
{
namespace scene
{

	//! Scene node for rendering a shadow volume into a stencil buffer.
	class CShadowVolumeSceneNode : public IShadowVolumeSceneNode
	{
	public:

		//! constructor
		CShadowVolumeSceneNode(const IMesh* shadowMesh, ISceneNode* parent, ISceneManager* mgr,
			s32 id, bool zfailmethod=true, f32 infinity=10000.0f);

		//! destructor
		virtual ~CShadowVolumeSceneNode();

		//! Sets the mesh from which the shadow volume should be generated.
		/** To optimize shadow rendering, use a simpler mesh for shadows.
		*/
		virtual void setShadowMesh(const IMesh* mesh) IRR_OVERRIDE;

		//! Updates the shadow volumes for current light positions.
		/** Called each render cycle from Animated Mesh SceneNode render method. */
		virtual void updateShadowVolumes() IRR_OVERRIDE;

		//! Set optimization used to create shadow volumes
		/** Default is ESV_SILHOUETTE_BY_POS. If the shadow 
		looks bad then give ESV_NONE a try (which will be slower). */
		virtual void setOptimization(ESHADOWVOLUME_OPTIMIZATION optimization) IRR_OVERRIDE;

		//! Get currently active optimization used to create shadow volumes
		virtual ESHADOWVOLUME_OPTIMIZATION getOptimization() const IRR_OVERRIDE
		{
			return Optimization;
		}

		//! pre render method
		virtual void OnRegisterSceneNode() IRR_OVERRIDE;

		//! renders the node.
		virtual void render() IRR_OVERRIDE;

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const IRR_OVERRIDE;

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const IRR_OVERRIDE { return ESNT_SHADOW_VOLUME; }

	private:

		typedef core::array<core::vector3df> SShadowVolume;

		void createShadowVolume(const core::vector3df& pos, bool isDirectional);
		u32 createEdgesAndCaps(const core::vector3df& light, bool isDirectional, SShadowVolume* svp, core::aabbox3d<f32>* bb);

		//! Generates adjacency information based on mesh indices.
		void calculateAdjacency();

		core::aabbox3d<f32> Box;

		// a shadow volume for every light
		core::array<SShadowVolume> ShadowVolumes;

		// a back cap bounding box for every light
		core::array<core::aabbox3d<f32> > ShadowBBox;

		core::array<core::vector3df> Vertices;
		core::array<u16> Indices;
		core::array<u16> Adjacency;
		core::array<u16> Edges;
		// tells if face is front facing
		core::array<bool> FaceData;
		bool AdjacencyDirtyFlag;

		const scene::IMesh* ShadowMesh;

		u32 IndexCount;
		u32 VertexCount;
		u32 ShadowVolumesUsed;

		f32 Infinity;
		bool UseZFailMethod;
		ESHADOWVOLUME_OPTIMIZATION Optimization;
	};

} // end namespace scene
} // end namespace irr

#endif
