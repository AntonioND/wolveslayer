// SPDX-License-Identifier: MIT
//
// Copyright (C) 2007 Christian Hoffmann (Payk)
// Copyright (C) 2024 Antonio Niño Díaz

#ifndef MUSIC_H__
#define MUSIC_H__

void InitSound(void);

// Give the filename and the ARM7 will do the rest to play that song
void StartSong(const char *Name);

void StopSong(void);

#endif // MUSIC_H__
