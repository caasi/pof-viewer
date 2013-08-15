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
	int bytes_read = 0;

	if (this->pofHeader) delete this->pofHeader;
	this->pofHeader = new POFHeader();

	file->read(this->pofHeader, sizeof(POFHeader));
	pof_header_print(this->pofHeader, 0);

	size = file->getSize();
	cout << "file size:\t" << size << endl;

	do {
		bytes_read = 0;

		POFChunkHeader* chunk_header = NULL;
		chunk_header = new POFChunkHeader;

		file->read(chunk_header, sizeof(POFChunkHeader));
	
		if (chunk_header->chunk_id == ID_HDR2) {
			pof_chunk_header_print(chunk_header, 0);
			
			POFObject *pof_obj = new POFObject;

			bytes_read = pof_chunk_hdr2_build(pof_obj, file);
			pof_chunk_hdr2_print(pof_obj, 0);
			pof_chunk_hdr2_clean(pof_obj);

			delete pof_obj;
		} else if (chunk_header->chunk_id == ID_OBJ2) {
			pof_chunk_header_print(chunk_header, 0);

			POFSubObject *sobj = new POFSubObject;

			bytes_read = pof_chunk_obj2_build(sobj, file);
			pof_chunk_obj2_print(sobj, 0);
			pof_chunk_obj2_clean(sobj);

			delete sobj;
		}

		file->seek(chunk_header->length - bytes_read, true);

		delete chunk_header;
	} while (file->getPos() < size);

	return NULL;
}

} // end of namespace scene
} // end of namespace irr
