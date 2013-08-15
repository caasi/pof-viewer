#include "CPOFMeshFileLoader.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ISceneManager.h"
#include "IFileSystem.h"
#include "IReadFile.h"

using namespace std;

int pof_string_read(POF_STRING *str, irr::io::IReadFile *file)
{
	int bytes_read = 0;
	
	bytes_read += file->read(&str->length, sizeof(POF_INT));
	str->text = NULL;
	if (str->length) {
		str->text = new POF_CHAR [str->length];
		bytes_read += file->read(str->text, sizeof(POF_CHAR) * str->length);
	}

	return bytes_read;
}

void pof_string_print(POF_STRING *str, unsigned int indent)
{
	INDENT(indent);
	printf("%d:", str->length);
	for (int i = 0; i < str->length; ++i) {
		putchar(str->text[i]);
	}
	putchar('\n');

	return;
}

void pof_string_clean(POF_STRING *str)
{
	if (str->text) delete [] str->text;

	return;
}

void pof_vector_print(POF_VECTOR *v, unsigned int indent)
{
	INDENT(indent);
	printf("{\n");
	INDENT(indent);
	printf("\tx: %f\n", v->x);
	INDENT(indent);
	printf("\ty: %f\n", v->y);
	INDENT(indent);
	printf("\tz: %f\n", v->z);
	INDENT(indent);
	printf("}\n");

	return;
}

void pof_header_print(POFHeader *header, unsigned int indent)
{
	char *h = (char *)&header->file_id;

	INDENT(indent);
	printf("file id:\t%c%c%c%c\n", h[0], h[1], h[2], h[3]);

	INDENT(indent);
	printf("file version:\t%u\n", header->version);
}

void pof_chunk_header_print(POFChunkHeader *header, unsigned int indent)
{
	char *h = (char *)&header->chunk_id;

	INDENT(indent);
	printf("chunk id:\t%c%c%c%c\n", h[0], h[1], h[2], h[3]);

	INDENT(indent);
	printf("chunk length:\t%u\n", header->length);
}

int pof_chunk_hdr2_build(POFObject *obj, irr::io::IReadFile *file)
{
	int i;
	int byte_read = 0;

	byte_read += file->read(obj, 40);

	obj->sobj_detail_levels = NULL;
	if (obj->num_detail_levels) {
		obj->sobj_detail_levels = new POF_INT [obj->num_detail_levels];
		for (i = 0; i < obj->num_detail_levels; ++i) {
			byte_read += file->read(&(obj->sobj_detail_levels[i]), sizeof(POF_INT));
		}
	}

	byte_read += file->read(&obj->num_debris, sizeof(POF_INT));
	obj->sobj_debris = NULL;
	if (obj->num_debris) {
		obj->sobj_debris = new POF_INT [obj->num_debris];
		for (i = 0; i < obj->num_debris; ++i) {
			byte_read += file->read(&(obj->sobj_debris[i]), sizeof(POF_INT));
		}
	}

	byte_read += file->read(&obj->mass, sizeof(POF_FLOAT));
	byte_read += file->read(&obj->mass_center, sizeof(POF_VECTOR));
	byte_read += file->read(&obj->moment_inertia, 9 * sizeof(POF_FLOAT));
	
	byte_read += file->read(&obj->num_cross_sections, sizeof(POF_INT));
	obj->cross_sections = NULL;
	if (obj->num_cross_sections != -1 && obj->num_cross_sections != 0) {
		obj->cross_sections = new POFChunkCrossSection [obj->num_cross_sections];
		for (i = 0; i < obj->num_cross_sections; ++i) {
			byte_read += file->read(&(obj->cross_sections[i]), sizeof(POFChunkCrossSection));
		}
	}

	byte_read += file->read(&obj->num_lights, sizeof(POF_INT));
	obj->lights = NULL;
	if (obj->num_lights) {
		obj->lights = new POFChunkLight [obj->num_lights];
		for (i = 0; i < obj->num_lights; ++i) {
			byte_read += file->read(&(obj->lights[i]), sizeof(POFChunkLight));
		}
	}

	return byte_read;
}

