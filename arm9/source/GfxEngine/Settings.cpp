// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/3D.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/Output/Inventory.h"
#include "GfxEngine/Output/Items.h"
#include "GfxEngine/Script/Script_Load.h"

// Those setting are for new game choice
static char StartMap[60];
static int StartMapX, StartMapY;

char MaincharMd2[60];
char MaincharText[60];

char weaponMd2[60];
char weaponText[60];

static void Setit(char *setting)
{
    char Filename[40];

    // Readout startmap
    if (strncmp("StartMap ", setting, 9) == 0)
    {
        sscanf(setting, "%*s %39s", Filename);
        snprintf(StartMap, sizeof(StartMap), "/wolveslayer/maps/%s", Filename);
    }

    if (strncmp("StartPos ", setting, 9) == 0)
    {
        sscanf(setting, "%*s %d", &StartMapX);
        sscanf(setting, "%*s %*d %d", &StartMapY);
    }

    if (strncmp("StartChar ", setting, 10) == 0)
    {
        sscanf(setting, "%*s %39s", Filename);
        snprintf(MaincharMd2, sizeof(MaincharMd2), "/wolveslayer/chars/%s", Filename);

        sscanf(setting, "%*s %*s %39s", Filename);
        snprintf(MaincharText, sizeof(MaincharText), "/wolveslayer/chars/%s", Filename);
    }

    if (strncmp("StartWeapon ", setting, 11) == 0)
    {
        sscanf(setting, "%*s %39s", Filename);
        snprintf(weaponMd2, sizeof(weaponMd2), "/wolveslayer/items/%s", Filename);

        sscanf(setting, "%*s %*s %39s", Filename);
        snprintf(weaponText, sizeof(weaponText), "/wolveslayer/items/%s", Filename);
    }
}

void ReadSettings(void)
{
    char text[100];
    int count = 0;

    FILE *ScriptFile = fopen("/wolveslayer/Settings.txt", "r");
    if (ScriptFile != NULL)
    {
        while (!feof(ScriptFile))
        {
            fgets(text, 99, ScriptFile);

            if (text[0] != '/')
            {
                Setit(text);
                count++;
            }

            strcpy(text, "");
        }
        fclose(ScriptFile);
    }

    // Add playerpos
    PlPosX  = StartMapX - 6;
    PlPosY  = StartMapY - 8;
    CamPosX = StartMapX - 6;
    CamPosY = StartMapY - 8;

    // Start script file
    ClearItems();
    ResetInventory();
    LoadItemList();
    LoadScript(StartMap);
}
