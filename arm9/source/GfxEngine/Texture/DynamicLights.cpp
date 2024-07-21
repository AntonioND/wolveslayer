#include "GfxEngine/3D.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/Limits.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Texture/Light.h"

u8 WorldLightR[128 * 128];
u8 WorldLightG[128 * 128];
u8 WorldLightB[128 * 128];

// 20x20 map
u16 ViewportMapLights[20][20];
bool ViewportMapBumpGroundS[20][20];
bool ViewportMapBumpGroundW[20][20];
bool ViewportMapBumpGroundE[20][20];
bool ViewportMapBumpWallS[20][20];
bool ViewportMapBumpWallW[20][20];
bool ViewportMapBumpWallE[20][20];

typedef struct {
    int X;
    int Y;
    int SX;
    int SY;
    bool Enable;
    int Direction;
    u8 Rcolor;
    u8 Gcolor;
    u8 Bcolor;
} DynamicLightSource;

static DynamicLightSource Fireflies[DynamicLight_Max];
static int DynamicLightCount = -1;

void ResetDynamic(void)
{
    DynamicLightCount = -1;
    for (int a = 0; a < DynamicLight_Max; a++)
        Fireflies[a].Enable = false;
}

void AddDynamic(int x, int y, u8 r, u8 g, u8 b)
{
    if (DynamicLightCount >= DynamicLight_Max)
        Crash("Too many dynamic lights:\n%d > %d", DynamicLightCount, DynamicLight_Max);

    DynamicLightCount++;
    Fireflies[DynamicLightCount].X      = x;
    Fireflies[DynamicLightCount].Y      = y;
    Fireflies[DynamicLightCount].Rcolor = r;
    Fireflies[DynamicLightCount].Gcolor = g;
    Fireflies[DynamicLightCount].Bcolor = b;
    Fireflies[DynamicLightCount].Enable = true;
}

static void TurnDyn(int a, bool l, bool r, bool u, bool d)
{
    // Now give a new valid dirrecion if possible
    if (!l && !d && !r && !u)
        return;

    int dirnew = 0;
    bool ok    = false;

    while (ok == false) {
        ok     = true;
        dirnew = rand() % 8;
        if ((dirnew == 0 || dirnew == 1 || dirnew == 7) && d == false)
            ok = false;
        if ((dirnew == 1 || dirnew == 2 || dirnew == 3) && l == false)
            ok = false;
        if ((dirnew == 6 || dirnew == 5 || dirnew == 7) && r == false)
            ok = false;
        if ((dirnew == 4 || dirnew == 3 || dirnew == 5) && u == false)
            ok = false;
    }

    Fireflies[a].Direction = dirnew;
}

