#include "window.h"
#include "windows.h"
#include "printer.h"


Window::~Window()
{

	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		delete windows_[index];
		windows_[index] = 0;
	}

	windows_.clear();

	for(unsigned int index = 0; index < scrollControlWindows_.size(); index++)
	{
		delete scrollControlWindows_[index];
		scrollControlWindows_[index] = 0;
	}

	scrollControlWindows_.clear();
}

bool Window::isHidden()
{
	return hidden_;
}

void Window::hide()
{
	hidden_ = true;
}

void Window::show()
{
	hidden_ = false;
}

void Window::activate()
{
	active_ = true;
}

void Window::deactivate()
{
	active_ = false;

	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		windows_[index]->deactivate();
	}
}

void Window::logic()
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(!windows_[index]->isHidden()) windows_[index]->logic();
	}
}

Window* Window::createWindow(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data))
{
	Window *newWindow = new Window(x, y, width, height, returnData, callBackFunction, updateFunction);

	newWindow->setWindowSystem(windowSystem_);

	newWindow->setParent(this);
	windows_.push_back(newWindow);

	return newWindow;
	
}

GraphicsWindow* Window::createGraphicsWindow(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data),
	void (*drawFunction)(void *data, GraphicsWindow *source))
{
	GraphicsWindow *newWindow = new GraphicsWindow(x, y, width, height, returnData, callBackFunction, updateFunction, drawFunction);

	newWindow->setWindowSystem(windowSystem_);

	newWindow->setParent(this);
	windows_.push_back(newWindow);

	return newWindow;
	
}



Button* Window::createButton(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData))
{

	Button *newButton = new Button(x, y, width, height, returnData, callBackFunction);

	newButton->setWindowSystem(windowSystem_);

	newButton->setParent(this);
	windows_.push_back(newButton);

	return newButton;
}

TextMenu *Window::createTextMenu(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data))
{
	TextMenu *newTextMenu = new TextMenu(x, y, width, height, returnData, callBackFunction, updateFunction);

	newTextMenu->setWindowSystem(windowSystem_);

	newTextMenu->setParent(this);
	windows_.push_back(newTextMenu);

	return newTextMenu;
}

Text *Window::createText(std::string text, int x, int y, int width, int height, void *returnData, void (*updateFunction)(void *data))
{
	Text *newText = new Text(text, x, y, width, height, returnData, updateFunction);

	newText->setWindowSystem(windowSystem_);

	newText->setParent(this);
	windows_.push_back(newText);

	return newText;
}

TextField *Window::createTextField(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data))
{
	TextField *newTextField = new TextField(x, y, width, height, returnData, callBackFunction, updateFunction);

	newTextField->setWindowSystem(windowSystem_);

	newTextField->setParent(this);
	windows_.push_back(newTextField);

	return newTextField;
}

void Window::createScrollControls()
{
	ScrollControlButton *left = new ScrollControlButton(5, height_ -30 -5, 30, 30);
	left->set_scrollControlButtonType(SCROLL_CONTROL_BUTTON_LEFT);
	addScrollControlWindow(left);

	ScrollControlButton *up = new ScrollControlButton(width_ -30 -5, 5, 30, 30);
	up->set_scrollControlButtonType(SCROLL_CONTROL_BUTTON_UP);
	addScrollControlWindow(up);

	ScrollControlButton *right = new ScrollControlButton(width_ -30 -30 -5 -5, height_ -30 -5, 30, 30);
	right->set_scrollControlButtonType(SCROLL_CONTROL_BUTTON_RIGHT);
	addScrollControlWindow(right);

	ScrollControlButton *down = new ScrollControlButton(width_ -30 -5, height_ -30 -30 -5 -5, 30, 30);
	down->set_scrollControlButtonType(SCROLL_CONTROL_BUTTON_DOWN);
	addScrollControlWindow(down);

}

void Window::setScrollControlsActive(bool active)
{
	scrollControlsActive_ = active;
}

