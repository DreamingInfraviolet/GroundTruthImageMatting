#pragma once
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "EDSDKTypes.h"
#include "propertymap.h"

/**
* this class serves as an interface to a camera object that is connected to the computer.
* */
class Camera
{
private:
	//Used to manage event threads, etc.
	//Declared as pointers to allow storage in a vector due to moving being deleted otherwise.
	std::mutex* mMutex;
	std::atomic<bool>* mReadyToTakePhoto;
	std::condition_variable* mConditionVariable;

	//General purpose variables
	EdsCameraRef mCameraRef = nullptr;
	EdsDeviceInfo* mDeviceInfo;
	bool mAvailable;
	bool mInformOutput;

	friend class CameraList;

public:

	enum class EnnumerableProperties {ShutterSpeed, ISO, Aperture};

	/**
	* The constructor initialising the camera.
	* @param debugOutput Whether to print operational information.
	* @param ref The Eds camera reference object.
	* @param info The populated device info object. Memory handled by the camera.
	* */
	Camera(bool debugOutput, EdsCameraRef ref, EdsDeviceInfo* info);

	/** Destructor that cleans up and frees memory, closing any connections. */
	~Camera();

	/** Returns whether the camera is availalbe for general use (it could have been disconnected, etc.) */
	bool available();

	/** Returns whether the camera is ready to take the next picture. */
	bool readyToShoot();

	/** Selects this as the active camera, creating a session.
	* If a camera has been already selected, it is deselected before the operation occurs. */
	bool select();

	/** Deselects the camera if it is currently selected, destroying the session. */
	void deselect();

	/** Returns the name of the camera. */
	std::string name();


	/** Gets all the possible value IDs for the given property. */
	std::vector<int> ennumeratePossibleValues(EnnumerableProperties ep);

	/**
	Sets the iso value. Returns false upon failure.
	* @param v The id of the iso value as defined in CameraList::isoMappings.
	* */
	bool iso(int v);

	/** Gets the iso id. Returns -1 upon failure. */
	int iso();

	/**
	* Sets the shutter speed. Returns false upon failure.
	* @param v The id of the shutter speed value as defined in CameraList::shutterSpeedMappingsMappings.
	* */
	bool shutterSpeed(int v);

	/** Gets the shutter speed id. Returns -1 upon failure. */
	int shutterSpeed();

	/** Sets the aperture width. Returns false upon failure.
	* @param v The id of the aperture value as defined in CameraList::apertureMappings.
	* */
	bool aperture(int v);

	/** Gets the aperture width id. Returns -1 upon failure. */
	int aperture();

	/**Takes a picture and stores it in the directory with the given name.
	* If the previous picture is still not saved, the function will block until that is done.
	* @param Name The name of the image file to be saved.
	* @param Directory The path where the file should be saved. The method will try to create the directory if it does not exist.
	* @return Sucess/Failure of the operation.
	* */
	bool shoot(const std::string& name, const std::string& directory);

	/** Resets the shutdown timer of the camera, keeping it awake for longer without powering off. */
	bool resetShutdownTimer();

	/**
	* A callback that receives object events from the camera.
	* Each event spawns a new thread to handle the callback.
	* @param inEvent Indicates the event type.
	* @param inRef a reference to the object created by the event.
	* @param inContext A pointer to the object passed in when registering the callback. In this case, ObjectCallbackInContext*.
	* */
	static EdsError EDSCALLBACK objectCallback(EdsObjectEvent inEvent, EdsBaseRef inRef, EdsVoid *inContext);
};


/**
* A singleton class that manages a list of all the cameras connected to the computer.
* */
class CameraList
{
private:

	/** Hidden constructor.
	* @param debugOutput Whether to print non-error output
	* @param id The id of the cameralist instance
	* */
	CameraList(bool debugOutput, unsigned long long id);

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

	/** Returns an ID of the camera session. Might be useless... */
	unsigned long long sessionID();

	//used to help keep track of the camera session.
	static unsigned long long camerasCreationRecord;

	unsigned long long mSessionID;
	bool mInformOutput;

	/**
	* The struct passed to inContext for the objectCallback.
	* */
	struct ObjectCallbackInContext
	{
		unsigned long long cameraListSessionID;
		Camera* camera;
	};

	friend class Camera;
public:

	//Mappings between property IDs and their textual counterparts.
	static const PropertyMap isoMappings;
	static const PropertyMap apertureMappings;
	static const PropertyMap shutterSpeedMappings;


	/** Performs shutdown operations. */
	~CameraList();


	std::vector<Camera> cameras;

	/**
	* Creates a new instance of the class if it was not created previously. Otherwise returns null.
	* It also attempts to initialise the SDK. If any errors are encountered, null is also returned.
	* @param debugOutput Determines whether non-error output will be printed through the Inform mechanism within
	                     CameraList and its cameras.
	* */
	static CameraList* create(bool debugOutput);

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