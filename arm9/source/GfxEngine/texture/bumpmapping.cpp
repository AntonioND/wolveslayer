#include "3D.h"

void LoadMBump3Texture(char filename[], int num, int *target, int *targetb, int *targetc)
{
    u8 *buffer8 = NULL;
    u8 *buffer8line = NULL;

    u16 pal[256];
    int width  = 0;
    int height = 0;

    FILE *bmp = fopen(filename, "r");
    if (bmp == NULL) {
        PrintOUT(filename, 5, 5, false, strlen(filename));
        PrintOUT("wasnt found", 5, 15, false, strlen("wasnt found"));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    fseek(bmp, 18, SEEK_SET);

    fread(&width, 4, 1, bmp);
    fread(&height, 4, 1, bmp);

    if (width != 64 || height != 64) {
        char bug[40];
        PrintOUT(filename, 5, 5, false, strlen(filename));
        PrintOUT("Texture isnt 64x64", 5, 15, false, strlen("Texture isnt 64x64"));
        sprintf(bug, "its %dx%d", width, height);
        PrintOUT(bug, 5, 25, false, strlen(bug));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    unsigned long colorCoding;
    fread(&colorCoding, 4, 1, bmp);

    if (((colorCoding & 0xFFFF0000) >> 16) != 8) {
        char bug[40];
        PrintOUT(filename, 5, 5, false, strlen(filename));
        PrintOUT("Texture isnt 8bit...", 5, 15, false, strlen("Texture isnt 8bit..."));
        sprintf(bug, "its %dBit", int((colorCoding & 0xFFFF0000) >> 16));
        PrintOUT(bug, 5, 25, false, strlen(bug));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    fseek(bmp, 34, SEEK_SET);
    unsigned long dataLength;
    fread(&dataLength, 4, 1, bmp);

    fseek(bmp, 46, SEEK_SET);
    unsigned long impcol;
    fread(&impcol, 4, 1, bmp);

    int i, q;

    switch ((colorCoding & 0xFFFF0000) >> 16) {
        case 8:
            // First read the pal
            fseek(bmp, 54, SEEK_SET);
            for (i = 0; i < 256; i++) {
                unsigned char r, g, b;
                unsigned long color;
                fread(&color, 4, 1, bmp);

                b = (color & 0x0FF);
                g = ((color >> 8) & 0x0FF);
                r = ((color >> 16) & 0x0FF);

                pal[i] = RGB15(r >> 3, g >> 3, b >> 3) | BIT(15);
            }
            buffer8 = (u8 *)malloc(width * height);
            buffer8line = (u8 *)malloc(width);

            for (i = 0; i < height; i++) {
                fread(buffer8line, 1, width, bmp);

                for (q = 0; q < width; q++) {
                    u8 color = buffer8line[q];
                    buffer8[q + (((height - 1) - i) * width)] = color;
                }
            }
            free(buffer8line);
            break;
        default:
            fprintf(stderr, "Invalid BMP format: %s", filename);
            while (1)
                ;
            break;
    };

    fclose(bmp);

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

    free(buffer8);

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
}

void LoadMBump5Texture(char filename[], int num)
{
    extern int BodenTexture[Ground_Count];
    extern int BodenTextureS[Ground_Count];
    extern int BodenTextureW[Ground_Count];
    extern int BodenTextureE[Ground_Count];
    extern u8 BodenSize[Ground_Count];

    u8 *buffer8 = NULL;
    u8 *buffer8line = NULL;

    u16 pal[256];
    int width  = 0;
    int height = 0;

    FILE *bmp = fopen(filename, "r");
    if (bmp == NULL) {
        PrintOUT(filename, 5, 5, false, strlen(filename));
        PrintOUT("wasnt found", 5, 15, false, strlen("wasnt found"));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    fseek(bmp, 18, SEEK_SET);

    fread(&width, 4, 1, bmp);
    fread(&height, 4, 1, bmp);

    if (width != height * 2 || (height != 32 && height != 64)) {
        char bug[40];
        PrintOUT(filename, 5, 5, false, strlen(filename));
        PrintOUT("Texture isnt 64x32 or 128x64", 5, 15, false, strlen("Texture isnt 64x32 or 128x64"));
        sprintf(bug, "its %dx%d", width, height);
        PrintOUT(bug, 5, 25, false, strlen(bug));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    unsigned long colorCoding;
    fread(&colorCoding, 4, 1, bmp);

    if (((colorCoding & 0xFFFF0000) >> 16) != 8) {
        char bug[40];
        PrintOUT(filename, 5, 5, false, strlen(filename));
        PrintOUT("Texture isnt 8bit...", 5, 15, false, strlen("Texture isnt 8bit..."));
        sprintf(bug, "its %dBit", int((colorCoding & 0xFFFF0000) >> 16));
        PrintOUT(bug, 5, 25, false, strlen(bug));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    fseek(bmp, 34, SEEK_SET);
    unsigned long dataLength;
    fread(&dataLength, 4, 1, bmp);

    fseek(bmp, 46, SEEK_SET);
    unsigned long impcol;
    fread(&impcol, 4, 1, bmp);

    int i, q;

    switch ((colorCoding & 0xFFFF0000) >> 16) {
        case 8:
            // First read the pal
            fseek(bmp, 54, SEEK_SET);
            for (i = 0; i < 256; i++) {
                unsigned char r, g, b;
                unsigned long color;
                fread(&color, 4, 1, bmp);

                b = (color & 0x0FF);
                g = ((color >> 8) & 0x0FF);
                r = ((color >> 16) & 0x0FF);

                pal[i] = RGB15(r >> 3, g >> 3, b >> 3) | BIT(15);
            }
            buffer8 = (u8 *)malloc(width * height);
            buffer8line = (u8 *)malloc(width);

            for (i = 0; i < height; i++) {
                fread(buffer8line, 1, width, bmp);

                for (q = 0; q < width; q++) {
                    u8 color = buffer8line[q];
                    buffer8[q + (((height - 1) - i) * width)] = color;
                }
            }
            free(buffer8line);
            break;
        default:
            fprintf(stderr, "Invalid BMP format: %s", filename);
            while (1)
                ;
            break;
    };

    fclose(bmp);

    u8 *buffNorm  = (u8 *)malloc(height * height);
    u8 *buffLeft  = (u8 *)malloc(height * height);
    u8 *buffRight = (u8 *)malloc(height * height);
    u8 *buffDown  = (u8 *)malloc(height * height);

    int x, y;
    // first we get the colors
    u16 red   = RGB15(255 >> 3, 0 >> 3, 0 >> 3) | BIT(15);
    u16 blue  = RGB15(0 >> 3, 0 >> 3, 255 >> 3) | BIT(15);
    u16 green = RGB15(0 >> 3, 255 >> 3, 0 >> 3) | BIT(15);

    // now we create 5 textures
    for (x = 0; x < height; x++) {
        for (y = 0; y < height; y++) {
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

    free(buffer8);

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
}
