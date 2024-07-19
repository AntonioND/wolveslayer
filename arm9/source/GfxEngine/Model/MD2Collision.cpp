#include <math.h>

#include "GfxEngine/Model/MD2Models.h"

#define sgn(x) (x < 0 ? -1 : 1)

static void Putpixel(int x, int y, int modelnum)
{
    if (x > -1 && x < 21)
        if (y > -1 && y < 21)
            Models[modelnum].Image[y] |= (1 << x);
}

bool GetModelCollsisionXY(int num, int x, int y, int rot)
{
    int X = 0, Y = 0;
    if (rot == 0) {
        X = x;
        Y = y;
    }

    if (rot == 2) {
        X = y;
        Y = 20 - x;
    }

    if (rot == 4) {
        X = 20 - x;
        Y = 20 - y;
    }

    if (rot == 6) {
        X = 20 - y;
        Y = x;
    }

    if (num < 0 || num >= MD2_Max)
        return true;
    if (ModelEnable[num] == false)
        return true;
    // return Models[num].Image[y];
    if ((Models[num].Image[Y] & (1 << X)) != 0)
        return false;
    else
        return true;
}

static void Line(int a, int b, int c, int d, int modelnum)
{
    long u, s, v, d1x, d1y, d2x, d2y, m, n;
    int i;
    u   = c - a;
    v   = d - b;
    d1x = sgn(u);
    d1y = sgn(v);
    d2x = sgn(u);
    d2y = 0;
    m   = abs(u);
    n   = abs(v);
    if (m <= n) {
        d2x = 0;
        d2y = sgn(v);
        m   = abs(v);
        n   = abs(u);
    }
    s = (int)(m / 2);
    for (i = 0; i < round(m); i++) {
        Putpixel(a, b, modelnum);
        s += n;
        if (s >= m) {
            s -= m;
            a += d1x;
            b += d1y;
        } else {
            a += d2x;
            b += d2y;
        }
    }
}

void MakeCollisionMap(int modelnum)
{
    if (ModelEnable[modelnum] == false)
        return;

    // Fist we clear the 2D coolision image of the model
    for (int a = 0; a < 21; a++)
        Models[modelnum].Image[a] = 0;

    // here we draw a 2d-birdeyeviewed image of the model which
    // must fit in a tile!
    int i, j, vert1num, vert2num;
    float x1, x2, y1, y2;
    nds_vertex_t *vert = Models[modelnum].rahmen[0].verts;
    // lets draw da shit!
    for (i = 0; i < Models[modelnum].header.num_tris; ++i) {
        for (j = 0; j < 3; ++j) {
            // first get 2 indices for the 2 vertices which build a 2D line
            // from a bird-eye-view
            vert1num = Models[modelnum].dreiecke[i].vertex[j];
            if (j == 0)
                vert2num = Models[modelnum].dreiecke[i].vertex[1];
            if (j == 1)
                vert2num = Models[modelnum].dreiecke[i].vertex[2];
            if (j == 2)
                vert2num = Models[modelnum].dreiecke[i].vertex[0];

            x1 = (10.5 + (f32tofloat(vert[vert1num].v[1]) * 21.0));
            y1 = (10.5 + (f32tofloat(vert[vert1num].v[0]) * 21.0));
            x2 = (10.5 + (f32tofloat(vert[vert2num].v[1]) * 21.0));
            y2 = (10.5 + (f32tofloat(vert[vert2num].v[0]) * 21.0));

            if (x1 < 0)
                x1 = 0;
            if (x1 > 20)
                x1 = 20;

            if (x2 < 0)
                x2 = 0;
            if (x2 > 20)
                x2 = 20;

            if (y1 < 0)
                y1 = 0;
            if (y1 > 20)
                y1 = 20;

            if (y2 < 0)
                y2 = 0;
            if (y2 > 20)
                y2 = 20;

            Line((int)x1, (int)y1, (int)x2, (int)y2, modelnum);
        }
    }
}

// This draws on the bottom screen the shapes of the collision map of a model
// when it is rotated in 4 different directions. For example, the player model
// is displayed with "modelnum = 0".
void ShowCollisionMap(int modelnum)
{
    for (int y = 0; y < 21; y++) {
        for (int x = 0; x < 21; x++) {
            if (GetModelCollsisionXY(modelnum, x, y, 0))
                BG_GFX_SUB[x + (y * 256)] = RGB8(255, 255, 255) | BIT(15);
            else
                BG_GFX_SUB[x + (y * 256)] = RGB8(0, 0, 0) | BIT(15);

            if (GetModelCollsisionXY(modelnum, x, y, 2))
                BG_GFX_SUB[x + 22 + (y * 256)] = RGB8(255, 255, 255) | BIT(15);
            else
                BG_GFX_SUB[x + 22 + (y * 256)] = RGB8(0, 0, 0) | BIT(15);

            if (GetModelCollsisionXY(modelnum, x, y, 4))
                BG_GFX_SUB[x + 44 + (y * 256)] = RGB8(255, 255, 255) | BIT(15);
            else
                BG_GFX_SUB[x + 44 + (y * 256)] = RGB8(0, 0, 0) | BIT(15);

            if (GetModelCollsisionXY(modelnum, x, y, 6))
                BG_GFX_SUB[x + 66 + (y * 256)] = RGB8(255, 255, 255) | BIT(15);
            else
                BG_GFX_SUB[x + 66 + (y * 256)] = RGB8(0, 0, 0) | BIT(15);
        }
    }
}
