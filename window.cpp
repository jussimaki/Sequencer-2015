#include "window.h"
#include "windows.h"
#include "printer.h"

#include <sstream>

void Event::add(void *object, void(*functionPointer)(void *, Widget *, Event *))
{
	pointers_.push_back(functionPointer);
	objects_.push_back(object);
}

void Event::clear()
{

	pointers_.clear();
	objects_.clear();
}

void MouseEvent::call(int localX, int localY, int widgetWidth, int widgetHeight, Widget *widget)
{

	localX_ = localX;
	localY_ = localY;
	widgetWidth_ = widgetWidth;
	widgetHeight_ = widgetHeight;

	for (int index = 0; index < pointers_.size(); index++)
	{
		pointers_[index](objects_[index], widget, this);
	}
}

void TextEvent::call(std::string text, Widget *widget)
{
	text_ = text;

	for (int index = 0; index < pointers_.size(); index++)
	{
		pointers_[index](objects_[index], widget, this);
	}
}

void ChooseEvent::call(int number, Widget *widget)
{
	number_ = number;

	for (int index = 0; index < pointers_.size(); index++)
	{
		pointers_[index](objects_[index], widget, this);
	}
}

bool clipArea(Rect *new_rect, Rect *old_rect)
{
	Rect against_this;

	against_this.x = old_rect->x;
	against_this.y = old_rect->y;
	against_this.width = old_rect->width;
	against_this.height = old_rect->height;

	if (
		(new_rect->x >= against_this.x + against_this.width)
		|| (new_rect->x + new_rect->width <= against_this.x)
		|| (new_rect->y >= against_this.y + against_this.height)
		|| (new_rect->y + new_rect->height <= against_this.y)
		)	return false;

	if (new_rect->x + new_rect->width > against_this.x + against_this.width)
	{
		int difference = (new_rect->x + new_rect->width) - (against_this.x + against_this.width);
		new_rect->width -= difference;
	}

	if (new_rect->x < against_this.x)
	{
		int difference = against_this.x - new_rect->x;

		new_rect->width -= difference;
		new_rect->x = against_this.x;
	}

	if (new_rect->y + new_rect->height > against_this.y + against_this.height)
	{
		int difference = (new_rect->y + new_rect->height) - (against_this.y + against_this.height);

		new_rect->height -= difference;
	}

	if (new_rect->y < against_this.y)
	{
		int difference = against_this.y - new_rect->y;

		new_rect->height -= difference;
		new_rect->y = against_this.y;
	}
	
	return true;
}


Window::~Window()
{

}


void Window::activate()
{
	active_ = true;
}

void Window::deactivate()
{
	active_ = false;

	
	for(unsigned int index = 0; index < regions_.size(); index++)
	{
		if (regions_[index].widget)
			regions_[index].widget->deactivate();

	}
	
}

bool mouseOnRectangle(int x, int y, int width, int height, int mouseX, int mouseY)
{
	if ((mouseX >= x)
		&& (mouseX < x + width)
		&& (mouseY >= y)
		&& (mouseY < y + height)) return true;
	else return false;
}

bool Widget::mouseOnWidget(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY)
{

	Rect newClip;

	newClip.x = x;
	newClip.y = y;
	newClip.width = width;
	newClip.height = height;

	if (!clipArea(&newClip, &oldClip)) return false;

		
	if ((globalMouseX >= newClip.x)
		&& (globalMouseX < newClip.x + newClip.width)
		&& (globalMouseY >= newClip.y)
		&& (globalMouseY < newClip.y + newClip.height)) return true;
	else return false;
}

void Window::mouseEvent(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input)
{
	Rect newClip;
	
	if(!getNewClip(x, y, width, height, oldClip, &newClip)) return;

	if (mouseOnWidget(newClip.x, newClip.y, newClip.width, newClip.height, oldClip, globalMouseX, globalMouseY))
	{
		int localX = globalMouseX - x;
		int localY = globalMouseY - y;

		if (input->MOUSE_BUTTON_CLICKED[0])
		{
			onClick_.call(localX, localY, width, height, this);

			click(localX, localY, x, y, width, height);
		}

		generalMouseEvent_.call(localX, localY, width, height, this);
	}
}

void Widget::logic(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input)
{

}


// called on widget if mouse is on it (or if user clicks and drags)
void Widget::mouseEvent(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input)
{
	int localX = globalMouseX -x;
	int localY = globalMouseY -y;
	

	if (input->MOUSE_BUTTON_CLICKED[0])
	{
		activate();
		onClick_.call(localX, localY, width, height, this);

		click(localX, localY, x, y, width, height);
	}

	generalMouseEvent_.call(localX, localY, width, height, this);
}

void Widget::click(int localX, int localY, int x, int y, int width, int height)
{
}


