// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIFont.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "os.h"
#include "fast_atof.h"
#include "coreutil.h"
#include "IGUIEnvironment.h"
#include "IReadFile.h"
#include "IVideoDriver.h"
#include "IGUISpriteBank.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIFont::CGUIFont(IGUIEnvironment *env, const io::path& filename)
: Driver(0), SpriteBank(0), Environment(env), WrongCharacter(0),
	MaxHeight(0), GlobalKerningWidth(0), GlobalKerningHeight(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIFont");
	#endif

	if (Environment)
	{
		// don't grab environment, to avoid circular references
		Driver = Environment->getVideoDriver();

		SpriteBank = Environment->getSpriteBank(filename);
		if (!SpriteBank)	// could be default-font which has no file
			SpriteBank = Environment->addEmptySpriteBank(filename);
		if (SpriteBank)
			SpriteBank->grab();
	}

	if (Driver)
		Driver->grab();

	setInvisibleCharacters ( L" " );
}


//! destructor
CGUIFont::~CGUIFont()
{
	if (Driver)
		Driver->drop();

	if (SpriteBank)
	{
		SpriteBank->drop();
		// TODO: spritebank still exists in gui-environment and should be removed here when it's
		// reference-count is 1. Just can't do that from here at the moment.
		// But spritebank would not be able to drop textures anyway because those are in texture-cache
		// where they can't be removed unless materials start reference-couting 'em.
	}
}


//! loads a font file from tsv
bool CGUIFont::load(io::IReadFile* file, const io::path& directory)
{
	if (!SpriteBank)
		return false;

	SpriteBank->clear();

	const long size = file->getSize();
	core::stringc Buffer;
	Buffer.reserve(size + 1);
	if (file->read(&Buffer[0], size) != static_cast<size_t>(size))
	{
		os::Printer::log("Could not read from file", ELL_ERROR);
		return false;
	}
	Buffer[size] = 0;
	const c8 *P = &Buffer[0], *End = P + size;

#define SkipSpace() \
		while (P < End && (*P == ' ' || *P == '\t' || *P == '\v')) { P++; }
#define SkipLine(rest) \
		while (rest && P < End && *P != '\r' && *P != '\n') P++; \
		while (P < End && (*P == '\r' || *P == '\n')) { P++; }

	while (P < End)
	{
		if (!strncmp(P, "Texture:", 8))
		{
			// add a texture
			P += 8;
			SkipSpace()
			core::stringc fn;
			for (; P < End && *P != '\r' && *P != '\n'; P++)
				fn.append(*P);
			SkipLine(0)

			bool flags[3];
			pushTextureCreationFlags(flags);

			// load texture
			io::path textureFullName = core::mergeFilename(directory, fn);
			SpriteBank->setTexture(0, Driver->getTexture(textureFullName));

			popTextureCreationFlags(flags);

			if (!SpriteBank->getTexture(0))
			{
				os::Printer::log("Unable to load texture in the font, aborting", ELL_ERROR);
				return false;
			}
		}
		else
		{
			// adding a character to this font
			SFontArea a;
			SGUISpriteFrame f;
			SGUISprite s;
			core::rect<s32> rect;
			wchar_t ch;

			// Format of a line:
			// <char (hex)> <X1> <Y1> <X2> <Y2> [u] [o]

			ch = static_cast<wchar_t>(core::strtoul16(P, &P));
			SkipSpace()

			rect.UpperLeftCorner.X = core::strtol10(P, &P);
			SkipSpace()
			rect.UpperLeftCorner.Y = core::strtol10(P, &P);
			SkipSpace()
			rect.LowerRightCorner.X = core::strtol10(P, &P);
			SkipSpace()
			rect.LowerRightCorner.Y = core::strtol10(P, &P);
			SkipSpace()

			if (core::isdigit(*P))
			{
				a.underhang = core::strtol10(P, &P);
				SkipSpace()
				if (core::isdigit(*P))
					a.overhang = core::strtol10(P, &P);
			}

			SkipLine(1)

			CharacterMap.insert(ch, Areas.size());

			// make frame
			f.rectNumber = SpriteBank->getPositions().size();
			f.textureNumber = 0;

			// add frame to sprite
			s.Frames.push_back(f);
			s.frameTime = 0;

			// add rectangle to sprite bank
			SpriteBank->getPositions().push_back(rect);
			a.width = rect.getWidth();
			a.spriteno = SpriteBank->getSprites().size();

			// add sprite to sprite bank
			SpriteBank->getSprites().push_back(s);

			// add character to font
			Areas.push_back(a);
		}
	}

#undef SkipSpace
#undef SkipLine

	// set bad character
	WrongCharacter = getAreaFromCharacter(L' ');

	setMaxHeight();

	return true;
}


