#ifndef WINDOW_H
#define WINDOW_H


#include <vector>
#include <string>

#include "windows.h"
#include "wrapper.h"

class Windows;



struct drawInterface
{

	unsigned int width, height;
	unsigned char *data;
};


typedef enum {
	ATTACH_LEFT,
	ATTACH_RIGHT,
	ATTACH_TOP,
	ATTACH_BOTTOM,
	ATTACH_NONE
} WINDOW_ATTACH_TYPE;

class Widget;
class Event
{
protected:
	std::vector <void(*)(void *, Widget *, Event *)> pointers_;
	std::vector <void *> objects_;

public:
	void add(void *object, void(*functionPointer)(void *, Widget *, Event *));
	void clear();
};


class MouseEvent : public Event
{
protected:

	int localX_, localY_;
	int widgetWidth_, widgetHeight_;
public:

	int getLocalX() { return localX_; }
	int getLocalY() { return localY_; }
	int getWidgetWidth() { return widgetWidth_; }
	int getWidgetHeight() { return widgetHeight_; }

	void call(int localX, int localY, int widgetWidth, int widgetHeight, Widget *widget);
};

class ChooseEvent : public Event
{
protected:
	int number_;
public:
	int getNumber() { return number_; }

	void call(int number, Widget *widget);

};


class TextEvent : public Event
{
	std::string text_;

public:

	std::string getText() { return text_; }

	void call(std::string text, Widget *widget);
};


struct Region
{
	bool visited;
	std::string regionName;

	std::string x_attach_name, y_attach_name, width_attach_name, height_attach_name;
	WINDOW_ATTACH_TYPE x_attach, y_attach, width_attach, height_attach;

	int x, y, width, height;
	
	Widget *widget;
};

class Widget
{
protected:

	Window *parent_;
	Windows *windowSystem_;

	bool mouseButtonHeldDown_[3];

	bool active_;
	bool disabled_;

	//Return_Type(Class_Name::* pointer_name) (Argument_List);

	
	bool mouseOnWidget(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY);
	void drawBorders(int x, int y, int width, int height, Rect oldClip, unsigned char r, unsigned char g, unsigned char b);
	void drawBox(int x, int y, int width, int height, Rect oldClip, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha);
public:
	Widget() : parent_(0), windowSystem_(0), active_(false), disabled_(false)
	{
		for (int button = 0; button < 3; button++)
			mouseButtonHeldDown_[button] = false;
	}

	virtual void draw(int x, int y, int width, int height, Rect oldClip);
	virtual void logic(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input);
	virtual void mouseEvent(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input);
	virtual void click(int localX, int localY, int x, int y, int width, int height);
	

	void setDisabled(bool disabled) { disabled_ = disabled; }
	bool disabled() { return disabled_; }

	virtual void activate() { active_ = true; }
	virtual void deactivate() { active_ = false; }
	bool active() { return active_; }

	bool *getMouseButtonHeldDown() { return mouseButtonHeldDown_; }
	Windows *getWindowSystem() { return windowSystem_; }
	virtual void setWindowSystem(Windows *windowSystem) { windowSystem_ = windowSystem; }

	MouseEvent onClick_;
	MouseEvent generalMouseEvent_;
};

class Label : public Widget
{
public:
	std::string text_;

	virtual void draw(int x, int y, int width, int height, Rect oldClip);
	
};
class Button : public Widget
{
protected:
	WindowSprite *sprite_;
	Rect spriteRect_;

	

public:
	Button() : name_(""), sprite_(0), r_(0), g_(192), b_(0) {}

	std::string name_;
	unsigned char r_, g_, b_;

	virtual void draw(int x, int y, int width, int height, Rect oldClip);
	void setName(std::string name) { name_ = name; }
	void setSprite(WindowSprite *sprite) { sprite_ = sprite; }

	void setSpriteRect(Rect *rect);

};

class Window : public Widget
{
protected:
		
	std::vector <Region> regions_;
	int x_, y_, width_, height_;
	int scrollControlSize_;
	int scrollBarMinSize_;
	int scrollAmount_;

	int scrollX_, scrollY_;

	bool hScrollControlsActive_;
	bool vScrollControlsActive_;
			
	Region *getRegion(std::string regionName);
	void clearVisitedRegions();
	int findRegionCoordinate(std::string regionName, WINDOW_ATTACH_TYPE attachType, int x, int y, int width, int height, int sum);
public:

	~Window();
	Window() : x_(0), y_(0), width_(0), height_(0), scrollX_(0), scrollY_(0), hScrollControlsActive_(false), vScrollControlsActive_(false), scrollControlSize_(15), scrollBarMinSize_(15), scrollAmount_(30) {}
	
	void setX(int x) {x_ = x;}
	void setY(int y) {y_ = y;}
	void setWidth(int width) { width_ = width; }
	void setHeight(int height) { height_ = height; }

	virtual int getScrollWidth(int x, int y, int width, int height);
	virtual int getScrollHeight(int x, int y, int width, int height);
	
	int getX() { return x_; }
	int getY() { return y_; }
	int getWidth() { return width_; }
	int getHeight() { return height_; }

	virtual bool getNewClip(int x, int y, int width, int height, Rect oldClip, Rect *newClip);

