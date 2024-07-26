// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/3D.h"
#include "GfxEngine/Output/Items.h"
#include "GfxEngine/Output/Touchscreen.h"

int npctalk = -1;

// The valid text holder...
static char Txt[200][20];
static int Ni, Nj;
static unsigned int CurChar;

int CurWord;
int wordnum;

bool nextpage;

void TextBoxmode(char TxtOffset[10][256], int cnt, int num)
{
#if 0
    // TODO: Restore this
    // extern u8 textbox_bin[256 * 96];
    // extern u16 textbox_pal[256];
#endif

    char *chunker;
    char Temp[256];
    // Lets chunk the text in sinlge owrds first..
    // Just textes with true-values (after compare) should be added...
    // it will turn out a valid text for that defined case...hopefully
    wordnum = 0;
    for (int a = 0; a < 10; a++)
    {
        if (screenmode != ScreenModeTextBox && cnt > a)
        {
            // we start with next #1
            strcpy(Temp, TxtOffset[a]);
            chunker = strtok(Temp, " ");

            // lets splitt...
            bool check = true;
            while (chunker != NULL && wordnum < 200)
            {
                // if word is a comparecommand and if its true or if word is just a word...
                if (strncmp("[", chunker, 1) != 0 && check == true)
                {
                    strcpy(Txt[wordnum], chunker);
                    wordnum++;
                }

                // Found a compare-thing...check if its true!...if not skip this page
                // It also could be a setfunction...in that case true will be given back and value setten like
                // it should be...hopefully
                if (strncmp("[", chunker, 1) == 0)
                {
                    check = ItemCheck_Set(chunker);
                }

                // next word please
                chunker = strtok(NULL, " ");
            }
        }
    }

    if (wordnum > 0)
    {
        if (num != -1)
            npctalk = num;

#if 0
        // Blue box
        WaitForFreeVblank();

        // TODO: Restore this
        for (int i = 0; i < 256; i++)
            for (int j = 0; j < 96; j++)
                if (textbox_bin[i + (j * 256)] != 0)
                    BG_GFX_SUB[i + ((j + 5) * 256)] = textbox_pal[textbox_bin[i + (j * 256)]];
#endif

        Ni = 0;
        Nj = 0;

        CurChar    = 1;
        CurWord    = 0;
        screenmode = ScreenModeTextBox;
        nextpage   = false;
    }
}

void TextBoxmodeHandler(void)
{
    if (CurWord < wordnum && Nj <= 80)
    {
        if (CurChar == 1)
        {
            Ni += (1 + strlen(Txt[CurWord])) * 8;
            if (Ni > 236)
            {
                Ni = (1 + strlen(Txt[CurWord])) * 8;
                Nj += 10;
            }
        }

        if (Nj <= 50)
            PrintOUT(Txt[CurWord], 11 + Ni - ((1 + strlen(Txt[CurWord])) * 8), 22 + Nj, false, CurChar);
        if (Nj <= 50)
            PrintOUT(Txt[CurWord], 11 + Ni - ((1 + strlen(Txt[CurWord])) * 8) + 1, 22 + Nj + 1, true, CurChar);

        CurChar++;
        if (CurChar > strlen(Txt[CurWord]))
        {
            CurWord++;
            CurChar = 1;
        }
    }
    if (Nj > 80)
        nextpage = true;
}

void ShownextPage(void)
{
    WaitForFreeVblank();

    for (int i = 5; i < 251; i++)
        for (int j = 5; j < 101; j++)
            BG_GFX_SUB[i + (j * 256)] = RGB15(0, 0, 31) | BIT(15);

    Nj -= 90;
    nextpage = false;
}
