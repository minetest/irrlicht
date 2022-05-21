// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_ENVIRONMENT_H_INCLUDED__
#define __C_GUI_ENVIRONMENT_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIEnvironment.h"
#include "IGUIElement.h"
#include "irrArray.h"
#include "IFileSystem.h"
#include "IOSOperator.h"

namespace irr
{

namespace gui
{

class CGUIEnvironment : public IGUIEnvironment, public IGUIElement
{
public:

	//! constructor
	CGUIEnvironment(io::IFileSystem* fs, video::IVideoDriver* driver, IOSOperator* op);

	//! destructor
	virtual ~CGUIEnvironment();

	//! draws all gui elements
	virtual void drawAll(bool useScreenSize) _IRR_OVERRIDE_;

	//! returns the current video driver
	virtual video::IVideoDriver* getVideoDriver() const _IRR_OVERRIDE_;

	//! returns pointer to the filesystem
	virtual io::IFileSystem* getFileSystem() const _IRR_OVERRIDE_;

	//! returns a pointer to the OS operator
	virtual IOSOperator* getOSOperator() const _IRR_OVERRIDE_;

	//! posts an input event to the environment
	virtual bool postEventFromUser(const SEvent& event) _IRR_OVERRIDE_;

	//! This sets a new event receiver for gui events. Usually you do not have to
	//! use this method, it is used by the internal engine.
	virtual void setUserEventReceiver(IEventReceiver* evr) _IRR_OVERRIDE_;

	//! removes all elements from the environment
	virtual void clear() _IRR_OVERRIDE_;

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

	//! returns the current gui skin
	virtual IGUISkin* getSkin() const _IRR_OVERRIDE_;

	//! Sets a new GUI Skin
	virtual void setSkin(IGUISkin* skin) _IRR_OVERRIDE_;

	//! Creates a new GUI Skin based on a template.
	/** \return Returns a pointer to the created skin.
	If you no longer need the skin, you should call IGUISkin::drop().
	See IReferenceCounted::drop() for more information. */
	virtual IGUISkin* createSkin(EGUI_SKIN_TYPE type) _IRR_OVERRIDE_;

	//! Creates the image list from the given texture.
	virtual IGUIImageList* createImageList(video::ITexture* texture,
					core::dimension2d<s32> imageSize, bool useAlphaChannel) _IRR_OVERRIDE_;

	//! returns the font
	virtual IGUIFont* getFont(const io::path& filename) _IRR_OVERRIDE_;

	//! add an externally loaded font
	virtual IGUIFont* addFont(const io::path& name, IGUIFont* font) _IRR_OVERRIDE_;

	//! remove loaded font
	virtual void removeFont(IGUIFont* font) _IRR_OVERRIDE_;

	//! returns default font
	virtual IGUIFont* getBuiltInFont() const _IRR_OVERRIDE_;

	//! returns the sprite bank
	virtual IGUISpriteBank* getSpriteBank(const io::path& filename) _IRR_OVERRIDE_;

	//! returns the sprite bank
	virtual IGUISpriteBank* addEmptySpriteBank(const io::path& name) _IRR_OVERRIDE_;

	//! adds an button. The returned pointer must not be dropped.
	virtual IGUIButton* addButton(const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0,const wchar_t* tooltiptext = 0) _IRR_OVERRIDE_;

	//! adds a scrollbar. The returned pointer must not be dropped.
	virtual IGUIScrollBar* addScrollBar(bool horizontal, const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1) _IRR_OVERRIDE_;

	//! Adds an image element.
	virtual IGUIImage* addImage(video::ITexture* image, core::position2d<s32> pos,
		bool useAlphaChannel=true, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0) _IRR_OVERRIDE_;

	//! adds an image. The returned pointer must not be dropped.
	virtual IGUIImage* addImage(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0, bool useAlphaChannel=true) _IRR_OVERRIDE_;

	//! adds a checkbox
	virtual IGUICheckBox* addCheckBox(bool checked, const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0) _IRR_OVERRIDE_;

	//! adds a list box
	virtual IGUIListBox* addListBox(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1, bool drawBackground=false) _IRR_OVERRIDE_;