	void setWidget(std::string regionName, Widget *widget);
	void addRegion(std::string regionName, int x, WINDOW_ATTACH_TYPE x_attach, std::string x_attach_name,
		int y, WINDOW_ATTACH_TYPE y_attach, std::string y_attach_name,
		int width, WINDOW_ATTACH_TYPE width_attach, std::string width_attach_name,
		int height, WINDOW_ATTACH_TYPE height_attach, std::string height_attach_name);

	void clearRegions() { regions_.clear(); }

	virtual void setWindowSystem(Windows *windowSystem);
	virtual void mouseEvent(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input);
	virtual void click(int localX, int localY, int x, int y, int width, int height);

	Rect getRegionRect(std::string regionName, int x, int y, int width, int height);

	void drawScrollControls(int x, int y, int width, int height, Rect oldClip);

	virtual void draw(int x, int y, int width, int height, Rect oldClip);

	virtual void logic(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input);

	void handleScrollControls(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input);

	Window *getParent();
	void setParent(Window *parent);

	
	void setVScrollControlsActive(bool active) { vScrollControlsActive_ = active; }
	void setHScrollControlsActive(bool active){ hScrollControlsActive_ = active; }

	void xScroll(int x);
	void yScroll(int y);

	void update();

	virtual void activate();
	virtual void deactivate();
	int getChildX(Window *child);
	int getChildY(Window *child);

	void setScrollX(int scrollX) { scrollX_ = scrollX; }
	void setScrollY(int scrollY) { scrollY_ = scrollY; }

	int getScrollX() { return scrollX_; }
	int getScrollY() { return scrollY_; }
	
};

class TextField : public Widget
{
protected:
	std::string oldText_;
	int position_;
	std::string text_;

public:

	bool readOnly_;

	TextField() : text_(""), oldText_(""), readOnly_(false), position_(0) {}

	virtual void logic(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input);
	virtual void draw(int x, int y, int width, int height, Rect oldClip);
//	virtual void mouseEvent(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input);
	virtual void click(int localX, int localY, int x, int y, int width, int height);

	void deactivate();

	void setText(std::string text)
	{
		text_ = text;
		position_ = 0;
	}

	std::string getText() { return text_; }

	TextEvent textEnter_;
	TextEvent textChange_;

};

class List : public Window
{
protected:
	std::vector <std::string> texts_;
	int row_;
public:
	List() : row_(-1) {}
	virtual void draw(int x, int y, int width, int height, Rect oldClip);
	virtual void click(int localX, int localY, int x, int y, int width, int height);

	virtual int getScrollWidth(int x, int y, int width, int height);
	virtual int getScrollHeight(int x, int y, int width, int height);


	void add(std::string text);
	void clear();
	void setRow(int row);
	void updateSrollY();
	int getRow() { return row_; }

	ChooseEvent chooseEvent_;

};

class RadioButtons : public Widget
{
protected:
	std::vector <std::string> texts_;
	int buttonNumber_;
	bool horizontal_;
public:
	RadioButtons() : buttonNumber_(-1), horizontal_(true) {}

	virtual void draw(int x, int y, int width, int height, Rect oldClip);
	virtual void click(int localX, int localY, int x, int y, int width, int height);


	void add(std::string text);
	void clear();
	void setButtonNumber(int buttonNumber) { buttonNumber_ = buttonNumber; }
	int getButtonNumber() { return buttonNumber_; }

	ChooseEvent chooseEvent_;
};


class DropMenu;

class DropList : public List
{
protected:
	DropMenu *owner_;
public:
	void setOwner(DropMenu *owner) { owner_ = owner; }
	virtual void click(int localX, int localY, int x, int y, int width, int height);
};

class DropMenu : public Widget
{
protected:
	DropList list_;
	std::vector <std::string> texts_;
	int row_;
public:
	DropMenu() : row_(-1) { list_.setOwner(this); }

	virtual void draw(int x, int y, int width, int height, Rect oldClip);
	virtual void click(int localX, int localY, int x, int y, int width, int height);

	void add(std::string text);
	void clear();
	void setRow(int row);
	int getRow() { return row_; }

	ChooseEvent chooseEvent_;

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
	
	std::vector <void(*)(void *, GraphicsWindow *, int, int)> drawFunctionPointers_;
	std::vector <void *> objects_;
	
	std::vector <SpriteInstance> spriteInstances_;

	int scrollWidth_, scrollHeight_;
public:
	GraphicsWindow() : scrollWidth_(0), scrollHeight_(0) {}
	void drawWindowSprite(WindowSprite sprite, Rect *dest, Rect *source, bool colorKey = true, int alpha = 255);
	
	virtual void draw(int x, int y, int width, int height, Rect oldClip);

	virtual int getScrollWidth(int x, int y, int width, int height);
	virtual int getScrollHeight(int x, int y, int width, int height);

	virtual bool getNewClip(int x, int y, int width, int height, Rect oldClip, Rect *newClip);

	void setScrollWidth(int width) { scrollWidth_ = width; }
	void setScrollHeight(int height) { scrollHeight_ = height; }

	void addDrawFunction(void *object, void(*functionPointer)(void *, GraphicsWindow *, int, int));
	void clearDrawFunctions();
};

#endif