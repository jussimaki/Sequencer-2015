#ifndef FILE_WINDOW_H
#define FILE_WINDOW_H

#include "window.h"
#include "windows.h"

#include "file_window_controller.h"


class FileWindowController;

class FileWindowView
{
	friend class FileWindowController;

private:
	Windows *system_;
	FileWindowController *fileWindowController_;


	Window fileWindow_;
	
	Label directoryLabel_;
	List fileList_;
	Button fileOpenButton_, fileBackButton_, fileCancelButton_;

	void update();
		

public:
	FileWindowView(Windows *system, FileWindowController* fileWindowController);
	void createLayout();
	void configureLayout();
	
	void open(int x, int y, int width, int height);
	void close();



};


#endif