#ifndef GFXENGINE_FILES_H__
#define GFXENGINE_FILES_H__

#include <nds.h>

// Load any file
void *LoadFile(const char *filename, size_t *size);

// For getting a 8 bit BMP into a pre-allocated 16 bit buffer
void LoadBmptoBuffer(const char *filename, u16 *picbuff);

// For getting a 8 bit BMP into a pre-allocated 8 bit buffer and a 16 bit palette
void LoadBmptoBuffer8(const char *filename, u8 *picbuff, u16 *palbuff);

// It allocates a buffer for the image and the palette and loads a 8 bit BMP into it
void LoadBmpAllocBuffer8(const char *filename, u8 **picbuff_, u16 **palbuff_, u32 *height_, u32 *width_);

// It allocates a buffer and loads a 24 bit BMP into it
void LoadBmpAllocBuffer24(const char *filename, u8 **picbuff_, u32 *height_, u32 *width_);

#endif // GFXENGINE_FILES_H__
