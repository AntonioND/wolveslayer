// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include <malloc.h>

#include <filesystem.h>

#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/Input/Circlestuff.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/Model/MD2Load.h"
#include "GfxEngine/Output/Textbox.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/Render/Render.h"
#include "GfxEngine/Texture/Light.h"
#include "GfxEngine/Ticks.h"
#include "Sound/Music.h"

#define ENABLE_SPLASH

void Splash(void)
{
    srand(0xDEADBEEF); // TODO: Replace by `srand(time(NULL))`?

    InitSound();
    StartSong("/wolveslayer/bgfx/title.mod");

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
    lcdSwap();

    REG_BLDCNT = BLEND_FADE_BLACK | BLEND_SRC_BG3 | BLEND_DST_BACKDROP;
    REG_BG3CNT = BG_BMP16_256x256;
    REG_BG3PA  = 1 << 8;
    REG_BG3PB  = 0;
    REG_BG3PC  = 0;
    REG_BG3PD  = 1 << 8;
    REG_BG3X   = 0;
    REG_BG3Y   = 0;

    videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);

    REG_BLDCNT_SUB = BLEND_FADE_BLACK | BLEND_SRC_BG3 | BLEND_DST_BACKDROP;
    REG_BG3CNT_SUB = BG_BMP16_256x256;
    REG_BG3PA_SUB  = 1 << 8;
    REG_BG3PB_SUB  = 0;
    REG_BG3PC_SUB  = 0;
    REG_BG3PD_SUB  = 1 << 8;
    REG_BG3X_SUB   = 0;
    REG_BG3Y_SUB   = 0;

#ifdef ENABLE_SPLASH
    REG_BLDY     = 15;
    REG_BLDY_SUB = 15;

    LoadBmptoBuffer("/wolveslayer/pic/scr1.bmp", BG_GFX);
    LoadBmptoBuffer("/wolveslayer/pic/scr2.bmp", BG_GFX_SUB);

    for (int u = 15; u >= 0; u--)
    {
        REG_BLDY     = u;
        REG_BLDY_SUB = u;
        swiWaitForVBlank();
    }
#endif // ENABLE_SPLASH

    InitCircles();

    LoadBmptoBuffer("/wolveslayer/pic/loading.bmp", scrL_bin);
    LoadBmptoBuffer("/wolveslayer/pic/touch.bmp", touch_bin);
    LoadBmptoBuffer8("/wolveslayer/pic/font1.bmp", Font1, textbox_pal);

    // LoadBmptoBuffer8("/rd/pic/box.bmp", textbox_bin, textbox_pal);

#ifdef ENABLE_SPLASH
    int pressed;
    for (int u = 0; u < 360; u++)
    {
        scanKeys();
        pressed = keysDown();
        swiWaitForVBlank();
        if ((pressed & KEY_TOUCH) || (pressed & KEY_START))
            u = 360;
    }

    for (int u = 15; u >= 0; u--)
    {
        REG_BLDY     = 15 - u;
        REG_BLDY_SUB = 15 - u;
        swiWaitForVBlank();
    }

    LoadBmptoBuffer("/wolveslayer/pic/scr3.bmp", BG_GFX);
    LoadBmptoBuffer("/wolveslayer/pic/scr4.bmp", BG_GFX_SUB);

    for (int u = 15; u >= 0; u--)
    {
        REG_BLDY     = u;
        REG_BLDY_SUB = u;
        swiWaitForVBlank();
    }

    scanKeys();
    pressed = keysDown();

    while (!((pressed & KEY_START) || (pressed & KEY_TOUCH)))
    {
        swiWaitForVBlank();
        scanKeys();
        pressed = keysDown();
    }
#endif // ENABLE_SPLASH
}

static int frameCounter  = 0;
static int loopCounter   = 0;
static int elapsedFrames = 0;
static int frameold      = 0;
static int polycount     = 0;

// static int capcount = 0;

static int frcapture = -1;

