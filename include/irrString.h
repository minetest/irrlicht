// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine" and the "irrXML" project.
// For conditions of distribution and use, see copyright notice in irrlicht.h and irrXML.h

#ifndef __IRR_STRING_H_INCLUDED__
#define __IRR_STRING_H_INCLUDED__

#include "irrTypes.h"
#include "irrAllocator.h"
#include "irrMath.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

namespace irr
{
namespace core
{

//! Very simple string class with some useful features.
/** string<c8> and string<wchar_t> both accept Unicode AND ASCII/Latin-1,
so you can assign Unicode to string<c8> and ASCII/Latin-1 to string<wchar_t>
(and the other way round) if you want to.

However, note that the conversation between both is not done using any encoding.
This means that c8 strings are treated as ASCII/Latin-1, not UTF-8, and
are simply expanded to the equivalent wchar_t, while Unicode/wchar_t
characters are truncated to 8-bit ASCII/Latin-1 characters, discarding all
other information in the wchar_t.

Helper functions for converting between UTF-8 and wchar_t are provided
outside the string class for explicit use.
*/

// forward declarations
template <typename T, typename TAlloc = irrAllocator<T> >
class string;
static size_t multibyteToWString(string<wchar_t>& destination, const char* source, u32 sourceSize);
static size_t wStringToMultibyte(string<c8>& destination, const wchar_t* source, u32 sourceSize);
inline bool isdigit(s32 c);

//! Returns a character converted to lower case
static inline u32 locale_lower ( u32 x )
{
	// ansi
	return x >= 'A' && x <= 'Z' ? x + 0x20 : x;
}

//! Returns a character converted to upper case
static inline u32 locale_upper ( u32 x )
{
	// ansi
	return x >= 'a' && x <= 'z' ? x + ( 'A' - 'a' ) : x;
}


template <typename T, typename TAlloc>
class string
{
public:

	typedef T char_type;

	//! Default constructor
	string()
	: array(0), allocated(1), used(1)
	{
		array = allocator.allocate(1); // new T[1];
		array[0] = 0;
	}


	//! Constructor
	string(const string<T,TAlloc>& other)
	: array(0), allocated(0), used(0)
	{
		*this = other;
	}

	//! Constructor from other string types
	template <class B, class A>
	string(const string<B, A>& other)
	: array(0), allocated(0), used(0)
	{
		*this = other;
	}


	//! Constructs a string from a float
	explicit string(const double number)
	: array(0), allocated(0), used(0)
	{
		c8 tmpbuf[255];
		snprintf_irr(tmpbuf, 255, "%0.6f", number);
		*this = tmpbuf;
	}


	//! Constructs a string from an int
	explicit string(int number)
	: array(0), allocated(0), used(0)
	{
		// store if negative and make positive

		bool negative = false;
		if (number < 0)
		{
			number *= -1;
			negative = true;
		}

		// temporary buffer for 16 numbers

		c8 tmpbuf[16]={0};
		u32 idx = 15;

		// special case '0'

		if (!number)
		{
			tmpbuf[14] = '0';
			*this = &tmpbuf[14];
			return;
		}

		// add numbers

		while(number && idx)
		{
			--idx;
			tmpbuf[idx] = (c8)('0' + (number % 10));
			number /= 10;
		}

		// add sign

		if (negative)
		{
			--idx;
			tmpbuf[idx] = '-';
		}

		*this = &tmpbuf[idx];
	}


	//! Constructs a string from an unsigned int
	explicit string(unsigned int number)
	: array(0), allocated(0), used(0)
	{
		// temporary buffer for 16 numbers

		c8 tmpbuf[16]={0};
		u32 idx = 15;

		// special case '0'

		if (!number)
		{
			tmpbuf[14] = '0';
			*this = &tmpbuf[14];
			return;
		}

		// add numbers

		while(number && idx)
		{
			--idx;
			tmpbuf[idx] = (c8)('0' + (number % 10));
			number /= 10;
		}

		*this = &tmpbuf[idx];
	}


	//! Constructs a string from a long
	explicit string(long number)
	: array(0), allocated(0), used(0)
	{
		// store if negative and make positive

		bool negative = false;
		if (number < 0)
		{
			number *= -1;
			negative = true;
		}

		// temporary buffer for 16 numbers

		c8 tmpbuf[16]={0};
		u32 idx = 15;

		// special case '0'

		if (!number)
		{
			tmpbuf[14] = '0';
			*this = &tmpbuf[14];
			return;
		}

		// add numbers

		while(number && idx)
		{
			--idx;
			tmpbuf[idx] = (c8)('0' + (number % 10));
			number /= 10;
		}

		// add sign

		if (negative)
		{
			--idx;
			tmpbuf[idx] = '-';
		}

		*this = &tmpbuf[idx];
	}


