#include "GfxEngine/3D.h"
#include "GfxEngine/Enemys.h"
#include "GfxEngine/Events.h"
#include "GfxEngine/input/Input.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/output/Touchscreen.h"
#include "GfxEngine/render/Render.h"
#include "GfxEngine/Settings.h"
#include "GfxEngine/texture/DynamicLights.h"
#include "GfxEngine/Villagers.h"

int main(int argc, char *argv[])
{
    // consoleDebugInit(DebugDevice_NOCASH);

    // extern int EquipedWeapon;

    E3D_Init();

    // Script loading for new game
    ReadSettings();

    extern int loopCounter;
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
