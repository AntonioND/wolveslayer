// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Render/Obj_Wall.h"
#include "GfxEngine/Texture/Bumpmapping.h"
#include "XML/tinyxml.h"

void LoadTextureCommand(TiXmlElement *map)
{
    TiXmlElement *texture = map->FirstChildElement("texture");

    while (texture)
    {
        // filename
        char FileNameCom[60];
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/obj/%s", texture->Attribute("file"));

        // id
        int id = -1;
        if (texture->Attribute("id"))
            sscanf(texture->Attribute("id"), "%i", &id);
        if ((id < 0) || (id >= ObjectTexture_Max))
            Crash("Invalid texture ID:\n%d (max %d)", id, ObjectTexture_Max);

        // bumpmapping
        bool bump = false;
        if (texture->Attribute("bumpmapping"))
        {
            if (strncmp("true", texture->Attribute("bumpmapping"), 4) == 0)
                bump = true;
        }

        // Load the texture
        if (bump == false)
            LoadModelTexture(FileNameCom, &(ObjectTexture[id].Texture), &(ObjectTexture[id].WidthHeight));
        else
            LoadMBump3Texture(FileNameCom, &(ObjectTexture[id].Texture), &(ObjectTexture[id].TextureB), &(ObjectTexture[id].TextureC));

        ObjectTexture[id].HasBump = bump;

        texture = texture->NextSiblingElement("texture");
    }
}
