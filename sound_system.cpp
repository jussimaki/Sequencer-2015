#include "sound_system.h"

#include <cstring>

void SoundSystem::callback_(void *soundSystemObject, unsigned char *buffer, int len)
{
	SoundSystem *pointer = (SoundSystem*)soundSystemObject;

	pointer->callback(buffer, len);
}

void SoundSystem::callback(unsigned char *buffer, int len)
{
	
	memset(buffer, 0, sizeof(unsigned char)*len);

	if (sequencerFunctionPointer_)
	{
		sequencerFunctionPointer_(sequencerObject_, buffer, len, sampleRate_);
	}


}

