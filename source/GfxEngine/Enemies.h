// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_ENEMIES_H__
#define GFXENGINE_ENEMIES_H__

#include "GfxEngine/Limits.h"

typedef struct {
    int X, Y;      // It's the HardPos on the Map(on which tile it stands)
    float SX, SY;  // It's softpos. It goes from -.5 to .5
    int TextNum;   // The Index for texture
    int Direction; // The Direcction for each NPC
    int Status;
    int Frame;
    int ATP;
    int HP;
    float Radius;
    bool IsBoss;
} EnemyInfo;

extern EnemyInfo Enemies[Enemies_Max];

extern int EnemyCount; // It's the count of NPCs

void ResetEnemies(void);
void AddEnemy(int x, int y, int texnum, int atp, int hp, float rad, bool boss);
void UpdateEnemy();

#endif // GFXENGINE_ENEMIES_H__