bool Window::handleScrollControlClick(int x, int y, int buttonNumber)
{
	bool clicked = false;
	if(scrollControlsActive_)
	{
		for(unsigned int index = 0; index < scrollControlWindows_.size(); index++)
		{
			if((x >= getChildX(scrollControlWindows_[index]))
				&& (x < getChildX(scrollControlWindows_[index]) + scrollControlWindows_[index]->getWidth())
				&& (y >= getChildY(scrollControlWindows_[index]))
				&& (y < getChildY(scrollControlWindows_[index]) + scrollControlWindows_[index]->getHeight())
				)
			{
				scrollControlWindows_[index]->click(x -getChildX(scrollControlWindows_[index]), y -getChildY(scrollControlWindows_[index]), buttonNumber);
				
				clicked = true;
			}
		}
	}

	return clicked;
}

void Window::click(int x, int y, int buttonNumber)
{
	bool scrollClicked = handleScrollControlClick(x, y, buttonNumber);

	if(scrollClicked) return;

	bool clicked = false;

	// deactivate child windows that aren't clicked
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(!((x >= getChildX(windows_[index]))
			&& (x < getChildX(windows_[index]) + windows_[index]->getWidth())
			&& (y >= getChildY(windows_[index]))
			&& (y < getChildY(windows_[index]) + windows_[index]->getHeight())
			))
		{
			windows_[index]->deactivate();
		}
	}

	// check if child windows are clicked
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(!windows_[index]->isHidden())
		{
			if((x >= getChildX(windows_[index]))
				&& (x < getChildX(windows_[index]) + windows_[index]->getWidth())
				&& (y >= getChildY(windows_[index]))
				&& (y < getChildY(windows_[index]) + windows_[index]->getHeight())
				)
			{

				windows_[index]->click(x -getChildX(windows_[index]), y -getChildY(windows_[index]), buttonNumber);
				windows_[index]->activate();

				clicked = true;
			}
		}
	}

	

	
	// if no child window has been clicked call callback function if there is one
	if(!clicked)
	{
		if(callBackFunction_)
		{
			callBackData_.callBackType = CALLBACK_CLICK;

			clickData_.buttonNumber = buttonNumber;
			clickData_.x = x;
			clickData_.y = y;
			
			callBackData_.callBackValue = &clickData_;

			(*callBackFunction_)(returnData_, this, &callBackData_);
			windowSystem_->update();
		}
	}
}



void Window::addScrollControlWindow(Window *window)
{
	window->setParent(this);
	scrollControlWindows_.push_back(window);
}
Window *Window::getParent()
{
	return parent_;
}

void Window::setParent(Window *parent)
{
	parent_ = parent;
}

Windows *Window::getWindowSystem()
{
	return windowSystem_;
}

void Window::setWindowSystem(Windows *windowSystem)
{
	windowSystem_ = windowSystem;

	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		windows_[index]->setWindowSystem(windowSystem);
	}

	for(unsigned int index = 0; index < scrollControlWindows_.size(); index++)
	{
		scrollControlWindows_[index]->setWindowSystem(windowSystem);
	}
}

// gives the location of the window in relation to the upper left corner of it's parent

int Window::getChildX(Window *child)
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(child == windows_[index]) return child->getX() -scrollX_;
	}

	for(unsigned int index = 0; index < scrollControlWindows_.size(); index++)
	{
		if(child == scrollControlWindows_[index]) return child->getX();
	}

	return -1;
}

int Window::getChildY(Window *child)
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(child == windows_[index]) return child->getY() -scrollY_;
	}

	for(unsigned int index = 0; index < scrollControlWindows_.size(); index++)
	{
		if(child == scrollControlWindows_[index]) return child->getY();
	}

	return -1;
}

// gives the window's screen coordinates
int Window::getGlobalX()
{

	if(parent_)
	{
		return parent_->getChildX(this) + parent_->getGlobalX();
	} else return x_;
}

