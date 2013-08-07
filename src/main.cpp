#include <irrlicht.h>
#ifdef _IRR_WINDOWS_
#include <windows.h>
#endif

#include <stdio.h>

using namespace irr;

#ifdef _WIN32

#pragma comment(lib, "Irrlicht.lib")
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char* argv[])
#endif
{
	printf("hello, world\n");

	return 0;
}
