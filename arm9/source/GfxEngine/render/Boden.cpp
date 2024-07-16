#include "../3D.h"

typedef struct {
    v16 v[4];
    u8 sidewalls;
} v16x4;

int BodenX, BodenY;

void SetCurBod(int x, int y)
{
    BodenX = x;
    BodenY = y;
}

// The way a Bodentexture should be loaded
void LoadBodenTexture(char filename[], int num)
{
    extern int BodenTexture[Ground_Count];
    extern u8 BodenSize[Ground_Count];

    u8 *buffer8;
    u16 *pal;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &buffer8, &pal, &height, &width);

    if (width != height || (width != 32 && width != 64))
        Crash("Texture isn't 32x32 or 64x64\nSize: %lux%lu\n%s", width, height, filename);

    glGenTextures(1, &BodenTexture[num]);
    glBindTexture(0, BodenTexture[num]);

    WaitForFreeVblank();

    BodenSize[num] = width;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, width, width, 0, TEXGEN_TEXCOORD, buffer8);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    free(buffer8);
    free(pal);
}

v16 vmini = floattov16(0.025f);

// The way a bodentexture should be rendered (floor inside and outside buildings).
void RenderBoden(int x, int y, int z, int textnum)
{
    extern int BodenTexture[Ground_Count];
    // extern int BodenTextureA[Ground_Count];
    // extern int BodenTextureN[Ground_Count];
    extern int BodenTextureS[Ground_Count];
    extern int BodenTextureW[Ground_Count];
    extern int BodenTextureE[Ground_Count];
    extern bool BodenBumpEnable[Ground_Count];
    extern t16 BodenTextPosStart[4];
    extern t16 BodenTextPosEnd[4];
    extern u8 BodenSize[Ground_Count];
    extern v16 BodenVertex1;
    extern v16 BodenVertex2;
    extern v16x4 Terrain[128][128];
    extern bool BodenTransEnable[Ground_Count];
    extern signed char TexBod[128][128];

    extern bool ViewportMapBumpGroundS[20][20];
    extern bool ViewportMapBumpGroundW[20][20];
    extern bool ViewportMapBumpGroundE[20][20];

    extern int CamPosX, CamPosY;

    v16x4 pointer = Terrain[BodenX][BodenY];

    int mode = (pointer.sidewalls >> 6) & 3;
    int a;
    int bodx, body;

    t16 start = BodenTextPosStart[0];
    t16 end;
    if (BodenSize[textnum] == 64)
        end = BodenTextPosEnd[1];
    else
        end = BodenTextPosEnd[0];

    if (BodenBumpEnable[textnum] == true)
        a = 3;
    else
        a = 0;

    glPushMatrix();
    glTranslatef32(inttof32(x), inttof32(0), inttof32(z));

    for (int b = 0; b <= a; b++) {
        v16 v1, v2, v3, v4;
        bool doit = false;

        if (b == 0) {
            glBindTexture(GL_TEXTURE_2D, BodenTexture[textnum]);
            bodx = BodenX;
            body = BodenY;
            v1   = pointer.v[0];
            v2   = pointer.v[1];
            v3   = pointer.v[2];
            v4   = pointer.v[3];
            doit = true;
            if (BodenTransEnable[TexBod[BodenX][BodenY]] == false)
                glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
            if (BodenTransEnable[TexBod[BodenX][BodenY]] == true)
                glPolyFmt(POLY_ALPHA(15) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
        }

        if (b == 1) {
            v1 += vmini;
            v2 += vmini;
            v3 += vmini;
            v4 += vmini;
        }

        if (b == 1 && ViewportMapBumpGroundS[BodenX - CamPosX + 5][BodenY - CamPosY + 3]) {
            glBindTexture(GL_TEXTURE_2D, BodenTextureS[textnum]);
            bodx = BodenX;
            body = BodenY + 1;
            doit = true;
        }

        if (b == 2 && ViewportMapBumpGroundW[BodenX - CamPosX + 5][BodenY - CamPosY + 3]) {
            glBindTexture(GL_TEXTURE_2D, BodenTextureW[textnum]);
            bodx = BodenX - 1;
            body = BodenY;
            doit = true;
        }

        if (b == 3 && ViewportMapBumpGroundE[BodenX - CamPosX + 5][BodenY - CamPosY + 3]) {
            glBindTexture(GL_TEXTURE_2D, BodenTextureE[textnum]);
            bodx = BodenX + 1;
            body = BodenY;
            doit = true;
        }

        if (mode == 0 && doit) {
            glBegin(GL_QUADS);

            {
                // Bottom left
                GrapLight(bodx, body);
                glTexCoord2t16(start, start);
                glVertex3v16(BodenVertex2, v1, BodenVertex2);

                // Top left
                GrapLight(bodx, body + 1);
                glTexCoord2t16(start, end);
                glVertex3v16(BodenVertex2, v3, BodenVertex1);

                // Top right
                GrapLight(bodx + 1, body + 1);
                glTexCoord2t16(end, end);
                glVertex3v16(BodenVertex1, v4, BodenVertex1);

                // Bottom right
                GrapLight(bodx + 1, body);
                glTexCoord2t16(end, start);
                glVertex3v16(BodenVertex1, v2, BodenVertex2);
            }

            glEnd();
        }

        if (mode == 1 && doit) {
            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(bodx, body);
                glTexCoord2t16(start, start);
                glVertex3v16(BodenVertex2, v1, BodenVertex2);

                // Top left
                GrapLight(bodx, body + 1);
                glTexCoord2t16(start, end);
                glVertex3v16(BodenVertex2, v3, BodenVertex1);

                // Top right
                GrapLight(bodx + 1, body + 1);
                glTexCoord2t16(end, end);
                glVertex3v16(BodenVertex1, v4, BodenVertex1);
                //***********************
                // Top right
                GrapLight(bodx + 1, body + 1);
                glTexCoord2t16(end, end);
                glVertex3v16(BodenVertex1, v4, BodenVertex1);

                // Bottom right
                GrapLight(bodx + 1, body);
                glTexCoord2t16(end, start);
                glVertex3v16(BodenVertex1, v2, BodenVertex2);

                // Bottom left
                GrapLight(bodx, body);
                glTexCoord2t16(start, start);
                glVertex3v16(BodenVertex2, v1, BodenVertex2);
            }

            glEnd();
        }

        if (mode == 2 && doit) {
            glBegin(GL_TRIANGLES);

            {
                // Bottom left
                GrapLight(bodx, body);
                glTexCoord2t16(start, start);
                glVertex3v16(BodenVertex2, v1, BodenVertex2);

                // Top left
                GrapLight(bodx, body + 1);
                glTexCoord2t16(start, end);
                glVertex3v16(BodenVertex2, v3, BodenVertex1);

                // Bottom right
                GrapLight(bodx + 1, body);
                glTexCoord2t16(end, start);
                glVertex3v16(BodenVertex1, v2, BodenVertex2);
                //***********************
                // Top right
                GrapLight(bodx + 1, body + 1);
                glTexCoord2t16(end, end);
                glVertex3v16(BodenVertex1, v4, BodenVertex1);

                // Bottom right
                GrapLight(bodx + 1, body);
                glTexCoord2t16(end, start);
                glVertex3v16(BodenVertex1, v2, BodenVertex2);

                // Top left
                GrapLight(bodx, body + 1);
                glTexCoord2t16(start, end);
                glVertex3v16(BodenVertex2, v3, BodenVertex1);
            }

            glEnd();
        }
    }
    glPopMatrix(1);
}

// The way a levelborder should be rendered
void RenderLevelBorderBoden(int x, int y, int z)
{
    t16 cx = 0, cy = 0;

    v16 BodenVert1 = floattov16(-.5);
    v16 BodenVert2 = floattov16(.5);

    glPushMatrix();
    glTranslatef32(inttof32(x), inttof32(0), inttof32(z));

    GFX_COLOR = 0;

    glBegin(GL_QUADS);

    {
        // Bottom left
        glTexCoord2t16(cy, cx);
        glVertex3v16(BodenVert2, GetTerrain(BodenX, BodenY, 0), BodenVert2);

        // Top left
        glTexCoord2t16(cy, cx);
        glVertex3v16(BodenVert2, GetTerrain(BodenX, BodenY, 2), BodenVert1);

        // Top right
        glTexCoord2t16(cy, cx);
        glVertex3v16(BodenVert1, GetTerrain(BodenX, BodenY, 3), BodenVert1);

        // Bottom right
        glTexCoord2t16(cy, cx);
        glVertex3v16(BodenVert1, GetTerrain(BodenX, BodenY, 1), BodenVert2);
    }

    glEnd();
    glPopMatrix(1);
}
