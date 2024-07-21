#ifndef GFXENGINE_RENDER_RENDER_H__
#define GFXENGINE_RENDER_RENDER_H__

#include <nds.h>

#include "GfxEngine/3D.h"

extern f32 waterheight;

float GetInterPolY(int x, int y, float sx, float sy);
void Refresh3D(void);

#endif // GFXENGINE_RENDER_RENDER_H__
