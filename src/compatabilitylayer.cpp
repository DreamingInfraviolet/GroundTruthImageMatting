#include "compatabilitylayer.h"
#include "io.h"

#ifdef _WIN32
#include <Windows.h>




//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
//Source: stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror#answer-17387176
std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	if (size == 0)
		return "Error unknown";

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}



DiskFreeSpace* FindDiskFreeSpace(const char* path)
{
	DiskFreeSpace* out = new DiskFreeSpace();

	out->pathName = path;
	BOOL status = GetDiskFreeSpace(path, &out->sectorsPerCluster, &out->bytesPerSector, &out->numberOfFreeClusters, &out->totalNumberOfClusters);

	//If failed
	if(status==0)
	{
		Error(std::string("Could not get free disk space with path \"")+ path + "\": " + GetLastErrorAsString());
		delete out;
		return nullptr;
	}
	else //if succeeded
	{
		return out;
	}
}


#else
#error MacOS not implemented.
#endif