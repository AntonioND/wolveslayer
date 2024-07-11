#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <nds/arm9/console.h>
#include <nds/arm9/sound.h>
using namespace std;
#include <list>

#define DEBUG 0

#ifndef __MAIN_H__
#define __MAIN_H__


	// Variables
extern volatile u32 frameCounter;
extern volatile u16 KEYS;
extern volatile u16 HKEYS;

	// Functions
extern int main(void);

#endif
