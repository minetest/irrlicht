// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IMeshSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "ISceneNode.h"

namespace irr
{
namespace scene
{

// A node which helps rendering a single buffer of an IMeshSceneNode
// It solves several problems:
// - Allowing the scene manager to sort meshbuffers. Currently it only sorts nodes, so we have to put each meshbuffer in an extra node to allow that
//   The reason we want that is 
//     a) Better sorting when a node has several transparent buffers (without it they will be just drawn in original order)
//     b) It can allow to avoid texture changes in the render-pipeline which can make quite a bit of a performance difference
// - It buffers the RenderPass. Bit of an abuse of this interface maybe?
//   Strangely the check for finding out the correct render pass constantly shows up in profilers.
//   Not exactly sure why as the check looks pretty cheap. My best guess is that there are some cache misses going on due to several virtual
//	 function pointers being involved in the transparency check.
// 
// For now (added pre Irrlicht 1.9) this interface is still a bit experimental. Maybe could go into public headers later, not sure yet.
// Or maybe the SceneManager shouldn't work with nodes at all but a simplified interface to render buffers from which Nodes can derive?
// CBufferRenderNode isn't really a node - it has to work around nearly all the ISceneNode functions, it only has to be one because
// the SceneManager can't sort anything else but nodes.
class CBufferRenderNode : public ISceneNode
{
public:
	CBufferRenderNode(irr::scene::IMeshSceneNode& parent, irr::scene::ISceneManager* mgr, irr::u32 bufferIdx)
		: ISceneNode(0, mgr)	// we don't want it in the scenegraph
		, MeshNodeParent(parent)
		, BufferIdx(bufferIdx)
		, RenderPass(ESNRP_NONE)
		, ParentDoesRender(true)
	{
		// While it's not the parent in the SceneGraph, we still want to allow accessing it
		// That can be useful p.E. in a light manager
		// Arguably if it's a good idea as it's a bit going against the ISceneNode interface - having a parent which doesn't have this node as child.
		// But the alternative is adding another member to the ISceneNode or having SceneManager not use the ISceneNode for rendering
		// So for now it should be fine, but if that interface ever get's public... we might have to reconsider
		Parent = &MeshNodeParent;
	}

	u32 prepareRendering(E_SCENE_NODE_RENDER_PASS pass, bool parentDoesRender)
	{
		RenderPass = pass;
		ParentDoesRender = parentDoesRender;
		if ( !ParentDoesRender )
			return SceneManager->registerNodeForRendering(this, pass);
		return 0;
	}

	E_SCENE_NODE_RENDER_PASS getRenderPass() const
	{
		return RenderPass;
	}

	// When true render() this node hasn't registered itself for rendering, but expects it's owner to do the rendering
	bool getDoesParentRender() const
	{
		return ParentDoesRender;
	}

	// Render meshbuffer, but don't set transformation
	// It's assumed that this function is only called from within the correct render stage
	void renderBuffer(video::IVideoDriver* driver)
	{
		const IMeshBuffer* mb = MeshNodeParent.getMesh()->getMeshBuffer(BufferIdx);
		if (mb)
		{
			const video::SMaterial& material = MeshNodeParent.getMaterial(BufferIdx);
			driver->setMaterial(material);
			driver->drawMeshBuffer(mb);
		}

		// Resetting after each rendering so direct calls to render() for parent node continue to work
		// Assuming each pass only runs once
		RenderPass = ESNRP_NONE;
		ParentDoesRender = true;
	}

	//! Renders the node.
	virtual void render() IRR_OVERRIDE
	{
		video::IVideoDriver* driver = SceneManager->getVideoDriver();
		driver->setTransform(video::ETS_WORLD, MeshNodeParent.getAbsoluteTransformation());
		renderBuffer(driver);
	}

	virtual const core::aabbox3d<f32>& getBoundingBox() const IRR_OVERRIDE
	{
		return MeshNodeParent.getMesh()->getMeshBuffer(BufferIdx)->getBoundingBox();
	}

	virtual video::SMaterial& getMaterial(u32 num) IRR_OVERRIDE
	{
		return MeshNodeParent.getMaterial(BufferIdx);
	}

	virtual u32 getMaterialCount() const IRR_OVERRIDE
	{
		return 1;
	}

	virtual const core::matrix4& getAbsoluteTransformation() const IRR_OVERRIDE
	{
		return MeshNodeParent.getAbsoluteTransformation();
	}

	virtual const core::aabbox3d<f32> getTransformedBoundingBox() const IRR_OVERRIDE
	{
		core::aabbox3d<f32> box = getBoundingBox();
		getAbsoluteTransformation().transformBoxEx(box);
		return box;
	}

	virtual void getTransformedBoundingBoxEdges(core::array< core::vector3d<f32> >& edges) const IRR_OVERRIDE
	{
		edges.set_used(8);
		getBoundingBox().getEdges( edges.pointer() );
		for ( u32 i=0; i<8; ++i )
			getAbsoluteTransformation().transformVect( edges[i] );
	}

	virtual core::matrix4 getRelativeTransformation() const IRR_OVERRIDE
	{
		return MeshNodeParent.getRelativeTransformation();
	}

	virtual s32 getID() const IRR_OVERRIDE
	{
		return MeshNodeParent.getID();
	}

	virtual const core::vector3df& getScale() const IRR_OVERRIDE
	{
		return MeshNodeParent.getScale();
	}

	virtual const core::vector3df& getRotation() const IRR_OVERRIDE
	{
		return MeshNodeParent.getRotation();
	}

	virtual const core::vector3df& getPosition() const IRR_OVERRIDE
	{
		return MeshNodeParent.getPosition();
	}

	virtual core::vector3df getAbsolutePosition() const IRR_OVERRIDE	
	{
		return MeshNodeParent.getAbsolutePosition();
	}

	virtual ITriangleSelector* getTriangleSelector() const IRR_OVERRIDE
	{
		return MeshNodeParent.getTriangleSelector();
	}

	// Not allowing any of that stuff
	virtual void OnRegisterSceneNode()IRR_OVERRIDE {}
	virtual void OnAnimate(u32 timeMs) IRR_OVERRIDE {}
	virtual void addChild(ISceneNode* child) IRR_OVERRIDE {}
	virtual void addAnimator(ISceneNodeAnimator* animator) IRR_OVERRIDE {}
	virtual void setScale(const core::vector3df& scale) IRR_OVERRIDE {}
	virtual void setRotation(const core::vector3df& rotation) IRR_OVERRIDE {}
	virtual void setPosition(const core::vector3df& newpos) IRR_OVERRIDE {}
	virtual void setParent(ISceneNode* newParent) IRR_OVERRIDE {}
	virtual void setTriangleSelector(ITriangleSelector* selector) IRR_OVERRIDE {}
	virtual void updateAbsolutePosition() IRR_OVERRIDE {}
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const IRR_OVERRIDE {}
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0) IRR_OVERRIDE	{}

private:
	irr::scene::IMeshSceneNode& MeshNodeParent;
	irr::u32 BufferIdx;	// Note: Not saving the meshbuffer pointer as meshes can add/remove buffers and we don't want to keep track of that
	E_SCENE_NODE_RENDER_PASS RenderPass;
	bool ParentDoesRender;
};

} // end namespace scene
} // end namespace irr