int Window::getGlobalY()
{

	if(parent_)
	{
		return parent_->getChildY(this) + parent_->getGlobalY();
	} else return y_;
}

int Window::getScrollX()
{
	return scrollX_;
}

int Window::getScrollY()
{
	return scrollY_;
}

void Window::xScroll(int x)
{
	scrollX_ += x;

	if(scrollX_ < 0) scrollX_ = 0;
	
}

void Window::yScroll(int y)
{
	scrollY_ += y;

	if(scrollY_ < 0) scrollY_ = 0;
}


int Window::getX()
{
	return x_;
}

int Window::getY()
{
	return y_;
}

int Window::getWidth()
{
	return width_;
}

int Window::getHeight()
{
	return height_;
}

void Window::update()
{
	if(updateFunction_)
	{
		(*updateFunction_)(returnData_);
	}

	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		windows_[index]->update();
	}
}

void Window::draw()
{
	Wrapper *wrapper = windowSystem_->getWrapper();

	Rect dest;
	dest.x = getGlobalX();
	dest.y = getGlobalY();
	dest.width = width_;
	dest.height = height_;

	Rect clip;

	clip.x = getGlobalX();
	clip.y = getGlobalY();
	clip.width = width_;
	clip.height = height_;

	if (!clipArea(&clip)) return;

	wrapper->drawRectangle(&dest, &clip, 255, 255, 255, 255);

	dest.x = getGlobalX()+1;
	dest.y = getGlobalY()+1;
	dest.width = width_-2;
	dest.height = height_-2;

	

	wrapper->drawRectangle(&dest, &clip, 0, 0, 0, 255);

	if (scrollControlsActive_)
	for (unsigned int index = 0; index < scrollControlWindows_.size(); index++)
	{
		scrollControlWindows_[index]->draw();
	}


	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(!windows_[index]->isHidden())
		windows_[index]->draw();
	}
		
}


bool Window::clipArea(Rect *new_rect)
{

	if(parent_)
	{
		Rect against_this;
		against_this.x = parent_->getGlobalX()+1;
		against_this.y = parent_->getGlobalY()+1;
		against_this.width = parent_->getWidth()-2;
		against_this.height = parent_->getHeight()-2;


		if(
			(new_rect->x >= against_this.x + against_this.width)
			|| (new_rect->x + new_rect->width <= against_this.x)
			|| (new_rect->y >= against_this.y + against_this.height)
			|| (new_rect->y + new_rect->height <= against_this.y)
			)	return false;

		
		if(new_rect->x + new_rect->width > against_this.x + against_this.width)
		{
			int difference =  (new_rect->x + new_rect->width) - (against_this.x + against_this.width);

			new_rect->width -= difference;
		}

		if(new_rect->x < against_this.x)
		{
			int difference = against_this.x - new_rect->x;

			new_rect->width -= difference;
			new_rect->x = against_this.x;
		}

		if(new_rect->y + new_rect->height > against_this.y + against_this.height)
		{
			int difference = (new_rect->y + new_rect->height) - (against_this.y + against_this.height);

			new_rect->height -= difference;
		}

		if(new_rect->y < against_this.y)
		{
			int difference = against_this.y - new_rect->y;

			new_rect->height -= difference;
			new_rect->y = against_this.y;
		}

		if(parent_->clipArea(new_rect) == false) return false;

	}

	return true;
}


GraphicsWindow::GraphicsWindow(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data),
	void (*drawFunction)(void *data, GraphicsWindow *source))
{

	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;
	returnData_ = returnData;
	callBackFunction_ = callBackFunction;
	updateFunction_ = updateFunction;
	drawFunction_ = drawFunction;

	createScrollControls();
}

GraphicsWindow::~GraphicsWindow()
{
	spriteInstances_.clear();
}

