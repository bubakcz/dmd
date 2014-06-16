#ifndef _BLENDER_VERTEX_STRUCT_H_INCLUDED
#define _BLENDER_VERTEX_STRUCT_H_INCLUDED

struct vertex_struct {
	float x,y,z;
	float nx,ny,nz;
	float u,v;
};

struct vertex_struct_v2 {
	float x,y,z;
	float nx,ny,nz;
	float u,v;
	float flag_x, flag_y;
};

struct vertex_struct_v3 {
	float x,y,z;
	float nx,ny,nz;
	float u,v;
	float flag_x, flag_y;
	int edge;
};

#endif _BLENDER_VERTEX_STRUCT_H_INCLUDED