	//! Constructs a string from an unsigned long
	explicit string(unsigned long number)
	: array(0), allocated(0), used(0)
	{
		// temporary buffer for 16 numbers

		c8 tmpbuf[16]={0};
		u32 idx = 15;

		// special case '0'

		if (!number)
		{
			tmpbuf[14] = '0';
			*this = &tmpbuf[14];
			return;
		}

		// add numbers

		while(number && idx)
		{
			--idx;
			tmpbuf[idx] = (c8)('0' + (number % 10));
			number /= 10;
		}

		*this = &tmpbuf[idx];
	}


	//! Constructor for copying a string from a pointer with a given length
	template <class B>
	string(const B* const c, u32 length)
	: array(0), allocated(0), used(0)
	{
		if (!c)
		{
			// correctly init the string to an empty one
			*this="";
			return;
		}

		allocated = used = length+1;
		array = allocator.allocate(used); // new T[used];

		for (u32 l = 0; l<length; ++l)
			array[l] = (T)c[l];

		array[length] = 0;
	}


	//! Constructor for Unicode and ASCII strings
	template <class B>
	string(const B* const c)
	: array(0), allocated(0), used(0)
	{
		*this = c;
	}


	//! Destructor
	~string()
	{
		allocator.deallocate(array); // delete [] array;
	}


	//! Assignment operator
	string<T,TAlloc>& operator=(const string<T,TAlloc>& other)
	{
		if (this == &other)
			return *this;

		used = other.size()+1;
		if (used>allocated)
		{
			allocator.deallocate(array); // delete [] array;
			allocated = used;
			array = allocator.allocate(used); //new T[used];
		}

		const T* p = other.c_str();
		for (u32 i=0; i<used; ++i, ++p)
			array[i] = *p;

		return *this;
	}

	//! Assignment operator for other string types
	template <class B, class A>
	string<T,TAlloc>& operator=(const string<B,A>& other)
	{
		*this = other.c_str();
		return *this;
	}


	//! Assignment operator for strings, ASCII and Unicode
	template <class B>
	string<T,TAlloc>& operator=(const B* const c)
	{
		if (!c)
		{
			if (!array)
			{
				array = allocator.allocate(1); //new T[1];
				allocated = 1;
			}
			used = 1;
			array[0] = 0x0;
			return *this;
		}

		if ((void*)c == (void*)array)
			return *this;

		u32 len = 0;
		const B* p = c;
		do
		{
			++len;
		} while(*p++);

		// we'll keep the old string for a while, because the new
		// string could be a part of the current string.
		T* oldArray = array;

		used = len;
		if (used>allocated)
		{
			allocated = used;
			array = allocator.allocate(used); //new T[used];
		}

		for (u32 l = 0; l<len; ++l)
			array[l] = (T)c[l];

		if (oldArray != array)
			allocator.deallocate(oldArray); // delete [] oldArray;

		return *this;
	}


	//! Append operator for other strings
	string<T,TAlloc> operator+(const string<T,TAlloc>& other) const
	{
		string<T,TAlloc> str(*this);
		str.append(other);

		return str;
	}


	//! Append operator for strings, ASCII and Unicode
	template <class B>
	string<T,TAlloc> operator+(const B* const c) const
	{
		string<T,TAlloc> str(*this);
		str.append(c);

		return str;
	}


	//! Direct access operator
	T& operator [](const u32 index)
	{
		_IRR_DEBUG_BREAK_IF(index>=used) // bad index
		return array[index];
	}


	//! Direct access operator
	const T& operator [](const u32 index) const
	{
		_IRR_DEBUG_BREAK_IF(index>=used) // bad index
		return array[index];
	}


	//! Equality operator
	bool operator==(const T* const str) const
	{
		if (!str)
			return false;

		u32 i;
		for (i=0; array[i] && str[i]; ++i)
			if (array[i] != str[i])
				return false;

		return (!array[i] && !str[i]);
	}


	//! Equality operator
	bool operator==(const string<T,TAlloc>& other) const
	{
		for (u32 i=0; array[i] && other.array[i]; ++i)
			if (array[i] != other.array[i])
				return false;

		return used == other.used;
	}


	//! Is smaller comparator
	bool operator<(const string<T,TAlloc>& other) const
	{
		for (u32 i=0; array[i] && other.array[i]; ++i)
		{
			const s32 diff = array[i] - other.array[i];
			if (diff)
				return (diff < 0);
		}

		return (used < other.used);
	}