void Widget::drawBorders(int x, int y, int width, int height, Rect oldClip, unsigned char r, unsigned char g, unsigned char b)
{
	Rect newClip;

	newClip.x = x;
	newClip.y = y;
	newClip.width = width;
	newClip.height = height;

	if (!clipArea(&newClip, &oldClip)) return;

	Wrapper *wrapper = windowSystem_->getWrapper();

	Rect rectangle;
	rectangle.x = x;
	rectangle.y = y;
	rectangle.width = width;
	rectangle.height = 1;

	wrapper->drawRectangle(&rectangle, &newClip, r, g, b, 255);


	rectangle.x = x;
	rectangle.y = y + height - 1;
	rectangle.width = width;
	rectangle.height = 1;

	wrapper->drawRectangle(&rectangle, &newClip, r, g, b, 255);

	rectangle.x = x;
	rectangle.y = y;
	rectangle.width = 1;
	rectangle.height = height;

	wrapper->drawRectangle(&rectangle, &newClip, r, g, b, 255);


	rectangle.x = x + width - 1;
	rectangle.y = y;
	rectangle.width = 1;
	rectangle.height = height;

	wrapper->drawRectangle(&rectangle, &newClip, r, g, b, 255);


}

void Widget::drawBox(int x, int y, int width, int height, Rect oldClip, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha)
{
	Rect newClip;

	newClip.x = x;
	newClip.y = y;
	newClip.width = width;
	newClip.height = height;

	if (!clipArea(&newClip, &oldClip)) return;

	Wrapper *wrapper = windowSystem_->getWrapper();

	Rect rectangle;
	rectangle.x = x;
	rectangle.y = y;
	rectangle.width = width;
	rectangle.height = height;

	wrapper->drawRectangle(&rectangle, &newClip, r, g, b, alpha);
}

void Widget::draw(int x, int y, int width, int height, Rect oldClip)
{
	drawBox(x, y, width, height, oldClip, 64, 64, 64, 224);
	drawBorders(x, y, width, height, oldClip, 255, 255, 255);
	Rect newClip;

	newClip.x = x;
	newClip.y = y;
	newClip.width = width;
	newClip.height = height;

	if (!clipArea(&newClip, &oldClip)) return;
		

}

void Label::draw(int x, int y, int width, int height, Rect oldClip)
{
	Rect newClip;

	newClip.x = x;
	newClip.y = y;
	newClip.width = width;
	newClip.height = height;

	if (!clipArea(&newClip, &oldClip)) return;

	Printer *printer = windowSystem_->getPrinter();

	printer->print((char*)text_.c_str(), x, y + height / 2 - printer->maxHeight() / 2, &newClip);
}

void Window::addRegion(std::string regionName, int x, WINDOW_ATTACH_TYPE x_attach, std::string x_attach_name,
	int y, WINDOW_ATTACH_TYPE y_attach, std::string y_attach_name,
	int width, WINDOW_ATTACH_TYPE width_attach, std::string width_attach_name,
	int height, WINDOW_ATTACH_TYPE height_attach, std::string height_attach_name)
{
	Region newRegion;

	newRegion.x = x;
	newRegion.x_attach = x_attach;
	newRegion.x_attach_name = x_attach_name;

	newRegion.y = y;
	newRegion.y_attach = y_attach;
	newRegion.y_attach_name = y_attach_name;

	newRegion.width = width;
	newRegion.width_attach = width_attach;
	newRegion.width_attach_name = width_attach_name;

	newRegion.height = height;
	newRegion.height_attach = height_attach;
	newRegion.height_attach_name = height_attach_name;

	newRegion.widget = 0;

	newRegion.regionName = regionName;

	regions_.push_back(newRegion);
}

void Window::setWidget(std::string regionName, Widget *widget)
{
	for (int index = 0; index < regions_.size(); index++)
	{
		if (regions_[index].regionName == regionName)
		{
			regions_[index].widget = widget;
			widget->setWindowSystem(windowSystem_);

			return;
		}
	}
}

void Window::clearVisitedRegions()
{
	for (int index = 0; index < regions_.size(); index++)
		regions_[index].visited = false;
}

int Window::findRegionCoordinate(std::string regionAttachName, WINDOW_ATTACH_TYPE attachType, int x, int y, int width, int height, int sum)
{

	if (regionAttachName == "")
	{
		
		switch (attachType)
		{
		case ATTACH_LEFT:
			return 0 + sum;
			break;
		case ATTACH_RIGHT:
			return width -1 + sum;
			break;
		case ATTACH_TOP:
			return 0 + sum;
			break;
		case ATTACH_BOTTOM:
			return height - 1 + sum;
			break;
		case ATTACH_NONE:
		default:
			return sum;
			break;
		}
	}


	Region *region = getRegion(regionAttachName);

	if (!region) return 0;

	if (region->visited) return 0;

	region->visited = true;



	int result;
	switch (attachType)
	{
	case ATTACH_LEFT:
		result = findRegionCoordinate(region->x_attach_name, region->x_attach, x, y, width, height, region->x) + sum;
		break;

	case ATTACH_RIGHT:
		
		if (region->width_attach != ATTACH_NONE)
		{
			result = findRegionCoordinate(region->width_attach_name, region->width_attach, x, y, width, height, region->width) + sum;
		}
		else
		{
			result = findRegionCoordinate(region->x_attach_name, region->x_attach, x, y, width, height, region->x) + region->width - 1 + sum;
		}
		break;

	case ATTACH_TOP:
		result = findRegionCoordinate(region->y_attach_name, region->y_attach, x, y, width, height, region->y) + sum;
		break;

	case ATTACH_BOTTOM:

		if (region->height_attach != ATTACH_NONE)
		{
			result = findRegionCoordinate(region->height_attach_name, region->height_attach, x, y, width, height, region->height) + sum;
		}
		else
		{
			result = findRegionCoordinate(region->y_attach_name, region->y_attach, x, y, width, height, region->y) + region->height - 1 + sum;
		}
		break;

	case ATTACH_NONE:
	default:

		result = sum;
		break;
	}

	region->visited = false;

	return result;
}

