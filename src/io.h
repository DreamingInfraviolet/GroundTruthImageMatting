#pragma once
#include <string>
#include <sstream>

void Error(const std::string& msg);
void Warning(const std::string& msg);
void Inform(const std::string& msg);

template <class T>
std::string ToString(const T t)
{
	std::ostringstream ss;
	ss << t;
	return ss.str();
}

template <class T>
std::string ToHexString(const T t)
{
	std::ostringstream ss;
	ss << std::hex << t;
	return ss.str();
}

std::string appendNameToPath(const std::string& name, const std::string& path);

std::string EdsCodeToString(long code);


//returns the ret parameter if the function fails, printing the given error message along with the error code.
//Returns the error code if ret==err
#define CHECK_EDS_ERROR(func, message, ret) {													\
								   int err = func; 												\
								   if(err!=EDS_ERR_OK) 											\
								   	{															\
								   		Error(std::string("Camera error in ") + 				\
								   		ToString(__FILE__) + " on line " +						\
								   		ToString(__LINE__) + ": " + message + 					\
								   		" | " + EdsCodeToString(err));  						\
								   		return ret;												\
								   	}															\
							   }


//Warns if an error has occured without returning.
#define WARN_EDS_ERROR(func, message)   {														\
								   int err = func;	 											\
								   if(err!=EDS_ERR_OK) 											\
								   		Warning(std::string("Camera error in ") + 				\
								   		ToString(__FILE__) + " on line " +						\
								   		ToString(__LINE__) + ": " + message + 					\
								   		" | " + EdsCodeToString(err));  						\
							    }