	//! Inequality operator
	bool operator!=(const T* const str) const
	{
		return !(*this == str);
	}


	//! Inequality operator
	bool operator!=(const string<T,TAlloc>& other) const
	{
		return !(*this == other);
	}


	//! Returns length of the string's content
	/** \return Length of the string's content in characters, excluding
	the trailing NUL. */
	u32 size() const
	{
		return used-1;
	}

	//! Informs if the string is empty or not.
	//! \return True if the string is empty, false if not.
	bool empty() const
	{
		return (size() == 0);
	}

	void clear(bool releaseMemory=true)
	{
		if ( releaseMemory )
		{
			reallocate(1);
		}
		array[0] = 0;
		used = 1;
	}

	//! Returns character string
	/** \return pointer to C-style NUL terminated string. */
	const T* c_str() const
	{
		return array;
	}


	//! Makes the string lower case.
	string<T,TAlloc>& make_lower()
	{
		for (u32 i=0; array[i]; ++i)
			array[i] = locale_lower ( array[i] );
		return *this;
	}


	//! Makes the string upper case.
	string<T,TAlloc>& make_upper()
	{
		for (u32 i=0; array[i]; ++i)
			array[i] = locale_upper ( array[i] );
		return *this;
	}


	//! Compares the strings ignoring case.
	/** \param other: Other string to compare.
	\return True if the strings are equal ignoring case. */
	bool equals_ignore_case(const string<T,TAlloc>& other) const
	{
		for(u32 i=0; array[i] && other[i]; ++i)
			if (locale_lower( array[i]) != locale_lower(other[i]))
				return false;

		return used == other.used;
	}

	//! Compares the strings ignoring case.
	/** \param other: Other string to compare.
		\param sourcePos: where to start to compare in the string
	\return True if the strings are equal ignoring case. */
	bool equals_substring_ignore_case(const string<T,TAlloc>&other, const s32 sourcePos = 0 ) const
	{
		if ( (u32) sourcePos >= used )
			return false;

		u32 i;
		for( i=0; array[sourcePos + i] && other[i]; ++i)
			if (locale_lower( array[sourcePos + i]) != locale_lower(other[i]))
				return false;

		return array[sourcePos + i] == 0 && other[i] == 0;
	}


	//! Compares the strings ignoring case.
	/** \param other: Other string to compare.
	\return True if this string is smaller ignoring case. */
	bool lower_ignore_case(const string<T,TAlloc>& other) const
	{
		for(u32 i=0; array[i] && other.array[i]; ++i)
		{
			s32 diff = (s32) locale_lower ( array[i] ) - (s32) locale_lower ( other.array[i] );
			if ( diff )
				return diff < 0;
		}

		return used < other.used;
	}


	//! compares the first n characters of the strings
	/** \param other Other string to compare.
	\param n Number of characters to compare
	\return True if the n first characters of both strings are equal. */
	bool equalsn(const string<T,TAlloc>& other, u32 n) const
	{
		u32 i;
		for(i=0; i < n && array[i] && other[i]; ++i)
			if (array[i] != other[i])
				return false;

		// if one (or both) of the strings was smaller then they
		// are only equal if they have the same length
		return (i == n) || (used == other.used);
	}


	//! compares the first n characters of the strings
	/** \param str Other string to compare.
	\param n Number of characters to compare
	\return True if the n first characters of both strings are equal. */
	bool equalsn(const T* const str, u32 n) const
	{
		if (!str)
			return false;
		u32 i;
		for(i=0; i < n && array[i] && str[i]; ++i)
			if (array[i] != str[i])
				return false;

		// if one (or both) of the strings was smaller then they
		// are only equal if they have the same length
		return (i == n) || (array[i] == 0 && str[i] == 0);
	}


	//! Appends a character to this string
	/** \param character: Character to append. */
	string<T,TAlloc>& append(T character)
	{
		if (used + 1 > allocated)
			reallocate(used + 1);

		++used;

		array[used-2] = character;
		array[used-1] = 0;

		return *this;
	}


	//! Appends a char string to this string
	/** \param other: Char string to append. */
	/** \param length: The length of the string to append. */
	string<T,TAlloc>& append(const T* const other, u32 length=0xffffffff)
	{
		if (!other)
			return *this;

		u32 len = 0;
		const T* p = other;
		while(*p)
		{
			++len;
			++p;
		}
		if (len > length)
			len = length;

		if (used + len > allocated)
			reallocate(used + len);

		--used;
		++len;

		for (u32 l=0; l<len; ++l)
			array[l+used] = *(other+l);

		used += len;

		return *this;
	}