Region * Window::getRegion(std::string regionName)
{
	
	for (int index = 0; index < regions_.size(); index++)
	{
		if (regions_[index].regionName == regionName) return &regions_[index];
	}

	return 0;
}

Rect Window::getRegionRect(std::string regionName, int x, int y, int width, int height)
{
	Rect rect;

	rect.width = 0;
	rect.height = 0;


	Region *region = getRegion(regionName);

	if (!region) return rect;

	
	
	clearVisitedRegions();
	int x1 = findRegionCoordinate(regionName, ATTACH_LEFT, x, y, width, height, 0);
	int y1 = findRegionCoordinate(regionName, ATTACH_TOP, x, y, width, height, 0);
	int x2 = findRegionCoordinate(regionName, ATTACH_RIGHT, x, y, width, height, 0);
	int y2 = findRegionCoordinate(regionName, ATTACH_BOTTOM, x, y, width, height, 0);


	rect.x = x + x1;
	rect.y = y + y1;
	rect.width = x2 - x1 + 1;
	rect.height = y2 - y1 + 1;

	
	return rect;
}

int Window::getScrollWidth(int x, int y, int width, int height)
{
	int w = 0;

	for (int index = 0; index < regions_.size(); index++)
	{
		std::string regionName = regions_[index].regionName;

		int x2 = findRegionCoordinate(regionName, ATTACH_RIGHT, x, y, width, height, 0) +5;

		if (w < x2) w = x2;
	}

	return w;
}

int Window::getScrollHeight(int x, int y, int width, int height)
{
	int h = 0;

	for (int index = 0; index < regions_.size(); index++)
	{
		std::string regionName = regions_[index].regionName;

		int y2 = findRegionCoordinate(regionName, ATTACH_BOTTOM, x, y, width, height, 0) +5;

		if (h < y2) h = y2;
	}

	return h;
}

void Window::click(int localX, int localY, int x, int y, int width, int height)
{
}


void Window::logic(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input)
{
		
	int localMouseX = globalMouseX - x;
	int localMouseY = globalMouseY - y;


	if (input->MOUSE_BUTTON_CLICKED[0])
		deactivate();

	bool clicked = false;

	for (int button = 0; button < 3; button++)
	{
		if (input->MOUSE_BUTTON_CLICKED[button]) clicked = true;
	}

	bool mouseMoved = false;

	if (input->mouse_move_x != 0 || input->mouse_move_y != 0) mouseMoved = true;



	for (int index = 0; index < regions_.size(); index++)
	{
		if (regions_[index].widget)
		{
			if (!regions_[index].widget->disabled())
			{
				Rect widget = getRegionRect(regions_[index].regionName, x, y, width, height);

				widget.x -= scrollX_;
				widget.y -= scrollY_;

				Rect newClip;
				newClip.x = x + 1;
				newClip.y = y + 1;
				newClip.width = width - 2;
				newClip.height = height - 2;

				bool mouseOn = mouseOnWidget(widget.x, widget.y, widget.width, widget.height, newClip, globalMouseX, globalMouseY);
				bool *buttonHeld = regions_[index].widget->getMouseButtonHeldDown();

				if (mouseOn)
				{
					for (int button = 0; button < 3; button++)
						if (input->MOUSE_BUTTON_CLICKED[button]) buttonHeld[button] = true;
				}

				bool heldAndReleased = false;

				for (int button = 0; button < 3; button++)
				{
					if (buttonHeld[button] && input->MOUSE_BUTTON_RELEASED[button])
					{
						buttonHeld[button] = false;
						heldAndReleased = true;
					}
				}

				bool held = false;

				for (int button = 0; button < 3; button++)
					if (buttonHeld[button]) held = true;



				if (
					(mouseOn && (mouseMoved || clicked))
					|| heldAndReleased
					|| (held && mouseMoved)

					)
					regions_[index].widget->mouseEvent(widget.x, widget.y, widget.width, widget.height, newClip, globalMouseX, globalMouseY, input);





				regions_[index].widget->logic(widget.x, widget.y, widget.width, widget.height, newClip, globalMouseX, globalMouseY, input);
			}
		}
	}

	
	if (input->MOUSE_BUTTON_CLICKED[0])
	{
		mouseEvent(x, y, width, height, oldClip, globalMouseX, globalMouseY, input);
	}



	handleScrollControls(x, y, width, height, oldClip, globalMouseX, globalMouseY, input);

}



