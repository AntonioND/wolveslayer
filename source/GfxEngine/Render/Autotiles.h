// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_RENDER_AUTOTILES_H__
#define GFXENGINE_RENDER_AUTOTILES_H__

#include <nds.h>

#include "GfxEngine/Limits.h"

typedef struct {
    int Texture[3];
    u32 ColorKeyMaster;

    u32 Ignorecolors[Autotile_Ignorecolors_Max];
    int IgnorecolorsNum;
} AutotileInfo;

extern AutotileInfo Autotile[Autotile_Max];

// Autotiles
void LoadAutotileTexture(char filename[], int num);
void RenderAutoGround(int x, int y, int z, int num);
void AddAutIgnore(int Tilenum, u32 Col);

#endif // GFXENGINE_RENDER_AUTOTILES_H__
