// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_MODEL_MD2LOAD_H__
#define GFXENGINE_MODEL_MD2LOAD_H__

void InitTableOfNormal(void);
void FreeModels(void);
void LoadModelTexture(char filename[], int *Target, int *w);
void LoadMD2Model(char Filename[], int num, int widthheight, int scale);

#endif // GFXENGINE_MODEL_MD2LOAD_H__
