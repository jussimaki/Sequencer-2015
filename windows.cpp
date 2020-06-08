#include "windows.h"
#include "window.h"

#include <sstream>

Windows::~Windows()
{
	windows_.clear();
	popups_.clear();

	for(unsigned int index = 0; index < spriteWindowSpritePairVector_.size(); index++)
	{
		wrapper_->releaseSprite(&spriteWindowSpritePairVector_[index].sprite);
	}

	spriteWindowSpritePairVector_.clear();

	for (unsigned int index = 0; index < canvasWindowCanvasPairVector_.size(); index++)
	{
		wrapper_->releaseCanvas(&canvasWindowCanvasPairVector_[index].canvas);
	}

	canvasWindowCanvasPairVector_.clear();
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
			windows_.erase(windows_.begin() + index);

			return;
		}
	}
}

void Windows::addPopup(Window *window)
{
	window->setWindowSystem(this);
	popups_.push_back(window);

	popupOpened_ = true;
}

void Windows::closePopup(Window *window)
{
	for (unsigned int index = 0; index < popups_.size(); index++)
	{
		if (popups_[index] == window)
		{
			popups_.erase(popups_.begin() + index);

			return;
		}
	}
}

bool Windows::popupOpened(Window *popup)
{
	for (int index = 0; index < popups_.size(); index++)
	{
		if (popups_[index] == popup) return true;
	}

	return false;
}

void Windows::draw()
{
	Rect rect;

	rect.x = 0;
	rect.y = 0;
	rect.width = wrapper_->getXResolution();
	rect.height = wrapper_->getYResolution();

	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		windows_[index]->draw(windows_[index]->getX(), windows_[index]->getY(),
			windows_[index]->getWidth(), windows_[index]->getHeight(), rect);
	}

	for (unsigned int index = 0; index < popups_.size(); index++)
	{
		popups_[index]->draw(popups_[index]->getX(), popups_[index]->getY(),
			popups_[index]->getWidth(), popups_[index]->getHeight(), rect);
	}

	Rect dest;
	dest.x = mouseX_;
	dest.y = mouseY_;
	dest.width = 15;
	dest.height = 15;

	wrapper_->drawRectangle(&dest, 0, 255, 0, 255, 128);

	

	
}


bool Windows::mouseOnPopup()
{

	bool mouseOnPopup = false;

	for (int index = 0; index < popups_.size(); index++)
	{
		int x = popups_[index]->getX();
		int y = popups_[index]->getY();

		int width = popups_[index]->getWidth();
		int height = popups_[index]->getHeight();

		if (mouseOnRectangle(x, y, width, height, mouseX_, mouseY_)) mouseOnPopup = true;

	}

	return mouseOnPopup;
}

void Windows::logic()
{
	popupOpened_ = false;

	inputFunctionality(wrapper_, &input_);

	mouseX_ += input_.mouse_move_x;
	mouseY_ += input_.mouse_move_y;
	
	Rect rect;

	rect.x = 0;
	rect.y = 0;
	rect.width = wrapper_->getXResolution();
	rect.height = wrapper_->getYResolution();
	

	int index = (int)windows_.size() - 1;
	if (windows_.size() > 0 && !mouseOnPopup())
		windows_[windows_.size() - 1]->logic(windows_[index]->getX(), windows_[index]->getY(), windows_[index]->getWidth(),
		windows_[index]->getHeight(), rect, mouseX_, mouseY_, &input_);

	for (int index = 0; index < popups_.size(); index++)
	{
		popups_[index]->logic(popups_[index]->getX(), popups_[index]->getY(), popups_[index]->getWidth(),
			popups_[index]->getHeight(), rect, mouseX_, mouseY_, &input_);
	}


	

	if (input_.MOUSE_BUTTON_CLICKED[0] && !popupOpened_)
	{
		if (!mouseOnPopup()) clearPopups();
	}
	
}

void Windows::update()
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		windows_[index]->update();
	}
}


WindowSprite Windows::loadWindowSprite(char *fileName)
{
	SpriteWindowSpritePair pair;

	wrapper_->loadSprite(&pair.sprite, fileName);
		

	pair.windowSprite.handle = spriteCounter_;

	spriteCounter_++;

	pair.windowSprite.width = pair.sprite.orig_width;
	pair.windowSprite.height = pair.sprite.orig_height;

	spriteWindowSpritePairVector_.push_back(pair);

	return pair.windowSprite;
}

WindowSprite Windows::getEmptyWindowSprite(unsigned int width, unsigned int height)
{
	SpriteWindowSpritePair pair;

	wrapper_->emptySprite(&pair.sprite, width, height);

	pair.windowSprite.handle = spriteCounter_;

	spriteCounter_++;

	pair.windowSprite.width = pair.sprite.orig_width;
	pair.windowSprite.height = pair.sprite.orig_height;

	spriteWindowSpritePairVector_.push_back(pair);

	return pair.windowSprite;
}

WindowCanvas Windows::getWindowCanvas(WindowSprite windowSprite)
{
	
	
	Sprite *sprite = 0;
	// find the corresponding sprite for the windowSprite
	sprite = getSprite(windowSprite.handle);

	// get found sprite's canvas
	Canvas canvas;
	wrapper_->getCanvas(&canvas, sprite);
	

	WindowCanvas newWindowCanvas;
	newWindowCanvas.handle = canvasCounter_;
	canvasCounter_++;
	
	newWindowCanvas.width = windowSprite.width;
	newWindowCanvas.height = windowSprite.height;
	newWindowCanvas.data = canvas.data; // windowCanvas shares data with canvas
			

	// associate the sprite's canvas with windowCanvas's handle
	CanvasWindowCanvasPair pair;
	pair.windowCanvas = newWindowCanvas;
	pair.canvas = canvas;
	
	canvasWindowCanvasPairVector_.push_back(pair);

	return newWindowCanvas;
}


void Windows::attachWindowCanvas(WindowCanvas windowCanvas)
{
	for (unsigned int index = 0; index < canvasWindowCanvasPairVector_.size(); index++)
	{
		if (canvasWindowCanvasPairVector_[index].windowCanvas.handle == windowCanvas.handle)
		{
			wrapper_->attachCanvas(&canvasWindowCanvasPairVector_[index].canvas);

			break;
		}
	}
}


// find the corresponding sprite for the windowSprite

Sprite *Windows::getSprite(unsigned int handle)
{
	for (unsigned int index = 0; index < spriteWindowSpritePairVector_.size(); index++)
	{
		if (spriteWindowSpritePairVector_[index].windowSprite.handle == handle)
			return &spriteWindowSpritePairVector_[index].sprite;
	}

	return 0;
}