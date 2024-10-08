// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_MODEL_MD2MODELS_H__
#define GFXENGINE_MODEL_MD2MODELS_H__

#include "GfxEngine/Limits.h"
#include "GfxEngine/Model/MD2Format.h"

extern MD2Entity Models[MD2_Max];

void Precalcmd2light(int n, int num);
void RenderMD2Model(int n, int num);
void RenderMD2ModelMirrored(int n, int num);

#endif // GFXENGINE_MODEL_MD2MODELS_H__
