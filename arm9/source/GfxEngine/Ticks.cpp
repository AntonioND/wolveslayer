static float sTicks = 0;

int ticks11 = 0;
int ticks32 = 0;

// Needed to animate things...
void HandleTicks(void)
{
    // Increase ticks32 every frame
    ticks32 += 1;
    if (ticks32 >= 32)
        ticks32 = 0;

    // Increase ticks11 every other frame
    sTicks += .5;
    if (sTicks >= 1)
    {
        ticks11++;
        sTicks = 0;
    }

    if (ticks11 > 10)
        ticks11 = 0;
}
