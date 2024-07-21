#ifndef GFXENGINE_RENDER_OBJ_HOUSE_H__
#define GFXENGINE_RENDER_OBJ_HOUSE_H__

#include <nds.h>

#include "GfxEngine/3D.h"

static const v16 mode0vertex  = floattov16(-1);
static const v16 mode1vertex  = floattov16(1);
static const v16 mode0vertex2 = floattov16(.8);
static const v16 mode1vertex2 = floattov16(-.8);

static const t16 walltex1  = inttot16(0);
static const t16 walltex1e = inttot16(31);

static const t16 walltex2  = inttot16(32);
static const t16 walltex2e = inttot16(64);

extern int WallX, WallY;
extern bool walltrans;

// The Wall needs to know where it stands to set up light correctly
void SetCurWall(int x, int y);

// Makes the walls transparent or native
void SetWallTrans(bool bol);

void RenderHouseM(int mode, f32 x, f32 y, f32 z);
void RenderHouseBorder(int mode, int direction, f32 x, f32 y, f32 z);
void RenderHouseCorner(int mode, int direction, f32 x, f32 y, f32 z);

#endif // GFXENGINE_RENDER_OBJ_HOUSE_H__
