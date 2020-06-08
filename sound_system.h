#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H

class SoundSystem
{
private:
	int sampleRate_;

	void(*sequencerFunctionPointer_)(void *, unsigned char*, int, int);
	void *sequencerObject_;

public:
	SoundSystem(int sampleRate) : sampleRate_(sampleRate), sequencerFunctionPointer_(0), sequencerObject_(0) {}
	
	void setSequencer(void(*sequencerFunctionPointer)(void*, unsigned char*, int, int), void *sequencerObject)
	{
		sequencerFunctionPointer_ = sequencerFunctionPointer;
		sequencerObject_ = sequencerObject;
	}

	static void callback_(void *soundSystemObject, unsigned char *buffer, int len);
	void callback(unsigned char *buffer, int len);


};

#endif