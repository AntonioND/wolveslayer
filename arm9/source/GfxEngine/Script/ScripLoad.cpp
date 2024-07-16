#include "Script.h"

uint16 keyPress2;

bool LOADINGNOw;

extern bool outside;

extern int Door[1];
extern float PlPosSX, PlPosSY;
extern float CamPosSX, CamPosSY;

extern void LoadChars(TiXmlElement *map);

extern void LoadTextureCommand(TiXmlElement *map);

extern void LoadBodenCommand(TiXmlElement *map);
extern void LoadObjectCommand(TiXmlElement *map);
extern void LoadAutoBodenCommand(TiXmlElement *map);
extern void LoadWaterCommand(TiXmlElement *map);
extern void LoadTerrainBorderCommand(TiXmlElement *map);

extern void LoadMapChangeCommand(TiXmlElement *map);
extern void LoadDoorCommand(TiXmlElement *map);

extern void LoadDynamicLightCommand(TiXmlElement *map);

void ucase(char character[256], char *New)
{
    strcpy(New, character);

    for (u32 a = 0; a < strlen(character); a++) {
        if ((character[a] > 0x60) && (character[a] < 0x7B))
            New[a] = character[a] - 0x20;
    }
}

char SongFile[40];

void UnloadStuff(void)
{
    // Reset all datas
    ResetMapChange();
    glResetTextures();
    ResetVillagers();
    ResetEnemys();
    ResetEvent();

    extern u32 AutotileColorKeyMaster[4];
    extern int AutotileIgnorecolorsNum[4];
    extern u32 BodenColorKey[Ground_Count];
    extern u32 ObjektColorKey[Object_Count];
    extern char ObjektTyp[Object_Count][13];
    extern float ObjectRadius[Object_Count];
    extern bool BodenTransEnable[Ground_Count];
    extern bool BodenBumpEnable[Ground_Count];

    int a;
    for (a = 0; a < Ground_Count; a++)
        BodenColorKey[a] = 0;
    for (a = 0; a < Ground_Count; a++)
        BodenTransEnable[a] = false;
    for (a = 0; a < Ground_Count; a++)
        BodenBumpEnable[a] = false;
    for (a = 0; a < 4; a++) {
        AutotileColorKeyMaster[a]  = 0;
        AutotileIgnorecolorsNum[a] = -1;
    }
    for (a = 0; a < Object_Count; a++)
        ObjektColorKey[a] = 0;
    for (a = 0; a < Object_Count; a++)
        strcpy(ObjektTyp[a], ".");
    for (a = 0; a < Object_Count; a++)
        ObjectRadius[a] = 0;
    FreeModels();
    ResetDynamic();

    PlPosSX  = 0;
    PlPosSY  = 0;
    CamPosSX = 0;
    CamPosSY = 0;
    outside  = true;
    // srand (-(~IPC->curtime[7]) - 1);
}

