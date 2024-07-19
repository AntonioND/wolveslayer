#ifndef GFXENGINE_RENDER_DOORS_H__
#define GFXENGINE_RENDER_DOORS_H__

#include "GfxEngine/3D.h"

void LoadDoorTexture(char filename[]);

void RenderHouseBorderDoor(int mode, int doorangle, int dirrection, f32 x, f32 y, f32 z);
void RenderWallBorderDoor(int mode, int doorangle, int dirrection, f32 x, f32 y, f32 z);

#endif // GFXENGINE_RENDER_DOORS_H__
