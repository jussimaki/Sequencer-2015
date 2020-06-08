#include "printer.h"

int Printer::textX(char *text, int index)
{
	int x = 0;
	
	for(int i = 0; i < index; i++)
	{
		char character = text[i];
		if(character == '\0') break;
		else
		{
			if(character >= starting_symbol_ && character <= last_symbol_)
			{
				x += alphabet_table_[(character -starting_symbol_)*4 + 2];
			}
			
		}
	}
	return x;
}

int Printer::textIndex(char *text, int x)
{
	int index = 0;

	int x_sum = 0;
	
	while(1)
	{
		if(text[index] == '\0') break;

		x_sum += alphabet_table_[(text[index] -starting_symbol_)*4 + 2];

		if(x > x_sum) index++;
		else break;
	}
	
	return index;
}

int Printer::textWidth(char *text)
{
	if(!text) return 0;
	int index = 0;
	int width = 0;
	
	while(1)
	{
		if(text[index] == '\0') break;
		else
		{
			if(text[index] >= starting_symbol_ && text[index] <= last_symbol_)
			{
				width += alphabet_table_[(text[index] -starting_symbol_)*4 + 2];
			}
			
			index++;
		}
	}
	
	return width;
}

int Printer::textHeight(char *text)
{
	if(!text) return 0;
	int index = 0;
	int height = 0;
	
	while(1)
	{
		if(text[index] == '\0') break;
		else
		{
			if(text[index] >= starting_symbol_ && text[index] <= last_symbol_)
			{
				if(height < alphabet_table_[(text[index] -starting_symbol_)*4 + 3])
					height = alphabet_table_[(text[index] -starting_symbol_)*4 + 3];
			}
			
			index++;
		}
	}
	
	return height;
}

int Printer::maxHeight()
{
	int height = 0;

	for (int index = 0; index <= last_symbol_ - starting_symbol_; index++)
	{
		if (height < alphabet_table_[(index) * 4 + 3])
			height = alphabet_table_[(index) * 4 + 3];
	}

	return height;
}

void Printer::print(char *text, int x, int y, Rect *clip)
{
	int orig_x = x;
	
	if(!text) return;
	int index = 0;

	while(1)
	{
		if(text[index] == '\0') break;
		else
		{
			if(text[index] == '\n')
			{
				x = orig_x;
				y += 20;
			}
			else if(text[index] >= starting_symbol_ && text[index] <= last_symbol_)
			{
				Rect source;
				
	
				source.x = alphabet_table_[(text[index] -starting_symbol_)*4];
				source.y = alphabet_table_[(text[index] -starting_symbol_)*4 + 1];
				source.width = alphabet_table_[(text[index] -starting_symbol_)*4 + 2];
				source.height = alphabet_table_[(text[index] -starting_symbol_)*4 + 3];
	
				Rect dest;
				dest.x = x;
				dest.y = y;
				dest.width = source.width;
				dest.height = source.height;
				
				wrapper_->drawSprite(alphabet_, &dest, &source, clip, true, 255);
				x += alphabet_table_[(text[index] -starting_symbol_)*4 + 2];
			}

			index++;
		}
	}
}
