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

extern void SndSetMemPool(void *memPool, u32 memPoolSize);
extern void SndPlayMOD(const void *modFile);
extern void SndStopMOD();
extern void SndPauseMOD(bool bPaused);
extern void SndSetMODCallback(MOD_CALLBACK callback);
extern void SoundSendCmd(SND_COMMAND_TYPE cmdType, u32 param32);
// extern bool checkMusicFade(LEVELS lvlIdx);
// extern void updateMusic(LEVELS lvlIdx);

// ----------

#ifdef __cplusplus
}
#endif

#endif // SOUND9_H
