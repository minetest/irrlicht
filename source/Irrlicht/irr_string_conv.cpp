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

#include "irr_string_conv.h"
#include "os.h"
#include "irr_hex.h"

#ifndef _WIN32
	#include <iconv.h>
#else
	#include <windows.h>
#endif

/*
	std::string / std::wstring conversion functions.
*/

#ifndef _WIN32

static bool convert(const char *to, const char *from, char *outbuf,
		size_t *outbuf_size, char *inbuf, size_t inbuf_size)
{
	iconv_t cd = iconv_open(to, from);

	char *inbuf_ptr = inbuf;
	char *outbuf_ptr = outbuf;

	size_t *inbuf_left_ptr = &inbuf_size;

	const size_t old_outbuf_size = *outbuf_size;
	size_t old_size = inbuf_size;
	while (inbuf_size > 0) {
		iconv(cd, &inbuf_ptr, inbuf_left_ptr, &outbuf_ptr, outbuf_size);
		if (inbuf_size == old_size) {
			iconv_close(cd);
			return false;
		}
		old_size = inbuf_size;
	}

	iconv_close(cd);
	*outbuf_size = old_outbuf_size - *outbuf_size;
	return true;
}

#ifdef __ANDROID__
// On Android iconv disagrees how big a wchar_t is for whatever reason
const char *DEFAULT_ENCODING = "UTF-32LE";
#elif defined(__NetBSD__) || defined(__OpenBSD__)
	// NetBSD does not allow "WCHAR_T" as a charset input to iconv.
	#include <sys/endian.h>
	#if BYTE_ORDER == BIG_ENDIAN
	const char *DEFAULT_ENCODING = "UTF-32BE";
	#else
	const char *DEFAULT_ENCODING = "UTF-32LE";
	#endif
#else
const char *DEFAULT_ENCODING = "WCHAR_T";
#endif

std::wstring utf8_to_wide(const std::string &input)
{
	const size_t inbuf_size = input.length();
	// maximum possible size, every character is sizeof(wchar_t) bytes
	size_t outbuf_size = input.length() * sizeof(wchar_t);

	char *inbuf = new char[inbuf_size]; // intentionally NOT null-terminated
	memcpy(inbuf, input.c_str(), inbuf_size);
	std::wstring out;
	out.resize(outbuf_size / sizeof(wchar_t));

#if defined(__ANDROID__) || defined(__NetBSD__) || defined(__OpenBSD__)
	static_assert(sizeof(wchar_t) == 4, "Unexpected wide char size");
#endif

	char *outbuf = reinterpret_cast<char*>(&out[0]);
	if (!convert(DEFAULT_ENCODING, "UTF-8", outbuf, &outbuf_size, inbuf, inbuf_size)) {
		irr::os::Printer::log(("Couldn't convert UTF-8 string 0x" + hex_encode(input) +
			" into wstring").c_str(), irr::ELL_WARNING);
		delete[] inbuf;
		return L"<invalid UTF-8 string>";
	}
	delete[] inbuf;

	out.resize(outbuf_size / sizeof(wchar_t));
	return out;
}

std::string wide_to_utf8(const std::wstring &input)
{
	const size_t inbuf_size = input.length() * sizeof(wchar_t);
	// maximum possible size: utf-8 encodes codepoints using 1 up to 4 bytes
	size_t outbuf_size = input.length() * 4;

	char *inbuf = new char[inbuf_size]; // intentionally NOT null-terminated
	memcpy(inbuf, input.c_str(), inbuf_size);
	std::string out;
	out.resize(outbuf_size);

	if (!convert("UTF-8", DEFAULT_ENCODING, &out[0], &outbuf_size, inbuf, inbuf_size)) {
		irr::os::Printer::log(("Couldn't convert UTF-8 string 0x" + hex_encode(inbuf, inbuf_size) +
			" into wstring").c_str(), irr::ELL_WARNING);
		delete[] inbuf;
		return "<invalid wide string>";
	}
	delete[] inbuf;

	out.resize(outbuf_size);
	return out;
}

#else // _WIN32

std::wstring utf8_to_wide(const std::string &input)
{
	size_t outbuf_size = input.size() + 1;
	wchar_t *outbuf = new wchar_t[outbuf_size];
	memset(outbuf, 0, outbuf_size * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, input.c_str(), input.size(),
		outbuf, outbuf_size);
	std::wstring out(outbuf);
	delete[] outbuf;
	return out;
}

std::string wide_to_utf8(const std::wstring &input)
{
	size_t outbuf_size = (input.size() + 1) * 6;
	char *outbuf = new char[outbuf_size];
	memset(outbuf, 0, outbuf_size);
	WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.size(),
		outbuf, outbuf_size, NULL, NULL);
	std::string out(outbuf);
	delete[] outbuf;
	return out;
}

#endif // _WIN32

/*
	irr::core::stringc / irr::core::stringw conversion functions.
	Implemented using the std versions.
*/

namespace irr
{
namespace core
{

size_t multibyteToWString(irr::core::stringw &destination, const irr::core::stringc &source)
{
	destination = utf8_to_wide(source.c_str()).c_str();
	return destination.size();
}

size_t multibyteToWString(irr::core::stringw &destination, const char *source)
{
	destination = utf8_to_wide(source).c_str();
	return destination.size();
}

size_t wStringToMultibyte(irr::core::stringc &destination, const irr::core::stringw &source)
{
	destination = wide_to_utf8(source.c_str()).c_str();
	return destination.size();
}

size_t wStringToMultibyte(irr::core::stringc &destination, const wchar_t *source)
{
	destination = wide_to_utf8(source).c_str();
	return destination.size();
}

} // namespace core
} // namespace irr

/*
	Another two interesting string conversion functions.
	std::string / irr::core::stringw this time.
*/

//! Create a UTF8 std::string from an irr::core::stringw.
std::string stringw_to_utf8(const irr::core::stringw &input)
{
	std::wstring str(input.c_str());
	return wide_to_utf8(str);
}

//! Create an irr::core:stringw from a UTF8 std::string.
irr::core::stringw utf8_to_stringw(const std::string &input)
{
	std::wstring str = utf8_to_wide(input);
	return irr::core::stringw(str.c_str());
}
