#include "../sound/Sound9.h"
#include "3D.h"

// some sounds
#if 0
u8 *schwert_bin;
u32 schwert_bin_size = 0;
u8 *wolfaua_bin;
u32 wolfaua_bin_size = 0;
u8 *wolfstirbt_bin;
u32 wolfstirbt_bin_size = 0;
#endif

void *ModfileA     = NULL;
size_t Modfilesize = 0;
char ModFilename[60];

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
    if (strncmp(Name, ModFilename, 60) == 0)
        return;

    snprintf(ModFilename, sizeof(ModFilename), "%s", Name);

    // Stop song and wait for it to stop playing
    SndStopMOD();
    swiWaitForVBlank();

    // Free old buffer
    free(ModfileA);

    // Load new song and send the buffer to the ARM7. We need to flush t he
    // cache so that the ARM7 sees the data we have just loaded.
    ModfileA = LoadFile(Name, &Modfilesize);
    DC_FlushRange(ModfileA, Modfilesize);
    SndSetMemPool(ModfileA, Modfilesize);

    // Start player again
    SndPlayMOD(ModfileA);
}

void Playhandler(void)
{
}
