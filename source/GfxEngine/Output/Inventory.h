// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_OUTPUT_INVENTORY_H__
#define GFXENGINE_OUTPUT_INVENTORY_H__

#include <nds.h>

#include "GfxEngine/Limits.h"

extern int Inventory[Inventory_Max];
extern int Inventory_Count;

extern int EquipedWeapon;

void ResetInventory(void);
void AddItemToInventory(u8 Index);

#endif // GFXENGINE_OUTPUT_INVENTORY_H__
