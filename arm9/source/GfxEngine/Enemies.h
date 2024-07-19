#ifndef GFXENGINE_ENEMIES_H__
#define GFXENGINE_ENEMIES_H__

extern int GegnerX[10], GegnerY[10];     // its the HardPos on the Map(on which tile it stands)
extern float GegnerSX[10], GegnerSY[10]; // Its softpos...goes from -.5 to .5
extern int GegnerTextNum[10];            // The Index for texture
extern int GegnerRichtung[10];           // The Direcction for each NPC
extern int GegnerCount;                  // Its the count of NPCs
extern int GegnerStatus[10];
extern int GegnerFrame[10];
extern int GegnerATP[10];
extern int GegnerHP[10];
extern float GegnerRadius[10];
extern bool GegnerIsBoss[10];

// Enemys
void ResetEnemys(void);
void AddEnemy(int x, int y, int texnum, int atp, int hp, float rad, bool boss);
void UpdateEnemy();

#endif // GFXENGINE_ENEMIES_H__
