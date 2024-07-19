#ifndef GFXENGINE_SCRIPT_SCRIPT_TILES_H__
#define GFXENGINE_SCRIPT_SCRIPT_TILES_H__

#include "XML/tinyxml.h"

// Gets the filename out of the command, for loading a ground texture
void LoadGroundCommand(TiXmlElement *map);

void LoadAutoGroundCommand(TiXmlElement *map);

// Gets the filename out of the command, for loading a water texture
void LoadWaterCommand(TiXmlElement *map);

void LoadTerrainBorderCommand(TiXmlElement *map);

#endif // GFXENGINE_SCRIPT_SCRIPT_TILES_H__
