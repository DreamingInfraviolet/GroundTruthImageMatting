#include "io.h"
#include <iostream>


void Error(const std::string& msg)
{
	std::cerr<<"Error: " <<msg<<"\n";
}

void Warning(const std::string& msg)
{
	std::cout<<"Warning: " <<msg<<"\n";
}

void Inform(const std::string& msg)
{
	std::cout << ":- " << msg << "\n";
}

std::string appendNameToPath(const std::string& name, const std::string& path)
{
	if (path.size() == 0)
		return name;

	//If separator already exists, just add
	if (path.back() == '\\' || path.back() == '/')
		return path + name;
	else
		return path + "/" + name;
}