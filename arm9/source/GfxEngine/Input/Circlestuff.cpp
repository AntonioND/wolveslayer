// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include <math.h>

#include "GfxEngine/3D.h"
#include "GfxEngine/PreCalc.h"

typedef struct {
    short X;
    short Y;
    u8 Direction;
    u8 DirectionDiagonal;
} circlepoint;

typedef struct {
    circlepoint *Pix;
    int count;
} circle;

#define CircleLutSize 6

static circle CircleLUT[CircleLutSize];

// This precalulated circles maight be usefull for hitdetection :D
static void makecircle(int rad)
{
    int ox    = 0;
    int oy    = 0;
    int deg   = 0;
    int count = 0;

    // ok first we count the pixels which will be used...
    f32 radius = inttof32(rad + 1);
    do
    {
        int X = round(f32tofloat(mulf32(radius, cosLerp(deg * 64))));
        int Y = round(f32tofloat(mulf32(radius, sinLerp(deg * 64))));

        if (ox != X || oy != Y)
            count++;

        ox = X;
        oy = Y;
        deg += 1;
    } while (deg <= 511);

    // fine...now we can malloc the space for it and fill it with datas :D
    CircleLUT[rad].count = count;
    CircleLUT[rad].Pix   = (circlepoint *)malloc(sizeof(circlepoint) * count);
    if (CircleLUT[rad].Pix == NULL)
        Crash("Not enough RAM:\n%s", __func__);

    count = 0;
    deg   = 0;
    ox    = 0;
    oy    = 0;

    do
    {
        int X = round(f32tofloat(mulf32(radius, cosLerp(deg * 64))));
        int Y = round(f32tofloat(mulf32(radius, sinLerp(deg * 64))));

        if (ox != X || oy != Y)
        {
            CircleLUT[rad].Pix[count].X = X;
            CircleLUT[rad].Pix[count].Y = Y;
            // if you just press up,down,left or right
            if (deg > 0)
                CircleLUT[rad].Pix[count].Direction = 3; // down-right
            if (deg > 128)
                CircleLUT[rad].Pix[count].Direction = 5; // down-left
            if (deg > 256)
                CircleLUT[rad].Pix[count].Direction = 7; // up-left
            if (deg > 384)
                CircleLUT[rad].Pix[count].Direction = 1; // up-right

            // if you press 2 directions
            if (deg > 448 || deg < 64)
                CircleLUT[rad].Pix[count].DirectionDiagonal = 2; // right
            if (deg > 64)
                CircleLUT[rad].Pix[count].DirectionDiagonal = 4; // down
            if (deg > 192)
                CircleLUT[rad].Pix[count].DirectionDiagonal = 6; // left
            if (deg > 320 && deg < 448)
                CircleLUT[rad].Pix[count].DirectionDiagonal = 0; // up
            count++;
        }
        // if you just press up,down,left or right
        if (deg == 0)
            CircleLUT[rad].Pix[count - 1].Direction = 2; // right
        if (deg == 128)
            CircleLUT[rad].Pix[count - 1].Direction = 4; // down
        if (deg == 256)
            CircleLUT[rad].Pix[count - 1].Direction = 6; // left
        if (deg == 384)
            CircleLUT[rad].Pix[count - 1].Direction = 0; // up
        // if you press 2 directions
        if (deg == 64)
            CircleLUT[rad].Pix[count - 1].DirectionDiagonal = 3; // down-right
        if (deg == 192)
            CircleLUT[rad].Pix[count - 1].DirectionDiagonal = 5; // down-left
        if (deg == 320)
            CircleLUT[rad].Pix[count - 1].DirectionDiagonal = 7; // up-left
        if (deg == 448)
            CircleLUT[rad].Pix[count - 1].DirectionDiagonal = 1; // up-right

        ox = X;
        oy = Y;
        deg += 1;
    } while (deg <= 511);
}

void InitCircles(void)
{
    for (int q = 1; q < CircleLutSize; q++)
        makecircle(q);
}

void DrawCircle(int num)
{
    if (num < 1 || num >= CircleLutSize)
        return;

    for (int q = 0; q < CircleLUT[num].count; q++)
    {
        int x = CircleLUT[num].Pix[q].X;
        int y = CircleLUT[num].Pix[q].Y;

        if (CircleLUT[num].Pix[q].Direction == 0)
            BG_GFX_SUB[x + 128 + ((y + 96) * 256)] = RGB15(31, 31, 31) | BIT(15);

        if (CircleLUT[num].Pix[q].Direction == 1)
            BG_GFX_SUB[x + 128 + ((y + 96) * 256)] = RGB15(0, 31, 31) | BIT(15);

        if (CircleLUT[num].Pix[q].Direction == 2)
            BG_GFX_SUB[x + 128 + ((y + 96) * 256)] = RGB15(31, 0, 31) | BIT(15);

        if (CircleLUT[num].Pix[q].Direction == 3)
            BG_GFX_SUB[x + 128 + ((y + 96) * 256)] = RGB15(31, 31, 0) | BIT(15);

        if (CircleLUT[num].Pix[q].Direction == 4)
            BG_GFX_SUB[x + 128 + ((y + 96) * 256)] = RGB15(0, 31, 0) | BIT(15);

        if (CircleLUT[num].Pix[q].Direction == 5)
            BG_GFX_SUB[x + 128 + ((y + 96) * 256)] = RGB15(0, 0, 31) | BIT(15);

        if (CircleLUT[num].Pix[q].Direction == 6)
            BG_GFX_SUB[x + 128 + ((y + 96) * 256)] = RGB15(31, 0, 0) | BIT(15);

        if (CircleLUT[num].Pix[q].Direction == 7)
            BG_GFX_SUB[x + 128 + ((y + 96) * 256)] = RGB15(0, 0, 0) | BIT(15);
    }
}

void Checkcolision(int x, int y, int sx, int sy, int rad, bool *values)
{
    for (int a = 0; a < 8; a++)
        values[a] = true;

    for (int q = 0; q < CircleLUT[rad].count; q++)
    {
        int X = CircleLUT[rad].Pix[q].X;
        int Y = CircleLUT[rad].Pix[q].Y;

        if (Passable(x, y, sx + X, sy + Y) == false)
            values[CircleLUT[rad].Pix[q].Direction] = false;
    }
}

void CheckcolisionDiagonal(int x, int y, int sx, int sy, int rad, bool *values)
{
    for (int a = 0; a < 8; a++)
        values[a] = true;

    for (int q = 0; q < CircleLUT[rad].count; q++)
    {
        int X = CircleLUT[rad].Pix[q].X;
        int Y = CircleLUT[rad].Pix[q].Y;

        if (Passable(x, y, sx + X, sy + Y) == false)
            values[CircleLUT[rad].Pix[q].DirectionDiagonal] = false;
    }
}
