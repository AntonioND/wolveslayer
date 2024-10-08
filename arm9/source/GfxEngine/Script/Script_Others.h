// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_SCRIPT_SCRIPT_OTHERS_H__
#define GFXENGINE_SCRIPT_SCRIPT_OTHERS_H__

#include "XML/tinyxml.h"

void ResetCounter(void);
void IncreaseCounter(void);
int ReturnCounter(void);
void UpdateCounter(void);

void LoadDynamicLightCommand(TiXmlElement *map);
void LoadMapChangeCommand(TiXmlElement *map);
void LoadDoorCommand(TiXmlElement *map);

#endif // GFXENGINE_SCRIPT_SCRIPT_OTHERS_H__
