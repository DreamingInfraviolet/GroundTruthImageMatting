#pragma once
#include <string>

/**
* These series of functions and structs serve to encapsulate important operating system calls to allow compilation
* seamlessly between Windows and MacOS.
* */


/**
* Stores basic information about a storage device used to host a particular path.
* */
struct DiskFreeSpace
{
	std::string pathName;
	unsigned long sectorsPerCluster;
	unsigned long bytesPerSector;
	unsigned long numberOfFreeClusters;
	unsigned  long totalNumberOfClusters;
};


/**
* Finds information about the given disk as described by the DiskFreeSpace struct.
* @param path The path which to analyse.
* @return A DiskFreeSpace struct upon success, or null upon failure.
* */
DiskFreeSpace* FindDiskFreeSpace(const char* path);