	//! Appends a string to this string
	/** \param other: String to append. */
	string<T,TAlloc>& append(const string<T,TAlloc>& other)
	{
		if (other.size() == 0)
			return *this;

		--used;
		const u32 len = other.size()+1;

		if (used + len > allocated)
			reallocate(used + len);

		for (u32 l=0; l<len; ++l)
			array[used+l] = other[l];

		used += len;

		return *this;
	}


	//! Appends a string of the length l to this string.
	/** \param other: other String to append to this string.
	\param length: How much characters of the other string to add to this one. */
	string<T,TAlloc>& append(const string<T,TAlloc>& other, u32 length)
	{
		if (other.size() == 0)
			return *this;

		if (other.size() < length)
		{
			append(other);
			return *this;
		}

		if (used + length > allocated)
			reallocate(used + length);

		--used;

		for (u32 l=0; l<length; ++l)
			array[l+used] = other[l];
		used += length;

		// ensure proper termination
		array[used]=0;
		++used;

		return *this;
	}

	//! Insert a certain amount of characters into the string before the given index
	//\param pos Insert the characters before this index
	//\param s String to insert. Must be at least of size n
	//\param n Number of characters from string s to use.
	string<T,TAlloc>& insert(u32 pos, const char* s, u32 n)
	{
		if ( pos < used )
		{
			reserve(used+n);

			// move stuff behind insert point
			const u32 end = used+n-1;
			for (u32 i=0; i<used-pos; ++i)
			{
				array[end-i] = array[end-(i+n)];
			}
			used += n;

			for (u32 i=0; i<n; ++i)
			{
				array[pos+i] = s[i];
			}
		}

		return *this;
	}

	//! Reserves some memory.
	/** \param count: Amount of characters to reserve. */
	void reserve(u32 count)
	{
		if (count < allocated)
			return;

		reallocate(count);
	}


	//! finds first occurrence of character in string
	/** \param c: Character to search for.
	\return Position where the character has been found,
	or -1 if not found. */
	s32 findFirst(T c) const
	{
		for (u32 i=0; i<used-1; ++i)
			if (array[i] == c)
				return i;

		return -1;
	}

	//! finds first occurrence of a character of a list in string
	/** \param c: List of characters to find. For example if the method
	should find the first occurrence of 'a' or 'b', this parameter should be "ab".
	\param count: Amount of characters in the list. Usually,
	this should be strlen(c)
	\return Position where one of the characters has been found,
	or -1 if not found. */
	s32 findFirstChar(const T* const c, u32 count=1) const
	{
		if (!c || !count)
			return -1;

		for (u32 i=0; i<used-1; ++i)
			for (u32 j=0; j<count; ++j)
				if (array[i] == c[j])
					return i;

		return -1;
	}


	//! Finds first position of a character not in a given list.
	/** \param c: List of characters not to find. For example if the method
	should find the first occurrence of a character not 'a' or 'b', this parameter should be "ab".
	\param count: Amount of characters in the list. Usually,
	this should be strlen(c)
	\return Position where the character has been found,
	or -1 if not found. */
	template <class B>
	s32 findFirstCharNotInList(const B* const c, u32 count=1) const
	{
		if (!c || !count)
			return -1;

		for (u32 i=0; i<used-1; ++i)
		{
			u32 j;
			for (j=0; j<count; ++j)
				if (array[i] == c[j])
					break;

			if (j==count)
				return i;
		}

		return -1;
	}

	//! Finds last position of a character not in a given list.
	/** \param c: List of characters not to find. For example if the method
	should find the first occurrence of a character not 'a' or 'b', this parameter should be "ab".
	\param count: Amount of characters in the list. Usually,
	this should be strlen(c)
	\return Position where the character has been found,
	or -1 if not found. */
	template <class B>
	s32 findLastCharNotInList(const B* const c, u32 count=1) const
	{
		if (!c || !count)
			return -1;

		for (s32 i=(s32)(used-2); i>=0; --i)
		{
			u32 j;
			for (j=0; j<count; ++j)
				if (array[i] == c[j])
					break;

			if (j==count)
				return i;
		}

		return -1;
	}

	//! finds next occurrence of character in string
	/** \param c: Character to search for.
	\param startPos: Position in string to start searching.
	\return Position where the character has been found,
	or -1 if not found. */
	s32 findNext(T c, u32 startPos) const
	{
		for (u32 i=startPos; i<used-1; ++i)
			if (array[i] == c)
				return i;

		return -1;
	}