void DisplCapture(void)
{
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

void vBlank(void)
{
    ++elapsedFrames;
    ++frameCounter;

    if (frameCounter % 2 == 1)
    {
        // Recalcs lightcolors based on the time of the day
        RunTime();

        HandleTicks();
    }

    if (frameCounter >= 60)
    {
        frameold     = loopCounter;
        frameCounter = 0;
        loopCounter  = 0;
    }

    if (screenmode == ScreenModeTextBox)
    {
        // If the player holds A, print message 3 times faster
        if (frameCounter % 2 == 1 && (keysHeld() & KEY_A))
            TextBoxmodeHandler();
        if (frameCounter % 6 == 1 && !(keysHeld() & KEY_A))
            TextBoxmodeHandler();
    }

    if (screenmode != ScreenModeTextBox && frameCounter % 59 == 0 && (keysHeld() & KEY_R))
    {
        char Tmp[50];

#ifdef ShowHeapUsage
        extern void *fake_heap_end;
        extern void *fake_heap_start;
        size_t total_heap = (uintptr_t)fake_heap_end - (uintptr_t)fake_heap_start;

        struct mallinfo mi = mallinfo();
        sprintf(Tmp, "Heap: %zu/%zu KB", mi.uordblks / 1024, total_heap / 1024);
        Print(Tmp, 80, 85);
#endif

#ifdef ShowPosition
        int px = PlPosX;
        int py = PlPosY;

        sprintf(Tmp, "PX: %d  PY: %d  ", px + 6, py + 8);
        Print(Tmp, 80, 105);
#endif

        sprintf(Tmp, "Frames   : %d  ", frameold); // we print it all 15 vblanks so 4 times per second
        Print(Tmp, 80, 115);

#ifdef ShowPolyCount
        sprintf(Tmp, "Polys   : %d  ", polycount); // we print it all 15 vblanks so 4 times per second
        Print(Tmp, 80, 125);
#endif
    }

#if 0
    // If this code is enabled, fatInitDefault() needs to be called at init.

    if ((keysHeld() & KEY_L))
        DisplCapture();

    if (frcapture != -1 && frameCounter == frcapture) {
        frcapture = -1;
        vramSetBankC(VRAM_C_SUB_BG);
        capcount++;

        char filename[20];
        sprintf(filename, "fat:/%d.raw", capcount);
        FILE *f = fopen(filename, "w");
        // if (f == NULL) {
        //     return;
        // }
        fwrite(BG_GFX_SUB, 256 * 256 * 2, 1, f);
        fclose(f);
    }
#endif
}

void WaitForFreeVblank(void)
{
    // important for multitask buffering and loading to vram
    swiWaitForVBlank();
    if (frameCounter % 59 == 0)
        swiWaitForVBlank();
}

void E3D_Init(void)
{
    powerOn(POWER_ALL);

    if (!nitroFSInit(NULL))
        Crash("Failed to initialize NitroFS");

    Splash();

    // Set mode 0 and set it to 3D
    videoSetMode(MODE_0_3D);

    // Texture setup
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    lcdSwap();
    videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);

    REG_BG3CNT_SUB = BG_BMP16_256x256;
    REG_BG3PA_SUB  = 1 << 8;
    REG_BG3PB_SUB  = 0;
    REG_BG3PC_SUB  = 0;
    REG_BG3PD_SUB  = 1 << 8;
    REG_BG3X_SUB   = 0;
    REG_BG3Y_SUB   = 0;

    ScreenMode();

    // OpenGL init
    irqSet(IRQ_VBLANK, vBlank);
    glInit();

    glViewport(0, 0, 255, 191);
    glClearColor(0, 0, 0, 31); // BG must be opaque for AA to work
    glClearPolyID(63);         // BG must have a unique polygon ID for AA to work
    glClearDepth(0x7FFF);

    glEnable(GL_TEXTURE);
    glEnable(GL_ANTIALIAS);
    glEnable(GL_BLEND);

    InitTableOfNormal();
    swiWaitForVBlank();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 256.0 / 192.0, .1, 40);
}

void E3D_StartRender()
{
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    gluLookAt(CamPosSX, (PlHeight * 1.25) + 3, 3.4 + CamPosSY - (PlHeight * .28), // Camera possition
              CamPosSX, .4 + (PlHeight * .67), CamPosSY + .1 - (PlHeight * .28),  // Look at
              0, 1, 0);                                                           // Up

    // Move away from the camera
    glTranslate3f32(0, 0, floattov16(-0.1));
}

void E3D_Render()
{
    Refresh3D();
}

void E3D_EndRender()
{
    loopCounter++;
#ifdef ShowPolyCount
    if (loopCounter == 2)
        glGetInt(GL_GET_POLYGON_RAM_COUNT, &polycount);
#endif

    glFlush(GL_WBUFFERING);
}

__attribute__((noreturn)) void Crash(const char *msg, ...)
{
    StopSong();

    irqSet(IRQ_VBLANK, NULL);

    consoleDemoInit();

    printf("FATAL ERROR!\n");
    printf("\n");

    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    printf("\n");

    while (1)
        swiWaitForVBlank();
}
