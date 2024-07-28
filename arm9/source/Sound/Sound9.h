// SPDX-License-Identifier: WTFPL
//
// SPDX-FileContributor: LiraNuna, 2006
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// ----- Change log -----
// 05/08/05 - Created
// ----------------------

#ifndef SOUND9_H
#define SOUND9_H

#include <nds.h>

#include "SoundCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----- Functions -----

void SndInit9();
void SndSetMemPool(void *memPool, u32 memPoolSize);
void SndPlayMOD(const void *modFile);
void SndStopMOD();
void SndPauseMOD(bool bPaused);
void SndSetMODCallback(MOD_CALLBACK callback);
void SoundSendCmd(SND_COMMAND_TYPE cmdType, u32 param32);

// ----------

#ifdef __cplusplus
}
#endif

#endif // SOUND9_H
