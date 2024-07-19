#ifndef GFXENGINE_OUTPUT_INVENTORY_H__
#define GFXENGINE_OUTPUT_INVENTORY_H__

#include <nds.h>

extern int Inventory[100];
extern int Inventory_Count;

extern int EquipedWeapon;

void ResetInventory(void);
void AddItemToInventory(u8 Index);

#endif // GFXENGINE_OUTPUT_INVENTORY_H__
