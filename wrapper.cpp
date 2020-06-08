#include "wrapper.h"


void Wrapper::audioCallback_(void *wrapperObject, Uint8 *stream, int len)
{
	Wrapper *wrapper = (Wrapper *)wrapperObject;

	if(wrapper)
		wrapper->audioCallback((unsigned char*)stream, len);
}

void Wrapper::audioCallback(Uint8 *stream, int len)
{
	if (soundSystemFunctionPointer_)
	{
		soundSystemFunctionPointer_(soundSystemObject_, stream, len);
	}
}

bool Wrapper::initialize(int x_resolution, int y_resolution, unsigned int sampleRate)
{
	xResolution_ = x_resolution;
	yResolution_ = y_resolution;

	
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) return false;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 

	int flags = SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGL;

	SDLScreen_ = SDL_SetVideoMode(xResolution_, yResolution_, 32, flags);

	if(!SDLScreen_) return false;
	
	SDL_ShowCursor(0);
	//SDL_WarpMouse(xResolution_ / 2, yResolution_ / 2);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	glOrtho(0, xResolution_, 0, yResolution_, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);


	if (soundSystemFunctionPointer_)
	{
		SDL_AudioSpec fmt;
		fmt.freq = sampleRate;
		fmt.format = AUDIO_S16;
		fmt.channels = 2;
		fmt.samples = 512;
		fmt.callback = &Wrapper::audioCallback_;
		fmt.userdata = this;

		if (SDL_OpenAudio(&fmt, NULL) < 0) return false;

		SDL_PauseAudio(0);
	}


	return true;
}

bool Wrapper::shutDown()
{
	SDL_Quit();

	return true;
}

int Wrapper::getXResolution()
{
	return xResolution_;
}

int Wrapper::getYResolution()
{
	return yResolution_;
}

void Wrapper::reverseBMPData(BMPData *data)
{
	unsigned char *row_data;

	row_data = new unsigned char[4 * data->width];

	for (unsigned int row = 0; row <= data->height / 2; row++)
	{

		// copy the bottom row to row_data
		memcpy(
			row_data,
			&(data->data[data->width * 4 * (data->height - 1 - row)]), 4 * sizeof(unsigned char)*data->width);

		// copy the top row to bottom row
		memcpy(
			&(data->data[data->width * 4 * (data->height - 1 - row)]),
			&(data->data[data->width * 4 * (row)]), 4 * sizeof(unsigned char)*data->width);

		// copy row_data to top row
		memcpy(
			&(data->data[data->width * 4 * (row)]),
			row_data, 4 * sizeof(unsigned char)*data->width);
	}

	delete[]row_data;
}

BMPData* Wrapper::readBMPData(char *fileName)
{
	BMPData *structure = new BMPData;

	//std::ifstream file(filename, std::ios_base::binary);

	BinaryFile file;
	openBinaryFile(fileName, &file, LITTLE_ENDIAN);

	unsigned short type;
	unsigned int useless_dword;
	unsigned short useless_word;
	unsigned short bits;
	unsigned int size;

	type = readUWord(&file);

	
	if (type != 0x4D42) { delete structure; closeBinaryFile(&file); return 0; }

	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);

	structure->orig_width = readUDword(&file);
	structure->orig_height = readUDword(&file);


	// the texture width and height must be in a power of 2
	unsigned int correctWidth = 1;
	while (correctWidth < structure->orig_width) correctWidth *= 2;

	unsigned int correctHeight = 1;
	while (correctHeight < structure->orig_height) correctHeight *= 2;

	unsigned int oldWidth = structure->orig_width;
	unsigned int oldHeight = structure->orig_height;

	structure->width = correctWidth;
	structure->height = correctHeight;

	structure->data = new unsigned char[structure->width*structure->height * 4];

	useless_word = readUWord(&file);

	bits = readUWord(&file);

	if (bits != 24) { delete[]structure->data; delete structure; closeBinaryFile(&file); return 0; }

	useless_dword = readUDword(&file);

	size = readUDword(&file);

	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);
	
	// the amount of useless bytes to make the bitmap's horizontal byte amount to be divisible by 4
	unsigned char useless_bytes = (4 - ((oldWidth * 3) % 4)) % 4;

	unsigned char *dataChunk = new unsigned char[(3 * oldWidth + useless_bytes)*oldHeight];

	readToBuffer((char*)dataChunk, sizeof(unsigned char)*((3 * oldWidth + useless_bytes)*oldHeight), &file);



	unsigned char *dataPointer = dataChunk;

	int spot = 4 * correctWidth*(correctHeight - oldHeight);



	for (unsigned int row = 0; row < oldHeight; row++)
	{
		for (unsigned int column = 0; column < oldWidth; column++)
		{

			structure->data[spot + 2] = dataPointer[0];
			structure->data[spot + 1] = dataPointer[1];
			structure->data[spot + 0] = dataPointer[2];

			dataPointer += 3;

			if (structure->data[spot] == 255 && structure->data[spot + 2] == 255 &&
				structure->data[spot + 1] == 0) structure->data[spot + 3] = 0;
			else structure->data[spot + 3] = 255;

			spot += 4;
		}

		spot += 4 * (correctWidth - oldWidth);
		dataPointer += useless_bytes;
	}



	delete[]dataChunk;

	closeBinaryFile(&file);
	
	return structure;

}

