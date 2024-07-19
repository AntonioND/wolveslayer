#include "GfxEngine/3D.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Model/MD2Collision.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Render/Autotiles.h"
#include "GfxEngine/Render/Ground.h"
#include "GfxEngine/Render/Wasser.h"
#include "GfxEngine/Script/Script_Objects.h"
#include "GfxEngine/Texture/DynamicLights.h"

u8 Precalcdata[128][128];

v16x4 Terrain[128][128];
v16 TerrainMid[128][128];

signed char TexGround[128][128];
signed char TexAGround[128][128];
signed char SetAGround[128][128];

signed char TexObj[128][128];
signed char ShapeObj[128][128];
signed char DirObj[128][128];

static void PreCalcGround(void)
{
    int choose, texturecounter;
    int choose2, texturecounter2;

    for (int yy = 0; yy < MapGetHr(); yy++) {
        for (int xx = 0; xx < MapGetWr(); xx++) {
            Precalcdata[xx][yy] = 0; // reset all stuff

            choose  = -3; // int value for choosing texture
            choose2 = -1;
            u32 Gnd = 0; // char[7] value for compare colorkeys
            u32 Obj = 0; // char[7] value for compare colorkeys

            // Ground
            if (xx >= 0 && yy >= 0)
                if (xx < MapGetWr() && yy < MapGetHr()) // Gnd hold now the hexavalue of current ground
                    Gnd = MapGroundGetRGB(xx, yy);

            // Objekt check (needed to determinate if there is a wall/house so we dont render ground)
            Obj = MapObjectGetRGB(xx, yy);
            for (texturecounter2 = 0; texturecounter2 < Object_Count; texturecounter2++)
                if (Obj == Objects[texturecounter2].ColorKey)
                    choose2 = texturecounter2;

            // Here we look which texture to use
            for (texturecounter = 0; texturecounter < Ground_Count; texturecounter++)
                if (Gnd == Ground[texturecounter].ColorKey)
                    choose = texturecounter;

            TexGround[xx][yy] = -2;
            if (choose != -3)
                TexGround[xx][yy] = choose;
            if (strncmp(Objects[choose2].Type, "HOUSE", 5) == 0)
                TexGround[xx][yy] = -1;
            if (strncmp(Objects[choose2].Type, "WALL", 4) == 0)
                TexGround[xx][yy] = -1;
            if (strncmp(Objects[choose2].Type, "BUMPWALL", 8) == 0)
                TexGround[xx][yy] = -1;
            if (MapGroundGetRGB(xx, yy) == (0 | BIT(15)) || MapGroundGetRGB(xx, yy) == 0)
                TexGround[xx][yy] = -1;
        }
    }
}

static void PreCalcWater(void)
{
    for (int yy = 0; yy < MapGetHr(); yy++) {
        for (int xx = 0; xx < MapGetWr(); xx++) {
            if (WasserKey != (0 | BIT(15)) && WasserKey != 0)
                if (MapGroundGetRGB(xx, yy) == WasserKey)
                    Precalcdata[xx][yy] |= (1 << water);
        }
    }
}

