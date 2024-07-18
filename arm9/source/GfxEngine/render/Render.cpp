#include "GfxEngine/3D.h"
#include "GfxEngine/input/Input.h"
#include "GfxEngine/render/Boden.h"
#include "GfxEngine/Tackt.h"
#include "GfxEngine/Villagers.h"

extern uint16 keyPress;

// Texturen zum anzeigen
extern int AutotileTextur[4][3];
extern char AutotileColorKeyA[4][7];
extern char AutotileColorKeyB[4][7];
extern int Wasser[1];
extern char WasserKey[7];
extern int Figuren[10];
extern int ObjektTex[Object_Count];
extern char ObjektTyp[Object_Count][13];
extern char ObjektColorKey[Object_Count][7];
extern bool ObjectCulling[Object_Count];
extern int Ufer[10];

// MapDatas
extern signed char TexBod[128][128];
extern signed char TexABod[128][128];
extern signed char SetABod[128][128];

extern signed char TexObj[128][128];
extern signed char ShapeObj[128][128];
extern signed char DirObj[128][128];

void RefreshWelt(void);
void RefreshWeltMirrow(void);

f32 waterheight = floattof32(-.1);

float GetInterPolY(int x, int y, float sx, float sy)
{
    if (x < 1)
        x = 0;
    if (x > MapGetWr())
        x = MapGetWr();
    if (y < 1)
        x = 0;
    if (y > MapGetHr())
        y = MapGetHr();

    float hpos1 = (f32tofloat(GetTerrain(x, y, 1)) * sx) + (f32tofloat(GetTerrain(x, y, 0)) * (1 - sx));
    float hpos2 = (f32tofloat(GetTerrain(x, y, 3)) * sx) + (f32tofloat(GetTerrain(x, y, 2)) * (1 - sx));
    float hpos  = (hpos2 * sy) + (hpos1 * (1 - sy));

    return hpos;
}

// Boden,Autotiles,Wasser  & Terrainseitenwände
typedef struct {
    v16 v[4];
    u8 sidewalls;
} v16x4;

void RefreshMap(void)
{
    // RefreshWelt();
    // return;
    int x = 0, y = 0;
    extern u8 Precalcdata[128][128];

    extern bool ObjectNoGround[Object_Count];
    extern v16x4 Terrain[128][128];
    int Wallbor = Obj_WallBor;

    int subset;
    int minx, maxx;

    for (int yy = 11; yy > -2; yy--) {
        minx = 5;
        maxx = 6;
        if (yy == 11 || yy == 10 || yy == 9) {
            minx = 2;
            maxx = 9;
        }
        if (yy == 8) {
            minx = 1;
            maxx = 9;
        }
        if (yy == 7 || yy == 6 || yy == 5) {
            minx = 1;
            maxx = 10;
        }
        if (yy == 4 || yy == 3 || yy == 2) {
            minx = 0;
            maxx = 11;
        }
        if (yy == 1 || yy == 0 || yy == -1) {
            minx = -1;
            maxx = 12;
        }
        if (yy <= -2) {
            minx = -2;
            maxx = 13;
        }

        for (int xx = maxx; xx > minx; xx--) {
            x = xx + CamPosX;
            y = yy + CamPosY;
            if (ObjectNoGround[TexObj[x][y]] == false) {
                SetCurBod(x, y); // We tell the renderengine where the boden is to set up maplight

                // Levelrand
                if ((TexBod[x][y] <= -1 && TexABod[x][y] <= -1 && !(Precalcdata[x][y] & (1 << water))) || (ShapeObj[x][y] == Wallbor)) {
                    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));
                    glBindTexture(GL_TEXTURE_2D, BodenTexture[0]);
                    RenderLevelBorderBoden(xx - 6, 0, yy - 8);
                }

                if (x < MapGetWr() && y < MapGetHr() && ShapeObj[x][y] != Wallbor) {
                    if (x > -1 && y > -1) {
                        // Boden
                        if (TexBod[x][y] >= 0 && SetABod[x][y] < 0) {
                            if (TexBod[x][y] >= 0)
                                RenderBoden(xx - 6, 0, yy - 8, TexBod[x][y]);
                        }

                        // AutoBoden
                        if (TexABod[x][y] != -2 && SetABod[x][y] >= 0 && SetABod[x][y] <= 3 && TexBod[x][y] == -2 && ShapeObj[x][y] != Wallbor) {
                            subset = 0;
                            if (TexABod[x][y] > 15)
                                subset = 1;
                            if (TexABod[x][y] > 31)
                                subset = 2;
                            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
                            glBindTexture(GL_TEXTURE_2D, AutotileTextur[SetABod[x][y]][subset]);

                            RenderAutoBoden(xx - 6, 0, yy - 8, TexABod[x][y] - (subset * 16));
                        }

                        // Wasser
                        if ((Precalcdata[x][y] & (1 << water)) && ShapeObj[x][y] != Wallbor) {
                            glBindTexture(GL_TEXTURE_2D, Wasser[0]);
                            RenderWasser(tackt32, inttof32(xx - 6), waterheight, inttof32(yy - 8));
                        }

                        // Ufer
                        if (Terrain[x][y].sidewalls) {
                            RenderUfer(x, y, inttof32(xx - 6), inttof32(yy - 8));
                        }
                    }
                }
            }
        }
    }
}

