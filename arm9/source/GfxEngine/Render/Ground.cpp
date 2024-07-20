#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Render/Ground.h"
#include "GfxEngine/Texture/DynamicLights.h"
#include "GfxEngine/Texture/Light.h"

int GroundX, GroundY;

// Speeds up a bit when thoose values are used (which are used often and are fixed)
const t16 GroundTextPosStart[4] = { inttot16(0), inttot16(32), inttot16(64), inttot16(96) };
const t16 GroundTextPosEnd[4] = { inttot16(32), inttot16(64), inttot16(96), inttot16(128) };

GroundInfo Ground[Ground_Max];

void SetCurGround(int x, int y)
{
    GroundX = x;
    GroundY = y;
}

// The way a ground texture should be loaded
void LoadGroundTexture(char filename[], int num)
{
    if ((num < 0) || (num >= Ground_Max))
        Crash("Invalid ground number:\n%d (max %d)", num, Ground_Max);

    u8 *buffer8;
    u16 *pal;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &buffer8, &pal, &height, &width);

    if (width != height || (width != 32 && width != 64))
        Crash("Texture isn't 32x32 or 64x64\nSize: %lux%lu\n%s", width, height, filename);

    glGenTextures(1, &(Ground[num].Texture));
    glBindTexture(0, Ground[num].Texture);

    WaitForFreeVblank();

    Ground[num].Size = width;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, width, width, 0, TEXGEN_TEXCOORD, buffer8);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    free(buffer8);
    free(pal);
}

const v16 vmini = floattov16(0.025f);

