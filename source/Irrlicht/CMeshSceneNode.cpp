// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CMeshSceneNode.h"
#include "CBufferRenderNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "ICameraSceneNode.h"
#include "IMeshCache.h"
#include "IAnimatedMesh.h"
#include "IMaterialRenderer.h"
#include "IFileSystem.h"
#ifdef _IRR_COMPILE_WITH_SHADOW_VOLUME_SCENENODE_
#include "CShadowVolumeSceneNode.h"
#else
#include "IShadowVolumeSceneNode.h"
#endif // _IRR_COMPILE_WITH_SHADOW_VOLUME_SCENENODE_

namespace irr
{
namespace scene
{

//! constructor
CMeshSceneNode::CMeshSceneNode(IMesh* mesh, ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position, const core::vector3df& rotation,
			const core::vector3df& scale)
	: IMeshSceneNode(parent, mgr, id, position, rotation, scale)
	, Mesh(0), Shadow(0), ReadOnlyMaterials(false)
{
	#ifdef _DEBUG
	setDebugName("CMeshSceneNode");
	#endif

	setMesh(mesh);
}


//! destructor
CMeshSceneNode::~CMeshSceneNode()
{
	setUsedBufferRenderNodes(0);
	if (Shadow)
		Shadow->drop();
	if (Mesh)
		Mesh->drop();
}


//! frame
void CMeshSceneNode::OnRegisterSceneNode()
{
	if (IsVisible && Mesh)
	{
		Box = Mesh->getBoundingBox(); // in case mesh was modified, as clipping happens when registering nodes for rendering

		// Because this node supports rendering of mixed mode meshes consisting of
		// transparent and solid material at the same time, we need to go through all
		// materials, check of what type they are and register this node for the right
		// render pass according to that.
		// Also some buffers might register into their own render node

		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		int transparentCount = 0;
		int solidCount = 0;

		if ( !(DebugDataVisible & scene::EDS_HALF_TRANSPARENCY) )
		{
			// count transparent and solid materials in this scene node
			const u32 numMaterials = ReadOnlyMaterials ? Mesh->getMeshBufferCount() : Materials.size();
			const bool parentRenders = NodeRegistration == ENR_DEFAULT || numMaterials < 2;
			for (u32 i=0; i<numMaterials; ++i)
			{
				const video::SMaterial& material = ReadOnlyMaterials ? Mesh->getMeshBuffer(i)->getMaterial() : Materials[i];

				if ( driver->needsTransparentRenderPass(material) )
				{
					BufferRenderNodes[i]->prepareRendering(ESNRP_TRANSPARENT, parentRenders);
					if ( parentRenders )
					{
						++transparentCount;
					}
				}
				else
				{
					BufferRenderNodes[i]->prepareRendering(ESNRP_SOLID, parentRenders);
					if ( parentRenders )
					{
						++solidCount;
					}
				}
			}
		}

		// register according to material types counted

		if (solidCount)
			SceneManager->registerNodeForRendering(this, ESNRP_SOLID);

		if (transparentCount)
			SceneManager->registerNodeForRendering(this, ESNRP_TRANSPARENT);

		if (Shadow)	// update (not render) shadow node after lights have been set up
			SceneManager->registerNodeForRendering(this, ESNRP_SKY_BOX);

		if (DebugDataVisible) // debug data has it's own render-stage between solid and transparence
			SceneManager->registerNodeForRendering(this, ESNRP_SHADOW);

		ISceneNode::OnRegisterSceneNode();
	}
}


//! renders the node.
void CMeshSceneNode::render()
{
	if (!Mesh )
		return;

	const E_SCENE_NODE_RENDER_PASS renderPass = SceneManager->getSceneNodeRenderPass();

	if ( renderPass == ESNRP_SKY_BOX )
	{
		if (Shadow )
			Shadow->updateShadowVolumes();
	}
	else if ( renderPass == ESNRP_SHADOW )
	{
		// for debug purposes only
		if ( DebugDataVisible )
		{
			video::IVideoDriver* driver = SceneManager->getVideoDriver();
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

			// render with half transparency
			if (DebugDataVisible & scene::EDS_HALF_TRANSPARENCY)
			{
				for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
				{
					irr::video::SMaterial mat = Materials[g];
					mat.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
					driver->setMaterial(mat);
					driver->drawMeshBuffer(Mesh->getMeshBuffer(g));
				}
			}

			video::SMaterial m;
			m.Lighting = false;
			m.AntiAliasing=0;
			driver->setMaterial(m);

			if (DebugDataVisible & scene::EDS_BBOX)
			{
				driver->draw3DBox(Box, video::SColor(255,255,255,255));
			}
			if (DebugDataVisible & scene::EDS_BBOX_BUFFERS)
			{
				for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
				{
					driver->draw3DBox(
						Mesh->getMeshBuffer(g)->getBoundingBox(),
						video::SColor(255,190,128,128));
				}
			}

			if (DebugDataVisible & scene::EDS_NORMALS)
			{
				// draw normals
 				const f32 debugNormalLength = SceneManager->getParameters()->getAttributeAsFloat(DEBUG_NORMAL_LENGTH);
				const video::SColor debugNormalColor = SceneManager->getParameters()->getAttributeAsColor(DEBUG_NORMAL_COLOR);
				const u32 count = Mesh->getMeshBufferCount();

				for (u32 i=0; i != count; ++i)
				{
					driver->drawMeshBufferNormals(Mesh->getMeshBuffer(i), debugNormalLength, debugNormalColor);
				}
			}

			// show mesh
			if (DebugDataVisible & scene::EDS_MESH_WIRE_OVERLAY)
			{
				m.Wireframe = true;
				driver->setMaterial(m);

				for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
				{
					driver->drawMeshBuffer(Mesh->getMeshBuffer(g));
				}
			}
		}
	}
	else // solid, transparent or unknown (none when render is called without SceneManager) render stages
	{
		video::IVideoDriver* driver = SceneManager->getVideoDriver();
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

		// render buffers, or at least those which don't render in their own node
		for (u32 i=0; i<BufferRenderNodes.size(); ++i)
		{
			CBufferRenderNode* bufRenderNode = BufferRenderNodes[i];
			if ( bufRenderNode->getDoesParentRender())
			{
				E_SCENE_NODE_RENDER_PASS bufferRenderPass = bufRenderNode->getRenderPass();

				// render() called without OnRegisterSceneNode, but still wants to render in a specific render stage
				// Note: Not checking transparency every time, as check got slightly expensive (I think it's prone to cache-misses)
				if ( bufferRenderPass == ESNRP_NONE && renderPass > ESNRP_NONE )	
				{
					if ( driver->needsTransparentRenderPass(getMaterial(i)) )
					{
						bufferRenderPass = ESNRP_TRANSPARENT;
					}
					else
					{
						bufferRenderPass = ESNRP_SOLID;
					}
				}

				if ( bufRenderNode->getRenderPass() == renderPass || renderPass == ESNRP_NONE)
					bufRenderNode->renderBuffer(driver);
			}
		}
	}
}


//! Removes a child from this scene node.
//! Implemented here, to be able to remove the shadow properly, if there is one,
//! or to remove attached children.
bool CMeshSceneNode::removeChild(ISceneNode* child)
{
	if (child && Shadow == child)
	{
		Shadow->drop();
		Shadow = 0;
	}

	return ISceneNode::removeChild(child);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CMeshSceneNode::getBoundingBox() const
{
	return Mesh ? Mesh->getBoundingBox() : Box;
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hierarchy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CMeshSceneNode::getMaterial(u32 i)
{
	if (Mesh && ReadOnlyMaterials && i<Mesh->getMeshBufferCount())
	{
		ReadOnlyMaterial = Mesh->getMeshBuffer(i)->getMaterial();
		return ReadOnlyMaterial;
	}

	if (i >= Materials.size())
		return ISceneNode::getMaterial(i);

	return Materials[i];
}


//! returns amount of materials used by this scene node.
u32 CMeshSceneNode::getMaterialCount() const
{
	if (Mesh && ReadOnlyMaterials)
		return Mesh->getMeshBufferCount();

	return Materials.size();
}

void CMeshSceneNode::setUsedBufferRenderNodes(irr::u32 num)
{
	if ( BufferRenderNodes.size() > num )
	{
		for ( irr::u32 i=num; i<BufferRenderNodes.size(); ++i )
			BufferRenderNodes[i]->drop();
		BufferRenderNodes.erase(num, BufferRenderNodes.size()-num);
	}
	else if ( BufferRenderNodes.size() < num )
	{
		for ( irr::u32 i=BufferRenderNodes.size(); i < num; ++i )
		{
			BufferRenderNodes.push_back( new CBufferRenderNode(*this, SceneManager, i) );
		}
	}
}

//! Sets a new mesh
void CMeshSceneNode::setMesh(IMesh* mesh)
{
	if (mesh)
	{
		mesh->grab();
		if (Mesh)
		{
			Mesh->drop();
		}

		Mesh = mesh;

		// Note: Mesh can change amount of meshbuffers later and we don't handle that so far so that would cause trouble
		// For now assuming users call setMesh again in that case
		copyMaterials();
		setUsedBufferRenderNodes(Mesh ? Mesh->getMeshBufferCount() : 0);
	}
}


//! Creates shadow volume scene node as child of this node
//! and returns a pointer to it.
IShadowVolumeSceneNode* CMeshSceneNode::addShadowVolumeSceneNode(
		const IMesh* shadowMesh, s32 id, bool zfailmethod, f32 infinity)
{
#ifdef _IRR_COMPILE_WITH_SHADOW_VOLUME_SCENENODE_
	if (!SceneManager->getVideoDriver()->queryFeature(video::EVDF_STENCIL_BUFFER))
		return 0;

	if (!shadowMesh)
		shadowMesh = Mesh; // if null is given, use the mesh of node

	if (Shadow)
		Shadow->drop();

	Shadow = new CShadowVolumeSceneNode(shadowMesh, this, SceneManager, id,  zfailmethod, infinity);
	return Shadow;
#else
	return 0;
#endif
}


void CMeshSceneNode::copyMaterials()
{
	Materials.clear();

	if (Mesh)
	{
		video::SMaterial mat;

		for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
		{
			IMeshBuffer* mb = Mesh->getMeshBuffer(i);
			if (mb)
				mat = mb->getMaterial();

			Materials.push_back(mat);
		}
	}
}


//! Writes attributes of the scene node.
void CMeshSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	IMeshSceneNode::serializeAttributes(out, options);

	if (options && (options->Flags&io::EARWF_USE_RELATIVE_PATHS) && options->Filename)
	{
		const io::path path = SceneManager->getFileSystem()->getRelativeFilename(
				SceneManager->getFileSystem()->getAbsolutePath(SceneManager->getMeshCache()->getMeshName(Mesh).getPath()),
				options->Filename);
		out->addString("Mesh", path.c_str());
	}
	else
		out->addString("Mesh", SceneManager->getMeshCache()->getMeshName(Mesh).getPath().c_str());
	out->addBool("ReadOnlyMaterials", ReadOnlyMaterials);
}


//! Reads attributes of the scene node.
void CMeshSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	io::path oldMeshStr = SceneManager->getMeshCache()->getMeshName(Mesh);
	io::path newMeshStr = in->getAttributeAsString("Mesh");
	ReadOnlyMaterials = in->getAttributeAsBool("ReadOnlyMaterials");

	if (newMeshStr != "" && oldMeshStr != newMeshStr)
	{
		IMesh* newMesh = 0;
		IAnimatedMesh* newAnimatedMesh = SceneManager->getMesh(newMeshStr.c_str());

		if (newAnimatedMesh)
			newMesh = newAnimatedMesh->getMesh(0);

		if (newMesh)
			setMesh(newMesh);
	}

	// optional attribute to assign the hint to the whole mesh
	if (in->existsAttribute("HardwareMappingHint") &&
		in->existsAttribute("HardwareMappingBufferType"))
	{
		scene::E_HARDWARE_MAPPING mapping = scene::EHM_NEVER;
		scene::E_BUFFER_TYPE bufferType = scene::EBT_NONE;

		core::stringc smapping = in->getAttributeAsString("HardwareMappingHint");
		if (smapping.equals_ignore_case("static"))
			mapping = scene::EHM_STATIC;
		else if (smapping.equals_ignore_case("dynamic"))
			mapping = scene::EHM_DYNAMIC;
		else if (smapping.equals_ignore_case("stream"))
			mapping = scene::EHM_STREAM;

		core::stringc sbufferType = in->getAttributeAsString("HardwareMappingBufferType");
		if (sbufferType.equals_ignore_case("vertex"))
			bufferType = scene::EBT_VERTEX;
		else if (sbufferType.equals_ignore_case("index"))
			bufferType = scene::EBT_INDEX;
		else if (sbufferType.equals_ignore_case("vertexindex"))
			bufferType = scene::EBT_VERTEX_AND_INDEX;

		IMesh* mesh = getMesh();
		if (mesh)
			mesh->setHardwareMappingHint(mapping, bufferType);
	}

	IMeshSceneNode::deserializeAttributes(in, options);
}


//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
/* In this way it is possible to change the materials a mesh causing all mesh scene nodes
referencing this mesh to change too. */
void CMeshSceneNode::setReadOnlyMaterials(bool readonly)
{
	ReadOnlyMaterials = readonly;
}


//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
bool CMeshSceneNode::isReadOnlyMaterials() const
{
	return ReadOnlyMaterials;
}


//! Creates a clone of this scene node and its children.
ISceneNode* CMeshSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
{
	if (!newParent)
		newParent = Parent;
	if (!newManager)
		newManager = SceneManager;

	CMeshSceneNode* nb = new CMeshSceneNode(Mesh, newParent,
		newManager, ID, RelativeTranslation, RelativeRotation, RelativeScale);

	nb->cloneMembers(this, newManager);
	nb->ReadOnlyMaterials = ReadOnlyMaterials;
	nb->Materials = Materials;
	nb->Shadow = Shadow;
	if ( nb->Shadow )
		nb->Shadow->grab();

	if (newParent)
		nb->drop();
	return nb;
}


} // end namespace scene
} // end namespace irr

