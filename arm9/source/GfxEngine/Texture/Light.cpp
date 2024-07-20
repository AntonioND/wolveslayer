#include "GfxEngine/3D.h"
#include "GfxEngine/Input/Input.h"
#include "GfxEngine/MapLoad.h"
#include "GfxEngine/Output/Touchscreen.h"
#include "GfxEngine/PreCalc.h"
#include "GfxEngine/Script/Script_Load.h"
#include "GfxEngine/Texture/DynamicLights.h"

int Blend = 8;
u8 EnvR = 255, EnvG = 255, EnvB = 255;

static u8 EnvR3, EnvG3, EnvB3;

static int Time, Time2 = 1200;

static int MdlLight[4][3];
static u16 MdlLight16[4];

static int rotation;

void SetRot(int rot)
{
    rotation = rot;
}

void RunTime(void)
{
    if (screenmode < ScreenModeTextBox)
        Time2++;
    if (Time2 > 2400)
        Time2 = 0;

    if (IsMapOutside())
        Time = Time2;
    else
        Time = 0;

    int r, g, b, r2, g2, b2;
    int interpol = 0;

    if (Time >= 0 && Time <= 600) {
        r = 64, g = 64, b = 128;
        r2 = 222, g2 = 128, b2 = 128;

        interpol = Time;
    }

    if (Time > 600 && Time <= 1200) {
        r = 222, g = 128, b = 128;
        r2 = 256, g2 = 256, b2 = 256;

        interpol = Time - 600;
    }

    if (Time > 1200 && Time <= 1800) {
        r = 256, g = 256, b = 256;
        r2 = 222, g2 = 128, b2 = 128;

        interpol = Time - 1200;
    }

    if (Time > 1800 && Time <= 2400) {
        r = 222, g = 128, b = 128;
        r2 = 64, g2 = 64, b2 = 128;
        interpol = Time - 1800;
    }

    float interpola = (float)interpol / 600;
    int Envr, Envg, Envb;
    Envr = (interpola * r2) + ((1 - interpola) * r);
    Envg = (interpola * g2) + ((1 - interpola) * g);
    Envb = (interpola * b2) + ((1 - interpola) * b);
    if (Envr <= 255)
        EnvR = Envr;
    if (Envg <= 255)
        EnvG = Envg;
    if (Envb <= 255)
        EnvB = Envb;
    if (Blend != 8) {
        EnvR = EnvR / 8 * Blend;
        EnvG = EnvG / 8 * Blend;
        EnvB = EnvB / 8 * Blend;
    }

    EnvR3 = EnvR / 3;
    EnvG3 = EnvG / 3;
    EnvB3 = EnvB / 3;
}

void GiveLight(int x, int y, u8 *col)
{
    v16x4 pointer = Terrain[x][y];

    if (x < 0 || x > 127 || y < 0 || y > 127) {
        col[0] = 0;
        col[1] = 0;
        col[2] = 0;
        return;
    }

    int r  = EnvR;
    int g  = EnvG;
    int b  = EnvB;
    int nx = 0;
    int ny = 0;
    // OK we have the standard terrain light...
    // but lets increse/decrese depending on NORMALFACE (a math incorect version but looks good and goes fast)
    // extern bool TerrainIsStair[128][128];

    // if (TerrainIsStair[x][y] == false) {
    if (pointer.v[0] > pointer.v[1])
        nx = 1;
    if (pointer.v[0] < pointer.v[1])
        nx = -1;
    if (pointer.v[0] > pointer.v[2])
        ny = 1;
    if (pointer.v[0] < pointer.v[2])
        ny = -1;
    // }

    r += WorldLightR[x + (y * 128)];
    g += WorldLightG[x + (y * 128)];
    b += WorldLightB[x + (y * 128)];

    r += EnvR3 * nx;
    r += EnvR3 * ny;
    g += EnvG3 * nx;
    g += EnvG3 * ny;
    b += EnvB3 * nx;
    b += EnvB3 * ny;

    if (r > 255)
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;
    if (r < 0)
        r = 0;
    if (g < 0)
        g = 0;
    if (b < 0)
        b = 0;

    if (Blend != 8) {
        r = r / 8 * Blend;
        g = g / 8 * Blend;
        b = b / 8 * Blend;
    }

    col[0] = (u8)r;
    col[1] = (u8)g;
    col[2] = (u8)b;
}

