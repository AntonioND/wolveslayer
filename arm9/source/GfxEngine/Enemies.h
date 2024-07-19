#ifndef GFXENGINE_ENEMIES_H__
#define GFXENGINE_ENEMIES_H__

typedef struct {
    int X, Y;     // It's the HardPos on the Map(on which tile it stands)
    float SX, SY; // It's softpos. It goes from -.5 to .5
    int TextNum;  // The Index for texture
    int Direction; // The Direcction for each NPC
    int Status;
    int Frame;
    int ATP;
    int HP;
    float Radius;
    bool IsBoss;
} EnemyInfo;

extern EnemyInfo Enemies[10];

extern int EnemyCount; // It's the count of NPCs

// Enemys
void ResetEnemys(void);
void AddEnemy(int x, int y, int texnum, int atp, int hp, float rad, bool boss);
void UpdateEnemy();

#endif // GFXENGINE_ENEMIES_H__
