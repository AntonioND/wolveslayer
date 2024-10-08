// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Render/Obj_House.h"
#include "GfxEngine/Texture/Light.h"

// TODO: Only one door texture supported for now
static int Door[1];

static const t16 overdoor = inttot16(17);
static const t16 ldoor    = inttot16(13);
static const t16 rdoor    = inttot16(21);

static int newcol1[3]; // for interpolated lights...
static int newcol2[3]; // stored to recycle

void LoadDoorTexture(char filename[])
{
    u8 *buffer8;
    u16 *pal;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &buffer8, &pal, &height, &width);

    if (width != 32 || height != 64)
        Crash("Texture isn't 32x64: %lux%lu\n%s", width, height, filename);

    glGenTextures(1, &Door[0]);
    glBindTexture(0, Door[0]);

    WaitForFreeVblank();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 32, 64, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffer8);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    free(buffer8);
    free(pal);
}

static void RenderDoorOutside(int mode, float angle, f32 x, f32 y, f32 z, bool trans)
{
    if (mode != 0 && mode != 1)
        return;

    if (trans)
        glPolyFmt(POLY_ALPHA(7) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));
    else
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));

    glBindTexture(GL_TEXTURE_2D, Door[0]);

    int alpha = 7;

    if (!walltrans || mode == 1)
        alpha = 31;

    v16 vert, vert2;
    if (mode == 0)
    {
        vert  = mode0vertex;
        vert2 = floattov16(.25);
        glPolyFmt(POLY_ALPHA(alpha) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(1));
    }
    if (mode == 1)
    {
        vert  = mode1vertex;
        vert2 = floattov16(-.25);
        glPolyFmt(POLY_ALPHA(alpha) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(0));
    }

    glPushMatrix();
    glTranslatef32(x, y, z);
    glRotatef(angle, 0, 1, 0);
    u8 r = EnvR, g = EnvG, b = EnvB;
    if (Blend != 8)
    {
        r = EnvR / 8 * Blend;
        g = EnvG / 8 * Blend;
        b = EnvB / 8 * Blend;
    }
    glColor3b(r, g, b);

    // Wall left
    glBegin(GL_QUADS);

    {
        // Bottom left
        glColor3b((u8)newcol1[0], (u8)newcol1[1], (u8)newcol1[2]);
        glTexCoord2t16(walltex1, walltex2e);
        glVertex3v16(floattov16(0), vert, floattov16(0));

        // Top left
        glTexCoord2t16(walltex1, walltex1);
        glVertex3v16(floattov16(0), vert2, floattov16(0));

        // Top right
        glColor3b((u8)newcol2[0], (u8)newcol2[1], (u8)newcol2[2]);
        glTexCoord2t16(walltex1e, walltex1);
        glVertex3v16(floattov16(.625), vert2, floattov16(0));

        // Bottom right
        glTexCoord2t16(walltex1e, walltex2e);
        glVertex3v16(floattov16(.625), vert, floattov16(0));
    }

    glEnd();

    glPopMatrix(1);
}
//____________________________________________________________________________________________

