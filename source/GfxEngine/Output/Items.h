// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_OUTPUT_ITEMS_H__
#define GFXENGINE_OUTPUT_ITEMS_H__

void LoadItemList(void);
void ClearItems(void);               // Clears all times....no ownership
bool ItemCheck_Set(const char *Com); // It compares and gives back if matches or not/or sets a value...
const char *GiveItemName(int Index);
const char *GiveItemImgName(int Index);
const char *GiveItemSrcA(int Index);
const char *GiveItemSrcB(int Index);
const char *GiveItemType(int Index);

#endif // GFXENGINE_OUTPUT_ITEMS_H__