//*******************************************************************************
// Welt
void RefreshWelt(void)
{
    extern v16 TerrainMid[128][128];
    // extern bool Mirrow[128][128];
    extern u8 Precalcdata[128][128];

    extern bool ObjectMirrow[Object_Count];
    extern int ObjectTextureID[Object_Count];
    extern bool ObjectIllumination[Object_Count];

    int x = 0, y = 0;

    int model     = Obj_MD2;
    int Housem    = Obj_HouseMid;
    int Housebor  = Obj_HouseBor;
    int Housecor  = Obj_HouseCor;
    int Housedoor = Obj_HouseDoor;

    int Wallbor = Obj_WallBor;
    // int WallAlone=Obj_WallAlone;
    int WallDoor = Obj_WallDoor;
    bool seeable;
    int minx, maxx;

    SetRot(0);

    // float spos[2];
    int pos[2];
    // spos[0] = 1;
    // spos[1] = .5;

    v16 height;
    f32 posx, posy;

    for (int yy = 12; yy > -4; yy--) {
        y = yy + CamPosY;
        if (yy == 12) {
            minx = 3;
            maxx = 8;
        }
        if (yy == 11 || yy == 10 || yy == 9) {
            minx = 2;
            maxx = 9;
        }
        if (yy == 8) {
            minx = 1;
            maxx = 9;
        }
        if (yy == 7 || yy == 6 || yy == 5) {
            minx = 1;
            maxx = 10;
        }
        if (yy == 4 || yy == 3 || yy == 2) {
            minx = 0;
            maxx = 11;
        }
        if (yy == 1 || yy == 0 || yy == -1) {
            minx = -1;
            maxx = 12;
        }
        if (yy <= -2) {
            minx = -2;
            maxx = 13;
        }

        for (int xx = maxx; xx > minx; xx--) {
            x = xx + CamPosX;

            seeable = true;
            if (x >= MapGetWr() || y >= MapGetHr())
                seeable = false;
            if (x < 0 || y < 0)
                seeable = false;

            if (seeable) {
                if (TexObj[x][y] >= 0) {
                    glBindTexture(GL_TEXTURE_2D, ObjektTex[ObjectTextureID[TexObj[x][y]]]);

                    height = TerrainMid[x][y];
                    posx   = inttof32(xx - 6);
                    posy   = inttof32(yy - 8);

                    // Models
                    if (ShapeObj[x][y] == model) {
                        pos[0] = x;
                        pos[1] = y;

                        // SetMdlLights(pos,spos,.4,0);   DirObj[x][y]
                        if (yy > -1 || ((Precalcdata[x][y] & (1 << mirrow)) && ObjectMirrow[TexObj[x][y]] == true)) {
                            if (!ObjectIllumination[TexObj[x][y]])
                                SetObjLights(pos, DirObj[x][y] * 64); // way faster and enough for objects
                            else
                                SetObjLightsSelfilluminated();
                            Precalcmd2light(stackt11, TexObj[x][y] + 10); // mesh1 lights
                        }
                        if (yy > -1) {
                            // Mesh
                            glPushMatrix();
                            if (ObjectCulling[TexObj[x][y]])
                                glPolyFmt(POLY_ALPHA(31) | POLY_CULL_FRONT | POLY_FORMAT_LIGHT0 | POLY_ID(ObjectTextureID[TexObj[x][y]] + 11 + 3));
                            else
                                glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(ObjectTextureID[TexObj[x][y]] + 11 + 3));
                            glTranslatef32(posx, height, posy);
                            glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
                            glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + (DirObj[x][y] * -64)));
                            RenderMD2Model(stackt11, TexObj[x][y] + 10);
                            glPopMatrix(1);
                        }
                        // Same for mirrowing
                        if ((Precalcdata[x][y] & (1 << mirrow)) && ObjectMirrow[TexObj[x][y]] == true) {
                            glPushMatrix();
                            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(ObjectTextureID[TexObj[x][y]] + 11 + 3));
                            glTranslatef32(posx, -height, posy);
                            glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
                            glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + (DirObj[x][y] * -64)));
                            RenderMD2ModelMirrowed(stackt11, TexObj[x][y] + 10);
                            glPopMatrix(1);
                        }
                    }

                    // House middle
                    if (ShapeObj[x][y] == Housem) {
                        SetCurWall(xx + CamPosX, yy + CamPosY);
                        if (yy > -1)
                            RenderHouseM(0, posx, floattof32(1.0 + GetHight(x, y)), posy); // Normal
                    }

                    // House border
                    if (ShapeObj[x][y] == Housebor) {
                        SetCurWall(xx + CamPosX, yy + CamPosY);
                        if (yy > -1)
                            RenderHouseBorder(0, DirObj[x][y], posx, floattof32(1.0 + GetHight(x, y)), posy); // Normal
                        if ((Precalcdata[x][y] & (1 << mirrow)))
                            RenderHouseBorder(1, DirObj[x][y], posx, floattof32(-.9 - GetHight(x, y)), posy); // Gespiegelt
                    }

                    // House corner
                    if (ShapeObj[x][y] == Housecor) {
                        SetCurWall(xx + CamPosX, yy + CamPosY);
                        if (yy > -1)
                            RenderHouseCorner(0, DirObj[x][y], posx, floattof32(1.0 + GetHight(x, y)), posy); // Normal
                        if ((Precalcdata[x][y] & (1 << mirrow)))
                            RenderHouseCorner(1, DirObj[x][y], posx, floattof32(-.9 - GetHight(x, y)), posy); // Gespiegelt
                    }

                    // House Border Door
                    if (ShapeObj[x][y] == Housedoor) {
                        SetCurWall(xx + CamPosX, yy + CamPosY);
                        if (yy > -1)
                            RenderHouseBorderDoor(0, GetMapDoorAngle(xx + CamPosX, yy + CamPosY), DirObj[x][y], posx,
                                                  floattof32(1.0 + GetHight(x, y)), posy); // Türslot Normal
                        if ((Precalcdata[x][y] & (1 << mirrow))) {
                            glBindTexture(GL_TEXTURE_2D,
                                          ObjektTex[TexObj[x][y]]); // We need that to set again...after rendering houspart with doorslot it
                            RenderHouseBorderDoor(1, GetMapDoorAngle(xx + CamPosX, yy + CamPosY), DirObj[x][y], posx,
                                                  floattof32(-.9 - GetHight(x, y)), posy); // Türslot Gespiegelt
                        }
                    }

                    // wallParts...finaly
                    // Wall border
                    // if (IsObjBumpWall(x, y) == true)
                    //     glBindTexture(GL_TEXTURE_2D, ObjektTexA[ObjectTextureID[TexObj[x][y]]]);

                    if (ShapeObj[x][y] == Wallbor) {
                        SetCurWall(xx + CamPosX, yy + CamPosY);
                        if (yy > -1)
                            RenderWallBorder(0, DirObj[x][y], posx, floattof32(1.0 + GetHight(x, y)), posy, TexObj[x][y]); // Normal
                        if ((Precalcdata[x][y] & (1 << mirrow)))
                            RenderWallBorder(1, DirObj[x][y], posx, floattof32(-1 - GetHight(x, y)), posy, TexObj[x][y]); // Gespiegelt
                    }

                    // Wall Border Door
                    if (ShapeObj[x][y] == WallDoor) {
                        SetCurWall(xx + CamPosX, yy + CamPosY);
                        if (yy > -1)
                            RenderWallBorderDoor(0, GetMapDoorAngle(xx + CamPosX, yy + CamPosY), DirObj[x][y], posx, floattof32(1 + GetHight(x, y)),
                                                 posy); // Türslot Normal
                        if ((Precalcdata[x][y] & (1 << mirrow))) {
                            glBindTexture(GL_TEXTURE_2D,
                                          ObjektTex[TexObj[x][y]]); // We need that to set again...after rendering houspart with doorslot it
                            RenderWallBorderDoor(1, GetMapDoorAngle(xx + CamPosX, yy + CamPosY), DirObj[x][y], posx,
                                                 floattof32(-1.02 - GetHight(x, y)), posy); // Türslot Gespiegelt
                        }
                    }
                }
            }
        }
    }
}

