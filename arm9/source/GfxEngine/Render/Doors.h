// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_RENDER_DOORS_H__
#define GFXENGINE_RENDER_DOORS_H__

#include "GfxEngine/3D.h"

void LoadDoorTexture(char filename[]);

void RenderHouseBorderDoor(int mode, int doorangle, int direction, f32 x, f32 y, f32 z);
void RenderWallBorderDoor(int mode, int doorangle, int direction, f32 x, f32 y, f32 z);

#endif // GFXENGINE_RENDER_DOORS_H__
