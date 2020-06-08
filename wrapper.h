#ifndef WRAPPER_H
#define WRAPPER_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <fstream>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#else
#error NOT_WINDOWS
#endif

#include "keysymbols.h"


#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1

struct Time
{
	long long int microSeconds;
};



struct Timer
{

#ifdef _WIN32

	LARGE_INTEGER start_time, counts;

#else
#error NOT_WINDOWS
#endif

};

struct BMPData
{
	unsigned int width, height;
	unsigned int orig_width, orig_height;
	unsigned char *data;
};



struct Sprite
{
	unsigned int width, height;
	unsigned int orig_width, orig_height;

	unsigned char *data;
	GLuint openGLtexture;
};

struct Canvas
{
	Sprite *sprite;
	unsigned int width, height;
	unsigned char *data;
};


struct Rect
{
	int x, y, width, height;
};

struct Wave
{
	short *buffer;
	short channels;
	unsigned int sampleRate;

	unsigned int samplesPerChannel;
};

struct InputData
{
	int mouse_move_x, mouse_move_y;

	Uint8 KEY_DOWN[KEY_LAST];
	
	unsigned char PAD_BUTTON_DOWN[24];

	unsigned char MOUSE_BUTTON_DOWN[3];
};

struct BinaryFile
{
	std::ifstream file;
	unsigned int endianness;
};


struct Lock
{

#ifdef _WIN32
	CRITICAL_SECTION cs;
#else
#error NOT_WINDOWS
#endif

};


class Wrapper
{
private:
	int xResolution_, yResolution_;
	SDL_Surface *SDLScreen_;

	Lock timerLock_;

	InputData inputData_;
	unsigned int endianness_;
	void (*soundSystemFunctionPointer_)(void *, unsigned char*, int);
	void *soundSystemObject_;
public:
	Wrapper() : xResolution_(0), yResolution_(0), SDLScreen_(0), soundSystemFunctionPointer_(0), soundSystemObject_(0)
	{
		initLock(&timerLock_);
		unsigned int tester = 1;
		char *ptr = (char*)&tester;

		if (*ptr == 1) endianness_ = LITTLE_ENDIAN;
		else endianness_ = BIG_ENDIAN;
	}

	~Wrapper()
	{
		deleteLock(&timerLock_);
	}

	void setSoundSystem(void(*soundSystemFunctionPointer)(void*, unsigned char*, int), void *soundSystemObject)
	{
		soundSystemFunctionPointer_ = soundSystemFunctionPointer;
		soundSystemObject_ = soundSystemObject;
	}


	void shutDownSound()
	{
		SDL_CloseAudio();
	}

	
	
	bool initialize(int x_resolution, int y_resolution, unsigned int sampleRate);
	bool shutDown();

	void getCanvas(Canvas *canvas, Sprite *sprite);
	void releaseCanvas(Canvas *canvas);
	void attachCanvas(Canvas *canvas);

	bool emptySprite(Sprite *sprite, unsigned int width, unsigned int height);
	bool loadSprite(Sprite *sprite, char *fileName);
	void releaseSprite(Sprite *sprite);

	bool drawSprite(Sprite *sprite, Rect *dest, Rect *source, Rect *clip, bool colorkey, int alpha);
	void drawRectangle(Rect *dest, Rect *clip, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha);

	BMPData* readBMPData(char *fileName);
	void reverseBMPData(BMPData *data);

	void clearScreen();
	void flip();

	int getXResolution();
	int getYResolution();


	bool loadWave(Wave *wave, char *filename);
	void releaseWave(Wave *wave);


	InputData *getInputData();
	bool readInput();
		

	//bool doesCodeEqualString(char *string, unsigned int code);

	void openBinaryFile(char *fileName, BinaryFile *file, unsigned int endianness);
	void closeBinaryFile(BinaryFile *file);

	void readToBuffer(char *buffer, unsigned int byteAmount, BinaryFile *file);
	unsigned char readUByte(BinaryFile *file);
	unsigned short readUWord(BinaryFile *file);
	unsigned int readUDword(BinaryFile *file);

	char readByte(BinaryFile *file);
	short readWord(BinaryFile *file);
	int readDword(BinaryFile *file);

	// audio stuff

	void audioCallback(Uint8 *stream, int len);
	static void audioCallback_(void *wrapperObject, Uint8 *stream, int len);

	// timer stuff

	void startTimer(Timer *timer);
	double getTimerSeconds(Timer *timer);

	// lock stuff

	void initLock(Lock *lock);
	void deleteLock(Lock *lock);

	void lock(Lock *lock);
	void unlock(Lock *lock);


};

#endif