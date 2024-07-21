#include "GfxEngine/3D.h"
#include "GfxEngine/Enemies.h"
#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Settings.h"
#include "XML/tinyxml.h"

int FigureTextures[FigureTex_Max];

static char Names[EnemyFigureTex_Max][20];
static int Atps[EnemyFigureTex_Max];
static int Hps[EnemyFigureTex_Max];
static float rads[EnemyFigureTex_Max];
static bool boss[EnemyFigureTex_Max];

void LoadChars(TiXmlElement *map)
{
    int textsize;

    // Load main character and weapon
    LoadModelTexture(MaincharText, &FigureTextures[CharacterFigureTex_ID], &textsize);
    LoadMD2Model(MaincharMd2, CharacterFigureTex_ID, textsize, 35);

    LoadModelTexture(weaponText, &FigureTextures[WeaponFigureTex_ID], &textsize);
    LoadMD2Model(weaponMd2, WeaponFigureTex_ID, textsize, 35);

    // Load enemy defenition:
    //
    // <enemydef name="Wolve1">
    //     <mesh        file="enemies/wolf.md2" scale="40" radius=".8"/>
    //     <texture     file="enemies/wolf.bmp"/>
    //     <atributes   healthpower="100" attackpower="2" endgame="true"/>
    // </enemydef>

    char FileNameCom[60];
    char TextureNameCom[60];
    int Num = -1;
    int scale;
    TiXmlElement *endef = map->FirstChildElement("enemydef");

    for (int a = 0; a < EnemyFigureTex_Max; a++)
        strcpy(Names[a], "-.-öäüß");

    while (endef && (Num < (EnemyFigureTex_Max - 1)))
    {
        TiXmlElement *texture   = endef->FirstChildElement("texture");
        TiXmlElement *mesh      = endef->FirstChildElement("mesh");
        TiXmlElement *atributes = endef->FirstChildElement("atributes");

        if (mesh && texture && atributes)
        {
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
            if (texture->Attribute("file") && mesh->Attribute("file"))
            {
                int id = EnemyFigureBaseTex_ID + Num;
                if (id >= FigureTex_Max)
                    Crash("Too many figures");

                LoadModelTexture(TextureNameCom, &FigureTextures[id], &textsize);
                LoadMD2Model(FileNameCom, id, textsize, scale);
            }
        }

        // next object
        endef = endef->NextSiblingElement("enemydef");
    }

    // Now place the enemies

    // <enemy name="Wolve1" pos="3,5"/>
    TiXmlElement *ene = map->FirstChildElement("enemy");

    while (ene)
    {
        if (ene->Attribute("name"))
        {
            // get the enemy name and search in the defenitions for right one
            char eName[20];
            snprintf(eName, sizeof(eName), "%s", ene->Attribute("name"));

            int enenum = -1;
            for (int a = 0; a < EnemyFigureTex_Max; a++)
                if (strncmp(Names[a], eName, strlen(eName)) == 0)
                    enenum = a;

            if (ene->Attribute("pos") && enenum >= 0)
            {
                int x, y;
                sscanf(ene->Attribute("pos"), "%i,%i", &x, &y);

                int id = EnemyFigureBaseTex_ID + enenum;
                AddEnemy(x, y, id, Atps[enenum], Hps[enenum], rads[enenum], boss[enenum]);
            }
        }

        ene = ene->NextSiblingElement("enemy");
    }
}
