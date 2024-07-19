#ifndef GFXENGINE_RENDER_GROUND_H__
#define GFXENGINE_RENDER_GROUND_H__

#include <nds.h>

#include "GfxEngine/Limits.h"

extern int GroundX, GroundY;

extern const t16 GroundTextPosStart[4];
extern const t16 GroundTextPosEnd[4];

static const v16 GroundVertex1 = floattov16(.508);
static const v16 GroundVertex2 = floattov16(-.508);

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

extern GroundInfo Ground[Ground_Max];

// The ground needs to know where it stands to set up light correctly
void SetCurGround(int x, int y);

void LoadGroundTexture(char filename[], int num);
void RenderGround(int x, int y, int z, int textnum);
void RenderLevelBorderGround(int x, int y, int z);

#endif // GFXENGINE_RENDER_GROUND_H__
