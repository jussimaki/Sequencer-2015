#ifndef WINDOWS_H
#define WINDOWS_H

#include <vector>

#include "wrapper.h"
#include "input_functionality.h"



struct Sprite;
class Window;
struct CallBackData;

class Printer;


struct WindowSprite
{
	unsigned int handle;
	unsigned int width, height;
};

struct WindowCanvas
{
	
	unsigned int handle;
	unsigned int width, height;
	unsigned char *data;
};

struct SpriteHandlePair
{
	Sprite *sprite;
	WindowSprite *windowSprite;
};

struct CanvasHandlePair
{
	Canvas *canvas;
	unsigned int handle;
};

class Windows
{
private:
	Wrapper *wrapper_;
	Printer *printer_;

	std::vector <Window *> windows_;

	Input input_;
	int mouseX_, mouseY_;

	unsigned int spriteCounter_;


	std::vector <SpriteHandlePair> spriteHandlePairVector_;
	std::vector <CanvasHandlePair> canvasHandlePairVector_;

public:
	~Windows();
	Windows(Wrapper *wrapper, Printer *printer): wrapper_(wrapper), printer_(printer), mouseX_(wrapper->getXResolution()/2), mouseY_(wrapper->getYResolution()/2),
		spriteCounter_(0) {clearInput(&input_);}

	Sprite *getSprite(unsigned int handle);



	Printer *getPrinter();

	void setWrapper(Wrapper *wrapper);
	Wrapper *getWrapper();

	Input *getInput();

	Window *createWindow(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data));
	void addWindow(Window *window);
	void closeWindow(Window *window);
	void draw();
	void logic();
	void update();
	void click(int x, int y, int buttonNumber);


	WindowSprite *loadWindowSprite(char *fileName);
	WindowSprite *getEmptyWindowSprite(unsigned int width, unsigned int height);
	WindowCanvas *getWindowCanvas(WindowSprite *windowSprite);
	void attachWindowCanvas(WindowCanvas *windowCanvas);


};

#endif