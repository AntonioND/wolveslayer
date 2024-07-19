#ifndef GFXENGINE_SCRIPT_SCRIPT_TEXTURES_H__
#define GFXENGINE_SCRIPT_SCRIPT_TEXTURES_H__

#include "GfxEngine/render/Obj_Wall.h"

#include "XML/tinyxml.h"

extern int TextureWidthHeight[Object_Count];
extern bool TexturehasBump[Object_Count];

void LoadTextureCommand(TiXmlElement *map);

#endif // GFXENGINE_SCRIPT_SCRIPT_TEXTURES_H__
