// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_COMBO_BOX_H_INCLUDED
#define IRR_C_GUI_COMBO_BOX_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIComboBox.h"
#include "IGUIStaticText.h"
#include "irrString.h"
#include "irrArray.h"

namespace irr
{
namespace gui
{
	class IGUIButton;
	class IGUIListBox;

	//! Single line edit box for editing simple text.
	class CGUIComboBox : public IGUIComboBox
	{
	public:

		//! constructor
		CGUIComboBox(IGUIEnvironment* environment, IGUIElement* parent,
			s32 id, core::rect<s32> rectangle);

		//! Returns amount of items in box
		virtual u32 getItemCount() const IRR_OVERRIDE;

		//! returns string of an item. the idx may be a value from 0 to itemCount-1
		virtual const wchar_t* getItem(u32 idx) const IRR_OVERRIDE;

		//! Returns item data of an item. the idx may be a value from 0 to itemCount-1
		virtual u32 getItemData(u32 idx) const IRR_OVERRIDE;

		//! Returns index based on item data
		virtual s32 getIndexForItemData( u32 data ) const IRR_OVERRIDE;

		//! adds an item and returns the index of it
		virtual u32 addItem(const wchar_t* text, u32 data) IRR_OVERRIDE;

		//! Removes an item from the combo box.
		virtual void removeItem(u32 id) IRR_OVERRIDE;

		//! deletes all items in the combo box
		virtual void clear() IRR_OVERRIDE;

		//! returns the text of the currently selected item
		virtual const wchar_t* getText() const IRR_OVERRIDE;

		//! returns id of selected item. returns -1 if no item is selected.
		virtual s32 getSelected() const IRR_OVERRIDE;

		//! sets the selected item. Set this to -1 if no item should be selected
		virtual void setSelected(s32 idx) IRR_OVERRIDE;

		//! sets the text alignment of the text part
		virtual void setTextAlignment(EGUI_ALIGNMENT horizontal, EGUI_ALIGNMENT vertical) IRR_OVERRIDE;

		//! Set the maximal number of rows for the selection listbox
		virtual void setMaxSelectionRows(u32 max) IRR_OVERRIDE;

		//! Get the maximal number of rows for the selection listbox
		virtual u32 getMaxSelectionRows() const IRR_OVERRIDE;

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

	private:

		void openCloseMenu();
		void sendSelectionChangedEvent();
		void updateListButtonWidth(s32 width);

		IGUIButton* ListButton;
		IGUIStaticText* SelectedText;
		IGUIListBox* ListBox;
		IGUIElement *LastFocus;


		struct SComboData
		{
			SComboData ( const wchar_t * text, u32 data )
				: Name (text), Data ( data ) {}

			core::stringw Name;
			u32 Data;
		};
		core::array< SComboData > Items;

		s32 Selected;
		EGUI_ALIGNMENT HAlign, VAlign;
		u32 MaxSelectionRows;
		bool HasFocus;
		IGUIFont* ActiveFont;
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // IRR_C_GUI_COMBO_BOX_H_INCLUDED
