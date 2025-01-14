// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_SCRIPT_SCRIPT_CHARS_H__
#define GFXENGINE_SCRIPT_SCRIPT_CHARS_H__

#include "GfxEngine/Limits.h"
#include "XML/tinyxml.h"

extern int FigureTextures[FigureTex_Max];

void LoadChars(TiXmlElement *map);

#endif // GFXENGINE_SCRIPT_SCRIPT_CHARS_H__
