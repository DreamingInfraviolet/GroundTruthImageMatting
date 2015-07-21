#include <camera.h>
#include <io.h>
#include "EDSDK.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//returns the ret parameter if the function fails, printing the given error message along with the error code.
#define CHECK_EDS_ERROR(func, message, ret) {													\
								   int err = func; 												\
								   if(err!=EDS_ERR_OK) 											\
								   	{															\
								   		Error(std::string("Camera error in ") + 				\
								   		ToString(__FILE__) + " on line " +						\
								   		ToString(__LINE__) + ": " + message + 					\
								   		" | Error Code " + ToString(err));  					\
								   		return ret;												\
								   	}															\
							   }


//Warns if an error has occured without returning.
#define WARN_EDS_ERROR(func, message)   {														\
								   int err = func;	 											\
								   if(err!=EDS_ERR_OK) 											\
								   		Warning(std::string("Camera error in ") + 				\
								   		ToString(__FILE__) + " on line " +						\
								   		ToString(__LINE__) + ": " + message + 					\
								   		" | Error Code " + ToString(err));  					\
							    }


//Conditional camera inform: Inform(msg) is called if mInformOutput is true.
#define CCINFORM(msg) {if(mInformOutput) Inform(msg);}

bool		CameraList::mExists = false;
CameraList* CameraList::mInstance = nullptr;
Camera*		CameraList::mActiveCamera = nullptr;


CameraList::CameraList(bool debugOutput)
{
	CCINFORM("Creating camera list.");
	mInformOutput = debugOutput;
}

CameraList* CameraList::create(bool debugOutput)
{
	if (mExists)
		return nullptr;
	else
	{
		CameraList* cl = new CameraList(debugOutput);

		if (!cl)
			return nullptr;

		if (!cl->initialise())
		{
			delete cl;
			return nullptr;
		}

		mExists = true;
		mInstance = cl;
		return cl;
	}
}

CameraList* CameraList::instance()
{
	return mInstance;
}

Camera * CameraList::activeCamera()
{
	return mActiveCamera;
}

CameraList::~CameraList()
{
	CCINFORM("Shutting down CameraList.");

	cameras.clear();
	mInstance = nullptr;

	WARN_EDS_ERROR(EdsTerminateSDK(), "Error shutting down the SDK");
}

bool CameraList::initialise()
{
	CHECK_EDS_ERROR(EdsInitializeSDK(), "Could not initialise the Canon SDK", false);
	return true;
}

void CameraList::activeCamera(Camera * cam)
{
	mActiveCamera = cam;
}


