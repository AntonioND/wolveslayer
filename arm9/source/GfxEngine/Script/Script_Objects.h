#ifndef GFXENGINE_SCRIPT_SCRIPT_OBJECTS_H__
#define GFXENGINE_SCRIPT_SCRIPT_OBJECTS_H__

#include <nds.h>

#include "GfxEngine/Limits.h"

#include "XML/tinyxml.h"

extern int ObjectTextureID[Object_Count];

// and 26 objects...but u can map textures twice....
extern char ObjektTyp[Object_Count][13];
extern u32 ObjektColorKey[Object_Count];
extern float ObjectRadius[Object_Count];
extern bool ObjectMirrow[Object_Count];
extern bool ObjectIllumination[Object_Count];
extern bool ObjectNoGround[Object_Count];
extern bool ObjectCulling[Object_Count];

int CountObjectsCommand(TiXmlElement *map);
int ReturnObjectsCount(void);

void LoadObjectCommand(TiXmlElement *map);

#endif // GFXENGINE_SCRIPT_SCRIPT_OBJECTS_H__
