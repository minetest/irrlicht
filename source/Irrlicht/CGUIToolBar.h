// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_TOOL_BAR_H_INCLUDED
#define IRR_C_GUI_TOOL_BAR_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIToolbar.h"

namespace irr
{
namespace gui
{

	//! Stays at the top of its parent like the menu bar and contains tool buttons
	class CGUIToolBar : public IGUIToolBar
	{
	public:

		//! constructor
		CGUIToolBar(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		//! Updates the absolute position.
		virtual void updateAbsolutePosition() IRR_OVERRIDE;

		//! Adds a button to the tool bar
		virtual IGUIButton* addButton(s32 id=-1, const wchar_t* text=0,const wchar_t* tooltiptext=0,
			video::ITexture* img=0, video::ITexture* pressed=0,
			bool isPushButton=false, bool useAlphaChannel=false) IRR_OVERRIDE;

	private:

		s32 ButtonX;
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif
