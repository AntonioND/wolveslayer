#include "GfxEngine/3D.h"
#include "GfxEngine/Enemies.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Model/MD2Models.h"
#include "GfxEngine/Output/Inventory.h"
#include "GfxEngine/Output/Textbox.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Render/Autotiles.h"
#include "GfxEngine/Render/Doors.h"
#include "GfxEngine/Render/Edge.h"
#include "GfxEngine/Render/Ground.h"
#include "GfxEngine/Render/Obj_House.h"
#include "GfxEngine/Render/Obj_Wall.h"
#include "GfxEngine/Render/Water.h"
#include "GfxEngine/Script/Script_Chars.h"
#include "GfxEngine/Script/Script_Objects.h"
#include "GfxEngine/Texture/Light.h"
#include "GfxEngine/Ticks.h"
#include "GfxEngine/Villagers.h"

const f32 waterheight = floattof32(-.1);

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

static void RefreshMap(void)
{
    int x = 0, y = 0;

    int Wallbor = Obj_WallBor;

    int subset;
    int minx, maxx;

    for (int yy = 11; yy > -2; yy--)
    {
        minx = 5;
        maxx = 6;
        if (yy == 11 || yy == 10 || yy == 9)
        {
            minx = 2;
            maxx = 9;
        }
        if (yy == 8)
        {
            minx = 1;
            maxx = 9;
        }
        if (yy == 7 || yy == 6 || yy == 5)
        {
            minx = 1;
            maxx = 10;
        }
        if (yy == 4 || yy == 3 || yy == 2)
        {
            minx = 0;
            maxx = 11;
        }
        if (yy == 1 || yy == 0 || yy == -1)
        {
            minx = -1;
            maxx = 12;
        }
        if (yy <= -2)
        {
            minx = -2;
            maxx = 13;
        }

        for (int xx = maxx; xx > minx; xx--)
        {
            x = xx + CamPosX;
            y = yy + CamPosY;

            if (Objects[TexObj[x][y]].NoGround)
                continue;

            SetCurGround(x, y); // We tell the render engine where the ground is to set up maplight

            // Levelrand
            if ((TexGround[x][y] <= -1 && TexAGround[x][y] <= -1 && !(Precalcdata[x][y] & (1 << B_Water))) || (ShapeObj[x][y] == Wallbor))
            {
                glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));
                glBindTexture(GL_TEXTURE_2D, Ground[0].Texture);
                RenderLevelBorderGround(xx - 6, 0, yy - 8);
            }

            if (x < MapGetWr() && y < MapGetHr() && ShapeObj[x][y] != Wallbor)
            {
                if (x > -1 && y > -1)
                {
                    // Ground
                    if (TexGround[x][y] >= 0 && SetAGround[x][y] < 0)
                    {
                        if (TexGround[x][y] >= 0)
                            RenderGround(xx - 6, 0, yy - 8, TexGround[x][y]);
                    }

                    // AutoGround
                    if (TexAGround[x][y] != -2 && SetAGround[x][y] >= 0 && SetAGround[x][y] <= 3 && TexGround[x][y] == -2
                        && ShapeObj[x][y] != Wallbor)
                    {
                        subset = 0;
                        if (TexAGround[x][y] > 15)
                            subset = 1;
                        if (TexAGround[x][y] > 31)
                            subset = 2;
                        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
                        glBindTexture(GL_TEXTURE_2D, Autotile[SetAGround[x][y]].Texture[subset]);

                        RenderAutoGround(xx - 6, 0, yy - 8, TexAGround[x][y] - (subset * 16));
                    }

                    // Water
                    if ((Precalcdata[x][y] & (1 << B_Water)) && ShapeObj[x][y] != Wallbor)
                    {
                        glBindTexture(GL_TEXTURE_2D, Water[0]);
                        RenderWater(ticks32, inttof32(xx - 6), waterheight, inttof32(yy - 8));
                    }

                    // Edge
                    if (Terrain[x][y].sidewalls)
                    {
                        RenderEdge(x, y, inttof32(xx - 6), inttof32(yy - 8));
                    }
                }
            }
        }
    }
}

