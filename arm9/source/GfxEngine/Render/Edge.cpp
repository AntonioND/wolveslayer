#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/Limits.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Render/Edge.h"
#include "GfxEngine/Texture/DynamicLights.h"
#include "GfxEngine/Texture/Light.h"

// TODO: Only one edge is actually used
EdgeInfo Edge[Edge_Max];

static u8 left;
static u8 right;
static u8 front;

static const v16 plusterrain     = floattov16(.5);
static const v16 minusterrain    = floattov16(-.5);
static const t16 t_null          = inttot16(0);
static const t16 t_zweiundreizig = inttot16(32);

#if 0
static t16 heightot16up(v16 h)
{
    int f = f32toint(mulf32(h, inttof32(4)));

    f = (f << 3);

    return inttot16(56 - f);
    // return minusterrain;
}
#endif

static t16 heightot16down(v16 h)
{
    int f = f32toint(mulf32(h, inttof32(4)));

    f = (f << 3);

    return inttot16(56 - f);
    // return minusterrain;
}

static void WallEdge(int x, int y, f32 xx, f32 yy)
{
    v16 bl, tl, br, tr;
    t16 tbl, ttl, tbr, ttr;
    int a;
    int b = 0;

    if (left == 3)
    {
        if (Edge[0].HasBump && ViewportMapBumpWallW[x - CamPosX + 5][y - CamPosY + 3])
            b = 1;
        else
            b = 0;

        bl  = GetTerrain(x - 1, y, 1);
        tbl = heightot16down(bl);
        tl  = GetTerrain(x, y, 0);
        ttl = heightot16down(tl);
        tr  = GetTerrain(x, y, 2);
        ttr = heightot16down(tr);
        br  = GetTerrain(x - 1, y, 3);
        tbr = heightot16down(br);

        for (a = -b; a <= b; a++)
        {
            if (a == -1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureC);
            // if (a == 0 && b != 0)
            //     glBindTexture(GL_TEXTURE_2D, Edge[0].TextureA);
            if (a == 0)
                glBindTexture(GL_TEXTURE_2D, Edge[0].Texture);
            if (a == 1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureB);

            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
            glPushMatrix();
            glTranslatef32(xx, 0, yy);

            glBegin(GL_QUADS);

            {
                // Bottom left
                GrapLight(x, y + a);
                glTexCoord2t16(t_null, tbl);
                glVertex3v16(minusterrain, bl, minusterrain);

                // Top left
                glTexCoord2t16(t_null, ttl);
                glVertex3v16(minusterrain, tl, minusterrain);

                // Top right
                GrapLight(x, y + 1 + a);
                glTexCoord2t16(t_zweiundreizig, ttr);
                glVertex3v16(minusterrain, tr, plusterrain);

                // Bottom right
                glTexCoord2t16(t_zweiundreizig, tbr);
                glVertex3v16(minusterrain, br, plusterrain);
            }

            glEnd();
            glPopMatrix(1);
        }
    }

    if (right == 3)
    {
        if (Edge[0].HasBump && ViewportMapBumpWallE[x - CamPosX + 5][y - CamPosY + 3])
            b = 1;
        else
            b = 0;

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));

        bl  = GetTerrain(x + 1, y, 0);
        tbl = heightot16down(bl);
        tl  = GetTerrain(x, y, 1);
        ttl = heightot16down(tl);
        tr  = GetTerrain(x, y, 3);
        ttr = heightot16down(tr);
        br  = GetTerrain(x + 1, y, 2);
        tbr = heightot16down(br);

        for (a = -b; a <= b; a++)
        {
            if (a == -1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureC);
            // if (a == 0 && b != 0)
            //     glBindTexture(GL_TEXTURE_2D, Edge[0].TextureA);
            if (a == 0)
                glBindTexture(GL_TEXTURE_2D, Edge[0].Texture);
            if (a == 1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureB);

            glPushMatrix();
            glTranslatef32(xx, 0, yy);

            glBegin(GL_QUADS);

            {
                // Bottom left
                GrapLight(x + 1, y + a);
                glTexCoord2t16(t_null, tbl);
                glVertex3v16(plusterrain, bl, minusterrain);

                // Top left
                glTexCoord2t16(t_null, ttl);
                glVertex3v16(plusterrain, tl, minusterrain);

                // Top right
                GrapLight(x + 1, y + 1 + a);
                glTexCoord2t16(t_zweiundreizig, ttr);
                glVertex3v16(plusterrain, tr, plusterrain);

                // Bottom right
                glTexCoord2t16(t_zweiundreizig, tbr);
                glVertex3v16(plusterrain, br, plusterrain);
            }

            glEnd();
            glPopMatrix(1);
        }
    }

    if (front == 3)
    {
        if (Edge[0].HasBump && ViewportMapBumpWallS[x - CamPosX + 5][y - CamPosY + 3])
            b = 1;
        else
            b = 0;

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));

        bl  = GetTerrain(x, y + 1, 0);
        tbl = heightot16down(bl);
        tl  = GetTerrain(x, y, 2);
        ttl = heightot16down(tl);
        tr  = GetTerrain(x, y, 3);
        ttr = heightot16down(tr);
        br  = GetTerrain(x, y + 1, 2);
        tbr = heightot16down(br);

        for (a = -b; a <= b; a++)
        {
            if (a == -1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureC);
            // if (a == 0 && b != 0)
            //     glBindTexture(GL_TEXTURE_2D, Edge[0].TextureA);
            if (a == 0)
                glBindTexture(GL_TEXTURE_2D, Edge[0].Texture);
            if (a == 1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureB);

            glPushMatrix();
            glTranslatef32(xx, 0, yy);

            glBegin(GL_QUADS);

            {
                // Bottom left
                GrapLight(x + a, y + 1);
                glTexCoord2t16(t_null, tbl);
                glVertex3v16(minusterrain, bl, plusterrain);

                // Top left
                glTexCoord2t16(t_null, ttl);
                glVertex3v16(minusterrain, tl, plusterrain);

                // Top right
                GrapLight(x + 1 + a, y + 1);
                glTexCoord2t16(t_zweiundreizig, ttr);
                glVertex3v16(plusterrain, tr, plusterrain);

                // Bottom right
                glTexCoord2t16(t_zweiundreizig, tbr);
                glVertex3v16(plusterrain, br, plusterrain);
            }

            glEnd();
            glPopMatrix(1);
        }
    }
}

