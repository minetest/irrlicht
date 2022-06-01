// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_SCENE_MANAGER_H_INCLUDED__
#define __C_SCENE_MANAGER_H_INCLUDED__

#include "ISceneManager.h"
#include "ISceneNode.h"
#include "ICursorControl.h"
#include "irrString.h"
#include "irrArray.h"
#include "IMeshLoader.h"
#include "CAttributes.h"

namespace irr
{
namespace io
{
	class IFileSystem;
}
namespace scene
{
	class IMeshCache;

	/*!
		The Scene Manager manages scene nodes, mesh resources, cameras and all the other stuff.
	*/
	class CSceneManager : public ISceneManager, public ISceneNode
	{
	public:

		//! constructor
		CSceneManager(video::IVideoDriver* driver, io::IFileSystem* fs,
			gui::ICursorControl* cursorControl, IMeshCache* cache = 0,
			gui::IGUIEnvironment *guiEnvironment = 0);

		//! destructor
		virtual ~CSceneManager();

		//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
		virtual IAnimatedMesh* getMesh(const io::path& filename, const io::path& alternativeCacheName) _IRR_OVERRIDE_;

		//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
		virtual IAnimatedMesh* getMesh(io::IReadFile* file) _IRR_OVERRIDE_;

		//! Returns an interface to the mesh cache which is shared between all existing scene managers.
		virtual IMeshCache* getMeshCache() _IRR_OVERRIDE_;

		//! returns the video driver
		virtual video::IVideoDriver* getVideoDriver() _IRR_OVERRIDE_;

		//! return the gui environment
		virtual gui::IGUIEnvironment* getGUIEnvironment() _IRR_OVERRIDE_;

		//! return the filesystem
		virtual io::IFileSystem* getFileSystem() _IRR_OVERRIDE_;

		//! adds a scene node for rendering an animated mesh model
		virtual IAnimatedMeshSceneNode* addAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
			bool alsoAddIfMeshPointerZero=false) _IRR_OVERRIDE_;

		//! adds a scene node for rendering a static mesh
		//! the returned pointer must not be dropped.
		virtual IMeshSceneNode* addMeshSceneNode(IMesh* mesh, ISceneNode* parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
			bool alsoAddIfMeshPointerZero=false) _IRR_OVERRIDE_;

		//! renders the node.
		virtual void render() _IRR_OVERRIDE_;

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const _IRR_OVERRIDE_;

		//! registers a node for rendering it at a specific time.
		virtual u32 registerNodeForRendering(ISceneNode* node, E_SCENE_NODE_RENDER_PASS pass = ESNRP_AUTOMATIC) _IRR_OVERRIDE_;

		//! Clear all nodes which are currently registered for rendering
		virtual void clearAllRegisteredNodesForRendering() _IRR_OVERRIDE_;

		//! draws all scene nodes
		virtual void drawAll() _IRR_OVERRIDE_;

		//! Adds a camera scene node to the tree and sets it as active camera.
		//! \param position: Position of the space relative to its parent where the camera will be placed.
		//! \param lookat: Position where the camera will look at. Also known as target.
		//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
		//! the camera will move too.
		//! \return Pointer to interface to camera
		virtual ICameraSceneNode* addCameraSceneNode(ISceneNode* parent = 0,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& lookat = core::vector3df(0,0,100),
			s32 id=-1, bool makeActive=true) _IRR_OVERRIDE_;

		//! Adds a billboard scene node to the scene. A billboard is like a 3d sprite: A 2d element,
		//! which always looks to the camera. It is usually used for things like explosions, fire,
		//! lensflares and things like that.
		virtual IBillboardSceneNode* addBillboardSceneNode(ISceneNode* parent = 0,
			const core::dimension2d<f32>& size = core::dimension2d<f32>(10.0f, 10.0f),
			const core::vector3df& position = core::vector3df(0,0,0), s32 id=-1,
			video::SColor shadeTop = 0xFFFFFFFF, video::SColor shadeBottom = 0xFFFFFFFF) _IRR_OVERRIDE_;