static void PreCalcAGround(void)
{
    u32 Gnd;
    bool UGnd, LGnd, RGnd, DGnd;
    bool ULGnd, DLGnd, URGnd, DRGnd;

    u32 Obj;
    int texturecounter = 0;
    int choose2 __attribute__((unused)), texturecounter2;

    for (int yy = 0; yy < MapGetHr(); yy++) {
        for (int xx = 0; xx < MapGetWr(); xx++) {
            choose2 = -1;

            Gnd = 0; // char[7] value for compare colorkeys
            Obj = 0; // char[7] value for compare colorkeys
            // Ground
            if (xx >= 0 && yy >= 0)
                if (xx < MapGetWr() && yy < MapGetHr()) // Gnd hold now the hexavalue of current ground
                    Gnd = MapGroundGetRGB(xx, yy);

            // Objekt check (needed to determinate if there is a wall so we dont render ground)
            Obj = MapObjectGetRGB(xx, yy);
            for (texturecounter2 = 0; texturecounter2 < Object_Count; texturecounter2++)
                if (Obj == Objects[texturecounter2].ColorKey)
                    choose2 = texturecounter2;

            TexAGround[xx][yy] = -1;
            SetAGround[xx][yy] = -1;

            // Here we look which texture to use
            for (texturecounter = 0; texturecounter < 4; texturecounter++) {
                if (Gnd == Autotile[texturecounter].ColorKeyMaster) {
                    SetAGround[xx][yy] = texturecounter;

                    LGnd  = false;
                    RGnd  = false;
                    UGnd  = false;
                    DGnd  = false;
                    ULGnd = false;
                    DLGnd = false;
                    URGnd = false;
                    DRGnd = false;

                    // Compare neighbours with Mastercolor
                    if (MapGroundGetRGB(xx - 1, yy) != Autotile[texturecounter].ColorKeyMaster)
                        LGnd = true;
                    if (MapGroundGetRGB(xx + 1, yy) != Autotile[texturecounter].ColorKeyMaster)
                        RGnd = true;
                    if (MapGroundGetRGB(xx, yy - 1) != Autotile[texturecounter].ColorKeyMaster)
                        UGnd = true;
                    if (MapGroundGetRGB(xx, yy + 1) != Autotile[texturecounter].ColorKeyMaster)
                        DGnd = true;
                    if (MapGroundGetRGB(xx - 1, yy - 1) != Autotile[texturecounter].ColorKeyMaster)
                        ULGnd = true;
                    if (MapGroundGetRGB(xx + 1, yy - 1) != Autotile[texturecounter].ColorKeyMaster)
                        URGnd = true;
                    if (MapGroundGetRGB(xx - 1, yy + 1) != Autotile[texturecounter].ColorKeyMaster)
                        DLGnd = true;
                    if (MapGroundGetRGB(xx + 1, yy + 1) != Autotile[texturecounter].ColorKeyMaster)
                        DRGnd = true;

                    // Compare neighbours with Ignore colors
                    for (int ign = 0; ign <= Autotile[texturecounter].IgnorecolorsNum; ign++) {
                        if (MapGroundGetRGB(xx - 1, yy) == Autotile[texturecounter].Ignorecolors[ign])
                            LGnd = false;
                        if (MapGroundGetRGB(xx + 1, yy) == Autotile[texturecounter].Ignorecolors[ign])
                            RGnd = false;
                        if (MapGroundGetRGB(xx, yy - 1) == Autotile[texturecounter].Ignorecolors[ign])
                            UGnd = false;
                        if (MapGroundGetRGB(xx, yy + 1) == Autotile[texturecounter].Ignorecolors[ign])
                            DGnd = false;
                        if (MapGroundGetRGB(xx - 1, yy - 1) == Autotile[texturecounter].Ignorecolors[ign])
                            ULGnd = false;
                        if (MapGroundGetRGB(xx + 1, yy - 1) == Autotile[texturecounter].Ignorecolors[ign])
                            URGnd = false;
                        if (MapGroundGetRGB(xx - 1, yy + 1) == Autotile[texturecounter].Ignorecolors[ign])
                            DLGnd = false;
                        if (MapGroundGetRGB(xx + 1, yy + 1) == Autotile[texturecounter].Ignorecolors[ign])
                            DRGnd = false;
                    }

                    // Tex alone
                    if ((LGnd == true) && (RGnd == true) && (UGnd == true) && (DGnd == true))
                        TexAGround[xx][yy] = 31;

                    // tex surounded others
                    if ((LGnd == false) && (RGnd == false) && (UGnd == false) && (DGnd == false)) {
                        if (ULGnd == false && URGnd == false && DLGnd == false && DRGnd == false)
                            TexAGround[xx][yy] = 9; // no diagonal oponents

                        if (ULGnd == true && URGnd == false && DLGnd == false && DRGnd == false)
                            TexAGround[xx][yy] = 16; // one oponent
                        if (ULGnd == false && URGnd == false && DLGnd == true && DRGnd == false)
                            TexAGround[xx][yy] = 20; // one oponent
                        if (ULGnd == false && URGnd == true && DLGnd == false && DRGnd == false)
                            TexAGround[xx][yy] = 24; // one oponent
                        if (ULGnd == false && URGnd == false && DLGnd == false && DRGnd == true)
                            TexAGround[xx][yy] = 28; // one oponent

                        if (ULGnd == true && URGnd == true && DLGnd == false && DRGnd == false)
                            TexAGround[xx][yy] = 17; // two oponents
                        if (ULGnd == false && URGnd == false && DLGnd == true && DRGnd == true)
                            TexAGround[xx][yy] = 21; // two oponents
                        if (ULGnd == false && URGnd == true && DLGnd == false && DRGnd == true)
                            TexAGround[xx][yy] = 25; // two oponents
                        if (ULGnd == true && URGnd == false && DLGnd == true && DRGnd == false)
                            TexAGround[xx][yy] = 29; // two oponents
                        if (ULGnd == false && URGnd == true && DLGnd == true && DRGnd == false)
                            TexAGround[xx][yy] = 18; // two oponents
                        if (ULGnd == true && URGnd == false && DLGnd == false && DRGnd == true)
                            TexAGround[xx][yy] = 22; // two oponents

                        if (ULGnd == true && URGnd == true && DLGnd == false && DRGnd == true)
                            TexAGround[xx][yy] = 26; // 3 oponents
                        if (ULGnd == true && URGnd == true && DLGnd == true && DRGnd == false)
                            TexAGround[xx][yy] = 30; // 3 oponents
                        if (ULGnd == false && URGnd == true && DLGnd == true && DRGnd == true)
                            TexAGround[xx][yy] = 19; // 3 oponents
                        if (ULGnd == true && URGnd == false && DLGnd == true && DRGnd == true)
                            TexAGround[xx][yy] = 23; // 3 oponents

                        if (ULGnd == true && URGnd == true && DLGnd == true && DRGnd == true)
                            TexAGround[xx][yy] = 27; // 4 oponents
                    }

                    // tex with border over it
                    if ((LGnd == false) && (RGnd == false) && (UGnd == true) && (DGnd == false)) {
                        if (DLGnd == false && DRGnd == false)
                            TexAGround[xx][yy] = 5;

                        if (DLGnd == false && DRGnd == true)
                            TexAGround[xx][yy] = 33;
                        if (DLGnd == true && DRGnd == false)
                            TexAGround[xx][yy] = 37;
                        if (DLGnd == true && DRGnd == true)
                            TexAGround[xx][yy] = 41;
                    }

                    // tex with border under it
                    if ((LGnd == false) && (RGnd == false) && (UGnd == false) && (DGnd == true)) {
                        if (ULGnd == false && URGnd == false)
                            TexAGround[xx][yy] = 13;

                        if (ULGnd == false && URGnd == true)
                            TexAGround[xx][yy] = 45;
                        if (ULGnd == true && URGnd == false)
                            TexAGround[xx][yy] = 34;
                        if (ULGnd == true && URGnd == true)
                            TexAGround[xx][yy] = 38;
                    }

                    // tex border on the right
                    if ((LGnd == true) && (RGnd == false) && (UGnd == false) && (DGnd == false)) {
                        if (URGnd == false && DRGnd == false)
                            TexAGround[xx][yy] = 8;

                        if (URGnd == true && DRGnd == false)
                            TexAGround[xx][yy] = 42;
                        if (URGnd == false && DRGnd == true)
                            TexAGround[xx][yy] = 46;
                        if (URGnd == true && DRGnd == true)
                            TexAGround[xx][yy] = 35;
                    }

                    // tex with border on the left
                    if ((LGnd == false) && (RGnd == true) && (UGnd == false) && (DGnd == false)) {
                        if (ULGnd == false && DLGnd == false)
                            TexAGround[xx][yy] = 10;

                        if (ULGnd == true && DLGnd == false)
                            TexAGround[xx][yy] = 39;
                        if (ULGnd == false && DLGnd == true)
                            TexAGround[xx][yy] = 43;
                        if (ULGnd == true && DLGnd == true)
                            TexAGround[xx][yy] = 47;
                    }

                    // tex with border up/left
                    if ((LGnd == true) && (RGnd == false) && (UGnd == true) && (DGnd == false)) {
                        if (DRGnd == false)
                            TexAGround[xx][yy] = 4;
                        else
                            TexAGround[xx][yy] = 32;
                    }

                    // tex with border up/right
                    if ((LGnd == false) && (RGnd == true) && (UGnd == true) && (DGnd == false)) {
                        if (DLGnd == false)
                            TexAGround[xx][yy] = 6;
                        else
                            TexAGround[xx][yy] = 36;
                    }

                    // tex with border down/left
                    if ((LGnd == true) && (RGnd == false) && (UGnd == false) && (DGnd == true)) {
                        if (URGnd == false)
                            TexAGround[xx][yy] = 12;
                        else
                            TexAGround[xx][yy] = 40;
                    }

                    // tex with border down/right
                    if ((LGnd == false) && (RGnd == true) && (UGnd == false) && (DGnd == true)) {
                        if (ULGnd == false)
                            TexAGround[xx][yy] = 14;
                        else
                            TexAGround[xx][yy] = 44;
                    }

                    // tex with border up/down
                    if ((LGnd == false) && (RGnd == false) && (UGnd == true) && (DGnd == true))
                        TexAGround[xx][yy] = 1;

                    // tex with border left/down/up
                    if ((LGnd == true) && (RGnd == false) && (UGnd == true) && (DGnd == true))
                        TexAGround[xx][yy] = 3;

                    // tex with border right/down/up
                    if ((LGnd == false) && (RGnd == true) && (UGnd == true) && (DGnd == true))
                        TexAGround[xx][yy] = 15;

                    // tex with border left/right
                    if ((LGnd == true) && (RGnd == true) && (UGnd == false) && (DGnd == false))
                        TexAGround[xx][yy] = 2;

                    // tex with border left/right/up
                    if ((LGnd == true) && (RGnd == true) && (UGnd == true) && (DGnd == false))
                        TexAGround[xx][yy] = 11;

                    // tex with border Left/right/down
                    if ((LGnd == true) && (RGnd == true) && (UGnd == false) && (DGnd == true))
                        TexAGround[xx][yy] = 7;

                    if (MapGroundGetRGB(xx, yy) == (0 | BIT(15)) || MapGroundGetRGB(xx, yy) == 0)
                        TexAGround[xx][yy] = -1;
                }
            }
        }
    }
}

