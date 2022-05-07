// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CAttributes.h"
#include "fast_atof.h"
#include "ITexture.h"
#include "IVideoDriver.h"

namespace irr
{
namespace io
{

/*
	Basic types, check documentation in IAttribute.h to see how they generally work.
*/

// Attribute implemented for boolean values
class CBoolAttribute : public IAttribute
{
public:

	CBoolAttribute(const char* name, bool value)
	{
		Name = name;
		setBool(value);
	}

	virtual s32 getInt() const _IRR_OVERRIDE_
	{
		return BoolValue ? 1 : 0;
	}

	virtual f32 getFloat() const _IRR_OVERRIDE_
	{
		return BoolValue ? 1.0f : 0.0f;
	}

	virtual bool getBool() const _IRR_OVERRIDE_
	{
		return BoolValue;
	}

	virtual void setInt(s32 intValue) _IRR_OVERRIDE_
	{
		BoolValue = (intValue != 0);
	}

	virtual void setFloat(f32 floatValue) _IRR_OVERRIDE_
	{
		BoolValue = (floatValue != 0);
	}

	virtual void setBool(bool boolValue) _IRR_OVERRIDE_
	{
		BoolValue = boolValue;
	}

	virtual E_ATTRIBUTE_TYPE getType() const _IRR_OVERRIDE_
	{
		return EAT_BOOL;
	}

	virtual const wchar_t* getTypeString() const _IRR_OVERRIDE_
	{
		return L"bool";
	}

	bool BoolValue;
};

// Attribute implemented for integers
class CIntAttribute : public IAttribute
{
public:

	CIntAttribute(const char* name, s32 value)
	{
		Name = name;
		setInt(value);
	}

	virtual s32 getInt() const _IRR_OVERRIDE_
	{
		return Value;
	}

	virtual f32 getFloat() const _IRR_OVERRIDE_
	{
		return (f32)Value;
	}

	virtual void setInt(s32 intValue) _IRR_OVERRIDE_
	{
		Value = intValue;
	}

	virtual void setFloat(f32 floatValue) _IRR_OVERRIDE_
	{
		Value = (s32)floatValue;
	};

	virtual E_ATTRIBUTE_TYPE getType() const _IRR_OVERRIDE_
	{
		return EAT_INT;
	}

	virtual const wchar_t* getTypeString() const _IRR_OVERRIDE_
	{
		return L"int";
	}

	s32 Value;
};

// Attribute implemented for floats
class CFloatAttribute : public IAttribute
{
public:

	CFloatAttribute(const char* name, f32 value)
	{
		Name = name;
		setFloat(value);
	}

	virtual s32 getInt() const _IRR_OVERRIDE_
	{
		return (s32)Value;
	}

	virtual f32 getFloat() const _IRR_OVERRIDE_
	{
		return Value;
	}

	virtual void setInt(s32 intValue) _IRR_OVERRIDE_
	{
		Value = (f32)intValue;
	}

	virtual void setFloat(f32 floatValue) _IRR_OVERRIDE_
	{
		Value = floatValue;
	}

	virtual E_ATTRIBUTE_TYPE getType() const _IRR_OVERRIDE_
	{
		return EAT_FLOAT;
	}

	virtual const wchar_t* getTypeString() const _IRR_OVERRIDE_
	{
		return L"float";
	}

	f32 Value;
};

} // end namespace io
} // end namespace irr
