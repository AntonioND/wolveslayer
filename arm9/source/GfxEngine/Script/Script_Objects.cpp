#include "GfxEngine/3D.h"
#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Render/Obj_Wall.h"
#include "GfxEngine/Script/Script_Objects.h"
#include "GfxEngine/Script/Script_Others.h"
#include "GfxEngine/Script/Script_Textures.h"

ObjectInfo Objects[Object_Max];

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
    int num = 0;

    TiXmlElement *object = map->FirstChildElement("object");

    while (object) {
        // id
        int id = -1;
        if (object->Attribute("textureid"))
            sscanf(object->Attribute("textureid"), "%i", &id);
        if ((id < 0) || (id >= Object_Max))
            Crash("Invalid object ID:\n%d (max %d)", id, Object_Max);
        Objects[num].TextureID = id;

        // filename
        const char *ObjTyp = "";
        strcpy(FileNameCom, "");
        if (object->Attribute("mesh")) {
            if (strncmp("[wall]", object->Attribute("mesh"), 6) == 0) {
                if (!ObjectTexture[id].HasBump)
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
        if (object->Attribute("colorkey")) {
            int r = 0, g = 0, b = 0;
            sscanf(object->Attribute("colorkey"), "%i,%i,%i", &r, &g, &b);
            Objects[num].ColorKey = r | (g << 8) | (b << 16) | (0 << 24);
        }

        // scale
        int scale = 40;
        if (object->Attribute("scale"))
            sscanf(object->Attribute("scale"), "%i", &scale);

        // radius
        float rad = -1;
        if (object->Attribute("radius")) {
            sscanf(object->Attribute("radius"), "%f", &rad);
            if (rad < 0)
                rad = 0;
            if (rad > .5)
                rad = .5;
        }
        Objects[num].Radius = rad;

        // render ground?
        bool dontrenderground = false;
        if (object->Attribute("renderground")) {
            if (strncmp("false", object->Attribute("renderground"), 5) == 0)
                dontrenderground = true;
        }
        Objects[num].NoGround = dontrenderground;

        // mirrowable?
        bool mirrowable = true;
        if (object->Attribute("mirrowable")) {
            if (strncmp("false", object->Attribute("mirrowable"), 5) == 0)
                mirrowable = false;
        }
        Objects[num].Mirrowable = mirrowable;

        // selfilluminated?
        bool ilu = false;
        if (object->Attribute("selfilluminated")) {
            if (strncmp("true", object->Attribute("selfilluminated"), 4) == 0)
                ilu = true;
        }
        Objects[num].Illumination = ilu;

        // cullfront?
        bool cull = false;
        if (object->Attribute("culling")) {
            if (strncmp("true", object->Attribute("culling"), 4) == 0)
                cull = true;
        }
        Objects[num].Culling = cull;

        // Final call to load it
        strcpy(Objects[num].Type, ObjTyp);

        if (strncmp("MODEL", Objects[num].Type, 5) == 0 && id >= 0)
            LoadMD2Model(FileNameCom, num + 10, ObjectTexture[id].WidthHeight, scale);

        // Light (objects children)
        int lightnum = 0;

        TiXmlElement *light = object->FirstChildElement("light");
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
