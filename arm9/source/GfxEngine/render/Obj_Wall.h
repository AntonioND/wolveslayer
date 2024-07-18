#ifndef GFXENGINE_RENDER_OBJ_WALL_H__
#define GFXENGINE_RENDER_OBJ_WALL_H__

#include "GfxEngine/3D.h"

// textureslots for objects
extern int ObjektTex[Object_Count];  // Standard slot for objects
//extern int ObjektTexA[Object_Count];
extern int ObjektTexB[Object_Count]; // Advanced slot for objects(bumpmapping)
extern int ObjektTexC[Object_Count]; // Advanced slot for objects(bumpmapping)

void RenderWallBorder(int mode, int dirrection, f32 x, f32 y, f32 z, int textnum);

#endif // GFXENGINE_RENDER_OBJ_WALL_H__
