#ifndef INPUT_FUNCTIONALITY_H
#define INPUT_FUNCTIONALITY_H

#include "wrapper.h"

struct Input
{
	char KEY_PRESSED[KEY_LAST];
	char KEY_RELEASED[KEY_LAST];
	char KEY_DOWN[KEY_LAST];

	char MOUSE_BUTTON_CLICKED[3];
	char MOUSE_BUTTON_DOWN[3];
	char MOUSE_BUTTON_RELEASED[3];

	int mouse_move_x, mouse_move_y;
};

void clearInput(Input *input);
void inputFunctionality(Wrapper *wrapper, Input *input);

#endif