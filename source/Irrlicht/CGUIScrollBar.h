// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_SCROLL_BAR_H_INCLUDED
#define IRR_C_GUI_SCROLL_BAR_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIScrollBar.h"
#include "IGUIButton.h"

namespace irr
{
namespace gui
{

	class CGUIScrollBar : public IGUIScrollBar
	{
	public:

		//! constructor
		CGUIScrollBar(bool horizontal, IGUIEnvironment* environment,
			IGUIElement* parent, s32 id, core::rect<s32> rectangle,
			bool noclip=false);

		//! destructor
		virtual ~CGUIScrollBar();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) IRR_OVERRIDE;

		//! draws the element and its children
		virtual void draw() IRR_OVERRIDE;

		virtual void OnPostRender(u32 timeMs) IRR_OVERRIDE;


		//! gets the maximum value of the scrollbar.
		virtual s32 getMax() const IRR_OVERRIDE;

		//! sets the maximum value of the scrollbar.
		virtual void setMax(s32 max) IRR_OVERRIDE;

		//! gets the minimum value of the scrollbar.
		virtual s32 getMin() const IRR_OVERRIDE;

		//! sets the minimum value of the scrollbar.
		virtual void setMin(s32 min) IRR_OVERRIDE;

		//! gets the small step value
		virtual s32 getSmallStep() const IRR_OVERRIDE;

		//! sets the small step value
		virtual void setSmallStep(s32 step) IRR_OVERRIDE;

		//! gets the large step value
		virtual s32 getLargeStep() const IRR_OVERRIDE;

		//! sets the large step value
		virtual void setLargeStep(s32 step) IRR_OVERRIDE;

		//! gets the current position of the scrollbar
		virtual s32 getPos() const IRR_OVERRIDE;

		//! sets the position of the scrollbar
		virtual void setPos(s32 pos) IRR_OVERRIDE;

		//! updates the rectangle
		virtual void updateAbsolutePosition() IRR_OVERRIDE;

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

	private:

		void refreshControls();
		s32 getPosFromMousePos(const core::position2di &p) const;

		IGUIButton* UpButton;
		IGUIButton* DownButton;

		core::rect<s32> SliderRect;

		bool Dragging;
		bool Horizontal;
		bool DraggedBySlider;
		bool TrayClick;
		s32 Pos;
		s32 DrawPos;
		s32 DrawHeight;
		s32 Min;
		s32 Max;
		s32 SmallStep;
		s32 LargeStep;
		s32 DesiredPos;
		u32 LastChange;
		video::SColor CurrentIconColor;

		f32 range () const { return (f32) ( Max - Min ); }
	};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif
