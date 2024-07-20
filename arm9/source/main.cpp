#include "GfxEngine/3D.h"
#include "GfxEngine/Enemies.h"
#include "GfxEngine/Events.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/Settings.h"
#include "GfxEngine/Texture/DynamicLights.h"
#include "GfxEngine/Villagers.h"

int main(int argc, char *argv[])
{
    // consoleDebugInit(DebugDevice_NOCASH);

    E3D_Init();

    // Script loading for new game
    ReadSettings();

    while (1) {
        scanKeys();

        CheckEndGame();

        ScreenModeHandler(); // For screenshots
        if (screenmode < ScreenModeTextBox)
            inputs();
        if (screenmode > ScreenModeTextBox)
            Menu();

        UpdateVillagers();
        UpdateEnemy();
        UpdateDynamic();

        MapDoorHandle();

        E3D_StartRender();
        E3D_Render();
        E3D_EndRender();

        // TODO: If we enable this, the CPU usage of the ARM9 drops a lot.
        // However, in areas where the framerate drops below 60 FPS it is forced
        // to go to 30 FPS. Without this wait the framerate will only drop as
        // much as it is needed, not directly to 30 FPS.
        // swiWaitForVBlank();
    }
    return 0;
}
