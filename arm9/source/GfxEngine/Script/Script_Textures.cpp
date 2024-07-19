#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Render/Obj_Wall.h"
#include "GfxEngine/Texture/Bumpmapping.h"
#include "XML/tinyxml.h"

int TextureWidthHeight[Object_Max];
bool TexturehasBump[Object_Max];

void LoadTextureCommand(TiXmlElement *map)
{
    TiXmlElement *texture = map->FirstChildElement("texture");

    while (texture) {
        // filename
        char FileNameCom[60];
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/obj/%s", texture->Attribute("file"));

        // id
        int id = -1;
        if (texture->Attribute("id"))
            sscanf(texture->Attribute("id"), "%i", &id);

        // bumpmapping
        bool bump = false;
        if (texture->Attribute("bumpmapping")) {
            if (strncmp("true", texture->Attribute("bumpmapping"), 4) == 0)
                bump = true;
        }

        // Final call to load that shit
        if (id < Object_Max && id >= 0) {
            if (bump == false)
                LoadModelTexture(FileNameCom, &ObjektTex[0], id, &TextureWidthHeight[0]);
            else
                LoadMBump3Texture(FileNameCom, id, &ObjektTex[0], &ObjektTexB[0], &ObjektTexC[0]);
        }

        TexturehasBump[id] = bump;

        texture = texture->NextSiblingElement("texture");
    }
}
