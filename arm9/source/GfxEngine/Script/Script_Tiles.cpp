#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Render/Autotiles.h"
#include "GfxEngine/Render/Ground.h"
#include "GfxEngine/Render/Edge.h"
#include "GfxEngine/Render/Wasser.h"
#include "GfxEngine/Texture/Bumpmapping.h"
#include "XML/tinyxml.h"

void LoadGroundCommand(TiXmlElement *map)
{
    int a = 0;
    TiXmlElement *ground = map->FirstChildElement("ground");

    while (ground && a < Ground_Count) {
        char FileNameCom[60];
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/tiles/%s", ground->Attribute("file"));

        // color-id
        if (ground->Attribute("colorkey")) {
            int r = 0, g = 0, b = 0;
            sscanf(ground->Attribute("colorkey"), "%i,%i,%i", &r, &g, &b);
            Ground[a].ColorKey = r | (g << 8) | (b << 16) | (0 << 24);
        }

        // mirrowing
        if (ground->Attribute("mirrowing"))
            if (strncmp("true", ground->Attribute("mirrowing"), 4) == 0)
                Ground[a].TransEnable = true;

        // Here is that final call to load that texture
        Ground[a].Enable = true;

        if (ground->Attribute("bump-mapping")) {
            if (strncmp("true", ground->Attribute("bump-mapping"), 4) != 0) {
                LoadGroundTexture(FileNameCom, a);
                Ground[a].BumpEnable = false;
            } else {
                LoadMBump5Texture(FileNameCom, a);
                Ground[a].BumpEnable = true;
            }
        } else {
            LoadGroundTexture(FileNameCom, a);
            Ground[a].BumpEnable = false;
        }

        ground = ground->NextSiblingElement("ground");
        a++;
    }
}

void LoadAutoGroundCommand(TiXmlElement *map)
{
    int a = 0;

    TiXmlElement *aground = map->FirstChildElement("autoground");
    TiXmlElement *ig;

    while (aground && a < 16) {
        char FileNameCom[60];
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/tiles/%s", aground->Attribute("file"));

        // color-id
        if (aground->Attribute("colorkey")) {
            int r = 0, g = 0, b = 0;
            sscanf(aground->Attribute("colorkey"), "%i,%i,%i", &r, &g, &b);
            Autotile[a].ColorKeyMaster = r | (g << 8) | (b << 16) | (0 << 24);
        }

        // Here is that final call to load that texture
        LoadAutotileTexture(FileNameCom, a);

        // load the ignore commands
        // TODO: This is unused
        ig = aground->FirstChildElement("ignore");
        int aa = 0;
        while (ig && aa < Autotile_Ignorecolors_Max) {
            // color-id
            int r = 0, g = 0, b = 0;
            if (ig->Attribute("colorkey")) {
                sscanf(ig->Attribute("colorkey"), "%i,%i,%i", &r, &g, &b);
                AddAutIgnore(a, u32(r | (g << 8) | (b << 16) | (0 << 24)));
            }

            ig = ig->NextSiblingElement("ignore");
            aa++;
        }

        aground = aground->NextSiblingElement("autoground");
        a++;
    }
}

void LoadWaterCommand(TiXmlElement *map)
{
    TiXmlElement *wasser = map->FirstChildElement("water");

    if (wasser) {
        char FileNameCom[60];
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/tiles/%s", wasser->Attribute("file"));

        if (wasser->Attribute("colorkey")) {
            int r = 0, g = 0, b = 0;
            sscanf(wasser->Attribute("colorkey"), "%i,%i,%i", &r, &g, &b);
            WasserKey = r | (g << 8) | (b << 16) | (0 << 24);
        }

        int size;
        LoadModelTexture(FileNameCom, &Wasser[0], 0, &size);
    }
}

void LoadTerrainBorderCommand(TiXmlElement *map)
{
    TiXmlElement *terrainborder = map->FirstChildElement("terrainborder");

    if (terrainborder) {
        bool bumpmapping = false;
        if (terrainborder->Attribute("bumpmapping")) {
            if (strncmp("true", terrainborder->Attribute("bumpmapping"), 4) == 0)
                bumpmapping = true;
        }

        if (terrainborder->Attribute("file")) {
            char FileNameCom[60];
            snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/tiles/%s", terrainborder->Attribute("file"));

            if (bumpmapping == false)
                LoadEdgeTexture(FileNameCom, &Edge[0], 0);
            else
                LoadMBump3Texture(FileNameCom, 0, &Edge[0], &EdgeB[0], &EdgeC[0]);

            EdgeBump[0] = bumpmapping;
        }
    }
}
