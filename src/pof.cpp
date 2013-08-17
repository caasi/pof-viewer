#include <stdio.h>

#include <assert.h>

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
	pof_vector_print(&obj->bounding_min, indent);

	INDENT(indent);
	printf("max bounding:\n");
	pof_vector_print(&obj->bounding_max, indent);

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
		INDENT(indent + 1);
		printf("type:\t%d\n", light->light_type);
		INDENT(indent + 1);
		printf("location:\n");
		pof_vector_print(&light->location, indent + 1);
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
	int	bytes_read = 0,
		offset = 0;

	bytes_read += file->read(&obj->id, sizeof(POF_INT));
	bytes_read += file->read(&obj->radius, sizeof(POF_FLOAT));
	bytes_read += file->read(&obj->parent, sizeof(POF_INT));
	bytes_read += file->read(&obj->offset, sizeof(POF_VECTOR));
	bytes_read += file->read(&obj->geometric_center, sizeof(POF_VECTOR));
	bytes_read += file->read(&obj->bounding_min, sizeof(POF_VECTOR));
	bytes_read += file->read(&obj->bounding_max, sizeof(POF_VECTOR));
	bytes_read += pof_string_read(&obj->name, file);
	bytes_read += pof_string_read(&obj->properties, file);
	bytes_read += file->read(&obj->movement_type, sizeof(POF_INT));
	bytes_read += file->read(&obj->movement_axis, sizeof(POF_INT));
	bytes_read += file->read(&obj->reserved, sizeof(POF_INT));
	bytes_read += file->read(&obj->bsp_data_size, sizeof(POF_INT));
	obj->bsp_data = new POF_UBYTE [obj->bsp_data_size];
	bytes_read += file->read(obj->bsp_data, obj->bsp_data_size);

	offset = 0;

	switch(pof_bsp_test(obj->bsp_data))
	{
		case BSP_EOF:
			pof_bsp_eof_print((BSPEndOfFile *)obj->bsp_data, 1);
			break;
		case BSP_DEFPOINTS:
		{
			BSPVertices *node = new BSPVertices;
			offset += pof_bsp_vertices_index(node, obj->bsp_data);
			pof_bsp_vertices_print(node, 1);
			delete node;
			break;
		}
		case BSP_FLATPOLY:
		{
			BSPFlatPolygon *node = new BSPFlatPolygon;
			offset += pof_bsp_flatpoly_index(node, obj->bsp_data + offset);
			pof_bsp_flatpoly_print(node, 1);
			delete node;
			break;
		}
		case BSP_TMAPOLY:
		{
			BSPTexturedPolygon *node = new BSPTexturedPolygon;
			offset += pof_bsp_tmappoly_index(node, obj->bsp_data + offset);
			pof_bsp_tmappoly_print(node, 1);
			delete node;
			break;
		}
		case BSP_SORTNORM:
		{
			BSPSortNormal *node = new BSPSortNormal;
			offset += pof_bsp_sortnorm_index(node, obj->bsp_data + offset);
			pof_bsp_sortnorm_print(node, 1);
			delete node;
			break;
		}
		case BSP_BOUNDBOX:
		{
			BSPBoundBox *node = new BSPBoundBox;
			offset += pof_bsp_boundbox_index(node, obj->bsp_data + offset);
			pof_bsp_boundbox_print(node, 1);
			delete node;
			break;
		}
		default:
			assert(false);
	}

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
	pof_vector_print(&obj->bounding_min, indent);

	INDENT(indent);
	printf("center:\n");
	pof_vector_print(&obj->geometric_center, indent);

	INDENT(indent);
	printf("min bounding:\n");
	pof_vector_print(&obj->bounding_min, indent);

	INDENT(indent);
	printf("max bounding:\n");
	pof_vector_print(&obj->bounding_max, indent);

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

BSP_ID pof_bsp_test(void *buffer)
{
	// return the id of current bsp data id
	return *(BSP_ID *)buffer;
}

void pof_bsp_eof_print(BSPEndOfFile *obj, unsigned int indent)
{
	INDENT(indent);
	printf("bsp id:\t%d\n", obj->id);

	INDENT(indent);
	printf("size:\t%d\n", obj->size);

	return;
}

int pof_bsp_vertex_index (BSPVertex *obj, POF_INT length, const void *buffer)
{
	int offset = 0;

	obj->point = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	obj->normals = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR) * length;

	return offset;
}

