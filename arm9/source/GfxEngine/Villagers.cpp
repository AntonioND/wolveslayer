// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/Output/Textbox.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Villagers.h"

VillagerInfo Villager[Villagers_Max];

int VillagerCount = -1; // It's the count of NPCs

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

    for (int a = 0; a < Villagers_Max; a++)
    {
        Villager[a].X         = -1;
        Villager[a].Y         = -1;
        Villager[a].SX        = 0;
        Villager[a].SY        = 0;
        Villager[a].TextNum   = -1;
        Villager[a].Direction = 0;
        for (int b = 0; b < VillagerSpeeches_Max; b++)
            strcpy(Villager[a].Speech[b], "");
        Villager[a].SpeechCount = 0;
    }
}

void AddTexttoVillager(char txt[256])
{
    int count = Villager[VillagerCount].SpeechCount;
    if (count >= Villagers_Max)
        Crash("Too many text strings\n%s", __func__);

    strcpy(Villager[VillagerCount].Speech[count], txt);
    Villager[VillagerCount].SpeechCount++;
}

void AddVillager(int x, int y, int texnum)
{
    if (VillagerCount >= Villagers_Max)
        Crash("Too many villagers\n%s", __func__);

    VillagerCount++;
    Villager[VillagerCount].X       = x;
    Villager[VillagerCount].Y       = y;
    Villager[VillagerCount].TextNum = texnum;
}

void TurnVillager(int a, bool l, bool r, bool u, bool d)
{
    // Now give a new valid dirrecion if possible
    if (!l && !d && !r && !u)
        return;

    int dirnew = 0;
    bool ok    = false;

    while (ok == false)
    {
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

    Villager[a].Direction = dirnew;
}

void UpdateVillagers()
{
    if (screenmode >= ScreenModeItem)
        return;

    for (int a = 0; a <= VillagerCount; a++)
    {
        bool l = true;
        bool r = true;
        bool u = true;
        bool d = true;

        float sx = Villager[a].SX * 10;
        float sy = Villager[a].SY * 10;

        for (int b = 2; b < 8; b++)
        {
            if (Passable(Villager[a].X, Villager[a].Y, sx + b, sy) == false)
                u = false;
            if (Passable(Villager[a].X, Villager[a].Y, sx + b, sy + 8) == false)
                d = false;
            if (Passable(Villager[a].X, Villager[a].Y, sx + 1, sy - 1 + b) == false)
                l = false;
            if (Passable(Villager[a].X, Villager[a].Y, sx + 8, sy - 1 + b) == false)
                r = false;
        }

        // Prevent crash with player

        float NPx = GetPX() + (PlPosSX + .5); // its playerpos here...
        float NPy = GetPY() + (PlPosSY + .5); // why? copy/paste/change a bit...easy

        float Px = Villager[a].X;
        float Py = Villager[a].Y;
        if (Villager[a].SY >= -.5)
            Py++;
        if (Villager[a].SY <= .5)
            Py--;
        if (Villager[a].SX >= -.5)
            Px++;
        if (Villager[a].SX <= .5)
            Px--;
        Px += Villager[a].SX + .5;
        Py += Villager[a].SY + .5;
        // Wow strange code....but should work to compare those positions right
        // first check distance
        float dx = Px - NPx;
        if (dx < 0)
            dx *= -1;
        float dy = Py - NPy;
        if (dy < 0)
            dy *= -1;

        // now lets compare
        if (dx > dy)
        {
            if (Py > NPy - .6 && Py < NPy + .6)
            {
                if (Px < NPx && Px + .6 > NPx)
                    r = false;
                if (Px > NPx && Px - .6 < NPx)
                    l = false;
            }
        }
        if (dy > dx)
        {
            if (Px > NPx - .6 && Px < NPx + .6)
            {
                if (Py < NPy && Py + .6 > NPy)
                    d = false;
                if (Py > NPy && Py - .6 < NPy)
                    u = false;
            }
        }

        // Prevent crashes with other NPCs

        int NPCnum;
        // NPx = GetPX() + (PlPosSX + .5); // its playerpos here...
        // NPy = GetPY() + (PlPosSY + .5); // why? copy/paste/change a bit...easy
        NPx = Villager[a].X;
        NPy = Villager[a].Y;

        for (NPCnum = 0; NPCnum <= VillagerCount; NPCnum++)
        {
            if (NPCnum == a)
                continue;

            Px = Villager[NPCnum].X;
            Py = Villager[NPCnum].Y;
            if (Villager[NPCnum].SY >= -.5)
                Py++;
            if (Villager[NPCnum].SY <= .5)
                Py--;
            if (Villager[NPCnum].SX >= -.5)
                Px++;
            if (Villager[NPCnum].SX <= .5)
                Px--;
            Px += Villager[NPCnum].SX + .5;
            Py += Villager[NPCnum].SY + .5;
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
            {
                if (Py > NPy - .6 && Py < NPy + .6)
                {
                    if (Px < NPx && Px + .6 > NPx)
                        r = false;
                    if (Px > NPx && Px - .6 < NPx)
                        l = false;
                }
            }
            if (dy > dx)
            {
                if (Px > NPx - .6 && Px < NPx + .6)
                {
                    if (Py < NPy && Py + .6 > NPy)
                        d = false;
                    if (Py > NPy && Py - .6 < NPy)
                        u = false;
                }
            }
        }

        // Movement
        if (screenmode != ScreenModeTextBox || a != npctalk) // talking NPCs cant move
        {
            // Direction change
            bool change = false;
            if ((Villager[a].Direction == 0 || Villager[a].Direction == 1 || Villager[a].Direction == 7) && d == false)
                change = true;
            if ((Villager[a].Direction == 1 || Villager[a].Direction == 2 || Villager[a].Direction == 3) && l == false)
                change = true;
            if ((Villager[a].Direction == 6 || Villager[a].Direction == 5 || Villager[a].Direction == 7) && r == false)
                change = true;
            if ((Villager[a].Direction == 4 || Villager[a].Direction == 3 || Villager[a].Direction == 5) && u == false)
                change = true;
            if (rand() % 60 == 0)
                change = true;

            if (change)
                TurnVillager(a, l, r, u, d);

            // Horizontal
            if ((Villager[a].Direction == 6 || Villager[a].Direction == 5 || Villager[a].Direction == 7) && r)
                Villager[a].SX += .025;
            if (Villager[a].SX > .5)
            {
                Villager[a].X += 1;
                Villager[a].SX -= 1;
            }
            if ((Villager[a].Direction == 1 || Villager[a].Direction == 2 || Villager[a].Direction == 3) && l)
                Villager[a].SX -= .025;
            if (Villager[a].SX < -.5)
            {
                Villager[a].X -= 1;
                Villager[a].SX += 1;
            }
            // Vertikal
            if ((Villager[a].Direction == 0 || Villager[a].Direction == 1 || Villager[a].Direction == 7) && d)
                Villager[a].SY += .025;
            if (Villager[a].SY > .5)
            {
                Villager[a].Y += 1;
                Villager[a].SY -= 1;
            }
            if ((Villager[a].Direction == 4 || Villager[a].Direction == 3 || Villager[a].Direction == 5) && u)
                Villager[a].SY -= .025;
            if (Villager[a].SY < -.5)
            {
                Villager[a].Y -= 1;
                Villager[a].SY += 1;
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
