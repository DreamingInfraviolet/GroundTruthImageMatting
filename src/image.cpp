#include "image.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include "io.h"
#include "camera.h"
#include <EDSDK.h>
#include "groundtruth.h"

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
	if (mImageRef)
		EdsRelease(mImageRef);
	mImageRef = nullptr;
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


ImageRaw& ImageRaw::operator = (ImageRaw&& img)
{
	if (mImageRef)
		EdsRelease(mImageRef);
	mImageRef = img.mImageRef;
	img.mImageRef = nullptr;
	mData.swap(img.mData);
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
	return *this;
}

ImageRaw& ImageRaw::operator = (const ImageRaw& img)
{
	if (mImageRef)
		EdsRelease(mImageRef);
	mData = img.mData;
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
	mImageRef = img.mImageRef;
	EdsRetain(mImageRef);
	return *this;
}

ImageRaw::ImageRaw(const ImageRaw& img)
{
	if (mImageRef)
		EdsRelease(mImageRef);
	mData = img.mData;
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
	mImageRef = img.mImageRef;
	EdsRetain(mImageRef);
}

ImageRaw::ImageRaw(ImageRaw&& img)
{
	if (mImageRef)
		EdsRelease(mImageRef);
	mImageRef = img.mImageRef;
	img.mImageRef = nullptr;
	mData.swap(img.mData);
	mFailed = img.mFailed;
	mWidth = img.mWidth;
	mHeight = img.mHeight;
}

