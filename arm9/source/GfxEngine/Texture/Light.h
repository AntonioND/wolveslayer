#ifndef GFXENGINE_TEXTURE_LIGHT_H__
#define GFXENGINE_TEXTURE_LIGHT_H__

#include <nds.h>

extern int Blend;
extern u8 EnvR, EnvG, EnvB;

void SetRot(int rot);
void RunTime(void); // recalcs the lightcolors(usid to run the day/night-swap

void GiveLight(int x, int y, u8 *col); // Same like above but gives color back (col[3] rgb)
void GrapLight(int x, int y);          // Simple graping lights(its used for terrain)

// This is used per vertex for a md2...a good alternative to hw light i hope
u16 GrapMdlLight(v16 nx, v16 normy, v16 nz);

// prepears some lights for moving md2
void SetMdlLights(int pos[2], float softpos[2], float radius, int rot);

// Prepares lights way faster for standing md2(tree or someting)
void SetObjLights(int pos[2], int rot);

// selfexplaining
void SetObjLightsSelfilluminated(void);

int GetLightRot(void);

// This is used per vertex for a md2...best alternative to hw light i hope
void GrapMd2Light(u8 Index, u16 &buff);

#endif // GFXENGINE_TEXTURE_LIGHT_H__
