#ifndef GFXENGINE_MAPLOAD_H__
#define GFXENGINE_MAPLOAD_H__

#include <nds.h>

extern u32 MapImage[256][256];

extern u8 MapLightR[128 * 128];
extern u8 MapLightG[128 * 128];
extern u8 MapLightB[128 * 128];

extern bool outside;

// MAP

u32 MapBodenGetRGB(int x, int y);

// For getting tiled size of map
int MapGetWr(void);
int MapGetHr(void);

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
float GetHight(int x, int y);

void LoadMap(char *filename);

// For reseting mapchange positions and mapchangedoors
void ResetMapChange(void);

// Adds a mapchange
void AddMapChange(int x, int y, char Filename[], int tox, int toy);

// Adds a door
void AddMapDoor(int x, int y, char Filename[], int tox, int toy, int key);

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
