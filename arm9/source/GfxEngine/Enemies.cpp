#include "GfxEngine/3D.h"
#include "GfxEngine/Enemies.h"
#include "GfxEngine/Input/Circlestuff.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/Render/Render.h"
#include "GfxEngine/Villagers.h"

EnemyInfo Enemies[Enemies_Max];

int EnemyCount = -1;

void ResetEnemies(void)
{
    EnemyCount = -1;

    for (int a = 0; a < Enemies_Max; a++)
    {
        Enemies[a].X         = -1;
        Enemies[a].Y         = -1;
        Enemies[a].SX        = 0;
        Enemies[a].SY        = 0;
        Enemies[a].TextNum   = -1;
        Enemies[a].Direction = 0;
        Enemies[a].Status    = -1;
        Enemies[a].Frame     = 0;
    }
}

void AddEnemy(int x, int y, int texnum, int atp, int hp, float rad, bool boss)
{
    if (EnemyCount >= Enemies_Max)
        Crash("Too many enemies");

    EnemyCount++;
    Enemies[EnemyCount].X       = x;
    Enemies[EnemyCount].Y       = y;
    Enemies[EnemyCount].TextNum = texnum;
    Enemies[EnemyCount].ATP     = atp;
    Enemies[EnemyCount].HP      = hp;
    Enemies[EnemyCount].Radius  = rad;
    Enemies[EnemyCount].IsBoss  = boss;
}

static void TurnEnemy(int a, bool l, bool r, bool u, bool d)
{
    // Now give a new valid dirrecion if possible
    if (!l && !d && !r && !u)
        return;

    int dirnew = 0;
    bool ok    = false;

    while (ok == false)
    {
        ok = true;

        dirnew = rand() % 8;
        if ((dirnew == 0 || dirnew == 1 || dirnew == 7) && d == false)
            ok = false;
        if ((dirnew == 1 || dirnew == 2 || dirnew == 3) && l == false)
            ok = false;
        if ((dirnew == 6 || dirnew == 5 || dirnew == 7) && r == false)
            ok = false;
        if ((dirnew == 4 || dirnew == 3 || dirnew == 5) && u == false)
            ok = false;
    }

    Enemies[a].Direction = dirnew;
}

static void TurnittoPlayer(int a)
{
    float NPx = GetPX() + (PlPosSX + .5); // its playerpos here...
    float NPy = GetPY() + (PlPosSY + .5); // why? copy/paste/change a bit...easy

    float Px = Enemies[a].X;
    float Py = Enemies[a].Y;
    if (Enemies[a].SY >= -.5)
        Py++;
    if (Enemies[a].SY <= .5)
        Py--;
    if (Enemies[a].SX >= -.5)
        Px++;
    if (Enemies[a].SX <= .5)
        Px--;
    Px += Enemies[a].SX + .5;
    Py += Enemies[a].SY + .5;

    float dx = Px - NPx;
    if (dx < 0)
        dx *= -1;
    float dy = Py - NPy;
    if (dy < 0)
        dy *= -1;

    // now we got the distance, and can look how to turn the NPC
    Enemies[a].Direction = -1;
    if (Px > NPx) // right
    {
        if ((dx / 2) > dy) // more side then up/down
        {
            Enemies[a].Direction = 2; // right
        }
        if ((dy / 2) > dx) // more up/down then side
        {
            if (Py > NPy)
                Enemies[a].Direction = 4; // down
            else
                Enemies[a].Direction = 0; // up
        }
        if (Enemies[a].Direction == -1) // both
        {
            if (Py > NPy)
                Enemies[a].Direction = 3; // down-right
            else
                Enemies[a].Direction = 1; // up-right
        }
    }
    else // left
    {
        if ((dx / 2) > dy) // more side then up/down
        {
            Enemies[a].Direction = 6; // left
        }
        if ((dy / 2) > dx) // more up/down then side
        {
            if (Py > NPy)
                Enemies[a].Direction = 4; // down
            else
                Enemies[a].Direction = 0; // up
        }
        if (Enemies[a].Direction == -1) // both
        {
            if (Py > NPy)
                Enemies[a].Direction = 5; // down-left
            else
                Enemies[a].Direction = 7; // up-left
        }
    }
}