	//! finds last occurrence of character in string
	/** \param c: Character to search for.
	\param start: start to search reverse ( default = -1, on end )
	\return Position where the character has been found,
	or -1 if not found. */
	s32 findLast(T c, s32 start = -1) const
	{
		start = core::clamp ( start < 0 ? (s32)(used) - 2 : start, 0, (s32)(used) - 2 );
		for (s32 i=start; i>=0; --i)
			if (array[i] == c)
				return i;

		return -1;
	}

	//! finds last occurrence of a character of a list in string
	/** \param c: List of strings to find. For example if the method
	should find the last occurrence of 'a' or 'b', this parameter should be "ab".
	\param count: Amount of characters in the list. Usually,
	this should be strlen(c)
	\return Position where one of the characters has been found,
	or -1 if not found. */
	s32 findLastChar(const T* const c, u32 count=1) const
	{
		if (!c || !count)
			return -1;

		for (s32 i=(s32)used-2; i>=0; --i)
			for (u32 j=0; j<count; ++j)
				if (array[i] == c[j])
					return i;

		return -1;
	}


	//! finds another string in this string
	/** \param str: Another string
	\param start: Start position of the search
	\return Positions where the string has been found,
	or -1 if not found. */
	template <class B>
	s32 find(const B* const str, const u32 start = 0) const
	{
		if (str && *str)
		{
			u32 len = 0;

			while (str[len])
				++len;

			if (len > used-1)
				return -1;

			for (u32 i=start; i<used-len; ++i)
			{
				u32 j=0;

				while(str[j] && array[i+j] == str[j])
					++j;

				if (!str[j])
					return i;
			}
		}

		return -1;
	}


	//! Returns a substring
	/** \param begin Start of substring.
	\param length Length of substring.
	\param make_lower copy only lower case */
	string<T> subString(u32 begin, s32 length, bool make_lower = false ) const
	{
		// if start after string
		// or no proper substring length
		if ((length <= 0) || (begin>=size()))
			return string<T>("");
		// clamp length to maximal value
		if ((length+begin) > size())
			length = size()-begin;

		// accounting for null terminator.
		s32 substrAllocLength = length + 1;
		string<T> o;
		o.reserve(substrAllocLength);

		if ( !make_lower )
		{
			for (s32 i=0; i<length; ++i)
				o.array[i] = array[i+begin];
		}
		else
		{
			for (s32 i=0; i<length; ++i)
				o.array[i] = locale_lower ( array[i+begin] );
		}

		o.array[substrAllocLength - 1] = 0;
		o.used = length + 1;

		return o;
	}


	//! Appends a character to this string
	/** \param c Character to append. */
	string<T,TAlloc>& operator += (T c)
	{
		append(c);
		return *this;
	}


	//! Appends a char string to this string
	/** \param c Char string to append. */
	string<T,TAlloc>& operator += (const T* const c)
	{
		append(c);
		return *this;
	}


	//! Appends a string to this string
	/** \param other String to append. */
	string<T,TAlloc>& operator += (const string<T,TAlloc>& other)
	{
		append(other);
		return *this;
	}


	//! Appends a string representation of a number to this string
	/** \param i Number to append. */
	string<T,TAlloc>& operator += (const int i)
	{
		append(string<T,TAlloc>(i));
		return *this;
	}


	//! Appends a string representation of a number to this string
	/** \param i Number to append. */
	string<T,TAlloc>& operator += (const unsigned int i)
	{
		append(string<T,TAlloc>(i));
		return *this;
	}


	//! Appends a string representation of a number to this string
	/** \param i Number to append. */
	string<T,TAlloc>& operator += (const long i)
	{
		append(string<T,TAlloc>(i));
		return *this;
	}


	//! Appends a string representation of a number to this string
	/** \param i Number to append. */
	string<T,TAlloc>& operator += (const unsigned long i)
	{
		append(string<T,TAlloc>(i));
		return *this;
	}


	//! Appends a string representation of a number to this string
	/** \param i Number to append. */
	string<T,TAlloc>& operator += (const double i)
	{
		append(string<T,TAlloc>(i));
		return *this;
	}


	//! Appends a string representation of a number to this string
	/** \param i Number to append. */
	string<T,TAlloc>& operator += (const float i)
	{
		append(string<T,TAlloc>(i));
		return *this;
	}


	//! Replaces all characters of a special type with another one
	/** \param toReplace Character to replace.
	\param replaceWith Character replacing the old one. */
	string<T,TAlloc>& replace(T toReplace, T replaceWith)
	{
		for (u32 i=0; i<used-1; ++i)
			if (array[i] == toReplace)
				array[i] = replaceWith;
		return *this;
	}


