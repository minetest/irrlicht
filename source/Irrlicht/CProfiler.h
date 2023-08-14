// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Written by Michael Zeilfelder

#ifndef IRR_C_PROFILER_H_INCLUDED
#define IRR_C_PROFILER_H_INCLUDED

#include "IProfiler.h"

namespace irr
{
class CProfiler : public IProfiler
{
public:

	CProfiler();
	virtual ~CProfiler();

	//! Write all profile-data into a string
    virtual void printAll(core::stringw &result, bool includeOverview,bool suppressUncalled) const  IRR_OVERRIDE;

	//! Write the profile data of one group into a string
    virtual void printGroup(core::stringw &result, u32 groupIndex, bool suppressUncalled) const  IRR_OVERRIDE;

protected:
	core::stringw makeTitleString() const;
	core::stringw getAsString(const SProfileData& data) const;
};
} // namespace irr

#endif // IRR_C_PROFILER_H_INCLUDED
