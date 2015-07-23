#include "image.h"
#include <assert.h>
#include <fstream>
#include "EDSDK.h"
#include "io.h"
#include "SOIL.h"



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


std::vector<unsigned char> ImageRaw::getRawRGB()
{
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
	

	std::vector<unsigned char> out;
	out.resize(3 * imageInfo.width*imageInfo.height);
	memcpy(&out[0], imageData, out.size());

	EdsUInt32 expectedImageSize;
	EdsGetLength(rgbStream, &expectedImageSize);
	assert(expectedImageSize == out.size());

	EdsRelease(rgbStream);

	return out;
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
		*(uint32_t*)(void*)rgbaptr = *(const uint32_t*)(const void*)rgbptr;
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
	std::vector<unsigned char> rgba = rgbToRgba(getRawRGB(), 255);
	if (rgba.size() == 0)
	{
		ImageRGBA out;
		out.fail();
		return out;
	}
	return ImageRGBA (&rgba[0], rgba.size(), mWidth, mHeight);
}

bool ImageRaw::loadFromFile(const char * path)
{
	Error("Loading RAW images is unsupported.");
	return false;
}

bool ImageRaw::saveToFile(const char * path)
{
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

bool ImageRGBA::loadFromFile(const char * path)
{
	if (SOIL_load_image(path, &mWidth, &mHeight, nullptr, SOIL_LOAD_RGBA) == 0)
	{
		Error(std::string("Could not load ") + path);
		return false;
	}
	return true;
}

bool ImageRGBA::saveToFile(const char * path)
{
	if (SOIL_save_image(path, SOIL_SAVE_TYPE_TGA, mWidth, mHeight, 4, &mData[0]) == 0)
	{
		Error(std::string("Could not save ") + path);
		return false;
	}

	return true;
}