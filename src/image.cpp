#include "image.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <assert.h>
#include <fstream>
#include "EDSDK.h"
#include "io.h"
#include "camera.h"


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

void ImageRaw::copyNonMovable(const ImageRaw & img)
{
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
}

std::vector<unsigned char> ImageRaw::rgbToRgba(const std::vector<unsigned char>& rgb, unsigned char alpha)
{
	if (rgb.size() % 3 != 0)
	{
		Warning("Can not convert RGB to RGBA: RGB array not divisibly by 3.");
		return{};
	}

	if (rgb.size() == 0)
		return{};

	std::vector<unsigned char> out;
	out.resize(rgb.size() / 3 * 4);


	//Adapted from http://stackoverflow.com/a/7069317
	const unsigned char* rgbptr = &rgb[0];
	unsigned char* rgbaptr = &out[0];
	unsigned count = rgb.size()/3;

	for (unsigned i = count; --i; rgbaptr += 4, rgbptr += 3)
		*(unsigned*)(void*)rgbaptr = *(const unsigned*)(const void*)rgbptr;
	for (unsigned j = 0; j<3; ++j)
		rgbaptr[j] = rgb[j];

	//Set alpha
	for (unsigned i = 3; i < out.size(); i += 4)
		out[i] = alpha;

	return out;
}

ImageRaw::ImageRaw()
{
}

ImageRaw::ImageRaw(EdsImageRef imageRef, const void * data, size_t dataSize, int width, int height)
{
	mImageRef = imageRef;
	mWidth = width;
	mHeight = height;
	mData.resize(dataSize);
	memcpy(&mData[0], data, dataSize);
}

ImageRaw::~ImageRaw()
{
	clear();
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
	return *this;
}

void ImageRaw::clear()
{
	Image::clear();
	if (mImageRef)
		EdsRelease(mImageRef);
}

ImageRGBA ImageRaw::asRGBA()
{
	EdsImageInfo imageInfo;
	CHECK_EDS_ERROR(EdsGetImageInfo(mImageRef, kEdsImageSrc_RAWFullView, &imageInfo),
		"Could not retrieve image info", ImageRGBA::getFailed());

	//Create stream
	EdsStreamRef rgbStream = nullptr;
	CHECK_EDS_ERROR(EdsCreateMemoryStream(3 * imageInfo.width*imageInfo.height, &rgbStream),
		"Failed to create memory stream", {});

	if (imageInfo.width*imageInfo.height == 0)
	{
		Warning("Cannot process empty image");
		return{ ImageRGBA::getFailed() };
	}

	//Get image in rgb format into memory
	EdsSize size;
	size.height = imageInfo.height;
	size.width = imageInfo.width;
	CHECK_EDS_ERROR(EdsGetImage(mImageRef, kEdsImageSrc_RAWFullView, kEdsTargetImageType_RGB,
		imageInfo.effectiveRect, size, rgbStream), "Could not retrieve the image", ImageRGBA::getFailed());

	//Assert byte size of the stream
	EdsUInt32 imageDataSize;
	EdsGetLength(rgbStream, &imageDataSize);
	assert(imageDataSize == size.width*size.height * 3);


	void* imageData;
	CHECK_EDS_ERROR(EdsGetPointer(rgbStream, &imageData), "Could not retrieve the image pointer", ImageRGBA::getFailed());

	//Create image mat for rgb
	cv::Mat imageRgb(size.height, size.width, CV_8UC3, imageData);

	//Create image mat for new rgba
	std::vector<unsigned char> rgbaData;
	rgbaData.resize(size.height*size.width*4);
	cv::Mat imageRgba(size.height, size.width, CV_8UC4, &rgbaData[0]);

	//Copy rgb into rgba, making conversions
	cv::cvtColor(imageRgb, imageRgba, CV_RGB2RGBA, 4);

	//Free rgb
	EdsRelease(rgbStream);

	return ImageRGBA (rgbaData, mWidth, mHeight);
}

bool ImageRaw::loadFromFile(const char * path)
{
	Error("Loading RAW images is unsupported.");
	return false;
}

bool ImageRaw::saveToFile(const char * path)
{
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

ImageRGBA::ImageRGBA()
{
}

ImageRGBA::ImageRGBA(const char* path)
{
	if(!loadFromFile(path))
		mFailed = true;
}

ImageRGBA::ImageRGBA(const void * data, size_t dataSize, int width, int height)
{
	mData.resize(dataSize);
	mWidth = width;
	mHeight = height;
	memcpy(&mData[0], data, dataSize);
}

ImageRGBA::ImageRGBA(std::vector<unsigned char>& vector, int width, int height)
{
	mData.swap(vector);
	mWidth = width;
	mHeight = height;
}

ImageRGBA ImageRGBA::getFailed()
{
	ImageRGBA out;
	out.fail();
	return out;
}

bool ImageRGBA::loadFromFile(const char * path)
{
	cv::Mat image = cv::imread(path, CV_8UC4);
	if (image.data==nullptr)
	{
		Error(std::string("Could not load ") + path);
		return false;
	}

	mData.resize(image.rows*image.cols*4);
	memcpy(&mData[0], image.data, mData.size());

	return true;
}

bool ImageRGBA::saveToFile(const char * path)
{
	if (mData.size() == 0)
		return false;

	cv::Mat image(mHeight, mWidth, CV_8UC4, &mData[0]);
	if(!cv::imwrite(path, image))
	{
		Error(std::string("Could not save ") + path);
		return false;
	}

	return true;
}