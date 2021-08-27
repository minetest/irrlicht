// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_CHECKBOX_H_INCLUDED
#define IRR_C_GUI_CHECKBOX_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUICheckBox.h"

namespace irr
{
namespace gui
{

	class CGUICheckBox : public IGUICheckBox
	{
	public:

		//! constructor
		CGUICheckBox(bool checked, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! set if box is checked
		virtual void setChecked(bool checked) IRR_OVERRIDE;

		//! returns if box is checked
		virtual bool isChecked() const IRR_OVERRIDE;

		//! Sets whether to draw the background
		virtual void setDrawBackground(bool draw) IRR_OVERRIDE;

		//! Checks if background drawing is enabled
		/** \return true if background drawing is enabled, false otherwise */
		virtual bool isDrawBackgroundEnabled() const IRR_OVERRIDE;

		//! Sets whether to draw the border
		virtual void setDrawBorder(bool draw) IRR_OVERRIDE;

		//! Checks if border drawing is enabled
		/** \return true if border drawing is enabled, false otherwise */
		virtual bool isDrawBorderEnabled() const IRR_OVERRIDE;

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

	private:

		u32 CheckTime;
		bool Pressed;
		bool Checked;
		bool Border;
		bool Background;
	};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // IRR_C_GUI_CHECKBOX_H_INCLUDED