		//! Adds a dummy transformation scene node to the scene graph.
		virtual IDummyTransformationSceneNode* addDummyTransformationSceneNode(
			ISceneNode* parent=0, s32 id=-1) _IRR_OVERRIDE_;

		//! Adds an empty scene node.
		virtual ISceneNode* addEmptySceneNode(ISceneNode* parent, s32 id=-1) _IRR_OVERRIDE_;

		//! Returns the root scene node. This is the scene node which is parent
		//! of all scene nodes. The root scene node is a special scene node which
		//! only exists to manage all scene nodes. It is not rendered and cannot
		//! be removed from the scene.
		//! \return Pointer to the root scene node.
		virtual ISceneNode* getRootSceneNode() _IRR_OVERRIDE_;

		//! Returns the current active camera.
		//! \return The active camera is returned. Note that this can be NULL, if there
		//! was no camera created yet.
		virtual ICameraSceneNode* getActiveCamera() const _IRR_OVERRIDE_;

		//! Sets the active camera. The previous active camera will be deactivated.
		//! \param camera: The new camera which should be active.
		virtual void setActiveCamera(ICameraSceneNode* camera) _IRR_OVERRIDE_;

		//! Adds an external mesh loader.
		virtual void addExternalMeshLoader(IMeshLoader* externalLoader) _IRR_OVERRIDE_;

		//! Returns the number of mesh loaders supported by Irrlicht at this time
		virtual u32 getMeshLoaderCount() const _IRR_OVERRIDE_;

		//! Retrieve the given mesh loader
		virtual IMeshLoader* getMeshLoader(u32 index) const _IRR_OVERRIDE_;

		//! Returns a pointer to the scene collision manager.
		virtual ISceneCollisionManager* getSceneCollisionManager() _IRR_OVERRIDE_;

		//! Returns a pointer to the mesh manipulator.
		virtual IMeshManipulator* getMeshManipulator() _IRR_OVERRIDE_;

		//! Adds a scene node to the deletion queue.
		virtual void addToDeletionQueue(ISceneNode* node) _IRR_OVERRIDE_;

		//! Returns the first scene node with the specified id.
		virtual ISceneNode* getSceneNodeFromId(s32 id, ISceneNode* start=0) _IRR_OVERRIDE_;

		//! Returns the first scene node with the specified name.
		virtual ISceneNode* getSceneNodeFromName(const c8* name, ISceneNode* start=0) _IRR_OVERRIDE_;

		//! Returns the first scene node with the specified type.
		virtual ISceneNode* getSceneNodeFromType(scene::ESCENE_NODE_TYPE type, ISceneNode* start=0) _IRR_OVERRIDE_;

		//! returns scene nodes by type.
		virtual void getSceneNodesFromType(ESCENE_NODE_TYPE type, core::array<scene::ISceneNode*>& outNodes, ISceneNode* start=0) _IRR_OVERRIDE_;

		//! Posts an input event to the environment. Usually you do not have to
		//! use this method, it is used by the internal engine.
		virtual bool postEventFromUser(const SEvent& event) _IRR_OVERRIDE_;

		//! Clears the whole scene. All scene nodes are removed.
		virtual void clear() _IRR_OVERRIDE_;

		//! Removes all children of this scene node
		virtual void removeAll() _IRR_OVERRIDE_;

		//! Returns interface to the parameters set in this scene.
		virtual io::IAttributes* getParameters() _IRR_OVERRIDE_;

		//! Returns current render pass.
		virtual E_SCENE_NODE_RENDER_PASS getSceneNodeRenderPass() const _IRR_OVERRIDE_;

		//! Creates a new scene manager.
		virtual ISceneManager* createNewSceneManager(bool cloneContent) _IRR_OVERRIDE_;

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const _IRR_OVERRIDE_ { return ESNT_SCENE_MANAGER; }

		//! Returns a mesh writer implementation if available
		virtual IMeshWriter* createMeshWriter(EMESH_WRITER_TYPE type) _IRR_OVERRIDE_;

		//! Get a skinned mesh, which is not available as header-only code
		virtual ISkinnedMesh* createSkinnedMesh() _IRR_OVERRIDE_;

