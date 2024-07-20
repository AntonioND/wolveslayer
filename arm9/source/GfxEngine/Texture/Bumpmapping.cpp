#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/Render/Ground.h"

void LoadMBump3Texture(char filename[], int *target, int *targetb, int *targetc)
{
    if (target == NULL || targetb == NULL || targetc == NULL)
        Crash("Invalid arguments:\n%s", __func__);

    u8 *buffer8;
    u16 *pal;
    u32 height, width;
    LoadBmpAllocBuffer8(filename, &buffer8, &pal, &height, &width);

    if (width != 64 || height != 64)
        Crash("Texture isn't 64x64\nSize: %lux%lu\n%s", width, height, filename);

    u8 buffLeft[32 * 64];
    u8 buffRight[32 * 64];
    u8 buffNorm[32 * 64];

    // first we get the colors
    u16 red  = RGB15(255 >> 3, 0 >> 3, 0 >> 3) | BIT(15);
    u16 blue = RGB15(0 >> 3, 0 >> 3, 255 >> 3) | BIT(15);
    // u16 black = RGB15(0 >> 3, 0 >> 3, 0 >> 3) | BIT(15);

    // now we create 3 textures
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 64; y++) {
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

    glGenTextures(1, target);
    glGenTextures(1, targetb);
    glGenTextures(1, targetc);

    WaitForFreeVblank();

    glBindTexture(0, *target);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 32, 64, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffNorm);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    glBindTexture(0, *targetb);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 32, 64, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffLeft);
    glAssignColorTable(0, *target);

    glBindTexture(0, *targetc);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 32, 64, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffRight);
    glAssignColorTable(0, *target);

    free(buffer8);
    free(pal);
}

void LoadMBump5Texture(char filename[], int num)
{
    if (num >= Ground_Max)
        Crash("Ground index too big:\n%d > %d", num, Ground_Max);

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

    Ground[num].Size = height;

    glGenTextures(1, &(Ground[num].Texture));
    glGenTextures(1, &(Ground[num].TextureS));
    glGenTextures(1, &(Ground[num].TextureW));
    glGenTextures(1, &(Ground[num].TextureE));

    WaitForFreeVblank();

    glBindTexture(0, Ground[num].Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, height, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffNorm);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    glBindTexture(0, Ground[num].TextureS);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, height, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffDown);
    glAssignColorTable(0, Ground[num].Texture);

    glBindTexture(0, Ground[num].TextureW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, height, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffLeft);
    glAssignColorTable(0, Ground[num].Texture);

    glBindTexture(0, Ground[num].TextureE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, height, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffRight);
    glAssignColorTable(0, Ground[num].Texture);

    free(buffNorm);
    free(buffLeft);
    free(buffRight);
    free(buffDown);

    free(buffer8);
    free(pal);
}
