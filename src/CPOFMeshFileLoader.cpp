#include "CPOFMeshFileLoader.h"

#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "ISceneManager.h"
#include "IFileSystem.h"
#include "IReadFile.h"

using namespace std;

namespace irr
{
namespace scene
{

CPOFMeshFileLoader::CPOFMeshFileLoader()
{
	this->pofHeader = NULL;
}

CPOFMeshFileLoader::~CPOFMeshFileLoader()
{
	if (this->pofHeader == NULL) {
		delete this->pofHeader;
		this->pofHeader = NULL;
	}
}

bool CPOFMeshFileLoader::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension(filename, "pof");
}

IAnimatedMesh* CPOFMeshFileLoader::createMesh(io::IReadFile* file)
{
	if (this->pofHeader) {
		delete this->pofHeader;
		cout << "clean up prev file" << endl;
	}

	this->pofHeader = new POFHeader();

	file->read(this->pofHeader, 8);

	cout << "file size:\t" << file->getSize() << endl;

	this->test();

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

	printf(
		"file id:\t%c%c%c%c\n",
		this->pofHeader->file_id[0],
		this->pofHeader->file_id[1],
		this->pofHeader->file_id[2],
		this->pofHeader->file_id[3]
	);
	cout << "file version:\t" << this->pofHeader->version << endl;

	return;
}

} // end of namespace scene
} // end of namespace irr
