#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Render/Autotiles.h"
#include "GfxEngine/Render/Boden.h"
#include "GfxEngine/Texture/Light.h"

int AutotileTextur[4][3];
u32 AutotileColorKeyMaster[4];
u32 AutotileIgnorecolors[10][4];
int AutotileIgnorecolorsNum[4];

// The way a Bodentexture should be loaded
void LoadAutotileTexture(char filename[], int num)
{
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

    int ax, bx, by;

    int palTextureName = -1;

    // lets load 16 first
    for (ax = 0; ax < 3; ax++) { // all 16 parts of 128x128 tile thing
        for (bx = 0; bx < 128; bx++) {
            for (by = 0; by < 128; by++) // each pixel
                part[bx + (by * 128)] = buffer8[(bx + (ax * 128)) + (by * 384)];
        }

        glGenTextures(1, &AutotileTextur[num][ax]);
        glBindTexture(0, AutotileTextur[num][ax]);

        WaitForFreeVblank();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 128, 128, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, part);

        if (palTextureName == -1) {
            // OK here we now have loaded pal and we got 36 fields a 32x32
            glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);
            palTextureName = AutotileTextur[num][ax];
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
    AutotileIgnorecolorsNum[Tilenum]++;
    AutotileIgnorecolors[AutotileIgnorecolorsNum[Tilenum]][Tilenum] = Col;
}

// The way a bodentexture should be rendered
void RenderAutoBoden(int x, int y, int z, int num)
{
    int cx = 0, cy = 0;
    v16x4 pointer = Terrain[BodenX][BodenY];

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
        GrapLight(BodenX, BodenY);
        glTexCoord2t16(BodenTextPosStart[cx], BodenTextPosStart[cy]);
        glVertex3v16(BodenVertex2, pointer.v[0], BodenVertex2);

        // Top left
        GrapLight(BodenX, BodenY + 1);
        glTexCoord2t16(BodenTextPosStart[cx], BodenTextPosEnd[cy]);
        glVertex3v16(BodenVertex2, pointer.v[2], BodenVertex1);

        // Top right
        GrapLight(BodenX + 1, BodenY + 1);
        glTexCoord2t16(BodenTextPosEnd[cx], BodenTextPosEnd[cy]);
        glVertex3v16(BodenVertex1, pointer.v[3], BodenVertex1);

        // Bottom right
        GrapLight(BodenX + 1, BodenY);
        glTexCoord2t16(BodenTextPosEnd[cx], BodenTextPosStart[cy]);
        glVertex3v16(BodenVertex1, pointer.v[1], BodenVertex2);
        glEnd();
    }

    if (mode == 1) {
        glBegin(GL_TRIANGLES);

        {
            // Bottom left
            GrapLight(BodenX, BodenY);
            glTexCoord2t16(BodenTextPosStart[cx], BodenTextPosStart[cy]);
            glVertex3v16(BodenVertex2, pointer.v[0], BodenVertex2);

            // Top left
            GrapLight(BodenX, BodenY + 1);
            glTexCoord2t16(BodenTextPosStart[cx], BodenTextPosEnd[cy]);
            glVertex3v16(BodenVertex2, pointer.v[2], BodenVertex1);

            // Top right
            GrapLight(BodenX + 1, BodenY + 1);
            glTexCoord2t16(BodenTextPosEnd[cx], BodenTextPosEnd[cy]);
            glVertex3v16(BodenVertex1, pointer.v[3], BodenVertex1);

            // ***********************

            // Top right
            GrapLight(BodenX + 1, BodenY + 1);
            glTexCoord2t16(BodenTextPosEnd[cx], BodenTextPosEnd[cy]);
            glVertex3v16(BodenVertex1, pointer.v[3], BodenVertex1);

            // Bottom right
            GrapLight(BodenX + 1, BodenY);
            glTexCoord2t16(BodenTextPosEnd[cx], BodenTextPosStart[cy]);
            glVertex3v16(BodenVertex1, pointer.v[1], BodenVertex2);

            // Bottom left
            GrapLight(BodenX, BodenY);
            glTexCoord2t16(BodenTextPosStart[cx], BodenTextPosStart[cy]);
            glVertex3v16(BodenVertex2, pointer.v[0], BodenVertex2);
        }

        glEnd();
    }

    if (mode == 2) {
        glBegin(GL_TRIANGLES);

        {
            // Bottom left
            GrapLight(BodenX, BodenY);
            glTexCoord2t16(BodenTextPosStart[cx], BodenTextPosStart[cy]);
            glVertex3v16(BodenVertex2, pointer.v[0], BodenVertex2);

            // Top left
            GrapLight(BodenX, BodenY + 1);
            glTexCoord2t16(BodenTextPosStart[cx], BodenTextPosEnd[cy]);
            glVertex3v16(BodenVertex2, pointer.v[2], BodenVertex1);

            // Bottom right
            GrapLight(BodenX + 1, BodenY);
            glTexCoord2t16(BodenTextPosEnd[cx], BodenTextPosStart[cy]);
            glVertex3v16(BodenVertex1, pointer.v[1], BodenVertex2);

            // ***********************

            // Top right
            GrapLight(BodenX + 1, BodenY + 1);
            glTexCoord2t16(BodenTextPosEnd[cx], BodenTextPosEnd[cy]);
            glVertex3v16(BodenVertex1, pointer.v[3], BodenVertex1);

            // Bottom right
            GrapLight(BodenX + 1, BodenY);
            glTexCoord2t16(BodenTextPosEnd[cx], BodenTextPosStart[cy]);
            glVertex3v16(BodenVertex1, pointer.v[1], BodenVertex2);

            // Top left
            GrapLight(BodenX, BodenY + 1);
            glTexCoord2t16(BodenTextPosStart[cx], BodenTextPosEnd[cy]);
            glVertex3v16(BodenVertex2, pointer.v[2], BodenVertex1);
        }

        glEnd();
    }

    glPopMatrix(1);
}