	//! Replaces all instances of a string with another one.
	/** \param toReplace The string to replace.
	\param replaceWith The string replacing the old one. */
	string<T,TAlloc>& replace(const string<T,TAlloc>& toReplace, const string<T,TAlloc>& replaceWith)
	{
		if (toReplace.size() == 0)
			return *this;

		const T* other = toReplace.c_str();
		const T* replace = replaceWith.c_str();
		const u32 other_size = toReplace.size();
		const u32 replace_size = replaceWith.size();

		// Determine the delta.  The algorithm will change depending on the delta.
		s32 delta = replace_size - other_size;

		// A character for character replace.  The string will not shrink or grow.
		if (delta == 0)
		{
			s32 pos = 0;
			while ((pos = find(other, pos)) != -1)
			{
				for (u32 i = 0; i < replace_size; ++i)
					array[pos + i] = replace[i];
				++pos;
			}
			return *this;
		}

		// We are going to be removing some characters.  The string will shrink.
		if (delta < 0)
		{
			u32 i = 0;
			for (u32 pos = 0; pos < used; ++i, ++pos)
			{
				// Is this potentially a match?
				if (array[pos] == *other)
				{
					// Check to see if we have a match.
					u32 j;
					for (j = 0; j < other_size; ++j)
					{
						if (array[pos + j] != other[j])
							break;
					}

					// If we have a match, replace characters.
					if (j == other_size)
					{
						for (j = 0; j < replace_size; ++j)
							array[i + j] = replace[j];
						i += replace_size - 1;
						pos += other_size - 1;
						continue;
					}
				}

				// No match found, just copy characters.
				array[i] = array[pos];
			}
			array[i-1] = 0;
			used = i;

			return *this;
		}

		// We are going to be adding characters, so the string size will increase.
		// Count the number of times toReplace exists in the string so we can allocate the new size.
		u32 find_count = 0;
		s32 pos = 0;
		while ((pos = find(other, pos)) != -1)
		{
			++find_count;
			++pos;
		}

		// Re-allocate the string now, if needed.
		u32 len = delta * find_count;
		if (used + len > allocated)
			reallocate(used + len);

		// Start replacing.
		pos = 0;
		while ((pos = find(other, pos)) != -1)
		{
			T* start = array + pos + other_size - 1;
			T* ptr   = array + used - 1;
			T* end   = array + delta + used -1;

			// Shift characters to make room for the string.
			while (ptr != start)
			{
				*end = *ptr;
				--ptr;
				--end;
			}

			// Add the new string now.
			for (u32 i = 0; i < replace_size; ++i)
				array[pos + i] = replace[i];

			pos += replace_size;
			used += delta;
		}

		return *this;
	}


	//! Removes characters from a string.
	/** \param c: Character to remove. */
	string<T,TAlloc>& remove(T c)
	{
		u32 pos = 0;
		u32 found = 0;
		for (u32 i=0; i<used-1; ++i)
		{
			if (array[i] == c)
			{
				++found;
				continue;
			}

			array[pos++] = array[i];
		}
		used -= found;
		array[used-1] = 0;
		return *this;
	}


	//! Removes a string from the string.
	/** \param toRemove: String to remove. */
	string<T,TAlloc>& remove(const string<T,TAlloc>& toRemove)
	{
		u32 size = toRemove.size();
		if ( size == 0 )
			return *this;
		u32 pos = 0;
		u32 found = 0;
		for (u32 i=0; i<used-1; ++i)
		{
			u32 j = 0;
			while (j < size)
			{
				if (array[i + j] != toRemove[j])
					break;
				++j;
			}
			if (j == size)
			{
				found += size;
				i += size - 1;
				continue;
			}

			array[pos++] = array[i];
		}
		used -= found;
		array[used-1] = 0;
		return *this;
	}


	//! Removes characters from a string.
	/** \param characters: Characters to remove. */
	string<T,TAlloc>& removeChars(const string<T,TAlloc> & characters)
	{
		if (characters.size() == 0)
			return *this;

		u32 pos = 0;
		u32 found = 0;
		for (u32 i=0; i<used-1; ++i)
		{
			// Don't use characters.findFirst as it finds the \0,
			// causing used to become incorrect.
			bool docontinue = false;
			for (u32 j=0; j<characters.size(); ++j)
			{
				if (characters[j] == array[i])
				{
					++found;
					docontinue = true;
					break;
				}
			}
			if (docontinue)
				continue;

			array[pos++] = array[i];
		}
		used -= found;
		array[used-1] = 0;

		return *this;
	}


