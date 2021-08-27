// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_I_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED
#define IRR_I_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED

#include "IParticleAffector.h"

namespace irr
{
namespace scene
{

//! A particle affector which rotates the particle system.
class IParticleRotationAffector : public IParticleAffector
{
public:

	//! Set the point that particles will rotate around
	virtual void setPivotPoint( const core::vector3df& point ) = 0;

	//! Set the speed in degrees per second in all 3 dimensions
	virtual void setSpeed( const core::vector3df& speed ) = 0;

	//! Get the point that particles are attracted to
	virtual const core::vector3df& getPivotPoint() const = 0;

	//! Get the speed in degrees per second in all 3 dimensions
	virtual const core::vector3df& getSpeed() const = 0;

	//! Get emitter type
	virtual E_PARTICLE_AFFECTOR_TYPE getType() const IRR_OVERRIDE { return EPAT_ROTATE; }
};

} // end namespace scene
} // end namespace irr

#endif // IRR_I_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED
