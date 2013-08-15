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
typedef struct pof_chunk_hdr2_cross_section
{
	POF_FLOAT depth;
	POF_FLOAT radius;
}			POFChunkCrossSection;
typedef struct pof_chunk_hdr2_light
{
	POF_VECTOR location;
	POF_INT light_type;
}			POFChunkLight;
typedef struct pof_chunk_hdr2
{
	POF_FLOAT max_radius;
	POF_INT obj_flags;
	POF_INT num_subobjects;

	POF_VECTOR min_bounding;
	POF_VECTOR max_bounding;

	POF_INT num_detail_levels;
	POF_INT *sobj_detail_levels;

	POF_INT num_debris;
	POF_INT *sobj_debris;

	POF_FLOAT mass;
	POF_VECTOR mass_center;
	POF_FLOAT moment_inertia[3][3];

	POF_INT num_cross_sections;
	POFChunkCrossSection *cross_sections;

	POF_INT num_lights;
	POFChunkLight *lights;
}			POFObject;
typedef struct pof_chunk_obj2
{
	POF_INT id;
	POF_FLOAT radius;
	POF_INT parent;
	POF_VECTOR offset;

	POF_VECTOR geometric_center;
	POF_VECTOR min_bounding;
	POF_VECTOR max_bounding;

	POF_STRING name;
	POF_STRING properties;
	POF_INT movement_type;
	POF_INT movement_axis;

	POF_INT reserved;
	POF_INT bsp_data_size;
	POF_UBYTE *bsp_data;
}			POFSubObject;
/* debug functions in C style for those structs */
#define INDENT(num) {for (unsigned int i = 0; i < num; ++i) putchar('\t');}

int pof_string_build(POF_STRING *str, irr::io::IReadFile *file);
void pof_string_print(POF_STRING *str, unsigned int indent);
void pof_string_clean(POF_STRING *str);

void pof_vector_print(POF_VECTOR *v, unsigned int indent);

void pof_header_print(POFHeader* header, unsigned int indent);

void pof_chunk_header_print(POFChunkHeader* header);

int pof_chunk_hdr2_build(POFObject *obj, irr::io::IReadFile *file);
void pof_chunk_hdr2_print(POFObject *obj, unsigned int indent);
void pof_chunk_hdr2_clean(POFObject *obj);

int pof_chunk_obj2_build(POFSubObject *obj, irr::io::IReadFile *file);
void pof_chunk_obj2_print(POFSubObject *obj, unsigned int indent);
void pof_chunk_obj2_clean(POFSubObject *obj);

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
