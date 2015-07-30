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
    * Returns a vector of possible values that the camera may take.
    * @param ep An enum specifying the properties being requested.
    * */
    std::vector<int> ennumeratePossibleValues(Camera::EnnumerableProperties ep);

    /**
    * Shoots a sequence of photos with the given parameters, saving the result.
    * @param startTime The time when shooting should start.
    * @param colours The list of colours to shoot with.
    * @param saveProcessed Whether to save processed images im processedExtension format.
    * @param saveraw Whether to saw the raw .cr2 images.
    * @param processedExtension The extension with which to save the processed image.
    * */
    bool shootSequence(std::chrono::time_point<std::chrono::system_clock> startTime,
        const QStringList& colours, bool saveProcessed, bool saveRaw, const std::string& processedExtension);
};