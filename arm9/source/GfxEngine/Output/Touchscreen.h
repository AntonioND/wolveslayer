// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef GFXENGINE_OUTPUT_TOUCHSCREEN_H__
#define GFXENGINE_OUTPUT_TOUCHSCREEN_H__

#include <nds.h>

// Touchscreen Functions

typedef enum {
    ScreenModeLoading = -1,
    ScreenModeNormal  = 0,
    ScreenModeTextBox = 2,
    ScreenModeItem    = 3,
    ScreenModePause   = 4,
} ScreenModes;

extern ScreenModes screenmode;

extern u8 Font1[668 * 11];
extern u16 scrL_bin[256 * 192];
extern u16 touch_bin[256 * 192];
extern u16 textbox_pal[256];

// Displays loading bg
void ScreenModeLOADING(void);

void UpdateBar(void);

// Displays BG (cls)
void ScreenMode(void);

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

#endif // GFXENGINE_OUTPUT_TOUCHSCREEN_H__
