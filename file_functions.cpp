#include "file_functions.h"


#ifdef _WIN32

bool getCurrentDirectory(DirectoryData *data)
{	
	
	GetCurrentDirectoryA(MAXFILENAMELENGTH-3, data->name);

	int index = 0;

	while (data->name[index] != '\0') index++;

	data->name[index] = '\\';
	data->name[index+1] = '\0';

	return true;
}

bool getFileObjects(DirectoryData *directory, std::vector <FileObject> *objects)
{
	if (directory->name[0] == '\0') // directory name empty, show drives
	{
		char drives[MAXFILENAMELENGTH];

		GetLogicalDriveStringsA(MAXFILENAMELENGTH, (char*)&drives);

		std::string string;

		int index = 0;

		while (drives[index] != '\0')
		{
			FileObject drive;
			drive.directory = false;
			drive.drive = true;
			drive.name;

			while (drives[index] != '\0')
			{
				drive.name += drives[index];
				index++;
			}

			index++;
			objects->push_back(drive);
		}
	}
	else
	{
		char name[MAXFILENAMELENGTH];

		int index = 0;
		while (directory->name[index] != '\0')
		{
			name[index] = directory->name[index];
			index++;
		}

		name[index] = '\\';
		name[index + 1] = '*';
		name[index + 2] = '\0';


		WIN32_FIND_DATAA fd;
		HANDLE hFind;

		hFind = FindFirstFileA(name, &fd);

		if (hFind == INVALID_HANDLE_VALUE) return false;

		FileObject fileObject;

		fileObject.drive = false;
		
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) fileObject.directory = true;
		else fileObject.directory = false;

		index = 0;

		while (fd.cFileName[index] != '\0')
		{
			fileObject.name += fd.cFileName[index];
			index++;
		} 

		if (fileObject.name != ".." && fileObject.name != ".")
		objects->push_back(fileObject);


		while (FindNextFileA(hFind, &fd))
		{
			FileObject fileObject;

			fileObject.drive = false;
			
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) fileObject.directory = true;
			else fileObject.directory = false;

			index = 0;

			while (fd.cFileName[index] != '\0')
			{
				fileObject.name += fd.cFileName[index];
				index++;
			}

			if (fileObject.name != ".." && fileObject.name != ".")
			objects->push_back(fileObject);
		}

	}
	return true;
}


void goBack(DirectoryData *data)
{
	int lastIndex = 0;
	int secondLastIndex = 0;
	bool found = false;
	bool secondLastFound = false;

	int index = 0;
	while (data->name[index] != '\0')
	{
		if (data->name[index] == '\\')
		{
			if (found)
			{
				secondLastIndex = lastIndex;
				secondLastFound = true;
			}

			lastIndex = index;
			found = true;
		}

		index++;
	}

	if (secondLastFound) data->name[secondLastIndex +1] = '\0';
	else data->name[0] = '\0';
}

bool advance(DirectoryData *directory, FileObject *object)
{
	int index = 0;
	while (directory->name[index] != '\0')
	{
		index++;
	}

	if (index + object->name.size() + 1 >= MAXFILENAMELENGTH - 2)
		return false;



	for (int index2 = 0; index2 < object->name.size(); index2++)
	{
		directory->name[index] = object->name[index2];
		index++;
	}

	if (object->directory) // drive name already includes ":\\" and file doesn't need any of that
	{
		directory->name[index] = '\\';
		index++;
	}

	directory->name[index] = '\0';

	return true;
}

void copyDirectoryData(DirectoryData *source, DirectoryData *destination)
{
	for (int index = 0; index < MAXFILENAMELENGTH - 2; index++)
		destination->name[index] = source->name[index];
}



std::string getFileNameString(DirectoryData *fileName)
{
	std::string string;

	int index = 0;
	while (fileName->name[index] != '\0')
	{
		string += fileName->name[index];

		index++;
	}


	return string;
}

#else
#error NOT_WINDOWS
#endif