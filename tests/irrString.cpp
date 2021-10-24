// Copyright (C) 2008-2012 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;

static bool testSelfAssignment()
{
	core::stringw myString(L"foo");
	myString = myString;
	return myString == core::stringw(L"foo");
}

static bool testSplit()
{
	std::cerr << "Test stringw::split()\n";
	core::stringw teststring(L"[b]this [/b] is a [color=0xff000000]test[/color].");
	core::list<core::stringw> parts1;
	teststring.split<core::list<core::stringw> >(parts1, L"[");
	core::array<core::stringw> parts2;
	teststring.split<core::array<core::stringw> >(parts2, L"[", 1, false, true);
	return (parts1.size()==4) && (parts2.size()==9);
}

static bool testFastAlloc()
{
	core::string<wchar_t, core::irrAllocatorFast<wchar_t> > FastString(L"abc");
	core::string<wchar_t, core::irrAllocatorFast<wchar_t> > FastStringLong(L"longer");

	FastString  = L"test";

	// cause a reallocation
	FastString = FastStringLong;

	// this test should either not compile or crash when the allocaters are messed up
	return true;
}

static bool testReplace()
{
	bool passed = true;
	// test string getting longer
	core::stringw str = L"no";
	str.replace(L"no", L"yes");
	passed &= assertLog(str == L"yes");

	str = L"nonono";
	str.replace(L"no", L"yes");
	passed &= assertLog(str == L"yesyesyes");
	
	str = L"nomaybenomaybeno";
	str.replace(L"no", L"yes");
	passed &= assertLog(str == L"yesmaybeyesmaybeyes");

	// test string staying same length
	str = L"one";
	str.replace(L"one", L"two");
	passed &= assertLog(str == L"two");
	
	str = L"oneone";
	str.replace(L"one", L"two");
	passed &= assertLog(str == L"twotwo");

	// test string getting shorter
	str = L"yes";
	str.replace(L"yes", L"no");
	passed &= assertLog(str == L"no");

	str = L"yesyes";
	str.replace(L"yes", L"no");
	passed &= assertLog(str == L"nono");

	// remove string-parts completely
	str = L"killme";
	str.replace(L"killme", L"");
	passed &= assertLog(str == L"");

	str = L"killmenow";
	str.replace(L"killme", L"");
	passed &= assertLog(str == L"now");

	str = L"nowkillme";
	str.replace(L"killme", L"");
	passed &= assertLog(str == L"now");

	// remove nothing
	str = L"keepme";
	str.replace(L"", L"whatever");
	passed &= assertLog(str == L"keepme");

	str = L"keepme";
	str.replace(L"", L"");
	passed&= assertLog(str == L"keepme");

	return passed;
}


bool testAppendStringc()
{
	bool passed = true;
	core::stringc str;
	// Test with character
	passed &= assertLog(str == "");
	str += 'W';
	passed &= assertLog(str == "W");
	str += 'i';
	passed &= assertLog(str == "Wi");
	str = "";
	passed &= assertLog(str == "");

	// Test with C-style string
	str += "Another Test";
	passed &= assertLog(str == "Another Test");
	str = "";
	str += 'A';
	str += "nother Test";
	passed &= assertLog(str == "Another Test");
	str = "";

	// Test with int
	str += 10;
	passed &= assertLog(str == "10");
	str += 0;
	passed &= assertLog(str == "100");
	str = "";
	str += -32;
	passed &= assertLog(str == "-32");
	str = "";

	// Test with unsigned int
	str += 21u;
	passed &= assertLog(str == "21");
	str += 0u;
	passed &= assertLog(str == "210");
	str = "";

	// Test with long int
	str += 456l;
	passed &= assertLog(str == "456");
	str += 0l;
	passed &= assertLog(str == "4560");
	str = "";
	str += -456l;
	passed &= assertLog(str == "-456");
	str = "";

	// Test with unsigned long
	str += 994ul;
	passed &= assertLog(str == "994");
	str += 0ul;
	passed &= assertLog(str == "9940");
	str = "";
	return passed;
}

bool testInsert()
{
	bool passed = true;
	core::stringc str;

	str.insert(0, "something", 4);
	passed &= assertLog(str == "some");
	str.insert(4, "thing", 5);
	passed &= assertLog(str == "something");
	str.insert(0, "is ", 3);
	passed &= assertLog(str == "is something");
	str.insert(3, "there ", 6);
	passed&= assertLog(str == "is there something");

	return passed;
}

