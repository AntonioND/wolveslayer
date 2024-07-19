#ifndef GFXENGINE_3D_H__
#define GFXENGINE_3D_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nds.h>
#include <nds/arm9/image.h>
#include <nds/arm9/trig_lut.h>

typedef int32_t f32;

// DebugInfo Options
#define ShowHeapUsage
#define ShowPosition
#define ShowPolyCount

// Other settings
#define Object_Count 26

// Some replacers for bitshifting inside precalcdata
#define water       0
#define mirrow      1
#define BumpgroundS 2
#define BumpgroundW 3
#define BumpgroundE 4
#define BumpwallS   5
#define BumpwallW   6
#define BumpwallE   7

__attribute__((noreturn)) void Crash(const char *msg, ...);

void WaitForFreeVblank(void); // important for multitask buffering and loading to vram

void vBlank(void);
void E3D_Init(void);
void E3D_StartRender();

///>>>>>>>>>>>>>>>3D

#define Obj_MD2 -10;

// Wallparts
#define Obj_WallAlone 10;
#define Obj_WallBor   11;
#define Obj_WallDoor  12;

// #define Obj_WallCor 12;//not visible thing in fact

// Houseparts
#define Obj_HouseMid  20;
#define Obj_HouseBor  21;
#define Obj_HouseCor  22;
#define Obj_HouseDoor 23;

#endif // GFXENGINE_3D_H__