//*******************************************************************************

static void RefreshWorld(void)
{
    int x = 0, y = 0;

    int model     = Obj_MD2;
    int Housem    = Obj_HouseMid;
    int Housebor  = Obj_HouseBor;
    int Housecor  = Obj_HouseCor;
    int Housedoor = Obj_HouseDoor;

    int Wallbor = Obj_WallBor;
    // int WallAlone = Obj_WallAlone;
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

    for (int yy = 12; yy > -4; yy--)
    {
        y = yy + CamPosY;
        if (yy == 12)
        {
            minx = 3;
            maxx = 8;
        }
        if (yy == 11 || yy == 10 || yy == 9)
        {
            minx = 2;
            maxx = 9;
        }
        if (yy == 8)
        {
            minx = 1;
            maxx = 9;
        }
        if (yy == 7 || yy == 6 || yy == 5)
        {
            minx = 1;
            maxx = 10;
        }
        if (yy == 4 || yy == 3 || yy == 2)
        {
            minx = 0;
            maxx = 11;
        }
        if (yy == 1 || yy == 0 || yy == -1)
        {
            minx = -1;
            maxx = 12;
        }
        if (yy <= -2)
        {
            minx = -2;
            maxx = 13;
        }

        for (int xx = maxx; xx > minx; xx--)
        {
            x = xx + CamPosX;

            seeable = true;
            if (x >= MapGetWr() || y >= MapGetHr())
                seeable = false;
            if (x < 0 || y < 0)
                seeable = false;

            if (!seeable)
                continue;

            if (TexObj[x][y] < 0)
                continue;

            ObjectInfo *obj = &Objects[TexObj[x][y]];

            glBindTexture(GL_TEXTURE_2D, ObjectTexture[obj->TextureID].Texture);

            height = TerrainMid[x][y];
            posx   = inttof32(xx - 6);
            posy   = inttof32(yy - 8);

            // Models
            if (ShapeObj[x][y] == model)
            {
                pos[0] = x;
                pos[1] = y;

                // SetMdlLights(pos,spos,.4,0);   DirObj[x][y]
                if (yy > -1 || ((Precalcdata[x][y] & (1 << B_Mirrowable)) && obj->Mirrowable))
                {
                    if (!obj->Illumination)
                        SetObjLights(pos, DirObj[x][y] * 64); // way faster and enough for objects
                    else
                        SetObjLightsSelfilluminated();

                    Precalcmd2light(ticks11, TexObj[x][y] + 10); // mesh1 lights
                }
                if (yy > -1)
                {
                    // Mesh
                    glPushMatrix();
                    if (obj->Culling)
                        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_FRONT | POLY_FORMAT_LIGHT0 | POLY_ID(obj->TextureID + 11 + 3));
                    else
                        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(obj->TextureID + 11 + 3));
                    glTranslatef32(posx, height, posy);
                    glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
                    glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + (DirObj[x][y] * -64)));
                    RenderMD2Model(ticks11, TexObj[x][y] + 10);
                    glPopMatrix(1);
                }
                // Same for mirrowing
                if ((Precalcdata[x][y] & (1 << B_Mirrowable)) && obj->Mirrowable == true)
                {
                    glPushMatrix();
                    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(obj->TextureID + 11 + 3));
                    glTranslatef32(posx, -height, posy);
                    glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
                    glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + (DirObj[x][y] * -64)));
                    RenderMD2ModelMirrowed(ticks11, TexObj[x][y] + 10);
                    glPopMatrix(1);
                }
            }

            // House middle
            if (ShapeObj[x][y] == Housem)
            {
                SetCurWall(xx + CamPosX, yy + CamPosY);
                if (yy > -1)
                    RenderHouseM(0, posx, floattof32(1.0 + GetHeight(x, y)), posy); // Normal
            }

            // House border
            if (ShapeObj[x][y] == Housebor)
            {
                SetCurWall(xx + CamPosX, yy + CamPosY);
                if (yy > -1) // Normal
                    RenderHouseBorder(0, DirObj[x][y], posx, floattof32(1.0 + GetHeight(x, y)), posy);
                if ((Precalcdata[x][y] & (1 << B_Mirrowable))) // Gespiegelt
                    RenderHouseBorder(1, DirObj[x][y], posx, floattof32(-.9 - GetHeight(x, y)), posy);
            }

            // House corner
            if (ShapeObj[x][y] == Housecor)
            {
                SetCurWall(xx + CamPosX, yy + CamPosY);
                if (yy > -1) // Normal
                    RenderHouseCorner(0, DirObj[x][y], posx, floattof32(1.0 + GetHeight(x, y)), posy);
                if ((Precalcdata[x][y] & (1 << B_Mirrowable))) // Gespiegelt
                    RenderHouseCorner(1, DirObj[x][y], posx, floattof32(-.9 - GetHeight(x, y)), posy);
            }

            // House Border Door
            if (ShapeObj[x][y] == Housedoor)
            {
                SetCurWall(xx + CamPosX, yy + CamPosY);
                if (yy > -1) // Türslot Normal
                {
                    int angle = GetMapDoorAngle(xx + CamPosX, yy + CamPosY);
                    RenderHouseBorderDoor(0, angle, DirObj[x][y], posx, floattof32(1.0 + GetHeight(x, y)), posy);
                }
                if ((Precalcdata[x][y] & (1 << B_Mirrowable)))
                {
                    // We need that to set again...after rendering houspart with doorslot it
                    glBindTexture(GL_TEXTURE_2D, ObjectTexture[TexObj[x][y]].Texture);

                    // Türslot Gespiegelt
                    int angle = GetMapDoorAngle(xx + CamPosX, yy + CamPosY);
                    RenderHouseBorderDoor(1, angle, DirObj[x][y], posx, floattof32(-.9 - GetHeight(x, y)), posy);
                }
            }

            // wallParts...finaly
            // Wall border
            // if (IsObjBumpWall(x, y) == true)
            //     glBindTexture(GL_TEXTURE_2D, ObjectTexture[Object[TexObj[x][y]].TextureID].TextureA);

            if (ShapeObj[x][y] == Wallbor)
            {
                SetCurWall(xx + CamPosX, yy + CamPosY);
                if (yy > -1) // Normal
                    RenderWallBorder(0, DirObj[x][y], posx, floattof32(1.0 + GetHeight(x, y)), posy, TexObj[x][y]);
                if ((Precalcdata[x][y] & (1 << B_Mirrowable))) // Gespiegelt
                    RenderWallBorder(1, DirObj[x][y], posx, floattof32(-1 - GetHeight(x, y)), posy, TexObj[x][y]);
            }

            // Wall Border Door
            if (ShapeObj[x][y] == WallDoor)
            {
                SetCurWall(xx + CamPosX, yy + CamPosY);
                if (yy > -1) // Türslot Normal
                {
                    int angle = GetMapDoorAngle(xx + CamPosX, yy + CamPosY);
                    RenderWallBorderDoor(0, angle, DirObj[x][y], posx, floattof32(1 + GetHeight(x, y)), posy);
                }
                if ((Precalcdata[x][y] & (1 << B_Mirrowable)))
                {
                    // We need that to set again...after rendering houspart with doorslot it
                    glBindTexture(GL_TEXTURE_2D, ObjectTexture[TexObj[x][y]].Texture);

                    // Türslot Gespiegelt
                    int angle = GetMapDoorAngle(xx + CamPosX, yy + CamPosY);
                    RenderWallBorderDoor(1, angle, DirObj[x][y], posx, floattof32(-1.02 - GetHeight(x, y)), posy);
                }
            }
        }
    }
}

