#include "3D.h"
#include "GfxEngine/input/Input.h"
#include "GfxEngine/output/Inventory.h"
#include "GfxEngine/output/Items.h"

// Those setting are for new game choice
static char StartMap[60];
static int StartMapX, StartMapY;

char MaincharMd2[60];
char MaincharText[60];

char waffeMd2[60];
char waffeText[60];

static void Setit(char *setting)
{
    char Filename[40];

    // Readout startmap
    if (strncmp("StartMap ", setting, 9) == 0) {
        sscanf(setting, "%*s %39s", Filename);
        snprintf(StartMap, sizeof(StartMap), "/wolveslayer/maps/%s", Filename);
    }

    if (strncmp("StartPos ", setting, 9) == 0) {
        sscanf(setting, "%*s %d", &StartMapX);
        sscanf(setting, "%*s %*d %d", &StartMapY);
    }

    if (strncmp("StartChar ", setting, 10) == 0) {
        sscanf(setting, "%*s %39s", Filename);
        snprintf(MaincharMd2, sizeof(MaincharMd2), "/wolveslayer/chars/%s", Filename);

        sscanf(setting, "%*s %*s %39s", Filename);
        snprintf(MaincharText, sizeof(MaincharText), "/wolveslayer/chars/%s", Filename);
    }

    if (strncmp("StartWaffe ", setting, 11) == 0) {
        sscanf(setting, "%*s %39s", Filename);
        snprintf(waffeMd2, sizeof(waffeMd2), "/wolveslayer/items/%s", Filename);

        sscanf(setting, "%*s %*s %39s", Filename);
        snprintf(waffeText, sizeof(waffeText), "/wolveslayer/items/%s", Filename);
    }
}

void ReadSettings(void)
{
    char text[100];
    int count = 0;

    FILE *ScriptFile = fopen("/wolveslayer/Settings.txt", "r");
    if (ScriptFile != NULL) {
        while (!feof(ScriptFile)) {
            fgets(text, 99, ScriptFile);

            if (text[0] != '/') {
                Setit(text);
                count++;
            }

            strcpy(text, "");
        }
        fclose(ScriptFile);
    }

    // Add playerpos
    PlPosX  = StartMapX - 6;
    PlPosY  = StartMapY - 8;
    CamPosX = StartMapX - 6;
    CamPosY = StartMapY - 8;

    // Start script file
    ClearItems();
    ResetInventory();
    LoadItemList();
    LoadScrip(StartMap);
}
