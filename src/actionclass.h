#pragma once
#include <memory>
#include <vector>
#include "camera.h"
#include <qstringlist.h>
#include <chrono>

namespace sf { class RenderWindow; }

/**
* This class is used by the Window to perform general processing unrelated to the interface.
* Tasks include managing the camera, taking picture sequences, etc.
* */

class CameraList;

class ActionClass
{
    /** Initialises the class, returning true upon success. */
    bool initialise();

    //
    std::unique_ptr<CameraList> mCameraList;

    //
    static ActionClass* sActionClass;

    /**
    * Generates a name for an image based on its path, colour and time.
    * @param folder A path to the location where the image is to be stored.
	* @param colour The colour string of the image
	* @param time_ The time to use, as returned by time(0)
    * */
    std::string generateFilePathNoExtension(const std::string& folder,
		const std::string& colour, time_t time_);

	/**
	* Changes the colours of the display and takes pictures for each colour.
	* Requires a valid SDL state.
	* @return A vector of pairs of a colour and its corresponding raw image.
	* @param colours A list of colours to take pictures with
	* @param delay Whether to delay the shooting until startTime
	* @param startTime The time when shooting should start.
	* */
	std::vector < std::pair<QColor, ImageRaw>  > ActionClass::shootPictures
		(const QStringList& colours, bool delay,
		std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now());

public:

    /** Attempts to create the class, returning null if the class exists of if creation failed. */
    static ActionClass* create();

    /** Undregisters the class. */
    ~ActionClass();

    /** Returns the current camera is. */
    int iso();

    /** Returns the current camera aperture. */
    int aperture();

    /** Returns the current camera shutter. */
    int shutter();

	/** Returns the current camera white balance. */
	int whiteBalance();

    /**
    * Sets the iso of the current camera.
    * @param text The text of the camera property, as defined by the internal Camera bidirectional mappings.
    * */
    void iso(const std::string& text);

	/**
	* Sets the aperture size of the current camera.
	* @param text The text of the camera property, as defined by the internal Camera bidirectional mappings.
	* */
	void aperture(const std::string& text);

	/**
	* Sets the shutter duration of the current camera.
	* @param text The text of the camera property, as defined by the internal Camera bidirectional mappings.
	* */
	void shutter(const std::string& text);

	/**
	* Sets the white balance of the current camera.
	* @param text The text of the camera property, as defined by the internal Camera bidirectional mappings.
	* */
	void whiteBalance(const std::string& text);

    /**
    * Returns a vector of possible values that the camera may take.
    * @param ep An enum specifying the properties being requested.
    * */
    std::vector<int> ennumeratePossibleValues(Camera::EnnumerableProperties ep);

    /**
    * Shoots a sequence of photos with the given parameters, saving the result.
	* Uses internal method to do the actual shooting.
    * @param startTime The time when shooting should start.
    * @param colours The list of colours to shoot with.
    * @param saveProcessed Whether to save processed images im processedExtension format.
    * @param saveraw Whether to saw the raw .cr2 images.
    * @param processedExtension The extension with which to save the processed image.
	* @param path The folder where the images should be saved.
    * */
    bool shootSequence(std::chrono::time_point<std::chrono::system_clock> startTime,
        const QStringList& colours, bool saveProcessed, bool saveRaw,bool saveGroundTruth,
		const std::string& processedExtension, const std::string& path);
};