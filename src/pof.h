#ifndef __C_POF_H_INCLUDED__
#define __C_POF_H_INCLUDED__

#include "pof_types.h"
#include "IReadFile.h"

/* chunk_id */
#define ID_POF		0x4f505350
#define ID_HDR2		0x32524448
#define ID_OBJ2		0x324a424f

/* structs */
typedef struct pof_header
{
	POF_INT file_id;	// for convenience
	POF_INT version;
} POFHeader;
typedef struct pof_chunk_header
{
	POF_INT chunk_id;
	POF_INT length;
} POFChunkHeader;
typedef struct pof_chunk_hdr2_cross_section
{
	POF_FLOAT depth;
	POF_FLOAT radius;
} POFChunkCrossSection;
typedef struct pof_chunk_hdr2_light
{
	POF_VECTOR location;
	POF_INT light_type;
} POFChunkLight;
typedef struct pof_chunk_hdr2
{
#include "pof_struct_hdr2.h"
}			POFObject;
typedef struct pof_chunk_obj2
{
#include "pof_struct_obj2.h"
}			POFSubObject;

/* debug functions in C style for those structs */
#define INDENT(num) {for (unsigned int i = 0; i < num; ++i) putchar('\t');}

int	pof_string_build(POF_STRING *str, irr::io::IReadFile *file);
void	pof_string_print(POF_STRING *str, unsigned int indent);
void	pof_string_clean(POF_STRING *str);

void	pof_vector_print(POF_VECTOR *v, unsigned int indent);

void	pof_header_print(POFHeader* header, unsigned int indent);

void	pof_chunk_header_print(POFChunkHeader* header, unsigned int indent);

int	pof_chunk_hdr2_build(POFObject *obj, irr::io::IReadFile *file);
void	pof_chunk_hdr2_print(POFObject *obj, unsigned int indent);
void	pof_chunk_hdr2_clean(POFObject *obj);

int	pof_chunk_obj2_build(POFSubObject *obj, irr::io::IReadFile *file);
void	pof_chunk_obj2_print(POFSubObject *obj, unsigned int indent);
void	pof_chunk_obj2_clean(POFSubObject *obj);

#endif
