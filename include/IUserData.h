// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_I_USER_DATA_H_INCLUDED
#define IRR_I_USER_DATA_H_INCLUDED

#include "irrTypes.h"

namespace irr
{
namespace io
{
	class IAttributes;
	struct SAttributeReadWriteOptions;
	
	//! Irrlicht may allow users to set their own data via those pointers
	//! Irrlicht has no memory control over IUserData, the user is completely responsible for that
	class IUserData
	{
		public:
			//! To identify the class type.
			//! You can for example use MAKE_IRR_ID to create four CC codes
			virtual irr::u32 getType() const { return 0; }	
			
			//! To be overloaded if comparisons matter
			//! You can then cast other to your derived class
			virtual bool compare(const IUserData& other) const
			{
				return getType() == other.getType();
			}
			
			//! Used internally by Irrlicht to check if data has changed
			bool operator!=(const IUserData& other) const
			{
				return !compare(other);
			}
			
		protected:
			// Irrlicht is never allowed to delete this
			// If users want to delete such objects they should go over derived classes
			~IUserData() {}
	};
	
} // end namespace io
} // end namespace irr

#endif // IRR_I_USER_DATA_H_INCLUDED
