// Copyright (C) 2008-2012 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace
{

// Basic tests for identity matrix
bool testIdentityMatrix()
{
	bool passed = true;
	matrix4 mat;
	// Check default init
	passed &= assertLog(mat == core::IdentityMatrix);
	passed &= assertLog(core::IdentityMatrix == mat);
	// Since the last test can be made with isDefinitelyIdentityMatrix we
	// set it to false here
	mat.setDefinitelyIdentityMatrix(false);
	passed &= assertLog(mat == core::IdentityMatrix);
	passed &= assertLog(core::IdentityMatrix == mat);
	// also equals should see this
	passed &= assertLog(mat.equals(core::IdentityMatrix));
	passed &= assertLog(core::IdentityMatrix.equals(mat));
	// Check inequality
	mat[12] = 5.f;
	passed &= assertLog(mat != core::IdentityMatrix);
	passed &= assertLog(core::IdentityMatrix != mat);
	passed &= assertLog(!mat.equals(core::IdentityMatrix));
	passed &= assertLog(!core::IdentityMatrix.equals(mat));

	// Test multiplication
	passed &= assertLog(mat ==(core::IdentityMatrix * mat));
	passed &= assertLog(mat.equals(core::IdentityMatrix * mat));
	passed &= assertLog(mat == (mat * core::IdentityMatrix));
	passed &= assertLog(mat.equals(mat * core::IdentityMatrix));

	return passed;
}

// Test rotations
bool testTransformations()
{
	bool passed = true;
	matrix4 m, s;
	m.setRotationDegrees(core::vector3df(30,40,50));
	s.setScale(core::vector3df(2,3,4));
	m *= s;
	m.setTranslation(core::vector3df(5,6,7));
	passed &= assertLog(core::vector3df(5,6,7).equals(m.getTranslation()));
	passed &= assertLog(core::vector3df(2,3,4).equals(m.getScale()));
	core::vector3df newRotation = m.getRotationDegrees();
	passed &= assertLog(
		core::vector3df(30,40,50).equals(newRotation, 0.000004f));

	m.setRotationDegrees(vector3df(90.0001f, 270.85f, 180.0f));
	s.setRotationDegrees(vector3df(0,0, 0.860866f));
	m *= s;
	newRotation = m.getRotationDegrees();
	passed &= assertLog(
		core::vector3df(0,270,270).equals(newRotation, 0.0001f));

	m.setRotationDegrees(vector3df(270.0f, 89.8264f, 0.000100879f));
	s.setRotationDegrees(vector3df(0,0, 0.189398f));
	m *= s;
	newRotation = m.getRotationDegrees();
	passed &= assertLog(
		core::vector3df(0,90,90).equals(newRotation, 0.0001f));

	m.setRotationDegrees(vector3df(270.0f, 89.0602f, 359.999f));
	s.setRotationDegrees(vector3df(0,0, 0.949104f));
	m *= s;
	newRotation = m.getRotationDegrees();
	passed &= assertLog(core::vector3df(0,90,89.999f).equals(newRotation));

	return passed;
}

// Test rotations
bool testRotations()
{
	bool passed = true;
	matrix4 rot1, rot2, rot3, rot4, rot5;
	core::vector3df vec1(1,2,3), vec12(1,2,3);
	core::vector3df vec2(-5,0,0), vec22(-5,0,0);
	core::vector3df vec3(20,0,-20), vec32(20,0,-20);
	// Make sure the matrix multiplication and
	// rotation application give same results.
	rot1.setRotationDegrees(core::vector3df(90,0,0));
	rot2.setRotationDegrees(core::vector3df(0,90,0));
	rot3.setRotationDegrees(core::vector3df(0,0,90));
	rot4.setRotationDegrees(core::vector3df(90,90,90));
	rot5 = rot3 * rot2 * rot1;
	passed &= assertLog(rot4.equals(rot5, ROUNDING_ERROR_f32));
	rot4.transformVect(vec1);rot5.transformVect(vec12);
	rot4.transformVect(vec2);rot5.transformVect(vec22);
	rot4.transformVect(vec3);rot5.transformVect(vec32);
	passed &= assertLog(vec1.equals(vec12));
	passed &= assertLog(vec2.equals(vec22));
	passed &= assertLog(vec3.equals(vec32));

	vec1.set(1,2,3);
	vec12.set(1,2,3);
	vec2.set(-5,0,0);
	vec22.set(-5,0,0);
	vec3.set(20,0,-20);
	vec32.set(20,0,-20);
	rot1.setRotationDegrees(core::vector3df(45,0,0));
	rot2.setRotationDegrees(core::vector3df(0,45,0));
	rot3.setRotationDegrees(core::vector3df(0,0,45));
	rot4.setRotationDegrees(core::vector3df(45,45,45));
	rot5 = rot3 * rot2 * rot1;
	passed &= assertLog(rot4.equals(rot5, ROUNDING_ERROR_f32));
	rot4.transformVect(vec1);
	rot5.transformVect(vec12);
	rot4.transformVect(vec2);
	rot5.transformVect(vec22);
	rot4.transformVect(vec3);
	rot5.transformVect(vec32);
	passed &= assertLog(vec1.equals(vec12));
	passed &= assertLog(vec2.equals(vec22));
	passed &= assertLog(vec3.equals(vec32, 2 * ROUNDING_ERROR_f32));

	vec1.set(1,2,3);
	vec12.set(1,2,3);
	vec2.set(-5,0,0);
	vec22.set(-5,0,0);
	vec3.set(20,0,-20);
	vec32.set(20,0,-20);
	rot1.setRotationDegrees(core::vector3df(-60,0,0));
	rot2.setRotationDegrees(core::vector3df(0,-60,0));
	rot3.setRotationDegrees(core::vector3df(0,0,-60));
	rot4.setRotationDegrees(core::vector3df(-60,-60,-60));
	rot5 = rot3 * rot2 * rot1;
	passed &= assertLog(rot4.equals(rot5, ROUNDING_ERROR_f32));
	rot4.transformVect(vec1);
	rot5.transformVect(vec12);
	rot4.transformVect(vec2);
	rot5.transformVect(vec22);
	rot4.transformVect(vec3);
	rot5.transformVect(vec32);
	passed &= assertLog(vec1.equals(vec12));
	passed &= assertLog(vec2.equals(vec22));
	// this one needs higher tolerance due to rounding issues
	passed &= assertLog(vec3.equals(vec32, 0.000002f));

	vec1.set(1,2,3);
	vec12.set(1,2,3);
	vec2.set(-5,0,0);
	vec22.set(-5,0,0);
	vec3.set(20,0,-20);
	vec32.set(20,0,-20);
	rot1.setRotationDegrees(core::vector3df(113,0,0));
	rot2.setRotationDegrees(core::vector3df(0,-27,0));
	rot3.setRotationDegrees(core::vector3df(0,0,193));
	rot4.setRotationDegrees(core::vector3df(113,-27,193));
	rot5 = rot3 * rot2 * rot1;
	passed &= assertLog(rot4.equals(rot5, ROUNDING_ERROR_f32));
	rot4.transformVect(vec1);
	rot5.transformVect(vec12);
	rot4.transformVect(vec2);
	rot5.transformVect(vec22);
	rot4.transformVect(vec3);
	rot5.transformVect(vec32);
	// these ones need higher tolerance due to rounding issues
	passed &= assertLog(vec1.equals(vec12, 0.000002f));
	passed &= assertLog(vec2.equals(vec22));
	passed &= assertLog(vec3.equals(vec32, 0.000002f));

	rot1.setRotationDegrees(core::vector3df(0,0,34));
	rot2.setRotationDegrees(core::vector3df(0,43,0));
	vec1=(rot2 * rot1).getRotationDegrees();
	passed &= assertLog(vec1.equals(
		core::vector3df(27.5400505f, 34.4302292f, 42.6845398f),
		0.000002f));

	// corner cases
	rot1.setRotationDegrees(irr::core::vector3df(180.0f, 0.f, 0.f));
	vec1=rot1.getRotationDegrees();
	passed &= assertLog(
		vec1.equals(core::vector3df(180.0f, 0.f, 0.f), 0.000002f));
	rot1.setRotationDegrees(irr::core::vector3df(0.f, 180.0f, 0.f));
	vec1=rot1.getRotationDegrees();
	passed &= assertLog(
		vec1.equals(core::vector3df(180.0f, 360, 180.0f), 0.000002f));
	rot1.setRotationDegrees(irr::core::vector3df(0.f, 0.f, 180.0f));
	vec1=rot1.getRotationDegrees();
	passed &= assertLog(
		vec1.equals(core::vector3df(0.f, 0.f, 180.0f), 0.000002f));

	rot1.makeIdentity();
	rot1.setRotationDegrees(core::vector3df(270.f,0,0));
	rot2.makeIdentity();
	rot2.setRotationDegrees(core::vector3df(-90.f,0,0));
	vec1 = (rot1 * rot2).getRotationDegrees();
	passed &= assertLog(vec1.equals(core::vector3df(180.f, 0.f, 0.0f)));

	return passed;
}

// Test isOrthogonal
bool testIsOrthogonal()
{
	matrix4 rotationMatrix;
	if (!assertLog(rotationMatrix.isOrthogonal()))
	{
		std::cerr << "irr::core::matrix4::isOrthogonal() "\
			"failed with Identity.\n";
		return false;
	}

	rotationMatrix.setRotationDegrees(vector3df(90, 0, 0));
	if (!assertLog(rotationMatrix.isOrthogonal()))
	{
		std::cerr << "irr::core::matrix4::isOrthogonal() "\
			"failed with rotation.\n";
		return false;
	}

	matrix4 translationMatrix;
	translationMatrix.setTranslation(vector3df(0, 3, 0));
	if (!assertLog(!translationMatrix.isOrthogonal()))
	{
		std::cerr <<"irr::core::matrix4::isOrthogonal() "\
			"failed with translation.\n";
		return false;
	}

	matrix4 scaleMatrix;
	scaleMatrix.setScale(vector3df(1, 2, 3));
	if (!assertLog(scaleMatrix.isOrthogonal()))
	{
		std::cerr << "irr::core::matrix4::isOrthogonal() "\
			"failed with scale.\n";
		return false;
	}

	return true;
}

bool checkMatrixRotation(irr::core::matrix4 &m,
		const vector3df &vector, const vector3df &expectedpassed)
{
	vector3df v(vector);
	m.rotateVect(v);
	if (assertLog(expectedpassed.equals(v)))
		return true;

	std::cerr << "checkMatrixRotation failed for vector "
		<< vector.X << ' ' << vector.Y << ' ' << vector.Z
		<< ". Expected "
		<< expectedpassed.X << ' '
		<< expectedpassed.Y << ' '
		<< expectedpassed.Z
		<< ", got " << v.X << ' ' << v.Y << ' ' << v.Z << '\n';

	std::cerr << "matrix: ";
	for (int i = 0; i < 16; ++i)
		std::cerr << m[i] << ' ';
	std::cerr << '\n';

	// Test failed.
	return false;
}

bool setRotationAxis()
{
	matrix4 m;
	vector3df v;
	
	// y up, x right, z depth (as usual)

	// y rotated around x-axis
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(1,0,0)),
			vector3df(0,1,0), vector3df(0, 0, 1))))
	{
		return false;
	}
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(180.f * DEGTORAD, vector3df(1,0,0)),
			vector3df(0,1,0), vector3df(0, -1, 0))))
	{
		return false;
	}
	
	// y rotated around negative x-axis
	m.makeIdentity();
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(-1,0,0)),
			vector3df(0,1,0), vector3df(0, 0, -1))))
	{
		return false;
	}
	
	// x rotated around x-axis
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(1,0,0)),
			vector3df(1,0,0), vector3df(1, 0, 0))))
	{
		return false;
	}

	// x rotated around y-axis
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(0,1,0)),
			vector3df(1,0,0), vector3df(0, 0, -1))))
	{
		return false;
	}
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(180.f * DEGTORAD, vector3df(0,1,0)),
			vector3df(1,0,0), vector3df(-1, 0, 0))))
	{
		return false;
	}
	
	// x rotated around negative y-axis
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(0,-1,0)),
			vector3df(1,0,0), vector3df(0, 0, 1))))
	{
		return false;
	} 
	
	// y rotated around y-axis
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(0,1,0)),
			vector3df(0,1,0), vector3df(0, 1, 0))))
	{
		return false;
	}

	// x rotated around z-axis
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(0,0,1)),
			vector3df(1,0,0), vector3df(0, 1, 0))))
	{
		return false;
	}
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(180.f * DEGTORAD, vector3df(0,0,1)),
			vector3df(1,0,0), vector3df(-1, 0, 0))))
	{
		return false;
	}

	// x rotated around negative z-axis
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(0,0,-1)),
			vector3df(1,0,0), vector3df(0, -1, 0))))
	{
		return false;
	}
	
	// y rotated around z-axis
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(0,0,1)),
			vector3df(0,1,0), vector3df(-1, 0, 0))))
	{
		return false;
	}
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(180.f * DEGTORAD, vector3df(0,0,1)),
			vector3df(0,1,0), vector3df(0, -1, 0))))
	{
		return false;
	}
	
	// z rotated around z-axis
	if (!assertLog(checkMatrixRotation(
		m.setRotationAxisRadians(90.f * DEGTORAD, vector3df(0,0,1)),
			vector3df(0,0,1), vector3df(0, 0, 1))))
	{
		return false;
	}

	
	return true;
}

}

bool matrixOps(void)
{
	bool passed = true;
	passed &= testIdentityMatrix();
	passed &= testRotations();
	passed &= testIsOrthogonal();
	passed &= testTransformations();
	passed &= setRotationAxis();
	return passed;
}

int main()
{
	return runTest(matrixOps, "testMatrixOps");
}
