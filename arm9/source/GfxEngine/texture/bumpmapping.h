#ifndef GFXENGINE_TEXTURE_BUMPMAPPING_H__
#define GFXENGINE_TEXTURE_BUMPMAPPING_H__

// Bumpmapping

// Used for walls only
void LoadMBump3Texture(char filename[], int num, int *target, int *targetb, int *targetc);
// Used for grounds only (no water and no autotiles!)
void LoadMBump5Texture(char filename[], int num);

#endif // GFXENGINE_TEXTURE_BUMPMAPPING_H__
