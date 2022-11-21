// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Written by Michael Zeilfelder

#include "CProfiler.h"
#include "CTimer.h"

namespace irr
{
IRRLICHT_API IProfiler& IRRCALLCONV getProfiler()
{
	static CProfiler profiler;
	return profiler;
}

CProfiler::CProfiler()
{
	Timer = new CTimer(true);

	addGroup(L"overview");
}

CProfiler::~CProfiler()
{
	if ( Timer )
		Timer->drop();
}

void CProfiler::printAll(core::stringw &ostream, bool includeOverview, bool suppressUncalled) const
{
    ostream += makeTitleString();
    ostream += L"\n";
	for ( u32 i=includeOverview ?0:1; i<ProfileGroups.size(); ++i )
    {
        printGroup( ostream, i, suppressUncalled );
    }
}

void CProfiler::printGroup(core::stringw &ostream, u32 idxGroup, bool suppressUncalled) const
{
    ostream += getAsString(ProfileGroups[idxGroup]);
    ostream += L"\n";

	// print overview for groups
    if ( idxGroup == 0 )
    {
		for ( u32 i=0; i<ProfileGroups.size(); ++i )
        {
			if ( !suppressUncalled || ProfileGroups[i].getCallsCounter() > 0)
            {
                ostream += getAsString(ProfileGroups[i]);
                ostream += L"\n";
            }
        }
    }
	// print all data in a group
    else
    {
		for ( u32 i=0; i<ProfileDatas.size(); ++i )
        {
			if ( (!suppressUncalled || ProfileDatas[i].getCallsCounter() > 0)
				&& ProfileDatas[i].getGroupIndex() == idxGroup )
            {
                ostream += getAsString(ProfileDatas[i]);
                ostream += L"\n";
            }
        }
    }
}

//! Convert the whole data into a string
core::stringw CProfiler::getAsString(const SProfileData& data) const
{
	if ( data.getCallsCounter() > 0 )
	{
		wchar_t dummy[512];
		swprintf_irr(dummy, 512, L"%-15.15s%-12u%-12u%-12u%-12u",
			data.getName().c_str(), data.getCallsCounter(), data.getTimeSum(),
			data.getTimeSum() / data.getCallsCounter(), data.getLongestTime());

		return core::stringw(dummy);
	}
	else
	{
		return data.getName();
	}
}

//! Return a string which describes the columns returned by getAsString
core::stringw CProfiler::makeTitleString() const
{
	return core::stringw("name           calls       time(sum)   time(avg)   time(max)");
}

} // namespace irr