bool Wrapper::emptySprite(Sprite *sprite, unsigned int width, unsigned int height)
{
	sprite->orig_width = width;
	sprite->orig_height = height;	

	unsigned int correctWidth = 1;
	
	while(correctWidth < sprite->orig_width) correctWidth *= 2;
	sprite->width = correctWidth;

	unsigned int correctHeight = 1;

	while(correctHeight < sprite->orig_height) correctHeight *= 2;
	sprite->height = correctHeight;

	sprite->data = new unsigned char [4*sprite->width*sprite->height];

	glGenTextures(1, &(sprite->openGLtexture));

	glBindTexture(GL_TEXTURE_2D, sprite->openGLtexture);
		

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	return true;
}


// creates a canvas corresponding to the sprite's size and attaches the sprite to the canvas
void Wrapper::getCanvas(Canvas *canvas, Sprite *sprite)
{
	canvas->sprite = sprite;

	canvas->width = sprite->orig_width;
	canvas->height = sprite->orig_height;

	// canvas has its own data that is later copied to sprite's data
	canvas->data = new unsigned char [4*canvas->width*canvas->height];

}

void Wrapper::releaseCanvas(Canvas *canvas)
{
	if(canvas->data) delete []canvas->data;

	canvas->data = 0;
}


// copies canvas's data to its sprite's data and attachs it to sprite's texture
void Wrapper::attachCanvas(Canvas *canvas)
{
	// if the sprite doesn't already have data, reserve memory for it
	if(!canvas->sprite->data) canvas->sprite->data = new unsigned char [4*canvas->sprite->width*canvas->sprite->height];

	// copy the data to sprite
	for(unsigned int y = 0; y < canvas->height; y++)
	{
		memcpy(&(canvas->sprite->data[4*canvas->sprite->width*(canvas->sprite->height-1-y)]), &(canvas->data[4*canvas->width*y]), sizeof(unsigned char)*4*canvas->width);
	}


	// attach sprite's data to openGL texture
	glBindTexture(GL_TEXTURE_2D, canvas->sprite->openGLtexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvas->sprite->width, canvas->sprite->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas->sprite->data);

}

bool Wrapper::loadSprite(Sprite *sprite, char *fileName)
{
	BMPData *loaded_BMP;
	
	// only loads the data to openGL texture, not to sprite's data
	sprite->data = 0;
	
	loaded_BMP = readBMPData(fileName);

	if(!loaded_BMP) return false;

	sprite->width = loaded_BMP->width;
	sprite->height = loaded_BMP->height;
	
	sprite->orig_width = loaded_BMP->orig_width;
	sprite->orig_height = loaded_BMP->orig_height;

	glGenTextures(1, &(sprite->openGLtexture));

	glBindTexture(GL_TEXTURE_2D, sprite->openGLtexture);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sprite->width, sprite->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded_BMP->data);
	delete []loaded_BMP->data;
	delete loaded_BMP;
	
	return true;
}

void Wrapper::releaseSprite(Sprite *sprite)
{
	if(sprite->data) delete []sprite->data;

	sprite->data = 0;

	glDeleteTextures(1, &(sprite->openGLtexture));
}