// this is used for terrain only..very simple but beatifull
void GrapLight(int x, int y)
{
    // u8 col[3];
    // GiveLight(x, y, col);
    // glColor3b(col[0], col[1], col[2]);
    // int xx = x - CamPosX;
    // int yy = y - CamPosY;
    GFX_COLOR = (u32)ViewportMapLights[x - CamPosX + 5][y - CamPosY + 3];
}

// internal interpolation function
static void GrapLightInterpolate(int x, int y, float sx, float sy, int *col)
{
    v16 ssx = floattov16(sx);
    v16 ssy = floattov16(sy);
    v16 msx = inttov16(1) - floattov16(sx);
    v16 msy = inttov16(1) - floattov16(sy);

    f32 r1 = inttof32(WorldLightR[x + (y * 128)]);
    f32 g1 = inttof32(WorldLightG[x + (y * 128)]);
    f32 b1 = inttof32(WorldLightB[x + (y * 128)]);
    f32 r2 = inttof32(WorldLightR[x + 1 + (y * 128)]);
    f32 g2 = inttof32(WorldLightG[x + 1 + (y * 128)]);
    f32 b2 = inttof32(WorldLightB[x + 1 + (y * 128)]);
    f32 r3 = inttof32(WorldLightR[x + ((y + 1) * 128)]);
    f32 g3 = inttof32(WorldLightG[x + ((y + 1) * 128)]);
    f32 b3 = inttof32(WorldLightB[x + ((y + 1) * 128)]);
    f32 r4 = inttof32(WorldLightR[x + 1 + ((y + 1) * 128)]);
    f32 g4 = inttof32(WorldLightG[x + 1 + ((y + 1) * 128)]);
    f32 b4 = inttof32(WorldLightB[x + 1 + ((y + 1) * 128)]);

    // Ok now we got values for 4 corners
    // we just need to interpolate them using sx,sy

    f32 rl = mulf32(r3, ssy) + mulf32(r1, msy);
    f32 gl = mulf32(g3, ssy) + mulf32(g1, msy);
    f32 bl = mulf32(b3, ssy) + mulf32(b1, msy);

    f32 rr = mulf32(r4, ssy) + mulf32(r2, msy);
    f32 gr = mulf32(g4, ssy) + mulf32(g2, msy);
    f32 br = mulf32(b4, ssy) + mulf32(b2, msy);

    f32 r = mulf32(rr, ssx) + mulf32(rl, msx);
    f32 g = mulf32(gr, ssx) + mulf32(gl, msx);
    f32 b = mulf32(br, ssx) + mulf32(bl, msx);

    // we have what we neededlets use the color
    col[0] = (u8)f32toint(r);
    col[1] = (u8)f32toint(g);
    col[2] = (u8)f32toint(b);
}