void CGUIFont::setMaxHeight()
{
	if ( !SpriteBank )
		return;

	MaxHeight = 0;

	core::array< core::rect<s32> >& p = SpriteBank->getPositions();

	for (u32 i=0; i<p.size(); ++i)
	{
		const s32 t = p[i].getHeight();
		if (t>MaxHeight)
			MaxHeight = t;
	}
}

void CGUIFont::pushTextureCreationFlags(bool(&flags)[3])
{
	flags[0] = Driver->getTextureCreationFlag(video::ETCF_ALLOW_NON_POWER_2);
	flags[1] = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
	flags[2] = Driver->getTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY);

	Driver->setTextureCreationFlag(video::ETCF_ALLOW_NON_POWER_2, true);
	Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	Driver->setTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY, true);
}

void CGUIFont::popTextureCreationFlags(const bool(&flags)[3])
{
	Driver->setTextureCreationFlag(video::ETCF_ALLOW_NON_POWER_2, flags[0]);
	Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, flags[1]);
	Driver->setTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY, flags[2]);
}

//! loads a font file, native file needed, for texture parsing
bool CGUIFont::load(io::IReadFile* file)
{
	if (!Driver)
		return false;

	return loadTexture(Driver->createImageFromFile(file),
				file->getFileName());
}


//! loads a font file, native file needed, for texture parsing
bool CGUIFont::load(const io::path& filename)
{
	if (!Driver)
		return false;

	return loadTexture(Driver->createImageFromFile( filename ),
				filename);
}


//! load & prepare font from ITexture
bool CGUIFont::loadTexture(video::IImage* image, const io::path& name)
{
	if (!image || !SpriteBank)
		return false;

	s32 lowerRightPositions = 0;

	video::IImage* tmpImage=image;
	bool deleteTmpImage=false;
	switch(image->getColorFormat())
	{
	case video::ECF_R5G6B5:
		tmpImage =  Driver->createImage(video::ECF_A1R5G5B5,image->getDimension());
		image->copyTo(tmpImage);
		deleteTmpImage=true;
		break;
	case video::ECF_A1R5G5B5:
	case video::ECF_A8R8G8B8:
		break;
	case video::ECF_R8G8B8:
		tmpImage = Driver->createImage(video::ECF_A8R8G8B8,image->getDimension());
		image->copyTo(tmpImage);
		deleteTmpImage=true;
		break;
	default:
		os::Printer::log("Unknown texture format provided for CGUIFont::loadTexture", ELL_ERROR);
		return false;
	}
	readPositions(tmpImage, lowerRightPositions);

	WrongCharacter = getAreaFromCharacter(L' ');

	// output warnings
	if (!lowerRightPositions || !SpriteBank->getSprites().size())
		os::Printer::log("Either no upper or lower corner pixels in the font file. If this font was made using the new font tool, please load the XML file instead. If not, the font may be corrupted.", ELL_ERROR);
	else
	if (lowerRightPositions != (s32)SpriteBank->getPositions().size())
		os::Printer::log("The amount of upper corner pixels and the lower corner pixels is not equal, font file may be corrupted.", ELL_ERROR);

	bool ret = ( !SpriteBank->getSprites().empty() && lowerRightPositions );

	if ( ret )
	{
		bool flags[3];
		pushTextureCreationFlags(flags);

		SpriteBank->addTexture(Driver->addTexture(name, tmpImage));

		popTextureCreationFlags(flags);
	}
	if (deleteTmpImage)
		tmpImage->drop();
	image->drop();

	setMaxHeight();

	return ret;
}