bool Wrapper::drawSprite(Sprite *sprite, Rect *dest, Rect *source, Rect *clip, bool colorkey, int alpha)
{
	if(!dest) return true;
	
	int source_width = 0;
	int source_height = 0;
	int source_x = 0, source_y = 0;
	
	int dest_x, dest_y, dest_width, dest_height;

	dest_x = dest->x;
	dest_y = dest->y;
	dest_width = dest->width;
	dest_height = dest->height;

	if(source)
	{
		source_width = source->width;
		source_height = source->height;
		source_x = source->x;
		source_y = source->y;
	}
	else
	{
		source_width = sprite->orig_width;
		source_height = sprite->orig_height;
		source_x = 0;
		source_y = 0;
	}

	int clip_x, clip_y, clip_width, clip_height;

	if(clip)
	{
		clip_x = clip->x;
		clip_y = clip->y;
		clip_width = clip->width;
		clip_height = clip->height;
	}
	else
	{
		clip_x = 0;
		clip_y = 0;
		clip_width = xResolution_;
		clip_height = yResolution_;
	}

	if(dest_x >= clip_x + clip_width)
		return true;

	if(dest_x + dest_width <= clip_x)
		return true;

	if(dest_y >= clip_y + clip_height)
		return true;

	if(dest_y + dest_height <= clip_y)
		return true;


	if(dest_x < clip_x)
	{
		int difference = clip_x - dest_x;
		dest_width -= difference;
		source_x += difference;
		source_width -= difference;

		dest_x = clip_x;
	}

	if(dest_x + dest_width > clip_x + clip_width)
	{
		int difference = dest_x + dest_width - (clip_x + clip_width);

		dest_width -= difference;
		source_width -= difference;
	}

	if(dest_y < clip_y)
	{
		int difference = clip_y - dest_y;
		dest_height -= difference;
		source_y += difference;
		source_height -= difference;

		dest_y = clip_y;
	}

	if(dest_y + dest_height > clip_y + clip_height)
	{
		int difference = dest_y + dest_height - (clip_y + clip_height);

		dest_height -= difference;
		source_height -= difference;
	}


	float f_x = (float)source_x;
	float f_width = (float)source_width;

	float f_y = (float)source_y;
	float f_height = (float)source_height;
	
		
	if(colorkey || (alpha != 255))
	{
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	} else glDisable(GL_BLEND);

	if(alpha != 255)
	{
		glColor4f(1.0f,1.0f,1.0f,((float)alpha/(float)255));
	} else glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	glBindTexture(GL_TEXTURE_2D, sprite->openGLtexture);
		
	glBegin(GL_QUADS);
	
	float x_mapping, y_mapping;

	x_mapping = f_x/((float)(sprite->width));
	y_mapping = 1.0f - f_y/((float)(sprite->height));
	glTexCoord2f(x_mapping,y_mapping);
	glVertex3f((GLfloat)dest_x, (GLfloat)(yResolution_ - (dest_y)), 0.0f);
	
	x_mapping = (f_x+f_width)/((float)(sprite->width));
	y_mapping = 1.0f - f_y/((float)(sprite->height));
	glTexCoord2f(x_mapping,y_mapping);
	glVertex3f((GLfloat)(dest_x + dest_width), (GLfloat)(yResolution_ - (dest_y)), 0.0f);
	
	x_mapping = (f_x+f_width)/((float)(sprite->width));
	y_mapping = 1.0f - (f_y+f_height)/((float)(sprite->height));
	glTexCoord2f(x_mapping,y_mapping);
	glVertex3f((GLfloat)(dest_x + dest_width), (GLfloat)(yResolution_ - (dest_y + dest_height)), 0.0f);
	
	x_mapping = f_x/((float)(sprite->width));
	y_mapping = 1.0f - (f_y+f_height)/((float)(sprite->height));
	glTexCoord2f(x_mapping,y_mapping);
	glVertex3f((GLfloat)(dest_x) , (GLfloat)(yResolution_ - (dest_y + dest_height)), 0.0f);
	
	glEnd();
	
	return true;
}

