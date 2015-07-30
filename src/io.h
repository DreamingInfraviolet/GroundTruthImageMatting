#pragma once
#include <string>
#include <sstream>

/**
* A series of function to help with basic input and output.
* */

/** Prints the given error message */
void Error(const std::string& msg);

/** Prints the given warning message. */
void Warning(const std::string& msg);

/** Prints the given information message. */
void Inform(const std::string& msg);

/** Converts the input to a string. */
template <class T>
std::string ToString(const T t)
{
    std::ostringstream ss;
    ss << t;
    return ss.str();
}

/** Converts the input to a hexadecimal string. */
template <class T>
std::string ToHexString(const T t)
{
    std::ostringstream ss;
    ss << std::hex << t;
    return ss.str();
}

/** Appends a filename to a path, taking care of / or \\ path endings (or lack thereof). */
std::string appendNameToPath(const std::string& name, const std::string& path);

/** Converts an Eds Error enum to the appropriate enum name. */
std::string EdsCodeToString(long code);

//The Eds SDK error system is frightening, so the following macros help with handling them conveniently.

//returns the ret parameter if the function fails, printing the given error message along with the error code.
//Returns the error code if ret==err
#define CHECK_EDS_ERROR(func, message, ret) {                                                               \
                                   int err = func;                                                          \
                                   if(err!=EDS_ERR_OK)                                                      \
                                                {                                                           \
                                                    Error(std::string("Camera error in ") +                 \
                                                    ToString(__FILE__) + " on line " +                      \
                                                    ToString(__LINE__) + ": " + message +                   \
                                                    " | " + EdsCodeToString(err));                          \
                                                    return ret;                                             \
                                                }                                                           \
                                            }


//returns the ret parameter if the function fails, printing the given error message along with the error code.
//Returns the error code if ret==err
//Performs act before returning in {} braces, allowing for cleanup.
#define CHECK_EDS_ERROR_ACT(func, message, ret, act){                                                       \
                                   int err = func;                                                          \
                                   if(err!=EDS_ERR_OK)                                                      \
                                   {                                                                        \
                                                    Error(std::string("Camera error in ") +                 \
                                                    ToString(__FILE__) + " on line " +                      \
                                                    ToString(__LINE__) + ": " + message +                   \
                                                    " | " + EdsCodeToString(err));                          \
													{act;}                                                  \
                                                    return ret;                                             \
								    }                                                                       \
                           }



//Warns if an error has occured without returning.
#define WARN_EDS_ERROR(func, message)   {                                                               \
                                           int err = func;                                              \
                                           if(err!=EDS_ERR_OK)                                          \
                                                Warning(std::string("Camera error in ") +               \
                                                ToString(__FILE__) + " on line " +                      \
                                                ToString(__LINE__) + ": " + message +                   \
                                                " | " + EdsCodeToString(err));                          \
                                        }