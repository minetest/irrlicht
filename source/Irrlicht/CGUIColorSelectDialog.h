// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_COLOR_SELECT_DIALOG_H_INCLUDED
#define IRR_C_GUI_COLOR_SELECT_DIALOG_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIColorSelectDialog.h"
#include "IGUIButton.h"
#include "IGUISpinBox.h"
#include "IGUIImage.h"
#include "irrArray.h"


namespace irr
{
namespace gui
{

	class CGUIColorSelectDialog : public IGUIColorSelectDialog
	{
	public:

		//! constructor
		CGUIColorSelectDialog(const wchar_t* title, IGUIEnvironment* environment, IGUIElement* parent, s32 id);

		//! destructor
		virtual ~CGUIColorSelectDialog();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		virtual video::SColor getColor() IRR_OVERRIDE;
		virtual video::SColorHSL getColorHSL() IRR_OVERRIDE;

	private:

		//! sends the event that the file has been selected.
		void sendSelectedEvent();

		//! sends the event that the file choose process has been canceld
		void sendCancelEvent();

		core::position2d<s32> DragStart;
		bool Dragging;
		IGUIButton* CloseButton;
		IGUIButton* OKButton;
		IGUIButton* CancelButton;

		core::array<IGUISpinBox*> Battery;

		struct SColorCircle
		{
			IGUIImage * Control;
			video::ITexture * Texture;
		};
		SColorCircle ColorRing;

		void buildColorRing( const core::dimension2d<u32> & dim, s32 supersample, const video::SColor& borderColor );
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // IRR_C_GUI_COLOR_SELECT_DIALOG_H_INCLUDED
