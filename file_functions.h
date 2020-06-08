#ifndef FILE_FUNCTIONS_H
#define FILE_FUNCTIONS_H

#include <vector>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#error NOT_WINDOWS
#endif


#define MAXFILENAMELENGTH 10000

struct DirectoryData
{
	char name[MAXFILENAMELENGTH-2];
};

struct FileObject
{
	std::string name;
	bool directory;
	bool drive;
};



void goBack(DirectoryData *data);
bool advance(DirectoryData *directory, FileObject *object);
bool getCurrentDirectory(DirectoryData *data);
bool getFileObjects(DirectoryData *directory, std::vector <FileObject> *objects);
void copyDirectoryData(DirectoryData *source, DirectoryData *destination);
std::string getFileNameString(DirectoryData *fileName);


#endif