//*******************************************************************************

static void RefreshPlayer(void)
{
    float sx = PlPosSX + .5;
    float sy = PlPosSY + .5;

    int pos[2];
    pos[0] = GetPX();
    pos[1] = GetPY();
    float spos[2];
    spos[0] = sx;
    spos[1] = sy;
    SetMdlLights(pos, spos, .4, PlDirection * 64);

    glBindTexture(GL_TEXTURE_2D, FigureTextures[CharacterFigureTex_ID]);

    float hpos = GetInterPolY(GetPX(), GetPY(), sx, sy);
    PlHeight   = hpos;

    // walk/standing
    int aniweapon = ticks11;
    if (PlStatus == 0)
    {
        if (EnemyCount >= 0)
            aniweapon = 22 + ticks11;

        if ((screenmode < ScreenModeTextBox) && (keysHeld() & (KEY_DOWN | KEY_UP | KEY_LEFT | KEY_RIGHT)))
            aniweapon += 11;
    }

    // Attacking
    if (PlStatus == 1)
    {
        aniweapon = 77 + (PlFrame / 3);

        if ((keysHeld() & (KEY_DOWN | KEY_UP | KEY_LEFT | KEY_RIGHT)))
            aniweapon = 144 + (PlFrame / 3);
    }

    // being hitten
    if (PlStatus == 2)
        aniweapon = 110 + PlFrame;

    // Dieing
    if (PlStatus == 3)
        aniweapon = 132 + (PlFrame / 2);

    // animation when winning
    if (PlStatus == -1)
        aniweapon = 122 + (PlFrame / 2);

    // Hero
    glPushMatrix();
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(1));
    glTranslatef(CamPosSX, hpos, CamPosSY);
    glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
    glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + PlDirection * -64));
    Precalcmd2light(aniweapon, 0);
    RenderMD2Model(aniweapon, 0);
    glPopMatrix(1);

    // Hero (mirrowed)

    if (Precalcdata[GetPX()][GetPY()] & (1 << B_Mirrowable))
    {
        glPushMatrix();
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(0));
        glTranslatef(CamPosSX, -.1 - hpos, CamPosSY);
        glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
        glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + PlDirection * -64));
        RenderMD2ModelMirrowed(aniweapon, 0);
        glPopMatrix(1);
    }

    // TODO: This would have to be modified if we supported different weapons
    // if (EquipedWeapon > -1) {

    glBindTexture(GL_TEXTURE_2D, FigureTextures[WeaponFigureTex_ID]);

    // Weapon
    glPushMatrix();
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(2));
    glTranslatef(CamPosSX, hpos, CamPosSY);
    glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
    glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + PlDirection * -64));
    Precalcmd2light(aniweapon, 1);
    RenderMD2Model(aniweapon, 1);
    glPopMatrix(1);

    // Weapon (mirrowed)
    if (Precalcdata[GetPX()][GetPY()] & (1 << B_Mirrowable))
    {
        glPushMatrix();
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(0));
        glTranslatef(CamPosSX, -.1 - hpos, CamPosSY);
        glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
        glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + PlDirection * -64));
        RenderMD2ModelMirrowed(aniweapon, 1);
        glPopMatrix(1);
    }

    // }

    // increase frame of animations
    // attacking
    if (PlStatus == 1)
    {
        PlFrame++;
        if (PlFrame >= 33)
            PlStatus = 0;
    }

    // beeing hitten
    if (PlStatus == 2)
    {
        PlFrame++;
        if (PlFrame >= 11)
            PlStatus = 0;
    }

    // dieing
    if (PlStatus == 3)
    {
        PlFrame++;
        if (PlFrame >= 22)
            PlFrame = 22;
    }

    // winning
    if (PlStatus == -1)
    {
        PlFrame++;
        if (PlFrame >= 22)
            PlFrame = 0;
    }
}

