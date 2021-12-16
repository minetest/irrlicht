// Copyright (C) 2008-2012 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;

static void logLine(const line2df &line)
{
	std::cerr << '(' << line.start.X << ',' << line.start.Y << ')'
		<< " to " << '(' << line.end.X << ',' << line.end.Y << ")\n";
}

static bool testLineIntersection(line2df const &line1,
		line2df const &line2,
		bool expectedHit,
		const vector2df &expectedIntersection)
{
	std::cerr << "\nLine 1 ";
	logLine(line1);
	std::cerr << "Line2 ";
	logLine(line2);
	std::cerr << '\n';

	vector2df intersection;
	bool linesIntersect = line1.intersectWith(line2, intersection);
	if (linesIntersect)
	{
		std::cerr << "Lines intersect at point ("
			<< intersection.X << ',' << intersection.Y << ")\n";
	};

	if (expectedHit)
	{
		if(!assertLog(linesIntersect))
		{
			std::cerr << "ERR: Lines do not intersect!\n";
			return false;
		}

		if (!assertLog(line1.isPointOnLine(intersection))
			&& line2.isPointOnLine(intersection))
		{
			std::cerr << "ERR: Point is not on both lines!\n";
			return false;
		}

		if (!assertLog(intersection == expectedIntersection))
		{
			std::cerr << "ERR: Intersection at wrong point!\n"
				<< "Expected ("
				<< expectedIntersection.X << ','
				<< expectedIntersection.Y << ")\n";
		}
	}
	else
	{
		if(!assertLog(!linesIntersect))
		{
			std::cerr << "ERR: Lines should not intersect!\n";
			return false;
		}
	}

	// If there were no failures.
	return true;
}

static bool testLines(line2df const &line1,
		line2df const &line2,
		bool expectedHit,
		const vector2df &expectedIntersection)
{
	// Test in both orders to make sure intersection is commutative.
	return testLineIntersection(
			line1, line2, expectedHit, expectedIntersection)
		&& testLineIntersection(
			line2, line1, expectedHit, expectedIntersection);
}

// Test the functionality of line2d>T>::intersectWith().
/** Validation is done with assertLog() against expected results. */
bool line2dIntersectWith(void)
{
	bool allExpected = true;

	// Crossing lines, horizontal and vertical
	allExpected &= testLines(line2df(vector2df(1,1),vector2df(1,3)),
							line2df(vector2df(0,2),vector2df(2,2)),
							true, vector2df(1,2));
	assertLog(allExpected);

	// Crossing lines, both diagonal
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(2,2)),
							line2df(vector2df(0,2),vector2df(2,0)),
							true, vector2df(1,1));
	assertLog(allExpected);

	// Non-crossing lines, horizontal and vertical
	allExpected &= testLines(line2df(vector2df(1,1),vector2df(1,3)),
							line2df(vector2df(0,4),vector2df(2,4)),
							false, vector2df());
	assertLog(allExpected);

	// Non-crossing lines, both diagonal
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(2,2)),
							line2df(vector2df(3,4),vector2df(4,3)),
							false, vector2df());
	assertLog(allExpected);

	// Meeting at a common point
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,0)),
							line2df(vector2df(1,0),vector2df(2,0)),
							true, vector2df(1,0));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,0)),
							line2df(vector2df(1,0),vector2df(0,1)),
							true, vector2df(1,0));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,0)),
							line2df(vector2df(1,0),vector2df(0,-1)),
							true, vector2df(1,0));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(0,1)),
							line2df(vector2df(0,1),vector2df(1,1)),
							true, vector2df(0,1));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(0,1)),
							line2df(vector2df(0,1),vector2df(1,-1)),
							true, vector2df(0,1));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(0,1)),
							line2df(vector2df(0,1),vector2df(0,2)),
							true, vector2df(0,1));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,0)),
							line2df(vector2df(1,0),vector2df(2,0)),
							true, vector2df(1,0));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,1)),
							line2df(vector2df(1,1),vector2df(0,2)),
							true, vector2df(1,1));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,1)),
							line2df(vector2df(1,1),vector2df(2,0)),
							true, vector2df(1,1));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,1)),
							line2df(vector2df(1,1),vector2df(2,2)),
							true, vector2df(1,1));
	assertLog(allExpected);


	// Parallel lines, no intersection
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,0)),
							line2df(vector2df(0,1),vector2df(1,1)),
							false, vector2df());
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(0,1)),
							line2df(vector2df(1,0),vector2df(1,1)),
							false, vector2df());
	assertLog(allExpected);

	// Non parallel lines, no intersection
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,0)),
							line2df(vector2df(0,1),vector2df(0,2)),
							false, vector2df());
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(0,1)),
							line2df(vector2df(1,0),vector2df(2,0)),
							false, vector2df());
	assertLog(allExpected);

	// Coincident (and thus parallel) lines
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,0)),
							line2df(vector2df(0,0),vector2df(1,0)),
							true, vector2df(0,0));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(2,0),vector2df(0,2)),
							line2df(vector2df(2,0),vector2df(0,2)),
							true, vector2df(2,0));
	assertLog(allExpected);

	// Two segments of the same unlimited line, but no intersection
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,1)),
							line2df(vector2df(2,2),vector2df(3,3)),
							false, vector2df());
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(1,0)),
							line2df(vector2df(2,0),vector2df(3,0)),
							false, vector2df());
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(0,1)),
							line2df(vector2df(0,2),vector2df(0,3)),
							false, vector2df());
	assertLog(allExpected);

	// Overlapping parallel lines
	allExpected &= testLines(line2df(vector2df(1,0),vector2df(2,0)),
							line2df(vector2df(0,0),vector2df(3,0)),
							true, vector2df(1.5f, 0));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,1),vector2df(0,2)),
							line2df(vector2df(0,0),vector2df(0,3)),
							true, vector2df(0, 1.5f));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(1,0),vector2df(2,0)),
							line2df(vector2df(0,0),vector2df(3,0)),
							true, vector2df(1.5f, 0));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,1),vector2df(0,2)),
							line2df(vector2df(0,0),vector2df(0,3)),
							true, vector2df(0, 1.5f));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(1,1),vector2df(2,2)),
							line2df(vector2df(0,0),vector2df(3,3)),
							true, vector2df(1.5f, 1.5f));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(1,2),vector2df(2,1)),
							line2df(vector2df(0,3),vector2df(3,0)),
							true, vector2df(1.5f, 1.5f));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(10,8)),
							line2df(vector2df(2.5f,2.0f),vector2df(5.0f,4.0f)),
							true, vector2df(3.75f, 3.0f));
	assertLog(allExpected);
	allExpected &= testLines(line2df(vector2df(0,0),vector2df(2000,1000)),
							line2df(vector2df(2,1),vector2df(2.2f,1.4f)),
							true, vector2df(2.0f, 1.0f));
	assertLog(allExpected);

	return allExpected;
}

bool getClosestPoint(void)
{
	// testcase that fails when integers are handled like floats
	irr::core::line2di line(-283, -372, 374, 289);
	irr::core::vector2di p1 = line.getClosestPoint( irr::core::vector2di(290,372) );
	irr::core::vector2di p2 = line.getClosestPoint( irr::core::vector2di(135,372) );
	return assertLog(p1 != p2);
}

bool testLine2d(void)
{
	bool allExpected = true;

	allExpected &= line2dIntersectWith();
	allExpected &= getClosestPoint();

	return allExpected;
}


int main()
{
	return runTest(testLine2d, "testline2DSimple");
}
