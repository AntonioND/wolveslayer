#ifndef GFXENGINE_MODEL_MD2MODELS_H__
#define GFXENGINE_MODEL_MD2MODELS_H__

#include "GfxEngine/Limits.h"
#include "GfxEngine/Model/MD2Format.h"

extern MD2Entity Models[MD2_Count];
extern bool ModelEnable[MD2_Count];

void Precalcmd2light(int n, int num);
void RenderMD2Model(int n, int num);
void RenderMD2ModelMirrowed(int n, int num);

#endif // GFXENGINE_MODEL_MD2MODELS_H__
