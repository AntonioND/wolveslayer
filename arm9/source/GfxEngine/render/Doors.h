#ifndef GFXENGINE_RENDER_DOORS_H__
#define GFXENGINE_RENDER_DOORS_H__

#include "GfxEngine/3D.h"

extern int Door[1];

extern char DoorSpeech[25][10][256]; // to hold 10 textes for each ddor with 256 chars
extern int DoorSpeechCount[25];      // Holds the Number of speeches a Door has...

void AddTexttoDoor(char txt[256]);

void LoadDoorTexture(char filename[]);

void RenderHouseBorderDoor(int mode, int doorangle, int dirrection, f32 x, f32 y, f32 z);
void RenderWallBorderDoor(int mode, int doorangle, int dirrection, f32 x, f32 y, f32 z);

#endif // GFXENGINE_RENDER_DOORS_H__
