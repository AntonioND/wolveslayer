// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_INPUT_INPUT_H__
#define GFXENGINE_INPUT_INPUT_H__

extern int CamPosX, CamPosY;
extern float CamPosSX, CamPosSY;

extern float PlPosX, PlPosY;
extern float PlPosSX, PlPosSY;
extern float PlHeight;
extern int PlDirection;
extern int PlStatus;
extern int PlFrame;
extern int PlHP;

extern int CellSelect;

int GetPX(void);
int GetPY(void);
float GetSY(void);
float GetSX(void);

void Menu(void);
void Interact(void);
void Inputs(void);

#endif // GFXENGINE_INPUT_INPUT_H__