void RenderHouseBorderDoor(int mode, int doorangle, int direction, f32 x, f32 y, f32 z)
{
    if (mode != 0 && mode != 1)
        return;

    bool trans = true;

    if (walltrans && mode != 1)
        glPolyFmt(POLY_ALPHA(7) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));
    else
    {
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));
        trans = false;
    }

    v16 vert, vert2, vert3;
    if (mode == 0)
    {
        vert  = mode0vertex;
        vert2 = mode0vertex2;
        vert3 = floattov16(.25);
    }
    if (mode == 1)
    {
        vert  = mode1vertex;
        vert2 = mode1vertex2;
        vert3 = floattov16(-.25);
    }

    glPushMatrix();

    glTranslatef32(x, y, z);
    glRotateYi((DEGREES_IN_CIRCLE / 512) * (direction * -128));
    u8 r = EnvR, g = EnvG, b = EnvB;

    int LightX1 = 0, LightY1 = 0;
    int LightX2 = 0, LightY2 = 0;

    u8 col1[3];
    u8 col2[3];

    if (direction == 0)
    {
        LightX1 = WallX;
        LightY1 = WallY + 1;
        LightX2 = WallX + 1;
        LightY2 = WallY + 1;
    }

    if (direction == 1)
    {
        LightX1 = WallX;
        LightY1 = WallY;
        LightX2 = WallX;
        LightY2 = WallY + 1;
    }

    if (direction == 2)
    {
        LightX1 = WallX + 1;
        LightY1 = WallY;
        LightX2 = WallX;
        LightY2 = WallY;
    }

    if (direction == 3)
    {
        LightX1 = WallX + 1;
        LightY1 = WallY + 1;
        LightX2 = WallX + 1;
        LightY2 = WallY;
    }

    // Here comes that interpolation

    GiveLight(LightX1, LightY1, col1);
    GiveLight(LightX2, LightY2, col2);

    newcol1[0] = f32toint(mulf32(floattov16(.625), inttof32(col1[0])) + mulf32(floattov16(1 - .625), inttof32(col2[0])));
    newcol1[1] = f32toint(mulf32(floattov16(.625), inttof32(col1[1])) + mulf32(floattov16(1 - .625), inttof32(col2[1])));
    newcol1[2] = f32toint(mulf32(floattov16(.625), inttof32(col1[2])) + mulf32(floattov16(1 - .625), inttof32(col2[2])));
    if (newcol1[0] > 255)
        newcol1[0] = 255;
    if (newcol1[1] > 255)
        newcol1[1] = 255;
    if (newcol1[2] > 255)
        newcol1[2] = 255;

    newcol2[0] = f32toint(mulf32(floattov16(.625), inttof32(col2[0])) + mulf32(floattov16(1 - .625), inttof32(col1[0])));
    newcol2[1] = f32toint(mulf32(floattov16(.625), inttof32(col2[1])) + mulf32(floattov16(1 - .625), inttof32(col1[1])));
    newcol2[2] = f32toint(mulf32(floattov16(.625), inttof32(col2[2])) + mulf32(floattov16(1 - .625), inttof32(col1[2])));

    if (newcol2[0] > 255)
        newcol2[0] = 255;
    if (newcol2[1] > 255)
        newcol2[1] = 255;
    if (newcol2[2] > 255)
        newcol2[2] = 255;

    // Wall top
    glBegin(GL_QUADS);

    {
        // Bottom left
        GrapLight(LightX1, LightY1);
        glTexCoord2t16(overdoor, walltex1);
        glVertex3v16(floattov16(-.5), vert3, floattov16(.5));

        // Top left
        glTexCoord2t16(walltex1, walltex1);
        glVertex3v16(floattov16(-.5), vert2, floattov16(.5));

        // Top right
        GrapLight(LightX2, LightY2);
        glTexCoord2t16(walltex1, walltex1e);
        glVertex3v16(floattov16(.5), vert2, floattov16(.5));

        // Bottom right
        glTexCoord2t16(overdoor, walltex1e);
        glVertex3v16(floattov16(.5), vert3, floattov16(.5));
    }

    glEnd();

    // Wall left
    glBegin(GL_QUADS);

    {
        // Bottom left
        GrapLight(LightX1, LightY1);
        glTexCoord2t16(walltex2e, walltex1);
        glVertex3v16(floattov16(-.5), vert, floattov16(.5));

        // Top left
        glTexCoord2t16(overdoor, walltex1);
        glVertex3v16(floattov16(-.5), vert3, floattov16(.5));

        // Top right
        glColor3b((u8)newcol1[0], (u8)newcol1[1], (u8)newcol1[2]);
        glTexCoord2t16(overdoor, ldoor);
        glVertex3v16(floattov16(-.3125), vert3, floattov16(.5));

        // Bottom right
        glTexCoord2t16(walltex2e, ldoor);
        glVertex3v16(floattov16(-.3125), vert, floattov16(.5));
    }

    glEnd();

    // Wall right
    glBegin(GL_QUADS);

    {
        // Bottom left
        glColor3b((u8)newcol2[0], (u8)newcol2[1], (u8)newcol2[2]);
        glTexCoord2t16(walltex2e, rdoor);
        glVertex3v16(floattov16(.3125), vert, floattov16(.5));

        // Top left
        glTexCoord2t16(overdoor, rdoor);
        glVertex3v16(floattov16(.3125), vert3, floattov16(.5));

        // Top right
        GrapLight(LightX2, LightY2);
        glTexCoord2t16(overdoor, walltex1e);
        glVertex3v16(floattov16(.5), vert3, floattov16(.5));

        // Bottom right
        glTexCoord2t16(walltex2e, walltex1e);
        glVertex3v16(floattov16(.5), vert, floattov16(.5));
    }

    glEnd();

    // Roof bottom
    if (mode == 0)
    {
        if (!walltrans)
        {
            if (Blend != 8)
            {
                r = EnvR / 8 * Blend;
                g = EnvG / 8 * Blend;
                b = EnvB / 8 * Blend;
            }
            glColor3b(r, g, b);
            glBegin(GL_QUADS);

            {
                // Bottom left
                glTexCoord2t16(walltex1, walltex2);
                glVertex3v16(floattov16(-.5), floattov16(1.3), floattov16(-.5));

                // Top left
                glTexCoord2t16(walltex2, walltex2);
                glVertex3v16(floattov16(-.5), floattov16(.8), floattov16(.5));

                // Top right
                glTexCoord2t16(walltex2, walltex2e);
                glVertex3v16(floattov16(.5), floattov16(.8), floattov16(.5));

                // Bottom right
                glTexCoord2t16(walltex1, walltex2e);
                glVertex3v16(floattov16(.5), floattov16(1.3), floattov16(-.5));
            }

            glEnd();
        }
    }

    glPopMatrix(1);
    if (direction == 0)
        RenderDoorOutside(mode, doorangle * -1, x - floattov16(.3125), y, z + floattov16(.5), trans);
    if (direction == 2)
        RenderDoorOutside(mode, 180 + doorangle * -1, x + floattov16(.3125), y, z - floattov16(.5), trans);
    if (direction == 1)
        RenderDoorOutside(mode, 90 + doorangle * -1, x - floattov16(.5), y, z - floattov16(.3125), trans);
    if (direction == 3)
        RenderDoorOutside(mode, -90 + doorangle * -1, x + floattov16(.5), y, z + floattov16(.3125), trans);
}

