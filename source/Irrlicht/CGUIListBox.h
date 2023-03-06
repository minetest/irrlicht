// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_LIST_BOX_H_INCLUDED
#define IRR_C_GUI_LIST_BOX_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIListBox.h"
#include "irrArray.h"

namespace irr
{
namespace gui
{

	class IGUIFont;
	class IGUIScrollBar;

	class CGUIListBox : public IGUIListBox
	{
	public:
		//! constructor
		CGUIListBox(IGUIEnvironment* environment, IGUIElement* parent,
			s32 id, core::rect<s32> rectangle, bool clip=true,
			bool drawBack=false, bool moveOverSelect=false);

		//! destructor
		virtual ~CGUIListBox();

		//! returns amount of list items
		virtual u32 getItemCount() const IRR_OVERRIDE;

		//! returns string of a list item. the id may be a value from 0 to itemCount-1
		virtual const wchar_t* getListItem(u32 id) const IRR_OVERRIDE;

		//! clears the list
		virtual void clear() IRR_OVERRIDE;

		//! returns id of selected item. returns -1 if no item is selected.
		virtual s32 getSelected() const IRR_OVERRIDE;

		//! sets the selected item. Set this to -1 if no item should be selected
		virtual void setSelected(s32 id) IRR_OVERRIDE;

		//! sets the selected item. Set this to -1 if no item should be selected
		virtual void setSelected(const wchar_t *item) IRR_OVERRIDE;

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		//! adds an list item with an icon
		//! \param text Text of list entry
		//! \param icon Sprite index of the Icon within the current sprite bank. Set it to -1 if you want no icon
		//! \return
		//! returns the id of the new created item
		virtual u32 addItem(const wchar_t* text, s32 icon=-1) IRR_OVERRIDE;

		//! Insert the item at the given index
		//! Return the index on success or -1 on failure.
		virtual s32 insertItem(u32 index, const wchar_t* text, s32 icon=-1) IRR_OVERRIDE;

		//! set the item at the given index
		virtual void setItem(u32 index, const wchar_t* text, s32 icon=-1) IRR_OVERRIDE;

		//! Returns the icon of an item
		virtual s32 getIcon(u32 id) const IRR_OVERRIDE;

		//! removes an item from the list
		virtual void removeItem(u32 id) IRR_OVERRIDE;

		//! get the the id of the item at the given absolute coordinates
		virtual s32 getItemAt(s32 xpos, s32 ypos) const IRR_OVERRIDE;

		//! Sets the sprite bank which should be used to draw list icons. This font is set to the sprite bank of
		//! the built-in-font by default. A sprite can be displayed in front of every list item.
		//! An icon is an index within the icon sprite bank. Several default icons are available in the
		//! skin through getIcon
		virtual void setSpriteBank(IGUISpriteBank* bank) IRR_OVERRIDE;

		//! set whether the listbox should scroll to newly selected items
		virtual void setAutoScrollEnabled(bool scroll) IRR_OVERRIDE;

		//! returns true if automatic scrolling is enabled, false if not.
		virtual bool isAutoScrollEnabled() const IRR_OVERRIDE;

		//! Update the position and size of the listbox, and update the scrollbar
		virtual void updateAbsolutePosition() IRR_OVERRIDE;

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

		//! set all item colors at given index to color
		virtual void setItemOverrideColor(u32 index, video::SColor color) IRR_OVERRIDE;

		//! set all item colors of specified type at given index to color
		virtual void setItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType, video::SColor color) IRR_OVERRIDE;

		//! clear all item colors at index
		virtual void clearItemOverrideColor(u32 index) IRR_OVERRIDE;

		//! clear item color at index for given colortype
		virtual void clearItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType) IRR_OVERRIDE;

		//! has the item at index its color overwritten?
		virtual bool hasItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType) const IRR_OVERRIDE;

		//! return the overwrite color at given item index.
		virtual video::SColor getItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType) const IRR_OVERRIDE;

		//! return the default color which is used for the given colorType
		virtual video::SColor getItemDefaultColor(EGUI_LISTBOX_COLOR colorType) const IRR_OVERRIDE;

		//! Swap the items at the given indices
		virtual void swapItems(u32 index1, u32 index2) IRR_OVERRIDE;

		//! set global itemHeight
		virtual void setItemHeight( s32 height ) IRR_OVERRIDE;

		//! Sets whether to draw the background
		virtual void setDrawBackground(bool draw) IRR_OVERRIDE;

		//! Access the vertical scrollbar
		virtual IGUIScrollBar* getVerticalScrollBar() const IRR_OVERRIDE;

	private:

		struct ListItem
		{
			ListItem() : Icon(-1)
			{}

			core::stringw Text;
			s32 Icon;

			// A multicolor extension
			struct ListItemOverrideColor
			{
				ListItemOverrideColor() : Use(false) {}
				bool Use;
				video::SColor Color;
			};
			ListItemOverrideColor OverrideColors[EGUI_LBC_COUNT];
		};

		void recalculateItemHeight();
		void selectNew(s32 ypos, bool onlyHover=false);
		void recalculateScrollPos();
		void updateScrollBarSize(s32 size);

		// extracted that function to avoid copy&paste code
		void recalculateItemWidth(s32 icon);

		// get labels used for serialization
		bool getSerializationLabels(EGUI_LISTBOX_COLOR colorType, core::stringc & useColorLabel, core::stringc & colorLabel) const;

		core::array< ListItem > Items;
		s32 Selected;
		s32 HoverSelected;	// When >= 0 we're in the middle of changing selection while mouse is pressed. We need to know so selected again isn't called too often.
		s32 ItemHeight;
		s32 ItemHeightOverride;
		s32 TotalItemHeight;
		s32 ItemsIconWidth;
		gui::IGUIFont* Font;
		gui::IGUISpriteBank* IconBank;
		gui::IGUIScrollBar* ScrollBar;
		u32 SelectTime;
		u32 LastKeyTime;
		core::stringw KeyBuffer;
		bool Selecting;
		bool DrawBack;
		bool MoveOverSelect;
		bool AutoScroll;
		bool HighlightWhenNotFocused;
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif
