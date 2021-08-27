// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_PARTICLE_BOX_EMITTER_H_INCLUDED
#define IRR_C_PARTICLE_BOX_EMITTER_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_PARTICLES_

#include "IParticleBoxEmitter.h"
#include "irrArray.h"
#include "aabbox3d.h"

namespace irr
{
namespace scene
{

//! A default box emitter
class CParticleBoxEmitter : public IParticleBoxEmitter
{
public:

	//! constructor
	CParticleBoxEmitter(
		const core::aabbox3df& box,
		const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
		u32 minParticlesPerSecond = 20,
		u32 maxParticlesPerSecond = 40,
		video::SColor minStartColor = video::SColor(255,0,0,0),
		video::SColor maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000,
		u32 lifeTimeMax=4000,
		s32 maxAngleDegrees=0,
		const core::dimension2df& minStartSize = core::dimension2df(5.0f,5.0f),
		const core::dimension2df& maxStartSize = core::dimension2df(5.0f,5.0f)
		);

	//! Prepares an array with new particles to emitt into the system
	//! and returns how much new particles there are.
	virtual s32 emitt(u32 now, u32 timeSinceLastCall, SParticle*& outArray) IRR_OVERRIDE;

	//! Set direction the emitter emits particles.
	virtual void setDirection( const core::vector3df& newDirection ) IRR_OVERRIDE { Direction = newDirection; }

	//! Set minimum number of particles emitted per second.
	virtual void setMinParticlesPerSecond( u32 minPPS ) IRR_OVERRIDE { MinParticlesPerSecond = minPPS; }

	//! Set maximum number of particles emitted per second.
	virtual void setMaxParticlesPerSecond( u32 maxPPS ) IRR_OVERRIDE { MaxParticlesPerSecond = maxPPS; }

	//! Set minimum start color.
	virtual void setMinStartColor( const video::SColor& color ) IRR_OVERRIDE { MinStartColor = color; }

	//! Set maximum start color.
	virtual void setMaxStartColor( const video::SColor& color ) IRR_OVERRIDE { MaxStartColor = color; }

	//! Set the maximum starting size for particles
	virtual void setMaxStartSize( const core::dimension2df& size ) IRR_OVERRIDE { MaxStartSize = size; }

	//! Set the minimum starting size for particles
	virtual void setMinStartSize( const core::dimension2df& size ) IRR_OVERRIDE { MinStartSize = size; }

	//! Set the minimum particle life-time in milliseconds
	virtual void setMinLifeTime( u32 lifeTimeMin ) IRR_OVERRIDE { MinLifeTime = lifeTimeMin; }

	//! Set the maximum particle life-time in milliseconds
	virtual void setMaxLifeTime( u32 lifeTimeMax ) IRR_OVERRIDE { MaxLifeTime = lifeTimeMax; }

	//!	Maximal random derivation from the direction
	virtual void setMaxAngleDegrees( s32 maxAngleDegrees ) IRR_OVERRIDE { MaxAngleDegrees = maxAngleDegrees; }

	//! Set box from which the particles are emitted.
	virtual void setBox( const core::aabbox3df& box ) IRR_OVERRIDE { Box = box; }

	//! Gets direction the emitter emits particles.
	virtual const core::vector3df& getDirection() const IRR_OVERRIDE { return Direction; }

	//! Gets minimum number of particles emitted per second.
	virtual u32 getMinParticlesPerSecond() const IRR_OVERRIDE { return MinParticlesPerSecond; }

	//! Gets maximum number of particles emitted per second.
	virtual u32 getMaxParticlesPerSecond() const IRR_OVERRIDE { return MaxParticlesPerSecond; }

	//! Gets minimum start color.
	virtual const video::SColor& getMinStartColor() const IRR_OVERRIDE { return MinStartColor; }

	//! Gets maximum start color.
	virtual const video::SColor& getMaxStartColor() const IRR_OVERRIDE { return MaxStartColor; }

	//! Gets the maximum starting size for particles
	virtual const core::dimension2df& getMaxStartSize() const IRR_OVERRIDE { return MaxStartSize; }

	//! Gets the minimum starting size for particles
	virtual const core::dimension2df& getMinStartSize() const IRR_OVERRIDE { return MinStartSize; }

	//! Get the minimum particle life-time in milliseconds
	virtual u32 getMinLifeTime() const IRR_OVERRIDE { return MinLifeTime; }

	//! Get the maximum particle life-time in milliseconds
	virtual u32 getMaxLifeTime() const IRR_OVERRIDE { return MaxLifeTime; }

	//!	Maximal random derivation from the direction
	virtual s32 getMaxAngleDegrees() const IRR_OVERRIDE { return MaxAngleDegrees; }

	//! Get box from which the particles are emitted.
	virtual const core::aabbox3df& getBox() const IRR_OVERRIDE { return Box; }

	//! Writes attributes of the object.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const IRR_OVERRIDE;

	//! Reads attributes of the object.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) IRR_OVERRIDE;

private:

	core::array<SParticle> Particles;
	core::aabbox3df Box;
	core::vector3df Direction;
	core::dimension2df MaxStartSize, MinStartSize;
	u32 MinParticlesPerSecond, MaxParticlesPerSecond;
	video::SColor MinStartColor, MaxStartColor;
	u32 MinLifeTime, MaxLifeTime;

	u32 Time;
	s32 MaxAngleDegrees;
};

} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_PARTICLES_

#endif