		//! Sets ambient color of the scene
		virtual void setAmbientLight(const video::SColorf &ambientColor) _IRR_OVERRIDE_;

		//! Returns ambient color of the scene
		virtual const video::SColorf& getAmbientLight() const _IRR_OVERRIDE_;

		//! Get current render time.
		virtual E_SCENE_NODE_RENDER_PASS getCurrentRenderPass() const _IRR_OVERRIDE_ { return CurrentRenderPass; }

		//! Set current render time.
		virtual void setCurrentRenderPass(E_SCENE_NODE_RENDER_PASS nextPass) _IRR_OVERRIDE_ { CurrentRenderPass = nextPass; }

		//! returns if node is culled
		virtual bool isCulled(const ISceneNode* node) const _IRR_OVERRIDE_;

	private:

		// load and create a mesh which we know already isn't in the cache and put it in there
		IAnimatedMesh* getUncachedMesh(io::IReadFile* file, const io::path& filename, const io::path& cachename);

		//! clears the deletion list
		void clearDeletionList();

		struct DefaultNodeEntry
		{
			DefaultNodeEntry()
			{ }

			DefaultNodeEntry(ISceneNode* n) :
				Node(n), TextureValue(0)
			{
				if (n->getMaterialCount())
					TextureValue = (n->getMaterial(0).getTexture(0));
			}

			bool operator < (const DefaultNodeEntry& other) const
			{
				return (TextureValue < other.TextureValue);
			}

			ISceneNode* Node;
			private:
			void* TextureValue;
		};

		//! sort on distance (center) to camera
		struct TransparentNodeEntry
		{
			TransparentNodeEntry()
			{ }

			TransparentNodeEntry(ISceneNode* n, const core::vector3df& camera)
				: Node(n)
			{
				Distance = Node->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(camera);
			}

			bool operator < (const TransparentNodeEntry& other) const
			{
				return Distance > other.Distance;
			}

			ISceneNode* Node;
			private:
				f64 Distance;
		};

		//! sort on distance (sphere) to camera
		struct DistanceNodeEntry
		{
			DistanceNodeEntry(ISceneNode* n, const core::vector3df& cameraPos)
				: Node(n)
			{
				setNodeAndDistanceFromPosition(n, cameraPos);
			}

			bool operator < (const DistanceNodeEntry& other) const
			{
				return Distance < other.Distance;
			}

			void setNodeAndDistanceFromPosition(ISceneNode* n, const core::vector3df & fromPosition)
			{
				Node = n;
				Distance = Node->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(fromPosition);
				Distance -= Node->getBoundingBox().getExtent().getLengthSQ() * 0.5;
			}

			ISceneNode* Node;
			private:
			f64 Distance;
		};

		//! video driver
		video::IVideoDriver* Driver;

		//! file system
		io::IFileSystem* FileSystem;

		//! GUI Enviroment ( Debug Purpose )
		gui::IGUIEnvironment* GUIEnvironment;

		//! cursor control
		gui::ICursorControl* CursorControl;

		//! collision manager
		ISceneCollisionManager* CollisionManager;

		//! render pass lists
		core::array<ISceneNode*> CameraList;
		core::array<ISceneNode*> SkyBoxList;
		core::array<DefaultNodeEntry> SolidNodeList;
		core::array<TransparentNodeEntry> TransparentNodeList;
		core::array<TransparentNodeEntry> TransparentEffectNodeList;
		core::array<ISceneNode*> GuiNodeList;

		core::array<IMeshLoader*> MeshLoaderList;
		core::array<ISceneNode*> DeletionList;

		//! current active camera
		ICameraSceneNode* ActiveCamera;
		core::vector3df camWorldPos; // Position of camera for transparent nodes.

		video::SColor ShadowColor;
		video::SColorf AmbientLight;

		//! String parameters
		// NOTE: Attributes are slow and should only be used for debug-info and not in release
		io::CAttributes* Parameters;

		//! Mesh cache
		IMeshCache* MeshCache;

		E_SCENE_NODE_RENDER_PASS CurrentRenderPass;
	};

} // end namespace video
} // end namespace scene

#endif

