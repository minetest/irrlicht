// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_MESH_VIEWER_H_INCLUDED
#define IRR_C_GUI_MESH_VIEWER_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIMeshViewer.h"
#include "SMaterial.h"

namespace irr
{

namespace gui
{

	class CGUIMeshViewer : public IGUIMeshViewer
	{
	public:

		//! constructor
		CGUIMeshViewer(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! destructor
		virtual ~CGUIMeshViewer();

		//! sets the mesh to be shown
		virtual void setMesh(scene::IAnimatedMesh* mesh) IRR_OVERRIDE;

		//! Gets the displayed mesh
		virtual scene::IAnimatedMesh* getMesh() const IRR_OVERRIDE;

		//! sets the material
		virtual void setMaterial(const video::SMaterial& material) IRR_OVERRIDE;

		//! gets the material
		virtual const video::SMaterial& getMaterial() const IRR_OVERRIDE;

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

	private:

		video::SMaterial Material;
		scene::IAnimatedMesh* Mesh;
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // IRR_C_GUI_MESH_VIEWER_H_INCLUDED
