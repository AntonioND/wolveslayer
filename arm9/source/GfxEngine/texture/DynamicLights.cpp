#include "GfxEngine/3D.h"
#include "GfxEngine/input/Input.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/texture/Light.h"

extern u8 MapLightR[128 * 128];
extern u8 MapLightG[128 * 128];
extern u8 MapLightB[128 * 128];

u8 WorldLightR[128 * 128];
u8 WorldLightG[128 * 128];
u8 WorldLightB[128 * 128];

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
    int Richtung;
    u8 Rcolor;
    u8 Gcolor;
    u8 Bcolor;
} DynamicLightSource;

DynamicLightSource Fireflys[10];
int DynamicLightCount = -1;

void ResetDynamic(void)
{
    DynamicLightCount = -1;
    for (int a = 0; a < 10; a++)
        Fireflys[a].Enable = false;
}

void AddDynamic(int x, int y, u8 r, u8 g, u8 b)
{
    DynamicLightCount++;
    Fireflys[DynamicLightCount].X      = x;
    Fireflys[DynamicLightCount].Y      = y;
    Fireflys[DynamicLightCount].Rcolor = r;
    Fireflys[DynamicLightCount].Gcolor = g;
    Fireflys[DynamicLightCount].Bcolor = b;
    Fireflys[DynamicLightCount].Enable = true;
}

void TurnDyn(int a, bool l, bool r, bool u, bool d)
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

    Fireflys[a].Richtung = dirnew;
}

