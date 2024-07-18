#include "GfxEngine/3D.h"

extern u8 EnvR, EnvG, EnvB;
extern int Blend;

int WallX, WallY;
bool walltrans = false;

v16 mode0vertex  = floattov16(-1);
v16 mode1vertex  = floattov16(1);
v16 mode0vertex2 = floattov16(.8);
v16 mode1vertex2 = floattov16(-.8);

void SetWallTrans(bool bol)
{
    walltrans = bol;
}

void SetCurWall(int x, int y)
{
    WallX = x;
    WallY = y;
}

t16 walltex1  = inttot16(0);
t16 walltex1e = inttot16(31);

t16 walltex2  = inttot16(32);
t16 walltex2e = inttot16(64);

//________________________________________________________________________
void RenderHouseM(int mode, f32 x, f32 y, f32 z)
{
    if (mode != 0 && mode != 1)
        return;

    if (!walltrans)
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0);
    if (walltrans)
        return;

    glPushMatrix();
    glTranslatef32(x, y, z);
    // glNormal3f(0, 1.0f, 0);
    u8 r = EnvR, g = EnvG, b = EnvB;
    if (Blend != 8) {
        r = EnvR / 8 * Blend;
        g = EnvG / 8 * Blend;
        b = EnvB / 8 * Blend;
    }
    glColor3b(r, g, b);

    // Dach GERADE
    glBegin(GL_QUADS);

    {
        // Bottom left
        glTexCoord2t16(walltex2, walltex2);
        glVertex3v16(floattov16(-.5), floattov16(1.3), floattov16(-.5));

        // Top left
        glTexCoord2t16(walltex2e, walltex2);
        glVertex3v16(floattov16(-.5), floattov16(1.3), floattov16(.5));

        // Top right
        glTexCoord2t16(walltex2e, walltex2e);
        glVertex3v16(floattov16(.5), floattov16(1.3), floattov16(.5));

        // Bottom right
        glTexCoord2t16(walltex2, walltex2e);
        glVertex3v16(floattov16(.5), floattov16(1.3), floattov16(-.5));
    }

    glEnd();
    glPopMatrix(1);
}

//________________________________________________________________________

