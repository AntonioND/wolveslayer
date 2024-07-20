#ifndef GFXENGINE_TICKS_H__
#define GFXENGINE_TICKS_H__

// Metronomes
extern int ticks11; // This counts from 0 to 10 (every other frame)
extern int ticks32; // This counts from 0 to 32 (every frame)

// Needed to animate things...
void HandleTicks(void);

#endif // GFXENGINE_TICKS_H__
