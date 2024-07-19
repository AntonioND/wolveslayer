#ifndef GFXENGINE_RENDER_AUTOTILES_H__
#define GFXENGINE_RENDER_AUTOTILES_H__

#include <nds.h>

// Texturen zum anzeigen
extern int AutotileTextur[4][3];
extern u32 AutotileColorKeyMaster[4];
extern u32 AutotileIgnorecolors[10][4];
extern int AutotileIgnorecolorsNum[4];

// Autotiles
void LoadAutotileTexture(char filename[], int num);
void RenderAutoBoden(int x, int y, int z, int num);
void AddAutIgnore(int Tilenum, u32 Col);

#endif // GFXENGINE_RENDER_AUTOTILES_H__