//*******************************************************************************
// Villagers
// TODO: This is currently unused
static void RefreshVillagers(void)
{
    for (int a = 0; a <= VillagerCount; a++)
    {
        if (!((Villager[a].X - 6) + Villager[a].SX - (CamPosX)-CamPosSX > -4))
            continue;
        if (!((Villager[a].X - 6) + Villager[a].SX - (CamPosX)-CamPosSX < 4))
            continue;
        if (!((Villager[a].Y - 8) + Villager[a].SY - (CamPosY)-CamPosSY > -8))
            continue;
        if (!((Villager[a].Y - 8) + Villager[a].SY - (CamPosY)-CamPosSY < 3))
            continue;

        float hpos = GetHeight(Villager[a].X, Villager[a].Y);
        glBindTexture(GL_TEXTURE_2D, FigureTextures[Villager[a].TextNum]);

        int vx = Villager[a].X;
        int vy = Villager[a].Y;

        float vsx = Villager[a].SX;
        float vsy = Villager[a].SY;

        // This is needed to get a better position
        if (vsy >= -.5)
            vy++;
        if (vsy <= .5)
            vy--;
        if (vsx >= -.5)
            vx++;
        if (vsx <= .5)
            vx--;

        vsx = Villager[a].SX + .5;
        vsy = Villager[a].SY + .5;

        int dir = Villager[a].Direction;
        if (a == npctalk && screenmode == ScreenModeTextBox)
        {
            dir = PlDirection + 4;
            if (dir > 7)
                dir -= 8;
        }

        int pos[2];
        float spos[2];

        pos[0]  = vx;
        pos[1]  = vy;
        spos[0] = vsx;
        spos[1] = vsy;
        SetMdlLights(pos, spos, .4, dir * 64);

        hpos = GetInterPolY(vx, vy, vsx, vsy);

        glPushMatrix();
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(Villager[a].TextNum + 1));
        glTranslatef((Villager[a].X - 6) + Villager[a].SX - (CamPosX), hpos, (Villager[a].Y - 8) + Villager[a].SY - (CamPosY));
        glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
        glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + dir * -64));
        if ((screenmode != ScreenModeTextBox || a != npctalk) && screenmode <= ScreenModeTextBox)
        {
            Precalcmd2light(ticks11 + 11, Villager[a].TextNum);
            RenderMD2Model(ticks11 + 11, Villager[a].TextNum);
        }
        else
        {
            Precalcmd2light(ticks11, Villager[a].TextNum);
            RenderMD2Model(ticks11, Villager[a].TextNum);
        }
        glPopMatrix(1);

        if (Precalcdata[Villager[a].X][Villager[a].Y] & (1 << B_Mirrowable))
        {
            glPushMatrix();
            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(0));
            glTranslatef((Villager[a].X - 6) + Villager[a].SX - (CamPosX), -hpos, (Villager[a].Y - 8) + Villager[a].SY - (CamPosY));
            glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
            glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + dir * -64));
            if ((screenmode != ScreenModeTextBox || a != npctalk) && screenmode <= ScreenModeTextBox)
                RenderMD2ModelMirrowed(ticks11 + 11, Villager[a].TextNum);
            else
                RenderMD2ModelMirrowed(ticks11, Villager[a].TextNum);
            glPopMatrix(1);
        }
    }
}

