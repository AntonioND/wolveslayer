// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/3D.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Render/Obj_House.h"
#include "GfxEngine/Render/Obj_Wall.h"
#include "GfxEngine/Texture/Light.h"

ObjectTextureInfo ObjectTexture[ObjectTexture_Max];

// Renders walls inside buildings
void RenderWallBorder(int mode, int direction, f32 x, f32 y, f32 z, int textnum)
{
    if (mode != 0 && mode != 1)
        return;

    v16 vert, vert2;
    if (mode == 0)
    {
        vert  = mode0vertex;
        vert2 = mode0vertex2;
    }
    if (mode == 1)
    {
        vert  = mode1vertex;
        vert2 = mode1vertex2;
    }

    if (walltrans && direction == 2 && mode != 1)
        glPolyFmt(POLY_ALPHA(7) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));
    else
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));

    int LightX1[3], LightY1[3];
    int LightX2[3], LightY2[3];

    int layers = 0;
    if (IsObjBumpWall(WallX, WallY) == true)
        layers = 2;

    if (direction == 0)
    {
        LightX1[0] = WallX;
        LightY1[0] = WallY + 1;
        LightX2[0] = WallX + 1;
        LightY2[0] = WallY + 1;

        LightX1[1] = WallX + 1;
        LightY1[1] = WallY + 1;
        LightX2[1] = WallX + 2;
        LightY2[1] = WallY + 1;

        LightX1[2] = WallX - 1;
        LightY1[2] = WallY + 1;
        LightX2[2] = WallX;
        LightY2[2] = WallY + 1;
    }

    if (direction == 1)
    {
        LightX1[0] = WallX;
        LightY1[0] = WallY;
        LightX2[0] = WallX;
        LightY2[0] = WallY + 1;

        LightX1[1] = WallX;
        LightY1[1] = WallY + 1;
        LightX2[1] = WallX;
        LightY2[1] = WallY + 2;

        LightX1[2] = WallX;
        LightY1[2] = WallY - 1;
        LightX2[2] = WallX;
        LightY2[2] = WallY;
    }

    if (direction == 2)
        return;

    if (direction == 3)
    {
        LightX1[0] = WallX + 1;
        LightY1[0] = WallY + 1;
        LightX2[0] = WallX + 1;
        LightY2[0] = WallY;

        LightX1[1] = WallX + 1;
        LightY1[1] = WallY;
        LightX2[1] = WallX + 1;
        LightY2[1] = WallY - 1;

        LightX1[2] = WallX + 1;
        LightY1[2] = WallY + 2;
        LightX2[2] = WallX + 1;
        LightY2[2] = WallY + 1;
    }

    glPushMatrix();

    glTranslatef32(x, y, z);

    glRotateYi((DEGREES_IN_CIRCLE / 512) * (direction * -128));

    for (int a = 0; a <= layers; a++)
    {
        if (a == 1)
            glBindTexture(GL_TEXTURE_2D, ObjectTexture[textnum].TextureB);

        if (a == 2)
            glBindTexture(GL_TEXTURE_2D, ObjectTexture[textnum].TextureC);

        // Wall Down
        glBegin(GL_QUADS);

        {
            // Bottom left
            GrapLight(LightX1[a], LightY1[a]);
            glTexCoord2t16(walltex1, walltex2e);
            glVertex3v16(floattov16(-.5), vert, floattov16(.5));

            // Top left
            glTexCoord2t16(walltex1, walltex1);
            glVertex3v16(floattov16(-.5), vert2, floattov16(.5));

            // Top right
            GrapLight(LightX2[a], LightY2[a]);
            glTexCoord2t16(walltex1e, walltex1);
            glVertex3v16(floattov16(.5), vert2, floattov16(.5));

            // Bottom right
            glTexCoord2t16(walltex1e, walltex2e);
            glVertex3v16(floattov16(.5), vert, floattov16(.5));
        }

        glEnd();
    }

    glPopMatrix(1);
}
