// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_ATTRIBUTES_H_INCLUDED__
#define __C_ATTRIBUTES_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "IAttributes.h"
#include "IAttribute.h"

namespace irr
{
namespace video
{
	class ITexture;
	class IVideoDriver;
}
namespace io
{


//! Implementation of the IAttributes interface
class CAttributes : public IAttributes
{
public:

	CAttributes(video::IVideoDriver* driver=0);
	~CAttributes();

	//! Returns amount of attributes in this collection of attributes.
	virtual u32 getAttributeCount() const _IRR_OVERRIDE_;

	//! Returns attribute name by index.
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual const c8* getAttributeName(s32 index) const _IRR_OVERRIDE_;

	//! Returns the type of an attribute
	//! \param attributeName: Name for the attribute
	virtual E_ATTRIBUTE_TYPE getAttributeType(const c8* attributeName) const _IRR_OVERRIDE_;

	//! Returns attribute type by index.
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual E_ATTRIBUTE_TYPE getAttributeType(s32 index) const _IRR_OVERRIDE_;

	//! Returns the type string of the attribute
	//! \param attributeName: String for the attribute type
	//! \param defaultNotFound Value returned when attributeName was not found
	virtual const wchar_t* getAttributeTypeString(const c8* attributeName, const wchar_t* defaultNotFound = L"unknown") const _IRR_OVERRIDE_;

	//! Returns the type string of the attribute by index.
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual const wchar_t* getAttributeTypeString(s32 index, const wchar_t* defaultNotFound = L"unknown") const _IRR_OVERRIDE_;

	//! Returns if an attribute with a name exists
	virtual bool existsAttribute(const c8* attributeName) const _IRR_OVERRIDE_;

	//! Returns attribute index from name, -1 if not found
	virtual s32 findAttribute(const c8* attributeName) const _IRR_OVERRIDE_;

	//! Removes all attributes
	virtual void clear() _IRR_OVERRIDE_;


	/*

		Integer Attribute

	*/

	//! Adds an attribute as integer
	virtual void addInt(const c8* attributeName, s32 value) _IRR_OVERRIDE_;

	//! Sets an attribute as integer value
	virtual void setAttribute(const c8* attributeName, s32 value) _IRR_OVERRIDE_;

	//! Gets an attribute as integer value
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual s32 getAttributeAsInt(const c8* attributeName, irr::s32 defaultNotFound=0) const _IRR_OVERRIDE_;

	//! Gets an attribute as integer value
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual s32 getAttributeAsInt(s32 index) const _IRR_OVERRIDE_;

	//! Sets an attribute as integer value
	virtual void setAttribute(s32 index, s32 value) _IRR_OVERRIDE_;

	/*

		Float Attribute

	*/

	//! Adds an attribute as float
	virtual void addFloat(const c8* attributeName, f32 value) _IRR_OVERRIDE_;

	//! Sets a attribute as float value
	virtual void setAttribute(const c8* attributeName, f32 value) _IRR_OVERRIDE_;

	//! Gets an attribute as float value
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual f32 getAttributeAsFloat(const c8* attributeName, irr::f32 defaultNotFound=0.f) const _IRR_OVERRIDE_;

	//! Gets an attribute as float value
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual f32 getAttributeAsFloat(s32 index) const _IRR_OVERRIDE_;

	//! Sets an attribute as float value
	virtual void setAttribute(s32 index, f32 value) _IRR_OVERRIDE_;


	/*
		Bool Attribute
	*/

	//! Adds an attribute as bool
	virtual void addBool(const c8* attributeName, bool value) _IRR_OVERRIDE_;

	//! Sets an attribute as boolean value
	virtual void setAttribute(const c8* attributeName, bool value) _IRR_OVERRIDE_;

	//! Gets an attribute as boolean value
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual bool getAttributeAsBool(const c8* attributeName, bool defaultNotFound=false) const _IRR_OVERRIDE_;

	//! Gets an attribute as boolean value
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual bool getAttributeAsBool(s32 index) const _IRR_OVERRIDE_;

	//! Sets an attribute as boolean value
	virtual void setAttribute(s32 index, bool value) _IRR_OVERRIDE_;

protected:

	core::array<IAttribute*> Attributes;

	IAttribute* getAttributeP(const c8* attributeName) const;

	video::IVideoDriver* Driver;
};

} // end namespace io
} // end namespace irr

#endif

