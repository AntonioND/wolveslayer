#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/Output/Textbox.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Villagers.h"

int VillagerX[Villagers_Max], VillagerY[Villagers_Max];     // its the HardPos on the Map(on which tile it stands)
float VillagerSX[Villagers_Max], VillagerSY[Villagers_Max]; // Its softpos...goes from -.5 to .5
int VillagerTextNum[Villagers_Max];                 // The Index for texture
int VillagerRichtung[Villagers_Max];                // The Direcction for each NPC
int VillagerCount = -1;                        // Its the count of NPCs
char VillagerSpeech[Villagers_Max][10][256];        // to hold 10 textes for each NPC with 300 chars
int VillagerSpeechCount[Villagers_Max];             // Holds the Number of speeches a NPC has...

int SpriteX, SpriteY;
float SpriteSX, SpriteSY;

void SetCurSprite(int x, int y, float sx, float sy)
{
    SpriteX  = x;
    SpriteY  = y;
    SpriteSX = sx;
    SpriteSY = sy;
}

void ResetVillagers(void)
{
    VillagerCount = -1;

    for (int a = 0; a < Villagers_Max; a++) {
        VillagerX[a]        = -1;
        VillagerY[a]        = -1;
        VillagerSX[a]       = 0;
        VillagerSY[a]       = 0;
        VillagerTextNum[a]  = -1;
        VillagerRichtung[a] = 0;
        for (int b = 0; b < 10; b++)
            strcpy(VillagerSpeech[a][b], "");
        VillagerSpeechCount[a] = 0;
    }
}

void AddTexttoVillager(char txt[256])
{
    if (VillagerSpeechCount[VillagerCount] < 10) {
        strcpy(VillagerSpeech[VillagerCount][VillagerSpeechCount[VillagerCount]], txt);
        VillagerSpeechCount[VillagerCount]++;
    }
}

void AddVillager(int x, int y, int texnum)
{
    if (Villagers_Max > VillagerCount + 1) {
        VillagerCount++;
        VillagerX[VillagerCount]       = x;
        VillagerY[VillagerCount]       = y;
        VillagerTextNum[VillagerCount] = texnum;
    }
}

void TurnVillager(int a, bool l, bool r, bool u, bool d)
{
    // Now give a new valid dirrecion if possible
    if (!l && !d && !r && !u)
        return;

    int dirnew = 0;
    bool ok    = false;

    while (ok == false) {
        ok = true;

        int dirnew = rand() % 8;
        if ((dirnew == 0 || dirnew == 1 || dirnew == 7) && d == false)
            ok = false;
        if ((dirnew == 1 || dirnew == 2 || dirnew == 3) && l == false)
            ok = false;
        if ((dirnew == 6 || dirnew == 5 || dirnew == 7) && r == false)
            ok = false;
        if ((dirnew == 4 || dirnew == 3 || dirnew == 5) && u == false)
            ok = false;
    }

    VillagerRichtung[a] = dirnew;
}

