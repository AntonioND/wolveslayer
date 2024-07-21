#include "GfxEngine/3D.h"
#include "GfxEngine/Limits.h"

int Inventory[Inventory_Max];
int Inventory_Count = 0;

int EquipedWeapon = -1;

void ResetInventory(void)
{
    for (int a = 0; a < Inventory_Max; a++)
        Inventory[a] = -1;
    Inventory_Count = 0;
    EquipedWeapon   = -1;
}

void AddItemToInventory(u8 Index)
{
    if (Inventory_Count < Inventory_Max)
    {
        Inventory[Inventory_Count] = Index;
        Inventory_Count++;
    }
}
