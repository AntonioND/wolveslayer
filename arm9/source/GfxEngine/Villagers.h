#ifndef GFXENGINE_VILLAGERS_H__
#define GFXENGINE_VILLAGERS_H__

#include "GfxEngine/Limits.h"

extern int VillagerX[Villagers_Max], VillagerY[Villagers_Max];
extern float VillagerSX[Villagers_Max], VillagerSY[Villagers_Max];
extern int VillagerTextNum[Villagers_Max];
extern int VillagerRichtung[Villagers_Max];
extern int VillagerCount;
extern char VillagerSpeech[Villagers_Max][10][256];
extern int VillagerSpeechCount[Villagers_Max];

extern int SpriteX, SpriteY;
extern float SpriteSX, SpriteSY;

// Villagers
void ResetVillagers(void);
void AddTexttoVillager(char txt[256]);
void AddVillager(int x, int y, int texnum);
void TurnVillager(int a, bool l, bool r, bool u, bool d);
void UpdateVillagers();

// Sprite
void SetCurSprite(int x, int y, float sx, float sy);
void LoadSpriteTexture(char filename[], char palname[], int *Target, int num);

#endif // GFXENGINE_VILLAGERS_H__
