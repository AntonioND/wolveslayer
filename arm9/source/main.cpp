#include "GfxEngine/3D.h"

int frcapture = -1;

void DisplCapture(void)
{
    extern int frameCounter;
    vramSetBankC(VRAM_C_LCD);
    uint8 srcBlend  = 0;
    uint8 destBlend = 15;
    uint8 bank      = 2;
    uint8 offset    = 0;
    uint8 size      = 3;
    uint8 source    = 0;
    uint8 srcOffset = 0;

    uint32 value = 0;
    value |= 1 << 31;                 // 31 is enable
    value |= 3 << 29;                 // 29-30 seems to have something to do with the blending
    value |= (srcOffset & 0x3) << 26; // capture source offset is 26-27
    value |= (source & 0x3) << 24;    // capture source is 24-25
    value |= (size & 0x3) << 20;      // capture data write size is 20-21
    value |= (offset & 0x3) << 18;    // write offset is 18-19
    value |= (bank & 0x3) << 16;      // vram bank select is 16-17
    value |= (srcBlend & 0xF) << 8;   // graphics blend evb is 8..12
    value |= (destBlend & 0xF) << 0;  // ram blend EVA is bits 0..4

    REG_DISPCAPCNT = value;

    frcapture = frameCounter + 3;
    if (frcapture > 60)
        frcapture -= 60;
}

int main(int argc, char *argv[])
{
    // consoleDebugInit(DebugDevice_NOCASH);

    // extern int EquipedWeapon;

    E3D_Init();

    // Script loading for new game
    ReadSettings();

    extern int screenmode;
    extern int loopCounter;
    // extern int frameold;
    extern int polycount;

    while (1) {
        scanKeys();
        CheckEndGame();
        ScreenModeHandler(); // For screenshots
        if (screenmode < 2)
            inputs();
        if (screenmode > 2)
            Menu();

        UpdateDorfis();
        UpdateEnemy();
        UpdateDynamic();

        MapDoorHandle();

        E3D_StartRender(); // Start render (if select is hold the perspective is different)

        // rendering
        Refresh3D();

        loopCounter++;
#ifdef ShowPolyCount
        if (loopCounter == 2)
            glGetInt(GL_GET_POLYGON_RAM_COUNT, &polycount);
#endif

        glFlush(GL_WBUFFERING); // Stop render
        // swiWaitForVBlank();
    }
    return 0;
}
