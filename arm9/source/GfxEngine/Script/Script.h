#ifndef GFXENGINE_SCRIPT_SCRIPT_H__
#define GFXENGINE_SCRIPT_SCRIPT_H__

#include "GfxEngine/3D.h"
#include "XML/tinyxml.h"

//>>>>>>>>>>>>>>>>Script

// Returns captial lettered text of the given text
void ucase(char *character, char *New);

// Loads a new script
void LoadScrip(char filename[]);

void ResetCounter(void);
void IncreaseCounter(void);
int ReturnCounter(void);
void UpdateCounter(void);

int ReturnObjectsCount(void);

#endif // GFXENGINE_SCRIPT_SCRIPT_H__
