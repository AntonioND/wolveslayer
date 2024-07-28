// SPDX-License-Identifier: WTFPL
//
// SPDX-FileContributor: LiraNuna, 2006
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// ----- Change log -----
// 05/08/05 - Created
// ----------------------

#ifndef SOUNDCOMMON_H
#define SOUNDCOMMON_H

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

// ----- Constants -----

typedef void (*MOD_CALLBACK)(u32 param, bool bRowTick);

typedef enum _SND_COMMAND_TYPE {
    SND_CMD_SETMEMPOOL,  // Param32: Address, Param[0 to 2]: Size
    SND_CMD_PLAYSONG,    // Param32: Pointer to song file (standard .mod)
    SND_CMD_STOPSONG,    // No params
    SND_CMD_PAUSESONG,   // Param[0]: 1=pause, 0=unpause
    SND_CMD_SETCALLBACK, // Param32: MOD_CALLBACK function pointer
    SND_CMD_SETVOLUME,   // Param32: volume

    SND_CMD_NUM,

} SND_COMMAND_TYPE;

// ----- Structures -----

// Commands filled out by ARM9 to tell ARM7 what to do
typedef struct _SND_COMMAND {
    u8 cmdType;  // SND_COMMAND_TYPE enum
    u8 param[3]; // Depend on cmdType
    u32 param32; // Mainly for commands that need to send a pointer

} SND_COMMAND;

#ifdef __cplusplus
}
#endif

#endif // SOUNDCOMMON_H
