#include "CPOFMeshFileLoader.h"

#include <iostream> 

#include "pof.h"
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
	int offset = 0;
	POF_UBYTE *buffer = NULL;

	if (this->pofHeader) delete this->pofHeader;
	this->pofHeader = new POFHeader();

	file->read(this->pofHeader, sizeof(POFHeader));
	pof_header_print(this->pofHeader, 0);

	size = file->getSize();
	cout << "file size:\t" << size << endl;

	buffer = new POF_UBYTE [size];
	file->read(buffer, size);

	do {
		POFChunkHeader* chunk_header = (POFChunkHeader *)(buffer + offset);
		offset += sizeof(POFChunkHeader);

		if (chunk_header->chunk_id == ID_HDR2) {
			pof_chunk_header_print(chunk_header, 0);
			
			POFObject *pof_obj = new POFObject;

			pof_chunk_hdr2_index(pof_obj, buffer + offset);
			pof_chunk_hdr2_print(pof_obj, 0);

			delete pof_obj;
		} else if (chunk_header->chunk_id == ID_OBJ2) {
			pof_chunk_header_print(chunk_header, 0);

			POFSubObject *sobj = new POFSubObject;

			pof_chunk_obj2_index(sobj, buffer + offset);
			pof_chunk_obj2_print(sobj, 0);

			delete sobj;
		}

		offset += chunk_header->length;
	} while (offset < size);

	if (buffer) delete [] buffer;

	return NULL;
}

} // end of namespace scene
} // end of namespace irr