void GraphicsWindow::drawWindowSprite(WindowSprite *sprite, Rect *dest, Rect *source, bool colorKey, int alpha)
{
	
	SpriteInstance newSprite;

	newSprite.handle = sprite->handle;

	newSprite.colorkey = colorKey;
	newSprite.alpha = alpha;

	newSprite.dest.x = dest->x;
	newSprite.dest.y = dest->y;

	newSprite.dest.width = dest->width;
	newSprite.dest.height = dest->height;

	if(source)
	{
		newSprite.source.x = source->x;
		newSprite.source.y = source->y;

		newSprite.source.width = source->width;
		newSprite.source.height = source->height;
	}
	else
	{
		newSprite.source.x = 0;
		newSprite.source.y = 0;

		newSprite.source.width = sprite->width;
		newSprite.source.height = sprite->height;
	}

	spriteInstances_.push_back(newSprite);
}


void GraphicsWindow::draw()
{

	Window::draw();

	if(drawFunction_) (*drawFunction_)(returnData_, this);

	Wrapper *wrapper = windowSystem_->getWrapper();

	Rect dest;
	dest.x = getGlobalX();
	dest.y = getGlobalY();
	dest.width = width_;
	dest.height = height_;

	Rect clip;

	clip.x = getGlobalX();
	clip.y = getGlobalY();
	clip.width = width_;
	clip.height = height_;

	if (clipArea(&clip))
	{
		
		for(unsigned int index = 0; index < spriteInstances_.size(); index++)
		{
			Rect dest;

			dest.x = getGlobalX() + spriteInstances_[index].dest.x -scrollX_;
			dest.y = getGlobalY() + spriteInstances_[index].dest.y -scrollY_;
			dest.width = spriteInstances_[index].dest.width;
			dest.height = spriteInstances_[index].dest.height;

			Rect source = spriteInstances_[index].source;

			Sprite *sprite = windowSystem_->getSprite(spriteInstances_[index].handle);

			wrapper->drawSprite(sprite, &dest, &source, &clip, spriteInstances_[index].colorkey, spriteInstances_[index].alpha);

		}

		
	}

	spriteInstances_.clear();

	
}



Button::Button(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData))
{
	buttonName_ = "";
	buttonSprite_ = 0;
	
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;
	returnData_ = returnData;
	callBackFunction_ = callBackFunction;

	scrollX_ = 0;
	scrollY_ = 0;

}

void Button::set_name(std::string name)
{
	buttonName_ = name;
}

void Button::set_Sprite(Sprite *sprite)
{
	buttonSprite_ = sprite;
}

void Button::set_SpriteRect(Rect *rect)
{
	buttonSpriteRect_.x = rect->x;
	buttonSpriteRect_.y = rect->y;
	buttonSpriteRect_.width = rect->width;
	buttonSpriteRect_.height = rect->height;
}

void Button::draw()
{
	Window::draw();
	Wrapper *wrapper = windowSystem_->getWrapper();

	Rect dest;
	dest.x = getGlobalX()+1;
	dest.y = getGlobalY()+1;
	dest.width = width_-2;
	dest.height = height_-2;

	Rect clip;

	clip.x = getGlobalX()+1;
	clip.y = getGlobalY()+1;
	clip.width = width_-2;
	clip.height = height_-2;

	if(!clipArea(&clip)) return;

	if(buttonSprite_)
	{
		wrapper->drawSprite(buttonSprite_, &dest, &buttonSpriteRect_, &clip, 0, 255);
	}
	else wrapper->drawRectangle(&dest, &clip, 255, 255, 0, 128);

	Printer *printer = windowSystem_->getPrinter();
	
	int text_x = dest.x + width_/2 -printer->textWidth((char *)buttonName_.c_str())/2;
	int text_y = dest.y + height_/2 -printer->textHeight((char *)buttonName_.c_str())/2;

	printer->print((char *)buttonName_.c_str(), text_x, text_y, &clip);
}

ScrollControlButton::ScrollControlButton(int x, int y, int width, int height)
{
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;
}

void ScrollControlButton::set_scrollControlButtonType(int type)
{
	scrollControlButtonType_ = type;
}

