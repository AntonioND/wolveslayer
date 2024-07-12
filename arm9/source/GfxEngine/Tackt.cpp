#include "3D.h"

int Tackt3er     = 0;
int QuarterTackt = 0;
float sTackt     = 0;
float sTackt2    = 0;
float sTackt3    = 0;
int stackt10     = 0;
int stackt11     = 0;
int tackt32;

void tackt(void)
{
    // extern int screenmode;
    extern bool LOADINGNOw;

    if (!LOADINGNOw) {
        RunTime(); // Recalcs lightcolors
        tackt32 += 1;
        if (tackt32 >= 32)
            tackt32 = 0;
        sTackt2 += .02;
        if (sTackt2 >= .98)
            sTackt2 = 0;
        sTackt3 += .1;
        if (sTackt3 >= 1)
            sTackt3 = 0;
        if (sTackt > 0)
            stackt10++;
        if (stackt10 > 9)
            stackt10 = 0;
        sTackt += .5;
        if (sTackt >= 1) {
            stackt11++;
            QuarterTackt++;
            sTackt = 0;
        }
        if (stackt11 > 10)
            stackt11 = 0;
        if (QuarterTackt > 3)
            QuarterTackt = 0; // Needed to animate things...
        Tackt3er++;
        if (Tackt3er == 3)
            Tackt3er = 0;
    }
}
