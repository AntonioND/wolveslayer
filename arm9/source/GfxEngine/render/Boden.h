#ifndef GFXENGINE_RENDER_BODEN_H__
#define GFXENGINE_RENDER_BODEN_H__

extern int BodenX, BodenY;

// Boden
void SetCurBod(int x, int y); // The Boden needs to know where it stands to set up light correctly
void LoadBodenTexture(char filename[], int num);
void RenderBoden(int x, int y, int z, int textnum);
void RenderLevelBorderBoden(int x, int y, int z);

#endif // GFXENGINE_RENDER_BODEN_H__