void pof_bsp_vertex_print (BSPVertex *obj, POF_INT length, unsigned int indent)
{
	INDENT(indent);
	printf("point:\n");
	pof_vector_print(obj->point, indent);

	INDENT(indent);
	printf("normals:\n");
	for (int i = 0; i < length; ++i) {
		pof_vector_print(&(obj->normals[i]), indent);
	}

	return;
}

int pof_bsp_vertices_index (BSPVertices *obj, const void *buffer)
{
	int offset = 0;

	obj->id = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->size = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->n_verts = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->n_norms = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->offset = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->norm_counts = (POF_CHAR *)(buffer + offset);
	/* important! it's not += obj->offset */
	offset = *obj->offset;

	obj->vertex_data = new BSPVertex* [*obj->n_verts];

	for (int i = 0; i < *obj->n_verts; ++i) {
		BSPVertex *vert = new BSPVertex;
		obj->vertex_data[i] = vert;
		offset += pof_bsp_vertex_index(vert, obj->norm_counts[i], (buffer + offset));
	}

	return offset;
}

void pof_bsp_vertices_print (BSPVertices *obj, unsigned int indent)
{
	INDENT(indent);
	printf("bsp id:\t%d\n", *obj->id);

	INDENT(indent);
	printf("size:\t%d\n", *obj->size);

	INDENT(indent);
	printf("n_verts:\t%d\n", *obj->n_verts);

	INDENT(indent);
	printf("n_norms:\t%d\n", *obj->n_norms);

	INDENT(indent);
	printf("offset:\t%d\n", *obj->offset);

	INDENT(indent);
	printf("norm_counts:\n");
	for (int i = 0; i < *obj->n_verts; ++i) {
		printf("%u ", obj->norm_counts[i]);
	}
	putchar('\n');

	INDENT(indent);
	printf("vertices data:\n");
	for (int i = 0; i < *obj->n_verts; ++i) {
		pof_bsp_vertex_print(obj->vertex_data[i], obj->norm_counts[i], indent + 1);
	}

	return;
}

void pof_bsp_vertices_clean (BSPVertices *obj)
{
	if (obj->vertex_data) {
		for (int i = 0; i < *obj->n_verts; ++i) {
			if (obj->vertex_data[i]) {
				delete obj->vertex_data[i];
				obj->vertex_data[i] = NULL;
			}
		}
		delete [] obj->vertex_data;
	}

	return;
}

void pof_bsp_flatvertex_print (BSPFlatVertex *obj, unsigned int indent)
{
	INDENT(indent);
	printf("vert num:\t%d\n", *obj->vert_num);

	INDENT(indent);
	printf("norm num:\t%d\n", *obj->norm_num);

	return;
}

int pof_bsp_flatpoly_index (BSPFlatPolygon *obj, const void *buffer)
{
	int offset = 0;

	obj->id = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->size = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->normal = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	obj->center = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	obj->radius = (POF_FLOAT *)(buffer + offset);
	offset += sizeof(POF_FLOAT);

	obj->n_verts = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->red = (POF_UBYTE *)(buffer + offset);
	offset += sizeof(POF_UBYTE);

	obj->green = (POF_UBYTE *)(buffer + offset);
	offset += sizeof(POF_UBYTE);

	obj->blue = (POF_UBYTE *)(buffer + offset);
	offset += sizeof(POF_UBYTE);

	obj->vertex_data = (BSPFlatVertex *)(buffer + offset);
	offset += sizeof(BSPFlatVertex) * *obj->n_verts;

	return offset;
}

void pof_bsp_flatpoly_print (BSPFlatPolygon *obj, unsigned int indent)
{
	INDENT(indent);
	printf("bsp id:\t%d\n", *obj->id);

	INDENT(indent);
	printf("size:\t%d\n", *obj->size);

	INDENT(indent);
	printf("normal:\n");
	pof_vector_print(obj->normal, indent);

	INDENT(indent);
	printf("center:\n");
	pof_vector_print(obj->center, indent);

	INDENT(indent);
	printf("radius:\t%f\n", *obj->radius);

	INDENT(indent);
	printf("n_verts:\t%d\n", *obj->n_verts);

	INDENT(indent);
	printf("rga:\t\t%x, %x, %x\n", *obj->red, *obj->green, *obj->blue);

	INDENT(indent);
	printf("vertices data:\n");
	for (int i = 0; i < *obj->n_verts; ++i) {
		pof_bsp_flatvertex_print(&(obj->vertex_data[i]), indent + 1);
	}

	return;
}

