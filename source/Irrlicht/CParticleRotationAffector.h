// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED
#define IRR_C_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_PARTICLES_

#include "IParticleRotationAffector.h"

namespace irr
{
namespace scene
{

//! Particle Affector for rotating particles about a point
class CParticleRotationAffector : public IParticleRotationAffector
{
public:

	CParticleRotationAffector( const core::vector3df& speed = core::vector3df(5.0f, 5.0f, 5.0f),
		const core::vector3df& point = core::vector3df() );

	//! Affects a particle.
	virtual void affect(u32 now, SParticle* particlearray, u32 count) IRR_OVERRIDE;

	//! Set the point that particles will attract to
	virtual void setPivotPoint( const core::vector3df& point ) IRR_OVERRIDE { PivotPoint = point; }

	//! Set the speed in degrees per second
	virtual void setSpeed( const core::vector3df& speed ) IRR_OVERRIDE { Speed = speed; }

	//! Get the point that particles are attracted to
	virtual const core::vector3df& getPivotPoint() const IRR_OVERRIDE { return PivotPoint; }

	//! Get the speed in degrees per second
	virtual const core::vector3df& getSpeed() const IRR_OVERRIDE { return Speed; }

	//! Writes attributes of the object.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;

	//! Reads attributes of the object.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

private:

	core::vector3df PivotPoint;
	core::vector3df Speed;
	u32 LastTime;
};

} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_PARTICLES_

#endif // IRR_C_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED
