// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_SPRITE_BANK_H_INCLUDED
#define IRR_C_GUI_SPRITE_BANK_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISpriteBank.h"

namespace irr
{

namespace video
{
	class IVideoDriver;
	class ITexture;
}

namespace gui
{

	class IGUIEnvironment;

//! Sprite bank interface.
class CGUISpriteBank : public IGUISpriteBank
{
public:

	CGUISpriteBank(IGUIEnvironment* env);
	virtual ~CGUISpriteBank();

	virtual core::array< core::rect<s32> >& getPositions() IRR_OVERRIDE;
	virtual core::array< SGUISprite >& getSprites() IRR_OVERRIDE;

	virtual u32 getTextureCount() const IRR_OVERRIDE;
	virtual video::ITexture* getTexture(u32 index) const IRR_OVERRIDE;
	virtual void addTexture(video::ITexture* texture) IRR_OVERRIDE;
	virtual void setTexture(u32 index, video::ITexture* texture) IRR_OVERRIDE;

	//! Add the texture and use it for a single non-animated sprite.
	virtual s32 addTextureAsSprite(video::ITexture* texture) IRR_OVERRIDE;

	//! clears sprites, rectangles and textures
	virtual void clear() IRR_OVERRIDE;

	//! Draws a sprite in 2d with position and color
	virtual void draw2DSprite(u32 index, const core::position2di& pos, const core::rect<s32>* clip=0,
				const video::SColor& color= video::SColor(255,255,255,255),
				u32 starttime=0, u32 currenttime=0, bool loop=true, bool center=false) IRR_OVERRIDE;

	//! Draws a sprite in 2d with destination rectangle and colors
	virtual void draw2DSprite(u32 index, const core::rect<s32>& destRect,
			const core::rect<s32>* clip=0,
			const video::SColor * const colors=0,
			u32 timeTicks = 0,
			bool loop=true) IRR_OVERRIDE;

	//! Draws a sprite batch in 2d using an array of positions and a color
	virtual void draw2DSpriteBatch(const core::array<u32>& indices, const core::array<core::position2di>& pos,
			const core::rect<s32>* clip=0,
			const video::SColor& color= video::SColor(255,255,255,255),
			u32 starttime=0, u32 currenttime=0,
			bool loop=true, bool center=false) IRR_OVERRIDE;

protected:

	bool getFrameNr(u32& frameNr, u32 index, u32 time, bool loop) const;

	struct SDrawBatch
	{
		core::array<core::position2di> positions;
		core::array<core::recti> sourceRects;
		u32 textureNumber;
	};

	core::array<SGUISprite> Sprites;
	core::array< core::rect<s32> > Rectangles;
	core::array<video::ITexture*> Textures;
	IGUIEnvironment* Environment;
	video::IVideoDriver* Driver;

};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // IRR_C_GUI_SPRITE_BANK_H_INCLUDED
