#include "../3D.h"

int Inventory[100];
int Inventory_Count = 0;
int EquipedWeapon   = -1;

void ResetInventory(void)
{
    for (int a = 0; a < 100; a++)
        Inventory[a] = -1;
    Inventory_Count = 0;
    EquipedWeapon   = -1;
}

void AddItemToInventory(u8 Index)
{
    if (Inventory_Count < 100) {
        Inventory[Inventory_Count] = Index;
        Inventory_Count++;
    }
}
