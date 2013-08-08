#include <irrlicht.h>
#ifdef _IRR_WINDOWS_
#include <windows.h>
#endif

#include <stdio.h>

#include "CPOFMeshFileLoader.h"

using namespace irr;

#ifdef _WIN32

#pragma comment(lib, "Irrlicht.lib")
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char* argv[])
#endif
{
	scene::CPOFMeshFileLoader pof_loader(NULL, NULL);

	IrrlichtDevice *device;

	device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640, 480), 32);
	if (!device) return 1;
	device->setWindowCaption(L"POF Viewer");

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	while(device->run())
	{
		driver->beginScene(true, true, video::SColor(255, 22, 22, 29));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();
	}

	device->drop();

	return 0;
}
