#ifndef GFXENGINE_RENDER_EDGE_H__
#define GFXENGINE_RENDER_EDGE_H__

#include "GfxEngine/3D.h"

// This is what fills the gaps between one ground tile and the one next to it.
// If there is a slope, this isn't required, but it is required if the height
// changes aren't continous.

extern int Edge[10];
extern int EdgeB[10];
extern int EdgeC[10];
extern bool EdgeBump[10];

// Will appear where terrain meet stairs or stair meet stair
void RenderEdge(int x, int y, f32 xx, f32 yy);

void LoadEdgeTexture(char filename[], int *Target, int num);

#endif // GFXENGINE_RENDER_EDGE_H__
