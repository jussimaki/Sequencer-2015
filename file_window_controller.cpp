#include "file_window_controller.h"

void FileWindowController::addCallBack(void *object, void(*functionPointer)(void *, FileWindowController*))
{
	functionPointers_.push_back(functionPointer);
	objects_.push_back(object);

}

void FileWindowController::clearCallBacks()
{
	functionPointers_.clear();
	objects_.clear();
}

void FileWindowController::initData()
{
	getCurrentDirectory(&directory_);

	fileObjects_.clear();
	directoryName_ = getFileNameString(&directory_);

	getFileObjects(&directory_, &fileObjects_);

	view_->update();
}


//responds to 
void FileWindowController::fileWindowCallback(Widget *widget, Event *event)
{
	fileObjects_.clear();
	getFileObjects(&directory_, &fileObjects_);


	if (event == &view_->fileOpenButton_.onClick_) // clicked open button
	{
		int row = view_->fileList_.getRow();
		if (row >= 0)
		{
			// if directory or drive, advance in file system
			if (fileObjects_[row].directory || fileObjects_[row].drive)
			{
				advance(&directory_, &fileObjects_[row]);
				fileObjects_.clear();

				directoryName_ = getFileNameString(&directory_);
				getFileObjects(&directory_, &fileObjects_);
				view_->update();
			}
			else // if file, set filename, close window and call callbacks
			{
				copyDirectoryData(&directory_, &fileName_);
				advance(&fileName_, &fileObjects_[row]);

				finalFileName_ = getFileNameString(&fileName_);

				view_->close();

				//call callback functions

				for (unsigned int index = 0; index < functionPointers_.size(); index++)
				{
					if (functionPointers_[index] && objects_[index])
						(*(functionPointers_[index]))(objects_[index], this);
				}
			}
		}
	}
	else if (event == &view_->fileBackButton_.onClick_)
	{
		goBack(&directory_);
		fileObjects_.clear();

		directoryName_ = getFileNameString(&directory_);
		getFileObjects(&directory_, &fileObjects_);
		view_->update();
	}
	else if (event == &view_->fileCancelButton_.onClick_)
	{
		view_->close();
	}
	
}


void FileWindowController::fileWindowCallback_(void *object, Widget *widget, Event *event)
{
	FileWindowController *p = (FileWindowController*)object;
	p->fileWindowCallback(widget, event);
}