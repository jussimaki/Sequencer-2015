#ifndef PRINTER_H
#define PRINTER_H

#include "wrapper.h"

class Printer
{
private:
	int *alphabet_table_;
	int starting_symbol_, last_symbol_;
	Sprite *alphabet_;
	Wrapper *wrapper_;

public:

	Printer(int *alphabet_table, int starting_symbol, int last_symbol, Wrapper *wrapper, Sprite *alphabet): alphabet_table_(alphabet_table),
		starting_symbol_(starting_symbol), last_symbol_(last_symbol), wrapper_(wrapper), alphabet_(alphabet){};

	void print(char *text, int x, int y, Rect *clip);
	
	int maxHeight();
	int textWidth(char *text);
	int textHeight(char *text);
	int textX(char *text, int index);
	int textIndex(char *text, int x);


};


#endif