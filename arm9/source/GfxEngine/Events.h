#ifndef GFXENGINE_EVENTS_H__
#define GFXENGINE_EVENTS_H__

#include <stdbool.h>

extern char EventSpeech[10][256]; // to hold 10 textes for event with 256 chars
extern int EventSpeechCount;      // Holds the Number of speeches a event has...

extern bool Gameended;

// Events
void ResetEvent(void);
void AddTexttoEvent(char txt[256]); // Adds an event which will be turn on when starting a map
void CheckEndGame(void);

#endif // GFXENGINE_EVENTS_H__