static void PreCalcObj(void)
{
    u32 Obj;
    int texturecounter, choose;

    bool ObjD, ObjU, ObjL, ObjR;
    bool GndD, GndU, GndL, GndR;

    bool wanddoor = false;

    for (int yy = 0; yy < MapGetHr(); yy++) {
        for (int xx = 0; xx < MapGetWr(); xx++) {
            choose = -1;
            Obj    = 0; // char[7] value for compare colorkeys

            if (xx >= 0 && yy >= 0 && xx < MapGetWr() && yy < MapGetHr())
                Obj = MapObjectGetRGB(xx, yy);

            // Here we look which texture to use
            for (texturecounter = 0; texturecounter < Object_Count; texturecounter++)
                if (Obj == Objects[texturecounter].ColorKey)
                    choose = texturecounter;
            TexObj[xx][yy]   = choose;
            ShapeObj[xx][yy] = -1;
            DirObj[xx][yy]   = 0;

            //>>>>>>>>>>>>>>>>>>>>>>>>>>>
            // MD2
            if (choose != -1 && strncmp(Objects[choose].Type, "MODEL", 5) == 0) {
                ShapeObj[xx][yy] = Obj_MD2;
            }

            //>>>>>>>>>>>>>>>>>>>>>>>>>>
            // HouseParts
            if (choose != -1 && strncmp(Objects[choose].Type, "HOUSE", 5) == 0) {
                ObjD = IsObjHouse(xx, yy + 1);
                ObjU = IsObjHouse(xx, yy - 1);
                ObjL = IsObjHouse(xx - 1, yy);
                ObjR = IsObjHouse(xx + 1, yy);
                if (strncmp(GetMapDoor(xx, yy), ".", 1) == 0)
                    wanddoor = false;
                if (strncmp(GetMapDoor(xx, yy), ".", 1) != 0)
                    wanddoor = true;

                // Wand Mitte
                if (ObjD == true) {             // If one field under is same wall
                    if (ObjU == true) {         // If one field upper is same wall
                        if (ObjL == true) {     // If one field left is same wall
                            if (ObjR == true) { // If one field right is same wall
                                ShapeObj[xx][yy] = Obj_HouseMid;
                                DirObj[xx][yy]   = 0;
                            }
                        }
                    }
                }

                // WandRand unten
                if (ObjD == false) {            // If one field under is same wall
                    if (ObjU == true) {         // If one field upper is same wall
                        if (ObjL == true) {     // If one field left is same wall
                            if (ObjR == true) { // If one field right is same wall
                                ShapeObj[xx][yy] = Obj_HouseBor;
                                if (wanddoor)
                                    ShapeObj[xx][yy] = Obj_HouseDoor;
                                DirObj[xx][yy] = 0;
                            }
                        }
                    }
                }

                // WandRand oben
                if (ObjD == true) {             // If one field under is same wall
                    if (ObjU == false) {        // If one field upper is same wall
                        if (ObjL == true) {     // If one field left is same wall
                            if (ObjR == true) { // If one field right is same wall
                                ShapeObj[xx][yy] = Obj_HouseBor;
                                if (wanddoor)
                                    ShapeObj[xx][yy] = Obj_HouseDoor;
                                DirObj[xx][yy] = 2;
                            }
                        }
                    }
                }

                // WandRand links
                if (ObjD == true) {             // If one field under is same wall
                    if (ObjU == true) {         // If one field upper is same wall
                        if (ObjL == false) {    // If one field left is same wall
                            if (ObjR == true) { // If one field right is same wall
                                ShapeObj[xx][yy] = Obj_HouseBor;
                                if (wanddoor)
                                    ShapeObj[xx][yy] = Obj_HouseDoor;
                                DirObj[xx][yy] = 1;
                            }
                        }
                    }
                }

                // WandRand rechts
                if (ObjD == true) {              // If one field under is same wall
                    if (ObjU == true) {          // If one field upper is same wall
                        if (ObjL == true) {      // If one field left is same wall
                            if (ObjR == false) { // If one field right is same wall
                                ShapeObj[xx][yy] = Obj_HouseBor;
                                if (wanddoor)
                                    ShapeObj[xx][yy] = Obj_HouseDoor;
                                DirObj[xx][yy] = 3;
                            }
                        }
                    }
                }

                // WandRand oben-links (ecke)
                if (ObjD == true) {             // If one field under is same wall
                    if (ObjU == false) {        // If one field upper is same wall
                        if (ObjL == false) {    // If one field left is same wall
                            if (ObjR == true) { // If one field right is same wall
                                ShapeObj[xx][yy] = Obj_HouseCor;
                                DirObj[xx][yy]   = 1;
                            }
                        }
                    }
                }

                // WandRand oben-rechts (ecke)
                if (ObjD == true) {              // If one field under is same wall
                    if (ObjU == false) {         // If one field upper is same wall
                        if (ObjL == true) {      // If one field left is same wall
                            if (ObjR == false) { // If one field right is same wall
                                ShapeObj[xx][yy] = Obj_HouseCor;
                                DirObj[xx][yy]   = 2;
                            }
                        }
                    }
                }

                // WandRand unten-links (ecke)
                if (ObjD == false) {            // If one field under is same wall
                    if (ObjU == true) {         // If one field upper is same wall
                        if (ObjL == false) {    // If one field left is same wall
                            if (ObjR == true) { // If one field right is same wall
                                ShapeObj[xx][yy] = Obj_HouseCor;
                                DirObj[xx][yy]   = 0;
                            }
                        }
                    }
                }

                // WandRand unten-rechts (ecke)
                if (ObjD == false) {             // If one field under is same wall
                    if (ObjU == true) {          // If one field upper is same wall
                        if (ObjL == true) {      // If one field left is same wall
                            if (ObjR == false) { // If one field right is same wall
                                ShapeObj[xx][yy] = Obj_HouseCor;
                                DirObj[xx][yy]   = 3;
                            }
                        }
                    }
                }
            }

            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
            // WallParts
            if (choose != -1 && (strncmp(Objects[choose].Type, "WALL", 4) == 0 || strncmp(Objects[choose].Type, "BUMPWALL", 8) == 0)) {
                if (strncmp(GetMapDoor(xx, yy), ".", 1) == 0)
                    wanddoor = false;
                if (strncmp(GetMapDoor(xx, yy), ".", 1) != 0)
                    wanddoor = true;

                ObjD = false;
                ObjL = false;
                ObjR = false;
                ObjU = false;
                GndL = true;
                GndR = true;
                GndU = true;
                GndD = true;
                if (IsObjWall(xx, yy + 1) || IsObjBumpWall(xx, yy + 1))
                    ObjD = true;
                if (IsObjWall(xx, yy - 1) || IsObjBumpWall(xx, yy - 1))
                    ObjU = true;
                if (IsObjWall(xx - 1, yy) || IsObjBumpWall(xx - 1, yy))
                    ObjL = true;
                if (IsObjWall(xx + 1, yy) || IsObjBumpWall(xx + 1, yy))
                    ObjR = true;
                if (xx == 0)
                    GndL = false;
                if (yy == 0)
                    GndU = false;
                if (xx == MapGetWr() - 1)
                    GndR = false;
                if (yy == MapGetHr() - 1)
                    GndU = false;
                if (GndL)
                    if (TexGround[xx - 1][yy] == -1)
                        GndL = false;
                if (GndR)
                    if (TexGround[xx + 1][yy] == -1)
                        GndR = false;
                if (GndU)
                    if (TexGround[xx][yy - 1] == -1)
                        GndU = false;
                if (GndD)
                    if (TexGround[xx][yy + 1] == -1)
                        GndD = false;

                int objcnt = 0;
                if (ObjD)
                    objcnt++;
                if (ObjU)
                    objcnt++;
                if (ObjL)
                    objcnt++;
                if (ObjR)
                    objcnt++;

                if (strncmp(GetMapDoor(xx, yy), ".", 1) == 0)
                    wanddoor = false;
                if (strncmp(GetMapDoor(xx, yy), ".", 1) != 0)
                    wanddoor = true;

                // Wand alleine (Säule)
                if (objcnt == 0) {
                    ShapeObj[xx][yy] = Obj_WallAlone;
                    DirObj[xx][yy]   = 0;
                }

                // Wand mit 2 nachbarn (wallborder)
                if (objcnt == 2) {
                    if ((ObjL && ObjR) || (ObjU && ObjD))
                        ShapeObj[xx][yy] = Obj_WallBor;
                    if (wanddoor)
                        if ((ObjL && ObjR) || (ObjU && ObjD))
                            ShapeObj[xx][yy] = Obj_WallDoor;

                    if (ObjL && ObjR) {
                        // if (yy < MapGetHr() / 2)
                        //     DirObj[xx][yy] = 0; // wand nach rechts
                        if (!GndD)
                            DirObj[xx][yy] = 2;
                        if (!GndU)
                            DirObj[xx][yy] = 0;
                    }
                    if (ObjU && ObjD) {
                        // if (xx < MapGetWr() / 2)
                        //     DirObj[xx][yy] = 3; // wand nach rechts
                        // else
                        //     DirObj[xx][yy] = 1; // wand nach links
                        if (!GndR)
                            DirObj[xx][yy] = 1;
                        if (!GndL)
                            DirObj[xx][yy] = 3;
                    }
                }
            }
        }
    }
}