void RenderHouseBorder(int mode, int dirrection, f32 x, f32 y, f32 z)
{
    if (mode != 0 && mode != 1)
        return;
    v16 vert, vert2;
    if (mode == 0) {
        vert  = mode0vertex;
        vert2 = mode0vertex2;
    }
    if (mode == 1) {
        vert  = mode1vertex;
        vert2 = mode1vertex2;
    }

    if (!walltrans || mode == 1)
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));
    if (walltrans)
        glPolyFmt(POLY_ALPHA(7) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));
    glPushMatrix();

    glTranslatef32(x, y, z);
    glRotateYi((DEGREES_IN_CIRCLE / 512) * (dirrection * -128));
    u8 r = EnvR, g = EnvG, b = EnvB;

    int LightX1 = 0, LightY1 = 0;
    int LightX2 = 0, LightY2 = 0;

    if (dirrection == 0) {
        LightX1 = WallX;
        LightY1 = WallY + 1;
        LightX2 = WallX + 1;
        LightY2 = WallY + 1;
    }

    if (dirrection == 1) {
        LightX1 = WallX;
        LightY1 = WallY;
        LightX2 = WallX;
        LightY2 = WallY + 1;
    }

    if (dirrection == 2) {
        LightX1 = WallX + 1;
        LightY1 = WallY;
        LightX2 = WallX;
        LightY2 = WallY;
    }

    if (dirrection == 3) {
        LightX1 = WallX + 1;
        LightY1 = WallY + 1;
        LightX2 = WallX + 1;
        LightY2 = WallY;
    }

    // Wand Down
    glBegin(GL_QUADS);

    {
        // Bottom left
        GrapLight(LightX1, LightY1);
        glTexCoord2t16(walltex2e, walltex1);
        glVertex3v16(floattov16(-.5), vert, floattov16(.5));

        // Top left
        glTexCoord2t16(walltex1, walltex1);
        glVertex3v16(floattov16(-.5), vert2, floattov16(.5));

        // Top right
        GrapLight(LightX2, LightY2);
        glTexCoord2t16(walltex1, walltex1e);
        glVertex3v16(floattov16(.5), vert2, floattov16(.5));

        // Bottom right
        glTexCoord2t16(walltex2e, walltex1e);
        glVertex3v16(floattov16(.5), vert, floattov16(.5));
    }

    glEnd();

    // Dach Unten
    if (mode == 0 && !walltrans) {
        if (Blend != 8) {
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

    glPopMatrix(1);
}

//__________________________________________________________
void RenderHouseCorner(int mode, int dirrection, f32 x, f32 y, f32 z)
{
    if (mode != 0 && mode != 1)
        return;

    v16 vert, vert2;
    if (mode == 0) {
        vert  = mode0vertex;
        vert2 = mode0vertex2;
    }
    if (mode == 1) {
        vert  = mode1vertex;
        vert2 = mode1vertex2;
    }

    if (!walltrans || mode == 1)
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));
    if (walltrans)
        glPolyFmt(POLY_ALPHA(7) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(3));

    glPushMatrix();
    glTranslatef32(x, y, z);
    glRotateYi((DEGREES_IN_CIRCLE / 512) * (dirrection * -128));

    u8 r = EnvR, g = EnvG, b = EnvB;

    int LightX1 = 0, LightY1 = 0;
    int LightX2 = 0, LightY2 = 0;
    int LightX3 = 0, LightY3 = 0;

    if (dirrection == 0) {
        LightX1 = WallX;
        LightY1 = WallY + 1;
        LightX2 = WallX + 1;
        LightY2 = WallY + 1;
        LightX3 = WallX;
        LightY3 = WallY;
    }

    if (dirrection == 1) {
        LightX1 = WallX;
        LightY1 = WallY;
        LightX2 = WallX;
        LightY2 = WallY + 1;
        LightX3 = WallX + 1;
        LightY3 = WallY;
    }

    if (dirrection == 2) {
        LightX1 = WallX + 1;
        LightY1 = WallY;
        LightX2 = WallX;
        LightY2 = WallY;
        LightX3 = WallX + 1;
        LightY3 = WallY + 1;
    }

    if (dirrection == 3) {
        LightX1 = WallX + 1;
        LightY1 = WallY + 1;
        LightX2 = WallX + 1;
        LightY2 = WallY;
        LightX3 = WallX;
        LightY3 = WallY + 1;
    }

    // Wand Down
    glBegin(GL_QUADS);

    {
        // Bottom left
        GrapLight(LightX1, LightY1);
        glTexCoord2t16(walltex2e, walltex1);
        glVertex3v16(floattov16(-.5), vert, floattov16(.5));

        // Top left
        glTexCoord2t16(walltex1, walltex1);
        glVertex3v16(floattov16(-.5), vert2, floattov16(.5));

        // Top right
        GrapLight(LightX2, LightY2);
        glTexCoord2t16(walltex1, walltex1e);
        glVertex3v16(floattov16(.5), vert2, floattov16(.5));

        // Bottom right
        glTexCoord2t16(walltex2e, walltex1e);
        glVertex3v16(floattov16(.5), vert, floattov16(.5));
    }

    glEnd();

    // Wand Left
    glBegin(GL_QUADS);

    {
        // Bottom left
        GrapLight(LightX3, LightY3);
        glTexCoord2t16(walltex2e, walltex1);
        glVertex3v16(floattov16(-.5), vert, floattov16(-.5));

        // Top left
        glTexCoord2t16(walltex1, walltex1);
        glVertex3v16(floattov16(-.5), vert2, floattov16(-.5));

        // Top right
        GrapLight(LightX1, LightY1);
        glTexCoord2t16(walltex1, walltex1e);
        glVertex3v16(floattov16(-.5), vert2, floattov16(.5));

        // Bottom right
        glTexCoord2t16(walltex2e, walltex1e);
        glVertex3v16(floattov16(-.5), vert, floattov16(.5));
    }

    glEnd();

    if (mode == 0 && !walltrans) {
        if (Blend != 8) {
            r = EnvR / 8 * Blend;
            g = EnvG / 8 * Blend;
            b = EnvB / 8 * Blend;
        }
        glColor3b(r, g, b);

        // Dach Part-Links
        glBegin(GL_TRIANGLE);

        {
            // Bottom left
            glTexCoord2t16(walltex2, walltex2);
            glVertex3v16(floattov16(-.5), floattov16(.8), floattov16(-.5));

            // Top left
            glTexCoord2t16(walltex2, walltex2e);
            glVertex3v16(floattov16(-.5), floattov16(.8), floattov16(.5));

            // Bottom right
            glTexCoord2t16(walltex1, walltex2);
            glVertex3v16(floattov16(.5), floattov16(1.3), floattov16(-.5));
        }

        glEnd();

        // Dach Part-Unten
        glBegin(GL_TRIANGLE);

        {
            // Top left
            glTexCoord2t16(walltex1e, walltex1e);
            glVertex3v16(floattov16(-.5), floattov16(.8), floattov16(.5));

            // Top right
            glTexCoord2t16(walltex1e, walltex2e);
            glVertex3v16(floattov16(.5), floattov16(.8), floattov16(.5));

            // Bottom right
            glTexCoord2t16(walltex1, walltex2e);
            glVertex3v16(floattov16(.5), floattov16(1.3), floattov16(-.5));
        }

        glEnd();
    }
    glPopMatrix(1);
}
