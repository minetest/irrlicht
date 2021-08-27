// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_I_GUI_COLOR_SELECT_DIALOG_H_INCLUDED
#define IRR_I_GUI_COLOR_SELECT_DIALOG_H_INCLUDED

#include "IGUIElement.h"

namespace irr
{
namespace gui
{

	//! Standard color chooser dialog.
	class IGUIColorSelectDialog : public IGUIElement
	{
	public:

		//! constructor
		IGUIColorSelectDialog(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_COLOR_SELECT_DIALOG, environment, parent, id, rectangle) {}

		//! get chosen color as usual SColor struct
		virtual video::SColor getColor() =0;

		//! get chosen color as HSL values
		virtual video::SColorHSL getColorHSL() =0;

	};


} // end namespace gui
} // end namespace irr

#endif