void CGUIFont::readPositions(video::IImage* image, s32& lowerRightPositions)
{
	if (!SpriteBank )
		return;

	const core::dimension2d<u32> size = image->getDimension();

	video::SColor colorTopLeft = image->getPixel(0,0);
	colorTopLeft.setAlpha(255);
	image->setPixel(0,0,colorTopLeft);
	video::SColor colorLowerRight = image->getPixel(1,0);
	video::SColor colorBackGround = image->getPixel(2,0);
	video::SColor colorBackGroundTransparent = 0;

	image->setPixel(1,0,colorBackGround);

	// start parsing

	core::position2d<s32> pos(0,0);
	for (pos.Y=0; pos.Y<(s32)size.Height; ++pos.Y)
	{
		for (pos.X=0; pos.X<(s32)size.Width; ++pos.X)
		{
			const video::SColor c = image->getPixel(pos.X, pos.Y);
			if (c == colorTopLeft)
			{
				image->setPixel(pos.X, pos.Y, colorBackGroundTransparent);
				SpriteBank->getPositions().push_back(core::rect<s32>(pos, pos));
			}
			else
			if (c == colorLowerRight)
			{
				// too many lower right points
				if (SpriteBank->getPositions().size()<=(u32)lowerRightPositions)
				{
					lowerRightPositions = 0;
					return;
				}

				image->setPixel(pos.X, pos.Y, colorBackGroundTransparent);
				SpriteBank->getPositions()[lowerRightPositions].LowerRightCorner = pos;
				// add frame to sprite bank
				SGUISpriteFrame f;
				f.rectNumber = lowerRightPositions;
				f.textureNumber = 0;
				SGUISprite s;
				s.Frames.push_back(f);
				s.frameTime = 0;
				SpriteBank->getSprites().push_back(s);
				// add character to font
				SFontArea a;
				a.overhang = 0;
				a.underhang = 0;
				a.spriteno = lowerRightPositions;
				a.width = SpriteBank->getPositions()[lowerRightPositions].getWidth();
				Areas.push_back(a);
				// map letter to character
				wchar_t ch = (wchar_t)(lowerRightPositions + 32);
				CharacterMap.set(ch, lowerRightPositions);

				++lowerRightPositions;
			}
			else
			if (c == colorBackGround)
				image->setPixel(pos.X, pos.Y, colorBackGroundTransparent);
		}
	}
}


//! set an Pixel Offset on Drawing ( scale position on width )
void CGUIFont::setKerningWidth(s32 kerning)
{
	GlobalKerningWidth = kerning;
}


//! set an Pixel Offset on Drawing ( scale position on width )
s32 CGUIFont::getKerningWidth(const wchar_t* thisLetter, const wchar_t* previousLetter) const
{
	s32 ret = GlobalKerningWidth;

	if (thisLetter)
	{
		ret += Areas[getAreaFromCharacter(*thisLetter)].overhang;

		if (previousLetter)
		{
			ret += Areas[getAreaFromCharacter(*previousLetter)].underhang;
		}
	}

	return ret;
}


//! set an Pixel Offset on Drawing ( scale position on height )
void CGUIFont::setKerningHeight(s32 kerning)
{
	GlobalKerningHeight = kerning;
}


//! set an Pixel Offset on Drawing ( scale position on height )
s32 CGUIFont::getKerningHeight () const
{
	return GlobalKerningHeight;
}


//! returns the sprite number from a given character
u32 CGUIFont::getSpriteNoFromChar(const wchar_t *c) const
{
	return Areas[getAreaFromCharacter(*c)].spriteno;
}


s32 CGUIFont::getAreaFromCharacter(const wchar_t c) const
{
	core::map<wchar_t, s32>::Node* n = CharacterMap.find(c);
	if (n)
		return n->getValue();
	else
		return WrongCharacter;
}

