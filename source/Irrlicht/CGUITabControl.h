// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_TAB_CONTROL_H_INCLUDED
#define IRR_C_GUI_TAB_CONTROL_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUITabControl.h"
#include "irrArray.h"

namespace irr
{
namespace gui
{
	class CGUITabControl;
	class IGUIButton;

	// A tab, onto which other gui elements could be added.
	class CGUITab : public IGUITab
	{
	public:

		//! constructor
		CGUITab(IGUIEnvironment* environment,
			IGUIElement* parent, const core::rect<s32>& rectangle,
			s32 id);

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		//! sets if the tab should draw its background
		virtual void setDrawBackground(bool draw=true) IRR_OVERRIDE;

		//! sets the color of the background, if it should be drawn.
		virtual void setBackgroundColor(video::SColor c) IRR_OVERRIDE;

		//! sets the color of the text
		virtual void setTextColor(video::SColor c) IRR_OVERRIDE;

		//! returns true if the tab is drawing its background, false if not
		virtual bool isDrawingBackground() const IRR_OVERRIDE;

		//! returns the color of the background
		virtual video::SColor getBackgroundColor() const IRR_OVERRIDE;

		virtual video::SColor getTextColor() const IRR_OVERRIDE;

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

	private:

		video::SColor BackColor;
		bool OverrideTextColorEnabled;
		video::SColor TextColor;
		bool DrawBackground;
	};


	//! A standard tab control
	class CGUITabControl : public IGUITabControl
	{
	public:

		//! destructor
		CGUITabControl(IGUIEnvironment* environment,
			IGUIElement* parent, const core::rect<s32>& rectangle,
			bool fillbackground=true, bool border=true, s32 id=-1);

		//! destructor
		virtual ~CGUITabControl();

		//! Adds a tab
		virtual IGUITab* addTab(const wchar_t* caption, s32 id=-1) IRR_OVERRIDE;

		//! Adds an existing tab
		virtual s32 addTab(IGUITab* tab) IRR_OVERRIDE;

		//! Insert the tab at the given index
		virtual IGUITab* insertTab(s32 idx, const wchar_t* caption, s32 id=-1) IRR_OVERRIDE;

		//! Insert an existing tab
		/** Note that it will also add the tab as a child of this TabControl.
		\return Index of added tab (should be same as the one passed) or -1 for failure*/
		virtual s32 insertTab(s32 idx, IGUITab* tab, bool serializationMode) IRR_OVERRIDE;

		//! Removes a tab from the tabcontrol
		virtual void removeTab(s32 idx) IRR_OVERRIDE;

		//! Clears the tabcontrol removing all tabs
		virtual void clear() IRR_OVERRIDE;

		//! Returns amount of tabs in the tabcontrol
		virtual s32 getTabCount() const IRR_OVERRIDE;

		//! Returns a tab based on zero based index
		virtual IGUITab* getTab(s32 idx) const IRR_OVERRIDE;

		//! Brings a tab to front.
		virtual bool setActiveTab(s32 idx) IRR_OVERRIDE;

		//! Brings a tab to front.
		virtual bool setActiveTab(IGUITab *tab) IRR_OVERRIDE;

		//! For given given tab find it's zero-based index (or -1 for not found)
		virtual s32 getTabIndex(const IGUIElement *tab) const IRR_OVERRIDE;

		//! Returns which tab is currently active
		virtual s32 getActiveTab() const IRR_OVERRIDE;

		//! get the the id of the tab at the given absolute coordinates
		virtual s32 getTabAt(s32 xpos, s32 ypos) const IRR_OVERRIDE;

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		//! Removes a child.
		virtual void removeChild(IGUIElement* child) IRR_OVERRIDE;

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;
		//! Set the height of the tabs
		virtual void setTabHeight( s32 height ) IRR_OVERRIDE;

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

		//! Get the height of the tabs
		virtual s32 getTabHeight() const IRR_OVERRIDE;

		//! set the maximal width of a tab. Per default width is 0 which means "no width restriction".
		virtual void setTabMaxWidth(s32 width ) IRR_OVERRIDE;

		//! get the maximal width of a tab
		virtual s32 getTabMaxWidth() const IRR_OVERRIDE;

		//! Set the alignment of the tabs
		//! note: EGUIA_CENTER is not an option
		virtual void setTabVerticalAlignment( gui::EGUI_ALIGNMENT alignment ) IRR_OVERRIDE;

		//! Get the alignment of the tabs
		virtual gui::EGUI_ALIGNMENT getTabVerticalAlignment() const IRR_OVERRIDE;

		//! Set the extra width added to tabs on each side of the text
		virtual void setTabExtraWidth( s32 extraWidth ) IRR_OVERRIDE;

		//! Get the extra width added to tabs on each side of the text
		virtual s32 getTabExtraWidth() const IRR_OVERRIDE;

		//! Update the position of the element, decides scroll button status
		virtual void updateAbsolutePosition() IRR_OVERRIDE;

	private:

		void scrollLeft();
		void scrollRight();
		bool needScrollControl( s32 startIndex=0, bool withScrollControl=false );
		s32 calcTabWidth(s32 pos, IGUIFont* font, const wchar_t* text, bool withScrollControl ) const;
		core::rect<s32> calcTabPos();
		void setVisibleTab(s32 idx);
		void removeTabButNotChild(s32 idx);

		void recalculateScrollButtonPlacement();
		void recalculateScrollBar();
		void refreshSprites();

		core::array<IGUITab*> Tabs;
		s32 ActiveTabIndex;
		bool Border;
		bool FillBackground;
		bool ScrollControl;
		s32 TabHeight;
		gui::EGUI_ALIGNMENT VerticalAlignment;
		IGUIButton* UpButton;
		IGUIButton* DownButton;
		s32 TabMaxWidth;
		s32 CurrentScrollTabIndex;
		s32 TabExtraWidth;
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif
