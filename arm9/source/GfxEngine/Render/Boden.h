#ifndef GFXENGINE_RENDER_BODEN_H__
#define GFXENGINE_RENDER_BODEN_H__

#include <nds.h>

#include "GfxEngine/Limits.h"

extern int BodenX, BodenY;

extern t16 BodenTextPosStart[4];
extern t16 BodenTextPosEnd[4];

static const v16 BodenVertex1 = floattov16(.508);
static const v16 BodenVertex2 = floattov16(-.508);

extern int BodenTexture[Ground_Count];
// extern int BodenTextureN[Ground_Count];
extern int BodenTextureS[Ground_Count];
extern int BodenTextureW[Ground_Count];
extern int BodenTextureE[Ground_Count];

extern bool BodenEnable[Ground_Count];
extern u32 BodenColorKey[Ground_Count];
extern bool BodenTransEnable[Ground_Count];
extern bool BodenBumpEnable[Ground_Count];
extern u8 BodenSize[Ground_Count];

// Boden
void SetCurBod(int x, int y); // The Boden needs to know where it stands to set up light correctly
void LoadBodenTexture(char filename[], int num);
void RenderBoden(int x, int y, int z, int textnum);
void RenderLevelBorderBoden(int x, int y, int z);

#endif // GFXENGINE_RENDER_BODEN_H__
