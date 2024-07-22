// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_3D_H__
#define GFXENGINE_3D_H__

#include <stdarg.h>

#include <nds.h>

typedef int32_t f32;

// DebugInfo Options
#define ShowHeapUsage
#define ShowPosition
#define ShowPolyCount

// Some replacers for bitshifting inside precalcdata
#define B_Water       0
#define B_Mirrorable  1
#define B_BumpgroundS 2
#define B_BumpgroundW 3
#define B_BumpgroundE 4
#define B_BumpwallS   5
#define B_BumpwallW   6
#define B_BumpwallE   7

__attribute__((noreturn)) void Crash(const char *msg, ...);

void WaitForFreeVblank(void); // important for multitask buffering and loading to vram
void vBlank(void);

void E3D_Init(void);
void E3D_StartRender();
void E3D_Render();
void E3D_EndRender();

///>>>>>>>>>>>>>>>3D

#define Obj_MD2 -10;

// Wallparts
#define Obj_WallAlone 10;
#define Obj_WallBor   11;
#define Obj_WallDoor  12;

// #define Obj_WallCor 12; // not visible thing in fact

// Houseparts
#define Obj_HouseMid  20;
#define Obj_HouseBor  21;
#define Obj_HouseCor  22;
#define Obj_HouseDoor 23;

#endif // GFXENGINE_3D_H__
