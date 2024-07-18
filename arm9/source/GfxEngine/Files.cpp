#include <stdio.h>

#include <nds.h>

#include "GfxEngine/3D.h"

void *LoadFile(const char *filename, size_t *size)
{
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
        Crash("Can't open file:\n%s", filename);

    fseek(f, 0, SEEK_END);

    size_t size_ = ftell(f);
    if (size_ == 0)
        Crash("File is empty:\n%s", filename);

    rewind(f);

    u8 *buffer = (u8 *)malloc(size_);
    if (buffer == NULL)
        Crash("Not enough memory for file:\n%s", filename);

    if (fread(buffer, size_, 1, f) != 1)
        Crash("Can't read file:\n%s", filename);

    if (fclose(f) != 0)
        Crash("Can't close file:\n%s", filename);

    if (size)
        *size = size_;

    return buffer;
}

static u32 read32(u8 *p)
{
    return ((u32)p[3] << 24) | ((u32)p[2] << 16) | ((u32)p[1] << 8) | (u32)p[0];
}

void LoadBmptoBuffer(const char *filename, u16 *picbuff)
{
    u8 *bmp_buffer = (u8 *)LoadFile(filename, NULL);

    u8 *bmp = bmp_buffer + 18;

    u32 width = read32(bmp);
    bmp += 4;
    u32 height = read32(bmp);
    bmp += 4;
    u32 colorCoding = read32(bmp);

    if (((colorCoding & 0xFFFF0000) >> 16) == 8) {
        u16 pal[256];

        // First read the palette
        bmp = bmp_buffer + 54;

        for (int i = 0; i < 256; i++) {
            u32 color = read32(bmp);
            bmp += 4;

            u32 b = color & 0xFF;
            u32 g = (color >> 8) & 0xFF;
            u32 r = (color >> 16) & 0xFF;

            pal[i] = RGB15(r >> 3, g >> 3, b >> 3) | BIT(15);
        }

        for (u32 y = 0; y < height; y++) {
            for (u32 x = 0; x < width; x++) {
                u8 color = *bmp++;

                picbuff[x + (((height - 1) - y) * width)] = pal[color];
            }
            if (width & 1)
                bmp += 4 - (width & 1);
        }
    } else {
        Crash("Invalid format for file:\n%s", filename);
    }

    free(bmp_buffer);
}

void LoadBmptoBuffer8(const char *filename, u8 *picbuff, u16 *palbuff)
{
    u8 *bmp_buffer = (u8 *)LoadFile(filename, NULL);

    u8 *bmp = bmp_buffer + 18;

    u32 width = read32(bmp);
    bmp += 4;
    u32 height = read32(bmp);
    bmp += 4;
    u32 colorCoding = read32(bmp);

    if (((colorCoding & 0xFFFF0000) >> 16) == 8) {
        // First read the palette
        bmp = bmp_buffer + 54;

        for (int i = 0; i < 256; i++) {
            u32 color = read32(bmp);
            bmp += 4;

            u32 b = color & 0xFF;
            u32 g = (color >> 8) & 0xFF;
            u32 r = (color >> 16) & 0xFF;

            palbuff[i] = RGB15(r >> 3, g >> 3, b >> 3) | BIT(15);
        }

        for (u32 y = 0; y < height; y++) {
            for (u32 x = 0; x < width; x++) {
                u8 color = *bmp++;

                picbuff[x + (((height - 1) - y) * width)] = color;
            }
            if (width & 1)
                bmp += 4 - (width & 1);
        }
    } else {
        Crash("Invalid format for file:\n%s", filename);
    }

    free(bmp_buffer);
}

void LoadBmpAllocBuffer8(const char *filename, u8 **picbuff_, u16 **palbuff_, u32 *height_, u32 *width_)
{
    sassert(picbuff_ != NULL && palbuff_ != NULL && height_ != NULL && width_ != NULL, "Invalid argument");

    u8 *bmp_buffer = (u8 *)LoadFile(filename, NULL);

    u8 *bmp = bmp_buffer + 18;

    u32 width = read32(bmp);
    bmp += 4;
    u32 height = read32(bmp);
    bmp += 4;
    u32 colorCoding = read32(bmp);

    *width_  = width;
    *height_ = height;

    if (((colorCoding & 0xFFFF0000) >> 16) == 8) {
        u8 *picbuff  = (u8 *)malloc(width * height);
        u16 *palbuff = (u16 *)malloc(256 * 2);

        if (picbuff == NULL || palbuff == NULL)
            Crash("Not enough memory for file:\n%s", filename);

        *picbuff_ = picbuff;
        *palbuff_ = palbuff;

        // First read the palette
        bmp = bmp_buffer + 54;

        for (int i = 0; i < 256; i++) {
            u32 color = read32(bmp);
            bmp += 4;

            u32 b = color & 0xFF;
            u32 g = (color >> 8) & 0xFF;
            u32 r = (color >> 16) & 0xFF;

            palbuff[i] = RGB15(r >> 3, g >> 3, b >> 3) | BIT(15);
        }

        for (u32 y = 0; y < height; y++) {
            for (u32 x = 0; x < width; x++) {
                u8 color = *bmp++;

                picbuff[x + (((height - 1) - y) * width)] = color;
            }
            if (width & 1)
                bmp += 4 - (width & 1);
        }
    } else {
        Crash("Invalid format for file:\n%s", filename);
    }

    free(bmp_buffer);
}

void LoadBmpAllocBuffer24(const char *filename, u8 **picbuff_, u32 *height_, u32 *width_)
{
    sassert(picbuff_ != NULL && height_ != NULL && width_ != NULL, "Invalid argument");

    u8 *bmp_buffer = (u8 *)LoadFile(filename, NULL);

    u8 *bmp = bmp_buffer + 18;

    u32 width = read32(bmp);
    bmp += 4;
    u32 height = read32(bmp);
    bmp += 4;
    u32 colorCoding = read32(bmp);

    *width_  = width;
    *height_ = height;

    if (((colorCoding & 0xFFFF0000) >> 16) == 24) {
        u8 *picbuff = (u8 *)malloc(width * height * 3);

        if (picbuff == NULL)
            Crash("Not enough memory for file:\n%s", filename);

        *picbuff_ = picbuff;

        bmp = bmp_buffer + 54;

        u8 *pic = picbuff;
        for (u32 y = 0; y < height; y++) {
            for (u32 x = 0; x < width; x++) {
                u8 b = *bmp++;
                u8 g = *bmp++;
                u8 r = *bmp++;

                *pic++ = b;
                *pic++ = g;
                *pic++ = r;
            }
            if ((width * 3) & 3)
                bmp += 4 - ((width * 3) & 3);
        }
    } else {
        Crash("Invalid format for file:\n%s", filename);
    }

    free(bmp_buffer);
}