void Window::handleScrollControls(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input)
{
	int scrollWidth = getScrollWidth(x, y, width, height);
	int scrollHeight = getScrollHeight(x, y, width, height);


	bool hScroll = false;
	bool vScroll = false;

	for (int index = 0; index < 2; index++)
	{
		int w = width;
		if (vScroll) w -= scrollControlSize_;

		if (hScrollControlsActive_ && scrollWidth > w) hScroll = true;

		int h = height;
		if (hScroll) h -= scrollControlSize_;

		if (vScrollControlsActive_ && scrollHeight > h) vScroll = true;
	}
		
	int sWidth = scrollWidth;
	if (vScroll) sWidth += scrollControlSize_;
	
	if (scrollX_ > sWidth - width) scrollX_ = sWidth - width;
	if (scrollX_ < 0) scrollX_ = 0;


	int sHeight = scrollHeight;
	if (hScroll) sHeight += scrollControlSize_;

	if (scrollY_ > sHeight - height) scrollY_ = sHeight - height;
	if (scrollY_ < 0) scrollY_ = 0;

	
	Rect up;
	up.x = x + width - scrollControlSize_;
	up.width = scrollControlSize_;
	up.y = y;
	up.height = scrollControlSize_;

	Rect down;
	down.x = up.x;
	down.width = scrollControlSize_;
	if (!hScroll) down.y = y + height - scrollControlSize_;
	else down.y = y + height - scrollControlSize_*2;
	down.height = scrollControlSize_;

	Rect left;
	left.x = x;
	left.width = scrollControlSize_;
	left.y = y + height - scrollControlSize_;
	left.height = scrollControlSize_;

	Rect right;
	if (!vScroll) right.x = x + width - scrollControlSize_;
	else right.x = x + width - scrollControlSize_*2;
	right.width = scrollControlSize_;
	right.y = left.y;
	right.height = scrollControlSize_;

	



	if (input->MOUSE_BUTTON_CLICKED[0])
	{
		if (hScroll)
		{
			if (mouseOnWidget(left.x, left.y, left.width, left.height, oldClip, globalMouseX, globalMouseY))
			{
				scrollX_ -= scrollAmount_;

				if (scrollX_ < 0) scrollX_ = 0;

			}
			if (mouseOnWidget(right.x, right.y, right.width, right.height, oldClip, globalMouseX, globalMouseY))
			{
				scrollX_ += scrollAmount_;

				if (scrollX_ > sWidth - width) scrollX_ = sWidth - width;
			}
		}

		if (vScroll)
		{
			if (mouseOnWidget(up.x, up.y, up.width, up.height, oldClip, globalMouseX, globalMouseY))
			{
				scrollY_ -= scrollAmount_;

				if (scrollY_ < 0) scrollY_ = 0;

			}
			if (mouseOnWidget(down.x, down.y, down.width, down.height, oldClip, globalMouseX, globalMouseY))
			{
				scrollY_ += scrollAmount_;

				if (scrollY_ > sHeight - height) scrollY_ = sHeight - height;
			}
		}
	}
}

Window *Window::getParent()
{
	return parent_;
}

void Window::setParent(Window *parent)
{
	parent_ = parent;
}

