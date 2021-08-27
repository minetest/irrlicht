#include "testUtils.h"

using namespace irr;
using namespace core;

bool testMergeFilename()
{
	// path mergeFilename(const path& path, const path& filename, const path& extension = "");
	if ( mergeFilename(IRR_TEXT(""), IRR_TEXT(""), IRR_TEXT("") ) != io::path(IRR_TEXT("")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder"), IRR_TEXT(""), IRR_TEXT("") ) != io::path(IRR_TEXT("folder/")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder/"), IRR_TEXT(""), IRR_TEXT("") ) != io::path(IRR_TEXT("folder/")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder"), IRR_TEXT("file"), IRR_TEXT("") ) != io::path(IRR_TEXT("folder/file")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder/"), IRR_TEXT("file"), IRR_TEXT("") ) != io::path(IRR_TEXT("folder/file")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder\\"), IRR_TEXT("file"), IRR_TEXT("") ) != io::path(IRR_TEXT("folder\\file")) )
		return false;
	if ( mergeFilename(IRR_TEXT(""), IRR_TEXT("file"), IRR_TEXT("") ) != io::path(IRR_TEXT("file")) )
		return false;
	if ( mergeFilename(IRR_TEXT("."), IRR_TEXT("file"), IRR_TEXT("") ) != io::path(IRR_TEXT("./file")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder"), IRR_TEXT(""), IRR_TEXT(".bmp") ) != io::path(IRR_TEXT("folder/.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder/"), IRR_TEXT(""), IRR_TEXT(".bmp") ) != io::path(IRR_TEXT("folder/.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder"), IRR_TEXT(""), IRR_TEXT("bmp") ) != io::path(IRR_TEXT("folder/.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT("."), IRR_TEXT(""), IRR_TEXT("bmp") ) != io::path(IRR_TEXT("./.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder"), IRR_TEXT("file"), IRR_TEXT("bmp") ) != io::path(IRR_TEXT("folder/file.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder/"), IRR_TEXT("file"), IRR_TEXT("bmp") ) != io::path(IRR_TEXT("folder/file.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder/"), IRR_TEXT("file"), IRR_TEXT(".bmp") ) != io::path(IRR_TEXT("folder/file.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT("folder"), IRR_TEXT("file.bmp"), IRR_TEXT("") ) != io::path(IRR_TEXT("folder/file.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT(""), IRR_TEXT("file"), IRR_TEXT("bmp") ) != io::path(IRR_TEXT("file.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT(""), IRR_TEXT("file"), IRR_TEXT(".bmp") ) != io::path(IRR_TEXT("file.bmp")) )
		return false;
	if ( mergeFilename(IRR_TEXT("c:"), IRR_TEXT("file"), IRR_TEXT(".bmp") ) != io::path(IRR_TEXT("c:/file.bmp")) ) // TODO: is this actually correct on windows?
		return false; 
	
	return true;
}

// Test the functionality of the Irrlicht timer
bool testCoreutil(void)
{
	bool ok = true;
	
	ok &= testMergeFilename();
	
	return ok;
}
