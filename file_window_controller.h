#ifndef FILE_WINDOW_CONTROLLER_H
#define FILE_WINDOW_CONTROLLER_H

#include "file_functions.h"

#include "file_window_view.h"

class FileWindowView;
class FileWindowController
{

private:
	FileWindowView *view_;
	
	DirectoryData directory_, fileName_;

	std::vector <FileObject> fileObjects_;
	std::string finalFileName_;

	std::string directoryName_;
	
	// callBack pointers for file opening
	std::vector <void(*)(void *, FileWindowController*)> functionPointers_;
	std::vector <void *> objects_;

public:

	void setView(FileWindowView *view) { view_ = view; }

	void static fileWindowCallback_(void *object, Widget *widget, Event *event);
	void fileWindowCallback(Widget *widget, Event *event);

	void addCallBack(void *object, void(*functionPointer)(void *, FileWindowController*));
	void clearCallBacks();

	

	void initData();

	unsigned int getFileObjectAmount() { return fileObjects_.size(); }
	FileObject getFileObject(unsigned int number) { return fileObjects_[number]; }
	std::string getDirectoryName() { return directoryName_; }
	std::string getFileName() { return finalFileName_; }

};

#endif