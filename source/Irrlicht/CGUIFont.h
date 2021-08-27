// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_GUI_FONT_H_INCLUDED
#define IRR_C_GUI_FONT_H_INCLUDED

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIFontBitmap.h"
#include "irrString.h"
#include "irrMap.h"
#include "IXMLReader.h"
#include "IReadFile.h"
#include "irrArray.h"

namespace irr
{

namespace video
{
	class IVideoDriver;
	class IImage;
}

namespace gui
{

	class IGUIEnvironment;

class CGUIFont : public IGUIFontBitmap
{
public:

	//! constructor
	CGUIFont(IGUIEnvironment* env, const io::path& filename);

	//! destructor
	virtual ~CGUIFont();

	//! loads a font from a texture file
	bool load(const io::path& filename);

	//! loads a font from a texture file
	bool load(io::IReadFile* file);

	//! loads a font from an XML file
	//\param directory Directory in which the bitmaps can be found
	bool load(io::IXMLReader* xml, const io::path& directory);

	//! draws an text and clips it to the specified rectangle if wanted
	virtual void draw(const core::stringw& text, const core::rect<s32>& position,
			video::SColor color, bool hcenter=false,
			bool vcenter=false, const core::rect<s32>* clip=0) IRR_OVERRIDE;

	//! returns the dimension of a text
	virtual core::dimension2d<u32> getDimension(const wchar_t* text) const IRR_OVERRIDE;

	//! Calculates the index of the character in the text which is on a specific position.
	virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x) const IRR_OVERRIDE;

	//! Returns the type of this font
	virtual EGUI_FONT_TYPE getType() const IRR_OVERRIDE { return EGFT_BITMAP; }

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual void setKerningWidth (s32 kerning) IRR_OVERRIDE;
	virtual void setKerningHeight (s32 kerning) IRR_OVERRIDE;

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual s32 getKerningWidth(const wchar_t* thisLetter=0, const wchar_t* previousLetter=0) const IRR_OVERRIDE;
	virtual s32 getKerningHeight() const IRR_OVERRIDE;

	//! gets the sprite bank
	virtual IGUISpriteBank* getSpriteBank() const IRR_OVERRIDE;

	//! returns the sprite number from a given character
	virtual u32 getSpriteNoFromChar(const wchar_t *c) const IRR_OVERRIDE;

	virtual void setInvisibleCharacters( const wchar_t *s ) IRR_OVERRIDE;

private:

	struct SFontArea
	{
		SFontArea() : underhang(0), overhang(0), width(0), spriteno(0) {}
		s32				underhang;
		s32				overhang;
		s32				width;
		u32				spriteno;
	};

	//! load & prepare font from ITexture
	bool loadTexture(video::IImage * image, const io::path& name);

	void readPositions(video::IImage* texture, s32& lowerRightPositions);

	s32 getAreaFromCharacter (const wchar_t c) const;
	void setMaxHeight();

	void pushTextureCreationFlags(bool(&flags)[3]);
	void popTextureCreationFlags(const bool(&flags)[3]);

	core::array<SFontArea>		Areas;
	core::map<wchar_t, s32>		CharacterMap;
	video::IVideoDriver*		Driver;
	IGUISpriteBank*			SpriteBank;
	IGUIEnvironment*		Environment;
	u32				WrongCharacter;
	s32				MaxHeight;
	s32				GlobalKerningWidth, GlobalKerningHeight;

	core::stringw Invisible;
};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // IRR_C_GUI_FONT_H_INCLUDED
