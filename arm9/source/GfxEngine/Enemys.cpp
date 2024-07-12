#include "3D.h"

int GegnerX[10], GegnerY[10];     // its the HardPos on the Map(on which tile it stands)
float GegnerSX[10], GegnerSY[10]; // Its softpos...goes from -.5 to .5
int GegnerTextNum[10];            // The Index for texture
int GegnerRichtung[10];           // The Direcction for each NPC
int GegnerCount = -1;             // Its the count of NPCs
int GegnerStatus[10];
int GegnerFrame[10];
int GegnerATP[10];
int GegnerHP[10];
float GegnerRadius[10];
bool GegnerIsBoss[10];

extern int SpriteX, SpriteY;
extern float SpriteSX, SpriteSY;

void ResetEnemys(void)
{
    GegnerCount = -1;
    // int b;
    for (int a = 0; a < 10; a++) {
        GegnerX[a]        = -1;
        GegnerY[a]        = -1;
        GegnerSX[a]       = 0;
        GegnerSY[a]       = 0;
        GegnerTextNum[a]  = -1;
        GegnerRichtung[a] = 0;
        GegnerStatus[a]   = -1;
        GegnerFrame[a]    = 0;
    }
}

void AddEnemy(int x, int y, int texnum, int atp, int hp, float rad, bool boss)
{
    if (10 > GegnerCount + 1) {
        GegnerCount++;
        GegnerX[GegnerCount]       = x;
        GegnerY[GegnerCount]       = y;
        GegnerTextNum[GegnerCount] = texnum;
        GegnerATP[GegnerCount]     = atp;
        GegnerHP[GegnerCount]      = hp;
        GegnerRadius[GegnerCount]  = rad;
        GegnerIsBoss[GegnerCount]  = boss;
    }
}