	//! Adds a file open dialog.
	virtual IGUIFileOpenDialog* addFileOpenDialog(const wchar_t* title = 0,
			bool modal=true, IGUIElement* parent=0, s32 id=-1,
			bool restoreCWD=false, io::path::char_type* startDir=0) _IRR_OVERRIDE_;

	//! adds a static text. The returned pointer must not be dropped.
	virtual IGUIStaticText* addStaticText(const wchar_t* text, const core::rect<s32>& rectangle,
		bool border=false, bool wordWrap=true, IGUIElement* parent=0, s32 id=-1, bool drawBackground = false) _IRR_OVERRIDE_;

	//! Adds an edit box. The returned pointer must not be dropped.
	virtual IGUIEditBox* addEditBox(const wchar_t* text, const core::rect<s32>& rectangle,
		bool border=false, IGUIElement* parent=0, s32 id=-1) _IRR_OVERRIDE_;

	//! Adds a tab control to the environment.
	virtual IGUITabControl* addTabControl(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, bool fillbackground=false, bool border=true, s32 id=-1) _IRR_OVERRIDE_;

	//! Adds tab to the environment.
	virtual IGUITab* addTab(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1) _IRR_OVERRIDE_;

	//! Adds a combo box to the environment.
	virtual IGUIComboBox* addComboBox(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1) _IRR_OVERRIDE_;

	//! sets the focus to an element
	virtual bool setFocus(IGUIElement* element) _IRR_OVERRIDE_;

	//! removes the focus from an element
	virtual bool removeFocus(IGUIElement* element) _IRR_OVERRIDE_;

	//! Returns if the element has focus
	virtual bool hasFocus(const IGUIElement* element, bool checkSubElements=false) const _IRR_OVERRIDE_;

	//! Returns the element with the focus
	virtual IGUIElement* getFocus() const _IRR_OVERRIDE_;

	//! Returns the element last known to be under the mouse
	virtual IGUIElement* getHovered() const _IRR_OVERRIDE_;

	//! Returns the root gui element.
	virtual IGUIElement* getRootGUIElement() _IRR_OVERRIDE_;

	virtual void OnPostRender( u32 time ) _IRR_OVERRIDE_;

	//! Find the next element which would be selected when pressing the tab-key
	virtual IGUIElement* getNextElement(bool reverse=false, bool group=false) _IRR_OVERRIDE_;

	//! Set the way the gui will handle focus changes
	virtual void setFocusBehavior(u32 flags) _IRR_OVERRIDE_;

	//! Get the way the gui does handle focus changes
	virtual u32 getFocusBehavior() const _IRR_OVERRIDE_;

	//! Adds a IGUIElement to deletion queue.
	virtual void addToDeletionQueue(IGUIElement* element) _IRR_OVERRIDE_;

private:

	//! clears the deletion queue
	void clearDeletionQueue();

	void updateHoveredElement(core::position2d<s32> mousePos);

	void loadBuiltInFont();

	struct SFont
	{
		io::SNamedPath NamedPath;
		IGUIFont* Font;

		bool operator < (const SFont& other) const
		{
			return (NamedPath < other.NamedPath);
		}
	};

	struct SSpriteBank
	{
		io::SNamedPath NamedPath;
		IGUISpriteBank* Bank;

		bool operator < (const SSpriteBank& other) const
		{
			return (NamedPath < other.NamedPath);
		}
	};

	struct SToolTip
	{
		IGUIStaticText* Element;
		u32 LastTime;
		u32 EnterTime;
		u32 LaunchTime;
		u32 RelaunchTime;
	};

	SToolTip ToolTip;

	core::array<SFont> Fonts;
	core::array<SSpriteBank> Banks;
	video::IVideoDriver* Driver;
	IGUIElement* Hovered;
	IGUIElement* HoveredNoSubelement;	// subelements replaced by their parent, so you only have 'real' elements here
	IGUIElement* Focus;
	core::position2d<s32> LastHoveredMousePos;
	IGUISkin* CurrentSkin;
	io::IFileSystem* FileSystem;
	IEventReceiver* UserReceiver;
	IOSOperator* Operator;
	u32 FocusFlags;
	core::array<IGUIElement*> DeletionQueue;

	static const io::path DefaultFontName;
};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // __C_GUI_ENVIRONMENT_H_INCLUDED__


