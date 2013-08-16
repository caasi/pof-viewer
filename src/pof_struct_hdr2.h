POF_FLOAT max_radius;
POF_INT obj_flags;
POF_INT num_subobjects;

POF_VECTOR bounding_min;
POF_VECTOR bounding_max;

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