void pof_chunk_hdr2_print(POFObject *obj, unsigned int indent)
{
	INDENT(indent);
	printf("max radius:\t%f\n", obj->max_radius);

	INDENT(indent);
	printf("obj flags:\t%x\n", obj->obj_flags);

	INDENT(indent);
	printf("sobj count:\t%d\n", obj->num_subobjects);

	INDENT(indent);
	printf("min bounding:\n");
	pof_vector_print(&obj->min_bounding, indent);

	INDENT(indent);
	printf("max bounding:\n");
	pof_vector_print(&obj->max_bounding, indent);

	INDENT(indent);
	printf("detail levels:\t%d\n", obj->num_detail_levels);
	for (int i = 0; i < obj->num_detail_levels; ++i) {
		INDENT(indent);
		printf("\t%d\n", obj->sobj_detail_levels[i]);
	}

	INDENT(indent);
	printf("debris count:\t%d\n", obj->num_debris);
	for (int i = 0; i < obj->num_debris; ++i) {
		INDENT(indent);
		printf("\t%d\n", obj->sobj_debris[i]);
	}

	INDENT(indent);
	printf("mass:\t\t%f\n", obj->mass);

	INDENT(indent);
	printf("mass center:\n");
	pof_vector_print(&obj->mass_center, indent);

	INDENT(indent);
	printf("cross sections:\t%d\n", obj->num_cross_sections);
	for (int i = 0; i < obj->num_cross_sections; ++i) {
		POFChunkCrossSection *section = &(obj->cross_sections[i]);
		INDENT(indent);
		printf("\tdepth:\t%f\n", section->depth);
		INDENT(indent);
		printf("\tradius:\t\t%f\n", section->radius);
	}

	INDENT(indent);
	printf("lights:\t%d\n", obj->num_lights);
	for (int i = 0; i < obj->num_lights; ++i) {
		POFChunkLight *light = &(obj->lights[i]);
		INDENT(indent);
		printf("\ttype:\t%d\n", light->light_type);
		INDENT(indent);
		printf("\tlocation:\n");
		pof_vector_print(&light->location, indent);
	}

	return;
}

void pof_chunk_hdr2_clean(POFObject *obj)
{
	if (obj->lights) delete [] obj->lights;
	if (obj->cross_sections) delete [] obj->cross_sections;
	if (obj->sobj_debris) delete [] obj->sobj_debris;

	return;
}

int pof_chunk_obj2_build(POFSubObject *obj, irr::io::IReadFile *file)
{
	int bytes_read = 0;

	bytes_read += file->read(&obj->id, sizeof(POF_INT));
	bytes_read += file->read(&obj->radius, sizeof(POF_FLOAT));
	bytes_read += file->read(&obj->parent, sizeof(POF_INT));
	bytes_read += file->read(&obj->offset, sizeof(POF_VECTOR));
	bytes_read += file->read(&obj->geometric_center, sizeof(POF_VECTOR));
	bytes_read += file->read(&obj->min_bounding, sizeof(POF_VECTOR));
	bytes_read += file->read(&obj->max_bounding, sizeof(POF_VECTOR));
	bytes_read += pof_string_read(&obj->name, file);
	bytes_read += pof_string_read(&obj->properties, file);
	bytes_read += file->read(&obj->movement_type, sizeof(POF_INT));
	bytes_read += file->read(&obj->movement_axis, sizeof(POF_INT));
	bytes_read += file->read(&obj->reserved, sizeof(POF_INT));
	bytes_read += file->read(&obj->bsp_data_size, sizeof(POF_INT));
	obj->bsp_data = new POF_UBYTE [obj->bsp_data_size];
	bytes_read += file->read(obj->bsp_data, obj->bsp_data_size);

	return bytes_read;
}

void pof_chunk_obj2_print(POFSubObject *obj, unsigned int indent)
{
	INDENT(indent);
	printf("subobject id:\t%d\n", obj->id);

	INDENT(indent);
	printf("radius:\t%f\n", obj->radius);

	INDENT(indent);
	printf("parent id:\t%d\n", obj->parent);

	INDENT(indent);
	printf("offset:\n");
	pof_vector_print(&obj->min_bounding, indent);

	INDENT(indent);
	printf("center:\n");
	pof_vector_print(&obj->geometric_center, indent);

	INDENT(indent);
	printf("min bounding:\n");
	pof_vector_print(&obj->min_bounding, indent);

	INDENT(indent);
	printf("max bounding:\n");
	pof_vector_print(&obj->max_bounding, indent);

	INDENT(indent);
	printf("name:\n");
	pof_string_print(&obj->name, indent + 1);

	INDENT(indent);
	printf("properties:\n");
	pof_string_print(&obj->properties, indent + 1);

	INDENT(indent);
	printf("movement type:\t%d\n", obj->movement_type);

	INDENT(indent);
	printf("movement axis:\t%d\n", obj->movement_axis);

	INDENT(indent);
	printf("reserved:\t%d\n", obj->reserved);

	INDENT(indent);
	printf("bsp data size:\t%d\n", obj->bsp_data_size);

	return;
}

void pof_chunk_obj2_clean(POFSubObject *obj)
{
	pof_string_clean(&obj->name);
	pof_string_clean(&obj->properties);
	if (obj->bsp_data) delete [] obj->bsp_data;

	return;
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

// test, private
void CPOFMeshFileLoader::test()
{
	POF_VECTOR v;
	//POF_STRING str;

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
