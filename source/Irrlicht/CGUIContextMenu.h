// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_CONTEXT_MENU_H_INCLUDED__
#define __C_GUI_CONTEXT_MENU_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIContextMenu.h"
#include "irrString.h"
#include "irrArray.h"
#include "IGUIFont.h"

namespace irr
{
namespace gui
{

	//! GUI Context menu interface.
	class CGUIContextMenu : public IGUIContextMenu
	{
	public:

		//! constructor
		CGUIContextMenu(IGUIEnvironment* environment,
			IGUIElement* parent, s32 id, core::rect<s32> rectangle,
			bool getFocus = true, bool allowFocus = true);

		//! destructor
		virtual ~CGUIContextMenu();

		//! set behavior when menus are closed
		virtual void setCloseHandling(ECONTEXT_MENU_CLOSE onClose) _IRR_OVERRIDE_;

		//! get current behavior when the menue will be closed
		virtual ECONTEXT_MENU_CLOSE getCloseHandling() const _IRR_OVERRIDE_;

		//! Returns amount of menu items
		virtual u32 getItemCount() const _IRR_OVERRIDE_;

		//! Adds a menu item.
		virtual u32 addItem(const wchar_t* text, s32 commandid,
				bool enabled, bool hasSubMenu, bool checked, bool autoChecking) _IRR_OVERRIDE_;

		//! Insert a menu item at specified position.
		virtual u32 insertItem(u32 idx, const wchar_t* text, s32 commandId, bool enabled,
			bool hasSubMenu, bool checked, bool autoChecking) _IRR_OVERRIDE_;

		//! Find a item which has the given CommandId starting from given index
		virtual s32 findItemWithCommandId(s32 commandId, u32 idxStartSearch) const _IRR_OVERRIDE_;

		//! Adds a separator item to the menu
		virtual void addSeparator() _IRR_OVERRIDE_;

		//! Returns text of the menu item.
		virtual const wchar_t* getItemText(u32 idx) const _IRR_OVERRIDE_;

		//! Sets text of the menu item.
		virtual void setItemText(u32 idx, const wchar_t* text) _IRR_OVERRIDE_;

		//! Returns if a menu item is enabled
		virtual bool isItemEnabled(u32 idx) const _IRR_OVERRIDE_;

		//! Sets if the menu item should be enabled.
		virtual void setItemEnabled(u32 idx, bool enabled) _IRR_OVERRIDE_;

		//! Returns if a menu item is checked
		virtual bool isItemChecked(u32 idx) const _IRR_OVERRIDE_;

		//! Sets if the menu item should be checked.
		virtual void setItemChecked(u32 idx, bool enabled) _IRR_OVERRIDE_;

		//! Removes a menu item
		virtual void removeItem(u32 idx) _IRR_OVERRIDE_;

		//! Removes all menu items
		virtual void removeAllItems() _IRR_OVERRIDE_;

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

		//! draws the element and its children
		virtual void draw() _IRR_OVERRIDE_;

		//! Returns the selected item in the menu
		virtual s32 getSelectedItem() const _IRR_OVERRIDE_;

		//! Returns a pointer to the submenu of an item.
		//! \return Pointer to the submenu of an item.
		virtual IGUIContextMenu* getSubMenu(u32 idx) const _IRR_OVERRIDE_;

		//! Sets the visible state of this element.
		virtual void setVisible(bool visible) _IRR_OVERRIDE_;

		//! should the element change the checked status on clicking
		virtual void setItemAutoChecking(u32 idx, bool autoChecking) _IRR_OVERRIDE_;

		//! does the element change the checked status on clicking
		virtual bool getItemAutoChecking(u32 idx) const _IRR_OVERRIDE_;

		//! Returns command id of a menu item
		virtual s32 getItemCommandId(u32 idx) const _IRR_OVERRIDE_;

		//! Sets the command id of a menu item
		virtual void setItemCommandId(u32 idx, s32 id) _IRR_OVERRIDE_;

		//! Adds a sub menu from an element that already exists.
		virtual void setSubMenu(u32 index, CGUIContextMenu* menu);

		//! When an eventparent is set it receives events instead of the usual parent element
		virtual void setEventParent(IGUIElement *parent) _IRR_OVERRIDE_;

	protected:

		void closeAllSubMenus();
		bool hasOpenSubMenu() const;

		struct SItem
		{
			core::stringw Text;
			bool IsSeparator;
			bool Enabled;
			bool Checked;
			bool AutoChecking;
			core::dimension2d<u32> Dim;
			s32 PosY;
			CGUIContextMenu* SubMenu;
			s32 CommandId;
		};

		virtual void recalculateSize();

		//! returns true, if an element was highlighted
		virtual bool highlight(const core::position2d<s32>& p, bool canOpenSubMenu);

		//! sends a click Returns:
		//! 0 if click went outside of the element,
		//! 1 if a valid button was clicked,
		//! 2 if a nonclickable element was clicked
		virtual u32 sendClick(const core::position2d<s32>& p);

		//! returns the item highlight-area
		virtual core::rect<s32> getHRect(const SItem& i, const core::rect<s32>& absolute) const;

		//! Gets drawing rect of Item
		virtual core::rect<s32> getRect(const SItem& i, const core::rect<s32>& absolute) const;


		core::array<SItem> Items;
		core::position2d<s32> Pos;
		IGUIElement* EventParent;
		IGUIFont *LastFont;
		ECONTEXT_MENU_CLOSE CloseHandling;
		s32 HighLighted;
		u32 ChangeTime;
		bool AllowFocus;
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // __C_GUI_CONTEXT_MENU_H_INCLUDED__

