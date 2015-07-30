#include "image.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include "EDSDK.h"
#include "io.h"

ImageRaw::ImageRaw(){}

ImageRaw::~ImageRaw()
{
	clear();
	if (mImageRef)
		EdsRelease(mImageRef);
}

size_t ImageRaw::getDataLength()
{
	return mData.size();
}

void ImageRaw::clear()
{
	//Frees the data
	mData.swap(decltype(mData)());
}

bool ImageRaw::failed()
{
	return mFailed;
}

void ImageRaw::fail()
{
	mFailed = true;
}

int ImageRaw::height()
{
	return mHeight;
}

int ImageRaw::width()
{
	return mWidth;
}

void ImageRaw::copyNonMovable(const ImageRaw & img)
{
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
}

ImageRaw::ImageRaw(EdsImageRef imageRef, const void * data, size_t dataSize, int width, int height)
{
	mImageRef = imageRef;
	mWidth = width;
	mHeight = height;
	mData.resize(dataSize);
	memcpy(&mData[0], data, dataSize);
}

ImageRaw ImageRaw::getFailed()
{
	ImageRaw out;
	out.fail();
	return out;
}

ImageRaw::ImageRaw(ImageRaw&& img)
{
	mImageRef = img.mImageRef;
	img.mImageRef = nullptr;
	mData.swap(img.mData);
	copyNonMovable(img);
}

ImageRaw::ImageRaw(const ImageRaw& img)
{
	mData = img.mData;
	copyNonMovable(img);
	mImageRef = img.mImageRef;
	EdsRetain(mImageRef);
}

ImageRaw& ImageRaw::operator = (ImageRaw&& img)
{
	mImageRef = img.mImageRef;
	img.mImageRef = nullptr;
	mData.swap(img.mData);
	copyNonMovable(img);
	return *this;
}

ImageRaw& ImageRaw::operator = (ImageRaw& img)
{
	mData = img.mData;
	copyNonMovable(img);
	mImageRef = img.mImageRef;
	EdsRetain(mImageRef);
	return *this;
}

bool ImageRaw::saveProcessed(const std::string& path)
{
	if (failed())
		return false;

	EdsImageInfo imageInfo;
	CHECK_EDS_ERROR(EdsGetImageInfo(mImageRef, kEdsImageSrc_RAWFullView, &imageInfo),
		"Could not retrieve image info", false);

	if (imageInfo.width*imageInfo.height == 0)
	{
		Warning("Cannot process empty image");
		return false;
	}

	//Create stream
	EdsStreamRef rgbStream = nullptr;
	CHECK_EDS_ERROR(EdsCreateMemoryStream(3 * imageInfo.width*imageInfo.height, &rgbStream),
		"Failed to create memory stream", false);

	//Get image in rgb format into memory
	EdsSize size;
	size.height = imageInfo.height;
	size.width = imageInfo.width;
	CHECK_EDS_ERROR(EdsGetImage(mImageRef, kEdsImageSrc_RAWFullView, kEdsTargetImageType_RGB,
		imageInfo.effectiveRect, size, rgbStream), "Could not retrieve the image", false);

	void* imageData;
	CHECK_EDS_ERROR(EdsGetPointer(rgbStream, &imageData), "Could not retrieve the image pointer", false);

	//Create image mat for rgb
	cv::Mat imageRgb(size.height, size.width, CV_8UC3, imageData);

	//Save mat
	if (!cv::imwrite(path, imageRgb))
	{
		EdsRelease(rgbStream);
		Error("Could not save image file");
		return false;
	}

	//Free rgb stream
	EdsRelease(rgbStream);

	return true;
}

bool ImageRaw::saveToFile(const std::string& path)
{
	if (failed())
		return false;

	if (mData.size() == 0)
		return false;

	std::fstream stream(path, std::ios::out | std::ios::binary);
	if (stream.fail())
	{
		Error(std::string("Could not save image ") + path);
		return false;
	}

	stream.write((const char*)&mData[0], mData.size());

	return true;
}