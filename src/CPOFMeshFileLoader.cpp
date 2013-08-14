#include "CPOFMeshFileLoader.h"

#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "ISceneManager.h"
#include "IFileSystem.h"
#include "IReadFile.h"

using namespace std;

void pof_header_print(POFHeader *header) {
	printf(
		"file id:\t%c%c%c%c\n",
		header->file_id[0],
		header->file_id[1],
		header->file_id[2],
		header->file_id[3]
	);
	printf("file version:\t%u\n", header->version);
}

void pof_chunk_header_print(POFChunkHeader *header) {
	printf(
		"chunk id:\t%c%c%c%c\n",
		header->chunk_id[0],
		header->chunk_id[1],
		header->chunk_id[2],
		header->chunk_id[3]
	);
	printf("chunk length:\t%u\n", header->length);
}

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
	if (this->pofHeader) {
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
	long int size = 0;
	POFChunkHeader* chunk_header = NULL;

	if (this->pofHeader) delete this->pofHeader;
	this->pofHeader = new POFHeader();

	file->read(this->pofHeader, sizeof(POFHeader));
	pof_header_print(this->pofHeader);

	size = file->getSize();
	cout << "file size:\t" << size << endl;

	do {
		chunk_header = new POFChunkHeader();

		file->read(chunk_header, sizeof(POFChunkHeader));
		pof_chunk_header_print(chunk_header);
		file->seek(chunk_header->length, true);

		delete chunk_header;
	} while (file->getPos() < size);

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

	assert(sizeof(POFHeader) == 8);
	assert(sizeof(POFChunkHeader) == 8);

	return;
}

} // end of namespace scene
} // end of namespace irr
