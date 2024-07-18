#include "3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/input/Input.h"
#include "GfxEngine/output/Textbox.h"
#include "GfxEngine/output/Touchscreen.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Villagers.h"

int DorfiX[Dorf_Max], DorfiY[Dorf_Max];     // its the HardPos on the Map(on which tile it stands)
float DorfiSX[Dorf_Max], DorfiSY[Dorf_Max]; // Its softpos...goes from -.5 to .5
int DorfiTextNum[Dorf_Max];                 // The Index for texture
int DorfiRichtung[Dorf_Max];                // The Direcction for each NPC
int DorfiCount = -1;                        // Its the count of NPCs
char DorfiSpeech[Dorf_Max][10][256];        // to hold 10 textes for each NPC with 300 chars
int DorfiSpeechCount[Dorf_Max];             // Holds the Number of speeches a NPC has...

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
    DorfiCount = -1;

    for (int a = 0; a < Dorf_Max; a++) {
        DorfiX[a]        = -1;
        DorfiY[a]        = -1;
        DorfiSX[a]       = 0;
        DorfiSY[a]       = 0;
        DorfiTextNum[a]  = -1;
        DorfiRichtung[a] = 0;
        for (int b = 0; b < 10; b++)
            strcpy(DorfiSpeech[a][b], "");
        DorfiSpeechCount[a] = 0;
    }
}

void AddTexttoDorfi(char txt[256])
{
    if (DorfiSpeechCount[DorfiCount] < 10) {
        strcpy(DorfiSpeech[DorfiCount][DorfiSpeechCount[DorfiCount]], txt);
        DorfiSpeechCount[DorfiCount]++;
    }
}

void AddDorfi(int x, int y, int texnum)
{
    if (Dorf_Max > DorfiCount + 1) {
        DorfiCount++;
        DorfiX[DorfiCount]       = x;
        DorfiY[DorfiCount]       = y;
        DorfiTextNum[DorfiCount] = texnum;
    }
}

void TurnDorfi(int a, bool l, bool r, bool u, bool d)
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

    DorfiRichtung[a] = dirnew;
}

void UpdateDorfis()
{
    bool l, r, u, d;

    if (screenmode < 3) {
        for (int a = 0; a <= DorfiCount; a++) {
            l = true;
            r = true;
            u = true;
            d = true;

            float sx = (DorfiSX[a]) * 10;
            float sy = (DorfiSY[a]) * 10;

            for (int b = 2; b < 8; b++) {
                if (Passable(DorfiX[a], DorfiY[a], sx + b, sy) == false)
                    u = false;
                if (Passable(DorfiX[a], DorfiY[a], sx + b, sy + 8) == false)
                    d = false;
                if (Passable(DorfiX[a], DorfiY[a], sx + 1, sy - 1 + b) == false)
                    l = false;
                if (Passable(DorfiX[a], DorfiY[a], sx + 8, sy - 1 + b) == false)
                    r = false;
            }

            // Chrash mit Player verhindern
            float Px, Py;
            float NPx, NPy;
            float dx, dy;
            NPx = GetPX() + (PlPosSX + .5); // its playerpos here...
            NPy = GetPY() + (PlPosSY + .5); // why? copy/paste/change a bit...easy

            Px = DorfiX[a];
            Py = DorfiY[a];
            if (DorfiSY[a] >= -.5)
                Py++;
            if (DorfiSY[a] <= .5)
                Py--;
            if (DorfiSX[a] >= -.5)
                Px++;
            if (DorfiSX[a] <= .5)
                Px--;
            Px += DorfiSX[a] + .5;
            Py += DorfiSY[a] + .5;
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
            NPx = DorfiX[a];
            NPy = DorfiY[a];

            for (NPCnum = 0; NPCnum <= DorfiCount; NPCnum++) {
                if (NPCnum != a) {
                    Px = DorfiX[NPCnum];
                    Py = DorfiY[NPCnum];
                    if (DorfiSY[NPCnum] >= -.5)
                        Py++;
                    if (DorfiSY[NPCnum] <= .5)
                        Py--;
                    if (DorfiSX[NPCnum] >= -.5)
                        Px++;
                    if (DorfiSX[NPCnum] <= .5)
                        Px--;
                    Px += DorfiSX[NPCnum] + .5;
                    Py += DorfiSY[NPCnum] + .5;
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
                if ((DorfiRichtung[a] == 0 || DorfiRichtung[a] == 1 || DorfiRichtung[a] == 7) && d == false)
                    change = true;
                if ((DorfiRichtung[a] == 1 || DorfiRichtung[a] == 2 || DorfiRichtung[a] == 3) && l == false)
                    change = true;
                if ((DorfiRichtung[a] == 6 || DorfiRichtung[a] == 5 || DorfiRichtung[a] == 7) && r == false)
                    change = true;
                if ((DorfiRichtung[a] == 4 || DorfiRichtung[a] == 3 || DorfiRichtung[a] == 5) && u == false)
                    change = true;
                if (rand() % 60 == 0)
                    change = true;

                if (change)
                    TurnDorfi(a, l, r, u, d);

                // Horizontal
                if ((DorfiRichtung[a] == 6 || DorfiRichtung[a] == 5 || DorfiRichtung[a] == 7) && r)
                    DorfiSX[a] += .025;
                if (DorfiSX[a] > .5) {
                    DorfiX[a] += 1;
                    DorfiSX[a] -= 1;
                }
                if ((DorfiRichtung[a] == 1 || DorfiRichtung[a] == 2 || DorfiRichtung[a] == 3) && l)
                    DorfiSX[a] -= .025;
                if (DorfiSX[a] < -.5) {
                    DorfiX[a] -= 1;
                    DorfiSX[a] += 1;
                }
                // Vertikal
                if ((DorfiRichtung[a] == 0 || DorfiRichtung[a] == 1 || DorfiRichtung[a] == 7) && d)
                    DorfiSY[a] += .025;
                if (DorfiSY[a] > .5) {
                    DorfiY[a] += 1;
                    DorfiSY[a] -= 1;
                }
                if ((DorfiRichtung[a] == 4 || DorfiRichtung[a] == 3 || DorfiRichtung[a] == 5) && u)
                    DorfiSY[a] -= .025;
                if (DorfiSY[a] < -.5) {
                    DorfiY[a] -= 1;
                    DorfiSY[a] += 1;
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
