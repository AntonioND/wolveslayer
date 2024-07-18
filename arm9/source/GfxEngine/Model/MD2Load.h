#ifndef GFXENGINE_MODEL_MD2LOAD_H__
#define GFXENGINE_MODEL_MD2LOAD_H__

void InitTableOfNormal(void);
void FreeModels(void);
void LoadModelTexture(char filename[], int *Target, int num, int *w);
void LoadMD2Model(char Filename[], int num, int widthheight, int scale);

#endif // GFXENGINE_MODEL_MD2LOAD_H__
