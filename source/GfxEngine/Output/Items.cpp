// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#include "GfxEngine/3D.h"
#include "GfxEngine/Limits.h"
#include "GfxEngine/Output/Inventory.h"

bool Key[Inventory_Max];
bool Bool[Inventory_Max];

// Items
typedef struct {
    char Name[20];
    char Type[20];
    char ImgFileName[40];
    char SrcA[40];
    char SrcB[40];
    int Price;
    int Var1;
    int Var2;
} Item_Types;

static Item_Types List[ItemType_Max];

// Returns capital lettered text of the given text
static void ucase(const char *old, char *new_)
{
    strcpy(new_, old);

    for (u32 a = 0; a < strlen(old); a++)
    {
        if ((old[a] > 0x60) && (old[a] < 0x7B))
            new_[a] = old[a] - 0x20;
    }
}

void LoadItemList(void)
{
    FILE *ScriptFile = fopen("/rd/items/items.txt", "r");

    // TODO: This file doesn't exist in the current game, so don't crash here
    if (ScriptFile == NULL)
        return;

    int Index  = -1;
    int weapon = -1;

    while (!feof(ScriptFile))
    {
        char Tmp[20];

        char text[100];
        char UcaseCom[100];
        fgets(text, 99, ScriptFile);
        ucase(text, (char *)&UcaseCom);

        if (strncmp("ITEM", UcaseCom, 4) == 0)
        {
            weapon = -1;
            sscanf(UcaseCom, "ITEM%d %19s", &Index, Tmp);
            if (strncmp("WEAPON", Tmp, 6) == 0)
                weapon = 1;
            snprintf(List[Index - 1].Type, sizeof(List[Index - 1].Type), "%s", Tmp);
        }

        if (strncmp("^^NAME", UcaseCom, 6) == 0 && Index != -1)
        {
            sscanf(text, "%*s %19s", Tmp);
            snprintf(List[Index - 1].Name, sizeof(List[Index - 1].Name), "%s", Tmp);
        }

        if (strncmp("^^IMG", UcaseCom, 5) == 0 && Index != -1)
        {
            sscanf(text, "%*s %19s", Tmp);
            snprintf(List[Index - 1].ImgFileName, sizeof(List[Index - 1].ImgFileName), "/rd/items/%s", Tmp);
        }

        if (strncmp("^^PRICE", UcaseCom, 7) == 0 && Index != -1)
        {
            int value = -1;
            sscanf(UcaseCom, "^^PRICE %d", &value);
            List[Index - 1].Price = value;
        }

        if (strncmp("^^DAMAGE", UcaseCom, 8) == 0 && Index != -1 && weapon != -1)
        {
            int value = -1;
            sscanf(UcaseCom, "^^DAMAGE %d", &value);
            List[Index - 1].Var1 = value;
        }

        if (strncmp("^^MD2", UcaseCom, 5) == 0 && Index != -1 && weapon != -1)
        {
            sscanf(UcaseCom, "^^MD2 %19s", Tmp);
            snprintf(List[Index - 1].SrcA, sizeof(List[Index - 1].SrcA), "/rd/items/%s", Tmp);

            sscanf(UcaseCom, "^^MD2 %*s %19s", Tmp);
            snprintf(List[Index - 1].SrcB, sizeof(List[Index - 1].SrcB), "/rd/items/%s", Tmp);
        }
    }

    fclose(ScriptFile);
}

void ClearItems(void)
{
    for (int a = 0; a < Inventory_Max; a++)
        Key[a] = false;

    for (int a = 0; a < ItemType_Max; a++)
    {
        strcpy(List[a].Name, "");
        strcpy(List[a].Type, "");
        strcpy(List[a].ImgFileName, "");
        strcpy(List[a].SrcA, "");
        strcpy(List[a].SrcB, "");
        List[a].Var1  = 0;
        List[a].Var2  = 0;
        List[a].Price = 0;
    }
}

const char *GiveItemName(int Index)
{
    if ((Index < 0) || (Index >= ItemType_Max))
        return "";

    return List[Index].Name;
}

const char *GiveItemImgName(int Index)
{
    if ((Index < 0) || (Index >= ItemType_Max))
        return "";

    return List[Index].ImgFileName;
}