void Window::setWindowSystem(Windows *windowSystem)
{
	windowSystem_ = windowSystem;
	
	for(unsigned int index = 0; index < regions_.size(); index++)
	{
		if (regions_[index].widget)
			regions_[index].widget->setWindowSystem(windowSystem);
			
	}

	
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



void Window::update()
{
	
}

bool Window::getNewClip(int x, int y, int width, int height, Rect oldClip, Rect *newClip)
{
	int scrollWidth = getScrollWidth(x, y, width, height);
	int scrollHeight = getScrollHeight(x, y, width, height);

	newClip->x = x +1;
	newClip->y = y +1;
	newClip->width = width -2;
	newClip->height = height -2;

	bool hScroll = false;
	bool vScroll = false;

	for (int index = 0; index < 2; index++)
	{
		int w = width;
		if (vScroll) w -= scrollControlSize_;

		if (hScrollControlsActive_ && scrollWidth > w) hScroll = true;

		int h = height;
		if (hScroll) h -= scrollControlSize_;

		if (vScrollControlsActive_ && scrollHeight > h) vScroll = true;
	}


	if (vScroll) newClip->width -= (scrollControlSize_ - 1);
	if (hScroll) newClip->height -= (scrollControlSize_ - 1);

	if (newClip->width < 0) newClip->width = 0;
	if (newClip->height < 0) newClip->height = 0;

	return clipArea(newClip, &oldClip);
}

void Window::drawScrollControls(int x, int y, int width, int height, Rect oldClip)
{
	int scrollWidth = getScrollWidth(x, y, width, height);
	int scrollHeight = getScrollHeight(x, y, width, height);

	bool hScroll = false;
	bool vScroll = false;

	for (int index = 0; index < 2; index++)
	{
		int w = width;
		if (vScroll) w -= scrollControlSize_;

		if (hScrollControlsActive_ && scrollWidth > w) hScroll = true;

		int h = height;
		if (hScroll) h -= scrollControlSize_;

		if (vScrollControlsActive_ && scrollHeight > h) vScroll = true;
	}

	int sWidth = scrollWidth;
	if (vScroll) sWidth += scrollControlSize_;

	if (scrollX_ > sWidth - width) scrollX_ = sWidth - width;
	if (scrollX_ < 0) scrollX_ = 0;


	int sHeight = scrollHeight;
	if (hScroll) sHeight += scrollControlSize_;

	if (scrollY_ > sHeight - height) scrollY_ = sHeight - height;
	if (scrollY_ < 0) scrollY_ = 0;

	Rect up;
	up.x = x + width - scrollControlSize_;
	up.width = scrollControlSize_;
	up.y = y;
	up.height = scrollControlSize_;

	Rect down;
	down.x = up.x;
	down.width = scrollControlSize_;
	if (!hScroll) down.y = y + height - scrollControlSize_;
	else down.y = y + height - scrollControlSize_*2;
	down.height = scrollControlSize_;

	Rect left;
	left.x = x;
	left.width = scrollControlSize_;
	left.y = y + height - scrollControlSize_;
	left.height = scrollControlSize_;

	Rect right;
	if (!vScroll) right.x = x + width - scrollControlSize_;
	else right.x = x + width - scrollControlSize_*2;
	right.width = scrollControlSize_;
	right.y = left.y;
	right.height = scrollControlSize_;

	if (vScroll)
	{
		int minus = 0;
		int buttons = scrollControlSize_*2;

		if (hScroll)
		{
			buttons += scrollControlSize_;
			minus = scrollControlSize_;
		}

		int difference = sHeight - height;
		int areaMinusButtons = height - buttons;
		int scrollPerPixel = 3;

		int result;
		result = difference / scrollPerPixel;
		int remainder = difference %scrollPerPixel;

		if (remainder) result++;

		int size = areaMinusButtons - result;

		int toBeAdded = (result*scrollPerPixel - difference);

		int scrollBoxY;

		scrollBoxY = ((scrollY_ + toBeAdded) / scrollPerPixel);


		if (size < scrollBarMinSize_)
		{
			size = scrollBarMinSize_;
			scrollPerPixel = difference / (areaMinusButtons - size);
			int remainder = difference % (areaMinusButtons - size);

			if (remainder) scrollPerPixel++;

			scrollBoxY = (scrollY_ / scrollPerPixel);
			scrollBoxY += (areaMinusButtons - size - (difference / scrollPerPixel))*scrollY_ / difference;
		}


		drawBorders(x + width - scrollControlSize_, y, scrollControlSize_, height - minus, oldClip, 255, 255, 255);

		drawBox(up.x, up.y, up.width, up.height, oldClip, 255, 255, 0, 192);
		drawBorders(up.x, up.y, up.width, up.height, oldClip, 255, 255, 255);

		drawBox(down.x, down.y, down.width, down.height, oldClip, 255, 255, 0, 192);
		drawBorders(down.x, down.y, down.width, down.height, oldClip, 255, 255, 255);


		drawBox(x + width - scrollControlSize_, y + scrollControlSize_ + scrollBoxY, scrollControlSize_, size, oldClip, 0, 255, 0, 192);
		drawBorders(x + width - scrollControlSize_, y + scrollControlSize_ + scrollBoxY, scrollControlSize_, size, oldClip, 255, 255, 255);
	}

	if (hScroll)
	{
		int buttons = 2*scrollControlSize_;
		int minus = 0;
		if (vScroll)
		{
			buttons += scrollControlSize_;
			minus = scrollControlSize_;
		}

		int difference = sWidth - width;
		int areaMinusButtons = width - buttons;
		int scrollPerPixel = 3;

		int result;
		result = difference / scrollPerPixel;
		int remainder = difference %scrollPerPixel;

		if (remainder) result++;

		int size = areaMinusButtons - result;

		int toBeAdded = (result*scrollPerPixel - difference);

		int scrollBoxX;

		scrollBoxX = ((scrollX_ + toBeAdded) / scrollPerPixel);


		if (size < scrollBarMinSize_)
		{
			size = scrollBarMinSize_;
			scrollPerPixel = difference / (areaMinusButtons - size);
			int remainder = difference % (areaMinusButtons - size);

			if (remainder) scrollPerPixel++;

			scrollBoxX = (scrollX_ / scrollPerPixel);
			scrollBoxX += (areaMinusButtons - size - (difference / scrollPerPixel))*scrollX_ / difference;
		}


		drawBorders(x, y + height - scrollControlSize_, width - minus, scrollControlSize_, oldClip, 255, 255, 255);


		drawBox(left.x, left.y, left.width, left.height, oldClip, 255, 255, 0, 192);
		drawBorders(left.x, left.y, left.width, left.height, oldClip, 255, 255, 255);


		drawBox(right.x, right.y, right.width, right.height, oldClip, 255, 255, 0, 192);
		drawBorders(right.x, right.y, right.width, right.height, oldClip, 255, 255, 255);


		drawBox(x + scrollControlSize_ + scrollBoxX, y + height - scrollControlSize_, size, scrollControlSize_, oldClip, 0, 255, 0, 192);
		drawBorders(x + scrollControlSize_ + scrollBoxX, y + height - scrollControlSize_, size, scrollControlSize_, oldClip, 255, 255, 255);


	}

}

void Window::draw(int x, int y, int width, int height, Rect oldClip)
{
	
	Widget::draw(x, y, width, height, oldClip);

	
	Rect newClip;
	
	if(!getNewClip(x, y, width, height, oldClip, &newClip)) return;

	
	for (int index = 0; index < regions_.size(); index++)
	{
		if (regions_[index].widget)
		{
			Rect rect = getRegionRect(regions_[index].regionName, x, y, width, height);
			
			rect.x -= scrollX_;
			rect.y -= scrollY_;

			regions_[index].widget->draw(rect.x, rect.y, rect.width, rect.height, newClip);
		}
	}

	drawScrollControls(x, y, width, height, oldClip);

}


void GraphicsWindow::addDrawFunction(void *object, void(*functionPointer)(void *, GraphicsWindow *, int, int))
{
	objects_.push_back(object);
	drawFunctionPointers_.push_back(functionPointer);
}

void GraphicsWindow::clearDrawFunctions()
{
	objects_.clear();
	drawFunctionPointers_.clear();
}

void GraphicsWindow::drawWindowSprite(WindowSprite sprite, Rect *dest, Rect *source, bool colorKey, int alpha)
{
	
	SpriteInstance newSprite;

	newSprite.handle = sprite.handle;

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

		newSprite.source.width = sprite.width;
		newSprite.source.height = sprite.height;
	}

	spriteInstances_.push_back(newSprite);
}