	//! Trims the string.
	/** Removes the specified characters (by default, Latin-1 whitespace)
	from the beginning and the end of the string. */
	string<T,TAlloc>& trim(const string<T,TAlloc> & whitespace = " \t\n\r")
	{
		// find start and end of the substring without the specified characters
		const s32 begin = findFirstCharNotInList(whitespace.c_str(), whitespace.used);
		if (begin == -1)
			return (*this="");

		const s32 end = findLastCharNotInList(whitespace.c_str(), whitespace.used);

		return (*this = subString(begin, (end +1) - begin));
	}

	//! Erase 0's at the end when a string ends with a floating point number
	/** After generating strings from floats we often end up with strings
		ending up with lots of zeros which don't add any value. Erase 'em all.
		Examples: "0.100000" becomes "0.1"
	              "10.000000" becomes "10"
				  "foo 3.140000" becomes "foo 3.14"
				  "no_num.000" stays "no_num.000"
				  "1." stays "1."
	*/
	string<T,TAlloc>& eraseTrailingFloatZeros(char decimalPoint='.')
	{
		s32 i=findLastCharNotInList("0", 1);
		if ( i > 0 && (u32)i < used-2 )	// non 0 must be found and not last char (also used is at least 2 when i > 0)
		{
			u32 eraseStart=i+1;
			u32 dot=0;
			if( core::isdigit(array[i]) )
			{
				while( --i>0 && core::isdigit(array[i]) );
				if ( array[i] == decimalPoint )
					dot = i;
			}
			else if ( array[i] == decimalPoint )
			{
				dot = i;
				eraseStart = i;
			}
			if ( dot > 0 && core::isdigit(array[dot-1]) )
			{
				array[eraseStart] = 0;
				used = eraseStart+1;
			}
		}
		return *this;
	}

	//! Erases a character from the string.
	/** May be slow, because all elements
	following after the erased element have to be copied.
	\param index: Index of element to be erased. */
	string<T,TAlloc>& erase(u32 index)
	{
		_IRR_DEBUG_BREAK_IF(index>=used) // access violation

		for (u32 i=index+1; i<used; ++i)
			array[i-1] = array[i];

		--used;
		return *this;
	}

	//! verify the existing string.
	string<T,TAlloc>& validate()
	{
		// terminate on existing null
		for (u32 i=0; i<allocated; ++i)
		{
			if (array[i] == 0)
			{
				used = i + 1;
				return *this;
			}
		}

		// terminate
		if ( allocated > 0 )
		{
			used = allocated;
			array[used-1] = 0;
		}
		else
		{
			used = 0;
		}

		return *this;
	}

	//! gets the last char of a string or null
	T lastChar() const
	{
		return used > 1 ? array[used-2] : 0;
	}

	//! Split string into parts (tokens).
	/** This method will split a string at certain delimiter characters
	into the container passed in as reference. The type of the container
	has to be given as template parameter. It must provide a push_back and
	a size method.
	\param ret The result container. Tokens are added, the container is not cleared.
	\param delimiter C-style string of delimiter characters
	\param countDelimiters Number of delimiter characters
	\param ignoreEmptyTokens Flag to avoid empty substrings in the result
	container. If two delimiters occur without a character in between or an
	empty substring would be placed in the result. Or if a delimiter is the last
	character an empty substring would be added at the end.	If this flag is set,
	only non-empty strings are stored.
	\param keepSeparators Flag which allows to add the separator to the
	result string. If this flag is true, the concatenation of the
	substrings results in the original string. Otherwise, only the
	characters between the delimiters are returned.
	\return The number of resulting substrings
	*/
	template<class container>
	u32 split(container& ret, const T* const delimiter, u32 countDelimiters=1, bool ignoreEmptyTokens=true, bool keepSeparators=false) const
	{
		if (!delimiter)
			return 0;

		const u32 oldSize=ret.size();

		u32 tokenStartIdx = 0;
		for (u32 i=0; i<used; ++i)
		{
			for (u32 j=0; j<countDelimiters; ++j)
			{
				if (array[i] == delimiter[j])
				{
					if (i - tokenStartIdx > 0)
						ret.push_back(string<T,TAlloc>(&array[tokenStartIdx], i - tokenStartIdx));
					else if ( !ignoreEmptyTokens )
						ret.push_back(string<T,TAlloc>());
					if ( keepSeparators )
					{
						ret.push_back(string<T,TAlloc>(&array[i], 1));
					}

					tokenStartIdx = i+1;
					break;
				}
			}
		}
		if ((used - 1) > tokenStartIdx)
			ret.push_back(string<T,TAlloc>(&array[tokenStartIdx], (used - 1) - tokenStartIdx));
		 else if ( !ignoreEmptyTokens )
                ret.push_back(string<T,TAlloc>());

		return ret.size()-oldSize;
	}