static void PrecalcTerrain(void)
{
    u8 r, g, b;

    u8 lr, lg, lb;
    u8 rr, rg, rb;
    u8 ur, ug, ub;
    u8 dr, dg, db;

    u8 ulr, ulg, ulb;
    u8 urr, urg, urb;
    u8 dlr, dlg, dlb;
    u8 drr, drg, drb;

    int x, y;

    // Precalc the generally terrain
    for (int yy = 0; yy <= MapGetHr(); yy++) {
        for (int xx = 0; xx <= MapGetWr(); xx++) {
            x = (xx * 3) + 2;
            y = (yy * 3) + 2;

            // current
            // sscanf(MapColors[x + ((y) * MapW)].w, "%02X%02X%02X", &r, &g, &b);
            GetRGBfromMap(x, y, r, g, b);

            // horizontal/vertikal
            if (xx > 0)
                GetRGBfromMap(x - 3, y, lr, lg, lb);
            else {
                lr = 0;
                lg = 0;
                lb = 0;
            }
            if (xx < MapGetWr())
                GetRGBfromMap(x + 3, y, rr, rg, rb);
            else {
                rr = 0;
                rg = 0;
                rb = 0;
            }
            if (yy > 0)
                GetRGBfromMap(x, y - 3, ur, ug, ub);
            else {
                ur = 0;
                ug = 0;
                ub = 0;
            }
            if (yy < MapGetHr())
                GetRGBfromMap(x, y + 3, dr, dg, db);
            else {
                dr = 0;
                dg = 0;
                db = 0;
            }

            // diagonal
            if (xx > 0 && yy > 0)
                GetRGBfromMap(x - 3, y - 3, ulr, ulg, ulb);
            else {
                ulr = 0;
                ulg = 0;
                ulb = 0;
            }
            if (yy > 0 && xx < MapGetWr())
                GetRGBfromMap(x + 3, y - 3, urr, urg, urb);
            else {
                urr = 0;
                urg = 0;
                urb = 0;
            }
            if (xx > 0 && yy < MapGetHr())
                GetRGBfromMap(x - 3, y + 3, dlr, dlg, dlb);
            else {
                dlr = 0;
                dlg = 0;
                dlb = 0;
            }
            if (xx < MapGetWr() && yy < MapGetHr())
                GetRGBfromMap(x + 3, y + 3, drr, drg, drb);
            else {
                drr = 0;
                drg = 0;
                drb = 0;
            }

            if (g == 0xB || b == 0xB) {
                Terrain[xx][yy].v[0] = floattov16(GetHight(xx, yy));
                Terrain[xx][yy].v[1] = floattov16(GetHight(xx, yy));
                Terrain[xx][yy].v[2] = floattov16(GetHight(xx, yy));
                Terrain[xx][yy].v[3] = floattov16(GetHight(xx, yy));
            } else {
                // Oneway-dirrectinal
                Terrain[xx][yy].v[0] = floattov16(GetHight(xx, yy));
                if (ug == 0xB || ub == 0xB)
                    if (floattov16(GetHight(xx, yy - 1)) > Terrain[xx][yy].v[0])
                        Terrain[xx][yy].v[0] = floattov16(GetHight(xx, yy - 1));
                if (lg == 0xB || lb == 0xB)
                    if (floattov16(GetHight(xx - 1, yy)) > Terrain[xx][yy].v[0])
                        Terrain[xx][yy].v[0] = floattov16(GetHight(xx - 1, yy));
                if (ulg == 0xB || ulb == 0xB)
                    if (floattov16(GetHight(xx - 1, yy - 1)) > Terrain[xx][yy].v[0])
                        Terrain[xx][yy].v[0] = floattov16(GetHight(xx - 1, yy - 1));

                Terrain[xx][yy].v[1] = floattov16(GetHight(xx + 1, yy));
                if (ug == 0xB || ub == 0xB)
                    if (floattov16(GetHight(xx, yy - 1)) > Terrain[xx][yy].v[1])
                        Terrain[xx][yy].v[1] = floattov16(GetHight(xx, yy - 1));
                if (rg == 0xB || rb == 0xB)
                    if (floattov16(GetHight(xx + 1, yy)) > Terrain[xx][yy].v[1])
                        Terrain[xx][yy].v[1] = floattov16(GetHight(xx + 1, yy));
                if (urg == 0xB || urb == 0xB)
                    if (floattov16(GetHight(xx + 1, yy - 1)) > Terrain[xx][yy].v[1])
                        Terrain[xx][yy].v[1] = floattov16(GetHight(xx + 1, yy - 1));

                Terrain[xx][yy].v[2] = floattov16(GetHight(xx, yy + 1));
                if (dg == 0xB || db == 0xB)
                    if (floattov16(GetHight(xx, yy + 1)) > Terrain[xx][yy].v[2])
                        Terrain[xx][yy].v[2] = floattov16(GetHight(xx, yy + 1));
                if (lg == 0xB || lb == 0xB)
                    if (floattov16(GetHight(xx - 1, yy)) > Terrain[xx][yy].v[2])
                        Terrain[xx][yy].v[2] = floattov16(GetHight(xx - 1, yy));
                if (dlg == 0xB || dlb == 0xB)
                    if (floattov16(GetHight(xx - 1, yy + 1)) > Terrain[xx][yy].v[2])
                        Terrain[xx][yy].v[2] = floattov16(GetHight(xx - 1, yy + 1));

                Terrain[xx][yy].v[3] = floattov16(GetHight(xx + 1, yy + 1));
                if (dg == 0xB || db == 0xB)
                    if (floattov16(GetHight(xx, yy + 1)) > Terrain[xx][yy].v[3])
                        Terrain[xx][yy].v[3] = floattov16(GetHight(xx, yy + 1));
                if (rg == 0xB || rb == 0xB)
                    if (floattov16(GetHight(xx + 1, yy)) > Terrain[xx][yy].v[3])
                        Terrain[xx][yy].v[3] = floattov16(GetHight(xx + 1, yy));
                if (drg == 0xB || drb == 0xB)
                    if (floattov16(GetHight(xx + 1, yy + 1)) > Terrain[xx][yy].v[3])
                        Terrain[xx][yy].v[3] = floattov16(GetHight(xx + 1, yy + 1));
            }
        }
    }

    // precalc the mid-y position of each terraintile (to place objects right)
    for (int yy = 0; yy <= MapGetHr(); yy++) {
        for (int xx = 0; xx <= MapGetWr(); xx++) {
            TerrainMid[xx][yy] = (Terrain[xx][yy].v[0] + Terrain[xx][yy].v[1] + Terrain[xx][yy].v[2] + Terrain[xx][yy].v[3]) / 4;
        }
    }

    // Precalc sidewalls of the terrain (appearing there where stairs meet stairs or stair mett terrain)
    for (int yy = 0; yy <= MapGetHr(); yy++) {
        for (int xx = 0; xx <= MapGetWr(); xx++) {
            Terrain[xx][yy].sidewalls = 0;

            // west-sidewall
            if (xx > 0) {
                // Nothpart
                if (Terrain[xx][yy].v[0] > Terrain[xx - 1][yy].v[1])
                    Terrain[xx][yy].sidewalls |= (1 << 0);
                // Southpart
                if (Terrain[xx][yy].v[2] > Terrain[xx - 1][yy].v[3])
                    Terrain[xx][yy].sidewalls |= (1 << 1);
            }

            // east-sidewall
            if (xx < MapGetWr()) {
                // Nothpart
                if (Terrain[xx][yy].v[1] > Terrain[xx + 1][yy].v[0])
                    Terrain[xx][yy].sidewalls |= (1 << 2);
                // Southpart
                if (Terrain[xx][yy].v[3] > Terrain[xx + 1][yy].v[2])
                    Terrain[xx][yy].sidewalls |= (1 << 3);
            }

            // south-sidewall
            if (yy > 0) {
                // westpart
                if (Terrain[xx][yy].v[2] > Terrain[xx][yy + 1].v[0])
                    Terrain[xx][yy].sidewalls |= (1 << 4);
                // eastpart
                if (Terrain[xx][yy].v[3] > Terrain[xx][yy + 1].v[1])
                    Terrain[xx][yy].sidewalls |= (1 << 5);
            }
        }
    }

    // Ok we got 2 bits left in the u8 sidewall
    // There is one thing which should be avoided...
    // terrain uses quads to render....sometime edges are turning on that thing
    // its better we take a decision for the edges instead of the nds
    // that needs to bits to decide between quad, 2 triangles with edge a,and 2 with edge b
    int vertnumh, vertnuml, vertnum;
    v16 height;
    v16 low;
    u8 decision;
    int vertcountl, vertcounth;
    for (int yy = 0; yy <= MapGetHr(); yy++) {
        for (int xx = 0; xx <= MapGetWr(); xx++) {
            vertnumh   = 0;
            vertnuml   = 0;
            vertcounth = 0;
            vertcountl = 0;
            height     = Terrain[xx][yy].v[0];
            low        = Terrain[xx][yy].v[0];
            // First we need to finded highest vertex of those 4 in order to decide
            if (Terrain[xx][yy].v[1] > height) {
                height   = Terrain[xx][yy].v[1];
                vertnumh = 1;
            }
            if (Terrain[xx][yy].v[2] > height) {
                height   = Terrain[xx][yy].v[2];
                vertnumh = 2;
            }
            if (Terrain[xx][yy].v[3] > height) {
                height   = Terrain[xx][yy].v[3];
                vertnumh = 3;
            }

            if (Terrain[xx][yy].v[1] < low) {
                low      = Terrain[xx][yy].v[1];
                vertnuml = 1;
            }
            if (Terrain[xx][yy].v[2] < low) {
                low      = Terrain[xx][yy].v[2];
                vertnuml = 2;
            }
            if (Terrain[xx][yy].v[3] < low) {
                low      = Terrain[xx][yy].v[3];
                vertnuml = 3;
            }
            decision = 0;

            if (Terrain[xx][yy].v[0] > TerrainMid[xx][yy])
                vertcounth++;
            if (Terrain[xx][yy].v[1] > TerrainMid[xx][yy])
                vertcounth++;
            if (Terrain[xx][yy].v[2] > TerrainMid[xx][yy])
                vertcounth++;
            if (Terrain[xx][yy].v[3] > TerrainMid[xx][yy])
                vertcounth++;
            if (Terrain[xx][yy].v[0] < TerrainMid[xx][yy])
                vertcountl++;
            if (Terrain[xx][yy].v[1] < TerrainMid[xx][yy])
                vertcountl++;
            if (Terrain[xx][yy].v[2] < TerrainMid[xx][yy])
                vertcountl++;
            if (Terrain[xx][yy].v[3] < TerrainMid[xx][yy])
                vertcountl++;

            if (vertcounth < vertcountl)
                vertnum = vertnumh;
            else
                vertnum = vertnuml;
            if (vertnum == 0 || vertnum == 3)
                decision = 1; // 2 triangles style a
            if (vertnum == 1 || vertnum == 2)
                decision = 2; // 2 triangles style b

            // Ok here we decide...if there is a neighbour with same height we use quad to render this
            if (Terrain[xx][yy].v[0] == Terrain[xx][yy].v[1] && Terrain[xx][yy].v[2] == Terrain[xx][yy].v[3])
                decision = 0;
            if (Terrain[xx][yy].v[0] == Terrain[xx][yy].v[2] && Terrain[xx][yy].v[1] == Terrain[xx][yy].v[3])
                decision = 0;

            // save decision
            if (decision == 1)
                Terrain[xx][yy].sidewalls |= (1 << 6);
            if (decision == 2)
                Terrain[xx][yy].sidewalls |= (1 << 7);
        }
    }
}

