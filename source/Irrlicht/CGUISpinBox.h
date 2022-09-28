// Copyright (C) 2006-2012 Michael Zeilfelder
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_SPIN_BOX_H_INCLUDED
#define IRR_C_GUI_SPIN_BOX_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISpinBox.h"

namespace irr
{
namespace gui
{
	class IGUIEditBox;
	class IGUIButton;

	class CGUISpinBox : public IGUISpinBox
	{
	public:

		//! constructor
		CGUISpinBox(const wchar_t* text, bool border, IGUIEnvironment* environment,
			IGUIElement* parent, s32 id, const core::rect<s32>& rectangle);

		//! destructor
		virtual ~CGUISpinBox();

		//! Access the edit box used in the spin control
		/** \param enable: If set to true, the override color, which can be set
		with IGUIEditBox::setOverrideColor is used, otherwise the
		EGDC_BUTTON_TEXT color of the skin. */
		virtual IGUIEditBox* getEditBox() const IRR_OVERRIDE;

		//! set the current value of the spinbox
		/** \param val: value to be set in the spinbox */
		virtual void setValue(f32 val) IRR_OVERRIDE;

		//! Get the current value of the spinbox
		virtual f32 getValue() const IRR_OVERRIDE;

		//! Get the value the spinbox would have for the given text
		virtual f32 getValueFor(const wchar_t* text) const IRR_OVERRIDE;

		//! set the range of values which can be used in the spinbox
		/** \param min: minimum value
		\param max: maximum value */
		virtual void setRange(f32 min, f32 max) IRR_OVERRIDE;

		//! get the minimum value which can be used in the spinbox
		virtual f32 getMin() const IRR_OVERRIDE;

		//! get the maximum value which can be used in the spinbox
		virtual f32 getMax() const IRR_OVERRIDE;

		//! step size by which values are changed when pressing the spin buttons
		/** \param step: stepsize used for value changes when pressing spin buttons */
		virtual void setStepSize(f32 step=1.f) IRR_OVERRIDE;

		//! returns the step size
		virtual f32 getStepSize() const IRR_OVERRIDE;

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! Draws the element and its children.
		virtual void draw() IRR_OVERRIDE;

		//! Sets the new caption of the element
		virtual void setText(const wchar_t* text) IRR_OVERRIDE;

		//! Returns caption of this element.
		virtual const wchar_t* getText() const IRR_OVERRIDE;

		//! Sets the number of decimal places to display.
		//! Note that this also rounds the range to the same number of decimal places.
		/** \param places: The number of decimal places to display, use -1 to reset */
		virtual void setDecimalPlaces(s32 places) IRR_OVERRIDE;

		//! Sets when the spinbox has to validate entered text.
		/** \param validateOn Can be any combination of EGUI_SPINBOX_VALIDATION bit flags */
		virtual void setValidateOn(u32 validateOn) IRR_OVERRIDE;

		//! Gets when the spinbox has to validate entered text.
		virtual u32 getValidateOn() const IRR_OVERRIDE;

		//! Gets previous value in EGET_SPINBOX_CHANGED events
		virtual f32 getOldValue() const IRR_OVERRIDE
		{
			return OldValue;
		}

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

	protected:
		void verifyValueRange(f32 val);
		void refreshSprites();

		IGUIEditBox * EditBox;
		IGUIButton * ButtonSpinUp;
		IGUIButton * ButtonSpinDown;
		video::SColor CurrentIconColor;
		f32 StepSize;
		f32 RangeMin;
		f32 RangeMax;

		core::stringw FormatString;
		s32 DecimalPlaces;
		u32 ValidateOn;	// combination of EGUI_SPINBOX_VALIDATION bit-flags
		f32 OldValue;
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // IRR_C_GUI_SPIN_BOX_H_INCLUDED
