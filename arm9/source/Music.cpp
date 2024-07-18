#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "sound/Sound9.h"

// some sounds
#if 0
u8 *schwert_bin;
u32 schwert_bin_size = 0;
u8 *wolfaua_bin;
u32 wolfaua_bin_size = 0;
u8 *wolfstirbt_bin;
u32 wolfstirbt_bin_size = 0;
#endif

static void *ModBuffer   = NULL;
static char *ModFilename = NULL;

void InitSound()
{
    SndInit9();

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
    if (ModFilename != NULL)
        free(ModFilename);

    ModFilename = strdup(Name);
    if (ModFilename == NULL)
        Crash("No memory for ModFilename:\n%s", Name);

    // Stop song and wait for it to stop playing
    SndStopMOD();
    swiWaitForVBlank();

    // Free old buffer
    if (ModBuffer != NULL)
        free(ModBuffer);

    // Load new song and send the buffer to the ARM7. We need to flush t he
    // cache so that the ARM7 sees the data we have just loaded.
    size_t Modfilesize = 0;

    ModBuffer = LoadFile(Name, &Modfilesize);
    DC_FlushRange(ModBuffer, Modfilesize);

    // Start player again
    SndPlayMOD(ModBuffer);
}

void StopSong(void)
{
    SndStopMOD();
    swiWaitForVBlank();
}
