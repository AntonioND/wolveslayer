// SPDX-License-Identifier: WTFPL
//
// SPDX-FileContributor: LiraNuna, 2006
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// ----- Change log -----
// 05/09/05 - Created.
// ----------------------

#include <string.h>

#include <nds.h>

#include "Sound9.h"
#include "SoundCommon.h"

#define MEMPOOL_SIZE (512 * 1024)
static u32 pool[MEMPOOL_SIZE / 4];

// Call once on startup
void SndInit9()
{
    DC_FlushRange(&pool, MEMPOOL_SIZE);
    SndSetMemPool(&pool, MEMPOOL_SIZE);
}

// Send command to ARM7 to set the memory area to load MOD files into
// for playing. Must be called at least once at startup, but can be
// called repeatedly to change the size of the sound area as needed.
//
// Args:
//   memPool        Pointer to an area in main memory.
//   memPoolSize    Size of the area. Must be at least the size
//                  of your largest MOD file.
void SndSetMemPool(void *memPool, u32 memPoolSize)
{
    SND_COMMAND cmd = { 0 };

    sassert(memPool != NULL, "NULL buffer");
    sassert(memPoolSize < (1 << 24), "Too big"); // Only 3 bytes available in parameters

    cmd.cmdType  = SND_CMD_SETMEMPOOL;
    cmd.param[0] = (u8)(memPoolSize);
    cmd.param[1] = (u8)(memPoolSize >> 8);
    cmd.param[2] = (u8)(memPoolSize >> 16);
    cmd.param32  = (u32)memPool;

    fifoSendDatamsg(FIFO_USER_01, sizeof(cmd), (u8 *)&cmd);
}

// Send command to ARM7 to play a song.
//
// Args:
//   modFile        Pointer to a standard .mod file.
void SndPlayMOD(const void *modFile)
{
    SND_COMMAND cmd = { 0 };

    cmd.cmdType = SND_CMD_PLAYSONG;
    cmd.param32 = (u32)modFile;

    fifoSendDatamsg(FIFO_USER_01, sizeof(cmd), (u8 *)&cmd);
}

// Send command to ARM7 to stop the song.
void SndStopMOD()
{
    SND_COMMAND cmd = { 0 };

    cmd.cmdType = SND_CMD_STOPSONG;

    fifoSendDatamsg(FIFO_USER_01, sizeof(cmd), (u8 *)&cmd);
}

// Send command to ARM7 to pause or unpause the song.
void SndPauseMOD(bool bPaused)
{
    SND_COMMAND cmd = { 0 };

    cmd.cmdType  = SND_CMD_STOPSONG;
    cmd.param[0] = (u8)bPaused;

    fifoSendDatamsg(FIFO_USER_01, sizeof(cmd), (u8 *)&cmd);
}

// Send command to ARM7 to set the MOD callback function.
// Callback is triggered by E0x effects in the song.
void SndSetMODCallback(MOD_CALLBACK callback)
{
    SND_COMMAND cmd = { 0 };

    cmd.cmdType = SND_CMD_SETCALLBACK;
    cmd.param32 = (u32)callback;

    fifoSendDatamsg(FIFO_USER_01, sizeof(cmd), (u8 *)&cmd);
}

void SoundSendCmd(SND_COMMAND_TYPE cmdType, u32 param32)
{
    SND_COMMAND cmd = { 0 };

    cmd.cmdType = cmdType;
    cmd.param32 = param32;

    fifoSendDatamsg(FIFO_USER_01, sizeof(cmd), (u8 *)&cmd);
}
