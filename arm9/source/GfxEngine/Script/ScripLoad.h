#ifndef GFXENGINE_SCRIPT_SCRIPTLOAD_H__
#define GFXENGINE_SCRIPT_SCRIPTLOAD_H__

#include <stdbool.h>

extern bool LoadingNow;

// Returns capital lettered text of the given text
void ucase(char *character, char *New);

// Loads a new script
void LoadScrip(char filename[]);

#endif // GFXENGINE_SCRIPT_SCRIPTLOAD_H__
