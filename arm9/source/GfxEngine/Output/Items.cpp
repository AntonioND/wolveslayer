#include "GfxEngine/3D.h"
#include "GfxEngine/Script/Script_Load.h"
#include "GfxEngine/Output/Inventory.h"

bool Key[100];
bool Bool[100];

// Items
typedef struct
{
    char Name[20];
    char Type[20];
    char ImgFileName[40];
    char SrcA[40];
    char SrcB[40];
    int Price;
    int Var1;
    int Var2;
} Item_Types;

Item_Types List[25];

void LoadItemList(void)
{
    FILE *ScriptFile = fopen("/rd/Items/Items.txt", "r");

    // TODO: This file doesn't exist, so don't crash here
    if (ScriptFile == NULL)
        return;

    char text[256];
    char UcaseCom[256];
    char Tmp[256];
    int zahl;

    int Index = -1;
    int waffe = -1;

    while (!feof(ScriptFile)) {
        fgets(text, 255, ScriptFile);
        ucase(text, (char *)&UcaseCom);

        if (strncmp("ITEM", UcaseCom, 4) == 0) {
            waffe = -1;
            sscanf(UcaseCom, "ITEM%d %s", &Index, Tmp);
            if (strncmp("WEAPON", Tmp, 6) == 0)
                waffe = 1;
            strcpy(List[Index - 1].Type, Tmp);
        }

        if (strncmp("^^NAME", UcaseCom, 6) == 0 && Index != -1) {
            sscanf(text, "%*s %s", Tmp);
            strcpy(List[Index - 1].Name, Tmp);
        }

        if (strncmp("^^IMG", UcaseCom, 5) == 0 && Index != -1) {
            sscanf(text, "%*s %s", Tmp);
            strcpy(List[Index - 1].ImgFileName, "/rd/items/");
            strcat(List[Index - 1].ImgFileName, Tmp);
        }

        if (strncmp("^^PRICE", UcaseCom, 7) == 0 && Index != -1) {
            sscanf(UcaseCom, "^^PRICE %d", &zahl);
            List[Index - 1].Price = zahl;
        }

        if (strncmp("^^DAMAGE", UcaseCom, 8) == 0 && Index != -1 && waffe != -1) {
            sscanf(UcaseCom, "^^DAMAGE %d", &zahl);
            List[Index - 1].Var1 = zahl;
        }

        if (strncmp("^^MD2", UcaseCom, 5) == 0 && Index != -1 && waffe != -1) {
            sscanf(UcaseCom, "^^MD2 %s", Tmp);
            strcpy(List[Index - 1].SrcA, "/rd/items/");
            strcat(List[Index - 1].SrcA, Tmp);

            sscanf(UcaseCom, "^^MD2 %*s %s", Tmp);
            strcpy(List[Index - 1].SrcB, "/rd/items/");
            strcat(List[Index - 1].SrcB, Tmp);
        }
    }

    fclose(ScriptFile);
}

void ClearItems(void)
{
    for (int a = 0; a < 100; a++)
        Key[a] = false;

    for (int a = 0; a < 25; a++) {
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
    if ((Index < 0) || (Index >= 25))
        return "";

    return List[Index].Name;
}

const char *GiveItemImgName(int Index)
{
    if ((Index < 0) || (Index >= 25))
        return "";

    return List[Index].ImgFileName;
}

const char *GiveItemSrcA(int Index)
{
    if ((Index < 0) || (Index >= 25))
        return "";

    return List[Index].SrcA;
}

const char *GiveItemSrcB(int Index)
{
    if ((Index < 0) || (Index >= 25))
        return "";

    return List[Index].SrcB;
}

const char *GiveItemType(int Index)
{
    if ((Index < 0) || (Index >= 25))
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
    if (strncmp("[KEY", UcaseCom, 4) == 0) {
        Container = (bool *)Key;
        strcpy(ContName, "[KEY");
    }

    if (strncmp("[BOOL", UcaseCom, 4) == 0) {
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
    if (strncmp("=S", Operator, 2) == 0) {
        strcpy(Tmp, ContName);
        strcat(Tmp, "%*d%*c%s");
        sscanf(UcaseCom, Tmp, Value);
    } else {
        strcpy(Tmp, ContName);
        strcat(Tmp, "%*d%*c%*c%s");
        sscanf(UcaseCom, Tmp, Value);
    }
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // We get all we needed to handle now
    if (Index > 0 && Index <= 100) {
        // Setting a value (=)
        if (strncmp("=S", Operator, 2) == 0) {
            if (strncmp("TRUE]", Value, 5) == 0)
                Container[Index - 1] = true;
            if (strncmp("FALSE]", Value, 6) == 0)
                Container[Index - 1] = false;
            return true; // Lets stop that shit now
        }

        // If not same (!=)
        if (strncmp("!=", Operator, 2) == 0) {
            if (strncmp("TRUE]", Value, 5) == 0 && Container[Index - 1] != true)
                return true;
            if (strncmp("FALSE]", Value, 6) == 0 && Container[Index - 1] != false)
                return true;
            return false;
        }

        // If same (==)
        if (strncmp("==", Operator, 2) == 0) {
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

    if (strncmp("[ADDITEM", UcaseCom, 5) == 0) {
        int a;
        sscanf(UcaseCom, "[ADDITEM%d]", &a);
        AddItemToInventory(a - 1);
    }

    // When it reaches here...something was made wrong...:(
    return false;
}
