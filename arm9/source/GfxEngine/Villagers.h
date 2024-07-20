#ifndef GFXENGINE_VILLAGERS_H__
#define GFXENGINE_VILLAGERS_H__

#include "GfxEngine/Limits.h"

typedef struct {
    int X, Y;             // It's the HardPos on the Map(on which tile it stands)
    float SX, SY;         // It's the softpos. It goes from -.5 to .5
    int TextNum;          // The Index for texture
    int Direction;        // The direction for each NPC
    int Count = -1;       // Its the count of NPCs
    char Speech[10][256]; // to hold 10 textes for each NPC with 300 chars
    int SpeechCount;      // Holds the Number of speeches a NPC has...
} VillagerInfo;

extern VillagerInfo Villager[Villagers_Max];

extern int VillagerCount;

extern int SpriteX, SpriteY;
extern float SpriteSX, SpriteSY;

// Villagers
void ResetVillagers(void);
void AddTexttoVillager(char txt[256]); // Adds a text to a specific villiager(npc)
void AddVillager(int x, int y, int texnum);
void TurnVillager(int a, bool l, bool r, bool u, bool d);
void UpdateVillagers();

// Sprite
void SetCurSprite(int x, int y, float sx, float sy);
void LoadSpriteTexture(char filename[], char palname[], int *Target, int num);

#endif // GFXENGINE_VILLAGERS_H__
