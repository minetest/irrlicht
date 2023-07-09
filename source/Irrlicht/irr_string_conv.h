/*
Minetest
Copyright (C) 2010-2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <string>
#include "irrString.h"

// Try to avoid converting between wide and UTF-8 unless you need to
// input/output stuff via Irrlicht.

/*
	std::string / std::wstring conversion functions.
*/

std::wstring utf8_to_wide(const std::string &input);
std::string wide_to_utf8(const std::wstring &input);

/*
	irr::core::stringc / irr::core::stringw conversion functions.
	Implemented using the std versions.
*/

namespace irr
{
namespace core
{

size_t multibyteToWString(irr::core::stringw &destination, const irr::core::stringc &source);
size_t multibyteToWString(irr::core::stringw &destination, const char *source);
size_t wStringToMultibyte(irr::core::stringc &destination, const irr::core::stringw &source);
size_t wStringToMultibyte(irr::core::stringc &destination, const wchar_t *source);

}
}

/*
	Another two interesting string conversion functions.
	std::string / irr::core::stringw this time.
*/

//! Create a UTF8 std::string from an irr::core::stringw.
inline std::string stringw_to_utf8(const irr::core::stringw &input)
{
	std::wstring str(input.c_str());
	return wide_to_utf8(str);
}

//! Create an irr::core:stringw from a UTF8 std::string.
inline irr::core::stringw utf8_to_stringw(const std::string &input)
{
	std::wstring str = utf8_to_wide(input);
	return irr::core::stringw(str.c_str());
}
