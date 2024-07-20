#ifndef GFXENGINE_SCRIPT_SCRIPT_LOAD_H__
#define GFXENGINE_SCRIPT_SCRIPT_LOAD_H__

#include <stdbool.h>

// Loads a new script
void LoadScript(char filename[]);

// This returns true if the map is outdoors, false if it is indoors.
// TODO: This isn't used in any of the original maps.
bool IsMapOutside(void);

#endif // GFXENGINE_SCRIPT_SCRIPT_LOAD_H__
