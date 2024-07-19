#ifndef GFXENGINE_RENDER_AUTOTILES_H__
#define GFXENGINE_RENDER_AUTOTILES_H__

#include <nds.h>

typedef struct {
    int Texture[3];
    u32 ColorKeyMaster;

    u32 Ignorecolors[10];
    int IgnorecolorsNum;
} AutotileInfo;

extern AutotileInfo Autotile[4];

// Autotiles
void LoadAutotileTexture(char filename[], int num);
void RenderAutoGround(int x, int y, int z, int num);
void AddAutIgnore(int Tilenum, u32 Col);

#endif // GFXENGINE_RENDER_AUTOTILES_H__
