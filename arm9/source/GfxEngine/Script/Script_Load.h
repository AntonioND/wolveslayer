#ifndef GFXENGINE_SCRIPT_SCRIPT_LOAD_H__
#define GFXENGINE_SCRIPT_SCRIPT_LOAD_H__

#include <stdbool.h>

extern bool LoadingNow;

// Returns capital lettered text of the given text
void ucase(const char *old, char *new_);

// Loads a new script
void LoadScript(char filename[]);

// This returns true if the map is outdoors, false if it is indoors.
bool IsMapOutside(void);

#endif // GFXENGINE_SCRIPT_SCRIPT_LOAD_H__
