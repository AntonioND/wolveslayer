#ifndef GFXENGINE_SCRIPT_SCRIPT_TEXTURES_H__
#define GFXENGINE_SCRIPT_SCRIPT_TEXTURES_H__

#include "GfxEngine/Limits.h"
#include "GfxEngine/Render/Obj_Wall.h"
#include "XML/tinyxml.h"

extern int TextureWidthHeight[Object_Max];
extern bool TexturehasBump[Object_Max];

void LoadTextureCommand(TiXmlElement *map);

#endif // GFXENGINE_SCRIPT_SCRIPT_TEXTURES_H__