void TurnEnemy(int a, bool l, bool r, bool u, bool d)
{
    // Now give a new valid dirrecion if possible
    if (!l && !d && !r && !u)
        return;

    int dirnew = 0;
    bool ok    = false;

    while (ok == false) {
        ok     = true;
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

    GegnerRichtung[a] = dirnew;
}

void TurnittoPlayer(int a)
{
    extern float PlPosSX, PlPosSY;

    float Px, Py;
    float NPx, NPy;
    float dx, dy;

    NPx = GetPX() + (PlPosSX + .5); // its playerpos here...
    NPy = GetPY() + (PlPosSY + .5); // why? copy/paste/change a bit...easy

    Px = GegnerX[a];
    Py = GegnerY[a];
    if (GegnerSY[a] >= -.5)
        Py++;
    if (GegnerSY[a] <= .5)
        Py--;
    if (GegnerSX[a] >= -.5)
        Px++;
    if (GegnerSX[a] <= .5)
        Px--;
    Px += GegnerSX[a] + .5;
    Py += GegnerSY[a] + .5;

    dx = Px - NPx;
    if (dx < 0)
        dx *= -1;
    dy = Py - NPy;
    if (dy < 0)
        dy *= -1;

    // now we got the distance, and can look how to turn the NPC
    GegnerRichtung[a] = -1;
    if (Px > NPx) {                // right
        if ((dx / 2) > dy) {       // more side then up/down
            GegnerRichtung[a] = 2; // right
        }
        if ((dy / 2) > dx) { // more up/down then side
            if (Py > NPy)
                GegnerRichtung[a] = 4; // down
            else
                GegnerRichtung[a] = 0; // up
        }
        if (GegnerRichtung[a] == -1) { // both
            if (Py > NPy)
                GegnerRichtung[a] = 3; // down-right
            else
                GegnerRichtung[a] = 1; // up-right
        }
    } else {                       // left
        if ((dx / 2) > dy) {       // more side then up/down
            GegnerRichtung[a] = 6; // left
        }
        if ((dy / 2) > dx) { // more up/down then side
            if (Py > NPy)
                GegnerRichtung[a] = 4; // down
            else
                GegnerRichtung[a] = 0; // up
        }
        if (GegnerRichtung[a] == -1) { // both
            if (Py > NPy)
                GegnerRichtung[a] = 5; // down-left
            else
                GegnerRichtung[a] = 7; // up-left
        }
    }
}

float gethposfromenemy(int a, float sx, float sy)
{
    extern float GetInterPolY(int x, int y, float sx, float sy);
    float vsx, vsy;
    int vx, vy;

    vx  = GegnerX[a];
    vy  = GegnerY[a];
    vsx = GegnerSX[a] + sx;
    vsy = GegnerSY[a] + sy;
    if (vsy >= -.5)
        vy++;
    if (vsy <= .5)
        vy--;
    if (vsx >= -.5)
        vx++;
    if (vsx <= .5)
        vx--; // This is needed to get a better position
    vsx = GegnerSX[a] + .5;
    vsy = GegnerSY[a] + .5;

    return GetInterPolY(vx, vy, vsx, vsy);
}

void UpdateEnemy()
{
    bool l, r, u, d;
    extern float PlPosSX, PlPosSY;
    extern int screenmode;
    extern int PlStatus;

    int a;
    // int b;
    float sx, sy;
    float Px, Py;
    float NPx, NPy;
    float dx, dy;
    bool change;
    int NPCnum;

    if (screenmode < 3) {
        for (a = 0; a <= GegnerCount; a++) {
            // check coolision just if enemy walks or follows player
            if (GegnerStatus[a] < 1) {
                l = true;
                r = true;
                u = true;
                d = true;

                sx = (GegnerSX[a]) * 20;
                sy = (GegnerSY[a]) * 20;

                bool val[8];
                Checkcolision(GegnerX[a], GegnerY[a], sx + 10, sy + 10, 5, (bool *)&val);
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

                Px = GegnerX[a];
                Py = GegnerY[a];
                if (GegnerSY[a] >= -.5)
                    Py++;
                if (GegnerSY[a] <= .5)
                    Py--;
                if (GegnerSX[a] >= -.5)
                    Px++;
                if (GegnerSX[a] <= .5)
                    Px--;
                Px += GegnerSX[a] + .5;
                Py += GegnerSY[a] + .5;
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
                GegnerStatus[a] = -1; // enemy wont follow player
                if (PlStatus != 3) {
                    if (dx > dy)
                        if (Py > NPy - (.9 + GegnerRadius[a]) && Py < NPy + (.9 + GegnerRadius[a])) {
                            if (Px < NPx && Px + (.9 + GegnerRadius[a]) > NPx) {
                                GegnerStatus[a] = 0;
                                TurnittoPlayer(a);
                            } // follow the player}
                            if (Px > NPx && Px - (.9 + GegnerRadius[a]) < NPx) {
                                GegnerStatus[a] = 0;
                                TurnittoPlayer(a);
                            } // follow the player}
                        }
                    if (dy > dx)
                        if (Px > NPx - (.9 + GegnerRadius[a]) && Px < NPx + (.9 + GegnerRadius[a])) {
                            if (Py < NPy && Py + (.9 + GegnerRadius[a]) > NPy) {
                                GegnerStatus[a] = 0;
                                TurnittoPlayer(a);
                            } // follow the player}
                            if (Py > NPy && Py - (.9 + GegnerRadius[a]) < NPy) {
                                GegnerStatus[a] = 0;
                                TurnittoPlayer(a);
                            } // follow the player}
                        }
                    // near enough to attack and to near to get closer
                    if (dx > dy)
                        if (Py > NPy - GegnerRadius[a] && Py < NPy + GegnerRadius[a]) {
                            if (Px < NPx && Px + GegnerRadius[a] > NPx) {
                                r = false;
                                if (rand() % 17 == 2)
                                    GegnerStatus[a] = 1;
                            }
                            if (Px > NPx && Px - GegnerRadius[a] < NPx) {
                                l = false;
                                if (rand() % 17 == 2)
                                    GegnerStatus[a] = 1;
                            }
                        }
                    if (dy > dx)
                        if (Px > NPx - GegnerRadius[a] && Px < NPx + GegnerRadius[a]) {
                            if (Py < NPy && Py + GegnerRadius[a] > NPy) {
                                d = false;
                                if (rand() % 17 == 2)
                                    GegnerStatus[a] = 1;
                            }
                            if (Py > NPy && Py - GegnerRadius[a] < NPy) {
                                u = false;
                                if (rand() % 17 == 2)
                                    GegnerStatus[a] = 1;
                            }
                        }
                }
                // Chrash mit anderen NPCs verhindern
                NPx = GegnerX[a];
                NPy = GegnerY[a];

                for (NPCnum = 0; NPCnum <= GegnerCount; NPCnum++) {
                    if (NPCnum != a) {
                        Px = GegnerX[NPCnum];
                        Py = GegnerY[NPCnum];
                        if (GegnerSY[NPCnum] >= -.5)
                            Py++;
                        if (GegnerSY[NPCnum] <= .5)
                            Py--;
                        if (GegnerSX[NPCnum] >= -.5)
                            Px++;
                        if (GegnerSX[NPCnum] <= .5)
                            Px--;
                        Px += GegnerSX[NPCnum] + .5;
                        Py += GegnerSY[NPCnum] + .5;
                        // Wow strange code....but should work to compare those positions right
                        // first check distance
                        dx = Px - NPx;
                        if (dx < 0)
                            dx *= -1;
                        dy = Py - NPy;
                        if (dy < 0)
                            dy *= -1;
                        // now lets compace
                        if (dx > dy)
                            if (Py > NPy - GegnerRadius[NPCnum] && Py < NPy + GegnerRadius[NPCnum] && GegnerStatus[NPCnum] != 3) {
                                if (Px < NPx && Px + GegnerRadius[NPCnum] > NPx)
                                    r = false;
                                if (Px > NPx && Px - GegnerRadius[NPCnum] < NPx)
                                    l = false;
                            }
                        if (dy > dx)
                            if (Px > NPx - GegnerRadius[NPCnum] && Px < NPx + GegnerRadius[NPCnum] && GegnerStatus[NPCnum] != 3) {
                                if (Py < NPy && Py + GegnerRadius[NPCnum] > NPy)
                                    d = false;
                                if (Py > NPy && Py - GegnerRadius[NPCnum] < NPy)
                                    u = false;
                            }
                    }
                }
            }

            // Movement
            // Direction change (random walking)
            if (GegnerStatus[a] == -1) {
                change = false;
                if ((GegnerRichtung[a] == 0 || GegnerRichtung[a] == 1 || GegnerRichtung[a] == 7) && d == false)
                    change = true;
                if ((GegnerRichtung[a] == 1 || GegnerRichtung[a] == 2 || GegnerRichtung[a] == 3) && l == false)
                    change = true;
                if ((GegnerRichtung[a] == 6 || GegnerRichtung[a] == 5 || GegnerRichtung[a] == 7) && r == false)
                    change = true;
                if ((GegnerRichtung[a] == 4 || GegnerRichtung[a] == 3 || GegnerRichtung[a] == 5) && u == false)
                    change = true;
                if (rand() % 60 == 0)
                    change = true;

                if (change)
                    TurnEnemy(a, l, r, u, d);
            }

            if (GegnerStatus[a] < 1) {
                // Horizontal
                if ((GegnerRichtung[a] == 6 || GegnerRichtung[a] == 5 || GegnerRichtung[a] == 7) && r)
                    GegnerSX[a] += .025;
                if (GegnerSX[a] > .5) {
                    GegnerX[a] += 1;
                    GegnerSX[a] -= 1;
                }
                if ((GegnerRichtung[a] == 1 || GegnerRichtung[a] == 2 || GegnerRichtung[a] == 3) && l)
                    GegnerSX[a] -= .025;
                if (GegnerSX[a] < -.5) {
                    GegnerX[a] -= 1;
                    GegnerSX[a] += 1;
                }
                // Vertikal
                if ((GegnerRichtung[a] == 0 || GegnerRichtung[a] == 1 || GegnerRichtung[a] == 7) && d)
                    GegnerSY[a] += .025;
                if (GegnerSY[a] > .5) {
                    GegnerY[a] += 1;
                    GegnerSY[a] -= 1;
                }
                if ((GegnerRichtung[a] == 4 || GegnerRichtung[a] == 3 || GegnerRichtung[a] == 5) && u)
                    GegnerSY[a] -= .025;
                if (GegnerSY[a] < -.5) {
                    GegnerY[a] -= 1;
                    GegnerSY[a] += 1;
                }
            }
        }
    }
}