//*******************************************************************************
// Spieler
void RefreshPlayer(void)
{
    extern u8 Precalcdata[128][128];
    extern int screenmode;
    extern int GegnerCount;
    // extern int EquipedWeapon;
    float sx = PlPosSX + .5;
    float sy = PlPosSY + .5;

    int pos[2];
    pos[0] = GetPX();
    pos[1] = GetPY();
    float spos[2];
    spos[0] = sx;
    spos[1] = sy;
    SetMdlLights(pos, spos, .4, PlRichtung * 64);

    glBindTexture(GL_TEXTURE_2D, Figuren[0]);

    float hpos = GetInterPolY(GetPX(), GetPY(), sx, sy);
    PlHeight   = hpos;

    // walk/standing
    int aniwaffe = stackt11;
    if (PlStatus == 0) {
        if (GegnerCount >= 0)
            aniwaffe = 22 + stackt11;
        if (screenmode < 2 && ((keysHeld() & KEY_DOWN) || (keysHeld() & KEY_UP) || (keysHeld() & KEY_LEFT) || (keysHeld() & KEY_RIGHT)))
            aniwaffe += 11;
    }

    // Attacking
    if (PlStatus == 1) {
        aniwaffe = 77 + (PlFrame / 3);
        if ((keysHeld() & KEY_DOWN) || (keysHeld() & KEY_UP) || (keysHeld() & KEY_LEFT) || (keysHeld() & KEY_RIGHT))
            aniwaffe = 144 + (PlFrame / 3);
    }

    // being hitten
    if (PlStatus == 2)
        aniwaffe = 110 + PlFrame;

    // Dieing
    if (PlStatus == 3)
        aniwaffe = 132 + (PlFrame / 2);

    // animation when winning
    if (PlStatus == -1)
        aniwaffe = 122 + (PlFrame / 2);

    // Hero
    glPushMatrix();
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(1));
    glTranslatef(CamPosSX, hpos, CamPosSY);
    glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
    glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + PlRichtung * -64));
    Precalcmd2light(aniwaffe, 0);
    RenderMD2Model(aniwaffe, 0);
    glPopMatrix(1);

    // Hero (mirrowed)

    if (Precalcdata[GetPX()][GetPY()] & (1 << mirrow)) {
        glPushMatrix();
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(0));
        glTranslatef(CamPosSX, -.1 - hpos, CamPosSY);
        glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
        glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + PlRichtung * -64));
        RenderMD2ModelMirrowed(aniwaffe, 0);
        glPopMatrix(1);
    }

    // if (EquipedWeapon > -1) {

    glBindTexture(GL_TEXTURE_2D, Figuren[1]);

    // Waffe
    glPushMatrix();
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
    glTranslatef(CamPosSX, hpos, CamPosSY);
    glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
    glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + PlRichtung * -64));
    Precalcmd2light(aniwaffe, 1);
    RenderMD2Model(aniwaffe, 1);
    glPopMatrix(1);

    // Waffe (mirrowed)
    if (Precalcdata[GetPX()][GetPY()] & (1 << mirrow)) {
        glPushMatrix();
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(0));
        glTranslatef(CamPosSX, -.1 - hpos, CamPosSY);
        glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
        glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + PlRichtung * -64));
        RenderMD2ModelMirrowed(aniwaffe, 1);
        glPopMatrix(1);
    }

    // }

    // increase frame of animations
    // attacking
    if (PlStatus == 1) {
        PlFrame++;
        if (PlFrame >= 33)
            PlStatus = 0;
    }

    // beeing hitten
    if (PlStatus == 2) {
        PlFrame++;
        if (PlFrame >= 11)
            PlStatus = 0;
    }

    // dieing
    if (PlStatus == 3) {
        PlFrame++;
        if (PlFrame >= 22)
            PlFrame = 22;
    }

    // winning
    if (PlStatus == -1) {
        PlFrame++;
        if (PlFrame >= 22)
            PlFrame = 0;
    }
}

