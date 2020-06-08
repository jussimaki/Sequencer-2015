#include "windows.h"
#include "window.h"


Windows::~Windows()
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		delete windows_[index];
		windows_[index] = 0;
	}

	windows_.clear();

	for(unsigned int index = 0; index < spriteHandlePairVector_.size(); index++)
	{
		if(spriteHandlePairVector_[index].sprite)
		{
			wrapper_->releaseSprite(spriteHandlePairVector_[index].sprite);

			delete spriteHandlePairVector_[index].sprite;
			spriteHandlePairVector_[index].sprite = 0;
		}

		if(spriteHandlePairVector_[index].windowSprite)
		{
			delete spriteHandlePairVector_[index].windowSprite;
			spriteHandlePairVector_[index].windowSprite = 0;
		}
	}

	spriteHandlePairVector_.clear();
}



Input *Windows::getInput()
{
	return &input_;
}

void Windows::setWrapper(Wrapper *wrapper)
{
	wrapper_ = wrapper;
}

Wrapper *Windows::getWrapper()
{
	return wrapper_;
}


Printer *Windows::getPrinter()
{
	return printer_;
}

Window* Windows::createWindow(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data))
{
	Window *newWindow = new Window(x, y, width, height, returnData, callBackFunction, updateFunction);

	newWindow->setWindowSystem(this);
	windows_.push_back(newWindow);

	return newWindow;
	
}


void Windows::addWindow(Window *window)
{
	window->setWindowSystem(this);
	windows_.push_back(window);
}

void Windows::closeWindow(Window *window)
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(windows_[index] == window)
		{
			delete windows_[index];
			windows_.erase(windows_.begin() + index);

			return;
		}
	}
}

void Windows::draw()
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(!windows_[index]->isHidden()) windows_[index]->draw();
	}

	Rect dest;
	dest.x = mouseX_;
	dest.y = mouseY_;
	dest.width = 15;
	dest.height = 15;

	wrapper_->drawRectangle(&dest, 0, 255, 0, 255, 128);

	
}

void Windows::logic()
{
	inputFunctionality(wrapper_, &input_);

	
	mouseX_ += input_.mouse_move_x;
	mouseY_ += input_.mouse_move_y;
	
	for(int button = 0; button < 3; button++)
		if(input_.MOUSE_BUTTON_CLICKED[button]) click(mouseX_, mouseY_, button);

	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(!windows_[index]->isHidden()) windows_[index]->logic();
	}

	
}

void Windows::update()
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		windows_[index]->update();
	}
}

void Windows::click(int x, int y, int buttonNumber)
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(!windows_[index]->isHidden()) 
		{
			if((x >= windows_[index]->getGlobalX())
				&& (x < windows_[index]->getGlobalX() + windows_[index]->getWidth())
				&& (y >= windows_[index]->getGlobalY())
				&& (y < windows_[index]->getGlobalY() + windows_[index]->getHeight())
				)
			{
				windows_[index]->click(x -windows_[index]->getGlobalX(), y -windows_[index]->getGlobalY(), buttonNumber);
				windows_[index]->activate();

			}
		}
	}
}

WindowSprite *Windows::loadWindowSprite(char *fileName)
{
	SpriteHandlePair pair;

	pair.sprite = new Sprite;
	wrapper_->loadSprite(pair.sprite, fileName);

	pair.windowSprite = new WindowSprite;

	pair.windowSprite->handle = spriteCounter_;

	spriteCounter_++;

	pair.windowSprite->width = pair.sprite->orig_width;
	pair.windowSprite->height = pair.sprite->orig_height;

	spriteHandlePairVector_.push_back(pair);

	return pair.windowSprite;
}

WindowSprite *Windows::getEmptyWindowSprite(unsigned int width, unsigned int height)
{
	SpriteHandlePair pair;

	pair.sprite = new Sprite;
	wrapper_->emptySprite(pair.sprite, width, height);

	pair.windowSprite = new WindowSprite;

	pair.windowSprite->handle = spriteCounter_;

	spriteCounter_++;

	pair.windowSprite->width = pair.sprite->orig_width;
	pair.windowSprite->height = pair.sprite->orig_height;

	spriteHandlePairVector_.push_back(pair);

	return pair.windowSprite;
}

WindowCanvas *Windows::getWindowCanvas(WindowSprite *windowSprite)
{
	
	WindowCanvas *newCanvas = new WindowCanvas;
	Sprite *sprite = 0;
	for(unsigned int index = 0; index < spriteHandlePairVector_.size(); index++)
	{
		if(spriteHandlePairVector_[index].windowSprite->handle == windowSprite->handle)
		{
			sprite = spriteHandlePairVector_[index].sprite;
						
			break;
		}

	}

	Canvas *canvas = wrapper_->getCanvas(sprite);
			
	newCanvas->handle = windowSprite->handle;
	newCanvas->width = sprite->orig_width;
	newCanvas->height = sprite->orig_height;
	newCanvas->data = canvas->data;
			
	CanvasHandlePair pair;

	pair.handle = newCanvas->handle;
	pair.canvas = canvas;
	
	canvasHandlePairVector_.push_back(pair);

	return newCanvas;
}


void Windows::attachWindowCanvas(WindowCanvas *windowCanvas)
{
	for(unsigned int index = 0; index < canvasHandlePairVector_.size(); index++)
	{
		if(canvasHandlePairVector_[index].handle == windowCanvas->handle)
		{
			wrapper_->attachCanvas(canvasHandlePairVector_[index].canvas);

			break;
		}
	}
}


Sprite *Windows::getSprite(unsigned int handle)
{
	for(unsigned int index = 0; index < spriteHandlePairVector_.size(); index++)
	{
		if(spriteHandlePairVector_[index].windowSprite->handle == handle)
			return spriteHandlePairVector_[index].sprite;
	}

	return 0;
}