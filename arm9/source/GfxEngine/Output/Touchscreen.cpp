// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include <math.h>

#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/Output/Inventory.h"
#include "GfxEngine/Output/Items.h"
#include "GfxEngine/Output/Textbox.h"
#include "GfxEngine/Output/Touchscreen.h"

// extern u8 textbox_bin[246 * 96];

u8 Font1[668 * 11];
u16 scrL_bin[256 * 192];
u16 touch_bin[256 * 192];
u16 textbox_pal[256];

ScreenModes screenmode = ScreenModeNormal;

void ScreenModeLOADING(void)
{
    screenmode = ScreenModeLoading;

    WaitForFreeVblank();
    for (int i = 0; i < 256 * 192; i++)
        BG_GFX_SUB[i] = scrL_bin[i];
}

void UpdateBar(void)
{
    int PX = 57;
    int PY = 148;
    int PW = 196;

    u16 col;
    col = RGB15(0, 31, 0) | BIT(15);
    if (PlHP <= 50)
        col = RGB15(0, 31, 31) | BIT(15);
    if (PlHP <= 33)
        col = RGB15(31, 15, 15) | BIT(15);

    // Draw Progressbar
    float curpro = (float)((float)PlHP / 100) * PW; // Yeah now it gets complicated (-:
    for (int c = PX; c <= PX + curpro; c++)
    {
        for (int b = 0; b < 7; b++)
            BG_GFX_SUB[c + ((PY + b) * 256)] = col;
    }

    // refresh rest
    if (PlHP != 100)
    {
        for (int c = PX + curpro; c <= PX + PW; c++)
        {
            for (int b = 0; b < 7; b++)
                BG_GFX_SUB[c + ((PY + b) * 256)] = ((u16 *)touch_bin)[c + (PY * 256)];
        }
    }
}

void ScreenMode(void)
{
    screenmode = ScreenModeNormal;

    // Draw IngameBG
    WaitForFreeVblank();

    for (int i = 0; i < 256 * 256; i++)
        BG_GFX_SUB[i] = ((u16 *)touch_bin)[i];

    UpdateBar();
}

void ScreenModeHandler(void)
{
    if (screenmode != ScreenModeTextBox)
        return;

    if ((keysDown() & (KEY_A | KEY_B)) && CurWord == wordnum && nextpage == false)
    {
        npctalk = -1;
        ScreenMode();
        CurWord = -1;
        wordnum = -1;
    }

    if ((keysDown() & (KEY_A | KEY_B)) && nextpage == true)
        ShownextPage();
}

void Print(const char *Text, int x, int y)
{
    int Counter;
    char Char;
    int i, j;

    int sw = 7, sh = 10;

    for (Counter = 0; Counter <= 255; Counter++)
    {
        Char = Text[Counter] - 32;
        for (int xx = Char * sw; xx <= Char * sw + (sw - 1); xx++)
        {
            for (int yy = 0; yy <= sh; yy++)
            {
                // Writes Font in black
                if (Font1[xx + yy * 668] == 0)
                {
                    i = x + xx - Char * sw + Counter * (sw + 1);
                    j = y + yy;
                    if (i < 256 && j < 192)
                    {
                        if (i > -1 && j > -1)
                            BG_GFX_SUB[i + (j * 256)] = RGB15(31, 31, 31) | BIT(15);
                    }
                }

                // Restores BG
                if (Font1[xx + yy * 668] == 1)
                {
                    i = x + xx - Char * sw + Counter * (sw + 1);
                    j = y + yy;

                    if (screenmode == ScreenModeNormal)
                    {
                        if (i < 256 && j < 192)
                        {
                            if (i > -1 && j > -1)
                                BG_GFX_SUB[i + (j * 256)] = ((u16 *)touch_bin)[i + (j * 256)] | BIT(15);
                        }
                    }

                    if (screenmode == ScreenModeLoading)
                    {
                        if (i < 256 && j < 192)
                        {
                            if (i > -1 && j > -1)
                                BG_GFX_SUB[i + (j * 256)] = ((u16 *)scrL_bin)[i + (j * 256)] | BIT(15);
                        }
                    }
                }
            }
        }

        if (Text[Counter + 1] == '\0')
            Counter = 255;
    }
}

void PrintOUT(const char *Text, int x, int y, bool color, int n)
{
    int sw = 7, sh = 10;

    for (int Counter = 0; Counter < n; Counter++)
    {
        char Char = Text[Counter] - 32;
        for (int xx = Char * sw; xx <= Char * sw + (sw - 1); xx++)
        {
            for (int yy = 0; yy <= sh; yy++)
            {
                // Writes Font in black
                if (Font1[xx + yy * 668] == 0)
                {
                    int i = x + xx - Char * sw + Counter * (sw + 1);
                    int j = y + yy;
                    if ((i < 256 && j < 192) && (i > -1 && j > -1))
                    {
                        if (color)
                            BG_GFX_SUB[i + (j * 256)] = RGB15(31, 31, 31) | BIT(15);
                        else
                            BG_GFX_SUB[i + (j * 256)] = RGB15(0, 0, 0) | BIT(15);
                    }
                }
            }
        }
        if (Text[Counter + 1] == '\0')
            Counter = 255;
    }
}

