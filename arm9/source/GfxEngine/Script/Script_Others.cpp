#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/Render/Doors.h"
#include "GfxEngine/Script/Script_Objects.h"
#include "GfxEngine/Script/Script_Others.h"
#include "GfxEngine/Texture/DynamicLights.h"

static int Counter;

void ResetCounter(void)
{
    Counter = 0;
}
void IncreaseCounter(void)
{
    Counter++;
}

int ReturnCounter(void)
{
    return Counter;
}

void UpdateCounter(void)
{
    int countofstuff = ReturnObjectsCount();

    PercentBar(Counter, countofstuff);
}

void LoadDynamicLightCommand(TiXmlElement *map)
{
    TiXmlElement *dynamiclight = map->FirstChildElement("dynamiclight");

    while (dynamiclight) {
        // Fireflys
        int x, y, r, g, b;
        TiXmlElement *firefly = dynamiclight->FirstChildElement("firefly");
        while (firefly) {
            // POSITION
            x = -1;
            y = -1;
            if (firefly->Attribute("pos"))
                sscanf(firefly->Attribute("pos"), "%i,%i", &x, &y);

            // COLOR
            r = -1;
            g = -1;
            b = -1;
            if (firefly->Attribute("color"))
                sscanf(firefly->Attribute("color"), "%i,%i,%i", &r, &g, &b);

            if (x + y + r + g + b > 0)
                AddDynamic(x, y, r, g, b);

            firefly = firefly->NextSiblingElement("firefly");
        }
        dynamiclight = dynamiclight->NextSiblingElement("dynamiclight");
    }
}

void LoadMapChangeCommand(TiXmlElement *map)
{
    int x, y;
    int x2, y2;
    char FileNameCom[60];

    TiXmlElement *mc = map->FirstChildElement("mapchange");
    while (mc) {
        // POSITION
        x = -1;
        y = -1;
        if (mc->Attribute("pos"))
            sscanf(mc->Attribute("pos"), "%i,%i", &x, &y);

        // TARGET POSITION
        x2 = -1;
        y2 = -1;
        if (mc->Attribute("targetpos"))
            sscanf(mc->Attribute("targetpos"), "%i,%i", &x2, &y2);

        // filename
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/maps/%s", mc->Attribute("targetmap"));

        if (x + y + x2 + y2 > -1)
            AddMapChange(x, y, FileNameCom, x2, y2);

        mc = mc->NextSiblingElement("mapchange");
    }
}

void LoadDoorCommand(TiXmlElement *map)
{
    int x, y;
    int x2, y2;
    char FileNameCom[60];

    TiXmlElement *doortext = map->FirstChildElement("doortexture");
    if (doortext) {
        // filename
        if (doortext->Attribute("file")) {
            snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/obj/%s", doortext->Attribute("file"));
            LoadDoorTexture(FileNameCom);
        }
    }

    TiXmlElement *door = map->FirstChildElement("door");
    while (door) {
        // POSITION
        x = -1;
        y = -1;
        if (door->Attribute("pos"))
            sscanf(door->Attribute("pos"), "%i,%i", &x, &y);

        // TARGET POSITION
        x2 = -1;
        y2 = -1;
        if (door->Attribute("targetpos"))
            sscanf(door->Attribute("targetpos"), "%i,%i", &x2, &y2);

        // filename
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/maps/%s", door->Attribute("targetmap"));

        if (x + y + x2 + y2 > -1)
            AddMapDoor(x, y, FileNameCom, x2, y2, 0);

        door = door->NextSiblingElement("door");
    }
}
