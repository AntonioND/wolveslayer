#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/Model/MD2Collision.h"
#include "GfxEngine/Model/MD2Format.h"
#include "GfxEngine/Model/MD2Models.h"

// Table of precalculated normals
static vec3_t anorms_table[162] = {
#include "Anorms.h"
};

// The table should be copied and ocnverted to here
static v16 anormtable16[162][3];

static const v16 zero = floattov16(0.0f);

void InitTableOfNormal(void)
{
    for (int a = 0; a < 162; a++) {
        anormtable16[a][0] = floattov16(anorms_table[a][0]);
        anormtable16[a][1] = floattov16(anorms_table[a][1]);
        anormtable16[a][2] = floattov16(anorms_table[a][2]);
    }
}

void FreeModels(void)
{
    for (int a = 0; a < MD2_Max; a++) {
        if (Models[a].Enabled == true) {
            for (int N = 0; N < Models[a].header.num_frames; ++N)
                free(Models[a].rahmen[N].verts);
            free(Models[a].rahmen);
            free(Models[a].flaechenkoords);
            free(Models[a].dreiecke);
            Models[a].Enabled = false;
        }
    }
}

void LoadMD2Model(char Filename[], int num, int widthheight, int scale)
{
    if ((num < 0) || (num >= MD2_Max))
        Crash("Invalid number:%d\n%s", num, __func__);

    MD2Entity *m = &Models[num];

    // If the last character of the name is m, reverse the texture. This is done
    // to tell apart models with the extension md2 and mdm.
    bool texturereverse = true;
    if (Filename[strlen(Filename) - 1] == 'm')
        texturereverse = false;

    u8 *md2_buffer = (u8 *)LoadFile(Filename, NULL);

    // Read header
    memcpy(&m->header, md2_buffer, sizeof(md2_header_t));

    if ((m->header.ident != 844121161) || (m->header.version != 8))
        Crash("MD2: Bad version:\n%s", Filename);

    // memory allocation
    m->texcoords = (md2_texCoord_t *)malloc(sizeof(md2_texCoord_t) * m->header.num_st);
    m->triangles = (md2_triangle_t *)malloc(sizeof(md2_triangle_t) * m->header.num_tris);
    m->frames    = (md2_frame_t *)malloc(sizeof(md2_frame_t) * m->header.num_frames);

    if ((m->texcoords == NULL) || (m->triangles == NULL) || (m->frames == NULL))
        Crash("MD2: Not enough RAM (1):\n%s", Filename);

    // Read float textcoords
    memcpy(m->texcoords, md2_buffer + m->header.offset_st, sizeof(md2_texCoord_t) * m->header.num_st);

    // read that float triangles
    memcpy(m->triangles, md2_buffer + m->header.offset_tris, sizeof(md2_triangle_t) * m->header.num_tris);

    // read frames
    u8 *ptr = md2_buffer + m->header.offset_frames;

    for (int i = 0; i < m->header.num_frames; ++i) {
        // memory allocation for vertices of this frame
        m->frames[i].verts = (md2_vertex_t *)malloc(sizeof(md2_vertex_t) * m->header.num_vertices);
        if (m->frames[i].verts == NULL)
            Crash("MD2: Not enough RAM (2):\n%s", Filename);

        size_t size;

        // read frame data
        size = sizeof(vec3_t);
        memcpy(m->frames[i].scale, ptr, size);
        ptr += size;

        size = sizeof(vec3_t);
        memcpy(m->frames[i].translate, ptr, size);
        ptr += size;

        size = sizeof(char) * 16;
        memcpy(m->frames[i].name, ptr, size);
        ptr += size;

        size = sizeof(md2_vertex_t) * m->header.num_vertices;
        memcpy(m->frames[i].verts, ptr, size);
        ptr += size;
    }

    free(md2_buffer);

    //<<<<<<<<Extatcly here we got a standard md2 model stored in a standard way
    // Now we convert them for a superfast nds renderway
    // ok erstma den shit mallocen
    m->flaechenkoords = (nds_texCoord_t *)malloc(sizeof(nds_texCoord_t) * m->header.num_st);
    m->dreiecke       = (nds_triangle_t *)malloc(sizeof(nds_triangle_t) * m->header.num_tris);
    m->rahmen         = (nds_frame_t *)malloc(sizeof(nds_frame_t) * m->header.num_frames);

    if ((m->flaechenkoords == NULL) || (m->dreiecke == NULL) || (m->rahmen == NULL))
        Crash("MD2: Not enough RAM (3):\n%s", Filename);

    for (int i = 0; i < m->header.num_frames; ++i) {
        m->rahmen[i].verts = (nds_vertex_t *)malloc(sizeof(nds_vertex_t) * m->header.num_vertices);
        if (m->rahmen[i].verts == NULL)
            Crash("MD2: Not enough RAM (4):\n%s", Filename);
    }

    // nun schön brav eins nach dem anderen von float und den dreck in fixed point math konvertieren
    float s, t;
    vec3_t v;
    md2_frame_t *pframe;
    md2_vertex_t *pvert; // der kram is nötig dafür

    // also framedata in rahmendaten umwandeln
    for (int N = 0; N < m->header.num_frames; ++N) {
        for (int I = 0; I < m->header.num_tris; ++I) {
            for (int j = 0; j < 3; ++j) {
                pframe = &m->frames[N];
                pvert  = &pframe->verts[m->triangles[I].vertex[j]];
                // dekomresse die aktuelle framedate
                v[0] = (pframe->scale[0] * pvert->v[0]) + pframe->translate[0];
                v[1] = (pframe->scale[1] * pvert->v[1]) + pframe->translate[1];
                v[2] = (pframe->scale[2] * pvert->v[2]) + pframe->translate[2];
                // Lager die unkompressed framevertexe
                m->rahmen[N].verts[m->triangles[I].vertex[j]].v[0] = floattov16(float(v[0] / 128) * scale);
                m->rahmen[N].verts[m->triangles[I].vertex[j]].v[1] = floattov16(float(v[1] / 128) * scale);
                m->rahmen[N].verts[m->triangles[I].vertex[j]].v[2] = floattov16(float(v[2] / 128) * scale);
            }
        }
    }

    f32 normx = 0, normz = 0;
    // Normal indices
    for (int N = 0; N < m->header.num_frames; ++N) {
        for (int I = 0; I < m->header.num_vertices; ++I) {
            m->rahmen[N].verts[I].colorindex = 0;
            for (int g = 0; g < 4; g++) {
                if (g == 0) {
                    normz = anormtable16[m->frames[N].verts[I].normalIndex][0];
                    normx = anormtable16[m->frames[N].verts[I].normalIndex][1];
                }
                if (g == 1) {
                    normz = anormtable16[m->frames[N].verts[I].normalIndex][1];
                    normx = -anormtable16[m->frames[N].verts[I].normalIndex][0];
                }
                if (g == 2) {
                    normz = -anormtable16[m->frames[N].verts[I].normalIndex][0];
                    normx = -anormtable16[m->frames[N].verts[I].normalIndex][1];
                }
                if (g == 3) {
                    normz = -anormtable16[m->frames[N].verts[I].normalIndex][1];
                    normx = anormtable16[m->frames[N].verts[I].normalIndex][0];
                }

                if (normz <= zero) {
                    if (g == 0)
                        if (normx < zero)
                            m->rahmen[N].verts[I].colorindex |= 0;
                    if (g == 0)
                        if (normx >= zero)
                            m->rahmen[N].verts[I].colorindex |= 1;
                    if (g == 1)
                        if (normx < zero)
                            m->rahmen[N].verts[I].colorindex |= (0 << 2);
                    if (g == 1)
                        if (normx >= zero)
                            m->rahmen[N].verts[I].colorindex |= (1 << 2);
                    if (g == 2)
                        if (normx < zero)
                            m->rahmen[N].verts[I].colorindex |= (0 << 4);
                    if (g == 2)
                        if (normx >= zero)
                            m->rahmen[N].verts[I].colorindex |= (1 << 4);
                    if (g == 3)
                        if (normx < zero)
                            m->rahmen[N].verts[I].colorindex |= (0 << 6);
                    if (g == 3)
                        if (normx >= zero)
                            m->rahmen[N].verts[I].colorindex |= (1 << 6);
                }

                if (normz > zero) {
                    if (g == 0)
                        if (normx < zero)
                            m->rahmen[N].verts[I].colorindex = 2;
                    if (g == 0)
                        if (normx >= zero)
                            m->rahmen[N].verts[I].colorindex = 3;
                    if (g == 1)
                        if (normx < zero)
                            m->rahmen[N].verts[I].colorindex |= (2 << 2);
                    if (g == 1)
                        if (normx >= zero)
                            m->rahmen[N].verts[I].colorindex |= (3 << 2);
                    if (g == 2)
                        if (normx < zero)
                            m->rahmen[N].verts[I].colorindex |= (2 << 4);
                    if (g == 2)
                        if (normx >= zero)
                            m->rahmen[N].verts[I].colorindex |= (3 << 4);
                    if (g == 3)
                        if (normx < zero)
                            m->rahmen[N].verts[I].colorindex |= (2 << 6);
                    if (g == 3)
                        if (normx >= zero)
                            m->rahmen[N].verts[I].colorindex |= (3 << 6);
                }
            }
        }
    }

    // Nun zu den texturkooridinaten
    for (int I = 0; I < m->header.num_tris; ++I) {
        for (int j = 0; j < 3; ++j) {
            if (texturereverse) {
                s = (float)m->texcoords[m->triangles[I].st[j]].s / m->header.skinwidth;
                t = (float)m->texcoords[m->triangles[I].st[j]].t / m->header.skinheight;
                m->flaechenkoords[m->triangles[I].st[j]].s = floattot16(float(s) * widthheight);
                m->flaechenkoords[m->triangles[I].st[j]].t = floattot16(float(1 - t) * widthheight);
            }

            if (!texturereverse) {
                s = (float)m->texcoords[m->triangles[I].st[j]].s / m->header.skinwidth;
                t = (float)m->texcoords[m->triangles[I].st[j]].t / m->header.skinheight;
                m->flaechenkoords[m->triangles[I].st[j]].s = floattot16(float(s) * widthheight);
                m->flaechenkoords[m->triangles[I].st[j]].t = floattot16(float(t) * widthheight);
            }
            m->dreiecke[I].st[j]     = m->triangles[I].st[j];     // Index for texture coords
            m->dreiecke[I].vertex[j] = m->triangles[I].vertex[j]; // Index for vertices
        }
    }

    for (int N = 0; N < m->header.num_frames; ++N)
        free(m->frames[N].verts);
    free(m->frames);
    free(m->texcoords);
    free(m->triangles);
    m->Enabled = true;

    // Now calculate the collision...
    MakeCollisionMap(num);
}

void LoadModelTexture(char filename[], int *Target, int *w)
{
    if (Target == NULL || w == NULL)
        Crash("Invalid arguments:\n%s", __func__);

    u8 *picbuff;
    u16 *palbuff;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &picbuff, &palbuff, &height, &width);

    if ((width != 64 || height != 64) && (width != 128 || height != 128))
        Crash("Texture isn't 64x64 or 128x128\nSize: %lux%lu\n%s", width, height, filename);

    *w = width;

    glGenTextures(1, Target);
    glBindTexture(0, *Target);

    WaitForFreeVblank();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, width, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, picbuff);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, palbuff);

    free(picbuff);
    free(palbuff);
}
