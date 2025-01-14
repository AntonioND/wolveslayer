// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_TEXTURE_BUMPMAPPING_H__
#define GFXENGINE_TEXTURE_BUMPMAPPING_H__

// Used for walls only
void LoadMBump3Texture(char filename[], int *target, int *targetb, int *targetc);

// Load ground bump mapped texture to a ground texture slot. Used for grounds only
// (no water and no autotiles!)
void LoadMBump5Texture(char filename[], int num);

#endif // GFXENGINE_TEXTURE_BUMPMAPPING_H__
