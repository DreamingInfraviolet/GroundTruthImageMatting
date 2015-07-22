#pragma once

/**
* A base class defining the basics of an image.
* */

#include <vector>
#include "EDSDKTypes.h"

class Image
{
protected:
	std::vector<unsigned char> mData;
	int mHeight = 0, mWidth = 0;
	bool mFailed = false;
public:

	virtual ~Image() {}

	/** Returns the size of the memory array in bytes. */
	size_t getDataLength();

	/** Frees image memory. */
	virtual void clear();

	/** Returns whether an image operation failed. */
	bool failed();

	/** Marks the image as invalid */
	void fail();

	/** Returns the height. */
	int height();

	/** Returns the width. */
	int width();

	/** Loads an image from a path indicated by the argument. */
	virtual bool loadFromFile(const char* file) = 0;

	/** Saves the image to a path indicated by the argument. */
	virtual bool saveToFile(const char* file) = 0;
};

class ImageRGBA;

/** A class to represent a RAW cr2 image. */
class ImageRaw : public Image
{
private:

	/**
	* Interpolates the image, getting the RGB values as defined by  EdsGetImage.
	* Return is empty upon failure.
	* */
	std::vector<unsigned char> getRawRGB();

	EdsImageRef mImageRef = nullptr;

public:
	/** A trivial constructor. */
	ImageRaw();

	/** Loads an image from the inputted file path. */
	ImageRaw(const std::string& path);

	/** Creates the image from memory, copying from the inputted source. */
	ImageRaw(EdsImageRef imageRef, const void* data, size_t dataSize, int width, int height);

	/** Move constructor. */
	ImageRaw(ImageRaw&& img);

	/** Copy constructor. */
	ImageRaw(const ImageRaw& img);

	/** Destructor. Class clear() */
	~ImageRaw();

	virtual void clear() override;

	/**
	* Creates a RGBA image from the class. The Canon SDK is assumed to be initialised.
	* @return The RGBA image. If failed, the resulting image's failed() will return true.
	* */
	ImageRGBA asRGBA();

	/** Overriden loading */
	virtual bool loadFromFile(const char* file) override;

	/** Overriden saving. */
	virtual bool saveToFile(const char* file) override;
};

/** A class to represent an RGBA RGBA image. Supports Alpha. */
class ImageRGBA : public Image
{
public:
	/** A trivial constructor. */
	ImageRGBA();

	/** Loads an image from the inputted file path. */
	ImageRGBA(const std::string& path);

	/** Creates the image from memory, copying from the inputted source. */
	ImageRGBA(const void* data, size_t dataSize, int width, int height);

	/** Overriden loading */
	virtual bool loadFromFile(const char* file) override;

	/** Overriden saving. */
	virtual bool saveToFile(const char* file) override;
};