	friend size_t multibyteToWString(string<wchar_t>& destination, const char* source, u32 sourceSize);
	friend size_t wStringToMultibyte(string<c8>& destination, const wchar_t* source, u32 sourceSize);

private:

	//! Reallocate the array, make it bigger or smaller
	void reallocate(u32 new_size)
	{
		T* old_array = array;

		array = allocator.allocate(new_size); //new T[new_size];
		allocated = new_size;

		const u32 amount = used < new_size ? used : new_size;
		for (u32 i=0; i<amount; ++i)
			array[i] = old_array[i];

		if (allocated < used)
			used = allocated;

		allocator.deallocate(old_array); // delete [] old_array;
	}

	//--- member variables

	T* array;
	u32 allocated;
	u32 used;
	TAlloc allocator;
};


//! Typedef for character strings
typedef string<c8> stringc;

//! Typedef for wide character strings
typedef string<wchar_t> stringw;

//! Convert multibyte string to wide-character string
/** Wrapper around mbstowcs from standard library, but directly using Irrlicht string class.
What the function does exactly depends on the LC_CTYPE of the current c locale.
\param destination Wide-character string receiving the converted source
\param source multibyte string
\return The number of wide characters written to destination, not including the eventual terminating null character or -1 when conversion failed */
static inline size_t multibyteToWString(string<wchar_t>& destination, const core::string<c8>& source)
{
	return multibyteToWString(destination, source.c_str(), (u32)source.size());
}

//! Convert multibyte string to wide-character string
/** Wrapper around mbstowcs from standard library, but directly writing to Irrlicht string class.
What the function does exactly depends on the LC_CTYPE of the current c locale.
\param destination Wide-character string receiving the converted source
\param source multibyte string
\return The number of wide characters written to destination, not including the eventual terminating null character  or -1 when conversion failed. */
static inline size_t multibyteToWString(string<wchar_t>& destination, const char* source)
{
	const u32 s = source ? (u32)strlen(source) : 0;
	return multibyteToWString(destination, source, s);
}

//! Internally used by the other multibyteToWString functions
static size_t multibyteToWString(string<wchar_t>& destination, const char* source, u32 sourceSize)
{
	if ( sourceSize )
	{
		destination.reserve(sourceSize+1);
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)	// 'mbstowcs': This function or variable may be unsafe. Consider using mbstowcs_s instead.
#endif
		const size_t written = mbstowcs(destination.array, source, (size_t)sourceSize);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
		if ( written != (size_t)-1 )
		{
			destination.used = (u32)written+1;
			destination.array[destination.used-1] = 0;
		}
		else
		{
			// Likely character which got converted until the invalid character was encountered are in destination now.
			// And it seems even 0-terminated, but I found no documentation anywhere that this (the 0-termination) is guaranteed :-(
			destination.clear();
		}
		return written;
	}
	else
	{
		destination.clear();
		return 0;
	}
}

//! Same as multibyteToWString, but the other way around
static inline size_t wStringToMultibyte(string<c8>& destination, const core::string<wchar_t>& source)
{
	return wStringToMultibyte(destination, source.c_str(), (u32)source.size());
}

//! Same as multibyteToWString, but the other way around
static inline size_t wStringToMultibyte(string<c8>& destination, const wchar_t* source)
{
	const u32 s = source ? (u32)wcslen(source) : 0;
	return wStringToMultibyte(destination, source, s);
}

//! Same as multibyteToWString, but the other way around
static size_t wStringToMultibyte(string<c8>& destination, const wchar_t* source, u32 sourceSize)
{
	if ( sourceSize )
	{
		destination.reserve(sourceSize+1);
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)	// 'wcstombs': This function or variable may be unsafe. Consider using wcstombs_s instead.
#endif
		const size_t written = wcstombs(destination.array, source, (size_t)sourceSize);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
		if ( written != (size_t)-1 )
		{
			destination.used = (u32)written+1;
			destination.array[destination.used-1] = 0;
		}
		else
		{
			// Likely character which got converted until the invalid character was encountered are in destination now.
			// And it seems even 0-terminated, but I found no documentation anywhere that this (the 0-termination) is guaranteed :-(
			destination.clear();
		}
		return written;
	}
	else
	{
		destination.clear();
		return 0;
	}
}


} // end namespace core
} // end namespace irr

#endif

