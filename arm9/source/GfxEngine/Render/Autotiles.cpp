#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Render/Autotiles.h"
#include "GfxEngine/Render/Ground.h"
#include "GfxEngine/Texture/Light.h"

AutotileInfo Autotile[Autotile_Max];

// The way a ground texture should be loaded
void LoadAutotileTexture(char filename[], int num)
{
    if ((num < 0) || (num >= Autotile_Max))
        Crash("Invalid number:%d\n%s", num, __func__);

    u8 *buffer8;
    u16 *pal;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &buffer8, &pal, &height, &width);

    if (width != 384 || height != 128)
        Crash("Texture isn't 384x128\nSize: %lux%lu\n%s", width, height, filename);

    swiWaitForVBlank();

    u8 *part = (u8 *)malloc(128 * 128);
    if (part == NULL)
        Crash("Not enough memory for buffer:\n%s", filename);

    int palTextureName = -1;

    // Let's load all 16 parts of 128x128 tile thing
    for (int ax = 0; ax < 3; ax++) {
        // Copy each pixel
        for (int bx = 0; bx < 128; bx++) {
            for (int by = 0; by < 128; by++)
                part[bx + (by * 128)] = buffer8[(bx + (ax * 128)) + (by * 384)];
        }

        glGenTextures(1, &(Autotile[num].Texture[ax]));
        glBindTexture(0, Autotile[num].Texture[ax]);

        WaitForFreeVblank();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 128, 128, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, part);

        if (palTextureName == -1) {
            // OK here we now have loaded pal and we got 36 fields a 32x32
            glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);
            palTextureName = Autotile[num].Texture[ax];
        } else {
            glAssignColorTable(0, palTextureName);
        }
    }

    free(buffer8);
    free(pal);
    free(part);
}

void AddAutIgnore(int Tilenum, u32 Col)
{
    // IgnorecolorsNum is initialized to -1, so this will start writing index 0
    Autotile[Tilenum].IgnorecolorsNum++;

    if (Autotile[Tilenum].IgnorecolorsNum >= Autotile_Ignorecolors_Max)
        Crash("Too many ignored colors");

    Autotile[Tilenum].Ignorecolors[Autotile[Tilenum].IgnorecolorsNum] = Col;
}

