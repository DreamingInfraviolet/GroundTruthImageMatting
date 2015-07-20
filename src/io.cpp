#include "io.h"
#include <iostream>


void Error(const std::string& msg)
{
	std::cout<<"Error: " <<msg<<"\n";
}

void Warning(const std::string& msg)
{
	std::cout<<"Warning: " <<msg<<"\n";
}