void ScrollControlButton::click(int x, int y, int buttonNumber)
{
	switch(scrollControlButtonType_)
	{
	case SCROLL_CONTROL_BUTTON_LEFT:
		parent_->xScroll(-30);
		break;
	case SCROLL_CONTROL_BUTTON_UP:
		parent_->yScroll(-30);
		break;
	case SCROLL_CONTROL_BUTTON_RIGHT:
		parent_->xScroll(30);
		break;
	case SCROLL_CONTROL_BUTTON_DOWN:
		parent_->yScroll(30);
		break;
	default: break;
	}
	
}




Text::Text(std::string text, int x, int y, int width, int height, void *returnData, void (*updateFunction)(void *data))
{
	text_ = text;
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;
	returnData_  = returnData;
	updateFunction_ = updateFunction;
}

void Text::draw()
{
	Rect clip;

	clip.x = getGlobalX();
	clip.y = getGlobalY();
	clip.width = width_;
	clip.height = height_;

	if (!clipArea(&clip)) return;


	Printer *printer = windowSystem_->getPrinter();

	int text_x = getGlobalX() + width_/2 -printer->textWidth((char *)text_.c_str())/2;
	int text_y = getGlobalY() + height_/2 -printer->textHeight((char *)text_.c_str())/2;
	
	printer->print((char*)text_.c_str(), text_x, text_y, &clip);
}

void Text::set_text(std::string text)
{
	text_ = text;
}

std::string Text::get_text()
{
	return text_;

}

TextMenu::TextMenu(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data))
{
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;
	returnData_ = returnData;
	callBackFunction_ = callBackFunction;
	updateFunction_ = updateFunction;

	textMenuChosenData_.row = 0;

	createScrollControls();
}

void TextMenu::add_text(std::string text)
{

	if(texts_.size() == 0)
	{
		textMenuChosenData_.row = 0;
		texts_.push_back(text);
	}
	else
	{
		texts_.insert(texts_.begin() + textMenuChosenData_.row+1, text);
		textMenuChosenData_.row++;
	}
	
}

void TextMenu::clear()
{
	texts_.clear();
}

int TextMenu::get_row()
{
	return textMenuChosenData_.row;
}
void TextMenu::set_row(int row)
{
	textMenuChosenData_.row = row;
}

void TextMenu::draw()
{
	Wrapper *wrapper = windowSystem_->getWrapper();

	Rect dest;
	dest.x = getGlobalX();
	dest.y = getGlobalY();
	dest.width = width_;
	dest.height = height_;

	Rect clip;

	clip.x = getGlobalX();
	clip.y = getGlobalY();
	clip.width = width_;
	clip.height = height_;

	if (!clipArea(&clip)) return;

	wrapper->drawRectangle(&dest, &clip, 0, 255, 0, 128);

	Printer *printer = windowSystem_->getPrinter();

	for(unsigned int index = 0; index < texts_.size(); index++)
	{
		if(textMenuChosenData_.row == index)
		{
			Rect background;
			background.x = getGlobalX();
			background.y = getGlobalY() -scrollY_ + 30 * index;
			background.width = width_;
			background.height = 30;

			wrapper->drawRectangle(&background, &clip, 0, 0, 255, 128);
		}

		printer->print((char*)texts_[index].c_str(), getGlobalX() + 5 -scrollX_, getGlobalY() + index*30 + 5 -scrollY_, &clip);
	}
	
	if(scrollControlsActive_)
	{
		for(unsigned int index = 0; index < scrollControlWindows_.size(); index++)
		{
			scrollControlWindows_[index]->draw();
		}
	}
}

