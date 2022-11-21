// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COSOperator.h"

#ifdef _IRR_WINDOWS_API_
#include <windows.h>
#else
#include <string.h>
#include <unistd.h>
#ifndef _IRR_ANDROID_PLATFORM_
#include <sys/types.h>
#ifdef _IRR_OSX_PLATFORM_
#include <sys/sysctl.h>
#endif
#endif
#endif

#if defined(_IRR_COMPILE_WITH_X11_DEVICE_)
#include "CIrrDeviceLinux.h"
#endif
#if defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
#import <Cocoa/Cocoa.h>
#endif

#include "fast_atof.h"

namespace irr
{

#if defined(_IRR_COMPILE_WITH_X11_DEVICE_)
// constructor  linux
	COSOperator::COSOperator(const core::stringc& osVersion, CIrrDeviceLinux* device)
: OperatingSystem(osVersion), IrrDeviceLinux(device)
{
}
#endif

// constructor
COSOperator::COSOperator(const core::stringc& osVersion) : OperatingSystem(osVersion)
{
	#ifdef _DEBUG
	setDebugName("COSOperator");
	#endif
}


//! returns the current operating system version as string.
const core::stringc& COSOperator::getOperatingSystemVersion() const
{
	return OperatingSystem;
}


//! copies text to the clipboard
//! \param text: text in utf-8
void COSOperator::copyToClipboard(const c8 *text) const
{
	if (strlen(text)==0)
		return;

// Windows version
#if defined(_IRR_WINDOWS_API_)
	if (!OpenClipboard(NULL) || text == 0)
		return;

	EmptyClipboard();

	core::stringw tempbuffer;
	core::multibyteToWString(tempbuffer, text);
	const u32 size = (tempbuffer.size() + 1) * sizeof(wchar_t);

	HGLOBAL clipbuffer;
	void * buffer;

	clipbuffer = GlobalAlloc(GMEM_MOVEABLE, size);
	buffer = GlobalLock(clipbuffer);

	memcpy(buffer, tempbuffer.c_str(), size);

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_UNICODETEXT, clipbuffer);
	CloseClipboard();

#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
    NSString *str = nil;
    NSPasteboard *board = nil;

    if ((text != NULL) && (strlen(text) > 0))
    {
        str = [NSString stringWithCString:text encoding:NSUTF8StringEncoding];
        board = [NSPasteboard generalPasteboard];
        [board declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:NSApp];
        [board setString:str forType:NSStringPboardType];
    }

#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
    if ( IrrDeviceLinux )
        IrrDeviceLinux->copyToClipboard(text);
#endif
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there, otherwise an utf-8 string.
const c8* COSOperator::getTextFromClipboard() const
{
#if defined(_IRR_WINDOWS_API_)
	if (!OpenClipboard(NULL))
		return 0;

	wchar_t * buffer = 0;

	HANDLE hData = GetClipboardData( CF_UNICODETEXT );
	buffer = (wchar_t*) GlobalLock( hData );

	core::wStringToMultibyte(ClipboardBuf, buffer);

	GlobalUnlock( hData );
	CloseClipboard();

	return ClipboardBuf.c_str();

#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
    NSString* str = nil;
    NSPasteboard* board = nil;
    char* result = 0;

    board = [NSPasteboard generalPasteboard];
    str = [board stringForType:NSStringPboardType];

    if (str != nil)
        result = (char*)[str cStringUsingEncoding:NSUTF8StringEncoding];

    return (result);

#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
    if ( IrrDeviceLinux )
        return IrrDeviceLinux->getTextFromClipboard();
    return 0;

#else

	return 0;
#endif
}


bool COSOperator::getSystemMemory(u32* Total, u32* Avail) const
{
#if defined(_IRR_WINDOWS_API_)

	MEMORYSTATUSEX MemoryStatusEx;
 	MemoryStatusEx.dwLength = sizeof(MEMORYSTATUSEX);

	// cannot fail
	GlobalMemoryStatusEx(&MemoryStatusEx);

	if (Total)
		*Total = (u32)(MemoryStatusEx.ullTotalPhys>>10);
	if (Avail)
		*Avail = (u32)(MemoryStatusEx.ullAvailPhys>>10);
	return true;

#elif defined(_IRR_POSIX_API_) && defined(_SC_PHYS_PAGES) && defined(_SC_AVPHYS_PAGES)
        long ps = sysconf(_SC_PAGESIZE);
        long pp = sysconf(_SC_PHYS_PAGES);
        long ap = sysconf(_SC_AVPHYS_PAGES);

	if (ps == -1 || (Total && pp == -1) || (Avail && ap == -1))
		return false;

	if (Total)
		*Total = (u32)((pp>>10)*ps);
	if (Avail)
		*Avail = (u32)((ap>>10)*ps);
	return true;
#elif defined(_IRR_OSX_PLATFORM_)
	int mib[2];
	int64_t physical_memory;
	size_t length;

	// Get the Physical memory size
	mib[0] = CTL_HW;
	mib[1] = HW_MEMSIZE;
	length = sizeof(int64_t);
	sysctl(mib, 2, &physical_memory, &length, NULL, 0);

	if (Total)
		*Total = (u32)(physical_memory>>10);
	if (Avail)
		*Avail = (u32)(physical_memory>>10); // we don't know better
	return true;
#else
	// TODO: implement for others
	return false;
#endif
}


} // end namespace

