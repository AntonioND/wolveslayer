// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/Enemies.h"
#include "GfxEngine/Events.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Model/MD2Collision.h"
#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Output/Textbox.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Render/Autotiles.h"
#include "GfxEngine/Render/Doors.h"
#include "GfxEngine/Render/Ground.h"
#include "GfxEngine/Render/Render.h"
#include "GfxEngine/Render/Water.h"
#include "GfxEngine/Script/Script_Chars.h"
#include "GfxEngine/Script/Script_Objects.h"
#include "GfxEngine/Script/Script_Others.h"
#include "GfxEngine/Script/Script_Textures.h"
#include "GfxEngine/Script/Script_Tiles.h"
#include "GfxEngine/Texture/DynamicLights.h"
#include "GfxEngine/Villagers.h"
#include "Sound/Music.h"

static bool outside;

static void UnloadStuff(void)
{
    // Reset all datas
    ResetMapChange();
    glResetTextures();
    ResetVillagers();
    ResetEnemies();
    ResetEvent();

    for (int a = 0; a < Ground_Max; a++)
    {
        Ground[a].ColorKey    = 0;
        Ground[a].TransEnable = false;
        Ground[a].BumpEnable  = false;
    }
    for (int a = 0; a < Autotile_Max; a++)
    {
        Autotile[a].ColorKeyMaster  = 0;
        Autotile[a].IgnorecolorsNum = -1;
    }
    for (int a = 0; a < Object_Max; a++)
    {
        Objects[a].ColorKey = 0;
        strcpy(Objects[a].Type, ".");
        Objects[a].Radius = 0;
    }
    for (int a = 0; a < Water_Max; a++)
    {
        Water[a].ColorKey = 0;
    }

    FreeModels();
    ResetDynamic();

    PlPosSX  = 0;
    PlPosSY  = 0;
    CamPosSX = 0;
    CamPosSY = 0;
    outside  = true;
}

void LoadScript(char filename[])
{
    irqSet(IRQ_VBLANK, NULL);

    ScreenModeLOADING();
    UnloadStuff();
    ResetCounter();  // for percentbar
    UpdateCounter(); // for percentbar

    // erst schaun ob alles da is
    FILE *xml = fopen(filename, "r");
    if (xml == NULL)
        Crash("Script not found:\n%s", filename);
    fclose(xml);

    // hier muss es abgehn...
    TiXmlDocument *xmlDoc = new TiXmlDocument(filename);
    if (!xmlDoc->LoadFile())
        Crash("Script invalid:\n%s", filename);

    // search for <map> </map>
    TiXmlElement *map = xmlDoc->FirstChildElement("map");
    if (!map)
        Crash("Tag called 'map' not found:\n%s", filename);

    // print atributes if there are some
    // mapfile (bmp)
    if (map->Attribute("file"))
    {
        char FileNameCom[60];
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/maps/%s", map->Attribute("file"));
        LoadMap(FileNameCom);
    }
    else
    {
        Crash("No map file specified:\n%s", filename);
    }

    // song
    char SongFile[40];
    if (map->Attribute("music"))
        snprintf(SongFile, sizeof(SongFile), "/wolveslayer/bgfx/%s", map->Attribute("music"));
    else
        Crash("No music file specified:\n%s", filename);

    // environment
    if (map->Attribute("env"))
    {
        if (strncmp("outside", map->Attribute("env"), 7) == 0)
            outside = true;
        else if (strncmp("inside", map->Attribute("env"), 6) == 0)
            outside = false;
        else
            Crash("Invalid environment specified:\n%s", filename);
    }
    else
    {
        Crash("No environment specified:\n%s", filename);
    }

    // Water level
    if (map->Attribute("waterheight"))
    {
        float val = -0.1;
        sscanf(map->Attribute("waterheight"), "%f", &val);
        waterheight = floattof32(val);
    }
    else
    {
        // Default value
        waterheight = floattof32(-0.1);
    }

    // Load the grounds
    LoadGroundCommand(map);
    LoadAutoGroundCommand(map);
    LoadWaterCommand(map);
    LoadTerrainBorderCommand(map);
    // load textures for objects
    LoadTextureCommand(map);
    // load objects
    LoadObjectCommand(map);
    // load mapchanges
    LoadMapChangeCommand(map);
    LoadDoorCommand(map);
    // load dynamiclights
    LoadDynamicLightCommand(map);

    // load datas about npcs and player (definetions,meshes,textures)
    LoadChars(map);

    StartSong(SongFile);

    xmlDoc->Clear();

    PreCalcAll();
    ScreenMode();

    // ShowCollisionMap(12);

    // TODO: You can use this to display a message when a new room is loaded
    // AddTexttoEvent(filename);

    TextBoxmode(EventSpeech, EventSpeechCount, -1);

    irqSet(IRQ_VBLANK, vBlank);
}

bool IsMapOutside(void)
{
    return outside;
}
