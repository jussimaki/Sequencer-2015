#include "input_functionality.h"


void clearInput(Input *input)
{
	input->mouse_move_x = 0;
	input->mouse_move_y = 0;

	for(int index = 0; index < KEY_LAST; index++)
	{
		input->KEY_DOWN[index] = 0;
		input->KEY_PRESSED[index] = 0;
		input->KEY_RELEASED[index] = 0;
	}

	for(int index = 0; index < 3; index++)
	{
		input->MOUSE_BUTTON_DOWN[index] = 0;
		input->MOUSE_BUTTON_CLICKED[index] = 0;
		input->MOUSE_BUTTON_RELEASED[index] = 0;
	}
}

void inputFunctionality(Wrapper *wrapper, Input *input)
{
	InputData *inputData = wrapper->getInputData();

	for(int index = 0; index < KEY_LAST; index++)
	{
		input->KEY_PRESSED[index] = 0;
		input->KEY_RELEASED[index] = 0;
		
		if(inputData->KEY_DOWN[index])
		{
			if(!input->KEY_DOWN[index])
			{
				input->KEY_PRESSED[index] = 1;
			}
		}
		else
		{
			if(input->KEY_DOWN[index])
			{
				input->KEY_RELEASED[index] = 1;
			}
		}

		input->KEY_DOWN[index] = inputData->KEY_DOWN[index];
	}

	for(int index = 0; index < 3; index++)
	{
		input->MOUSE_BUTTON_CLICKED[index] = 0;
		input->MOUSE_BUTTON_RELEASED[index] = 0;
		
		if(inputData->MOUSE_BUTTON_DOWN[index])
		{
			if(!input->MOUSE_BUTTON_DOWN[index])
			{
				input->MOUSE_BUTTON_CLICKED[index] = 1;
			}
		}
		else
		{
			if(input->MOUSE_BUTTON_DOWN[index])
			{
				input->MOUSE_BUTTON_RELEASED[index] = 1;
			}
		}

		input->MOUSE_BUTTON_DOWN[index] = inputData->MOUSE_BUTTON_DOWN[index];
	}

	input->mouse_move_x = inputData->mouse_move_x;
	input->mouse_move_y = inputData->mouse_move_y;
}