static void PrecalcMirrow(void)
{
    for (int yy = 0; yy <= MapGetHr(); yy++) {
        for (int xx = 0; xx <= MapGetWr(); xx++) {
            // Precalcdata[xx][yy] |= (0 << mirrow);

            for (int mx = xx - 1; mx <= xx + 1; mx++) {
                for (int my = yy; my <= yy + 3; my++) {
                    if (mx >= 0 && my >= 0) {
                        if (TexGround[mx][my] > -1)
                            if (Ground[TexGround[mx][my]].TransEnable && GetHight(mx, my) == 0)
                                Precalcdata[xx][yy] |= (1 << mirrow);

                        if (Precalcdata[xx][yy] & (1 << water))
                            Precalcdata[xx][yy] |= (1 << mirrow);
                    }
                }
            }
        }
    }
}

static u16 Lightu16(int x, int y, int num)
{
    if (num == 1)
        x++;
    if (num == 2)
        y++;
    if (num == 3) {
        x++;
        y++;
    }

    if (x < 0 || x > MapGetWr())
        return 0;
    if (y < 0 || y > MapGetHr())
        return 0;

    return RGB15((u8)WorldLightR[x + (y * 128)] >> 3, (u8)WorldLightG[x + (y * 128)] >> 3, (u8)WorldLightB[x + (y * 128)] >> 3);
}