bool testLowerUpper()
{
	bool passed = true;
	irr::core::array<irr::core::stringc>
		stringsOrig, targetLower, targetUpper;
	stringsOrig.push_back("abc");
	targetLower.push_back("abc");
	targetUpper.push_back("ABC");
	stringsOrig.push_back("ABC");
	targetLower.push_back("abc");
	targetUpper.push_back("ABC");
	stringsOrig.push_back("Abc");
	targetLower.push_back("abc");
	targetUpper.push_back("ABC");
	stringsOrig.push_back("aBBc");
	targetLower.push_back("abbc");
	targetUpper.push_back("ABBC");
	stringsOrig.push_back("abC");
	targetLower.push_back("abc");
	targetUpper.push_back("ABC");
	stringsOrig.push_back("");
	targetLower.push_back("");
	targetUpper.push_back("");
	/* TODO: those are not supported so far
	stringsOrig.push_back("ßäöü");
	targetLower.push_back("ßäöü");
	targetUpper.push_back("ßÄÖÜ");
	stringsOrig.push_back("ßÄÖÜ");
	targetLower.push_back("ßäöü");
	targetUpper.push_back("ßÄÖÜ");
	*/

	for ( irr::u32 i=0; i<stringsOrig.size(); ++i )
	{
		irr::core::stringc c = stringsOrig[i];
		c.make_lower();
		passed &= assertLog(c == targetLower[i]);

		c = stringsOrig[i];
		c.make_upper();
		passed &= assertLog(c == targetUpper[i]);

		irr::core::stringw w = irr::core::stringw(stringsOrig[i]);
		c.make_lower();
		passed &= assertLog(c == irr::core::stringw(targetLower[i]));

		c = irr::core::stringw(stringsOrig[i]);
		c.make_upper();
		passed &= assertLog(c == irr::core::stringw(targetUpper[i]));
	}

	return passed;
}

bool testFindFunctions()
{
	irr::core::stringc dot(".");
	irr::s32 p = dot.findFirst(0);
	if ( p >= 0 )
		return false;

	irr::core::stringc empty("");
	p = empty.findLastCharNotInList("x",1);
	if ( p >= 0 )
		return false;

	irr::core::stringc lastX("max");
	p = lastX.findLastCharNotInList("x",1);
	if ( p != 1 )
		return false;
	p = lastX.findLastCharNotInList("y",1);
	if ( p != 2 )
		return false;

	p = empty.findLast('x');
	if ( p >= 0 )
		return false;

	p = dot.findLast('.');
	if ( p != 0 )
		return false;

	p = empty.findLastChar("ab", 2);
	if ( p >= 0 )
		return false;

	p = dot.findLastChar("-.", 2);
	if ( p != 0 )
		return false;

	return true;
}

bool testErase()
{
	if ( stringc(1.f).eraseTrailingFloatZeros() != stringc("1") )
		return false;

	if ( stringc("0.100000").eraseTrailingFloatZeros() != stringc("0.1") )
		return false;

	if ( stringc("10.000000").eraseTrailingFloatZeros() !=  stringc("10") )
		return false;

	if ( stringc("foo 3.140000").eraseTrailingFloatZeros() != stringc("foo 3.14") )
		return false;

	if ( stringc("no_num.000").eraseTrailingFloatZeros() != stringc("no_num.000") )
		return false;

	if ( stringc("1.").eraseTrailingFloatZeros() != stringc("1.") )
		return false;

	return true;
}

// Test the functionality of irrString
/** Validation is done with assert_log() against expected results. */
bool testIrrString(void)
{
	bool allExpected = true;

	std::cerr << "Test string\n";
	{
		// Check empty string
		core::stringc empty;
		assertLog(empty.size() == 0);
		assertLog(empty[0] == 0);
		assertLog(empty.c_str() != 0);
		assertLog(*(empty.c_str()) == 0);
		// Assign content
		empty = "Test";
		assertLog(empty.size() == 4);
		assertLog(empty[0] == 'T');
		assertLog(empty[3] == 't');
		assertLog(*(empty.c_str()) == 'T');
		//Assign empty string, should be same as in the beginning
		empty = "";
		assertLog(empty.size() == 0);
		assertLog(empty[0] == 0);
		assertLog(*(empty.c_str()) == 0);
	}
	std::cerr << "Test stringw\n";
	{
		core::stringw empty;
		assertLog(empty.size() == 0);
		assertLog(empty[0] == 0);
		assertLog(empty.c_str() != 0);
		assertLog(*(empty.c_str()) == 0);
		empty = L"Test";
		assertLog(empty.size() == 4);
		assertLog(empty[0] == L'T');
		assertLog(empty[3] == 't');
		assertLog(*(empty.c_str()) == L'T');
		empty = L"";
		assertLog(empty.size() == 0);
		assertLog(empty[0] == 0);
		assertLog(*(empty.c_str()) == 0);
		assertLog(allExpected &= testSplit());
	}
	allExpected &= testAppendStringc();

	allExpected &= testInsert();

	std::cout << "Test io::path\n";
	{
		// Only test that this type exists, it's one from above
		io::path myPath;
		myPath = "Some text"; // Only to avoid wrong optimizations
	}

	std::cerr << "Test self assignment\n";
	allExpected &= testSelfAssignment();

	std::cerr << "test fast alloc\n";
	allExpected &= testFastAlloc();

	std::cerr << "test replace\n";
	allExpected &= testReplace();

	std::cerr << "test make_lower and make_uppers\n";
	allExpected &= testLowerUpper();

	std::cerr << "test find functions\n";
	allExpected &= testFindFunctions();

	std::cerr << "test erase functions\n";
	allExpected &= testErase();

	assertLog(allExpected);

	return allExpected;
}

int main()
{
	return runTest(testIrrString, "testIrrString");
}