static float gethposfromenemy(int a, float sx, float sy)
{
    float vx  = Enemies[a].X;
    float vy  = Enemies[a].Y;
    float vsx = Enemies[a].SX + sx;
    float vsy = Enemies[a].SY + sy;

    // This is needed to get a better position
    if (vsy >= -.5)
        vy++;
    if (vsy <= .5)
        vy--;
    if (vsx >= -.5)
        vx++;
    if (vsx <= .5)
        vx--;

    vsx = Enemies[a].SX + .5;
    vsy = Enemies[a].SY + .5;

    return GetInterPolY(vx, vy, vsx, vsy);
}

void UpdateEnemy()
{
    float Px, Py;
    float NPx, NPy;
    float dx, dy;
    bool change;
    bool l, r, u, d;

    if (screenmode < ScreenModeItem)
    {
        for (int a = 0; a <= EnemyCount; a++)
        {
            // check coolision just if enemy walks or follows player
            if (Enemies[a].Status < 1)
            {
                l = true;
                r = true;
                u = true;
                d = true;

                float sx = Enemies[a].SX * 20;
                float sy = Enemies[a].SY * 20;

                bool val[8];
                Checkcolision(Enemies[a].X, Enemies[a].Y, sx + 10, sy + 10, 5, (bool *)&val);
                u = val[0];
                if (gethposfromenemy(a, 0, -.025) != gethposfromenemy(a, 0, 0))
                    u = false;
                r = val[2];
                if (gethposfromenemy(a, .025, 0) != gethposfromenemy(a, 0, 0))
                    r = false;
                d = val[4];
                if (gethposfromenemy(a, 0, .025) != gethposfromenemy(a, 0, 0))
                    d = false;
                l = val[6];
                if (gethposfromenemy(a, -.025, 0) != gethposfromenemy(a, 0, 0))
                    l = false;

                // Chrash mit Player verhindern
                NPx = GetPX() + (PlPosSX + .5); // its playerpos here...
                NPy = GetPY() + (PlPosSY + .5); // why? copy/paste/change a bit...easy

                Px = Enemies[a].X;
                Py = Enemies[a].Y;
                if (Enemies[a].SY >= -.5)
                    Py++;
                if (Enemies[a].SY <= .5)
                    Py--;
                if (Enemies[a].SX >= -.5)
                    Px++;
                if (Enemies[a].SX <= .5)
                    Px--;
                Px += Enemies[a].SX + .5;
                Py += Enemies[a].SY + .5;
                // Wow strange code....but should work to compare those positions right
                // first check distance
                dx = Px - NPx;
                if (dx < 0)
                    dx *= -1;
                dy = Py - NPy;
                if (dy < 0)
                    dy *= -1;
                // now lets compare
                // near enough to see player
                Enemies[a].Status = -1; // enemy wont follow player
                if (PlStatus != 3)
                {
                    if (dx > dy)
                        if (Py > NPy - (.9 + Enemies[a].Radius) && Py < NPy + (.9 + Enemies[a].Radius))
                        {
                            if (Px < NPx && Px + (.9 + Enemies[a].Radius) > NPx)
                            {
                                Enemies[a].Status = 0;
                                TurnittoPlayer(a);
                            } // follow the player
                            if (Px > NPx && Px - (.9 + Enemies[a].Radius) < NPx)
                            {
                                Enemies[a].Status = 0;
                                TurnittoPlayer(a);
                            } // follow the player
                        }
                    if (dy > dx)
                        if (Px > NPx - (.9 + Enemies[a].Radius) && Px < NPx + (.9 + Enemies[a].Radius))
                        {
                            if (Py < NPy && Py + (.9 + Enemies[a].Radius) > NPy)
                            {
                                Enemies[a].Status = 0;
                                TurnittoPlayer(a);
                            } // follow the player
                            if (Py > NPy && Py - (.9 + Enemies[a].Radius) < NPy)
                            {
                                Enemies[a].Status = 0;
                                TurnittoPlayer(a);
                            } // follow the player
                        }
                    // near enough to attack and to near to get closer
                    if (dx > dy)
                        if (Py > NPy - Enemies[a].Radius && Py < NPy + Enemies[a].Radius)
                        {
                            if (Px < NPx && Px + Enemies[a].Radius > NPx)
                            {
                                r = false;
                                if (rand() % 17 == 2)
                                    Enemies[a].Status = 1;
                            }
                            if (Px > NPx && Px - Enemies[a].Radius < NPx)
                            {
                                l = false;
                                if (rand() % 17 == 2)
                                    Enemies[a].Status = 1;
                            }
                        }
                    if (dy > dx)
                        if (Px > NPx - Enemies[a].Radius && Px < NPx + Enemies[a].Radius)
                        {
                            if (Py < NPy && Py + Enemies[a].Radius > NPy)
                            {
                                d = false;
                                if (rand() % 17 == 2)
                                    Enemies[a].Status = 1;
                            }
                            if (Py > NPy && Py - Enemies[a].Radius < NPy)
                            {
                                u = false;
                                if (rand() % 17 == 2)
                                    Enemies[a].Status = 1;
                            }
                        }
                }
                // Chrash mit anderen NPCs verhindern
                NPx = Enemies[a].X;
                NPy = Enemies[a].Y;

                for (int NPCnum = 0; NPCnum <= EnemyCount; NPCnum++)
                {
                    if (NPCnum != a)
                    {
                        Px = Enemies[NPCnum].X;
                        Py = Enemies[NPCnum].Y;
                        if (Enemies[NPCnum].SY >= -.5)
                            Py++;
                        if (Enemies[NPCnum].SY <= .5)
                            Py--;
                        if (Enemies[NPCnum].SX >= -.5)
                            Px++;
                        if (Enemies[NPCnum].SX <= .5)
                            Px--;
                        Px += Enemies[NPCnum].SX + .5;
                        Py += Enemies[NPCnum].SY + .5;
                        // Wow strange code....but should work to compare those positions right
                        // first check distance
                        dx = Px - NPx;
                        if (dx < 0)
                            dx *= -1;
                        dy = Py - NPy;
                        if (dy < 0)
                            dy *= -1;
                        // now lets compare
                        if (dx > dy)
                            if (Py > NPy - Enemies[NPCnum].Radius && Py < NPy + Enemies[NPCnum].Radius && Enemies[NPCnum].Status != 3)
                            {
                                if (Px < NPx && Px + Enemies[NPCnum].Radius > NPx)
                                    r = false;
                                if (Px > NPx && Px - Enemies[NPCnum].Radius < NPx)
                                    l = false;
                            }
                        if (dy > dx)
                            if (Px > NPx - Enemies[NPCnum].Radius && Px < NPx + Enemies[NPCnum].Radius && Enemies[NPCnum].Status != 3)
                            {
                                if (Py < NPy && Py + Enemies[NPCnum].Radius > NPy)
                                    d = false;
                                if (Py > NPy && Py - Enemies[NPCnum].Radius < NPy)
                                    u = false;
                            }
                    }
                }
            }

            // Movement
            // Direction change (random walking)
            if (Enemies[a].Status == -1)
            {
                change = false;
                if ((Enemies[a].Direction == 0 || Enemies[a].Direction == 1 || Enemies[a].Direction == 7) && d == false)
                    change = true;
                if ((Enemies[a].Direction == 1 || Enemies[a].Direction == 2 || Enemies[a].Direction == 3) && l == false)
                    change = true;
                if ((Enemies[a].Direction == 6 || Enemies[a].Direction == 5 || Enemies[a].Direction == 7) && r == false)
                    change = true;
                if ((Enemies[a].Direction == 4 || Enemies[a].Direction == 3 || Enemies[a].Direction == 5) && u == false)
                    change = true;
                if (rand() % 60 == 0)
                    change = true;

                if (change)
                    TurnEnemy(a, l, r, u, d);
            }

            if (Enemies[a].Status < 1)
            {
                // Horizontal
                if ((Enemies[a].Direction == 6 || Enemies[a].Direction == 5 || Enemies[a].Direction == 7) && r)
                    Enemies[a].SX += .025;
                if (Enemies[a].SX > .5)
                {
                    Enemies[a].X += 1;
                    Enemies[a].SX -= 1;
                }
                if ((Enemies[a].Direction == 1 || Enemies[a].Direction == 2 || Enemies[a].Direction == 3) && l)
                    Enemies[a].SX -= .025;
                if (Enemies[a].SX < -.5)
                {
                    Enemies[a].X -= 1;
                    Enemies[a].SX += 1;
                }
                // Vertikal
                if ((Enemies[a].Direction == 0 || Enemies[a].Direction == 1 || Enemies[a].Direction == 7) && d)
                    Enemies[a].SY += .025;
                if (Enemies[a].SY > .5)
                {
                    Enemies[a].Y += 1;
                    Enemies[a].SY -= 1;
                }
                if ((Enemies[a].Direction == 4 || Enemies[a].Direction == 3 || Enemies[a].Direction == 5) && u)
                    Enemies[a].SY -= .025;
                if (Enemies[a].SY < -.5)
                {
                    Enemies[a].Y -= 1;
                    Enemies[a].SY += 1;
                }
            }
        }
    }
}