//*******************************************************************************
// Dorfbwohner (Villagers)
#if 0
void RefreshDorfis(void)
{
    extern u8 Precalcdata[128][128];

    extern int screenmode;
    extern int npctalk;
    int dir, a, vx, vy;
    int pos[2];
    float spos[2];
    float hpos, vsx, vsy;

    for (a = 0; a <= DorfiCount; a++) {
        hpos = GetHight(DorfiX[a], DorfiY[a]);
        glBindTexture(GL_TEXTURE_2D, Figuren[DorfiTextNum[a]]);
        glColorTable(GL_RGB256, ...); // TODO: Update this if needed

        if ((DorfiX[a] - 6) + DorfiSX[a] - (CamPosX)-CamPosSX > -4) {
            if ((DorfiX[a] - 6) + DorfiSX[a] - (CamPosX)-CamPosSX < 4) {
                if ((DorfiY[a] - 8) + DorfiSY[a] - (CamPosY)-CamPosSY > -8) {
                    if ((DorfiY[a] - 8) + DorfiSY[a] - (CamPosY)-CamPosSY < 3) {
                        vx  = DorfiX[a];
                        vy  = DorfiY[a];
                        vsx = DorfiSX[a];
                        vsy = DorfiSY[a];
                        if (vsy >= -.5)
                            vy++;
                        if (vsy <= .5)
                            vy--;
                        if (vsx >= -.5)
                            vx++;
                        if (vsx <= .5)
                            vx--; // This is needed to get a better position
                        vsx = DorfiSX[a] + .5;
                        vsy = DorfiSY[a] + .5;
                        dir = DorfiRichtung[a];
                        if (a == npctalk && screenmode == 2) {
                            dir = PlRichtung + 4;
                            if (dir > 7)
                                dir -= 8;
                        }

                        pos[0]  = vx;
                        pos[1]  = vy;
                        spos[0] = vsx;
                        spos[1] = vsy;
                        SetMdlLights(pos, spos, .4, dir * 64);

                        hpos = GetInterPolY(vx, vy, vsx, vsy);

                        glPushMatrix();
                        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(DorfiTextNum[a] + 1));
                        glTranslatef((DorfiX[a] - 6) + DorfiSX[a] - (CamPosX), hpos, (DorfiY[a] - 8) + DorfiSY[a] - (CamPosY));
                        glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
                        glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + dir * -64));
                        if ((screenmode != 2 || a != npctalk) && screenmode <= 2) {
                            Precalcmd2light(stackt11 + 11, DorfiTextNum[a]);
                            RenderMD2Model(stackt11 + 11, DorfiTextNum[a]);
                        } else {
                            Precalcmd2light(stackt11, DorfiTextNum[a]);
                            RenderMD2Model(stackt11, DorfiTextNum[a]);
                        }
                        glPopMatrix(1);

                        if (Precalcdata[DorfiX[a]][DorfiY[a]] & (1 << mirrow)) {
                            glPushMatrix();
                            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(0));
                            glTranslatef((DorfiX[a] - 6) + DorfiSX[a] - (CamPosX), -hpos, (DorfiY[a] - 8) + DorfiSY[a] - (CamPosY));
                            glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
                            glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + dir * -64));
                            if ((screenmode != 2 || a != npctalk) && screenmode <= 2)
                                RenderMD2ModelMirrowed(stackt11 + 11, DorfiTextNum[a]);
                            else
                                RenderMD2ModelMirrowed(stackt11, DorfiTextNum[a]);
                            glPopMatrix(1);
                        }
                    }
                }
            }
        }
    }
}
#endif