void UpdateVillagers()
{
    bool l, r, u, d;

    if (screenmode < 3) {
        for (int a = 0; a <= VillagerCount; a++) {
            l = true;
            r = true;
            u = true;
            d = true;

            float sx = (VillagerSX[a]) * 10;
            float sy = (VillagerSY[a]) * 10;

            for (int b = 2; b < 8; b++) {
                if (Passable(VillagerX[a], VillagerY[a], sx + b, sy) == false)
                    u = false;
                if (Passable(VillagerX[a], VillagerY[a], sx + b, sy + 8) == false)
                    d = false;
                if (Passable(VillagerX[a], VillagerY[a], sx + 1, sy - 1 + b) == false)
                    l = false;
                if (Passable(VillagerX[a], VillagerY[a], sx + 8, sy - 1 + b) == false)
                    r = false;
            }

            // Chrash mit Player verhindern
            float Px, Py;
            float NPx, NPy;
            float dx, dy;
            NPx = GetPX() + (PlPosSX + .5); // its playerpos here...
            NPy = GetPY() + (PlPosSY + .5); // why? copy/paste/change a bit...easy

            Px = VillagerX[a];
            Py = VillagerY[a];
            if (VillagerSY[a] >= -.5)
                Py++;
            if (VillagerSY[a] <= .5)
                Py--;
            if (VillagerSX[a] >= -.5)
                Px++;
            if (VillagerSX[a] <= .5)
                Px--;
            Px += VillagerSX[a] + .5;
            Py += VillagerSY[a] + .5;
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
                if (Py > NPy - .6 && Py < NPy + .6) {
                    if (Px < NPx && Px + .6 > NPx)
                        r = false;
                    if (Px > NPx && Px - .6 < NPx)
                        l = false;
                }
            if (dy > dx)
                if (Px > NPx - .6 && Px < NPx + .6) {
                    if (Py < NPy && Py + .6 > NPy)
                        d = false;
                    if (Py > NPy && Py - .6 < NPy)
                        u = false;
                }

            // Chrash mit anderen NPCs verhindern
            int NPCnum;
            // NPx = GetPX() + (PlPosSX + .5); // its playerpos here...
            // NPy = GetPY() + (PlPosSY + .5); // why? copy/paste/change a bit...easy
            NPx = VillagerX[a];
            NPy = VillagerY[a];

            for (NPCnum = 0; NPCnum <= VillagerCount; NPCnum++) {
                if (NPCnum != a) {
                    Px = VillagerX[NPCnum];
                    Py = VillagerY[NPCnum];
                    if (VillagerSY[NPCnum] >= -.5)
                        Py++;
                    if (VillagerSY[NPCnum] <= .5)
                        Py--;
                    if (VillagerSX[NPCnum] >= -.5)
                        Px++;
                    if (VillagerSX[NPCnum] <= .5)
                        Px--;
                    Px += VillagerSX[NPCnum] + .5;
                    Py += VillagerSY[NPCnum] + .5;
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
                        if (Py > NPy - .6 && Py < NPy + .6) {
                            if (Px < NPx && Px + .6 > NPx)
                                r = false;
                            if (Px > NPx && Px - .6 < NPx)
                                l = false;
                        }
                    if (dy > dx)
                        if (Px > NPx - .6 && Px < NPx + .6) {
                            if (Py < NPy && Py + .6 > NPy)
                                d = false;
                            if (Py > NPy && Py - .6 < NPy)
                                u = false;
                        }
                }
            }

            // Movement
            if (screenmode != 2 || a != npctalk) { // talking NPCs cant move
                // Direction change
                bool change = false;
                if ((VillagerRichtung[a] == 0 || VillagerRichtung[a] == 1 || VillagerRichtung[a] == 7) && d == false)
                    change = true;
                if ((VillagerRichtung[a] == 1 || VillagerRichtung[a] == 2 || VillagerRichtung[a] == 3) && l == false)
                    change = true;
                if ((VillagerRichtung[a] == 6 || VillagerRichtung[a] == 5 || VillagerRichtung[a] == 7) && r == false)
                    change = true;
                if ((VillagerRichtung[a] == 4 || VillagerRichtung[a] == 3 || VillagerRichtung[a] == 5) && u == false)
                    change = true;
                if (rand() % 60 == 0)
                    change = true;

                if (change)
                    TurnVillager(a, l, r, u, d);

                // Horizontal
                if ((VillagerRichtung[a] == 6 || VillagerRichtung[a] == 5 || VillagerRichtung[a] == 7) && r)
                    VillagerSX[a] += .025;
                if (VillagerSX[a] > .5) {
                    VillagerX[a] += 1;
                    VillagerSX[a] -= 1;
                }
                if ((VillagerRichtung[a] == 1 || VillagerRichtung[a] == 2 || VillagerRichtung[a] == 3) && l)
                    VillagerSX[a] -= .025;
                if (VillagerSX[a] < -.5) {
                    VillagerX[a] -= 1;
                    VillagerSX[a] += 1;
                }
                // Vertikal
                if ((VillagerRichtung[a] == 0 || VillagerRichtung[a] == 1 || VillagerRichtung[a] == 7) && d)
                    VillagerSY[a] += .025;
                if (VillagerSY[a] > .5) {
                    VillagerY[a] += 1;
                    VillagerSY[a] -= 1;
                }
                if ((VillagerRichtung[a] == 4 || VillagerRichtung[a] == 3 || VillagerRichtung[a] == 5) && u)
                    VillagerSY[a] -= .025;
                if (VillagerSY[a] < -.5) {
                    VillagerY[a] -= 1;
                    VillagerSY[a] += 1;
                }
            }
        }
    }
}

void LoadSpriteTexture(char filename[], char palname[], int *Target, int num)
{
    u8 *buffer;
    u16 *pal;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &buffer, &pal, &height, &width);

    glGenTextures(1, &Target[num]);
    glBindTexture(0, Target[num]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 128, 256, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffer);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    free(buffer);
    free(pal);
}