const char *GiveItemSrcA(int Index)
{
    if ((Index < 0) || (Index >= ItemType_Max))
        return "";

    return List[Index].SrcA;
}

const char *GiveItemSrcB(int Index)
{
    if ((Index < 0) || (Index >= ItemType_Max))
        return "";

    return List[Index].SrcB;
}

const char *GiveItemType(int Index)
{
    if ((Index < 0) || (Index >= ItemType_Max))
        return "";

    return List[Index].Type;
}

bool BoolCheck_Set(const char *UcaseCom)
{
    bool *Container;   // Its a link to right script intern array
    char ContName[20]; // Its the name for right container
    int Index;         // index for key array
    char Operator[3];  // oparator (compare: != or ==) (set: =)...like c++
    char Value[10];    // can be true or false
    char Tmp[20];      // it just holds values to merge some things

    // More containers can be accesed...we need to link right one
    if (strncmp("[KEY", UcaseCom, 4) == 0)
    {
        Container = (bool *)Key;
        strcpy(ContName, "[KEY");
    }

    if (strncmp("[BOOL", UcaseCom, 4) == 0)
    {
        Container = (bool *)Bool;
        strcpy(ContName, "[BOOL");
    }

    // Ok whe have some bool types....Key & Bool for now...
    // here we get the KeyIndex
    strcpy(Tmp, ContName);
    strcat(Tmp, "%i");
    sscanf(UcaseCom, Tmp, &Index);

    // here we get the operator
    strcpy(Tmp, ContName);
    strcat(Tmp, "%*d%c%c");
    sscanf(UcaseCom, Tmp, &Operator[0], &Operator[1]);
    Operator[2] = '\0';
    if (Operator[1] != '=')
        Operator[1] = 'S'; // Ahh a setoperator

    // Here we get the value to compare or set...
    if (strncmp("=S", Operator, 2) == 0)
    {
        snprintf(Tmp, sizeof(Tmp), "%s%s", ContName, "%*d%*c%s");
        sscanf(UcaseCom, Tmp, Value);
    }
    else
    {
        snprintf(Tmp, sizeof(Tmp), "%s%s", ContName, "%*d%*c%*c%s");
        sscanf(UcaseCom, Tmp, Value);
    }
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // We get all we needed to handle now
    if (Index > 0 && Index <= Inventory_Max)
    {
        // Setting a value (=)
        if (strncmp("=S", Operator, 2) == 0)
        {
            if (strncmp("TRUE]", Value, 5) == 0)
                Container[Index - 1] = true;
            if (strncmp("FALSE]", Value, 6) == 0)
                Container[Index - 1] = false;
            return true; // Lets stop that shit now
        }

        // If not same (!=)
        if (strncmp("!=", Operator, 2) == 0)
        {
            if (strncmp("TRUE]", Value, 5) == 0 && Container[Index - 1] != true)
                return true;
            if (strncmp("FALSE]", Value, 6) == 0 && Container[Index - 1] != false)
                return true;
            return false;
        }

        // If same (==)
        if (strncmp("==", Operator, 2) == 0)
        {
            if (strncmp("TRUE]", Value, 5) == 0 && Container[Index - 1] == true)
                return true;
            if (strncmp("FALSE]", Value, 6) == 0 && Container[Index - 1] == false)
                return true;
            return false;
        }
    }

    // When it reaches here...something was made wrong...:(
    return false;
}

bool ItemCheck_Set(const char *Com)
{
    char UcaseCom[256];

    ucase(Com, (char *)&UcaseCom); // make command uppercaes for non-casesensetive

    // For checking/setting some different boolean values
    if (strncmp("[KEY", UcaseCom, 4) == 0)
        return BoolCheck_Set(UcaseCom);
    if (strncmp("[BOOL", UcaseCom, 5) == 0)
        return BoolCheck_Set(UcaseCom);

    if (strncmp("[ADDITEM", UcaseCom, 5) == 0)
    {
        int a;
        sscanf(UcaseCom, "[ADDITEM%d]", &a);
        AddItemToInventory(a - 1);
    }

    // When it reaches here...something was made wrong...:(
    return false;
}
