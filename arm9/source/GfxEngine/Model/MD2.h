#include "../3D.h"

#define MD2_Count 62

//------------------------------Types

typedef struct {
    int ident;   /* magic number: "IDP2" */
    int version; /* version: must be 8 */

    int skinwidth;  /* texture width */
    int skinheight; /* texture height */

    int framesize; /* size in bytes of a frame */

    int num_skins;    /* number of skins */
    int num_vertices; /* number of vertices per frame */
    int num_st;       /* number of texture coordinates */
    int num_tris;     /* number of triangles */
    int num_glcmds;   /* number of opengl commands */
    int num_frames;   /* number of frames */

    int offset_skins;  /* offset skin data */
    int offset_st;     /* offset texture coordinate data */
    int offset_tris;   /* offset triangle data */
    int offset_frames; /* offset frame data */
    int offset_glcmds; /* offset OpenGL command data */
    int offset_end;    /* offset end of file */

} md2_header_t;

typedef float vec3_t[3];

typedef struct {
    unsigned char v[3];        /* position */
    unsigned char normalIndex; /* normal vector index */
} md2_vertex_t;

typedef struct {
    vec3_t scale;        /* scale factor */
    vec3_t translate;    /* translation vector */
    char name[16];       /* frame name */
    md2_vertex_t *verts; /* list of frame's vertices */

} md2_frame_t;

typedef struct {
    unsigned short vertex[3]; /* vertex indices of the triangle */
    unsigned short st[3];     /* tex. coord. indices */

} md2_triangle_t;

typedef struct {
    short s;
    short t;

} md2_texCoord_t;

//----------better structs
typedef v16 nds_vec3_t[3];

typedef struct {
    nds_vec3_t v; /* uncompressed vertex (translated & scaled) */
    u8 colorindex;
} nds_vertex_t;

typedef struct {
    char name[16];       /* frame name */
    nds_vertex_t *verts; /* list of frame's vertices */

} nds_frame_t;

typedef struct {
    unsigned short vertex[3]; /* vertex indices of the triangle */
    unsigned short st[3];     /* tex. coord. indices */

} nds_triangle_t;

typedef struct {
    t16 s;
    t16 t;
} nds_texCoord_t;

typedef struct {
    md2_header_t header;

    // Those will disapear and just hold values before converting
    md2_texCoord_t *texcoords;
    md2_triangle_t *triangles;
    md2_frame_t *frames;

    // Those will hold new values which will be rendered fast
    nds_texCoord_t *flaechenkoords;
    nds_triangle_t *dreiecke;
    nds_frame_t *rahmen;

    // holds an 2d image for collision
    u32 Image[21];
} MD2Entity;
