#ifndef GFXENGINE_EVENTS_H__
#define GFXENGINE_EVENTS_H__

#include <stdbool.h>

#include "GfxEngine/Limits.h"

extern char EventSpeech[EventSpeech_Max][256]; // to hold textes for event with 256 chars
extern int EventSpeechCount;                   // Holds the Number of speeches a event has...

extern bool Gameended;

// Events
void ResetEvent(void);
void AddTexttoEvent(char txt[256]); // Adds an event which will be turn on when starting a map
void CheckEndGame(void);

#endif // GFXENGINE_EVENTS_H__