//*******************************************************************************

static void RefreshEnemies(void)
{
    for (int a = 0; a <= EnemyCount; a++)
    {
        int blend = 31;
        // walking arround or following
        int aniset = 11 + ticks11;
        // attacking
        if (Enemies[a].Status == 1)
            aniset = 22 + float(Enemies[a].Frame / 22.0 * 18);
        // being hitten
        if (Enemies[a].Status == 2)
            aniset = 33 + Enemies[a].Frame;
        // dies
        if (Enemies[a].Status == 3)
        {
            aniset = 55 + (Enemies[a].Frame / 4);
            blend  = 31 - float(Enemies[a].Frame / 44 * 31);
        }

        if (!((Enemies[a].X - 6) + Enemies[a].SX - (CamPosX)-CamPosSX > -4))
            continue;
        if (!((Enemies[a].X - 6) + Enemies[a].SX - (CamPosX)-CamPosSX < 4))
            continue;
        if (!((Enemies[a].Y - 8) + Enemies[a].SY - (CamPosY)-CamPosSY > -6))
            continue;
        if (!((Enemies[a].Y - 8) + Enemies[a].SY - (CamPosY)-CamPosSY < 3))
            continue;

        float hpos = GetHeight(Enemies[a].X, Enemies[a].Y);
        glBindTexture(GL_TEXTURE_2D, FigureTextures[Enemies[a].TextNum]);

        int vx = Enemies[a].X;
        int vy = Enemies[a].Y;

        float vsx = Enemies[a].SX;
        float vsy = Enemies[a].SY;

        // This is needed to get a better position
        if (vsy >= -.5)
            vy++;
        if (vsy <= .5)
            vy--;
        if (vsx >= -.5)
            vx++;
        if (vsx <= .5)
            vx--;

        vsx = Enemies[a].SX + .5;
        vsy = Enemies[a].SY + .5;

        int dir = Enemies[a].Direction;

        int pos[2];
        float spos[2];

        pos[0]  = vx;
        pos[1]  = vy;
        spos[0] = vsx;
        spos[1] = vsy;
        SetMdlLights(pos, spos, .4, dir * 64);

        hpos = GetInterPolY(vx, vy, vsx, vsy);

        glPushMatrix();
        glPolyFmt(POLY_ALPHA(blend) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(Enemies[a].TextNum + 1));
        glTranslatef((Enemies[a].X - 6) + Enemies[a].SX - (CamPosX), hpos, (Enemies[a].Y - 8) + Enemies[a].SY - (CamPosY));
        glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
        glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + dir * -64));
        Precalcmd2light(aniset, Enemies[a].TextNum);
        RenderMD2Model(aniset, Enemies[a].TextNum);
        glPopMatrix(1);

        if (Precalcdata[Enemies[a].X][Enemies[a].Y] & (1 << B_Mirrowable))
        {
            glPushMatrix();
            glPolyFmt(POLY_ALPHA(blend) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_ID(0));
            glTranslatef((Enemies[a].X - 6) + Enemies[a].SX - (CamPosX), -hpos, (Enemies[a].Y - 8) + Enemies[a].SY - (CamPosY));
            glRotateXi((DEGREES_IN_CIRCLE / 512) * (-128));
            glRotateZi((DEGREES_IN_CIRCLE / 512) * (-128 + dir * -64));
            RenderMD2ModelMirrowed(aniset, Enemies[a].TextNum);
            glPopMatrix(1);
        }

        // increasing frame
        if (Enemies[a].Status == 1)
        {
            Enemies[a].Frame++;
            if (Enemies[a].Frame >= 18)
            {
                Enemies[a].Frame  = 0;
                Enemies[a].Status = -1;
                if (PlStatus != 3 && PlStatus != 2) // player gets some
                {
                    PlStatus = 2;
                    PlFrame  = 0;
                    PlHP -= rand() % Enemies[a].ATP;
                    if (PlHP <= 0)
                    {
                        PlStatus = 3;
                        PlHP     = 0;
                    }
                    UpdateBar();
                }
            }
        }
        if (Enemies[a].Status >= 2)
        {
            Enemies[a].Frame++;
            if (Enemies[a].Frame > 11 && Enemies[a].Status != 3)
            {
                Enemies[a].Frame  = 0;
                Enemies[a].Status = -1;
            }

            if (Enemies[a].Frame > 44 && Enemies[a].Status == 3)
                Enemies[a].Frame = 44;
        }
    }
}

void Refresh3D(void)
{
    RefreshEnemies();
    RefreshVillagers();
    RefreshPlayer();
    RefreshWorld();
    RefreshMap();
}
