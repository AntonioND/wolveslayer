#ifndef GFXENGINE_MODEL_MD2COLLISION_H__
#define GFXENGINE_MODEL_MD2COLLISION_H__

#include <stdbool.h>

// Returns true or false if at a certain 2d pos of a topviewed model is something
bool GetModelCollsisionXY(int num, int x, int y, int rot);

void MakeCollisionMap(int modelnum);
void ShowCollisionMap(int modelnum);

#endif // GFXENGINE_MODEL_MD2COLLISION_H__
