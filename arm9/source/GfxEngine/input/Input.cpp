#include "GfxEngine/3D.h"
#include "GfxEngine/input/circlestuff.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/texture/Light.h"
#include "GfxEngine/Villagers.h"

// Variblen f�r die matrix
int CamPosX = 15, CamPosY = 0;
float CamPosSX = 0, CamPosSY = 0;

float PlPosX = 15, PlPosY = 0;
float PlPosSX = 0, PlPosSY = 0;
float PlHeight = 0;
int PlRichtung = 0;
int PlStatus   = 0; // aviable :D
int PlFrame    = 0;
int PlHP       = 100;

//**************************************************
// DATENBESCHAFFUNGEN

int GetPX(void)
{
    int x = PlPosX + 6;
    if (PlPosSX >= -.5)
        x++;
    if (PlPosSX <= .5)
        x--;
    return x;
}

int GetPY(void)
{
    int y = PlPosY + 8;
    if (PlPosSY >= -.5)
        y++;
    if (PlPosSY <= .5)
        y--;
    return y;
}

float GetSY(void)
{
    // int y = PlPosY + 8;
    float sy = PlPosSY + .5;
    return sy;
}

float GetSX(void)
{
    int x    = PlPosX + 6;
    float sx = PlPosSX;
    if (PlPosSY < -.5) {
        x++;
        sx = PlPosSX + 1;
    }
    if (PlPosSY > .5) {
        x--;
        sx = PlPosSX - 1;
    }
    return sx;
}

//******************************************************************************
// Menu
void Menu(void)
{
#if 0
    extern int MouseOnButt;
    extern int screenmode;
    touchPosition touch;
    extern int CellSelect;
    extern int Inventory[100];
    extern int EquipedWeapon;
    extern int GegnerCount;

    int Old, Old2;
    Old = MouseOnButt;

    MouseOnButt = -1;
    Old2        = CellSelect;
    if (keysHeld() & KEY_TOUCH) {
        touch = touchReadXY();

        if (touch.py > 144 && touch.py < 144 + 32) { // Buttonfield
            if (touch.px > 29 && touch.px < 29 + 64)
                MouseOnButt = 1;
            if (touch.px > 29 + 130 && touch.px < 29 + 129 + 64)
                MouseOnButt = 2;
        }

        if (screenmode == 3) {
            if (touch.py > 22 && touch.py < 220)
                if (touch.px > 134 && touch.px < 227)
                    CellSelect = (touch.py - 22) / 10;

            if (CellSelect >= 10)
                CellSelect = 9;
            if (Old2 != CellSelect)
                ItemMode();

            if (strncmp("WEAPON", GiveItemType(Inventory[CellSelect]), 6) == 0) {
                if (touch.py > 87 && touch.py < 119)
                    if (touch.px > 45 && touch.px < 109)
                        if (GegnerCount == -1)
                            MouseOnButt = 10;
            }
        }
    }

    int s = 0;

    if (!(keysHeld() & KEY_TOUCH) && Old == 10) {
        if (EquipedWeapon == CellSelect)
            EquipedWeapon = -1;
        else
            EquipedWeapon = CellSelect;
        ItemMode();
        s = 1;
    } // Equip/DeEquip Weapon

    if (!(keysHeld() & KEY_TOUCH) && Old == 1) {
        CellSelect = 0;
        ItemMode();
        s = 1;
    }
    if (!(keysHeld() & KEY_TOUCH) && Old == 2 && screenmode < 3) {
        PauseMode();
        s = 1;
    }

    if (!(keysHeld() & KEY_TOUCH) && MouseOnButt != Old && s == 0)
        ScreenMode();
#endif
}