void Wrapper::drawRectangle(Rect *dest, Rect *clip, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha)
{

	if(!dest) return;

	int dest_x, dest_y, dest_width, dest_height;

	dest_x = dest->x;
	dest_y = dest->y;
	dest_width = dest->width;
	dest_height = dest->height;


	int clip_x, clip_y, clip_width, clip_height;

	if(clip)
	{
		clip_x = clip->x;
		clip_y = clip->y;
		clip_width = clip->width;
		clip_height = clip->height;
	}
	else
	{
		clip_x = 0;
		clip_y = 0;
		clip_width = xResolution_;
		clip_height = yResolution_;
	}

	if(dest_x >= clip_x + clip_width) return;

	if(dest_x + dest_width <= clip_x) return;

	if(dest_y >= clip_y + clip_height) return;

	if(dest_y + dest_height <= clip_y) return;


	if(dest_x < clip_x)
	{
		int difference = clip_x - dest_x;
		dest_width -= difference;

		dest_x = clip_x;
	}

	if(dest_x + dest_width > clip_x + clip_width)
	{
		int difference = dest_x + dest_width - (clip_x + clip_width);

		dest_width -= difference;
	}

	if(dest_y < clip_y)
	{
		int difference = clip_y - dest_y;
		dest_height -= difference;

		dest_y = clip_y;
	}

	if(dest_y + dest_height > clip_y + clip_height)
	{
		int difference = dest_y + dest_height - (clip_y + clip_height);

		dest_height -= difference;
	}
	

	if(alpha != 255)
	{
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	} else glDisable(GL_BLEND);

	if(alpha != 255)
	{
		glColor4f(1.0f,1.0f,1.0f,((float)alpha/(float)255));
	} else glColor4f(1.0f,1.0f,1.0f,1.0f);

	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(((float)r/(float)255),((float)g/(float)255),((float)b/(float)255),((float)alpha/(float)255));
	
	glBegin(GL_QUADS);

	glVertex3f((GLfloat)dest_x, (GLfloat)(yResolution_ - (dest_y)), 0.0f);
	glVertex3f((GLfloat)(dest_x + dest_width), (GLfloat)(yResolution_ - (dest_y)), 0.0f);
	glVertex3f((GLfloat)(dest_x + dest_width), (GLfloat)(yResolution_ - (dest_y + dest_height)), 0.0f);
	glVertex3f((GLfloat)(dest_x) , (GLfloat)(yResolution_ - (dest_y + dest_height)), 0.0f);

	glEnd();
	
}

void Wrapper::clearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT);
}


void Wrapper::flip()
{
	SDL_GL_SwapBuffers();
}

bool Wrapper::loadWave(Wave *wave, char *fileName)
{
	BinaryFile file;

	openBinaryFile(fileName, &file, LITTLE_ENDIAN);

	if (!file.file.is_open()) return false;

	unsigned int RIFF = readUDword(&file);

	if (RIFF != 0x46464952) // "RIFF"
	{
		closeBinaryFile(&file); return false;
	}

	unsigned int chunkSize = readUDword(&file);

	unsigned int WAVE = readUDword(&file);

	if (WAVE != 0x45564157) // "WAVE"
	{
		closeBinaryFile(&file); return false;
	}


	unsigned int subchunkID;
	unsigned int subchunkSize;
	bool done = false;

	// "fmt "

	short int format;
	short int channels;
	unsigned int sampleRate;
	unsigned short bitsPerSample;

	

	while (!done)
	{
		unsigned int bytesRead = 0;

		subchunkID = readUDword(&file);
		subchunkSize = readUDword(&file);

		// read subchunk
		switch (subchunkID)
		{
		case 0x20746D66: // "fmt "

			format = readUWord(&file);

			if (format != 1) // must be PCM (1)
			{
				closeBinaryFile(&file); return false;
			}

			channels = readUWord(&file);
			wave->channels = channels;

			sampleRate = readUDword(&file);
			wave->sampleRate = sampleRate;

			readUDword(&file);
			readUWord(&file);
			
			bitsPerSample = readUWord(&file);

			if (bitsPerSample != 16)
			{
				closeBinaryFile(&file); return false;
			}


			bytesRead = 16;
			break;


		case 0x61746164:	// "data"


			wave->samplesPerChannel = ((subchunkSize) / (bitsPerSample/8)) / wave->channels;
			wave->buffer = new signed short[(subchunkSize) / (bitsPerSample / 8)];
			readToBuffer((char*)wave->buffer, subchunkSize, &file);
			bytesRead = subchunkSize;
			done = true;

			break;

		default:
			bytesRead = 0;
			break;
		}


		if (!done) for (unsigned int index = 0; index < subchunkSize - bytesRead; index++) readByte(&file);

	}




		
	
	closeBinaryFile(&file);

	return true;
}

void Wrapper::releaseWave(Wave *wave)
{
	if (wave->buffer)
	{
		delete[] wave->buffer;
		wave->buffer = 0;
	}

	wave->samplesPerChannel = 0;
}

bool Wrapper::readInput()
{
	Uint8 mouse_buttons;

	//SDL_Event sdl_event;
	//while (SDL_PollEvent(&sdl_event)){}

	SDL_PumpEvents();

	Uint8 *key = SDL_GetKeyState(NULL);

	for(int index = 0; index < KEY_LAST; index++)
	{
		inputData_.KEY_DOWN [index] = key[index];
	}

	
	mouse_buttons = SDL_GetRelativeMouseState(&(inputData_.mouse_move_x), &(inputData_.mouse_move_y));
			
	for(int index = 0; index < 3; index++)
	{
		if(mouse_buttons & SDL_BUTTON(index+1))
		{
			inputData_.MOUSE_BUTTON_DOWN[index] = 1;
		}
		else
		{
			inputData_.MOUSE_BUTTON_DOWN[index] = 0;
		}
	}

	return true;
}