void RenderWallBorderDoor(int mode, int doorangle, int direction, f32 x, f32 y, f32 z)
{
    if (mode != 0 && mode != 1)
        return;

    bool trans = true;

    if (walltrans && direction == 2 && mode != 1)
        glPolyFmt(POLY_ALPHA(7) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));
    else
    {
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));
        trans = false;
    }

    v16 vert, vert2, vert3;
    if (mode == 0)
    {
        vert  = mode0vertex;
        vert2 = mode0vertex2;
        vert3 = floattov16(.25);
    }
    if (mode == 1)
    {
        vert  = mode1vertex;
        vert2 = mode1vertex2;
        vert3 = floattov16(-.25);
    }

    u8 col1[3];
    u8 col2[3];

    int LightX1 = 0, LightY1 = 0;
    int LightX2 = 0, LightY2 = 0;

    if (direction == 0)
    {
        LightX1 = WallX;
        LightY1 = WallY + 1;
        LightX2 = WallX + 1;
        LightY2 = WallY + 1;
    }

    if (direction == 1)
    {
        LightX1 = WallX;
        LightY1 = WallY;
        LightX2 = WallX;
        LightY2 = WallY + 1;
    }

    if (direction == 2)
    {
        LightX1 = WallX + 1;
        LightY1 = WallY;
        LightX2 = WallX;
        LightY2 = WallY;
    }

    if (direction == 3)
    {
        LightX1 = WallX + 1;
        LightY1 = WallY + 1;
        LightX2 = WallX + 1;
        LightY2 = WallY;
    }

    GiveLight(LightX1, LightY1, col1);
    GiveLight(LightX2, LightY2, col2);

    newcol1[0] = f32toint(mulf32(floattov16(.625), inttof32(col1[0])) + mulf32(floattov16(1 - .625), inttof32(col2[0])));
    newcol1[1] = f32toint(mulf32(floattov16(.625), inttof32(col1[1])) + mulf32(floattov16(1 - .625), inttof32(col2[1])));
    newcol1[2] = f32toint(mulf32(floattov16(.625), inttof32(col1[2])) + mulf32(floattov16(1 - .625), inttof32(col2[2])));

    if (newcol1[0] > 255)
        newcol1[0] = 255;
    if (newcol1[1] > 255)
        newcol1[1] = 255;
    if (newcol1[2] > 255)
        newcol1[2] = 255;

    newcol2[0] = f32toint(mulf32(floattov16(.625), inttof32(col2[0])) + mulf32(floattov16(1 - .625), inttof32(col1[0])));
    newcol2[1] = f32toint(mulf32(floattov16(.625), inttof32(col2[1])) + mulf32(floattov16(1 - .625), inttof32(col1[1])));
    newcol2[2] = f32toint(mulf32(floattov16(.625), inttof32(col2[2])) + mulf32(floattov16(1 - .625), inttof32(col1[2])));

    if (newcol2[0] > 255)
        newcol2[0] = 255;
    if (newcol2[1] > 255)
        newcol2[1] = 255;
    if (newcol2[2] > 255)
        newcol2[2] = 255;

    if (direction != 2)
    {
        glPushMatrix();

        glTranslatef32(x, y, z);
        glRotateYi((DEGREES_IN_CIRCLE / 512) * (direction * -128));

        // Wall top
        glBegin(GL_QUADS);

        {
            // Bottom left
            GrapLight(LightX1, LightY1);
            glTexCoord2t16(overdoor, walltex1);
            glVertex3v16(floattov16(-.5), vert3, floattov16(.5));

            // Top left
            glTexCoord2t16(walltex1, walltex1);
            glVertex3v16(floattov16(-.5), vert2, floattov16(.5));

            // Top right
            GrapLight(LightX2, LightY2);
            glTexCoord2t16(walltex1, walltex1e);
            glVertex3v16(floattov16(.5), vert2, floattov16(.5));

            // Bottom right
            glTexCoord2t16(overdoor, walltex1e);
            glVertex3v16(floattov16(.5), vert3, floattov16(.5));
        }

        glEnd();

        // Wall left
        glBegin(GL_QUADS);

        {
            // Bottom left
            GrapLight(LightX1, LightY1);
            glTexCoord2t16(walltex2e, walltex1);
            glVertex3v16(floattov16(-.5), vert, floattov16(.5));

            // Top left
            glTexCoord2t16(overdoor, walltex1);
            glVertex3v16(floattov16(-.5), vert3, floattov16(.5));

            // Top right
            glColor3b((u8)newcol1[0], (u8)newcol1[1], (u8)newcol1[2]);
            glTexCoord2t16(overdoor, ldoor);
            glVertex3v16(floattov16(-.3125), vert3, floattov16(.5));

            // Bottom right
            glTexCoord2t16(walltex2e, ldoor);
            glVertex3v16(floattov16(-.3125), vert, floattov16(.5));
        }

        glEnd();

        // Wall right
        glBegin(GL_QUADS);

        {
            // Bottom left
            glColor3b((u8)newcol2[0], (u8)newcol2[1], (u8)newcol2[2]);
            glTexCoord2t16(walltex2e, rdoor);
            glVertex3v16(floattov16(.3125), vert, floattov16(.5));

            // Top left
            glTexCoord2t16(overdoor, rdoor);
            glVertex3v16(floattov16(.3125), vert3, floattov16(.5));

            // Top right
            GrapLight(LightX2, LightY2);
            glTexCoord2t16(overdoor, walltex1e);
            glVertex3v16(floattov16(.5), vert3, floattov16(.5));

            // Bottom right
            glTexCoord2t16(walltex2e, walltex1e);
            glVertex3v16(floattov16(.5), vert, floattov16(.5));
        }

        glEnd();

        glPopMatrix(1);
    }

    if (direction == 0)
        RenderDoorOutside(mode, doorangle * -1, x - floattov16(.3125), y, z + floattov16(.5), trans);
    if (direction == 2)
        RenderDoorOutside(mode, 180 + doorangle * -1, x + floattov16(.3125), y, z - floattov16(.5), trans);
    if (direction == 1)
        RenderDoorOutside(mode, 90 + doorangle * -1, x - floattov16(.5), y, z - floattov16(.3125), trans);
    if (direction == 3)
        RenderDoorOutside(mode, -90 + doorangle * -1, x + floattov16(.5), y, z + floattov16(.3125), trans);
}
