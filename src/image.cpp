#include "image.h"
#include <fstream>
#include "io.h"
#include "camera.h"
#include <EDSDK.h>
#include <opencv2/opencv.hpp>

//If defined, the ground truth image will be halved.
#define HALVE_GT_IMAGE

ImageRaw::ImageRaw(){}

ImageRaw::~ImageRaw()
{
	clear();
}

size_t ImageRaw::getDataLength()
{
	return mData.size();
}

void ImageRaw::clear()
{
	//Frees the data
	mData.swap(decltype(mData)());
	mImageRef.clear();
	mWidth = 0;
	mHeight = 0;
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

ImageRaw::ImageRaw(EdsImageRef imageRef, const void * data, size_t dataSize, int width, int height)
{
	clear();
	mImageRef.mRef = imageRef;
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


ImageRaw& ImageRaw::operator = (ImageRaw&& img)
{
	mImageRef = std::move(img.mImageRef);
	mData.swap(img.mData);
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
	return *this;
}

ImageRaw& ImageRaw::operator = (const ImageRaw& img)
{
	mData = img.mData;
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
	mImageRef = img.mImageRef;
	return *this;
}

ImageRaw::ImageRaw(const ImageRaw& img)
{
	mData = img.mData;
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
	mImageRef = img.mImageRef;
}

ImageRaw::ImageRaw(ImageRaw&& img)
{
	mImageRef = std::move(img.mImageRef);
	mData.swap(img.mData);
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
}

bool ImageRaw::saveProcessed(const std::string& path, const RawRgbEds* rgb)
{
	Inform("Saving " + path);

	if (failed())
		return false;

	int width, height;
	EdsStreamContainer container;


	if (rgb == nullptr)
	{
		auto rgbStream(findRgb());
		width = std::get<0>(rgbStream);
		height = std::get<1>(rgbStream);
		container = (std::get<2>(rgbStream));
		if (width*height == 0)
			return false;
	}
	else
	{
		width = std::get<0>(*rgb);
		height = std::get<1>(*rgb);
		container = (std::get<2>(*rgb));
	}

	if (container.size() == 0)
		return false;


	//Create image mat for rgb
	cv::Mat imageRgb(height, width, CV_8UC3, container.pointer());

	//Save mat
	if (!cv::imwrite(path, imageRgb))
	{
		Error("Could not save image file");
		return false;
	}



	return true;
}

bool ImageRaw::saveToFile(const std::string& path)
{
	Inform("Saving " + path);

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

RawRgbEds ImageRaw::findRgb()
{
	if (failed())
		return{};

	EdsImageInfo imageInfo;
	CHECK_EDS_ERROR(EdsGetImageInfo(mImageRef.mRef, kEdsImageSrc_RAWFullView, &imageInfo),
		"Could not retrieve image info", {});

	if (imageInfo.width*imageInfo.height == 0)
	{
		Warning("Cannot process empty image");
		return{};
	}

	//Create stream
	EdsStreamContainer rgbStream;
	rgbStream.setDepends(mImageRef.mRef);
	CHECK_EDS_ERROR(EdsCreateMemoryStream(3 * imageInfo.width*imageInfo.height, &rgbStream.mRef),
		"Failed to create memory stream", {});

	//Get image in rgb format into memory
	EdsSize size;
	size.height = imageInfo.height;
	size.width = imageInfo.width;
	CHECK_EDS_ERROR(EdsGetImage(mImageRef.mRef, kEdsImageSrc_RAWFullView, kEdsTargetImageType_RGB,
		imageInfo.effectiveRect, size, rgbStream.mRef), "Could not retrieve the image", {});

	return std::make_tuple(size.width, size.height, rgbStream);
}