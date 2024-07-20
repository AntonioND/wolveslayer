#include "GfxEngine/3D.h"
#include "GfxEngine/Files.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Output/Textbox.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Render/Doors.h"
#include "GfxEngine/Render/Obj_Wall.h"
#include "GfxEngine/Render/Render.h"
#include "GfxEngine/Script/Script_Load.h"
#include "GfxEngine/Script/Script_Objects.h"
#include "GfxEngine/Texture/DynamicLights.h"
#include "GfxEngine/Texture/Light.h"

u32 MapImage[256][256];

u8 MapLightR[128 * 128];
u8 MapLightG[128 * 128];
u8 MapLightB[128 * 128];

static int MapW, MapH;
static int MapWreal, MapHreal;

typedef struct {
    char Name[60];

    int PosX, PosY;     // Location of the player in the origin map
    int TOPosX, TOPosY; // Destination of the player in the destination map

    bool IsDoor; // Set to true if the change is a door
    int DoorKey;
    int DoorAngle;
    char DoorSpeech[10][256]; // to hold 10 textes for each door with 256 chars
    int DoorSpeechCount;      // Holds the Number of speeches a Door has...
} MapChangeInfo;

static MapChangeInfo MapChange[25];

static int MapChangeCounter;

v16 GetTerrain(int x, int y, int num)
{
    return Terrain[x][y].v[num];
}

void ResetMapChange(void)
{
    MapChangeCounter = 0;

    for (int a = 0; a < 25; a++) {
        MapChange[a].IsDoor  = false;
        MapChange[a].DoorKey = 0;
        MapChange[a].PosX    = -1;
        MapChange[a].PosY    = -1;
        MapChange[a].TOPosX  = -1;
        MapChange[a].TOPosY  = -1;
        strcpy(MapChange[a].Name, "");
        MapChange[a].DoorAngle = 0;
        for (int b = 0; b < 10; b++)
            strcpy(MapChange[a].DoorSpeech[b], "");
        MapChange[a].DoorSpeechCount = 0;
    }
}

void AddMapChange(int x, int y, char Filename[], int tox, int toy)
{
    if (MapChangeCounter >= 25)
        Crash("Too many map changes");

    MapChange[MapChangeCounter].IsDoor  = false; // It isn't a door
    MapChange[MapChangeCounter].DoorKey = 0;     // So it doesnt need a key
    MapChange[MapChangeCounter].PosX    = x;
    MapChange[MapChangeCounter].PosY    = y;
    MapChange[MapChangeCounter].TOPosX  = tox;
    MapChange[MapChangeCounter].TOPosY  = toy;
    snprintf(MapChange[MapChangeCounter].Name, sizeof(MapChange[MapChangeCounter].Name), "%s", Filename);
    MapChangeCounter++;
}

void AddMapDoor(int x, int y, char Filename[], int tox, int toy, int key)
{
    if (MapChangeCounter >= 25)
        Crash("Too many map doors");

    MapChange[MapChangeCounter].IsDoor    = true; // It is a door
    MapChange[MapChangeCounter].DoorKey   = key;  // So it needs a key
    MapChange[MapChangeCounter].PosX      = x;
    MapChange[MapChangeCounter].PosY      = y;
    MapChange[MapChangeCounter].TOPosX    = tox;
    MapChange[MapChangeCounter].TOPosY    = toy;
    MapChange[MapChangeCounter].DoorAngle = 0;
    snprintf(MapChange[MapChangeCounter].Name, sizeof(MapChange[MapChangeCounter].Name), "%s", Filename);
    MapChangeCounter++;
}

// TODO: This is unused
void AddTexttoDoor(char txt[256])
{
    int a = MapChangeCounter - 1;

    int count = MapChange[a].DoorSpeechCount;
    if (count >= 10)
        Crash("Too many texts in door");

    strcpy(MapChange[a].DoorSpeech[count], txt);
    MapChange[a].DoorSpeechCount++;
}

void GetRGBfromMap(int x, int y, u8 &r, u8 &g, u8 &b)
{
    r = u8(MapImage[x][y] & (0xFF));
    g = u8((MapImage[x][y] & (0xFF00)) >> 8);
    b = u8((MapImage[x][y] & (0xFF0000)) >> 16);
}

const char *GetMapDoor(int x, int y)
{
    for (int i = 0; i < MapChangeCounter; i++) {
        if (MapChange[i].PosX == x && MapChange[i].PosY == y && MapChange[i].IsDoor)
            return MapChange[i].Name;
    }
    return ".";
}

int GetMapDoorAngle(int x, int y)
{
    for (int i = 0; i < MapChangeCounter; i++) {
        if (MapChange[i].PosX == x && MapChange[i].PosY == y && MapChange[i].IsDoor)
            return MapChange[i].DoorAngle;
    }
    return -1;
}

void OpenMapDoor(int x, int y)
{
    for (int i = 0; i < MapChangeCounter; i++) {
        if (MapChange[i].PosX == x && MapChange[i].PosY == y && MapChange[i].IsDoor && MapChange[i].DoorAngle == 0) {
            MapChange[i].DoorAngle = 10;
            // TODO: This can be used if a door can't be opened due to not
            // having a key or something like that.
            // TextBoxmode(MapChange[i].DoorSpeech, MapChange[i].DoorSpeechCount, -1);
        }
    }
}

void MapDoorHandle(void)
{
    for (int i = 0; i < MapChangeCounter; i++) {
        if (MapChange[i].DoorAngle >= 1 && MapChange[i].DoorAngle <= 80)
            MapChange[i].DoorAngle += 10;

        // Mapchange if door is opened
        if (MapChange[i].DoorAngle >= 80) {
            char Filename[50];
            int newx = 0, newy = 0;
            snprintf(Filename, sizeof(Filename), "%s", GetMapChange(MapChange[i].PosX, MapChange[i].PosY));
            if (Filename[0] != '.') {
                newx = MapChange[i].TOPosX - 6;
                newy = MapChange[i].TOPosY - 8;
                printf("\x1b[2J");

                for (int b = 8; b >= 0; b--) {
                    Blend = b;
                    RunTime();
                    E3D_StartRender();
                    UpdateDynamic();
                    E3D_Render();
                    E3D_EndRender();
                }

                LoadScript(Filename);
                PlPosX  = newx;
                PlPosY  = newy;
                CamPosX = newx;
                CamPosY = newy;

                for (int b = 0; b <= 8; b++) {
                    Blend = b;
                    RunTime();
                    E3D_StartRender();
                    UpdateDynamic();
                    E3D_Render();
                    E3D_EndRender();
                }
            }
        }
    }
}

const char *GetMapChange(int x, int y)
{
    for (int i = 0; i < MapChangeCounter; i++) {
        if (MapChange[i].PosX == x && MapChange[i].PosY == y)
            return MapChange[i].Name;
    }
    return ".";
}

int GetMapChangePOSX(int x, int y)
{
    for (int i = 0; i < MapChangeCounter; i++) {
        if (MapChange[i].PosX == x && MapChange[i].PosY == y)
            return MapChange[i].TOPosX;
    }
    return 0;
}

int GetMapChangePOSY(int x, int y)
{
    for (int i = 0; i < MapChangeCounter; i++) {
        if (MapChange[i].PosX == x && MapChange[i].PosY == y)
            return MapChange[i].TOPosY;
    }
    return 0;
}

int MapGetWr(void)
{
    return MapWreal;
}

int MapGetHr(void)
{
    return MapHreal;
}

// For getting ground colors
u32 MapGroundGetRGB(int x, int y)
{
    int xx = (x * 3) + 1;
    int yy = (y * 3) + 1;

    // Return invalid value for the graphics engine
    if ((x < 0) || (y < 0) || (x >= MapWreal) || (y >= MapHreal))
        return 0;

    return MapImage[xx][yy];
}

u8 MapObjGetRot(int x, int y)
{
    int xx  = (x * 3) + 1;
    int yy  = (y * 3) + 2;
    int rot = 0;
    u8 rot2 = 0;

    // Return invalid value for the graphics engine
    if ((x < 0) || (y < 0) || (x >= MapWreal) || (y >= MapHreal))
        return 0;

    rot = u8(MapImage[xx][yy] & 255);
    if (rot >= 0)
        rot2 = 0;
    if (rot >= 63)
        rot2 = 2;
    if (rot >= 127)
        rot2 = 4;
    if (rot >= 191)
        rot2 = 6;

    return rot2;
}

u32 MapObjectGetRGB(int x, int y)
{
    if ((x > -1 && y > -1) && (x < MapGetWr() && y < MapGetHr())) {
        int xx = (x * 3) + 2;
        int yy = (y * 3) + 1;
        return MapImage[xx][yy];
    }

    return 0;
}

bool IsObjHouse(int x, int y)
{
    int choose = -1;

    if (x > -1 && y > -1) {
        if (x < MapGetWr() && y < MapGetHr()) {
            u32 Obj = MapObjectGetRGB(x, y);

            for (int texturecounter = 0; texturecounter < 10; texturecounter++) {
                if (Obj == Objects[texturecounter].ColorKey)
                    choose = texturecounter;
            }

            if (choose != -1 && strncmp(Objects[choose].Type, "HOUSE", 5) == 0)
                return true;
        }
    }

    return false;
}

bool IsObjWall(int x, int y)
{
    int choose = -1;

    if (x > -1 && y > -1) {
        if (x < MapGetWr() && y < MapGetHr()) {
            u32 Obj = MapObjectGetRGB(x, y);

            for (int texturecounter = 0; texturecounter < 10; texturecounter++) {
                if (Obj == Objects[texturecounter].ColorKey)
                    choose = texturecounter;
            }

            if (choose != -1 && strncmp(Objects[choose].Type, "WALL", 4) == 0)
                return true;
        }
    }

    return false;
}

bool IsObjBumpWall(int x, int y)
{
    int choose = -1;

    if (x > -1 && y > -1) {
        if (x < MapGetWr() && y < MapGetHr()) {
            u32 Obj = MapObjectGetRGB(x, y);

            for (int texturecounter = 0; texturecounter < 10; texturecounter++) {
                if (Obj == Objects[texturecounter].ColorKey)
                    choose = texturecounter;
            }

            if (choose != -1 && strncmp(Objects[choose].Type, "BUMPWALL", 8) == 0)
                return true;
        }
    }

    return false;
}

float GetHight(int x, int y)
{
    int xx = (x * 3) + 2;
    int yy = (y * 3) + 2;

    // Return invalid value for the graphics engine
    if ((x < 0) || (y < 0) || (x >= MapWreal) || (y >= MapHreal))
        return 0;

    uint r = MapImage[xx][yy] & 255;

    float r2 = float((r >> 5) / 4.0f);

    return r2;
}

// Maploading routine
void LoadMap(char *filename)
{
    u8 *picbuff;
    u32 width, height;
    LoadBmpAllocBuffer24(filename, &picbuff, &height, &width);

    MapW     = width;
    MapH     = height;
    MapWreal = width / 3;
    MapHreal = height / 3;

    memset(MapImage, 0, sizeof(MapImage));

    u8 *pic = picbuff;
    for (int i = height - 1; i >= 0; i--) {
        for (u32 q = 0; q < width; q++) {
            u32 b = *pic++;
            u32 g = *pic++;
            u32 r = *pic++;

            if (q >= 0 && q < 256)
                if (i >= 0 && i < 256)
                    MapImage[q][i] = (r) | (g << 8) | (b << 16) | (0 << 24);

            if (q % 3 == 0 && i % 3 == 0) {
                int point = (q / 3) + (i / 3) * 128;
                if (point < 128 * 128 && point >= 0) {
                    MapLightR[point] = r;
                    MapLightG[point] = g;
                    MapLightB[point] = b;

                    WorldLightR[point] = r;
                    WorldLightG[point] = g;
                    WorldLightB[point] = b;
                }
            }
        }
    }

    free(picbuff);
}