//******************************************************************************
// Interaction
void Interact(void)
{
    // Lets start interact
    extern int GegnerCount;
    extern int GegnerX[10], GegnerY[10];
    extern float GegnerSX[10], GegnerSY[10];
    extern int GegnerStatus[10];
    extern int GegnerFrame[10];
    extern int GegnerHP[10];
    extern float GegnerRadius[10];
    extern bool Gameended;

    bool l, r, u, d;
    l = false;
    r = false;
    u = false;
    d = false;

    if (PlRichtung == 0) {
        d = true;
    }
    if (PlRichtung == 1) {
        d = true;
        l = true;
    }
    if (PlRichtung == 2) {
        l = true;
    }
    if (PlRichtung == 3) {
        u = true;
        l = true;
    }
    if (PlRichtung == 4) {
        u = true;
    }
    if (PlRichtung == 5) {
        u = true;
        r = true;
    }
    if (PlRichtung == 6) {
        r = true;
    }
    if (PlRichtung == 7) {
        d = true;
        r = true;
    }

    // doors
    if (PlStatus == 0 && Gameended == false) {
        if (keysDown() & KEY_A) {
            if (d) {
                OpenMapDoor(GetPX(), GetPY() + 1);
            }

            if (l) {
                OpenMapDoor(GetPX() - 1, GetPY());
            }

            if (u) {
                OpenMapDoor(GetPX(), GetPY() - 1);
            }

            if (r) {
                OpenMapDoor(GetPX() + 1, GetPY());
            }
        }

        if (keysDown() & KEY_B) {
            // Attack if possible
            if (GegnerCount >= 0) {
                PlStatus = 1; // hit him!
                PlFrame  = 0;
            }
        }
    }
    // enemys
    if (PlStatus == 1 && PlFrame == 21 && Gameended == false) {
        // Check on which site which NPC is
        int nl, nr, nu, nd;
        nl = -1;
        nr = -1;
        nu = -1;
        nd = -1;
        float Px, Py;
        int NPCnum;
        float NPx, NPy;
        Px = GetPX() + (PlPosSX + .5);
        Py = GetPY() + (PlPosSY + .5);

        for (NPCnum = 0; NPCnum <= GegnerCount; NPCnum++) {
            NPx = GegnerX[NPCnum];
            NPy = GegnerY[NPCnum];
            if (GegnerSY[NPCnum] >= -.5)
                NPy++;
            if (GegnerSY[NPCnum] <= .5)
                NPy--;
            if (GegnerSX[NPCnum] >= -.5)
                NPx++;
            if (GegnerSX[NPCnum] <= .5)
                NPx--;
            NPx += GegnerSX[NPCnum] + .5;
            NPy += GegnerSY[NPCnum] + .5;
            // Wow strange code....but should work to compare those positions right
            // now lets compace
            if (Py > NPy - GegnerRadius[NPCnum] && Py < NPy + GegnerRadius[NPCnum] && GegnerStatus[NPCnum] < 2) {
                if (Px < NPx && Px + GegnerRadius[NPCnum] > NPx)
                    nr = NPCnum;
                if (Px > NPx && Px - GegnerRadius[NPCnum] < NPx)
                    nl = NPCnum;
            }
            if (Px > NPx - GegnerRadius[NPCnum] && Px < NPx + GegnerRadius[NPCnum] && GegnerStatus[NPCnum] < 2) {
                if (Py < NPy && Py + GegnerRadius[NPCnum] > NPy)
                    nd = NPCnum;
                if (Py > NPy && Py - GegnerRadius[NPCnum] < NPy)
                    nu = NPCnum;
            }
        }

        if (d && nd > -1) {
            GegnerHP[nd] -= rand() % 5;

            if (GegnerHP[nd] > 0) {
                GegnerStatus[nd] = 2;
                GegnerFrame[nd]  = 0;
            }

            if (GegnerHP[nd] <= 0) {
                GegnerStatus[nd] = 3;
                GegnerFrame[nd]  = 0;
            }
        }

        if (l && nl > -1) {
            GegnerHP[nl] -= rand() % 5;

            if (GegnerHP[nl] > 0) {
                GegnerStatus[nl] = 2;
                GegnerFrame[nl]  = 0;
            }

            if (GegnerHP[nl] <= 0) {
                GegnerStatus[nl] = 3;
                GegnerFrame[nl]  = 0;
            }
        }

        if (u && nu > -1) {
            GegnerHP[nu] -= rand() % 5;

            if (GegnerHP[nu] > 0) {
                GegnerStatus[nu] = 2;
                GegnerFrame[nu]  = 0;
            }

            if (GegnerHP[nu] <= 0) {
                GegnerStatus[nu] = 3;
                GegnerFrame[nu]  = 0;
            }
        }

        if (r && nr > -12) {
            GegnerHP[nu] -= rand() % 5;

            if (GegnerHP[nu] > 0) {
                GegnerStatus[nu] = 2;
                GegnerFrame[nu]  = 0;
            }

            if (GegnerHP[nu] <= 0) {
                GegnerStatus[nu] = 3;
                GegnerFrame[nu]  = 0;
            }
        }
    }

#if 0
    // Lets start interact
    if (keysDown() & KEY_A) {
        bool l, r, u, d;
        l = false;
        r = false;
        u = false;
        d = false;

        if (PlRichtung == 0) {
            d = true;
        }
        if (PlRichtung == 1) {
            d = true;
            l = true;
        }
        if (PlRichtung == 2) {
            l = true;
        }
        if (PlRichtung == 3) {
            u = true;
            l = true;
        }
        if (PlRichtung == 4) {
            u = true;
        }
        if (PlRichtung == 5) {
            u = true;
            r = true;
        }
        if (PlRichtung == 6) {
            r = true;
        }
        if (PlRichtung == 7) {
            d = true;
            r = true;
        }

        // Check on which site which NPC is
        int nl, nr, nu, nd;
        nl = -1;
        nr = -1;
        nu = -1;
        nd = -1;
        float Px, Py;
        int NPCnum;
        float NPx, NPy;
        Px = GetPX() + (PlPosSX + .5);
        Py = GetPY() + (PlPosSY + .5);

        for (NPCnum = 0; NPCnum <= DorfiCount; NPCnum++) {
            NPx = DorfiX[NPCnum];
            NPy = DorfiY[NPCnum];
            if (DorfiSY[NPCnum] >= -.5)
                NPy++;
            if (DorfiSY[NPCnum] <= .5)
                NPy--;
            if (DorfiSX[NPCnum] >= -.5)
                NPx++;
            if (DorfiSX[NPCnum] <= .5)
                NPx--;
            NPx += DorfiSX[NPCnum] + .5;
            NPy += DorfiSY[NPCnum] + .5;
            // Wow strange code....but should work to compare those positions right
            // now lets compace
            if (Py > NPy - .8 && Py < NPy + .8) {
                if (Px < NPx && Px + .8 > NPx)
                    nr = NPCnum;
                if (Px > NPx && Px - .8 < NPx)
                    nl = NPCnum;
            }
            if (Px > NPx - .8 && Px < NPx + .8) {
                if (Py < NPy && Py + .8 > NPy)
                    nd = NPCnum;
                if (Py > NPy && Py - .8 < NPy)
                    nu = NPCnum;
            }
        }

        if (d) {
            if (nd != -1) {
                // NPCmode(nd);
                TextBoxmode(DorfiSpeech[nd], DorfiSpeechCount[nd], nd);
                return;
            }
            OpenMapDoor(GetPX(), GetPY() + 1);
        }

        if (l) {
            if (nl != -1) {
                TextBoxmode(DorfiSpeech[nl], DorfiSpeechCount[nl], nl);
                return;
            }
            OpenMapDoor(GetPX() - 1, GetPY());
        }

        if (u) {
            if (nu != -1) {
                TextBoxmode(DorfiSpeech[nu], DorfiSpeechCount[nu], nu);
                return;
            }
            OpenMapDoor(GetPX(), GetPY() - 1);
        }

        if (r) {
            if (nr != -1) {
                TextBoxmode(DorfiSpeech[nr], DorfiSpeechCount[nr], nr);
                return;
            }
            OpenMapDoor(GetPX() + 1, GetPY());
        }
    }
#endif
}