int CameraList::ennumerate()
{
	CCINFORM("Ennumerating cameras.");

	EdsCameraListRef cameraList = NULL;
	CHECK_EDS_ERROR(EdsGetCameraList(&cameraList), "Could not populate the Canon camera list", -1);

	EdsUInt32 childCount;
	CHECK_EDS_ERROR(EdsGetChildCount(cameraList, &childCount), "Could not determine the number of cameras online", -1);

	cameras.reserve(childCount);

	for (unsigned iCamera = 0; iCamera < childCount; ++iCamera)
	{
		EdsCameraRef camera;
		CHECK_EDS_ERROR(EdsGetChildAtIndex(cameraList, iCamera, &camera),
			std::string("Could not retrieve camera [") + ToString(iCamera) + "]", -1);

		EdsDeviceInfo* info = new EdsDeviceInfo();
		CHECK_EDS_ERROR(EdsGetDeviceInfo(camera, info), "Could not get camera info", -1);

		cameras.emplace_back(mInformOutput, camera, info);
	}

	return childCount;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Camera::Camera(bool debugOutput, EdsCameraRef ref, EdsDeviceInfo* info)
{
	mInformOutput = debugOutput;
	mDeviceInfo = info;
	mCameraRef = ref;

	CCINFORM(std::string("Creating Camera: ") + name());
}

Camera::~Camera()
{
	deselect();
	if (mDeviceInfo)
		delete mDeviceInfo;
}

bool Camera::available()
{
	return true;
}

bool Camera::select()
{
	CCINFORM(std::string("Selecting camera ") + name());

	//Lock UI
	EdsSendStatusCommand(mCameraRef, kEdsCameraStatusCommand_UILock, 0);

	//Create session
	CHECK_EDS_ERROR(EdsOpenSession(mCameraRef), "Could not open camera session", false);

	//Register selection

	//if no CameraList instance
	CameraList* cameraList = CameraList::instance();
	if (cameraList == nullptr)
	{
		Error("Can not select a camera with no valid CameraList.");
		return false;
	}

	//Get Current active camera
	
	Camera* activeCamera = cameraList->activeCamera();

	//Deselect any previous cameras.
	if (activeCamera != nullptr)
		activeCamera->deselect();

	//Make change
	cameraList->activeCamera(this);

	return true;
}

void Camera::deselect()
{
	CCINFORM(std::string("Deselecting camera ") + name());

	//if no CameraList instance
	CameraList* cameraList = CameraList::instance();
	if (cameraList == nullptr)
	{
		Error("Can not deselect a camera with no valid CameraList.");
		return;
	}

	//Deselect
	Camera* activeCamera = cameraList->activeCamera();
	if (activeCamera == this)
	{
		//Close session
		WARN_EDS_ERROR(EdsCloseSession(mCameraRef), "Could not close camera session");

		//Unlock UI
		EdsSendStatusCommand(mCameraRef, kEdsCameraStatusCommand_UIUnLock, 0);

		//Reset current active camera
		cameraList->activeCamera(nullptr);
	}
}

std::string Camera::name()
{
	return mDeviceInfo->szDeviceDescription;
}

bool Camera::iso(IsoValue v)
{
	CCINFORM(std::string("Setting iso value ") + ToHexString(v) + " for " + name());
	return true;
}

Camera::IsoValue Camera::iso()
{
	EdsDataType dataType;
	EdsUInt32 dataSize;
	CHECK_EDS_ERROR(EdsGetPropertySize(mCameraRef, kEdsPropID_Tv, 0, &dataType, &dataSize), "Could not get propery size", ISO_INVALID);

	CCINFORM(std::string("Retrieving iso value ") + "not implemented" + " for " + name());
}

std::vector<Camera::IsoValue> Camera::isoValues()
{
	CCINFORM("Ennumerating iso values.");

	EdsPropertyDesc desc;
	CHECK_EDS_ERROR(EdsGetPropertyDesc(mCameraRef, kEdsPropID_ISOSpeed, &desc), "Could not retrieve Iso propery desc", {});

	std::vector<IsoValue> out;
	out.reserve(desc.numElements);

	for (int i = 0; i < desc.numElements; ++i)
		out.push_back((IsoValue)desc.propDesc[i]);

	return out;
}


bool Camera::shutterSpeed(int v)
{
	CCINFORM(std::string("Setting shutter speed ") + ToHexString(v) + " for " + name());
	return true;
}

int Camera::shutterSpeed()
{
	CCINFORM(std::string("Retrieving shutter speed ") + "not implemented" + " for " + name());
	return 0;
}

bool Camera::aperture(int v)
{
	CCINFORM(std::string("Setting aperture value ") + ToHexString(v) + " for " + name());
	return true;
}

int Camera::aperture()
{
	CCINFORM(std::string("Retrieving aperture value ") + "not implemented" + " for " + name());
	return 0;
}

std::vector<Camera::ApertureValue> Camera::apertureValues()
{
	CCINFORM("Ennumerating aperture values.");

	EdsPropertyDesc desc;
	CHECK_EDS_ERROR(EdsGetPropertyDesc(mCameraRef, kEdsPropID_Av, &desc), "Could not retrieve Av propery desc", {});

	std::vector<ApertureValue> out;
	out.reserve(desc.numElements);

	for (int i = 0; i < desc.numElements; ++i)
		out.push_back((ApertureValue)desc.propDesc[i]);

	return out;
}

bool Camera::focus(int v)
{
	CCINFORM(std::string("Setting focus value ") + ToString(v) + " for " + name());

	return true;
}

int Camera::focus()
{
	CCINFORM(std::string("Retrieving aperture value ") + "not implemented" + " for " + name());
	return 0;
}

bool Camera::shoot(const std::string& name, const std::string& directory)
{
	CCINFORM(std::string("Taking picture to \"") + directory+"\\"+name+ "\" for " + this->name());
	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////