static void PrecalcStaticBump(void)
{
    for (int yy = 0; yy <= MapGetHr(); yy++) {
        for (int xx = 0; xx <= MapGetWr(); xx++) {
            // left-up
            if (Lightu16(xx, yy, 0) != Lightu16(xx, yy - 1, 0))
                Precalcdata[xx][yy] |= (1 << BumpwallW);
            if (Lightu16(xx, yy, 0) != Lightu16(xx - 1, yy, 0))
                Precalcdata[xx][yy] |= (1 << BumpgroundW);
            if (Lightu16(xx, yy, 0) != Lightu16(xx, yy + 1, 0)) {
                Precalcdata[xx][yy] |= (1 << BumpgroundS);
                Precalcdata[xx][yy] |= (1 << BumpwallW);
            }
            if (Lightu16(xx, yy, 0) != Lightu16(xx + 1, yy, 0))
                Precalcdata[xx][yy] |= (1 << BumpgroundE);

            // right-up
            if (Lightu16(xx, yy, 1) != Lightu16(xx, yy - 1, 1))
                Precalcdata[xx][yy] |= (1 << BumpwallE);
            if (Lightu16(xx, yy, 1) != Lightu16(xx - 1, yy, 1))
                Precalcdata[xx][yy] |= (1 << BumpgroundW);
            if (Lightu16(xx, yy, 1) != Lightu16(xx, yy + 1, 1)) {
                Precalcdata[xx][yy] |= (1 << BumpgroundS);
                Precalcdata[xx][yy] |= (1 << BumpwallE);
            }
            if (Lightu16(xx, yy, 1) != Lightu16(xx + 1, yy, 1))
                Precalcdata[xx][yy] |= (1 << BumpgroundE);

            // left-down
            if (Lightu16(xx, yy, 2) != Lightu16(xx, yy - 1, 2))
                Precalcdata[xx][yy] |= (1 << BumpwallW);
            if (Lightu16(xx, yy, 2) != Lightu16(xx - 1, yy, 2)) {
                Precalcdata[xx][yy] |= (1 << BumpgroundW);
                Precalcdata[xx][yy] |= (1 << BumpwallS);
            }
            if (Lightu16(xx, yy, 2) != Lightu16(xx, yy + 1, 2)) {
                Precalcdata[xx][yy] |= (1 << BumpgroundS);
                Precalcdata[xx][yy] |= (1 << BumpwallW);
            }
            if (Lightu16(xx, yy, 2) != Lightu16(xx + 1, yy, 2)) {
                Precalcdata[xx][yy] |= (1 << BumpgroundE);
                Precalcdata[xx][yy] |= (1 << BumpwallS);
            }

            // right-down
            if (Lightu16(xx, yy, 3) != Lightu16(xx, yy - 1, 3))
                Precalcdata[xx][yy] |= (1 << BumpwallE);
            if (Lightu16(xx, yy, 3) != Lightu16(xx - 1, yy, 3)) {
                Precalcdata[xx][yy] |= (1 << BumpgroundW);
                Precalcdata[xx][yy] |= (1 << BumpwallS);
            }
            if (Lightu16(xx, yy, 3) != Lightu16(xx, yy + 1, 3)) {
                Precalcdata[xx][yy] |= (1 << BumpgroundS);
                Precalcdata[xx][yy] |= (1 << BumpwallE);
            }
            if (Lightu16(xx, yy, 3) != Lightu16(xx + 1, yy, 3)) {
                Precalcdata[xx][yy] |= (1 << BumpgroundE);
                Precalcdata[xx][yy] |= (1 << BumpwallS);
            }
        }
    }
}

