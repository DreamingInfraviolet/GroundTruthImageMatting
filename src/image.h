#pragma once
#include <vector>
#include "EDSDKTypes.h"
#include "edscontainer.h"
#include "rawrgbeds.h"
#include <stdint.h>

namespace cv { class Mat; }
class EdsStreamContainer;

/** A specialised class to represent a .cr2 image object. */
class ImageRaw
{
private:
	std::vector<uint16_t> mData;
	int mHeight = 0, mWidth = 0;
	bool mFailed = false;

	//The Eds image reference object.
	EdsContainer<EdsImageRef> mImageRef;

public:

	//Used to store raw RGB information of an image:
	//<width,height,stream>



	/** A trivial constructor. */
	ImageRaw();

	/**
	* Creates the image from memory, copying from the inputted source.
	* The memory must describe a valid .cr2 object.
	* @param imageRef the EdsImageRef object of the image.
	* @param data A pointer to the data to be copied (EDS raw image)
	* @dataSize The number of bytes that constitute the data.
	* @width The width of the image.
	* @height The height of the image.
	* */
	ImageRaw(EdsImageRef imageRef, const void* data, size_t dataSize, int width, int height);

	ImageRaw(const ImageRaw& img);
	ImageRaw(ImageRaw&& img);

	/** Clears the image. */
	~ImageRaw();

	/** Creates a failed empty image. */
	static ImageRaw getFailed();

	/** Move operator. */
	ImageRaw& operator = (ImageRaw&& img);

	/** Copy operator. */
	ImageRaw& operator = (const ImageRaw& img);

	/** Clears the image, releasing the Eds image reference object. */
	void clear();

	/**
	* Saves the image in a format that can be handled by OpenCV.
	* The second parameter is the RGB data obtained by . findRgb(). */
	bool saveProcessed(const std::string& path, const RawRgbEds& rgb);

	/** Saves the .cr2 image to the specified path. */
	bool saveToFile(const std::string& path);

	/** Returns the size of the memory array in bytes. */
	size_t getDataLength();

	/** Returns whether an image operation failed. */
	bool failed();

	/** Marks the image as invalid */
	void fail();

	/** Returns the height. */
	int height();

	/** Returns the width. */
	int width();

	/** Generates the rgb data of the image. */
	RawRgbEds findRgb();
};