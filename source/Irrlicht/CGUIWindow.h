// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_WINDOW_H_INCLUDED
#define IRR_C_GUI_WINDOW_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIWindow.h"

namespace irr
{
namespace gui
{
	class IGUIButton;

	class CGUIWindow : public IGUIWindow
	{
	public:

		//! constructor
		CGUIWindow(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! destructor
		virtual ~CGUIWindow();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! update absolute position
		virtual void updateAbsolutePosition() IRR_OVERRIDE;

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		//! Returns pointer to the close button
		virtual IGUIButton* getCloseButton() const IRR_OVERRIDE;

		//! Returns pointer to the minimize button
		virtual IGUIButton* getMinimizeButton() const IRR_OVERRIDE;

		//! Returns pointer to the maximize button
		virtual IGUIButton* getMaximizeButton() const IRR_OVERRIDE;

		//! Returns true if the window is draggable, false if not
		virtual bool isDraggable() const IRR_OVERRIDE;

		//! Sets whether the window is draggable
		virtual void setDraggable(bool draggable) IRR_OVERRIDE;

		//! Set if the window background will be drawn
		virtual void setDrawBackground(bool draw) IRR_OVERRIDE;

		//! Get if the window background will be drawn
		virtual bool getDrawBackground() const IRR_OVERRIDE;

		//! Set if the window titlebar will be drawn
		//! Note: If the background is not drawn, then the titlebar is automatically also not drawn
		virtual void setDrawTitlebar(bool draw) IRR_OVERRIDE;

		//! Get if the window titlebar will be drawn
		virtual bool getDrawTitlebar() const IRR_OVERRIDE;

		//! Returns the rectangle of the drawable area (without border and without titlebar)
		virtual core::rect<s32> getClientRect() const IRR_OVERRIDE;

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

	protected:

		void updateClientRect();
		void refreshSprites();

		IGUIButton* CloseButton;
		IGUIButton* MinButton;
		IGUIButton* RestoreButton;
		core::rect<s32> ClientRect;
		video::SColor CurrentIconColor;

		core::position2d<s32> DragStart;
		bool Dragging, IsDraggable;
		bool DrawBackground;
		bool DrawTitlebar;
		bool IsActive;
	};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif
