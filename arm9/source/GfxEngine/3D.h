#ifndef GFXENGINE_3D_H__
#define GFXENGINE_3D_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nds.h>
#include <nds/arm9/image.h>
#include <nds/arm9/trig_lut.h>

typedef int32_t f32;

// DebugInfo Options
#define ShowHeapUsage
#define ShowPosition
#define ShowPolyCount

// Other settings
#define Object_Count 26

// Some replacers for bitshifting inside precalcdata
#define water       0
#define mirrow      1
#define BumpgroundS 2
#define BumpgroundW 3
#define BumpgroundE 4
#define BumpwallS   5
#define BumpwallW   6
#define BumpwallE   7

__attribute__((noreturn)) void Crash(const char *msg, ...);

// Sound
void InitSound(void);
void StartSong(const char *Name); // Overgive filename and arm7 will do the rest for playing that song
// void StopSong(void);//Must! be called before starting new one...best is wait a vblank
void Playhandler(void); // checks if we need to start song again

// Bmp and general file loading
void *LoadFile(const char *filename, size_t *size);
void LoadBmptoBuffer(const char *filename, u16 *picbuff); // For getting 8bit bmp into a 16bit buffer
void LoadBmptoBuffer8(const char *filename, u8 *picbuff, u16 *palbuff);
void LoadBmpAllocBuffer8(const char *filename, u8 **picbuff_, u16 **palbuff_, u32 *height_, u32 *width_);
void LoadBmpAllocBuffer24(const char *filename, u8 **picbuff_, u32 *height_, u32 *width_);

void WaitForFreeVblank(void); // important for multitask buffering and loading to vram

void vBlank(void);
void E3D_Init(void);
void E3D_StartRender();

// Touchscreen Functions

void UpdateBar(void);
// Displays BG (cls)
void ScreenMode(void);
// Displays loading bg
void ScreenModeLOADING(void);
// Starting capture
void CaptureMode(void);
// Determinates if capture was done, and if so swaps back to sceenmode
void ScreenModeHandler(void);
// Prints a text on specific position and restores bg
void Print(const char *Text, int x, int y);
// Doesnt restore BG but can swap between black and white and just print n chars
void PrintOUT(const char *Text, int x, int y, bool color, int n);
// For loading things is nice to see the progress
void PercentBar(int cur, int max);
void ItemMode(void);
void PauseMode(void);
void EquipMode(void);

//>>>>>>>>>>>>>>>>>MAP

void LoadMap(char *filename);
u32 MapBodenGetRGB(int x, int y);
u32 MapObjectGetRGB(int x, int y);
u8 MapObjGetRot(int x, int y);
bool IsObjHouse(int x, int y);    // deterimantes if there is a housepart at give pos.
bool IsObjWall(int x, int y);     // deterimantes if there is a wall at give pos.
bool IsObjBumpWall(int x, int y); // deterimantes if there is a bumpmapped wall at give pos.
float GetHight(int x, int y);
int MapGetWr(void);
int MapGetHr(void);

// Mapchanges

// For reseting mapchange positions and mapchangedoors
void ResetMapChange(void);
void AddMapChange(int x, int y, char Filename[], int tox, int toy);        // adds a mapchange
void AddMapDoor(int x, int y, char Filename[], int tox, int toy, int key); // adds a door
const char *GetMapChange(int x, int y);
int GetMapChangePOSX(int x, int y);
int GetMapChangePOSY(int x, int y);
// mapdoors
const char *GetMapDoor(int x, int y);
int GetMapDoorAngle(int x, int y);
void OpenMapDoor(int x, int y); // This sets the door angle to 1 if it is unlocked
void MapDoorHandle(void);       // This opens doors which angle is greater then 0
v16 GetTerrain(int x, int y, int num);
//>>>>>>>>>>>>>>>>Script
void LoadScrip(char filename[]);        // Loads a new script

//>>>>>>>>>>>>>>>>Refresh
void Refresh3D(void);

///>>>>>>>>>>>>>>>3D
// MD2 Models

void InitTableOfNormal(void);
void FreeModels(void);
void LoadModelTexture(char filename[], int *Target, int num, int *w);
void LoadMD2Model(char Filename[], int num, int widthheight, int scale);
void Precalcmd2light(int n, int num);
void RenderMD2Model(int n, int num);
void RenderMD2ModelMirrowed(int n, int num);
bool GetModelCollsisionXY(int num, int x, int y, int rot); // gets a true or false if at a certain 2d pos of a topviewed model is something

// Ufer
void RenderUfer(int x, int y, f32 xx, f32 yy); // Will appear where terrain meet stairs or stair meet stair
void LoadUferTexture(char filename[], int *Target, int num);

// Sprite
void RenderSprite(int frame, int dirrection, float x, float y, float z);

// Wasser
void LoadWasserTexture(char filename[], int *Target, int num);
void RenderWasser(int floating, f32 x, f32 y, f32 z);

// Objects
void LoadObjectTexture(char filename[], int *Target, int num);
void LoadDoorTexture(char filename[]);
void AddTexttoDoor(char txt[256]);

#define Obj_MD2 -10;

// Wallparts
#define Obj_WallAlone 10;
#define Obj_WallBor   11;
#define Obj_WallDoor  12;

// #define Obj_WallCor 12;//not visible thing in fact

void RenderWallBorderDoor(int mode, int doorangle, int dirrection, f32 x, f32 y, f32 z);

// Houseparts
#define Obj_HouseMid  20;
#define Obj_HouseBor  21;
#define Obj_HouseCor  22;
#define Obj_HouseDoor 23;

void SetCurWall(int x, int y); // The Wall needs to know where it stands to set up light correctly
void SetWallTrans(bool bol);   // Makes the walls transparent or native
void RenderHouseM(int mode, f32 x, f32 y, f32 z);
void RenderHouseBorder(int mode, int dirrection, f32 x, f32 y, f32 z);
void RenderHouseBorderDoor(int mode, int doorangle, int dirrection, f32 x, f32 y, f32 z);
void RenderHouseCorner(int mode, int dirrection, f32 x, f32 y, f32 z);

// Bumpmapping

// Used for walls only
void LoadMBump3Texture(char filename[], int num, int *target, int *targetb, int *targetc);
// Used for grounds only (no water and no autotiles!)
void LoadMBump5Texture(char filename[], int num);

// Enemys
void ResetEnemys(void);
void AddEnemy(int x, int y, int texnum, int atp, int hp, float rad, bool boss);
void UpdateEnemy();

// Events
void ResetEvent(void);
void AddTexttoEvent(char txt[256]);
void CheckEndGame(void);

// Textbox

// will be called if char talks to NPC or other things
void TextBoxmode(char TxtOffset[10][256], int cnt, int num);
// This will put char by char the text to Dialogbox...
void TextBoxmodeHandler(void);
void ShownextPage(void);

// Items
void LoadItemList(void);
void ClearItems(void);         // Clears all times....no ownership
bool ItemCheck_Set(const char *Com); // It compares and gives back if matches or not/or sets a value...
const char *GiveItemName(int Index);
const char *GiveItemImgName(int Index);
const char *GiveItemSrcA(int Index);
const char *GiveItemSrcB(int Index);
const char *GiveItemType(int Index);

// Inventory
void ResetInventory(void);
void AddItemToInventory(u8 Index);

#endif // GFXENGINE_3D_H__
