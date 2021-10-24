// Copyright (C) 2008-2012 Colin MacDonald and Christian Stehno
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

bool testIrrCoreEquals()
{
	bool passed = true;
	// float tests
	passed &= assertLog(irr::core::equals(99.f, 99.f));
	passed &= assertLog(irr::core::equals(99.f, 98.f, 1.f));
	
	// double tests
	passed &= assertLog(irr::core::equals(99.0, 99.0));
	passed &= assertLog(irr::core::equals(99.0, 98.0, 1.0));
	
	// int tests
	passed &= assertLog(irr::core::equals(99, 99));
	passed &= assertLog(irr::core::equals(99, 98, 1));
	passed &= assertLog(!irr::core::equals(99, 98, 0));
	passed &= assertLog(irr::core::equals(-99, -99));
	passed &= assertLog(irr::core::equals(-99, -98, 1));
	passed &= assertLog(!irr::core::equals(-99, -98, 0));

	// iszero is a specialized equals method
	// float tests
	passed &= assertLog(irr::core::iszero(0.0f));
	passed &= assertLog(!irr::core::iszero(-1.0f));
	passed &= assertLog(irr::core::iszero(1.0f, 1.0f));

	// double tests
	passed &= assertLog(irr::core::iszero(0.0));
	passed &= assertLog(!irr::core::iszero(-1.0));
	passed &= assertLog(irr::core::iszero(-2.0, 2.0));

	// int tests
	passed &= assertLog(irr::core::iszero(0));
	passed &= assertLog(!irr::core::iszero(-1));
	passed &= assertLog(irr::core::iszero(1, 1));


	return passed;
}

int main()
{
	return runTest(testIrrCoreEquals, "testIrrCoreEquals");
}
