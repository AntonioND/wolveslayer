// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/3D.h"
#include "GfxEngine/Enemies.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/Render/Render.h"
#include "GfxEngine/Settings.h"
#include "GfxEngine/Texture/DynamicLights.h"
#include "GfxEngine/Texture/Light.h"
#include "Sound/Music.h"

char EventSpeech[EventSpeech_Max][256]; // to hold textes for event with 256 chars
int EventSpeechCount;                   // Holds the Number of speeches a event has...

void ResetEvent(void)
{
    for (int b = 0; b < EventSpeech_Max; b++)
        strcpy(EventSpeech[b], "");
    EventSpeechCount = 0;
}

void AddTexttoEvent(char txt[256])
{
    if (EventSpeechCount >= EventSpeech_Max)
        Crash("Too many event speeches");

    strcpy(EventSpeech[EventSpeechCount], txt);
    EventSpeechCount++;
}

bool Gameended = false;

void CheckEndGame(void)
{
    if (Gameended == true && (keysDown() & KEY_START))
    {
        Gameended = false;
        PlStatus  = 0;
        PlFrame   = 0;
        PlHP      = 100;

        for (int b = 8; b >= 0; b--)
        {
            Blend = b;
            RunTime();
            E3D_StartRender();
            UpdateDynamic();
            E3D_Render();
            E3D_EndRender();
        }

        ReadSettings();

        for (int b = 0; b <= 8; b++)
        {
            Blend = b;
            RunTime();
            E3D_StartRender();
            UpdateDynamic();
            E3D_Render();
            E3D_EndRender();
        }
    }

    if (Gameended == false)
    {
        // check if player died
        if (PlHP == 0)
        {
            Gameended = true;
            StartSong("/wolveslayer/bgfx/game-over.mod");
            Print("YOU SUCKED!!!", 80, 75);
            Print(" Restart with", 80, 85);
            Print("  Start-Key  ", 80, 95);
        }

        // check if endboss died
        for (int a = 0; a <= EnemyCount; a++)
        {
            if (Enemies[a].IsBoss == true && Enemies[a].HP <= 0)
            {
                Gameended = true;
                StartSong("/wolveslayer/bgfx/game-finished.mod");
                PlStatus = -1;
                PlFrame  = 0;
                Print(" YOU OWNED!!!", 80, 75);
                Print(" Restart with", 80, 85);
                Print("  Start-Key  ", 80, 95);
            }
        }
    }
}
