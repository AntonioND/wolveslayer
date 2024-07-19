#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Render/Autotiles.h"
#include "GfxEngine/Render/Boden.h"
#include "GfxEngine/Render/Ufer.h"
#include "GfxEngine/Render/Wasser.h"
#include "GfxEngine/Texture/Bumpmapping.h"
#include "XML/tinyxml.h"

void LoadBodenCommand(TiXmlElement *map)
{
    int a = 0;
    TiXmlElement *boden = map->FirstChildElement("ground");

    while (boden && a < Ground_Count) {
        char FileNameCom[60];
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/tiles/%s", boden->Attribute("file"));

        // color-id
        if (boden->Attribute("colorkey")) {
            int r = 0, g = 0, b = 0;
            sscanf(boden->Attribute("colorkey"), "%i,%i,%i", &r, &g, &b);
            Ground[a].ColorKey = r | (g << 8) | (b << 16) | (0 << 24);
        }

        // mirrowing
        if (boden->Attribute("mirrowing"))
            if (strncmp("true", boden->Attribute("mirrowing"), 4) == 0)
                Ground[a].TransEnable = true;

        // Here is that final call to load that texture
        Ground[a].Enable = true;

        if (boden->Attribute("bump-mapping")) {
            if (strncmp("true", boden->Attribute("bump-mapping"), 4) != 0) {
                LoadBodenTexture(FileNameCom, a);
                Ground[a].BumpEnable = false;
            } else {
                LoadMBump5Texture(FileNameCom, a);
                Ground[a].BumpEnable = true;
            }
        } else {
            LoadBodenTexture(FileNameCom, a);
            Ground[a].BumpEnable = false;
        }

        boden = boden->NextSiblingElement("ground");
        a++;
    }
}

void LoadAutoBodenCommand(TiXmlElement *map)
{
    int a = 0;

    TiXmlElement *aboden = map->FirstChildElement("autoground");
    TiXmlElement *ig;

    while (aboden && a < 16) {
        char FileNameCom[60];
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/tiles/%s", aboden->Attribute("file"));

        // color-id
        if (aboden->Attribute("colorkey")) {
            int r = 0, g = 0, b = 0;
            sscanf(aboden->Attribute("colorkey"), "%i,%i,%i", &r, &g, &b);
            AutotileColorKeyMaster[a] = r | (g << 8) | (b << 16) | (0 << 24);
        }

        // Here is that final call to load that texture
        LoadAutotileTexture(FileNameCom, a);

        // load the ignore commands
        ig = aboden->FirstChildElement("ignore");
        int aa = 0;
        while (ig && aa < 10) {
            // color-id
            int r = 0, g = 0, b = 0;
            if (ig->Attribute("colorkey")) {
                sscanf(ig->Attribute("colorkey"), "%i,%i,%i", &r, &g, &b);
                AddAutIgnore(a, u32(r | (g << 8) | (b << 16) | (0 << 24)));
            }

            ig = ig->NextSiblingElement("ignore");
            a++; // TODO: Should this be aa?
        }

        aboden = aboden->NextSiblingElement("autoground");
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
                LoadUferTexture(FileNameCom, &Ufer[0], 0);
            else
                LoadMBump3Texture(FileNameCom, 0, &Ufer[0], &UferB[0], &UferC[0]);

            UferBump[0] = bumpmapping;
        }
    }
}
