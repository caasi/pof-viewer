#ifndef __C_POF_MESH_FILE_LOADER_H_INCLUDED__
#define __C_POF_MESH_FILE_LOADER_H_INCLUDED__

#include <stdint.h>

#include "IMeshLoader.h"

/* basic types */
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

/* chunk_id */
#define ID_POF		0x4f505350
#define ID_HDR2		0x32524448
#define ID_OBJ2		0x324a424f

/* structs */
typedef struct pof_header
{
	POF_INT file_id;	// for convenience
	POF_INT version;
}			POFHeader;
typedef struct pof_chunk_header
{
	POF_INT chunk_id;
	POF_INT length;
}			POFChunkHeader;
typedef struct pof_chunk_hdr2
{
	POF_FLOAT max_radius;
	POF_INT obj_flags;
	POF_INT num_subobjects;

	POF_VECTOR min_bounding;
	POF_VECTOR max_bounding;

	POF_INT num_detail_levels;
	POF_INT sobj_detaillevels[];

	POF_INT num_debris;
	POF_INT sobj_debris[];

	POF_FLOAT mass;
	POF_FLOAT mass_center;
	POF_FLOAT moment_intetia;

	POF_INT num_cross_sections;
	POF_FLOAT depth[];
	POF_FLOAT radius;

	POF_INT num_lights;
	POF_VECTOR location[];
	POF_INT lighttype;
}			POFObject;
/* debug functions in C style for those structs */
void pof_header_print(POFHeader* header);
void pof_chunk_header_print(POFChunkHeader* header);

/* begin */
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