// The way a ground texture should be rendered
void RenderAutoGround(int x, int y, int z, int num)
{
    int cx = 0, cy = 0;
    v16x4 pointer = Terrain[GroundX][GroundY];

    if (num == 0) {
        cx = 0;
        cy = 0;
    }
    if (num == 1) {
        cx = 1;
        cy = 0;
    }
    if (num == 2) {
        cx = 2;
        cy = 0;
    }
    if (num == 3) {
        cx = 3;
        cy = 0;
    }

    if (num == 4) {
        cx = 0;
        cy = 1;
    }
    if (num == 5) {
        cx = 1;
        cy = 1;
    }
    if (num == 6) {
        cx = 2;
        cy = 1;
    }
    if (num == 7) {
        cx = 3;
        cy = 1;
    }

    if (num == 8) {
        cx = 0;
        cy = 2;
    }
    if (num == 9) {
        cx = 1;
        cy = 2;
    }
    if (num == 10) {
        cx = 2;
        cy = 2;
    }
    if (num == 11) {
        cx = 3;
        cy = 2;
    }

    if (num == 12) {
        cx = 0;
        cy = 3;
    }
    if (num == 13) {
        cx = 1;
        cy = 3;
    }
    if (num == 14) {
        cx = 2;
        cy = 3;
    }
    if (num == 15) {
        cx = 3;
        cy = 3;
    }

    glPushMatrix();
    glTranslatef32(inttof32(x), inttof32(0), inttof32(z));

    int mode = (pointer.sidewalls >> 6) & 3;

    if (mode == 0) {
        glBegin(GL_QUADS);
        // Bottom left
        GrapLight(GroundX, GroundY);
        glTexCoord2t16(GroundTextPosStart[cx], GroundTextPosStart[cy]);
        glVertex3v16(GroundVertex2, pointer.v[0], GroundVertex2);

        // Top left
        GrapLight(GroundX, GroundY + 1);
        glTexCoord2t16(GroundTextPosStart[cx], GroundTextPosEnd[cy]);
        glVertex3v16(GroundVertex2, pointer.v[2], GroundVertex1);

        // Top right
        GrapLight(GroundX + 1, GroundY + 1);
        glTexCoord2t16(GroundTextPosEnd[cx], GroundTextPosEnd[cy]);
        glVertex3v16(GroundVertex1, pointer.v[3], GroundVertex1);

        // Bottom right
        GrapLight(GroundX + 1, GroundY);
        glTexCoord2t16(GroundTextPosEnd[cx], GroundTextPosStart[cy]);
        glVertex3v16(GroundVertex1, pointer.v[1], GroundVertex2);
        glEnd();
    }

    if (mode == 1) {
        glBegin(GL_TRIANGLES);

        {
            // Bottom left
            GrapLight(GroundX, GroundY);
            glTexCoord2t16(GroundTextPosStart[cx], GroundTextPosStart[cy]);
            glVertex3v16(GroundVertex2, pointer.v[0], GroundVertex2);

            // Top left
            GrapLight(GroundX, GroundY + 1);
            glTexCoord2t16(GroundTextPosStart[cx], GroundTextPosEnd[cy]);
            glVertex3v16(GroundVertex2, pointer.v[2], GroundVertex1);

            // Top right
            GrapLight(GroundX + 1, GroundY + 1);
            glTexCoord2t16(GroundTextPosEnd[cx], GroundTextPosEnd[cy]);
            glVertex3v16(GroundVertex1, pointer.v[3], GroundVertex1);

            // ***********************

            // Top right
            GrapLight(GroundX + 1, GroundY + 1);
            glTexCoord2t16(GroundTextPosEnd[cx], GroundTextPosEnd[cy]);
            glVertex3v16(GroundVertex1, pointer.v[3], GroundVertex1);

            // Bottom right
            GrapLight(GroundX + 1, GroundY);
            glTexCoord2t16(GroundTextPosEnd[cx], GroundTextPosStart[cy]);
            glVertex3v16(GroundVertex1, pointer.v[1], GroundVertex2);

            // Bottom left
            GrapLight(GroundX, GroundY);
            glTexCoord2t16(GroundTextPosStart[cx], GroundTextPosStart[cy]);
            glVertex3v16(GroundVertex2, pointer.v[0], GroundVertex2);
        }

        glEnd();
    }

    if (mode == 2) {
        glBegin(GL_TRIANGLES);

        {
            // Bottom left
            GrapLight(GroundX, GroundY);
            glTexCoord2t16(GroundTextPosStart[cx], GroundTextPosStart[cy]);
            glVertex3v16(GroundVertex2, pointer.v[0], GroundVertex2);

            // Top left
            GrapLight(GroundX, GroundY + 1);
            glTexCoord2t16(GroundTextPosStart[cx], GroundTextPosEnd[cy]);
            glVertex3v16(GroundVertex2, pointer.v[2], GroundVertex1);

            // Bottom right
            GrapLight(GroundX + 1, GroundY);
            glTexCoord2t16(GroundTextPosEnd[cx], GroundTextPosStart[cy]);
            glVertex3v16(GroundVertex1, pointer.v[1], GroundVertex2);

            // ***********************

            // Top right
            GrapLight(GroundX + 1, GroundY + 1);
            glTexCoord2t16(GroundTextPosEnd[cx], GroundTextPosEnd[cy]);
            glVertex3v16(GroundVertex1, pointer.v[3], GroundVertex1);

            // Bottom right
            GrapLight(GroundX + 1, GroundY);
            glTexCoord2t16(GroundTextPosEnd[cx], GroundTextPosStart[cy]);
            glVertex3v16(GroundVertex1, pointer.v[1], GroundVertex2);

            // Top left
            GrapLight(GroundX, GroundY + 1);
            glTexCoord2t16(GroundTextPosStart[cx], GroundTextPosEnd[cy]);
            glVertex3v16(GroundVertex2, pointer.v[2], GroundVertex1);
        }

        glEnd();
    }

    glPopMatrix(1);
}
