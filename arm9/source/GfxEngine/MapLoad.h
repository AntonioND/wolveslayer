// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_MAPLOAD_H__
#define GFXENGINE_MAPLOAD_H__

#include <nds.h>

#include "GfxEngine/3D.h"

extern u8 MapLightR[128 * 128];
extern u8 MapLightG[128 * 128];
extern u8 MapLightB[128 * 128];

// MAP

u32 MapGroundGetRGB(int x, int y);

// For getting tiled size of map
int MapGetWr(void);
int MapGetHr(void);

void GetRGBfromMap(int x, int y, u8 &r, u8 &g, u8 &b);

// For getting ObjectDatas
u32 MapObjectGetRGB(int x, int y);

// For getting Objectrotation
u8 MapObjGetRot(int x, int y);

// Determinates if there is a housepart at the given pos.
bool IsObjHouse(int x, int y);
// Determinates if there is a wall at the given pos.
bool IsObjWall(int x, int y);
// Determinates if there is a bumpmapped wall at the given pos.
bool IsObjBumpWall(int x, int y);

// for getting height
f32 GetHeight(int x, int y);

void LoadMap(char *filename);

// For reseting mapchange positions and mapchangedoors
void ResetMapChange(void);

// Adds a mapchange
void AddMapChange(int x, int y, char Filename[], int tox, int toy);

// Adds a door
void AddMapDoor(int x, int y, char Filename[], int tox, int toy, int key);

void AddTexttoDoor(char txt[256]);

const char *GetMapChange(int x, int y);
int GetMapChangePOSX(int x, int y);
int GetMapChangePOSY(int x, int y);

// mapdoors

const char *GetMapDoor(int x, int y);
int GetMapDoorAngle(int x, int y);

// This sets the door angle to 1 if it is unlocked
void OpenMapDoor(int x, int y);

// This opens doors which angle is greater then 0
void MapDoorHandle(void);

v16 GetTerrain(int x, int y, int num);

#endif // GFXENGINE_MAPLOAD_H__
