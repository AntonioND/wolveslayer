#ifndef GFXENGINE_SCRIPT_SCRIPT_OBJECTS_H__
#define GFXENGINE_SCRIPT_SCRIPT_OBJECTS_H__

#include <nds.h>

#include "GfxEngine/Limits.h"
#include "XML/tinyxml.h"

typedef struct {
    int TextureID; // Internal engine texture ID used for this object

    char Type[13]; // Type of the object (a string)
    u32 ColorKey;  // 24 BPP RGB color

    float Radius;
    bool Mirrorable;   // Is mirrorable
    bool Illumination; // Self illuminated
    bool NoGround;     // Don't render ground
    bool Culling;      // If true, cull front. If false, cull none.
} ObjectInfo;

extern ObjectInfo Objects[Object_Max];

int CountObjectsCommand(TiXmlElement *map);
int ReturnObjectsCount(void);

void LoadObjectCommand(TiXmlElement *map);

#endif // GFXENGINE_SCRIPT_SCRIPT_OBJECTS_H__
