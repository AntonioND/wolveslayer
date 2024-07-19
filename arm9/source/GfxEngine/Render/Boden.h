#ifndef GFXENGINE_RENDER_BODEN_H__
#define GFXENGINE_RENDER_BODEN_H__

#include <nds.h>

#include "GfxEngine/Limits.h"

extern int GroundX, GroundY;

extern const t16 BodenTextPosStart[4];
extern const t16 BodenTextPosEnd[4];

static const v16 BodenVertex1 = floattov16(.508);
static const v16 BodenVertex2 = floattov16(-.508);

typedef struct {
    int Texture;
    // int TextureN;
    int TextureS;
    int TextureW;
    int TextureE;

    bool Enable;
    u32 ColorKey;
    bool TransEnable;
    bool BumpEnable;
    u8 Size;
} GroundInfo;

extern GroundInfo Ground[Ground_Count];

// Boden
void SetCurBod(int x, int y); // The Boden needs to know where it stands to set up light correctly
void LoadBodenTexture(char filename[], int num);
void RenderBoden(int x, int y, int z, int textnum);
void RenderLevelBorderBoden(int x, int y, int z);

#endif // GFXENGINE_RENDER_BODEN_H__
