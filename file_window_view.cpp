#include "file_window_view.h"


FileWindowView::FileWindowView(Windows *system, FileWindowController* fileWindowController) : system_(system), fileWindowController_(fileWindowController)
{
	fileWindowController->setView(this);

	createLayout();
	configureLayout();
}

void FileWindowView::createLayout()
{

	fileWindow_.addRegion("DirectoryLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		600, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	fileWindow_.addRegion("FileList",
		0, ATTACH_LEFT, "DirectoryLabel",
		10, ATTACH_BOTTOM, "DirectoryLabel",
		200, ATTACH_NONE, "",
		300, ATTACH_NONE, "");

	fileWindow_.addRegion("FileOpenButton",
		0, ATTACH_LEFT, "FileList",
		10, ATTACH_BOTTOM, "FileList",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	fileWindow_.addRegion("FileBackButton",
		10, ATTACH_RIGHT, "FileOpenButton",
		0, ATTACH_TOP, "FileOpenButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	fileWindow_.addRegion("FileCancelButton",
		10, ATTACH_RIGHT, "FileBackButton",
		0, ATTACH_TOP, "FileBackButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	
	
}

void FileWindowView::configureLayout()
{
	fileWindow_.setWidget("DirectoryLabel", &directoryLabel_);
	fileWindow_.setWidget("FileList", &fileList_);
	fileWindow_.setWidget("FileOpenButton", &fileOpenButton_);
	fileWindow_.setWidget("FileBackButton", &fileBackButton_);
	fileWindow_.setWidget("FileCancelButton", &fileCancelButton_);

	fileList_.setVScrollControlsActive(true);


	fileOpenButton_.setName("Open");
	fileOpenButton_.onClick_.add(fileWindowController_, &(FileWindowController::fileWindowCallback_));

	fileBackButton_.setName("Back");
	fileBackButton_.onClick_.add(fileWindowController_, &(FileWindowController::fileWindowCallback_));

	fileCancelButton_.setName("Cancel");
	fileCancelButton_.onClick_.add(fileWindowController_, &(FileWindowController::fileWindowCallback_));
}



void FileWindowView::update()
{
	fileList_.clear();
	
	for (unsigned int index = 0; index < fileWindowController_->getFileObjectAmount(); index++)
	{
		FileObject object = fileWindowController_->getFileObject(index);

		std::string string;

		string = object.name;
		if (object.directory) string += " (DIR)";
		if (object.drive) string += " (DRIVE)";

		fileList_.add(string);
	}
	


	directoryLabel_.text_ = fileWindowController_->getDirectoryName();
}

void FileWindowView::open(int x, int y, int width, int height)
{
	fileWindow_.setX(x);
	fileWindow_.setY(y);
	fileWindow_.setWidth(width);
	fileWindow_.setHeight(height);

	system_->addWindow(&fileWindow_);
}

void FileWindowView::close()
{
	system_->closeWindow(&fileWindow_);
}

