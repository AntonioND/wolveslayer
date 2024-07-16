#include "Script.h"

int TextureWidthHeight[Object_Count];
bool TexturehasBump[Object_Count];

extern int ObjektTex[Object_Count];  // Standard slot for objects
extern int ObjektTexB[Object_Count]; // Advanced slot for objects(bumpmapping)
extern int ObjektTexC[Object_Count]; // Advanced slot for objects(bumpmapping)

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
        if (id < Object_Count && id >= 0) {
            if (bump == false)
                LoadModelTexture(FileNameCom, &ObjektTex[0], id, &TextureWidthHeight[0]);
            else
                LoadMBump3Texture(FileNameCom, id, &ObjektTex[0], &ObjektTexB[0], &ObjektTexC[0]);
        }

        TexturehasBump[id] = bump;

        texture = texture->NextSiblingElement("texture");
    }
}