bool GraphicsWindow::getNewClip(int x, int y, int width, int height, Rect oldClip, Rect *newClip)
{
	int scrollWidth = getScrollWidth(x, y, width, height);
	int scrollHeight = getScrollHeight(x, y, width, height);

	newClip->x = x;
	newClip->y = y;
	newClip->width = width;
	newClip->height = height;

	bool hScroll = false;
	bool vScroll = false;

	for (int index = 0; index < 2; index++)
	{
		int w = width;
		if (vScroll) w -= scrollControlSize_;

		if (hScrollControlsActive_ && scrollWidth > w) hScroll = true;

		int h = height;
		if (hScroll) h -= scrollControlSize_;

		if (vScrollControlsActive_ && scrollHeight > h) vScroll = true;
	}


	if (vScroll) newClip->width -= (scrollControlSize_);
	if (hScroll) newClip->height -= (scrollControlSize_);

	if (newClip->width < 0) newClip->width = 0;
	if (newClip->height < 0) newClip->height = 0;

	return clipArea(newClip, &oldClip);
}


void GraphicsWindow::draw(int x, int y, int width, int height, Rect oldClip)
{
	if (disabled_) return;
	spriteInstances_.clear();

	for (int index = 0; index < drawFunctionPointers_.size(); index++)
	{
		(*(drawFunctionPointers_[index]))(objects_[index], this, width, height);
	}


	//Widget::draw(x,y,width,height,oldClip);
	Wrapper *wrapper = windowSystem_->getWrapper();

	Rect dest;
	dest.x = x;
	dest.y = y;
	dest.width = width;
	dest.height = height;

	Rect clip;

	clip.x = x;
	clip.y = y;
	clip.width = width;
	clip.height = height;

	if (clipArea(&clip, &oldClip))
	{	
		for(unsigned int index = 0; index < spriteInstances_.size(); index++)
		{
			Rect dest;

			dest.x = x + spriteInstances_[index].dest.x -scrollX_;
			dest.y = y + spriteInstances_[index].dest.y -scrollY_;
			dest.width = spriteInstances_[index].dest.width;
			dest.height = spriteInstances_[index].dest.height;

			Rect source = spriteInstances_[index].source;

			Sprite *sprite = windowSystem_->getSprite(spriteInstances_[index].handle);

			wrapper->drawSprite(sprite, &dest, &source, &clip, spriteInstances_[index].colorkey, spriteInstances_[index].alpha);
		}
	}

	spriteInstances_.clear();

	drawScrollControls(x, y, width, height, oldClip);

}

int GraphicsWindow::getScrollWidth(int x, int y, int width, int height)
{
	return scrollWidth_;
}