static void UpdateFireflies(void)
{
    int a, b;
    bool r, l, u, d;
    int sx, sy;
    int dx, dy;
    bool change;
    float Rcol1, Rcol2, Rcol3, Rcol4;
    float Gcol1, Gcol2, Gcol3, Gcol4;
    float Bcol1, Bcol2, Bcol3, Bcol4;

    if (screenmode < ScreenModeItem) {
        for (a = 0; a < DynamicLight_Max; a++) {
            if (Fireflies[a].Enable == true) {
                l = true;
                r = true;
                u = true;
                d = true;

                sx = (Fireflies[a].SX - 10) / 2;
                sy = (Fireflies[a].SY - 10) / 2;

                for (b = 2; b < 8; b++) {
                    if (Passable(Fireflies[a].X, Fireflies[a].Y, sx + b, sy) == false)
                        u = false;
                    if (Passable(Fireflies[a].X, Fireflies[a].Y, sx + b, sy + 8) == false)
                        d = false;
                    if (Passable(Fireflies[a].X, Fireflies[a].Y, sx + 1, sy - 1 + b) == false)
                        l = false;
                    if (Passable(Fireflies[a].X, Fireflies[a].Y, sx + 8, sy - 1 + b) == false)
                        r = false;
                }

                if (Fireflies[a].X == MapGetWr())
                    r = false;
                if (Fireflies[a].Y == MapGetHr())
                    d = false;

                change = false;
                if ((Fireflies[a].Direction == 0 || Fireflies[a].Direction == 1 || Fireflies[a].Direction == 7) && d == false)
                    change = true;
                if ((Fireflies[a].Direction == 1 || Fireflies[a].Direction == 2 || Fireflies[a].Direction == 3) && l == false)
                    change = true;
                if ((Fireflies[a].Direction == 6 || Fireflies[a].Direction == 5 || Fireflies[a].Direction == 7) && r == false)
                    change = true;
                if ((Fireflies[a].Direction == 4 || Fireflies[a].Direction == 3 || Fireflies[a].Direction == 5) && u == false)
                    change = true;
                if (rand() % 20 == 0)
                    change = true;

                if (change)
                    TurnDyn(a, l, r, u, d);

                // redraw old lightmap

                for (dx = 0; dx <= 1; dx++) {
                    for (dy = 0; dy <= 1; dy++) {
                        WorldLightR[Fireflies[a].X + dx + (Fireflies[a].Y + dy) * 128] = MapLightR[Fireflies[a].X + dx + (Fireflies[a].Y + dy) * 128];
                        WorldLightG[Fireflies[a].X + dx + (Fireflies[a].Y + dy) * 128] = MapLightG[Fireflies[a].X + dx + (Fireflies[a].Y + dy) * 128];
                        WorldLightB[Fireflies[a].X + dx + (Fireflies[a].Y + dy) * 128] = MapLightB[Fireflies[a].X + dx + (Fireflies[a].Y + dy) * 128];
                    }
                }

                // Horizontal
                if ((Fireflies[a].Direction == 6 || Fireflies[a].Direction == 5 || Fireflies[a].Direction == 7) && r)
                    Fireflies[a].SX += 1;
                if (Fireflies[a].SX > 19) {
                    Fireflies[a].X += 1;
                    Fireflies[a].SX = 0;
                }
                if ((Fireflies[a].Direction == 1 || Fireflies[a].Direction == 2 || Fireflies[a].Direction == 3) && l)
                    Fireflies[a].SX -= 1;
                if (Fireflies[a].SX < 0) {
                    Fireflies[a].X -= 1;
                    Fireflies[a].SX = 19;
                }
                // Vertikal
                if ((Fireflies[a].Direction == 0 || Fireflies[a].Direction == 1 || Fireflies[a].Direction == 7) && d)
                    Fireflies[a].SY += 1;
                if (Fireflies[a].SY > 19) {
                    Fireflies[a].Y += 1;
                    Fireflies[a].SY = 0;
                }
                if ((Fireflies[a].Direction == 4 || Fireflies[a].Direction == 3 || Fireflies[a].Direction == 5) && u)
                    Fireflies[a].SY -= 1;
                if (Fireflies[a].SY < 0) {
                    Fireflies[a].Y -= 1;
                    Fireflies[a].SY = 19;
                }
            }
        }

        for (a = 0; a < DynamicLight_Max; a++) {
            if (Fireflies[a].Enable == true) {
                sx = Fireflies[a].SX;
                sy = Fireflies[a].SY;

                Rcol1 = float((Fireflies[a].Rcolor / 20 * (sx)) / 20 * (sy)) + WorldLightR[Fireflies[a].X + 1 + (Fireflies[a].Y + 1) * 128];
                Gcol1 = float((Fireflies[a].Gcolor / 20 * (sx)) / 20 * (sy)) + WorldLightG[Fireflies[a].X + 1 + (Fireflies[a].Y + 1) * 128];
                Bcol1 = float((Fireflies[a].Bcolor / 20 * (sx)) / 20 * (sy)) + WorldLightB[Fireflies[a].X + 1 + (Fireflies[a].Y + 1) * 128];

                Rcol2 = float((Fireflies[a].Rcolor / 20 * (20 - sx)) / 20 * (sy)) + WorldLightR[Fireflies[a].X + (Fireflies[a].Y + 1) * 128];
                Gcol2 = float((Fireflies[a].Gcolor / 20 * (20 - sx)) / 20 * (sy)) + WorldLightG[Fireflies[a].X + (Fireflies[a].Y + 1) * 128];
                Bcol2 = float((Fireflies[a].Bcolor / 20 * (20 - sx)) / 20 * (sy)) + WorldLightB[Fireflies[a].X + (Fireflies[a].Y + 1) * 128];

                Rcol3 = float((Fireflies[a].Rcolor / 20 * (sx)) / 20 * (20 - sy)) + WorldLightR[Fireflies[a].X + 1 + (Fireflies[a].Y) * 128];
                Gcol3 = float((Fireflies[a].Gcolor / 20 * (sx)) / 20 * (20 - sy)) + WorldLightG[Fireflies[a].X + 1 + (Fireflies[a].Y) * 128];
                Bcol3 = float((Fireflies[a].Bcolor / 20 * (sx)) / 20 * (20 - sy)) + WorldLightB[Fireflies[a].X + 1 + (Fireflies[a].Y) * 128];

                Rcol4 = float((Fireflies[a].Rcolor / 20 * (20 - sx)) / 20 * (20 - sy)) + WorldLightR[Fireflies[a].X + (Fireflies[a].Y) * 128];
                Gcol4 = float((Fireflies[a].Gcolor / 20 * (20 - sx)) / 20 * (20 - sy)) + WorldLightG[Fireflies[a].X + (Fireflies[a].Y) * 128];
                Bcol4 = float((Fireflies[a].Bcolor / 20 * (20 - sx)) / 20 * (20 - sy)) + WorldLightB[Fireflies[a].X + (Fireflies[a].Y) * 128];

                if (Rcol1 > 255)
                    Rcol1 = 255;
                if (Gcol1 > 255)
                    Gcol1 = 255;
                if (Bcol1 > 255)
                    Bcol1 = 255;

                if (Rcol2 > 255)
                    Rcol2 = 255;
                if (Gcol2 > 255)
                    Gcol2 = 255;
                if (Bcol2 > 255)
                    Bcol2 = 255;

                if (Rcol3 > 255)
                    Rcol3 = 255;
                if (Gcol3 > 255)
                    Gcol3 = 255;
                if (Bcol3 > 255)
                    Bcol3 = 255;

                if (Rcol4 > 255)
                    Rcol4 = 255;
                if (Gcol4 > 255)
                    Gcol4 = 255;
                if (Bcol4 > 255)
                    Bcol4 = 255;

                WorldLightR[Fireflies[a].X + (Fireflies[a].Y) * 128] = Rcol4;
                WorldLightG[Fireflies[a].X + (Fireflies[a].Y) * 128] = Gcol4;
                WorldLightB[Fireflies[a].X + (Fireflies[a].Y) * 128] = Bcol4;

                WorldLightR[Fireflies[a].X + 1 + (Fireflies[a].Y) * 128] = Rcol3;
                WorldLightG[Fireflies[a].X + 1 + (Fireflies[a].Y) * 128] = Gcol3;
                WorldLightB[Fireflies[a].X + 1 + (Fireflies[a].Y) * 128] = Bcol3;

                WorldLightR[Fireflies[a].X + (Fireflies[a].Y + 1) * 128] = Rcol2;
                WorldLightG[Fireflies[a].X + (Fireflies[a].Y + 1) * 128] = Gcol2;
                WorldLightB[Fireflies[a].X + (Fireflies[a].Y + 1) * 128] = Bcol2;

                WorldLightR[Fireflies[a].X + 1 + (Fireflies[a].Y + 1) * 128] = Rcol1;
                WorldLightG[Fireflies[a].X + 1 + (Fireflies[a].Y + 1) * 128] = Gcol1;
                WorldLightB[Fireflies[a].X + 1 + (Fireflies[a].Y + 1) * 128] = Bcol1;
            }
        }
    }
}