void CGUIFont::setInvisibleCharacters( const wchar_t *s )
{
	Invisible = s;
}


//! returns the dimension of text
core::dimension2d<u32> CGUIFont::getDimension(const wchar_t* text) const
{
	core::dimension2d<u32> dim(0, 0);
	core::dimension2d<u32> thisLine(0, MaxHeight);

	for (const wchar_t* p = text; *p; ++p)
	{
		bool lineBreak=false;
		if (*p == L'\r') // Mac or Windows breaks
		{
			lineBreak = true;
			if (p[1] == L'\n') // Windows breaks
				++p;
		}
		else if (*p == L'\n') // Unix breaks
		{
			lineBreak = true;
		}
		if (lineBreak)
		{
			dim.Height += thisLine.Height;
			if (dim.Width < thisLine.Width)
				dim.Width = thisLine.Width;
			thisLine.Width = 0;
			continue;
		}

		const SFontArea &area = Areas[getAreaFromCharacter(*p)];

		thisLine.Width += area.underhang;
		thisLine.Width += area.width + area.overhang + GlobalKerningWidth;
	}

	dim.Height += thisLine.Height;
	if (dim.Width < thisLine.Width)
		dim.Width = thisLine.Width;

	return dim;
}

//! draws some text and clips it to the specified rectangle if wanted
void CGUIFont::draw(const core::stringw& text, const core::rect<s32>& position,
					video::SColor color,
					bool hcenter, bool vcenter, const core::rect<s32>* clip
				)
{
	if (!Driver || !SpriteBank)
		return;

	core::dimension2d<s32> textDimension;	// NOTE: don't make this u32 or the >> later on can fail when the dimension width is < position width
	core::position2d<s32> offset = position.UpperLeftCorner;

	if (hcenter || vcenter || clip)
		textDimension = getDimension(text.c_str());

	if (hcenter)
		offset.X += (position.getWidth() - textDimension.Width) >> 1;

	if (vcenter)
		offset.Y += (position.getHeight() - textDimension.Height) >> 1;

	if (clip)
	{
		core::rect<s32> clippedRect(offset, textDimension);
		clippedRect.clipAgainst(*clip);
		if (!clippedRect.isValid())
			return;
	}

	core::array<u32> indices(text.size());
	core::array<core::position2di> offsets(text.size());

	for(u32 i = 0;i < text.size();i++)
	{
		wchar_t c = text[i];

		bool lineBreak=false;
		if ( c == L'\r') // Mac or Windows breaks
		{
			lineBreak = true;
			if ( text[i + 1] == L'\n') // Windows breaks
				c = text[++i];
		}
		else if ( c == L'\n') // Unix breaks
		{
			lineBreak = true;
		}

		if (lineBreak)
		{
			offset.Y += MaxHeight;
			offset.X = position.UpperLeftCorner.X;

			if ( hcenter )
			{
				offset.X += (position.getWidth() - textDimension.Width) >> 1;
			}
			continue;
		}

		SFontArea& area = Areas[getAreaFromCharacter(c)];

		offset.X += area.underhang;
		if ( Invisible.findFirst ( c ) < 0 )
		{
			indices.push_back(area.spriteno);
			offsets.push_back(offset);
		}

		offset.X += area.width + area.overhang + GlobalKerningWidth;
	}

	SpriteBank->draw2DSpriteBatch(indices, offsets, clip, color);
}


//! Calculates the index of the character in the text which is on a specific position.
s32 CGUIFont::getCharacterFromPos(const wchar_t* text, s32 pixel_x) const
{
	s32 x = 0;
	s32 idx = 0;

	while (text[idx])
	{
		const SFontArea& a = Areas[getAreaFromCharacter(text[idx])];

		x += a.width + a.overhang + a.underhang + GlobalKerningWidth;

		if (x >= pixel_x)
			return idx;

		++idx;
	}

	return -1;
}


IGUISpriteBank* CGUIFont::getSpriteBank() const
{
	return SpriteBank;
}

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