int GraphicsWindow::getScrollHeight(int x, int y, int width, int height)
{
	return scrollHeight_;
}


void Button::setSpriteRect(Rect *rect)
{
	spriteRect_.x = rect->x;
	spriteRect_.y = rect->y;
	spriteRect_.width = rect->width;
	spriteRect_.height = rect->height;
}

void Button::draw(int x, int y, int width, int height, Rect oldClip)
{
	

	Wrapper *wrapper = windowSystem_->getWrapper();

	Rect dest;
	dest.x = x+1;
	dest.y = y+1;
	dest.width = width -2;
	dest.height = height -2;

	Rect newClip;

	newClip.x = x;
	newClip.y = y;
	newClip.width = width;
	newClip.height = height;

	if(!clipArea(&newClip, &oldClip)) return;
	
	if(sprite_)
	{
		wrapper->drawSprite(windowSystem_->getSprite(sprite_->handle), &dest, &spriteRect_, &newClip, 0, 255);
	}
	else
	{
		if (disabled_) drawBox(x, y, width, height, oldClip, 0, 0, 0, 224);
		else drawBox(x, y, width, height, oldClip, r_, g_, b_, 224);
	}

	Printer *printer = windowSystem_->getPrinter();
	
	int text_x = dest.x + width/2 -printer->textWidth((char *)name_.c_str())/2;
	int text_y = dest.y + height/2 -printer->maxHeight()/2;

	printer->print((char *)name_.c_str(), text_x, text_y, &newClip);

	if (disabled_) drawBorders(x, y, width, height, oldClip, 32, 32, 32);
	else drawBorders(x, y, width, height, oldClip, 255, 255, 255);
}

void TextField::click(int localX, int localY, int x, int y, int width, int height)
{

	std::stringstream stream;

	position_ = 0;

	Printer *printer = windowSystem_->getPrinter();
	position_ = printer->textIndex((char*)text_.c_str(), localX - 10);

	if (position_ < 0) position_ = 0;
}

void TextField::deactivate()
{
	if (active_ == true)
	{
		active_ = false;
		textEnter_.call(text_, this);
	}
}

void TextField::logic(int x, int y, int width, int height, Rect oldClip, int globalMouseX, int globalMouseY, Input *input)
{
	if (!active_) return;

	bool change = false;
	for (int key = KEY_a; key <= KEY_z; key++)
	{
		if (input->KEY_PRESSED[key])
		{
			change = true;
			char character = key - KEY_a + 'a';

			text_.insert(text_.begin() + position_, character);
			position_++;
		}
	}

	if (input->KEY_PRESSED[KEY_SPACE])
	{
		change = true;
		text_.insert(text_.begin() + position_, ' ');
		position_++;
	}

	if (input->KEY_PRESSED[KEY_BACKSPACE] && position_ > 0)
	{
		change = true;
		text_.erase(text_.begin() + position_ - 1);
		position_--;
	}

	if (input->KEY_PRESSED[KEY_RETURN])
	{
		textEnter_.call(text_, this);
		active_ = false;
	}

	
	if(change) textChange_.call(text_, this);
}

void TextField::draw(int x, int y, int width, int height, Rect oldClip)
{

	Rect newClip;

	newClip.x = x;
	newClip.y = y;
	newClip.width = width;
	newClip.height = height;

	if (!clipArea(&newClip, &oldClip)) return;

	drawBox(x, y, width, height, oldClip, 24, 24, 24, 255);

	Printer *printer = windowSystem_->getPrinter();
	printer->print((char*)text_.c_str(), x + 10, y + (height - printer->maxHeight()) / 2, &newClip);

	

	Rect rect;
	rect.x = x + 10 - 1 + printer->textX((char*)text_.c_str(), position_);
	rect.y = y + (height - printer->maxHeight()) / 2;
	rect.width = 1;
	rect.height = 15;

	Wrapper *wrapper = windowSystem_->getWrapper();

	
	if (active_) wrapper->drawRectangle(&rect, &newClip, 255, 0, 0, 128);
	
}

void List::click(int localX, int localY, int x, int y, int width, int height)
{
	for (unsigned int index = 0; index < texts_.size(); index++)
	{
		if (mouseOnRectangle(0, index*30-scrollY_, width, 30, localX,localY))
		{
			row_ = index;
			chooseEvent_.call(index, this);
		}
	}
}

void List::draw(int x, int y, int width, int height, Rect oldClip)
{
	Window::draw(x, y, width, height, oldClip);

	Rect newClip;

	if (!getNewClip(x, y, width, height, oldClip, &newClip)) return;

	Printer *printer = windowSystem_->getPrinter();
	for (unsigned int index = 0; index < texts_.size(); index++)
	{
		if (index == row_)
		{
			

			drawBox(x, y - scrollY_ + 30 * index, width, 30, newClip, 255, 0, 0, 224);
		}

		printer->print((char*)texts_[index].c_str(), x + 5 - scrollX_, y + index * 30 + 5 - scrollY_, &newClip);
	}
}