void pof_bsp_tmapvertex_print (BSPTexturedVertex *obj, unsigned int indent)
{
	INDENT(indent);
	printf("vert_num:\t%u\n", *obj->vert_num);

	INDENT(indent);
	printf("norm_num:\t%u\n", *obj->norm_num);

	INDENT(indent);
	printf("uv:\t\t%f, %f\n", *obj->u, *obj->v);

	return;
}

int pof_bsp_tmappoly_index (BSPTexturedPolygon *obj, const void *buffer)
{
	int offset = 0;

	obj->id = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->size = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->normal = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	obj->center = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	obj->radius = (POF_FLOAT *)(buffer + offset);
	offset += sizeof(POF_FLOAT);

	obj->n_verts = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->tmap_num = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->vertex_data = (BSPTexturedVertex *)(buffer + offset);
	offset += sizeof(BSPTexturedVertex) * *obj->n_verts;

	return offset;
}

void pof_bsp_tmappoly_print (BSPTexturedPolygon *obj, unsigned int indent)
{
	INDENT(indent);
	printf("bsp id:\t%d\n", *obj->id);

	INDENT(indent);
	printf("size:\t%d\n", *obj->size);

	INDENT(indent);
	printf("normal:\n");
	pof_vector_print(obj->normal, indent);

	INDENT(indent);
	printf("center:\n");
	pof_vector_print(obj->center, indent);

	INDENT(indent);
	printf("radius:\t%f\n", *obj->radius);

	INDENT(indent);
	printf("n_verts:\t%d\n", *obj->n_verts);

	INDENT(indent);
	printf("tmap_num:\t%d\n", *obj->tmap_num);

	INDENT(indent);
	printf("vertices data:\n");
	for (int i = 0; i < *obj->n_verts; ++i) {
		pof_bsp_tmapvertex_print(&(obj->vertex_data[i]), indent + 1);
	}
	return;
}

int pof_bsp_sortnorm_index (BSPSortNormal *obj, const void *buffer)
{
	int offset = 0;

	obj->id = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->size = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->plane_normal = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	obj->plane_point = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	obj->reserved = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->offset_front = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->offset_back = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->offset_prelist = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->offset_postlist = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);
	
	obj->offset_online = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->bounding_min = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	obj->bounding_max = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	return offset;
}

void pof_bsp_sortnorm_print (BSPSortNormal *obj, unsigned int indent)
{
	INDENT(indent);
	printf("bsp id:\t%d\n", *obj->id);

	INDENT(indent);
	printf("size:\t%d\n", *obj->size);

	INDENT(indent);
	printf("plane normal:\n");
	pof_vector_print(obj->plane_normal, indent);

	INDENT(indent);
	printf("plane point:\n");
	pof_vector_print(obj->plane_point, indent);

	INDENT(indent);
	printf("reserved:\t%d\n", *obj->reserved);

	INDENT(indent);
	printf("offset front:\t%d\n", *obj->offset_front);

	INDENT(indent);
	printf("offset back:\t%d\n", *obj->offset_back);

	INDENT(indent);
	printf("offset prelist:\t%d\n", *obj->offset_prelist);

	INDENT(indent);
	printf("offset postlist:\t%d\n", *obj->offset_postlist);

	INDENT(indent);
	printf("min bounding:\n");
	pof_vector_print(obj->bounding_min, indent);

	INDENT(indent);
	printf("max bounding:\n");
	pof_vector_print(obj->bounding_max, indent);

	return;
}

int pof_bsp_boundbox_index (BSPBoundBox *obj, const void *buffer)
{
	int offset = 0;

	obj->id = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->size = (POF_INT *)(buffer + offset);
	offset += sizeof(POF_INT);

	obj->min = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	obj->max = (POF_VECTOR *)(buffer + offset);
	offset += sizeof(POF_VECTOR);

	return offset;
}

void pof_bsp_boundbox_print (BSPBoundBox *obj, unsigned int indent)
{
	INDENT(indent);
	printf("bsp id:\t%d\n", *obj->id);

	INDENT(indent);
	printf("size:\t%d\n", *obj->size);

	INDENT(indent);
	printf("min:\n");
	pof_vector_print(obj->min, indent);

	INDENT(indent);
	printf("max:\n");
	pof_vector_print(obj->max, indent);

	return;
}