bool Passable(int x, int y, int sxx, int syy)
{
    if (sxx >= 21) {
        sxx -= 21;
        x++;
    }
    if (syy >= 21) {
        syy -= 21;
        y++;
    }
    if (sxx < 0) {
        sxx += 21;
        x--;
    }
    if (syy < 0) {
        syy += 21;
        y--;
    }
    if (sxx >= 21) {
        sxx -= 21;
        x++;
    }
    if (syy >= 21) {
        syy -= 21;
        y++;
    }
    if (sxx < 0) {
        sxx += 21;
        x--;
    }
    if (syy < 0) {
        syy += 21;
        y--;
    }

    if (x > MapGetWr() || y > MapGetHr())
        return false;

    int tex_obj = TexObj[x][y];
    if (tex_obj == -1)
        return true;

    const char *type = Objects[tex_obj].Type;

    if (strncmp(type, "MODEL", 5) == 0 && Objects[tex_obj].Radius != 0)
        return GetModelCollsisionXY(tex_obj + 10, sxx, syy, DirObj[x][y]);

    if (strncmp(type, "WALL", 4) == 0)
        return false;
    if (strncmp(type, "HOUSE", 5) == 0)
        return false;
    if (strncmp(type, "BUMPWALL", 8) == 0)
        return false;

    return true;
}

static void PrecalcObjectRot(void)
{
    // DirObj[128][128];
    // MapObjGetRot(int x, int y)

    for (int yy = 0; yy <= MapGetHr(); yy++) {
        for (int xx = 0; xx <= MapGetWr(); xx++) {
            const char *type = Objects[TexObj[xx][yy]].Type;

            if (strncmp(type, "MODEL", 5) == 0) {
                DirObj[xx][yy] = MapObjGetRot(xx, yy);
            }
        }
    }
}

void PreCalcAll(void)
{
    PreCalcGround();
    PreCalcAGround();
    PreCalcObj();
    PreCalcWater();
    PrecalcTerrain();
    PrecalcMirrow();
    PrecalcStaticBump();
    PrecalcObjectRot();
}
