#include <nds.h>
#include <string.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/image.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define f32 int32

/* DebugInfo Options */
#define ShowPosition
#define ShowPolyCount


//Other settings
#define Object_Count 26
#define Ground_Count 31

//Some replacers for bitshifting inside precalcdata
#define water 0
#define mirrow 1
#define BumpgroundS 2
#define BumpgroundW 3
#define BumpgroundE 4
#define BumpwallS 5
#define BumpwallW 6
#define BumpwallE 7

//Sound
void InitSound(void);
void StartSong(const char *Name);//Overgive filename and arm7 will do the rest for playing that song
//void StopSong(void);//Must! be called before starting new one...best is wait a vblank
void Playhandler(void);//checks if we need to start song again

//Bmp
void LoadBmptoBuffer(const char* filename,u16* picbuff);//For getting 8bit bmp into a 16bit buffer

void WaitForFreeVblank(void);//important for multitask buffering and loading to vram 

void vBlank(void);
void vBlankMusic(void);
void tackt(void);
void SetMainBg(unsigned short int *pic);
void E3D_Init(void);
void E3D_StartRender();
void glTranslatef32(f32 x, f32 y, f32 z);
void glScalef32(f32 x, f32 y, f32 z);

//Touchscreen Functions
void UpdateBar(void);//HP bar update
void ScreenMode(void);//Displays BG (cls)
void ScreenModeLOADING(void);//Displays loading bg
void CaptureMode(void);//Starting capture
void ScreenModeHandler(void);//Determinates if capture was done, and if so swaps back to sceenmode
void Print(const char* Text,int x,int y);//Prints a text on specific position and restores bg
void PrintOUT(const char* Text,int x,int y,bool color,int n);//Doesnt restore BG but can swap between black and white and just print n chars
void PercentBar(int cur,int max);//For loading things is nice to see the progress
void ItemMode(void);
void PauseMode(void);
void EquipMode(void);

//>>>>>>>>>>>>>>>>>MAP
void PreCalcAll(void);//That one precalculates which textures to use...gives use  nice speed

	void LoadMap(char* filename);
	u32 MapBodenGetRGB(int x,int y);
	u32 MapObjectGetRGB(int x,int y);
	u8 MapObjGetRot(int x,int y);
	bool IsObjHouse(int x,int y);//deterimantes if there is a housepart at give pos.
	bool IsObjWall(int x,int y);//deterimantes if there is a wall at give pos.
	bool IsObjBumpWall(int x,int y);//deterimantes if there is a bumpmapped wall at give pos.
	float GetHight(int x,int y);
	int MapGetWr(void);
	int MapGetHr(void);
	//Mapchanges
	void ResetMapChange(void);//For reseting mapchange positions and mapchangedoors
	void AddMapChange(int x,int y, char Filename[],int tox, int toy);//adds a mapchange
	void AddMapDoor(int x,int y, char Filename[],int tox, int toy,int key);//adds a door
	const char* GetMapChange(int x,int y);
	int GetMapChangePOSX(int x,int y);
	int GetMapChangePOSY(int x,int y);
	//mapdoors
	const char* GetMapDoor(int x,int y);
	int GetMapDoorAngle(int x,int y);
	void OpenMapDoor(int x,int y);//This sets the door angle to 1 if it is unlocked
	void MapDoorHandle(void);//This opens doors which angle is greater then 0
	v16 GetTerrain(int x,int y,int num);
//>>>>>>>>>>>>>>>>Script
void ReadSettings(void);//gets some settings
void LoadScrip(char filename[]);//Loads a new script
void ucase (char character[256],char *New);//Returns cptial lettered text of the given text

//>>>>>>>>>>>>>>>>Input
void inputs(void);
int GetPX(void);
int GetPY(void);
bool Passable(int x,int y,int sxx,int syy);
void Menu(void);
void InitCircles(void);//this is neccecary for a nice hitdetection
//a circle based hitdetection...u give it pos,rad and pointer and it tells where it cant go to :D
void Checkcolision(int x,int y,int sx,int sy,int rad,bool *values);
void CheckcolisionDiagonal(int x,int y,int sx,int sy,int rad,bool *values);
void CheckcolisionEnemy(int x,int y,int sx,int sy,int rad,bool *values);

//>>>>>>>>>>>>>>>>Refresh
void Refresh3D(void);

///>>>>>>>>>>>>>>>3D
//MD2 Models

void InitTableOfNormal(void);
void FreeModels(void);
void LoadModelTexture(char filename[],int* Target,int* TargetPal,int num,int *w);
void LoadMD2Model(char Filename[],int num,int widthheight,int scale);
void Precalcmd2light(int n,int num);
void RenderMD2Model(int n,int num);
void RenderMD2ModelMirrowed (int n,int num);
bool GetModelCollsisionXY(int num,int x,int y,int rot);//gets a true or false if at a certain 2d pos of a topviewed model is something 