static void HalfWallEdgeA(int x, int y, f32 xx, f32 yy)
{
    v16 bl, tl, br;
    // v16 tr;
    t16 tbl, ttl, tbr;
    // t16 ttr;
    int a;
    int b = 0;

    if (left == 1)
    {
        if (Edge[0].HasBump && ViewportMapBumpWallW[x - CamPosX + 5][y - CamPosY + 3])
            b = 1;
        else
            b = 0;
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
        bl  = GetTerrain(x - 1, y, 1);
        tbl = heightot16down(bl);
        tl  = GetTerrain(x, y, 0);
        ttl = heightot16down(tl);
        // tr  = GetTerrain(x, y, 2);
        // ttr = heightot16down(tr);
        br  = GetTerrain(x - 1, y, 3);
        tbr = heightot16down(br);

        for (a = -b; a <= b; a++)
        {
            if (a == -1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureC);
            // if (a == 0 && b != 0)
            //     glBindTexture(GL_TEXTURE_2D, Edge[0].TextureA);
            if (a == 0)
                glBindTexture(GL_TEXTURE_2D, Edge[0].Texture);
            if (a == 1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureB);

            glPushMatrix();
            glTranslatef32(xx, 0, yy);

            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(x, y + a);
                glTexCoord2t16(t_null, tbl);
                glVertex3v16(minusterrain, bl, minusterrain);

                // Top left
                glTexCoord2t16(t_null, ttl);
                glVertex3v16(minusterrain, tl, minusterrain);
                GrapLight(x, y + 1 + a);

                // Bottom right
                glTexCoord2t16(t_zweiundreizig, tbr);
                glVertex3v16(minusterrain, br, plusterrain);
            }

            glEnd();
            glPopMatrix(1);
        }
    }

    if (right == 1)
    {
        if (Edge[0].HasBump && ViewportMapBumpWallE[x - CamPosX + 5][y - CamPosY + 3])
            b = 1;
        else
            b = 0;

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));

        bl  = GetTerrain(x + 1, y, 0);
        tbl = heightot16down(bl);
        tl  = GetTerrain(x, y, 1);
        ttl = heightot16down(tl);
        // tr  = GetTerrain(x, y, 3);
        // ttr = heightot16down(tr);
        br  = GetTerrain(x + 1, y, 2);
        tbr = heightot16down(br);

        for (a = -b; a <= b; a++)
        {
            if (a == -1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureC);
            // if (a == 0 && b != 0)
            //     glBindTexture(GL_TEXTURE_2D, Edge[0].TextureA);
            if (a == 0)
                glBindTexture(GL_TEXTURE_2D, Edge[0].Texture);
            if (a == 1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureB);

            glPushMatrix();
            glTranslatef32(xx, 0, yy);

            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(x + 1, y + a);
                glTexCoord2t16(t_null, tbl);
                glVertex3v16(plusterrain, bl, minusterrain);

                // Top left
                glTexCoord2t16(t_null, ttl);
                glVertex3v16(plusterrain, tl, minusterrain);
                GrapLight(x + 1, y + 1 + a);

                // Bottom right
                glTexCoord2t16(t_zweiundreizig, tbr);
                glVertex3v16(plusterrain, br, plusterrain);
            }

            glEnd();
            glPopMatrix(1);
        }
    }

    if (front == 1)
    {
        if (Edge[0].HasBump && ViewportMapBumpWallS[x - CamPosX + 5][y - CamPosY + 3])
            b = 1;
        else
            b = 0;

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));

        bl  = GetTerrain(x, y + 1, 0);
        tbl = heightot16down(bl);
        tl  = GetTerrain(x, y, 2);
        ttl = heightot16down(tl);
        // tr  = GetTerrain(x, y, 3);
        // ttr = heightot16down(tr);
        br  = GetTerrain(x, y + 1, 2);
        tbr = heightot16down(br);

        for (a = -b; a <= b; a++)
        {
            if (a == -1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureC);
            // if (a == 0 && b != 0)
            //     glBindTexture(GL_TEXTURE_2D, Edge[0].TextureA);
            if (a == 0)
                glBindTexture(GL_TEXTURE_2D, Edge[0].Texture);
            if (a == 1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureB);

            glPushMatrix();
            glTranslatef32(xx, 0, yy);

            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(x + a, y + 1);
                glTexCoord2t16(t_null, tbl);
                glVertex3v16(minusterrain, bl, plusterrain);

                // Top left
                glTexCoord2t16(t_null, ttl);
                glVertex3v16(minusterrain, tl, plusterrain);
                GrapLight(x + 1 + a, y + 1);

                // Bottom right
                glTexCoord2t16(t_zweiundreizig, tbr);
                glVertex3v16(plusterrain, br, plusterrain);
            }

            glEnd();
            glPopMatrix(1);
        }
    }
}