InputData* Wrapper::getInputData()
{
	return &inputData_;
}

// File IO stuff

void Wrapper::openBinaryFile(char *fileName, BinaryFile *file, unsigned int endianness)
{
	file->endianness = endianness;
	file->file = std::ifstream(fileName, std::ios_base::binary);
}

void Wrapper::closeBinaryFile(BinaryFile *file)
{
	file->file.close();
}

void Wrapper::readToBuffer(char *buffer, unsigned int byteAmount, BinaryFile *file)
{
	file->file.read((char*)buffer, byteAmount);
}

unsigned char Wrapper::readUByte(BinaryFile *file)
{
	unsigned char byte;

	file->file.read((char*)&byte, sizeof(unsigned char));

	return byte;
}

char Wrapper::readByte(BinaryFile *file)
{
	char byte;

	file->file.read(&byte, sizeof(char));

	return byte;
}


unsigned short Wrapper::readUWord(BinaryFile *file)
{
	unsigned short number;
	file->file.read((char*)&number, sizeof(unsigned short));
	
	if (endianness_ != file->endianness)
	{
		unsigned short final = 0;
		unsigned short mask = 0xFF;

		final = number & mask;

		number >>= 8;
		final <<= 8;

		final = final | (number & mask);

		return final;
	}

	return number;
}

short Wrapper::readWord(BinaryFile *file)
{
	short number;
	file->file.read((char*)&number, sizeof(short));

	if (endianness_ != file->endianness)
	{
		unsigned short temp_number = *(unsigned short*)&number;


		unsigned short final = 0;
		unsigned short mask = 0xFF;

		final = temp_number & mask;

		temp_number >>= 8;
		final <<= 8;

		final = final | (temp_number & mask);


		short signed_final = *(short*)&final;
		return signed_final;
	}

	return number;
}

unsigned int Wrapper::readUDword(BinaryFile *file)
{
	unsigned int number;
	file->file.read((char*)&number, sizeof(unsigned int));

	if (endianness_ != file->endianness)
	{
		unsigned int final = 0;
		unsigned int mask = 0xFF;

		final = number & mask;

		number >>= 8;
		final <<= 8;

		final = final | (number & mask);

		number >>= 8;
		final <<= 8;

		final = final | (number & mask);

		number >>= 8;
		final <<= 8;

		final = final | (number & mask);

		return final;
	}

	return number;
}


int Wrapper::readDword(BinaryFile *file)
{
	int number;
	file->file.read((char*)&number, sizeof(int));

	if (endianness_ != file->endianness)
	{
		unsigned int temp_number = *(unsigned int*)&number;

		unsigned int final = 0;
		unsigned int mask = 0xFF;

		final = temp_number & mask;

		temp_number >>= 8;
		final <<= 8;

		final = final | (temp_number & mask);

		temp_number >>= 8;
		final <<= 8;

		final = final | (temp_number & mask);

		temp_number >>= 8;
		final <<= 8;

		final = final | (temp_number & mask);

		int signed_final = *(int*)&final;

		return signed_final;
	}

	return number;
}

// Lock stuff
void Wrapper::initLock(Lock *lock)
{
	InitializeCriticalSection(&lock->cs);
}
void Wrapper::deleteLock(Lock *lock)
{
	DeleteCriticalSection(&lock->cs);
}

void Wrapper::lock(Lock *lock)
{
	EnterCriticalSection(&lock->cs);
}

void Wrapper::unlock(Lock *lock)
{
	LeaveCriticalSection(&lock->cs);
}


// Timer stuff

void Wrapper::startTimer(Timer *timer)
{
	lock(&timerLock_);
	
#ifdef _WIN32

	QueryPerformanceCounter(&timer->start_time);
	QueryPerformanceFrequency(&timer->counts);

#else
#error NOT_WINDOWS
#endif

	unlock(&timerLock_);
}

double Wrapper::getTimerSeconds(Timer *timer)
{
	lock(&timerLock_);

#ifdef _WIN32

	long long int counts;
	counts = timer->counts.QuadPart;

	LARGE_INTEGER ticks;
	QueryPerformanceCounter(&ticks);

	long long int difference;
	difference = ticks.QuadPart - timer->start_time.QuadPart;

	double seconds = (double)(difference) / (double)counts;




#else
#error NOT_WINDOWS
#endif

	unlock(&timerLock_);

	return seconds;
}