void TextMenu::click(int x, int y, int buttonNumber)
{

	bool scrollClicked = handleScrollControlClick(x, y, buttonNumber);

	if(scrollClicked) return;

	for(unsigned int index = 0; index < texts_.size(); index++)
	{
		Rect text;

		text.x = 0;
		text.width = width_;
		text.y = index * 30 -scrollY_;
		text.height = 30;

		if((x >= text.x)
			&& (x < text.x + text.width)
			&& (y >= text.y)
			&& (y < text.y + text.height)
			)
		{
			textMenuChosenData_.row = index;

			if(callBackFunction_)
			{
				callBackData_.callBackType = CALLBACK_TEXT_CHOSEN;
				callBackData_.callBackValue = &textMenuChosenData_;

				(*callBackFunction_)(returnData_, this, &callBackData_);
				windowSystem_->update();
			}

		}
	}

}

TextField::TextField(int x, int y, int width, int height, void *returnData, void (*callBackFunction)(void *data, Window *source, CallBackData *callBackData), void (*updateFunction)(void *data))
{
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;
	returnData_ = returnData;
	callBackFunction_ = callBackFunction;
	updateFunction_ = updateFunction;

	position_ = 0;
}

void TextField::logic()
{

	Input *input = windowSystem_->getInput();

	if(active_)
	{

		for(int key = KEY_a; key <= KEY_z; key++)
		{
			if(input->KEY_PRESSED[key])
			{
				char character = key - KEY_a + 'a';

				text_.insert(text_.begin() + position_, character);
				position_++;
			}
		}

		if(input->KEY_PRESSED[KEY_SPACE])
		{
			text_.insert(text_.begin() + position_, ' ');
			position_++;
		}

		if(input->KEY_PRESSED[KEY_BACKSPACE] && position_ > 0)
		{
			text_.erase(text_.begin() + position_ -1);
			position_--;
		}

		if(input->KEY_PRESSED[KEY_RETURN])
		{
			textFieldData_.text = text_;

			if(callBackFunction_)
			{
				callBackData_.callBackType = CALLBACK_TEXTFIELD_MODIFIED;
				callBackData_.callBackValue = &textFieldData_;
			
				(*callBackFunction_)(returnData_, this, &callBackData_);
				windowSystem_->update();
			}

			set_text(text_);
			deactivate();
		}
	}
	
}

void TextField::draw()
{
	Rect dest;
	dest.x = getGlobalX();
	dest.y = getGlobalY();
	dest.width = width_;
	dest.height = height_;

	Rect clip;

	clip.x = getGlobalX();
	clip.y = getGlobalY();
	clip.width = width_;
	clip.height = height_;

	if (!clipArea(&clip)) return;

	Wrapper *wrapper = windowSystem_->getWrapper();
	
	wrapper->drawRectangle(&dest, &clip, 0, 0, 0, 128);

	Printer *printer = windowSystem_->getPrinter();
	printer->print((char*)text_.c_str(), getGlobalX() + 10, getGlobalY() +  (height_ -30)/2, &clip);

	if(active_)
	{
		
		Rect rect;
		rect.x = getGlobalX() + 10 - 1 + printer->textX((char*)text_.c_str(), position_);
		rect.y = getGlobalY() +  (height_ -30)/2;
		rect.width = 1;
		rect.height = 15;

		wrapper->drawRectangle(&rect, &clip, 255, 0, 0, 128);

		for(unsigned int index = 0; index <= text_.size(); index++)
		{
			rect.x = rect.x = getGlobalX() + 10 + printer->textX((char*)text_.c_str(), index);
			rect.y = getGlobalY() +  (height_ -30)/2 + 20;
			rect.width = 1;
			rect.height = 5;

			wrapper->drawRectangle(&rect, &clip, 0, 255, 0, 128);
		}
	}


}

void TextField::deactivate()
{
	active_ = false;

	text_ = oldText_;
}
void TextField::click(int x, int y, int buttonNumber)
{
	position_ = 0;

	
	Printer *printer = windowSystem_->getPrinter();
	position_ = printer->textIndex((char*)text_.c_str(), x-10);

	if(position_ < 0) position_ = 0;
}

void TextField::set_text(std::string text)
{
	oldText_ = text;
	text_ = text;
	position_ = (unsigned int)text_.size();
}

std::string TextField::get_text()
{
	return text_;
}