#pragma once
#include <vector>
#include "EDSDKTypes.h"

/** A specialised class to represent a .cr2 image object. */
class ImageRaw
{
private:
	std::vector<unsigned char> mData;
	int mHeight = 0, mWidth = 0;
	bool mFailed = false;

    /** Copies values into the class that can not be moved. */
    void copyNonMovable(const ImageRaw& img);

    //The Eds image reference object.
    EdsImageRef mImageRef = nullptr;

public:
    /** A trivial constructor. */
    ImageRaw();

    /**
    * Creates the image from memory, copying from the inputted source.
    * The memory must describe a valid .cr2 object.
    * @param imageRef the EdsImageRef object of the image.
    * @param data A pointer to the data to be copied.
    * @dataSize The number of bytes that constitute the data.
    * @width The width of the image.
    * @height The height of the image.
    * */
    ImageRaw(EdsImageRef imageRef, const void* data, size_t dataSize, int width, int height);

	/** Clears the image. */
	~ImageRaw();

    /** Creates a failed empty image. */
    static ImageRaw getFailed();

    /** Move constructor to help performance */
    ImageRaw(ImageRaw&& img);

    /** Copy constructor. */
    ImageRaw(const ImageRaw& img);

    /** Move operator. */
    ImageRaw& operator = (ImageRaw&& img);

    /** Copy operator. */
    ImageRaw& operator = (ImageRaw& img);

    /** Clears the image, releasing the Eds image reference object. */
    void clear();

    /** Saves the image in a format that can be handled by OpenCV. */
	bool saveProcessed(const std::string& path);

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
};