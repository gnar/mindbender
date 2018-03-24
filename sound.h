#ifndef SOUND_H
#define SOUND_H

//#include <fmod.h>
//#include <fmod_errors.h>

typedef int FMOD_SYSTEM;

void InitSound(int freq = 0);

void DoneSound();

FMOD_SYSTEM *SoundSystem();

#endif

