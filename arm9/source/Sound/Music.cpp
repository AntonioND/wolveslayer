// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include <libxm7.h>

#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "Sound/Music.h"

// some sounds
#if 0
u8 *schwert_bin;
u32 schwert_bin_size = 0;
u8 *wolfaua_bin;
u32 wolfaua_bin_size = 0;
u8 *wolfstirbt_bin;
u32 wolfstirbt_bin_size = 0;
#endif

bool ModIsPlaying = false;
static void *ModBuffer   = NULL;
static char *ModFilename = NULL;

// Assign FIFO_USER_07 channel to libxm7
#define FIFO_XM7 (FIFO_USER_07)

static XM7_ModuleManager_Type *ModInfo = NULL;

void InitSound()
{
#if 0
    FILE *f;
    f = fopen("/rd/sfx/schwert.bin", "rb");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        schwert_bin_size = ftell(f);
        fseek(f, 0, SEEK_SET);
        for (int a = 0; a < schwert_bin_size; a++)
            fread(&schwert_bin[a], 1, 1, f);
        fclose(f);
    }

    f = fopen("/rd/sfx/wolfaua.bin", "rb");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        wolfaua_bin_size = ftell(f);
        fseek(f, 0, SEEK_SET);
        for (int a = 0; a < wolfaua_bin_size; a++)
            fread(&wolfaua_bin[a], 1, 1, f);
        fclose(f);
    }

    f = fopen("/rd/sfx/wolfstirbt.bin", "rb");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        wolfstirbt_bin_size = ftell(f);
        fseek(f, 0, SEEK_SET);
        for (int a = 0; a < wolfstirbt_bin_size; a++)
            fread(&wolfstirbt_bin[a], 1, 1, f);
        fclose(f);
    }

    setGenericSound(11025, 127, 64, 1);
#endif
}

void StartSong(const char *Name)
{
    // Stop song before loading a new one
    if (ModIsPlaying)
        StopSong();

    ModFilename = strdup(Name);
    if (ModFilename == NULL)
        Crash("No memory for ModFilename:\n%s", Name);

    ModInfo = (XM7_ModuleManager_Type *)calloc(1, sizeof(XM7_ModuleManager_Type));
    if (ModInfo == NULL)
        Crash("No memory for MOD info:\n%s", Name);

    // Load new song and send the buffer to the ARM7
    size_t Modfilesize = 0;

    ModBuffer = LoadFile(Name, &Modfilesize);

    u16 res = XM7_LoadMOD(ModInfo, ModBuffer);
    if (res != 0)
        Crash("libxm7 error: 0x%04x\n", res);

    // The default replay style doesn't work well with the MOD files in the game
    XM7_SetReplayStyle(ModInfo, XM7_REPLAY_STYLE_MOD_PLAYER);

    // We need to flush the cache so that the ARM7 sees the updated data. This
    // needs to be done after we have finished reading and writing the
    // MOD-related buffers.
    DC_FlushAll();

    // Start player
    fifoSendValue32(FIFO_XM7, (u32)ModInfo);

    ModIsPlaying = true;
}

void StopSong(void)
{
    if (!ModIsPlaying)
        return;

    // Stop player
    fifoSendValue32(FIFO_XM7, 0);
    swiWaitForVBlank();
    swiWaitForVBlank();

    free(ModFilename);
    free(ModBuffer);

    XM7_UnloadMOD(ModInfo);
    free(ModInfo);

    ModIsPlaying = false;
}
