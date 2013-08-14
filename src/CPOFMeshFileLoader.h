#ifndef __C_POF_MESH_FILE_LOADER_H_INCLUDED__
#define __C_POF_MESH_FILE_LOADER_H_INCLUDED__

#include <stdint.h>

#include "IMeshLoader.h"

typedef	int32_t		POF_INT;
typedef	uint32_t	POF_UINT;
typedef	int16_t		POF_SHORT;
typedef	uint16_t	POF_USHORT;
typedef	signed char	POF_CHAR;
typedef	unsigned char	POF_UBYTE;
typedef	float		POF_FLOAT;
typedef	struct pof_vector
{
	POF_FLOAT x;
	POF_FLOAT y;
	POF_FLOAT z;
}			POF_VECTOR;
typedef struct pof_string
{
	POF_INT length;
	POF_CHAR* text;	// it's not null-terminated in the POF files
}			POF_STRING;
typedef struct pof_header
{
	char file_id[4];
	int version;
}			POFHeader;

namespace irr
{
namespace io
{
	class IFileSystem;
	class IReadFile;
} // end of namespace io
namespace scene
{
class ISceneManager;

class CPOFMeshFileLoader : public IMeshLoader
{
public:
	CPOFMeshFileLoader();

	~CPOFMeshFileLoader();

	virtual bool isALoadableFileExtension(const io::path& filename) const;

	virtual IAnimatedMesh* createMesh(io::IReadFile* file);

private:
	POFHeader *pofHeader;
	void test();
};

} //end of namespace scene
} // end of namespace irr

#endif
