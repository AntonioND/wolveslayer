#include "GfxEngine/3D.h"
#include "GfxEngine/Script/Script_Load.h"
#include "GfxEngine/Texture/Light.h"

static float sTackt = 0;

int stackt11 = 0;
int tackt32  = 0;

// Needed to animate things...
void tackt(void)
{
    if (!LoadingNow) {
        RunTime(); // Recalcs lightcolors

        tackt32 += 1;
        if (tackt32 >= 32)
            tackt32 = 0;

        sTackt += .5;
        if (sTackt >= 1) {
            stackt11++;
            sTackt = 0;
        }

        if (stackt11 > 10)
            stackt11 = 0;
    }
}
