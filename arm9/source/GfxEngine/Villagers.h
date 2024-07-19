#ifndef GFXENGINE_VILLAGERS_H__
#define GFXENGINE_VILLAGERS_H__

#include "GfxEngine/Limits.h"

extern int DorfiX[Dorf_Max], DorfiY[Dorf_Max];
extern float DorfiSX[Dorf_Max], DorfiSY[Dorf_Max];
extern int DorfiTextNum[Dorf_Max];
extern int DorfiRichtung[Dorf_Max];
extern int DorfiCount;
extern char DorfiSpeech[Dorf_Max][10][256];
extern int DorfiSpeechCount[Dorf_Max];

extern int SpriteX, SpriteY;
extern float SpriteSX, SpriteSY;

// Villagers
void ResetVillagers(void);
void AddTexttoDorfi(char txt[256]);
void AddDorfi(int x, int y, int texnum);
void TurnDorfi(int a, bool l, bool r, bool u, bool d);
void UpdateDorfis();

// Sprite
void SetCurSprite(int x, int y, float sx, float sy);
void LoadSpriteTexture(char filename[], char palname[], int *Target, int num);

#endif // GFXENGINE_VILLAGERS_H__
