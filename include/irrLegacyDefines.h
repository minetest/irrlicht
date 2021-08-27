// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_LEGACY_DEFINES_H_INCLUDED
#define IRR_LEGACY_DEFINES_H_INCLUDED

//! Include this file _after_ irrlicht.h
//! It contains old defines which got replaced in Irrlicht.
//! So including this header is a quick fix to allow users compiling old code 
//! without having to rewrite it all. Thought in the long run you should 
//! switch to the new defines as well.

// Defines replaced in Irrlicht 1.9 as they were reserved identifiers in c++
#define _IRR_DEPRECATED_ IRR_DEPRECATED
#define _IRR_OVERRIDE_ IRR_OVERRIDE
#define _IRR_DEBUG_BREAK_IF IRR_DEBUG_BREAK_IF
#define _IRR_TEXT IRR_TEXT

// Defines which are used in Irrlicht 1.9 and changed their name as they were reserved identifiers in c++
// Those can't be set here as it's defines _users_ can set.
// If you have any of those in your code, you will have to change them there.
// _IRR_DONT_DO_MEMORY_DEBUGGING_HERE -> IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#endif
