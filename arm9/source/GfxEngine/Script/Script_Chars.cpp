#include "GfxEngine/3D.h"
#include "GfxEngine/Enemies.h"
#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Settings.h"
#include "XML/tinyxml.h"

int Figuren[10];

char Names[8][20];
int Atps[8];
int Hps[8];
float rads[8];
bool boss[8];

void LoadChars(TiXmlElement *map)
{
    int textsize;
    // loadmainchar and weapon
    LoadModelTexture(MaincharText, &Figuren[0], &textsize);
    LoadMD2Model(MaincharMd2, 0, textsize, 35);

    LoadModelTexture(waffeText, &Figuren[1], &textsize);
    LoadMD2Model(waffeMd2, 1, textsize, 35);

    // load enemy defenition stuff
    // <enemydef name="Wolve1">
    //     <mesh        file="enemys/wolf.md2" scale="40" radius=".8"/>
    //     <texture     file="enemys/wolf.bmp" />
    //     <atributes   healthpower="100" attackpower="2" endgame="true"/>
    // </enemydef>

    char FileNameCom[60];
    char TextureNameCom[60];
    int Num = -1;
    int scale;
    TiXmlElement *endef = map->FirstChildElement("enemydef");
    // TiXmlElement *atributes;

    for (int a = 0; a < 8; a++)
        strcpy(Names[a], "-.-öäüß");

    while (endef && Num < 7) {
        TiXmlElement *texture   = endef->FirstChildElement("texture");
        TiXmlElement *mesh      = endef->FirstChildElement("mesh");
        TiXmlElement *atributes = endef->FirstChildElement("atributes");

        if (mesh && texture && atributes) {
            Num++;

            // copy enemydef-name
            if (endef->Attribute("name"))
                snprintf(Names[Num], sizeof(Names[Num]), "%s", endef->Attribute("name"));

            // add attackpower
            if (atributes->Attribute("attackpower"))
                sscanf(atributes->Attribute("attackpower"), "%i", &Atps[Num]);

            // add hp
            if (atributes->Attribute("healthpower"))
                sscanf(atributes->Attribute("healthpower"), "%i", &Hps[Num]);

            // get the texturefilename
            snprintf(TextureNameCom, sizeof(TextureNameCom), "/wolveslayer/chars/%s", texture->Attribute("file"));

            // get meshdata
            snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/chars/%s", mesh->Attribute("file"));

            // getscale factor
            scale = 35;
            if (mesh->Attribute("scale"))
                sscanf(mesh->Attribute("scale"), "%i", &scale);

            // get radius
            rads[Num] = .8;
            if (mesh->Attribute("radius"))
                sscanf(mesh->Attribute("radius"), "%f", &rads[Num]);

            // get boss status
            boss[Num] = false;
            if (atributes->Attribute("endgame"))
                if (strncmp(atributes->Attribute("endgame"), "true", 4) == 0)
                    boss[Num] = true;

            // loadstuff
            if (texture->Attribute("file") && mesh->Attribute("file")) {
                if (2 + Num >= 10)
                    Crash("Too many figures");

                LoadModelTexture(TextureNameCom, &Figuren[2 + Num], &textsize);
                LoadMD2Model(FileNameCom, 2 + Num, textsize, scale);
            }
        }

        // next object
        endef = endef->NextSiblingElement("enemydef");
    }

    // now place the enemys
    // <enemy name="Wolve1" pos="3,5"/>
    TiXmlElement *ene = map->FirstChildElement("enemy");

    while (ene) {
        if (ene->Attribute("name")) {
            // get the enemy name and search in the defenitions for right one
            char eName[20];
            snprintf(eName, sizeof(eName), "%s", ene->Attribute("name"));

            int enenum = -1;
            for (int a = 0; a < 8; a++)
                if (strncmp(Names[a], eName, strlen(eName)) == 0)
                    enenum = a;

            if (ene->Attribute("pos") && enenum >= 0) {
                int x, y;
                sscanf(ene->Attribute("pos"), "%i,%i", &x, &y);
                AddEnemy(x, y, enenum + 2, Atps[enenum], Hps[enenum], rads[enenum], boss[enenum]);
            }
        }

        ene = ene->NextSiblingElement("enemy");
    }
}
