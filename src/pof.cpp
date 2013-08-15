#include <stdio.h>

#include "pof.h"
#include "IReadFile.h"

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
	int byte_read = 0;

	byte_read += file->read(obj, 40);

	obj->sobj_detail_levels = NULL;
	if (obj->num_detail_levels) {
		obj->sobj_detail_levels = new POF_INT [obj->num_detail_levels];
		byte_read += file->read(obj->sobj_detail_levels, sizeof(POF_INT) * obj->num_detail_levels);
	}

	byte_read += file->read(&obj->num_debris, sizeof(POF_INT));
	obj->sobj_debris = NULL;
	if (obj->num_debris) {
		obj->sobj_debris = new POF_INT [obj->num_debris];
		byte_read += file->read(obj->sobj_debris, sizeof(POF_INT) * obj->num_debris);
	}

	byte_read += file->read(&obj->mass, sizeof(POF_FLOAT));
	byte_read += file->read(&obj->mass_center, sizeof(POF_VECTOR));
	byte_read += file->read(&obj->moment_inertia, 9 * sizeof(POF_FLOAT));
	
	byte_read += file->read(&obj->num_cross_sections, sizeof(POF_INT));
	obj->cross_sections = NULL;
	if (obj->num_cross_sections != -1 && obj->num_cross_sections != 0) {
		obj->cross_sections = new POFChunkCrossSection [obj->num_cross_sections];
		byte_read += file->read(obj->cross_sections, sizeof(POFChunkCrossSection) * obj->num_cross_sections);
	}

	byte_read += file->read(&obj->num_lights, sizeof(POF_INT));
	obj->lights = NULL;
	if (obj->num_lights) {
		obj->lights = new POFChunkLight [obj->num_lights];
		byte_read += file->read(obj->lights, sizeof(POFChunkLight) * obj->num_lights);
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