//*******************************************************************************
// Gegner (Enemys)
void RefreshEnemys(void)
{
    extern u8 Precalcdata[128][128];

    // extern int screenmode;
    extern int GegnerX[10], GegnerY[10];
    extern float GegnerSX[10], GegnerSY[10];
    extern int GegnerTextNum[10];
    extern int GegnerRichtung[10];
    extern int GegnerCount;
    extern int GegnerStatus[10];
    extern int GegnerFrame[10];
    extern int GegnerATP[10];

    int aniset = 0;
    int blend;

    int dir, a, vx, vy;
    int pos[2];
    float spos[2];
    float hpos, vsx, vsy;

    for (a = 0; a <= GegnerCount; a++) {
        blend = 31;
        // walking arround or following
        aniset = 11 + stackt11;
        // attacking
        if (GegnerStatus[a] == 1)
            aniset = 22 + float(GegnerFrame[a] / 22.0 * 18);
        // being hitten
        if (GegnerStatus[a] == 2)
            aniset = 33 + GegnerFrame[a];
        // dies
        if (GegnerStatus[a] == 3) {
            aniset = 55 + (GegnerFrame[a] / 4);
            blend  = 31 - float(GegnerFrame[a] / 44 * 31);
        }

        hpos = GetHight(GegnerX[a], GegnerY[a]);
        glBindTexture(GL_TEXTURE_2D, Figuren[GegnerTextNum[a]]);

        if ((GegnerX[a] - 6) + GegnerSX[a] - (CamPosX)-CamPosSX > -4)
            if ((GegnerX[a] - 6) + GegnerSX[a] - (CamPosX)-CamPosSX < 4)
                if ((GegnerY[a] - 8) + GegnerSY[a] - (CamPosY)-CamPosSY > -6)
                    if ((GegnerY[a] - 8) + GegnerSY[a] - (CamPosY)-CamPosSY < 3) {
                        vx  = GegnerX[a];
                        vy  = GegnerY[a];
                        vsx = GegnerSX[a];
                        vsy = GegnerSY[a];
                        if (vsy >= -.5)
                            vy++;
                        if (vsy <= .5)
                            vy--;
                        if (vsx >= -.5)
                            vx++;
                        if (vsx <= .5)
                            vx--; // This is needed to get a better position
                        vsx = GegnerSX[a] + .5;
                        vsy = GegnerSY[a] + .5;
                        dir = GegnerRichtung[a];

                        pos[0]  = vx;
                        pos[1]  = vy;
                        spos[0] = vsx;
                        spos[1] = vsy;
                        SetMdlLights(pos, spos, .4, dir * 64);

                        hpos = GetInterPolY(vx, vy, vsx, vsy);

                        glPushMatrix();
                        glPolyFmt(POLY_ALPHA(blend) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(GegnerTextNum[a] + 1));
                        glTranslatef((GegnerX[a] - 6) + GegnerSX[a] - (CamPosX), hpos, (GegnerY[a] - 8) + GegnerSY[a] - (CamPosY));
                        glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
                        glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + dir * -64));
                        Precalcmd2light(aniset, GegnerTextNum[a]);
                        RenderMD2Model(aniset, GegnerTextNum[a]);
                        glPopMatrix(1);

                        if (Precalcdata[GegnerX[a]][GegnerY[a]] & (1 << mirrow)) {
                            glPushMatrix();
                            glPolyFmt(POLY_ALPHA(blend) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(0));
                            glTranslatef((GegnerX[a] - 6) + GegnerSX[a] - (CamPosX), -hpos, (GegnerY[a] - 8) + GegnerSY[a] - (CamPosY));
                            glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
                            glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + dir * -64));
                            RenderMD2ModelMirrowed(aniset, GegnerTextNum[a]);
                            glPopMatrix(1);
                        }

                        // increasing frame
                        if (GegnerStatus[a] == 1) {
                            GegnerFrame[a]++;
                            if (GegnerFrame[a] >= 18) {
                                GegnerFrame[a]  = 0;
                                GegnerStatus[a] = -1;
                                if (PlStatus != 3 && PlStatus != 2) { // player gets some
                                    PlStatus = 2;
                                    PlFrame  = 0;
                                    PlHP -= rand() % GegnerATP[a];
                                    if (PlHP <= 0) {
                                        PlStatus = 3;
                                        PlHP     = 0;
                                    }
                                    UpdateBar();
                                }
                            }
                        }
                        if (GegnerStatus[a] >= 2) {
                            GegnerFrame[a]++;
                            if (GegnerFrame[a] > 11 && GegnerStatus[a] != 3) {
                                GegnerFrame[a]  = 0;
                                GegnerStatus[a] = -1;
                            }

                            if (GegnerFrame[a] > 44 && GegnerStatus[a] == 3)
                                GegnerFrame[a] = 44;
                        }
                    }
    }
}

void Refresh3D(void)
{
    RefreshEnemys();
    // RefreshDorfis();
    RefreshPlayer();
    RefreshWelt();
    RefreshMap();
}
