#include "CPOFMeshFileLoader.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ISceneManager.h"
#include "IFileSystem.h"
#include "IReadFile.h"

using namespace std;

void pof_header_print(POFHeader *header)
{
	char *h = (char *)&header->file_id;
	printf("file id:\t%c%c%c%c\n", h[0], h[1], h[2], h[3]);
	printf("file version:\t%u\n", header->version);
}

void pof_chunk_header_print(POFChunkHeader *header)
{
	char *h = (char *)&header->chunk_id;
	printf("chunk id:\t%c%c%c%c\n", h[0], h[1], h[2], h[3]);
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

void pof_chunk_hdr2_print(POFObject *obj)
{
	printf("max radius:\t%f\n", obj->max_radius);
	printf("obj flags:\t%x\n", obj->obj_flags);
	printf("sobj count:\t%d\n", obj->num_subobjects);
	printf(
		"min bounding:\t%f, %f, %f\n",
		obj->min_bounding.x,
		obj->min_bounding.y,
		obj->min_bounding.z
	);
	printf(
		"max bounding:\t%f, %f, %f\n",
		obj->max_bounding.x,
		obj->max_bounding.y,
		obj->max_bounding.z
	);
	printf("detail levels:\t%d\n", obj->num_detail_levels);
	for (int i = 0; i < obj->num_detail_levels; ++i) {
		printf("\t%d\n", obj->sobj_detail_levels[i]);
	}
	printf("debris count:\t%d\n", obj->num_debris);
	for (int i = 0; i < obj->num_debris; ++i) {
		printf("\t%d\n", obj->sobj_debris[i]);
	}
	printf("mass:\t\t%f\n", obj->mass);
	printf(
		"mass center:\t%f, %f, %f\n",
		obj->mass_center.x,
		obj->mass_center.y,
		obj->mass_center.z
	);
	printf("cross sections:\t%d\n", obj->num_cross_sections);
	for (int i = 0; i < obj->num_cross_sections; ++i) {
		POFChunkCrossSection *section = &(obj->cross_sections[i]);
		printf("\tdepth:\t%f\n", section->depth);
		printf("\tradius:\t%f\n", section->radius);
	}
	printf("lights:\t%d\n", obj->num_lights);
	for (int i = 0; i < obj->num_lights; ++i) {
		POFChunkLight *light = &(obj->lights[i]);
		printf("\ttype:\t%d\n", light->light_type);
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
	int byte_read = 0;
	POFChunkHeader* chunk_header = NULL;
	POFObject* pof_obj = NULL;

	if (this->pofHeader) delete this->pofHeader;
	this->pofHeader = new POFHeader();

	file->read(this->pofHeader, sizeof(POFHeader));
	pof_header_print(this->pofHeader);

	size = file->getSize();
	cout << "file size:\t" << size << endl;

	do {
		byte_read = 0;

		chunk_header = new POFChunkHeader();

		file->read(chunk_header, sizeof(POFChunkHeader));
	
		if (chunk_header->chunk_id == ID_HDR2) {
			pof_chunk_header_print(chunk_header);
			
			pof_obj = new POFObject();
			byte_read = pof_chunk_hdr2_build(pof_obj, file);
			pof_chunk_hdr2_print(pof_obj);
			pof_chunk_hdr2_clean(pof_obj);
			delete pof_obj;
		} else if (chunk_header->chunk_id == ID_OBJ2) {
			// do nothing
		}

		file->seek(chunk_header->length - byte_read, true);

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
