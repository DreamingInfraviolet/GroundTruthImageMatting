#include "image.h"
#include <assert.h>
#include <fstream>
#include "EDSDK.h"
#include "io.h"

//returns the ret parameter if the function fails, printing the given error message along with the error code.
//Returns the error code if ret==err
#define CHECK_EDS_ERROR(func, message, ret) {													\
								   int err = func; 												\
								   if(err!=EDS_ERR_OK) 											\
								   	{															\
								   		Error(std::string("Camera error in ") + 				\
								   		ToString(__FILE__) + " on line " +						\
								   		ToString(__LINE__) + ": " + message + 					\
								   		" | Error Code " + ToString(err));  					\
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
								   		" | Error Code " + ToString(err));  					\
							    }



size_t Image::getDataLength()
{
	return mData.size();
}

void Image::clear()
{
	//Frees the data
	mData.swap(decltype(mData)());
}

bool Image::failed()
{
	return mFailed;
}

void Image::fail()
{
	mFailed = true;
}

int Image::height()
{
	return mHeight;
}

int Image::width()
{
	return mWidth;
}

///Left off here

std::vector<unsigned char> ImageRaw::getRawRGB()
{
	return std::vector<unsigned char>();


	EdsImageInfo imageInfo;
	CHECK_EDS_ERROR(EdsGetImageInfo(mImageRef, kEdsImageSrc_RAWFullView, &imageInfo), "Could not retrieve image info", {});

	//Create stream
	EdsStreamRef rgbStream = nullptr;
	CHECK_EDS_ERROR(EdsCreateMemoryStream(3 * imageInfo.width*imageInfo.height, &rgbStream),
		"Failed to create memory stream", {});

	//Get image in rgb format into memory
	EdsSize size;
	size.height = imageInfo.height;
	size.width = imageInfo.width;
	CHECK_EDS_ERROR(EdsGetImage(mImageRef, kEdsImageSrc_RAWFullView, kEdsTargetImageType_RGB,
		imageInfo.effectiveRect, size, rgbStream), "Could not retrieve the image", {});

	void* imageData;
	CHECK_EDS_ERROR(EdsGetPointer(rgbStream, &imageData), "Could not retrieve the image pointer", {});

	/*
	std::string bmpPath = appendNameToPath(camera->mLastSaveName + ".bmp", camera->mLastSaveDirectory);

	if (SOIL_save_image(bmpPath.c_str(), SOIL_SAVE_TYPE_BMP,
		size.width, size.height, 3, (const unsigned char*)imageData) == 0)
	{
		Error("Could not save BMP image.");
		return false;
	}
	*/
}

ImageRaw::ImageRaw()
{
}

ImageRaw::ImageRaw(const std::string & path)
{
	//Open file
	std::fstream stream(path, std::ios::in | std::ios::binary | std::ios::ate);
	if (stream.fail())
	{
		mFailed = true;
		return;
	}

	//Read data
	mData.resize(stream.tellg());
	stream.seekg(0);
	stream.read((char*)&mData[0], mData.size());
}

ImageRaw::ImageRaw(EdsImageRef imageRef, const void * data, size_t dataSize, int width, int height)
{
	mData.resize(dataSize);
	memcpy(&mData[0], data, dataSize);
}

ImageRaw::ImageRaw(ImageRaw && img)
{
	mData.swap(img.mData);
	mHeight = img.mHeight;
	mWidth = img.mWidth;
	mFailed = img.mFailed;
	mImageRef = mImageRef;
}

ImageRaw::ImageRaw(const ImageRaw & img)
{
	mData = img.mData;
	mHeight = img.mHeight;
	mWidth = img.mWidth;
	mFailed = img.mFailed;
	mImageRef = mImageRef;
}

ImageRaw::~ImageRaw()
{
	clear();
}

void ImageRaw::clear()
{
	Image::clear();
	if (mImageRef)
		EdsRelease(mImageRef);
}

ImageRGBA ImageRaw::asRGBA()
{
	return ImageRGBA();
}

bool ImageRaw::loadFromFile(const char * file)
{
	return false;
}

bool ImageRaw::saveToFile(const char * file)
{
	return false;
}

ImageRGBA::ImageRGBA()
{
}

ImageRGBA::ImageRGBA(const std::string & path)
{
}

ImageRGBA::ImageRGBA(const void * data, size_t dataSize, int width, int height)
{
}

bool ImageRGBA::loadFromFile(const char * file)
{
	return false;
}

bool ImageRGBA::saveToFile(const char * file)
{
	return false;
}

bool ImageRGBA::loadFromFile(const char * file)
{
	return false;
}

bool ImageRGBA::saveToFile(const char * file)
{
	return false;
}
