#ifndef GFXENGINE_SCRIPT_SCRIPT_TILES_H__
#define GFXENGINE_SCRIPT_SCRIPT_TILES_H__

#include "XML/tinyxml.h"

// Gets the filename out of the command, for loading a bodentexture
void LoadBodenCommand(TiXmlElement *map);

void LoadAutoBodenCommand(TiXmlElement *map);

// Gets the filename out of the command, for loading a wassertexture
void LoadWaterCommand(TiXmlElement *map);

void LoadTerrainBorderCommand(TiXmlElement *map);

#endif // GFXENGINE_SCRIPT_SCRIPT_TILES_H__