bool ImageRaw::saveProcessed(const std::string& path, const ImageRaw::RawRgb* rgb)
{
	Inform("Saving " + path);

	if (failed())
		return false;

	int width, height;
	EdsStreamContainer container;


	if (rgb == nullptr)
	{
		auto rgbStream (findRgb());
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

	if (container.size()==0)
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

ImageRaw::RawRgb ImageRaw::findRgb()
{
	if (failed())
		return{};

	EdsImageInfo imageInfo;
	CHECK_EDS_ERROR(EdsGetImageInfo(mImageRef, kEdsImageSrc_RAWFullView, &imageInfo),
		"Could not retrieve image info", {});

	if (imageInfo.width*imageInfo.height == 0)
	{
		Warning("Cannot process empty image");
		return{};
	}

	//Create stream
	EdsStreamContainer rgbStream;
	rgbStream.setImageRef(mImageRef);
	CHECK_EDS_ERROR(EdsCreateMemoryStream(3 * imageInfo.width*imageInfo.height, &rgbStream.mRef),
		"Failed to create memory stream", {});

	//Get image in rgb format into memory
	EdsSize size;
	size.height = imageInfo.height;
	size.width = imageInfo.width;
	CHECK_EDS_ERROR(EdsGetImage(mImageRef, kEdsImageSrc_RAWFullView, kEdsTargetImageType_RGB,
		imageInfo.effectiveRect, size, rgbStream.mRef), "Could not retrieve the image", {});

	return std::make_tuple(size.width, size.height, rgbStream);
}

std::vector<cv::Mat>
	ImageRaw::generateGroundTruth(RawRgb& foreground1, RawRgb& foreground2, RawRgb& foreground3,
	RawRgb& foreground4, RawRgb& foreground5,
	RawRgb& background1, RawRgb& background2, RawRgb& background3,
	RawRgb& background4, RawRgb& background5)
{
	using namespace cv;

	Mat mUcharf1(std::get<1>(foreground1), std::get<0>(foreground1), CV_8UC3, std::get<2>(foreground1).pointer());
	Mat mUcharf2(std::get<1>(foreground2), std::get<0>(foreground2), CV_8UC3, std::get<2>(foreground2).pointer());
	Mat mUcharf3(std::get<1>(foreground3), std::get<0>(foreground3), CV_8UC3, std::get<2>(foreground3).pointer());
	Mat mUcharf4(std::get<1>(foreground4), std::get<0>(foreground4), CV_8UC3, std::get<2>(foreground4).pointer());
	Mat mUcharf5(std::get<1>(foreground5), std::get<0>(foreground5), CV_8UC3, std::get<2>(foreground5).pointer());

	Mat mUcharb1(std::get<1>(background1), std::get<0>(background1), CV_8UC3, std::get<2>(background1).pointer());
	Mat mUcharb2(std::get<1>(background2), std::get<0>(background2), CV_8UC3, std::get<2>(background2).pointer());
	Mat mUcharb3(std::get<1>(background3), std::get<0>(background3), CV_8UC3, std::get<2>(background3).pointer());
	Mat mUcharb4(std::get<1>(background4), std::get<0>(background4), CV_8UC3, std::get<2>(background4).pointer());
	Mat mUcharb5(std::get<1>(background5), std::get<0>(background5), CV_8UC3, std::get<2>(background5).pointer());

	Mat mFloatf1;
	Mat mFloatf2;
	Mat mFloatf3;
	Mat mFloatf4;
	Mat mFloatf5;

	Mat mFloatb1;
	Mat mFloatb2;
	Mat mFloatb3;
	Mat mFloatb4;
	Mat mFloatb5;

	{
		Mat mFloatff1;
		mUcharf1.convertTo(mFloatff1, CV_32FC3);
		resize(mFloatff1, mFloatf1, Size(std::get<0>(foreground1)/2, std::get<1>(foreground1)/2));
		std::get<2>(foreground1).clear();
	}
	{
		Mat mFloatff2;
		mUcharf2.convertTo(mFloatff2, CV_32FC3);
		resize(mFloatff2, mFloatf2, Size(std::get<0>(foreground2)/2, std::get<1>(foreground2)/2));
		std::get<2>(foreground2).clear();
	}
	{
		Mat mFloatff3;
		mUcharf3.convertTo(mFloatff3, CV_32FC3);
		resize(mFloatff3, mFloatf3, Size(std::get<0>(foreground3)/2, std::get<1>(foreground3)/2));
		std::get<2>(foreground3).clear();
	}
	{
		Mat mFloatff4;
		mUcharf4.convertTo(mFloatff4, CV_32FC3);
		resize(mFloatff4, mFloatf4, Size(std::get<0>(foreground4)/2, std::get<1>(foreground4)/2));
		std::get<2>(foreground4).clear();
	}
	{
		Mat mFloatff5;
		mUcharf5.convertTo(mFloatff5, CV_32FC3);
		resize(mFloatff5, mFloatf5, Size(std::get<0>(foreground5)/2, std::get<1>(foreground5)/2));
		std::get<2>(foreground5).clear();
	}

	{
		Mat mFloatbf1;
		mUcharb1.convertTo(mFloatbf1, CV_32FC3);
		resize(mFloatbf1, mFloatb1, Size(std::get<0>(background1)/2, std::get<1>(background1)/2));
		std::get<2>(background1).clear();
	}
	{
		Mat mFloatbf2;
		mUcharb2.convertTo(mFloatbf2, CV_32FC3);
		resize(mFloatbf2, mFloatb2, Size(std::get<0>(background2)/2, std::get<1>(background2)/2));
		std::get<2>(background2).clear();
	}
	{
		Mat mFloatbf3;
		mUcharb3.convertTo(mFloatbf3, CV_32FC3);
		resize(mFloatbf3, mFloatb3, Size(std::get<0>(background3)/2, std::get<1>(background3)/2));
		std::get<2>(background3).clear();
	}
	{
		Mat mFloatbf4;
		mUcharb4.convertTo(mFloatbf4, CV_32FC3);
		resize(mFloatbf4, mFloatb4, Size(std::get<0>(background4)/2, std::get<1>(background4)/2));
		std::get<2>(background4).clear();
	}
	{
		Mat mFloatbf5;
		mUcharb5.convertTo(mFloatbf5, CV_32FC3);
		resize(mFloatbf5, mFloatb5, Size(std::get<0>(background5)/2, std::get<1>(background5)/2));
		std::get<2>(background5).clear();
	}

	Mat a;
	Mat f;
	f.create(std::get<1>(background4) / 2, std::get<0>(background4) / 2, CV_32FC3);
	Mat af;
	af.create(std::get<1>(background4) / 2, std::get<0>(background4) / 2, CV_32FC3);

	a = GG::groundTruthAlpha2(mFloatf1, mFloatf2, mFloatf3, mFloatf4, mFloatf5,
		mFloatb1, mFloatb2, mFloatb3, mFloatb4, mFloatb5, f, af);

	//Convert results to rgb

	Mat aR, aRgb, fRgb, afRgb;
	a.convertTo(aR, CV_8UC1, 255);
	cvtColor(aR, aRgb, CV_GRAY2RGB);

	f.convertTo(fRgb, CV_8UC4);
	af.convertTo(afRgb, CV_8UC4);

	return{ aRgb, fRgb, afRgb };
}