//*******************************************************************************
// Inputs
void inputs(void)
{
    bool u, l, d, r;
    bool ul, ur, dl, dr;

    extern bool Gameended;
    extern int GegnerStatus[10];

    int obwalldoor = Obj_WallDoor;

    if (Gameended)
        return;

    // sets wall transparent if player is behind it
    SetWallTrans(false);
    if (IsObjHouse(GetPX(), GetPY() + 1) || IsObjHouse(GetPX(), GetPY() + 2))
        SetWallTrans(true);
    if (IsObjWall(GetPX(), GetPY() + 1) == true)
        if (ShapeObj[GetPX()][GetPY() + 1] == obwalldoor)
            SetWallTrans(true);
    if (IsObjWall(GetPX(), GetPY() + 2) == true)
        if (ShapeObj[GetPX()][GetPY() + 2] == obwalldoor)
            SetWallTrans(true);
    if (IsObjBumpWall(GetPX(), GetPY() + 1) == true)
        if (ShapeObj[GetPX()][GetPY() + 1] == obwalldoor)
            SetWallTrans(true);
    if (IsObjBumpWall(GetPX(), GetPY() + 2) == true)
        if (ShapeObj[GetPX()][GetPY() + 2] == obwalldoor)
            SetWallTrans(true);

    float sx = PlPosSX * 20;
    float sy = PlPosSY * 20;

    bool val[8];
    int pressedkeycount = 0;
    if (keysHeld() & KEY_DOWN)
        pressedkeycount++;
    if (keysHeld() & KEY_UP)
        pressedkeycount++;
    if (keysHeld() & KEY_LEFT)
        pressedkeycount++;
    if (keysHeld() & KEY_RIGHT)
        pressedkeycount++;

    if (pressedkeycount == 2)
        CheckcolisionDiagonal(PlPosX + 6, PlPosY + 8, sx + 10, sy + 10, 5, (bool *)&val);
    else
        Checkcolision(PlPosX + 6, PlPosY + 8, sx + 10, sy + 10, 5, (bool *)&val);

    u  = val[0];
    ur = val[1];
    r  = val[2];
    dr = val[3];
    d  = val[4];
    dl = val[5];
    l  = val[6];
    ul = val[7];

    // Chrash mit NPCs verhindern
    float Px, Py;
    float dx, dy;
    int NPCnum;
    float NPx, NPy;
    Px = GetPX() + (PlPosSX + .5);
    Py = GetPY() + (PlPosSY + .5);

    extern int GegnerCount;
    extern int GegnerX[10], GegnerY[10];
    extern float GegnerSX[10], GegnerSY[10];
    extern float GegnerRadius[10];

    for (NPCnum = 0; NPCnum <= GegnerCount; NPCnum++) {
        NPx = GegnerX[NPCnum];
        NPy = GegnerY[NPCnum];
        if (GegnerSY[NPCnum] >= -.5)
            NPy++;
        if (GegnerSY[NPCnum] <= .5)
            NPy--;
        if (GegnerSX[NPCnum] >= -.5)
            NPx++;
        if (GegnerSX[NPCnum] <= .5)
            NPx--;
        NPx += GegnerSX[NPCnum] + .5;
        NPy += GegnerSY[NPCnum] + .5;
        // Wow strange code....but should work to compare those positions right
        // first check distance
        dx = Px - NPx;
        if (dx < 0)
            dx *= -1;
        dy = Py - NPy;
        if (dy < 0)
            dy *= -1;
        // now lets compace
        if (dx > dy)
            if (Py > NPy - GegnerRadius[NPCnum] && Py < NPy + GegnerRadius[NPCnum] && GegnerStatus[NPCnum] != 3) {
                if (Px < NPx && Px + GegnerRadius[NPCnum] > NPx)
                    r = false;
                if (Px > NPx && Px - GegnerRadius[NPCnum] < NPx)
                    l = false;
            }
        if (dy > dx)
            if (Px > NPx - GegnerRadius[NPCnum] && Px < NPx + GegnerRadius[NPCnum] && GegnerStatus[NPCnum] != 3) {
                if (Py < NPy && Py + GegnerRadius[NPCnum] > NPy)
                    d = false;
                if (Py > NPy && Py - GegnerRadius[NPCnum] < NPy)
                    u = false;
            }
    }

    if (PlStatus != 3) {
        // Look in given dirrection
        if (keysHeld() & KEY_DOWN)
            PlRichtung = 0;
        if (keysHeld() & KEY_LEFT)
            PlRichtung = 2;
        if (keysHeld() & KEY_UP)
            PlRichtung = 4;
        if (keysHeld() & KEY_RIGHT)
            PlRichtung = 6;

        if (keysHeld() & KEY_DOWN)
            if (keysHeld() & KEY_LEFT)
                PlRichtung = 1;
        if (keysHeld() & KEY_DOWN)
            if (keysHeld() & KEY_RIGHT)
                PlRichtung = 7;
        if (keysHeld() & KEY_UP)
            if (keysHeld() & KEY_LEFT)
                PlRichtung = 3;
        if (keysHeld() & KEY_UP)
            if (keysHeld() & KEY_RIGHT)
                PlRichtung = 5;

        // Movement
        // Player(aka da playa :D)
        // linear
        if (pressedkeycount == 1) {
            if ((keysHeld() & KEY_UP) && u) {
                if (ul == true && ur == true)
                    PlPosSY -= .05;                           // nothing in our way...go ahead
                if (ul == true && l == true && ur == false) { // something blocks on the right
                    PlPosSY -= .05;                           // go up
                    PlPosSX -= .05;                           // and go left
                }
                if (ur == true && r == true && ul == false) { // something blocks on the left
                    PlPosSY -= .05;                           // go up
                    PlPosSX += .05;                           // and go right
                }
            }

            if ((keysHeld() & KEY_DOWN) && d) {
                if (dl == true && dr == true)
                    PlPosSY += .05;                           // nothing in our way...go ahead
                if (dl == true && l == true && dr == false) { // something blocks on the right
                    PlPosSY += .05;                           // go down
                    PlPosSX -= .05;                           // and go left
                }
                if (dr == true && r == true && dl == false) { // something blocks on the left
                    PlPosSY += .05;                           // go down
                    PlPosSX += .05;                           // and go right
                }
            }

            if ((keysHeld() & KEY_LEFT) && l) {
                if (dl == true && ul == true)
                    PlPosSX -= .05;                           // nothing in our way...go ahead
                if (dl == true && d == true && ul == false) { // something blocks on the top
                    PlPosSX -= .05;                           // go left
                    PlPosSY += .05;                           // and go down
                }
                if (ul == true && u == true && dl == false) { // something blocks on the bottom
                    PlPosSX -= .05;                           // go left
                    PlPosSY -= .05;                           // and go up
                }
            }

            if ((keysHeld() & KEY_RIGHT) && r) {
                if (dr == true && ur == true)
                    PlPosSX += .05;                           // nothing in our way...go ahead
                if (dr == true && d == true && ur == false) { // something blocks on the top
                    PlPosSX += .05;                           // go right
                    PlPosSY += .05;                           // and go down
                }
                if (ur == true && u == true && dr == false) { // something blocks on the bottom
                    PlPosSX += .05;                           // go right
                    PlPosSY -= .05;                           // and go up
                }
            }
        }
        // diagonal
        if (pressedkeycount == 2) {
            if ((keysHeld() & KEY_UP) && (keysHeld() & KEY_LEFT) && ul) {
                if (u && l) { // nothing in our way...go ahead
                    PlPosSX -= .05;
                    PlPosSY -= .05;
                }

                if (u && !l) { // left is blocked but we can go up
                    PlPosSY -= .05;
                }

                if (!u && l) { // left isn't blocked but we cant go up
                    PlPosSX -= .05;
                }
            }

            if ((keysHeld() & KEY_UP) && (keysHeld() & KEY_RIGHT) && ur) {
                if (u && r) { // nothing in our way...go ahead
                    PlPosSX += .05;
                    PlPosSY -= .05;
                }

                if (u && !r) { // right is blocked but we can go up
                    PlPosSY -= .05;
                }

                if (!u && r) { // right isn't blocked but we cant go up
                    PlPosSX += .05;
                }
            }
            if ((keysHeld() & KEY_DOWN) && (keysHeld() & KEY_LEFT) && dl) {
                if (d && l) { // nothing in our way...go ahead
                    PlPosSX -= .05;
                    PlPosSY += .05;
                }

                if (d && !l) { // left is blocked but we can go down
                    PlPosSY += .05;
                }

                if (!d && l) { // left isn't blocked but we cant go down
                    PlPosSX -= .05;
                }
            }

            if ((keysHeld() & KEY_DOWN) && (keysHeld() & KEY_RIGHT) && dr) {
                if (d && r) { // nothing in our way...go ahead
                    PlPosSX += .05;
                    PlPosSY += .05;
                }

                if (d && !r) { // right is blocked but we can go down
                    PlPosSY += .05;
                }

                if (!d && r) { // right isn't blocked but we cant go down
                    PlPosSX += .05;
                }
            }
        }

        if (PlPosSX > .5) {
            PlPosX += 1;
            PlPosSX -= 1;
        }
        if (PlPosSX < -.5) {
            PlPosX -= 1;
            PlPosSX += 1;
        }
        if (PlPosSY > .5) {
            PlPosY += 1;
            PlPosSY -= 1;
        }
        if (PlPosSY < -.5) {
            PlPosY -= 1;
            PlPosSY += 1;
        }

        CamPosY  = PlPosY;
        CamPosX  = PlPosX;
        CamPosSY = PlPosSY;
        CamPosSX = PlPosSX;
        if (CamPosSY <= -.5) {
            CamPosY -= 1;
            CamPosSY += 1;
        }

        // Mapchange
        char Filename[50];
        int newx = 0, newy = 0;
        snprintf(Filename, sizeof(Filename), "%s", GetMapChange(GetPX(), GetPY()));
        if (Filename[0] != '.') {
            newx = GetMapChangePOSX(GetPX(), GetPY()) - 6;
            newy = GetMapChangePOSY(GetPX(), GetPY()) - 8;
            printf("\x1b[2J");

            for (int b = 8; b >= 0; b--) {
                Blend = b;
                RunTime();
                E3D_StartRender();
                UpdateDynamic();
                Refresh3D();
                glPopMatrix(1);
                glFlush(GL_WBUFFERING);
            }

            LoadScrip(Filename);
            PlPosX  = newx;
            PlPosY  = newy;
            CamPosX = newx;
            CamPosY = newy;

            for (int b = 0; b <= 8; b++) {
                Blend = b;
                RunTime();
                E3D_StartRender();
                UpdateDynamic();
                Refresh3D();
                glPopMatrix(1);
                glFlush(GL_WBUFFERING);
            }
        }

        Interact(); // interact...sure or?
        Menu();
    }
}
