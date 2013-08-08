#include "CPOFMeshFileLoader.h"

#include <iostream>
#include <assert.h>

#include "ISceneManager.h"
#include "IFileSystem.h"
#include "IReadFile.h"

using namespace std;

namespace irr
{
namespace scene
{

/*
CPOFMeshFileLoader::CPOFMeshFileLoader(scene::ISceneManager* smgr, io::IFileSystem* fs)
{
	this->test();
}
*/

bool CPOFMeshFileLoader::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension(filename, "pof");
}

IAnimatedMesh* CPOFMeshFileLoader::createMesh(io::IReadFile* file)
{
	cout << "file size: " << file->getSize() << endl;

	return NULL;
}

// test, private
void CPOFMeshFileLoader::test()
{
	POF_VECTOR v;
	POF_STRING str;

	assert(sizeof(POF_INT) == 4);
	assert(sizeof(POF_UINT) == 4);
	assert(sizeof(POF_SHORT) == 2);
	assert(sizeof(POF_USHORT) == 2);
	assert(sizeof(POF_CHAR) == 1);
	assert(sizeof(POF_UBYTE) == 1);
	assert(sizeof(POF_FLOAT) == 4);
	assert(sizeof(v) == 12);

	return;
}

} // end of namespace scene
} // end of namespace irr
