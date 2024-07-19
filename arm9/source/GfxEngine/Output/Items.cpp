#include "GfxEngine/3D.h"

// bool Key[100];
// bool Bool[100];

// Items
#if 0
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
#endif

// Item_Types List[25];

void LoadItemList(void)
{
#if 0
    FILE *ScriptFile = fopen("/rd/Items/Items.txt", "r");

    if (ScriptFile != NULL) {
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
    } else
        while (1) {
        }
#endif
}

void ClearItems(void)
{
#if 0
    int a;
    for (a = 0; a < 100; a++) {
        Key[a] = false;
    }

    for (a = 0; a < 25; a++) {
        strcpy(List[a].Name, "-");
        strcpy(List[a].Type, "-");
        strcpy(List[a].ImgFileName, "-");
        strcpy(List[a].SrcA, "-");
        strcpy(List[a].SrcB, "-");
        List[a].Var1  = 0;
        List[a].Var2  = 0;
        List[a].Price = 0;
    }
#endif
}

const char *GiveItemName(int Index)
{
    // return List[Index].Name;
    return NULL;
}

const char *GiveItemImgName(int Index)
{
    // return List[Index].ImgFileName;
    return NULL;
}

const char *GiveItemSrcA(int Index)
{
    // return List[Index].SrcA;
    return NULL;
}

const char *GiveItemSrcB(int Index)
{
    // return List[Index].SrcB;
    return NULL;
}

const char *GiveItemType(int Index)
{
    // return List[Index].Type;
    return NULL;
}

bool BoolCheck_Set(const char *UcaseCom)
{
#if 0
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
#endif
    return false;
}

bool ItemCheck_Set(const char *Com)
{
#if 0
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
#endif

    return false;
}