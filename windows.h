#ifndef WINDOWS_H
#define WINDOWS_H

#include <vector>

#include "printer.h"
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

struct SpriteWindowSpritePair
{
	Sprite sprite;
	WindowSprite windowSprite;
};

struct CanvasWindowCanvasPair
{
	Canvas canvas;
	WindowCanvas windowCanvas;
};

bool mouseOnRectangle(int x, int y, int width, int height, int mouseX, int mouseY);

class Windows
{
private:
	Wrapper *wrapper_;
	Printer *printer_;

	std::vector <Window *> windows_;
	std::vector <Window *> popups_;

	Input input_;
	int mouseX_, mouseY_;

	unsigned int spriteCounter_;
	unsigned int canvasCounter_;

	bool popupOpened_;

	std::vector <SpriteWindowSpritePair> spriteWindowSpritePairVector_;
	std::vector <CanvasWindowCanvasPair> canvasWindowCanvasPairVector_;

public:
	~Windows();
	Windows(Wrapper *wrapper, Printer *printer): wrapper_(wrapper), printer_(printer), mouseX_(wrapper->getXResolution()/2), mouseY_(wrapper->getYResolution()/2),
		spriteCounter_(0), canvasCounter_(0), popupOpened_(false) {clearInput(&input_);}

	Sprite *getSprite(unsigned int handle);



	Printer *getPrinter();

	void setWrapper(Wrapper *wrapper);
	Wrapper *getWrapper();

	Input *getInput();

	void addWindow(Window *window);
	void closeWindow(Window *window);

	bool mouseOnPopup();
	bool popupOpened(Window *popup);

	void addPopup(Window *window);
	void clearPopups() { popups_.clear(); }
	void closePopup(Window *window);
	void draw();
	void logic();
	void update();
	

	WindowSprite loadWindowSprite(char *fileName);
	WindowSprite getEmptyWindowSprite(unsigned int width, unsigned int height);
	WindowCanvas getWindowCanvas(WindowSprite windowSprite);
	void attachWindowCanvas(WindowCanvas windowCanvas);


};

#endif