void List::add(std::string text)
{
	texts_.push_back(text);

	if (row_ == -1) row_ = 0;
}

void List::clear()
{
	texts_.clear();
	row_ = -1;
}


int List::getScrollWidth(int x, int y, int width, int height)
{
	int result = 0;

	Printer *printer = windowSystem_->getPrinter();

	for (int index = 0; index < texts_.size(); index++)
	{
		int w = printer->textWidth((char*)texts_[index].c_str()) +10;

		if (result < w) result = w;
	}

	return result;
}

int List::getScrollHeight(int x, int y, int width, int height)
{
	return texts_.size() * 30;
}

void List::setRow(int row)
{
	row_ = row;

	if (row_ < 0) row_ = 0;
	if (row_ >= (int)texts_.size()) row_ = (int)texts_.size() - 1;
}

void List::updateSrollY()
{
	scrollY_ = row_ * 30;
}


void DropList::click(int localX, int localY, int x, int y, int width, int height)
{
	for (unsigned int index = 0; index < texts_.size(); index++)
	{
		if (mouseOnRectangle(0, index * 30 - scrollY_, width, 30, localX, localY))
		{
			owner_->setRow(index);
			owner_->chooseEvent_.call(index, owner_);

			windowSystem_->closePopup(this);
		}
	}
}


void RadioButtons::add(std::string text)
{
	texts_.push_back(text);

	if (buttonNumber_ == -1) buttonNumber_ = 0;
}

void RadioButtons::clear()
{
	texts_.clear();
	buttonNumber_ = -1;
}

void RadioButtons::draw(int x, int y, int width, int height, Rect oldClip)
{
	Rect newClip;

	newClip.x = x;
	newClip.y = y;
	newClip.width = width;
	newClip.height = height;

	if (!clipArea(&newClip, &oldClip)) return;


	Printer *printer = windowSystem_->getPrinter();

	int position = 0;

	for (unsigned int index = 0; index < texts_.size(); index++)
	{
		if (index == buttonNumber_)
		{
			drawBox(x + position, y + (height -15)/2, 15, 15, newClip, 255, 0, 0, 224);
		}
		
		drawBorders(x + position, y + (height - 15) / 2, 15, 15, newClip, 255, 255, 255);

		position += 20;

		printer->print((char*)texts_[index].c_str(), x + position, y + (height - printer->maxHeight()) / 2, &newClip);
		position += printer->textWidth((char*)texts_[index].c_str()) + 5;
	}
}

void RadioButtons::click(int localX, int localY, int x, int y, int width, int height)
{
	Printer *printer = windowSystem_->getPrinter();

	int position = 0;

	for (unsigned int index = 0; index < texts_.size(); index++)
	{
		if (mouseOnRectangle(position, 0 + (height - 15) / 2, 15, 15, localX, localY))
		{
			buttonNumber_ = index;
			chooseEvent_.call(buttonNumber_, this);
		}
		
		position += 20;
				
		position += printer->textWidth((char*)texts_[index].c_str()) + 5;
	}
}


void DropMenu::draw(int x, int y, int width, int height, Rect oldClip)
{
	Widget::draw(x, y, width, height, oldClip);

	Rect newClip;

	newClip.x = x;
	newClip.y = y;
	newClip.width = width -15;
	newClip.height = height;

	if (!clipArea(&newClip, &oldClip)) return;
	
	Printer *printer = windowSystem_->getPrinter();

	if (row_ >= 0 && row_ < texts_.size())
	{
		printer->print((char*)texts_[row_].c_str(), x + 5, y + (height - printer->maxHeight()) / 2, &newClip);
	}

	
	drawBox(x + width - 15, y, 15, height, oldClip, 255, 128, 0, 192);
	drawBorders(x + width - 15, y, 15, height, oldClip, 255, 255, 255);


}

void DropMenu::click(int localX, int localY, int x, int y, int width, int height)
{

	if (windowSystem_->popupOpened(&list_))
	{
		windowSystem_->closePopup(&list_);
	}
	else
	{
		list_.clear();

		for (int index = 0; index < texts_.size(); index++)
		{
			list_.add(texts_[index]);
		}

		list_.setX(x);
		list_.setY(y + height);
		list_.setWidth(width + 15);
		list_.setHeight(200);
		list_.setVScrollControlsActive(true);

		list_.setRow(row_);
		list_.updateSrollY();


		windowSystem_->clearPopups();
		windowSystem_->addPopup(&list_);
	}

	

}

void DropMenu::add(std::string text)
{
	texts_.push_back(text);

	if (row_ == -1) row_ = 0;
}

void DropMenu::clear()
{
	texts_.clear();
	row_ = -1;
}

void DropMenu::setRow(int row)
{
	row_ = row;

	if (row_ < 0) row_ = 0;
	if (row_ >= (int)texts_.size()) row_ = (int)texts_.size() - 1;
}
