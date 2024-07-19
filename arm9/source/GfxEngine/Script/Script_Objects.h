#ifndef GFXENGINE_SCRIPT_SCRIPT_OBJECTS_H__
#define GFXENGINE_SCRIPT_SCRIPT_OBJECTS_H__

#include "XML/tinyxml.h"

int CountObjectsCommand(TiXmlElement *map);
int ReturnObjectsCount(void);

void LoadObjectCommand(TiXmlElement *map);

#endif // GFXENGINE_SCRIPT_SCRIPT_OBJECTS_H__
