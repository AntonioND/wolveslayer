#ifndef GFXENGINE_RENDER_WATER_H__
#define GFXENGINE_RENDER_WATER_H__

#include "GfxEngine/3D.h"
#include "GfxEngine/Limits.h"

// TODO: Currently there isn't any map that uses water

typedef struct {
    int Texture;
    u32 ColorKey;
} WaterInfo;

extern WaterInfo Water[Water_Max];

void RenderWater(int floating, f32 x, f32 y, f32 z);

#endif // GFXENGINE_RENDER_WATER_H__
