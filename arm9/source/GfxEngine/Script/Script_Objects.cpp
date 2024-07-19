#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Script/Script_Objects.h"
#include "GfxEngine/Script/Script_Others.h"
#include "GfxEngine/Script/Script_Textures.h"

ObjectInfo Objects[Object_Count];

int OBJCOUNT;

int CountObjectsCommand(TiXmlElement *map)
{
    int num = 0;

    TiXmlElement *object = map->FirstChildElement("object");

    while (object) {
        object = object->NextSiblingElement("object");
        num++;
    }
    return num;
}

int ReturnObjectsCount(void)
{
    return OBJCOUNT;
}

void LoadObjectCommand(TiXmlElement *map)
{
    OBJCOUNT = CountObjectsCommand(map);

    char FileNameCom[60];
    // char usca[60];
    const char *ObjTyp = NULL;
    int id;
    int num = 0;
    uint r, g, b;
    int scale;
    bool dontrenderground;
    bool mirrowable;
    bool ilu;
    bool cull;
    float rad;

    TiXmlElement *light;
    int lightnum;
    TiXmlElement *object = map->FirstChildElement("object");

    while (object) {
        id = -1;

        // id
        if (object->Attribute("textureid"))
            sscanf(object->Attribute("textureid"), "%i", &id);
        Objects[num].TextureID = id;

        // filename
        strcpy(FileNameCom, "");
        if (object->Attribute("mesh")) {
            if (strncmp("[wall]", object->Attribute("mesh"), 6) == 0) {
                if (!TexturehasBump[id])
                    ObjTyp = "WALL";
                else
                    ObjTyp = "BUMPWALL";
            } else if (strncmp("[house]", object->Attribute("mesh"), 7) == 0) {
                ObjTyp = "HOUSE";
            } else {
                ObjTyp = "MODEL";
                snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/obj/%s", object->Attribute("mesh"));
            }
        }

        // color-id
        r = 0;
        g = 0;
        b = 0;
        if (object->Attribute("colorkey")) {
            sscanf(object->Attribute("colorkey"), "%i,%i,%i", &r, &g, &b);
            Objects[num].ColorKey = (r) | (g << 8) | (b << 16) | (0 << 24);
        }

        // scale
        scale = 40;
        if (object->Attribute("scale"))
            sscanf(object->Attribute("scale"), "%i", &scale);

        // radius
        rad = -1;
        if (object->Attribute("radius")) {
            sscanf(object->Attribute("radius"), "%f", &rad);
            if (rad < 0)
                rad = 0;
            if (rad > .5)
                rad = .5;
        }
        Objects[num].Radius = rad;

        // render ground?
        dontrenderground = false;
        if (object->Attribute("renderground")) {
            if (strncmp("false", object->Attribute("renderground"), 5) == 0)
                dontrenderground = true;
        }
        Objects[num].NoGround = dontrenderground;

        // mirrowable?
        mirrowable = true;
        if (object->Attribute("mirrowable")) {
            if (strncmp("false", object->Attribute("mirrowable"), 5) == 0)
                mirrowable = false;
        }
        Objects[num].Mirrowable = mirrowable;

        // selfilluminated?
        ilu = false;
        if (object->Attribute("selfilluminated")) {
            if (strncmp("true", object->Attribute("selfilluminated"), 4) == 0)
                ilu = true;
        }
        Objects[num].Illumination = ilu;

        // cullfront?
        cull = false;
        if (object->Attribute("culling")) {
            if (strncmp("true", object->Attribute("culling"), 4) == 0)
                cull = true;
        }
        Objects[num].Culling = cull;

        // Final call to load it
        if (id < Object_Count && id >= 0)
            strcpy(Objects[num].Type, ObjTyp);

        if (strncmp("MODEL", Objects[num].Type, 5) == 0 && id >= 0)
            LoadMD2Model(FileNameCom, num + 10, TextureWidthHeight[id], scale);

        // Light (objects children)
        light    = object->FirstChildElement("light");
        lightnum = 0;
        while (light && lightnum < 10) {
            light = object->NextSiblingElement("light");
            lightnum++;
        }

        // next object
        object = object->NextSiblingElement("object");
        num++;
        IncreaseCounter();
        UpdateCounter();
    }
}