void PercentBar(int cur, int max)
{
    // Draw Box
    int PX = 62;
    int PY = 95;
    int PW = 131;

#if 0
    // This draws a bigger rectangle as a progress bar, but it doesn't look well
    // with the current background.

    for (int a = PX; a <= PX + PW; a++) {
        BG_GFX_SUB[a + (PY * 256)]       = RGB15(31, 31, 31) | BIT(15);
        BG_GFX_SUB[a + ((PY + 8) * 256)] = RGB15(31, 31, 31) | BIT(15);

        for (int b = PY; b <= PY + 8; b++) {
            BG_GFX_SUB[PX + (b * 256)]      = RGB15(31, 31, 31) | BIT(15);
            BG_GFX_SUB[PX + PW + (b * 256)] = RGB15(31, 31, 31) | BIT(15);
        }
    }
#endif

    // Draw Progressbar
    float curpro = (float)((float)cur / max) * (PW); // Yeah now it gets complicated (-:

    for (int c = PX; c <= PX + curpro; c++)
    {
        BG_GFX_SUB[c + (PY * 256)]       = RGB15(31, 31, 31) | BIT(15);
        BG_GFX_SUB[c + ((PY + 1) * 256)] = RGB15(31, 31, 31) | BIT(15);
    }
}

void ItemMode(void)
{
    int i, j;
    screenmode = ScreenModeItem;

#if 1
    // Draw IngameBG
    WaitForFreeVblank();
    for (i = 0; i < 256 * 256; i++)
        BG_GFX_SUB[i] = ((u16 *)touch_bin)[i];
#else
    // TODO: Restore the images below instead of using touch_bin

    // Draw IngameBG
    WaitForFreeVblank();
    // for (i = 0; i < 256 * 256; i++)
    //     BG_GFX_SUB[i] = ((u16 *)touchmenu_bin)[i];

    // Draw Buttons
    for (i = 0; i < 64; i++)
    {
        for (j = 0; j < 32; j++)
        {
            BG_GFX_SUB[(i + 29) + ((j + 144) * 256)]       = ((u16 *)buttonsel_bin)[i + (j * 64)];
            BG_GFX_SUB[(i + 29 + 130) + ((j + 144) * 256)] = ((u16 *)button_bin)[i + (j * 64)];

            if (GegnerCount == -1)
            {
                if (EquipedWeapon != CellSelect)
                    if (strncmp("WEAPON", GiveItemType(Inventory[CellSelect]), 6) == 0)
                        BG_GFX_SUB[(i + 45) + ((j + 87) * 256)] = ((u16 *)button_bin)[i + (j * 64)];

                if (EquipedWeapon == CellSelect)
                    if (strncmp("WEAPON", GiveItemType(Inventory[CellSelect]), 6) == 0)
                        BG_GFX_SUB[(i + 45) + ((j + 87) * 256)] = ((u16 *)buttonsel_bin)[i + (j * 64)];
            }
        }
    }
#endif

    PrintOUT("Item", 43, 154, true, strlen("Item"));
    PrintOUT("Back", 42 + 131, 154, true, strlen("Back"));

    PrintOUT("Item", 44, 155, false, strlen("Item"));
    PrintOUT("Back", 43 + 131, 155, false, strlen("Back"));

    for (int a = 0; a < 10; a++)
    {
        if (a == CellSelect)
        {
            for (i = 0; i < 93; i++)
                for (j = 0; j < 9; j++)
                    BG_GFX_SUB[(i + 134) + ((j + 22 + (a * 10)) * 256)] = RGB15(0, 0, 31) | BIT(15);
        }

        if (Inventory[a] == -1)
            PrintOUT("<none>", 135, 21 + (a * 10), true, strlen("<none>"));
        else
            PrintOUT(GiveItemName(Inventory[a]), 135, 21 + (a * 10), true, strlen(GiveItemName(Inventory[a])));
    }

    PrintOUT(GiveItemName(Inventory[CellSelect]), 30, 24, true, strlen(GiveItemName(Inventory[CellSelect])));
    PrintOUT(GiveItemType(Inventory[CellSelect]), 55, 74, true, strlen(GiveItemType(Inventory[CellSelect])));

    if (strlen(GiveItemImgName(Inventory[CellSelect])) > 2)
    {
        u16 Img[32 * 32];
        LoadBmptoBuffer(GiveItemImgName(Inventory[CellSelect]), Img);

        for (i = 0; i < 32; i++)
            for (j = 0; j < 32; j++)
                BG_GFX_SUB[(i + 63) + ((j + 40) * 256)] = ((u16 *)Img)[i + (j * 32)];
    }
}

void PauseMode(void)
{
    screenmode = ScreenModePause;

    // Draw IngameBG
    WaitForFreeVblank();
    for (int i = 0; i < 256 * 256; i++)
        BG_GFX_SUB[i] = ((u16 *)touch_bin)[i];

#if 0
    // TODO: Restore image
    // Draw Buttons
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 32; j++) {
            BG_GFX_SUB[(i + 29 + 130) + ((j + 144) * 256)] = ((u16 *)buttonsel_bin)[i + (j * 64)];
        }
    }
#endif

    PrintOUT("Item", 43, 154, true, strlen("Item"));
    PrintOUT("Back", 42 + 131, 154, true, strlen("Back"));

    PrintOUT("Item", 44, 155, false, strlen("Item"));
    PrintOUT("Back", 43 + 131, 155, false, strlen("Back"));
}
