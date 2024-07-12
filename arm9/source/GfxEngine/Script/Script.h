#include "../3D.h"
#include "XML/tinyxml.h"
#include <malloc.h>

//>>>>>>>>>>>>>>>>Script
void ucase (char character[256],char *New);//Returns cptial lettered text of the given text
void LoadScrip(char filename[]);//Loads a new script

void ResetCounter(void);
void IncreaseCounter(void);
int ReturnCounter(void);
void UpdateCounter(void);

int ReturnObjectsCount(void);