static void HalfWallEdgeB(int x, int y, f32 xx, f32 yy)
{
    v16 bl, br, tr;
    // v16 tl;
    t16 tbl, tbr, ttr;
    // t16 ttl;
    int a;
    int b = 0;

    if (left == 2)
    {
        if (Edge[0].HasBump && ViewportMapBumpWallW[x - CamPosX + 5][y - CamPosY + 3])
            b = 1;
        else
            b = 0;

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));

        bl  = GetTerrain(x - 1, y, 1);
        tbl = heightot16down(bl);
        // tl  = GetTerrain(x, y, 0);
        // ttl = heightot16down(tl);
        tr  = GetTerrain(x, y, 2);
        ttr = heightot16down(tr);
        br  = GetTerrain(x - 1, y, 3);
        tbr = heightot16down(br);

        for (a = -b; a <= b; a++)
        {
            if (a == -1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureC);
            // if (a == 0 && b != 0)
            //     glBindTexture(GL_TEXTURE_2D, Edge[0].TextureA);
            if (a == 0)
                glBindTexture(GL_TEXTURE_2D, Edge[0].Texture);
            if (a == 1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureB);

            glPushMatrix();
            glTranslatef32(xx, 0, yy);

            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(x, y + a);
                glTexCoord2t16(t_null, tbl);
                glVertex3v16(minusterrain, bl, minusterrain);

                // Top right
                GrapLight(x, y + 1 + a);
                glTexCoord2t16(t_zweiundreizig, ttr);
                glVertex3v16(minusterrain, tr, plusterrain);

                // Bottom right
                glTexCoord2t16(t_zweiundreizig, tbr);
                glVertex3v16(minusterrain, br, plusterrain);
            }

            glEnd();
            glPopMatrix(1);
        }
    }

    if (right == 2)
    {
        if (Edge[0].HasBump && ViewportMapBumpWallE[x - CamPosX + 5][y - CamPosY + 3])
            b = 1;
        else
            b = 0;

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));

        bl  = GetTerrain(x + 1, y, 0);
        tbl = heightot16down(bl);
        // tl  = GetTerrain(x, y, 1);
        // ttl = heightot16down(tl);
        tr  = GetTerrain(x, y, 3);
        ttr = heightot16down(tr);
        br  = GetTerrain(x + 1, y, 2);
        tbr = heightot16down(br);

        for (a = -b; a <= b; a++)
        {
            if (a == -1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureC);
            // if (a == 0 && b != 0)
            //     glBindTexture(GL_TEXTURE_2D, Edge[0].TextureA);
            if (a == 0)
                glBindTexture(GL_TEXTURE_2D, Edge[0].Texture);
            if (a == 1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureB);

            glPushMatrix();
            glTranslatef32(xx, 0, yy);

            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(x + 1, y + a);
                glTexCoord2t16(t_null, tbl);
                glVertex3v16(plusterrain, bl, minusterrain);

                // Top right
                GrapLight(x + 1, y + 1 + a);
                glTexCoord2t16(t_zweiundreizig, ttr);
                glVertex3v16(plusterrain, tr, plusterrain);

                // Bottom right
                glTexCoord2t16(t_zweiundreizig, tbr);
                glVertex3v16(plusterrain, br, plusterrain);
            }

            glEnd();
            glPopMatrix(1);
        }
    }

    if (front == 2)
    {
        if (Edge[0].HasBump && ViewportMapBumpWallS[x - CamPosX + 5][y - CamPosY + 3])
            b = 1;
        else
            b = 0;

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));

        bl  = GetTerrain(x, y + 1, 0);
        tbl = heightot16down(bl);
        // tl  = GetTerrain(x, y, 2);
        // ttl = heightot16down(tl);
        tr  = GetTerrain(x, y, 3);
        ttr = heightot16down(tr);
        br  = GetTerrain(x, y + 1, 2);
        tbr = heightot16down(br);

        for (a = -b; a <= b; a++)
        {
            if (a == -1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureC);
            // if(a == 0 && b != 0)
            //     glBindTexture(GL_TEXTURE_2D, Edge[0].TextureA);
            if (a == 0)
                glBindTexture(GL_TEXTURE_2D, Edge[0].Texture);
            if (a == 1)
                glBindTexture(GL_TEXTURE_2D, Edge[0].TextureB);

            glPushMatrix();
            glTranslatef32(xx, 0, yy);

            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(x + a, y + 1);
                glTexCoord2t16(t_null, tbl);
                glVertex3v16(minusterrain, bl, plusterrain);

                // Top right
                GrapLight(x + 1 + a, y + 1);
                glTexCoord2t16(t_zweiundreizig, ttr);
                glVertex3v16(plusterrain, tr, plusterrain);

                // Bottom right
                glTexCoord2t16(t_zweiundreizig, tbr);
                glVertex3v16(plusterrain, br, plusterrain);
            }

            glEnd();
            glPopMatrix(1);
        }
    }
}

void RenderEdge(int x, int y, f32 xx, f32 yy)
{
    u8 sidewalls = Terrain[x][y].sidewalls;

    if (sidewalls)
    {
        if (xx > inttof32(0))
            left = sidewalls & (3 << 0);
        else
            left = 0; // clip invisible sidewall

        if (xx < inttof32(0))
            right = (sidewalls & (3 << 2)) >> 2;
        else
            right = 0; // clip invisible sidewall

        front = (sidewalls & (3 << 4)) >> 4;

        WallEdge(x, y, xx, yy);
        HalfWallEdgeA(x, y, xx, yy);
        HalfWallEdgeB(x, y, xx, yy);
    }
}

void LoadEdgeTexture(char filename[], int *Target)
{
    u8 *buffer8;
    u16 *pal;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &buffer8, &pal, &height, &width);

    if (width != 32 || height != 64)
        Crash("Texture isn't 32x64\nSize: %lux%lu\n%s", width, height, filename);

    glGenTextures(1, Target);
    glBindTexture(0, *Target);

    WaitForFreeVblank();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 32, 64, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffer8);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    free(buffer8);
    free(pal);
}
