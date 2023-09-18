#include <irrlicht.h>
#include <iostream>

using namespace irr;

int main(int argc, char *argv[])
{
	if (argc < 2)
		return 1;

	SIrrlichtCreationParameters p;
	p.DriverType = video::EDT_NULL;
	p.WindowSize = core::dimension2du(640, 480);
	p.LoggingLevel = ELL_DEBUG;

	auto *device = createDeviceEx(p);
	if (!device)
		return 1;
	auto *smgr = device->getSceneManager();

	while (__AFL_LOOP(10000)) {
		auto *mfile = device->getFileSystem()->createAndOpenFile(argv[1]);
		if (!mfile)
			continue;
		scene::IAnimatedMesh *mesh;
		// Irrlicht matches on file extension so we have to do this by hand
		for (u32 i = 0; i < smgr->getMeshLoaderCount(); i++) {
			mfile->seek(0);
			mesh = smgr->getMeshLoader(i)->createMesh(mfile);
			if (mesh) {
				core::stringc msg("Loaded using loader #");
				msg += core::stringc(i);
				device->getLogger()->log(msg.c_str(), ELL_DEBUG);
				break;
			}
		}
		if (mesh)
			mesh->drop();
		mfile->drop();
	}

	device->drop();
	return 0;
}
