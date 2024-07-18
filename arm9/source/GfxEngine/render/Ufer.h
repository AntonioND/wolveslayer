#ifndef GFXENGINE_RENDER_UFER_H__
#define GFXENGINE_RENDER_UFER_H__

#include "GfxEngine/3D.h"

// Ufer

extern int Ufer[10];
extern int UferB[10];
extern int UferC[10];
extern bool UferBump[10];

// Will appear where terrain meet stairs or stair meet stair
void RenderUfer(int x, int y, f32 xx, f32 yy);

void LoadUferTexture(char filename[], int *Target, int num);

#endif // GFXENGINE_RENDER_UFER_H__
