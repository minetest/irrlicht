#include <stdio.h>
#include <irrlicht.h>

using namespace irr;

// Builds with an Irrlicht version that still has XML (e.g. 1.9.0mt2)
// g++ -Wall -O2 XMLFontConverter.cpp -o XMLFontConverter -I/tmp/irrlicht/include -L/tmp/irrlicht/lib/Linux -lIrrlichtMt

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("Usage: XMLFontConverter <source.xml> <destination.tsv>\n");
		return 1;
	}

	IrrlichtDevice *device = createDevice(video::EDT_NULL,
			core::dimension2d<u32>(800, 600), 32, false, false, false, 0);

	io::IXMLReader *xml = device->getFileSystem()->createXMLReader(argv[1]);
	io::IWriteFile *wf = device->getFileSystem()->createAndWriteFile(argv[2]);
	u32 nchars = 0;

	while (xml->read())
	{
		if (io::EXN_ELEMENT != xml->getNodeType())
			continue;
		if (core::stringw(L"Texture") == xml->getNodeName())
		{
			core::stringc fn = xml->getAttributeValue(L"filename");

			core::stringc line("Texture: ");
			line.append(fn);
			line.append('\n');
			wf->write(line.c_str(), line.size());
		}
		else if (core::stringw(L"c") == xml->getNodeName())
		{
			core::rect<s32> rect;
			s32 underhang = xml->getAttributeValueAsInt(L"u");
			s32 overhang = xml->getAttributeValueAsInt(L"o");
			core::stringc rectstr = xml->getAttributeValue(L"r");
			wchar_t ch = xml->getAttributeValue(L"c")[0];

			nchars++;

			const c8 *c = rectstr.c_str();
			s32 val = core::strtol10(c, &c);
			rect.UpperLeftCorner.X = val;
			while (*c == L' ' || *c == L',') c++;
			val = core::strtol10(c, &c);
			rect.UpperLeftCorner.Y = val;
			while (*c == L' ' || *c == L',') c++;
			val = core::strtol10(c, &c);
			rect.LowerRightCorner.X = val;
			while (*c == L' ' || *c == L',') c++;
			val = core::strtol10(c, &c);
			rect.LowerRightCorner.Y = val;

			core::stringc line;
			char tmp[6];

			snprintf(tmp, 6, "%04X", static_cast<u32>(ch));
			line.append(tmp);
			line.append('\t');

			line.append(core::stringc(rect.UpperLeftCorner.X));
			line.append('\t');
			line.append(core::stringc(rect.UpperLeftCorner.Y));
			line.append('\t');
			line.append(core::stringc(rect.LowerRightCorner.X));
			line.append('\t');
			line.append(core::stringc(rect.LowerRightCorner.Y));

			if (underhang != 0 || overhang != 0)
			{
				line.append('\t');
				line.append(core::stringc(underhang));
				if (overhang != 0)
				{
					line.append('\t');
					line.append(core::stringc(overhang));
				}
			}

			line.append('\n');
			wf->write(line.c_str(), line.size());
		}
	}

	printf("Success: %d chars\n", nchars);
	xml->drop();
	wf->drop();
	device->drop();

	return 0;
}
