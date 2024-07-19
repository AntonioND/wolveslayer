#ifndef GFXENGINE_PRECALC_H__
#define GFXENGINE_PRECALC_H__

#include <nds.h>

typedef struct {
    v16 v[4];
    u8 sidewalls;
} v16x4;

extern u8 Precalcdata[128][128];

// Ground,Autotiles,Wasser  & Terrainseitenwände
extern v16x4 Terrain[128][128];
extern v16 TerrainMid[128][128];

// MapDatas
extern signed char TexGround[128][128];
extern signed char TexAGround[128][128];
extern signed char SetAGround[128][128];

extern signed char TexObj[128][128];
extern signed char ShapeObj[128][128];
extern signed char DirObj[128][128];

// Input
bool Passable(int x, int y, int sxx, int syy);

// That one precalculates which textures to use...gives use  nice speed
void PreCalcAll(void);

#endif // GFXENGINE_PRECALC_H__
