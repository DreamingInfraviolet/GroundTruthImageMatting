#pragma once
#include <vector>
#include "EDSDKTypes.h"

namespace cv { class Mat; }
class EdsStreamContainer;

/** A specialised class to represent a .cr2 image object. */
class ImageRaw
{
private:
	std::vector<unsigned char> mData;
	int mHeight = 0, mWidth = 0;
	bool mFailed = false;

    //The Eds image reference object.
    EdsImageRef mImageRef = nullptr;

public:

	//Used to store raw RGB information of an image.
	//<width,height,stream>
	typedef std::tuple<int, int, EdsStreamContainer> RawRgb;

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
	* If the RGB data was generated beforehand, it may be passed in as the last parameter for performance. */
	bool saveProcessed(const std::string& path, const ImageRaw::RawRgb* rgb = nullptr);

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
	RawRgb findRgb();

	/** Takes in 10 images and generates the ground truth image. Returns null upon failure. Destroys inputs for memory*/
	static std::vector<cv::Mat>
		generateGroundTruth( RawRgb& foreground1, RawRgb& foreground2, RawRgb& foreground3,
		RawRgb& foreground4, RawRgb& foreground5,
		RawRgb& background1, RawRgb& background2, RawRgb& background3,
		RawRgb& background4, RawRgb& background5);
};