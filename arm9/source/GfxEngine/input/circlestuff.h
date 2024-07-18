#ifndef GFXENGINE_INPUT_CIRCLESTUFF_H__
#define GFXENGINE_INPUT_CIRCLESTUFF_H__

#include <stdbool.h>

// This is necessary for a nice hit detection
void InitCircles(void);

// TODO: This is unused
void DrawCircle(int num);

// A circle based hit detection. You give it pos, rad and pointer and it tells where it cant go to :D
void Checkcolision(int x, int y, int sx, int sy, int rad, bool *values);
void CheckcolisionDiagonal(int x, int y, int sx, int sy, int rad, bool *values);

#endif // GFXENGINE_INPUT_CIRCLESTUFF_H__
