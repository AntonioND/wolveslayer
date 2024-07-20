#ifndef GFXENGINE_TEXTURE_DYNAMICLIGHTS_H__
#define GFXENGINE_TEXTURE_DYNAMICLIGHTS_H__

#include <nds.h>

extern u8 WorldLightR[128 * 128];
extern u8 WorldLightG[128 * 128];
extern u8 WorldLightB[128 * 128];

extern u16 ViewportMapLights[20][20];
extern bool ViewportMapBumpGroundS[20][20];
extern bool ViewportMapBumpGroundW[20][20];
extern bool ViewportMapBumpGroundE[20][20];
extern bool ViewportMapBumpWallS[20][20];
extern bool ViewportMapBumpWallW[20][20];
extern bool ViewportMapBumpWallE[20][20];

void ResetDynamic(void);

// Adds a randomy-flying-arround-dynamic-light(with given color)
void AddDynamic(int x, int y, u8 r, u8 g, u8 b);

void UpdateDynamic(void);

#endif // GFXENGINE_TEXTURE_DYNAMICLIGHTS_H__
