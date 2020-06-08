#ifndef WINDOW_H
#define WINDOW_H


#include <vector>
#include <string>

#include "windows.h"
#include "wrapper.h"

class Windows;



struct CallBackData
{
	int callBackType;
	void *callBackValue;
};


struct MouseClickData
{
	int buttonNumber;
	int x, y;
};

struct LiftMouseData
{
	int buttonNumber;
	int x, y;
};

struct MouseMoveData
{
	int x, y;
};

struct TextMenuChosenData
{
	int row;
};

struct TextFieldData
{
	std::string text;
};

struct drawInterface
{

	unsigned int width, height;
	unsigned char *data;
};


typedef enum {
	CALLBACK_CLICK,
	CALLBACK_MOUSE_MOVE,
	CALLBACK_LIFT_BUTTON,
	CALLBACK_TEXT_CHOSEN,
	CALLBACK_TEXTFIELD_MODIFIED
} CALLBACK_TYPE;

class Button;
class TextMenu;
class Text;
class TextField;
class GraphicsWindow;

class Window
{
protected:

	Window *parent_;
	Windows *windowSystem_;
	std::vector <Window *> windows_;
	int x_, y_, width_, height_;

	int scrollX_, scrollY_;

	bool scrollControlsActive_;
	void *returnData_;
	CallBackData callBackData_;

	MouseClickData clickData_;
	MouseMoveData mouseMoveData_;

	bool clipArea(Rect *rect);


	(void (*callBackFunction_))(void *data, Window *source, CallBackData *callBackData);
	(void (*updateFunction_))(void *data);

	bool active_;

	bool hidden_;

	void createScrollControls();
	std::vector <Window *> scrollControlWindows_;
public:

	~Window();
	Window(): x_(0), y_(0), width_(0), height_(0), returnData_(0), callBackFunction_(0), updateFunction_(0), parent_(0), windowSystem_(0), scrollX_(0), scrollY_(0), scrollControlsActive_(false), active_(false), hidden_(false) {}
	Window(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data)): x_(x), y_(y), width_(width), height_(height), returnData_(returnData),
		callBackFunction_(callBackFunction), updateFunction_(updateFunction), parent_(0), windowSystem_(0), scrollX_(0), scrollY_(0), scrollControlsActive_(false), active_(false), hidden_(false)  {createScrollControls();}

	Window *createWindow(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data));

	GraphicsWindow *createGraphicsWindow(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data),
		void (*drawFunction)(void *data, GraphicsWindow *source));
	Button *createButton(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData));
	TextMenu *createTextMenu(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data));
	Text *createText(std::string text, int x, int y, int width, int height, void *returnData, void (*updateFunction)(void *data));
	TextField *createTextField(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data));

	virtual void draw();

	bool handleScrollControlClick(int x, int y, int buttonNumber);

	void hide();
	void show();

	bool isHidden();
	void addScrollControlWindow(Window *window);

	Window *getParent();
	void setParent(Window *parent);

	Windows *getWindowSystem();
	void setWindowSystem(Windows *windowSystem);
	void setScrollControlsActive(bool active);

	virtual void click(int x, int y, int buttonNumber);

	void xScroll(int x);
	void yScroll(int y);

	void update();

	virtual void logic();
	virtual void activate();
	virtual void deactivate();
	int getChildX(Window *child);
	int getChildY(Window *child);
	int getGlobalX();
	int getGlobalY();
	int getX();
	int getY();
	int getScrollX();
	int getScrollY();
	int getWidth();
	int getHeight();
};


struct SpriteInstance
{
	Rect dest;
	Rect source;
	bool colorkey;
	int alpha;

	unsigned int handle;
};

class GraphicsWindow : public Window
{
protected:

	void (*drawFunction_)(void *data, GraphicsWindow *source);
public:

	GraphicsWindow(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data),
		void (*drawFunction)(void *data, GraphicsWindow *source));

	~GraphicsWindow();
	
	std::vector <SpriteInstance> spriteInstances_;
	void drawWindowSprite(WindowSprite *sprite, Rect *dest, Rect *source, bool colorKey = true, int alpha = 255);
	
	virtual void draw();
};

class Button : public Window
{
protected:
	std::string buttonName_;
	Sprite *buttonSprite_;
	Rect buttonSpriteRect_;
public:
	Button(): buttonName_(""), buttonSprite_(0) {}
	Button(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData));

	virtual void draw();
	void set_name(std::string name);
	void set_Sprite(Sprite *sprite);
	void set_SpriteRect(Rect *rect);
};

#define SCROLL_CONTROL_BUTTON_LEFT 0
#define SCROLL_CONTROL_BUTTON_UP 1
#define SCROLL_CONTROL_BUTTON_RIGHT 2
#define SCROLL_CONTROL_BUTTON_DOWN 3

#define SCROLL_CONTROL_BUTTON_SPRITE "scroll_control_buttons.bmp"

#define SCROLL_CONTROL_BUTTON_LEFT_X 0
#define SCROLL_CONTROL_BUTTON_LEFT_Y 0
#define SCROLL_CONTROL_BUTTON_LEFT_WIDTH 50
#define SCROLL_CONTROL_BUTTON_LEFT_HEIGHT 50

class ScrollControlButton : public Button
{
protected:
	int scrollControlButtonType_;
public:
	ScrollControlButton(int x, int y, int width, int height);
	void set_scrollControlButtonType(int type);
	virtual void click(int x, int y, int buttonNumber);

};

class TextMenu : public Window
{
protected:
	std::vector <std::string> texts_;

	TextMenuChosenData textMenuChosenData_;
public:
	TextMenu(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data));
	void add_text(std::string text);
	virtual void click(int x, int y, int buttonNumber);
	virtual void draw();
	void clear();
	int get_row();
	void set_row(int row);
};

class Text : public Window
{
protected:
	std::string text_;

public:
	Text(std::string text, int x, int y, int width, int height, void *returnData, void (*updateFunction)(void *data));
	void set_text(std::string text);
	std::string get_text();
	virtual void draw();
};

class TextField : public Window
{
protected:
	std::string oldText_;
	std::string text_;
	unsigned int position_;

	TextFieldData textFieldData_;
public:
	TextField(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data));
	void set_text(std::string text);
	std::string get_text();
	virtual void deactivate();
	virtual void logic();
	virtual void draw();
	virtual void click(int x, int y, int buttonNumber);


};
#endif