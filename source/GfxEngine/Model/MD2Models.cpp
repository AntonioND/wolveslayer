// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/Model/MD2Format.h"
#include "GfxEngine/Model/MD2Models.h"
#include "GfxEngine/Texture/Light.h"

MD2Entity Models[MD2_Max];

static u16 *ColorContainer;

void Precalcmd2light(int n, int num)
{
    if ((num < 0) || (num >= MD2_Max))
        Crash("Invalid number:%d\n%s", num, __func__);

    if ((n < 0) || (n > Models[num].header.num_frames - 1))
        n = Models[num].header.num_frames - 1;

    if (Models[num].Enabled == false)
        return;

    nds_vertex_t *vert = Models[num].rahmen[n].verts;
    free(ColorContainer);
    ColorContainer = (u16 *)malloc(2 * Models[num].header.num_vertices);
    if (ColorContainer == NULL)
        Crash("Not enough RAM:\n%s", __func__);

    int rot = GetLightRot() * 2;
    for (int i = 0; i < Models[num].header.num_vertices; ++i)
        GrapMd2Light((vert[i].colorindex & (3 << rot)) >> rot, ColorContainer[i]);
}

// This renders MD2 objects, which is almost everything that you see in the world.
void RenderMD2Model(int n, int num)
{
    if ((num < 0) || (num >= MD2_Max))
        Crash("Invalid number:%d\n%s", num, __func__);

    if (Models[num].Enabled == false)
        Crash("Drawing disabled MD2:%d\n%s", num, __func__);

    if (n < 0)
        n = 0;
    if (n > Models[num].header.num_frames - 1)
        n = Models[num].header.num_frames - 1;

    glBegin(GL_TRIANGLES);

    nds_vertex_t *vert = Models[num].rahmen[n].verts;
    for (int i = 0; i < Models[num].header.num_tris; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int stnum   = Models[num].dreiecke[i].st[j];
            int vertnum = Models[num].dreiecke[i].vertex[j];

            GFX_COLOR = (vuint32)ColorContainer[vertnum];
            glTexCoord2t16(Models[num].flaechenkoords[stnum].s, Models[num].flaechenkoords[stnum].t);
            glVertex3v16(vert[vertnum].v[0], vert[vertnum].v[1], vert[vertnum].v[2]);
        }
    }

    glEnd();
}

void RenderMD2ModelMirrored(int n, int num)
{
    if ((num < 0) || (num >= MD2_Max))
        Crash("Invalid number:%d\n%s", num, __func__);

    if (Models[num].Enabled == false)
        Crash("Drawing disabled MD2:%d\n%s", num, __func__);

    if (n < 0)
        n = 0;
    if (n > Models[num].header.num_frames - 1)
        n = Models[num].header.num_frames - 1;

    glBegin(GL_TRIANGLES);

    nds_vertex_t *vert = Models[num].rahmen[n].verts;
    for (int i = 0; i < Models[num].header.num_tris; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int stnum   = Models[num].dreiecke[i].st[j];
            int vertnum = Models[num].dreiecke[i].vertex[j];

            GFX_COLOR = (vuint32)ColorContainer[vertnum];
            glTexCoord2t16(Models[num].flaechenkoords[stnum].s, Models[num].flaechenkoords[stnum].t);
            glVertex3v16(vert[vertnum].v[0], vert[vertnum].v[1], -vert[vertnum].v[2]);
        }
    }

    glEnd();
}