// The way a ground texture should be rendered (floor inside and outside buildings).
void RenderGround(int x, int y, int z, int textnum)
{
    v16x4 pointer = Terrain[GroundX][GroundY];

    int mode = (pointer.sidewalls >> 6) & 3;
    int a;
    int ground_x, ground_y;

    t16 start = GroundTextPosStart[0];
    t16 end;
    if (Ground[textnum].Size == 64)
        end = GroundTextPosEnd[1];
    else
        end = GroundTextPosEnd[0];

    if (Ground[textnum].BumpEnable == true)
        a = 3;
    else
        a = 0;

    glPushMatrix();
    glTranslatef32(inttof32(x), inttof32(0), inttof32(z));

    for (int b = 0; b <= a; b++) {
        v16 v1, v2, v3, v4;
        bool doit = false;

        if (b == 0) {
            glBindTexture(GL_TEXTURE_2D, Ground[textnum].Texture);
            ground_x = GroundX;
            ground_y = GroundY;
            v1   = pointer.v[0];
            v2   = pointer.v[1];
            v3   = pointer.v[2];
            v4   = pointer.v[3];
            doit = true;

            if (Ground[TexGround[GroundX][GroundY]].TransEnable == false)
                glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
            else
                glPolyFmt(POLY_ALPHA(15) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
        }

        if (b == 1) {
            v1 += vmini;
            v2 += vmini;
            v3 += vmini;
            v4 += vmini;
        }

        if (b == 1 && ViewportMapBumpGroundS[GroundX - CamPosX + 5][GroundY - CamPosY + 3]) {
            glBindTexture(GL_TEXTURE_2D, Ground[textnum].TextureS);
            ground_x = GroundX;
            ground_y = GroundY + 1;
            doit = true;
        }

        if (b == 2 && ViewportMapBumpGroundW[GroundX - CamPosX + 5][GroundY - CamPosY + 3]) {
            glBindTexture(GL_TEXTURE_2D, Ground[textnum].TextureW);
            ground_x = GroundX - 1;
            ground_y = GroundY;
            doit = true;
        }

        if (b == 3 && ViewportMapBumpGroundE[GroundX - CamPosX + 5][GroundY - CamPosY + 3]) {
            glBindTexture(GL_TEXTURE_2D, Ground[textnum].TextureE);
            ground_x = GroundX + 1;
            ground_y = GroundY;
            doit = true;
        }

        if (mode == 0 && doit) {
            glBegin(GL_QUADS);

            {
                // Bottom left
                GrapLight(ground_x, ground_y);
                glTexCoord2t16(start, start);
                glVertex3v16(GroundVertex2, v1, GroundVertex2);

                // Top left
                GrapLight(ground_x, ground_y + 1);
                glTexCoord2t16(start, end);
                glVertex3v16(GroundVertex2, v3, GroundVertex1);

                // Top right
                GrapLight(ground_x + 1, ground_y + 1);
                glTexCoord2t16(end, end);
                glVertex3v16(GroundVertex1, v4, GroundVertex1);

                // Bottom right
                GrapLight(ground_x + 1, ground_y);
                glTexCoord2t16(end, start);
                glVertex3v16(GroundVertex1, v2, GroundVertex2);
            }

            glEnd();
        }

        if (mode == 1 && doit) {
            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(ground_x, ground_y);
                glTexCoord2t16(start, start);
                glVertex3v16(GroundVertex2, v1, GroundVertex2);

                // Top left
                GrapLight(ground_x, ground_y + 1);
                glTexCoord2t16(start, end);
                glVertex3v16(GroundVertex2, v3, GroundVertex1);

                // Top right
                GrapLight(ground_x + 1, ground_y + 1);
                glTexCoord2t16(end, end);
                glVertex3v16(GroundVertex1, v4, GroundVertex1);
                //***********************
                // Top right
                GrapLight(ground_x + 1, ground_y + 1);
                glTexCoord2t16(end, end);
                glVertex3v16(GroundVertex1, v4, GroundVertex1);

                // Bottom right
                GrapLight(ground_x + 1, ground_y);
                glTexCoord2t16(end, start);
                glVertex3v16(GroundVertex1, v2, GroundVertex2);

                // Bottom left
                GrapLight(ground_x, ground_y);
                glTexCoord2t16(start, start);
                glVertex3v16(GroundVertex2, v1, GroundVertex2);
            }

            glEnd();
        }

        if (mode == 2 && doit) {
            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(ground_x, ground_y);
                glTexCoord2t16(start, start);
                glVertex3v16(GroundVertex2, v1, GroundVertex2);

                // Top left
                GrapLight(ground_x, ground_y + 1);
                glTexCoord2t16(start, end);
                glVertex3v16(GroundVertex2, v3, GroundVertex1);

                // Bottom right
                GrapLight(ground_x + 1, ground_y);
                glTexCoord2t16(end, start);
                glVertex3v16(GroundVertex1, v2, GroundVertex2);
                //***********************
                // Top right
                GrapLight(ground_x + 1, ground_y + 1);
                glTexCoord2t16(end, end);
                glVertex3v16(GroundVertex1, v4, GroundVertex1);

                // Bottom right
                GrapLight(ground_x + 1, ground_y);
                glTexCoord2t16(end, start);
                glVertex3v16(GroundVertex1, v2, GroundVertex2);

                // Top left
                GrapLight(ground_x, ground_y + 1);
                glTexCoord2t16(start, end);
                glVertex3v16(GroundVertex2, v3, GroundVertex1);
            }

            glEnd();
        }
    }
    glPopMatrix(1);
}

// The way a levelborder should be rendered
void RenderLevelBorderGround(int x, int y, int z)
{
    t16 cx = 0, cy = 0;

    v16 GroundVert1 = floattov16(-.5);
    v16 GroundVert2 = floattov16(.5);

    glPushMatrix();
    glTranslatef32(inttof32(x), inttof32(0), inttof32(z));

    GFX_COLOR = 0;

    glBegin(GL_QUADS);

    {
        // Bottom left
        glTexCoord2t16(cy, cx);
        glVertex3v16(GroundVert2, GetTerrain(GroundX, GroundY, 0), GroundVert2);

        // Top left
        glTexCoord2t16(cy, cx);
        glVertex3v16(GroundVert2, GetTerrain(GroundX, GroundY, 2), GroundVert1);

        // Top right
        glTexCoord2t16(cy, cx);
        glVertex3v16(GroundVert1, GetTerrain(GroundX, GroundY, 3), GroundVert1);

        // Bottom right
        glTexCoord2t16(cy, cx);
        glVertex3v16(GroundVert1, GetTerrain(GroundX, GroundY, 1), GroundVert2);
    }

    glEnd();
    glPopMatrix(1);
}
