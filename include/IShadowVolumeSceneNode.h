// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SHADOW_VOLUME_SCENE_NODE_H_INCLUDED__
#define __I_SHADOW_VOLUME_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"

namespace irr
{
namespace scene
{
	class IMesh;

	enum ESHADOWVOLUME_OPTIMIZATION
	{
		//! Create volumes around every triangle
		ESV_NONE,

		//! Create volumes only around the silhouette of the mesh
		/** This can reduce the number of volumes drastically,
		but will have an upfront-cost where it calculates adjacency of
		triangles. Also it will not work with all models. Basically
		if you see strange black shadow lines then you have a model
		for which it won't work.
		We get that information about adjacency by comparing the positions of 
		all edges in the mesh (even if they are in different meshbuffers). */
		ESV_SILHOUETTE_BY_POS
	};

	//! Scene node for rendering a shadow volume into a stencil buffer.
	class IShadowVolumeSceneNode : public ISceneNode
	{
	public:

		//! constructor
		IShadowVolumeSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
			: ISceneNode(parent, mgr, id) {}

		//! Sets the mesh from which the shadow volume should be generated.
		/** To optimize shadow rendering, use a simpler mesh for shadows.
		*/
		virtual void setShadowMesh(const IMesh* mesh) = 0;

		//! Updates the shadow volumes for current light positions.
		virtual void updateShadowVolumes() = 0;

		//! Set optimization used to create shadow volumes
		/** Default is ESV_SILHOUETTE_BY_POS. If the shadow 
		looks bad then give ESV_NONE a try (which will be slower). 
		Alternatively you can try to fix the model, it's often
		because it's not closed (aka if you'd put water in it then 
		that would leak out). */
		virtual void setOptimization(ESHADOWVOLUME_OPTIMIZATION optimization) = 0;

		//! Get currently active optimization used to create shadow volumes
		virtual ESHADOWVOLUME_OPTIMIZATION getOptimization() const = 0;
	};

} // end namespace scene
} // end namespace irr

#endif