void UpdateFireFlys(void)
{
    extern int screenmode;
    int a, b;
    bool r, l, u, d;
    int sx, sy;
    int dx, dy;
    bool change;
    float Rcol1, Rcol2, Rcol3, Rcol4;
    float Gcol1, Gcol2, Gcol3, Gcol4;
    float Bcol1, Bcol2, Bcol3, Bcol4;

    if (screenmode < 3) {
        for (a = 0; a < 10; a++) {
            if (Fireflys[a].Enable == true) {
                l = true;
                r = true;
                u = true;
                d = true;

                sx = (Fireflys[a].SX - 10) / 2;
                sy = (Fireflys[a].SY - 10) / 2;

                for (b = 2; b < 8; b++) {
                    if (Passable(Fireflys[a].X, Fireflys[a].Y, sx + b, sy) == false)
                        u = false;
                    if (Passable(Fireflys[a].X, Fireflys[a].Y, sx + b, sy + 8) == false)
                        d = false;
                    if (Passable(Fireflys[a].X, Fireflys[a].Y, sx + 1, sy - 1 + b) == false)
                        l = false;
                    if (Passable(Fireflys[a].X, Fireflys[a].Y, sx + 8, sy - 1 + b) == false)
                        r = false;
                }

                if (Fireflys[a].X == MapGetWr())
                    r = false;
                if (Fireflys[a].Y == MapGetHr())
                    d = false;

                change = false;
                if ((Fireflys[a].Richtung == 0 || Fireflys[a].Richtung == 1 || Fireflys[a].Richtung == 7) && d == false)
                    change = true;
                if ((Fireflys[a].Richtung == 1 || Fireflys[a].Richtung == 2 || Fireflys[a].Richtung == 3) && l == false)
                    change = true;
                if ((Fireflys[a].Richtung == 6 || Fireflys[a].Richtung == 5 || Fireflys[a].Richtung == 7) && r == false)
                    change = true;
                if ((Fireflys[a].Richtung == 4 || Fireflys[a].Richtung == 3 || Fireflys[a].Richtung == 5) && u == false)
                    change = true;
                if (rand() % 20 == 0)
                    change = true;

                if (change)
                    TurnDyn(a, l, r, u, d);

                // redraw old lightmap

                for (dx = 0; dx <= 1; dx++) {
                    for (dy = 0; dy <= 1; dy++) {
                        WorldLightR[Fireflys[a].X + dx + (Fireflys[a].Y + dy) * 128] = MapLightR[Fireflys[a].X + dx + (Fireflys[a].Y + dy) * 128];
                        WorldLightG[Fireflys[a].X + dx + (Fireflys[a].Y + dy) * 128] = MapLightG[Fireflys[a].X + dx + (Fireflys[a].Y + dy) * 128];
                        WorldLightB[Fireflys[a].X + dx + (Fireflys[a].Y + dy) * 128] = MapLightB[Fireflys[a].X + dx + (Fireflys[a].Y + dy) * 128];
                    }
                }

                // Horizontal
                if ((Fireflys[a].Richtung == 6 || Fireflys[a].Richtung == 5 || Fireflys[a].Richtung == 7) && r)
                    Fireflys[a].SX += 1;
                if (Fireflys[a].SX > 19) {
                    Fireflys[a].X += 1;
                    Fireflys[a].SX = 0;
                }
                if ((Fireflys[a].Richtung == 1 || Fireflys[a].Richtung == 2 || Fireflys[a].Richtung == 3) && l)
                    Fireflys[a].SX -= 1;
                if (Fireflys[a].SX < 0) {
                    Fireflys[a].X -= 1;
                    Fireflys[a].SX = 19;
                }
                // Vertikal
                if ((Fireflys[a].Richtung == 0 || Fireflys[a].Richtung == 1 || Fireflys[a].Richtung == 7) && d)
                    Fireflys[a].SY += 1;
                if (Fireflys[a].SY > 19) {
                    Fireflys[a].Y += 1;
                    Fireflys[a].SY = 0;
                }
                if ((Fireflys[a].Richtung == 4 || Fireflys[a].Richtung == 3 || Fireflys[a].Richtung == 5) && u)
                    Fireflys[a].SY -= 1;
                if (Fireflys[a].SY < 0) {
                    Fireflys[a].Y -= 1;
                    Fireflys[a].SY = 19;
                }
            }
        }

        for (a = 0; a < 10; a++) {
            if (Fireflys[a].Enable == true) {
                sx = Fireflys[a].SX;
                sy = Fireflys[a].SY;

                Rcol1 = float((Fireflys[a].Rcolor / 20 * (sx)) / 20 * (sy)) + WorldLightR[Fireflys[a].X + 1 + (Fireflys[a].Y + 1) * 128];
                Gcol1 = float((Fireflys[a].Gcolor / 20 * (sx)) / 20 * (sy)) + WorldLightG[Fireflys[a].X + 1 + (Fireflys[a].Y + 1) * 128];
                Bcol1 = float((Fireflys[a].Bcolor / 20 * (sx)) / 20 * (sy)) + WorldLightB[Fireflys[a].X + 1 + (Fireflys[a].Y + 1) * 128];

                Rcol2 = float((Fireflys[a].Rcolor / 20 * (20 - sx)) / 20 * (sy)) + WorldLightR[Fireflys[a].X + (Fireflys[a].Y + 1) * 128];
                Gcol2 = float((Fireflys[a].Gcolor / 20 * (20 - sx)) / 20 * (sy)) + WorldLightG[Fireflys[a].X + (Fireflys[a].Y + 1) * 128];
                Bcol2 = float((Fireflys[a].Bcolor / 20 * (20 - sx)) / 20 * (sy)) + WorldLightB[Fireflys[a].X + (Fireflys[a].Y + 1) * 128];

                Rcol3 = float((Fireflys[a].Rcolor / 20 * (sx)) / 20 * (20 - sy)) + WorldLightR[Fireflys[a].X + 1 + (Fireflys[a].Y) * 128];
                Gcol3 = float((Fireflys[a].Gcolor / 20 * (sx)) / 20 * (20 - sy)) + WorldLightG[Fireflys[a].X + 1 + (Fireflys[a].Y) * 128];
                Bcol3 = float((Fireflys[a].Bcolor / 20 * (sx)) / 20 * (20 - sy)) + WorldLightB[Fireflys[a].X + 1 + (Fireflys[a].Y) * 128];

                Rcol4 = float((Fireflys[a].Rcolor / 20 * (20 - sx)) / 20 * (20 - sy)) + WorldLightR[Fireflys[a].X + (Fireflys[a].Y) * 128];
                Gcol4 = float((Fireflys[a].Gcolor / 20 * (20 - sx)) / 20 * (20 - sy)) + WorldLightG[Fireflys[a].X + (Fireflys[a].Y) * 128];
                Bcol4 = float((Fireflys[a].Bcolor / 20 * (20 - sx)) / 20 * (20 - sy)) + WorldLightB[Fireflys[a].X + (Fireflys[a].Y) * 128];

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

                WorldLightR[Fireflys[a].X + (Fireflys[a].Y) * 128] = Rcol4;
                WorldLightG[Fireflys[a].X + (Fireflys[a].Y) * 128] = Gcol4;
                WorldLightB[Fireflys[a].X + (Fireflys[a].Y) * 128] = Bcol4;

                WorldLightR[Fireflys[a].X + 1 + (Fireflys[a].Y) * 128] = Rcol3;
                WorldLightG[Fireflys[a].X + 1 + (Fireflys[a].Y) * 128] = Gcol3;
                WorldLightB[Fireflys[a].X + 1 + (Fireflys[a].Y) * 128] = Bcol3;

                WorldLightR[Fireflys[a].X + (Fireflys[a].Y + 1) * 128] = Rcol2;
                WorldLightG[Fireflys[a].X + (Fireflys[a].Y + 1) * 128] = Gcol2;
                WorldLightB[Fireflys[a].X + (Fireflys[a].Y + 1) * 128] = Bcol2;

                WorldLightR[Fireflys[a].X + 1 + (Fireflys[a].Y + 1) * 128] = Rcol1;
                WorldLightG[Fireflys[a].X + 1 + (Fireflys[a].Y + 1) * 128] = Gcol1;
                WorldLightB[Fireflys[a].X + 1 + (Fireflys[a].Y + 1) * 128] = Bcol1;
            }
        }
    }
}

void UpdateViewableLights(void)
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
                ViewportMapBumpGroundS[xx + 5][yy + 3] = Precalcdata[x][y] & (1 << BumpgroundS);
                ViewportMapBumpGroundW[xx + 5][yy + 3] = Precalcdata[x][y] & (1 << BumpgroundW);
                ViewportMapBumpGroundE[xx + 5][yy + 3] = Precalcdata[x][y] & (1 << BumpgroundE);
                ViewportMapBumpWallS[xx + 5][yy + 3]   = Precalcdata[x][y] & (1 << BumpwallS);
                ViewportMapBumpWallW[xx + 5][yy + 3]   = Precalcdata[x][y] & (1 << BumpwallW);
                ViewportMapBumpWallE[xx + 5][yy + 3]   = Precalcdata[x][y] & (1 << BumpwallE);
            }
        }
    }
}

void UpdateAutobumps(void)
{
}

void UpdateDynamic(void)
{
    UpdateFireFlys();
    UpdateViewableLights();
    UpdateAutobumps();
}
