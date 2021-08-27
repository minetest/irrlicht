// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_MENU_H_INCLUDED
#define IRR_C_GUI_MENU_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "CGUIContextMenu.h"

namespace irr
{
namespace gui
{

	//! GUI menu interface.
	class CGUIMenu : public CGUIContextMenu
	{
	public:

		//! constructor
		CGUIMenu(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! Updates the absolute position.
		virtual void updateAbsolutePosition() IRR_OVERRIDE;

	protected:

		virtual void recalculateSize() IRR_OVERRIDE;

		//! returns the item highlight-area
		virtual core::rect<s32> getHRect(const SItem& i, const core::rect<s32>& absolute) const IRR_OVERRIDE;

		//! Gets drawing rect of Item
		virtual core::rect<s32> getRect(const SItem& i, const core::rect<s32>& absolute) const IRR_OVERRIDE;
	};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_
#endif //  IRR_C_GUI_MENU_H_INCLUDED