static void UpdateViewableLights(void)
{
    for (int yy = 13; yy > -4; yy--) {
        int y = yy + CamPosY;

        int minx = 4;
        int maxx = 7;

        if (yy == 13 || yy == 12 || yy == 11 || yy == 10 || yy == 9) {
            minx = 1;
            maxx = 10;
        }
        if (yy == 8) {
            minx = 0;
            maxx = 10;
        }
        if (yy == 7 || yy == 6 || yy == 5) {
            minx = 0;
            maxx = 11;
        }
        if (yy == 4 || yy == 3 || yy == 2) {
            minx = -1;
            maxx = 12;
        }
        if (yy == 1 || yy == 0 || yy == -1) {
            minx = -2;
            maxx = 13;
        }
        if (yy <= -2) {
            minx = -3;
            maxx = 14;
        }

        for (int xx = maxx; xx > minx; xx--) {
            // maplight color
            u8 col[3];
            int x = xx + CamPosX;
            GiveLight(x, y, col);
            ViewportMapLights[xx + 5][yy + 3] = RGB15((u8)col[0] >> 3, (u8)col[1] >> 3, (u8)col[2] >> 3);

            // reset Bump-mapping status
            if (x >= 0 && y >= 0 && y <= MapGetHr() && x <= MapGetWr()) {
                ViewportMapBumpGroundS[xx + 5][yy + 3] = Precalcdata[x][y] & (1 << B_BumpgroundS);
                ViewportMapBumpGroundW[xx + 5][yy + 3] = Precalcdata[x][y] & (1 << B_BumpgroundW);
                ViewportMapBumpGroundE[xx + 5][yy + 3] = Precalcdata[x][y] & (1 << B_BumpgroundE);
                ViewportMapBumpWallS[xx + 5][yy + 3]   = Precalcdata[x][y] & (1 << B_BumpwallS);
                ViewportMapBumpWallW[xx + 5][yy + 3]   = Precalcdata[x][y] & (1 << B_BumpwallW);
                ViewportMapBumpWallE[xx + 5][yy + 3]   = Precalcdata[x][y] & (1 << B_BumpwallE);
            }
        }
    }
}

static void UpdateAutobumps(void)
{
}

void UpdateDynamic(void)
{
    UpdateFireflies();
    UpdateViewableLights();
    UpdateAutobumps();
}