void LoadScrip(char filename[])
{
    irqSet(IRQ_VBLANK, NULL);
    ScreenModeLOADING();
    UnloadStuff();
    ResetCounter();  // for percentbar
    UpdateCounter(); // for percentbar

    strcpy(SongFile, "");

    // erst schaun ob alles da is
    FILE *xml = fopen(filename, "r");
    if (xml == NULL) {
        // Fatal error, cannot load
        Crash("Script not found:\n%s", filename);
    }
    fclose(xml);

    // hier muss es abgehn...
    TiXmlDocument *xmlDoc = new TiXmlDocument(filename);
    if (!xmlDoc->LoadFile()) {
        // Fatal error, cannot load
        Crash("Script invalid:\n%s", filename);
    }

    // search for <map> </map>
    TiXmlElement *map = xmlDoc->FirstChildElement("map");
    if (!map)
        Crash("Tag called 'map' not found:\n%s", filename);

    // print atributes if there are some
    // mapfile (bmp)
    if (map->Attribute("file")) {
        char FileNameCom[60];
        snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/maps/%s", map->Attribute("file"));
        LoadMap(FileNameCom);
    } else {
        Crash("->No Mapfile specified:\n%s", filename);
    }

    // song
    if (map->Attribute("music")) {
        snprintf(SongFile, sizeof(SongFile), "/wolveslayer/bgfx/%s", map->Attribute("music"));
    } else {
        Crash("->No Musikfile specified:\n%s", filename);
    }

    // environment
    if (map->Attribute("env")) {
        if (strncmp("outside", map->Attribute("env"), 7) == 0)
            outside = true;
        if (strncmp("inside", map->Attribute("env"), 6) == 0)
            outside = false;
    } else {
        Crash("->No environment specified:\n%s", filename);
    }

    // load the grounds
    LoadBodenCommand(map);
    LoadAutoBodenCommand(map);
    LoadWaterCommand(map);
    LoadTerrainBorderCommand(map);
    // load textures for objects
    LoadTextureCommand(map);
    // load objects
    LoadObjectCommand(map);
    // load mapchanges
    LoadMapChangeCommand(map);
    LoadDoorCommand(map);
    // load dynamiclights
    LoadDynamicLightCommand(map);

    // load datas about npcs and player (definetions,meshes,textures)
    LoadChars(map);

    StartSong(SongFile);

    xmlDoc->Clear();

    PreCalcAll();
    ScreenMode();

    // extern void ShowCollisionMap(int modelnum);
    // ShowCollisionMap(12);

    extern char EventSpeech[10][256]; // to hold 10 textes for event with 256 chars
    extern int EventSpeechCount;      // Holds the Number of speeches a event has...
    TextBoxmode(EventSpeech, EventSpeechCount, -1);
    irqSet(IRQ_VBLANK, vBlank);
}

void LoadMapChange(char *Com)
{
    // scan out Map-FileName
    char FileName[40];
    sscanf(Com, "%*s %*i %*i %39s", FileName);

    // scan out positions
    int x = 0, y = 0;
    sscanf(Com, "%*s %i %i", &x, &y);

    // scan out GotTOpositions
    int tox = 0, toy = 0;
    sscanf(Com, "%*s %*i %*i %*s %i %i", &tox, &toy);

    // Make a hole path with that
    char FileNameCom[60];
    snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/maps/%s", FileName);

    // Here is that final call add the exit
    AddMapChange(x, y, FileNameCom, tox, toy);
}

void LoadMapChangeDoor(char *Com)
{
    // Example:
    //   MapChangeDoor Key0 25 5 Map2.txt 1 6

    // scan out Map-FileName
    char FileName[40];
    sscanf(Com, "%*s KEY%*i %*i %*i %39s %*i %*i", FileName);

    // scan out Keynumber
    int Keynum;
    sscanf(Com, "%*s KEY%i %*i %*i %*s %*i %*i", &Keynum);
    if (Keynum > 100 || Keynum < 0)
        Keynum = 0;

    // scan out positions
    int x = 0, y = 0;
    sscanf(Com, "%*s KEY%*i %i %i %*s %*i %*i", &x, &y);

    // scan out GotTOpositions
    int tox = 0, toy = 0;
    sscanf(Com, "%*s KEY%*i %*i %*i %*s %i %i", &tox, &toy);

    // Make a hole path with that
    char FileNameCom[60];
    snprintf(FileNameCom, sizeof(FileNameCom), "/wolveslayer/maps/%s", FileName);

    // Here is that final call add the exit
    AddMapDoor(x, y, FileNameCom, tox, toy, Keynum);
}

void AddEvent(char *com)
{
    // If it is 0 we know that AddText was called after Loading a NPC
    char *pch;
    pch = strtok(com, " ");   // First was command "^^addtext"
    pch = strtok(NULL, "\""); // next is a "

    AddTexttoEvent(pch);
}

void AddDoorTXT(char *com)
{
    // If it is 0 we know that AddText was called after Loading a NPC
    char *pch;
    pch = strtok(com, " ");   // First was command "^^addtext"
    pch = strtok(NULL, "\""); // next is a "

    AddTexttoDoor(pch);
}