//>>>>>>>>>>>>>>>>Lights
void RunTime(void);//recalcs the lightcolors(usid to run the day/night-swap
void SetRot(int rot);
void GrapLight(int x,int y);//Simple graping lights(its used for terrain)
void GiveLight(int x,int y,u8* col);//Same like above but gives color back (col[3] rgb)
void PreCalcLightHelper(int pos[2],float softpos[2],float radius,int rot);
void SetMdlLights(int pos[2],float softpos[2],float radius,int rot);//prepears some lights for moving md2
void SetObjLights(int pos[2],int rot);//Prepares lights way faster for standing md2(tree or someting)
void SetObjLightsSelfilluminated(void);//selfexplaining
u16 GrapMdlLight(v16 nx,v16 normy,v16 nz);//This is used per vertex for a md2...a good alternative to hw light i hope
void GrapMd2Light(u8 Index,u16 &buff);//This is used per vertex for a md2...best alternative to hw light i hope
int GetLightRot(void);

//Dynamic Lights
void ResetDynamic(void);
void AddDynamic(int x,int y,u8 r,u8 g,u8 b);
void UpdateDynamic(void);

//Autotiles
void LoadAutotileTexture(char filename[],int num);
void RenderAutoBoden( int x, int y, int z,int num);
void AddAutIgnore(int Tilenum, u32 Col);

//Boden
void LoadBodenTexture(char filename[],int num);
void SetCurBod(int x, int y);//The Boden needs to know where it stands to set up light correctly
void SetCurUf(int x, int y);
void RenderBoden ( int x, int y, int z,int textnum);
void RenderLevelBorderBoden ( int x, int y, int z);
//Ufer
void RenderUfer(int x, int y,f32 xx, f32 yy); //Will appear where terrain meet stairs or stair meet stair
void LoadUferTexture(char filename[],int* Target,int* TargetPal,int num);

//Sprite
void LoadSpriteTexture(char filename[],char palname[],int* Target,int* TargetPal,int num);
void RenderSprite ( int frame,int dirrection,float x, float y, float z);
void SetCurSprite(int x, int y,float sx,float sy);

//Wasser
void LoadWasserTexture(char filename[],int* Target,int* TargetPal,int num);
void RenderWasser (int floating, f32 x, f32 y, f32 z);

//Objects
void LoadObjectTexture(char filename[],int* Target,int* TargetPal,int num); 
void LoadDoorTexture(char filename[]);
void AddTexttoDoor(char txt[256]);

	#define Obj_MD2 -10;

	//Wallparts
	#define Obj_WallAlone 10;
	#define Obj_WallBor 11;
	#define Obj_WallDoor 12;
	
	//#define Obj_WallCor 12;//not visible thing in fact

	void RenderWallBorder (int mode,int dirrection ,f32 x, f32 y, f32 z,int textnum);
	void RenderWallBorderDoor (int mode,int doorangle,int dirrection ,f32 x, f32 y, f32 z);
	//Houseparts
	#define Obj_HouseMid 20;
	#define Obj_HouseBor 21;
	#define Obj_HouseCor 22;	
	#define Obj_HouseDoor 23;
	
	void SetCurWall(int x, int y);//The Wall needs to know where it stands to set up light correctly
	void SetWallTrans (bool bol);//Makes the walls transparent or native
	void RenderHouseM (int mode,f32 x, f32 y, f32 z);
	void RenderHouseBorder(int mode,int dirrection ,f32 x, f32 y, f32 z);
	void RenderHouseBorderDoor (int mode,int doorangle,int dirrection ,f32 x, f32 y, f32 z);
	void RenderHouseCorner (int mode,int dirrection ,f32 x, f32 y, f32 z);

//Bumpmapping
void LoadMBump3Texture(char filename[],int num,int* targetp,int* target,int* targetb,int* targetc);//used for walls only
void LoadMBump5Texture(char filename[],int num);//used for grounds only (no water and no autotiles!)

//Villagers
void ResetVillagers(void);
void AddDorfi(int x,int y,int texnum);
void AddTexttoDorfi(char txt[256]);
void UpdateDorfis();
#define Dorf_Max 5 //The max. count of NPCs

//Enemys
void ResetEnemys(void);
void AddEnemy(int x,int y,int texnum,int atp,int hp,float rad,bool boss);
void UpdateEnemy();

//Events
void ResetEvent(void);
void AddTexttoEvent(char txt[256]);
void CheckEndGame(void);

//Textbox
void TextBoxmode(char TxtOffset[10][256],int cnt,int num);//will be called if char talks to NPC or other things
void TextBoxmodeHandler(void);//This will put char by char the text to Dialogbox...
void ShownextPage(void);

//Items
void LoadItemList(void);
void ClearItems(void);//Clears all times....no ownership
bool ItemCheck_Set(char* Com);//It compares and gives back if matches or not/or sets a value...
const char* GiveItemName(int Index);
const char* GiveItemImgName(int Index);
const char* GiveItemSrcA(int Index);
const char* GiveItemSrcB(int Index);
const char* GiveItemType(int Index);

//Inventory
void ResetInventory(void);
void AddItemToInventory(u8 Index);
