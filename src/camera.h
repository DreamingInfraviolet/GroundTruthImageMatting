#pragma once
#include <vector>
#include <string>
#include "EDSDKTypes.h"

/**
* this class serves as an interface to a camera object that is connected to the computer.
* */
class Camera
{
private:
	std::string mName;
	EdsCameraRef mCameraRef = nullptr;
	EdsDeviceInfo mDeviceInfo;
	bool mAvailable;

	friend class CameraList;
public:

	/** Returns whether the camera is availalbe for use (it could have been disconnected, etc.) */
	bool available();

	/** Selects this as the active camera, creating a session.
	* If a camera has been already selected, it is deselected before the operation occurs. */
	bool select();

	/** Deselects the camera if it is currently selected, destroying the session. */
	void deselect();

	/** Returns the name of the camera. */
	std::string name();

	/** Sets the iso value. */
	bool iso(int v);

	/** Gets the iso value. */
	int iso();

	/** Sets the exposure time. */
	bool exposureTime(int v);

	/** Gets the exposure time. */
	int exposureTime();

	/** Sets the aperture width. */
	bool aperture(int v);

	/** Gets the aperture width. */
	int aperture();

	/** Sets the focus value. */
	bool focus(int value);

	/**Gets the focus value. */
	int focus();

	/**Takes a picture and stores it in the directory with the given name.
	* @param Name The name of the image file to be saved.
	* @param Directory The path where the file should be saved. The method will try to create the directory if it does not exist.
	* */
	bool shoot(const std::string& name, const std::string& directory);

};


/**
* A singleton class that manages a list of all the cameras connected to the computer.
* */
class CameraList
{
private:

	/** Hidden constructor. */
	CameraList() {}

	/** A value indicating whether an instance of the class exists. */
	static bool mExists;

	/** The currently existing instance, if any. */
	static CameraList* mInstance;

	/** The currently active camera, if any. */
	static Camera* mActiveCamera;

	/** Initialises the SDK. */
	bool initialise();

	/** Sets a camera as selected. */
	void activeCamera(Camera* cam);

	friend class Camera;
public:

	/** Performs shutdown operations. */
	~CameraList();


	std::vector<Camera> cameras;

	/** Creates a new instance of the class if it was not created previously. Otherwise returns null.
	* It also attempts to initialise the SDK. If any errors are encountered, null is also returned. */
	static CameraList* create();

	/** Returns the current instance of the camera list in existence. */
	static CameraList* instance();

	/** Returns the currently selected camera. */
	Camera* activeCamera();


	/**
	* Finds and initialises all available cameras, filling the camera list.
	* @return The number of cameras found. Returns -1 if an error has occured.
	* */
	int ennumerate();
};