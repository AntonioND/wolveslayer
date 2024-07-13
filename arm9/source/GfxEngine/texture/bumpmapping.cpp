#include "3D.h"

void LoadMBump3Texture(char filename[], int num, int *target, int *targetb, int *targetc)
{
    u8 *buffer8;
    u16 *pal;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &buffer8, &pal, &height, &width);

    if (width != 64 || height != 64)
        Crash("Texture isn't 64x64\nSize: %lux%lu\n%s", width, height, filename);

    u8 buffLeft[32 * 64];
    u8 buffRight[32 * 64];
    u8 buffNorm[32 * 64];

    int x, y;
    // first we get the colors
    u16 red  = RGB15(255 >> 3, 0 >> 3, 0 >> 3) | BIT(15);
    u16 blue = RGB15(0 >> 3, 0 >> 3, 255 >> 3) | BIT(15);
    // u16 black = RGB15(0 >> 3, 0 >> 3, 0 >> 3) | BIT(15);

    // now we create 3 textures
    for (x = 0; x < 32; x++) {
        for (y = 0; y < 64; y++) {
            buffNorm[x + (y * 32)]  = buffer8[(x) + (y * 64)];
            buffLeft[x + (y * 32)]  = 0;
            buffRight[x + (y * 32)] = 0;

            if (pal[buffer8[(x + 32) + (y * 64)]] == red) {
                buffLeft[x + (y * 32)]  = buffer8[(x) + (y * 64)];
                buffRight[x + (y * 32)] = 0;
            }

            if (pal[buffer8[(x + 32) + (y * 64)]] == blue) {
                buffLeft[x + (y * 32)]  = 0;
                buffRight[x + (y * 32)] = buffer8[(x) + (y * 64)];
            }
        }
    }

    glGenTextures(1, &target[num]);
    glGenTextures(1, &targetb[num]);
    glGenTextures(1, &targetc[num]);

    WaitForFreeVblank();

    glBindTexture(0, target[num]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 32, 64, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffNorm);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    glBindTexture(0, targetb[num]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 32, 64, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffLeft);
    glAssignColorTable(0, target[num]);

    glBindTexture(0, targetc[num]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 32, 64, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffRight);
    glAssignColorTable(0, target[num]);

    free(buffer8);
    free(pal);
}

void LoadMBump5Texture(char filename[], int num)
{
    extern int BodenTexture[Ground_Count];
    extern int BodenTextureS[Ground_Count];
    extern int BodenTextureW[Ground_Count];
    extern int BodenTextureE[Ground_Count];
    extern u8 BodenSize[Ground_Count];

    u8 *buffer8;
    u16 *pal;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &buffer8, &pal, &height, &width);

    if (width != height * 2 || (height != 32 && height != 64))
        Crash("Texture isn't 64x32 or 128x64\nSize: %lux%lu\n%s", width, height, filename);

    u8 *buffNorm  = (u8 *)malloc(height * height);
    u8 *buffLeft  = (u8 *)malloc(height * height);
    u8 *buffRight = (u8 *)malloc(height * height);
    u8 *buffDown  = (u8 *)malloc(height * height);

    if (buffNorm == NULL || buffLeft == NULL || buffRight == NULL || buffDown == NULL)
        Crash("Not enough memory for buffers:\n%s", filename);

    // first we get the colors
    u16 red   = RGB15(255 >> 3, 0 >> 3, 0 >> 3) | BIT(15);
    u16 blue  = RGB15(0 >> 3, 0 >> 3, 255 >> 3) | BIT(15);
    u16 green = RGB15(0 >> 3, 255 >> 3, 0 >> 3) | BIT(15);

    // now we create 5 textures
    for (u32 x = 0; x < height; x++) {
        for (u32 y = 0; y < height; y++) {
            buffNorm[x + (y * height)]  = buffer8[(x) + (y * width)];
            buffLeft[x + (y * height)]  = 0;
            buffRight[x + (y * height)] = 0;
            buffDown[x + (y * height)]  = 0;

            if (pal[buffer8[(x + height) + (y * width)]] == red) {
                buffRight[x + (y * height)] = buffer8[(x) + (y * width)];
            }

            if (pal[buffer8[(x + height) + (y * width)]] == blue) {
                buffLeft[x + (y * height)] = buffer8[(x) + (y * width)];
            }

            if (pal[buffer8[(x + height) + (y * width)]] == green) {
                buffDown[x + (y * height)] = buffer8[(x) + (y * width)];
            }
        }
    }

    BodenSize[num] = height;

    glGenTextures(1, &BodenTexture[num]);
    glGenTextures(1, &BodenTextureS[num]);
    glGenTextures(1, &BodenTextureW[num]);
    glGenTextures(1, &BodenTextureE[num]);

    WaitForFreeVblank();

    glBindTexture(0, BodenTexture[num]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, height, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffNorm);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    glBindTexture(0, BodenTextureS[num]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, height, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffDown);
    glAssignColorTable(0, BodenTexture[num]);

    glBindTexture(0, BodenTextureW[num]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, height, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffLeft);
    glAssignColorTable(0, BodenTexture[num]);

    glBindTexture(0, BodenTextureE[num]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, height, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffRight);
    glAssignColorTable(0, BodenTexture[num]);

    free(buffNorm);
    free(buffLeft);
    free(buffRight);
    free(buffDown);

    free(buffer8);
    free(pal);
}
