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

	enum IsoValue
	{
		ISO_6 = 0x00000028,
		ISO_12 = 0x00000030,
		ISO_25 = 0x00000038,
		ISO_50 = 0x00000040,
		ISO_100 = 0x00000048,
		ISO_125 = 0x0000004b,
		ISO_160 = 0x0000004d,
		ISO_200 = 0x00000050,
		ISO_250 = 0x00000053,
		ISO_320 = 0x00000055,
		ISO_400 = 0x00000058,
		ISO_500 = 0x0000005b,
		ISO_640 = 0x0000005d,
		ISO_800 = 0x00000060,
		ISO_1000 = 0x00000063,
		ISO_1250 = 0x00000065,
		ISO_1600 = 0x00000068,
		ISO_3200 = 0x00000070,
		ISO_6400 = 0x00000078,
		ISO_12800 = 0x00000080,
		ISO_25600 = 0x00000088,
		ISO_INVALID = 0xffffffff
	};

	enum ApertureValue
	{
		AV_1 = 0x08,
		AV_11 = 0x40,
		AV_1_1 = 0x0B,
		AV_13_1_3 = 0x43, //13 1/3
		AV_1_2 = 0x0C,
		AV_13 = 0x44,
		AV_1_2_1_3 = 0x0D, //1.2 1/3...
		AV_14 = 0x45,
		AV_1_4 = 0x10,
		AV_16 = 0x48,
		AV_1_6 = 0x13,
		AV_18 = 0x4B,
		AV_1_8 = 0x14,
		AV_19 = 0x4C,
		AV_1_8_1_3 = 0x15,
		AV_20 = 0x4D,
		AV_2 = 0x18,
		AV_22 = 0x50,
		AV_2_2 = 0x1B,
		AV_25 = 0x53,
		AV_2_5 = 0x1C,
		AV_27 = 0x54,
		AV_2_5_1_3 = 0x1D,
		AV_29 = 0x55,
		AV_2_8 = 0x20,
		AV_32 = 0x58,
		AV_3_2 = 0x23,
		AV_36 = 0x5B,
		AV_3_5 = 0x24,
		AV_38 = 0x5C,
		AV_3_5_1_3 = 0x25,
		AV_40 = 0x5D,
		AV_4 = 0x28,
		AV_45 = 0x60,
		AV_4_5 = 0x2B,
		AV_51 = 0x63,
		AV_4_5 = 0x2C,
		AV_54 = 0x64,
		AV_5_0 = 0x2D,
		AV_57 = 0x65,
		AV_5_6 = 0x30,
		AV_64 = 0x68,
		AV_6_3 = 0x33,
		AV_72 = 0x6B,
		AV_6_7 = 0x34,
		AV_76 = 0x6C,
		AV_7_1 = 0x35,
		AV_80 = 0x6D,
		AV_8 = 0x38,
		AV_91 = 0x70,
		AV_9 = 0x3B,
		AV_INVALID = 0xffffffff,
		AV_9_5 = 0x3C,
		AV_10 = 0x3D
	};

	/** Returns whether the camera is availalbe for use (it could have been disconnected, etc.) */
	bool available();

	/** Selects this as the active camera, creating a session.
	* If a camera has been already selected, it is deselected before the operation occurs. */
	bool select();

	/** Deselects the camera if it is currently selected, destroying the session. */
	void deselect();

	/** Returns the name of the camera. */
	std::string name();

	/** Sets the iso value. Returns false upon failure. */
	bool iso(IsoValue v);

	/** Gets the iso value. Returns -1 upon failure. */
	IsoValue iso();

	/** Gets all the possible Iso values for the camera. */
	std::vector<IsoValue> isoValues();

	/** Sets the shutter speed. Returns false upon failure. */
	bool shutterSpeed(int v);

	/** Gets the shutter speed. Returns -1 upon failure. */
	int shutterSpeed();

	/** Gets the maximum shutter speed. Returns -1 upon failure. */
	int maxShutterSpeed();

	/** Gets the minimum shutter speed. Returns -1 upon failure. */
	int minShutterSpeed();

	/** Sets the aperture width. Returns false upon failure. */
	bool aperture(int v);

	/** Gets the aperture width. Returns -1 upon failure. */
	int aperture();

	/** Gets all the possible Aperture values for the camera. */
	std::vector<ApertureValue> apertureValues();

	/** Sets the focus value. Returns false upon failure. */
	bool focus(int value);

	/**Gets the focus value. Returns -1 upon failure. */
	int focus();

	/**Takes a picture and stores it in the directory with the given name.
	* @param Name The name of the image file to be saved.
	* @param Directory The path where the file should be saved. The method will try to create the directory if it does not exist.
	* @return Sucess/Failure of the operation.
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