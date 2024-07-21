#ifndef GFXENGINE_RENDER_OBJ_WALL_H__
#define GFXENGINE_RENDER_OBJ_WALL_H__

#include "GfxEngine/3D.h"
#include "GfxEngine/Limits.h"

typedef struct {
    int WidthHeight;
    bool HasBump;

    int Texture; // Standard slot for objects

    // int TextureA;
    int TextureB; // Advanced slot for objects with bump mapping
    int TextureC; // Advanced slot for objects with bump mapping
} ObjectTextureInfo;

extern ObjectTextureInfo ObjectTexture[ObjectTexture_Max];

void RenderWallBorder(int mode, int direction, f32 x, f32 y, f32 z, int textnum);

#endif // GFXENGINE_RENDER_OBJ_WALL_H__