// This should be called before rendering a model...
// it graps lights for 4 dirrections and hold it so rendering the mdl goes fast(hopefully)
// it uses interpolation...just neccecary for moving models
void SetMdlLights(int pos[2], float softpos[2], float radius, int rot)
{
    if (rot >= 0)
        rotation = 0;
    if (rot >= 127)
        rotation = 1;
    if (rot >= 255)
        rotation = 2;
    if (rot >= 383)
        rotation = 3;

    int posx1 = pos[0], posy1 = pos[1];
    int posx2 = pos[0], posy2 = pos[1];
    int posx3 = pos[0], posy3 = pos[1];
    int posx4 = pos[0], posy4 = pos[1];

    float sposx1 = softpos[0] - radius;
    if (sposx1 < 0) {
        sposx1++;
        posx1--;
    }
    float sposx2 = softpos[0] + radius;
    if (sposx2 > 1) {
        sposx2--;
        posx2++;
    }
    float sposx3 = softpos[0] - radius;
    if (sposx3 < 0) {
        sposx3++;
        posx3--;
    }
    float sposx4 = softpos[0] + radius;
    if (sposx4 > 1) {
        sposx4--;
        posx4++;
    }

    float sposy1 = softpos[1] - radius;
    if (sposy1 < 0) {
        sposy1++;
        posy1--;
    }
    float sposy2 = softpos[1] - radius;
    if (sposy2 < 0) {
        sposy2++;
        posy2--;
    }
    float sposy3 = softpos[1] + radius;
    if (sposy3 > 1) {
        sposy3--;
        posy3++;
    }
    float sposy4 = softpos[1] + radius;
    if (sposy4 > 1) {
        sposy4--;
        posy4++;
    }

    GrapLightInterpolate(posx1, posy1, sposx1, sposy1, &MdlLight[0][0]);
    GrapLightInterpolate(posx2, posy2, sposx2, sposy2, &MdlLight[1][0]);
    GrapLightInterpolate(posx3, posy3, sposx3, sposy3, &MdlLight[2][0]);
    GrapLightInterpolate(posx4, posy4, sposx4, sposy4, &MdlLight[3][0]);

    MdlLight[0][0] += EnvR;
    MdlLight[0][1] += EnvG;
    MdlLight[0][2] += EnvB;
    MdlLight[1][0] += EnvR;
    MdlLight[1][1] += EnvG;
    MdlLight[1][2] += EnvB;
    MdlLight[2][0] += EnvR;
    MdlLight[2][1] += EnvG;
    MdlLight[2][2] += EnvB;
    MdlLight[3][0] += EnvR;
    MdlLight[3][1] += EnvG;
    MdlLight[3][2] += EnvB;

    for (int a = 0; a < 3; a++) {
        if (MdlLight[0][a] > 255)
            MdlLight[0][a] = 255;
        if (MdlLight[1][a] > 255)
            MdlLight[1][a] = 255;
        if (MdlLight[2][a] > 255)
            MdlLight[2][a] = 255;
        if (MdlLight[3][a] > 255)
            MdlLight[3][a] = 255;

        if (Blend != 8)
            MdlLight[0][a] = MdlLight[0][a] / 8 * Blend;
        if (Blend != 8)
            MdlLight[1][a] = MdlLight[1][a] / 8 * Blend;
        if (Blend != 8)
            MdlLight[2][a] = MdlLight[2][a] / 8 * Blend;
        if (Blend != 8)
            MdlLight[3][a] = MdlLight[3][a] / 8 * Blend;
    }

    MdlLight16[0] = RGB15((u8)MdlLight[0][0] >> 3, (u8)MdlLight[0][1] >> 3, (u8)MdlLight[0][2] >> 3);
    MdlLight16[1] = RGB15((u8)MdlLight[1][0] >> 3, (u8)MdlLight[1][1] >> 3, (u8)MdlLight[1][2] >> 3);
    MdlLight16[2] = RGB15((u8)MdlLight[2][0] >> 3, (u8)MdlLight[2][1] >> 3, (u8)MdlLight[2][2] >> 3);
    MdlLight16[3] = RGB15((u8)MdlLight[3][0] >> 3, (u8)MdlLight[3][1] >> 3, (u8)MdlLight[3][2] >> 3);
}

// this is way faster then other methods(inertpolate) and used for Obj which doesnt move
void SetObjLights(int pos[2], int rot)
{
    if (rot >= 0)
        rotation = 0;
    if (rot >= 127)
        rotation = 1;
    if (rot >= 255)
        rotation = 2;
    if (rot >= 383)
        rotation = 3;

    int xx = pos[0] - CamPosX;
    int yy = pos[1] - CamPosY;

    MdlLight16[0] = (u32)ViewportMapLights[xx + 5][yy + 3];
    MdlLight16[1] = (u32)ViewportMapLights[xx + 6][yy + 3];
    MdlLight16[2] = (u32)ViewportMapLights[xx + 5][yy + 4];
    MdlLight16[3] = (u32)ViewportMapLights[xx + 6][yy + 4];
}

void SetObjLightsSelfilluminated(void)
{
    u32 white;
    u8 zw;

    if (Blend != 8)
        zw = (255 / 8 * Blend);

    if (Blend == 8)
        white = (u32)RGB15((u8)255 >> 3, (u8)255 >> 3, (u8)255 >> 3);
    else
        white = (u32)RGB15((u8)zw >> 3, (u8)zw >> 3, (u8)zw >> 3);

    MdlLight16[0] = white;
    MdlLight16[1] = white;
    MdlLight16[2] = white;
    MdlLight16[3] = white;
}

int GetLightRot(void)
{
    return rotation;
}

void GrapMd2Light(u8 Index, u16 &buff)
{
    buff = MdlLight16[Index];
}
