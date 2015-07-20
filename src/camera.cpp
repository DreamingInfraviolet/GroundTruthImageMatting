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


bool		CameraList::mExists = false;
CameraList* CameraList::mInstance = nullptr;
Camera*		CameraList::mActiveCamera = nullptr;

CameraList* CameraList::create()
{
	if (mExists)
		return nullptr;
	else
	{
		CameraList* cl = new CameraList();

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
	EdsCameraListRef cameraList = NULL;
	CHECK_EDS_ERROR(EdsGetCameraList(&cameraList), "Could not populate the Canon camera list", -1);

	EdsUInt32 childCount;
	CHECK_EDS_ERROR(EdsGetChildCount(cameraList, &childCount), "Could not determine the number of cameras online", -1);

	cameras.resize(childCount);

	for (unsigned iCamera = 0; iCamera < childCount; ++iCamera)
	{
		EdsCameraRef camera;
		CHECK_EDS_ERROR(EdsGetChildAtIndex(cameraList, iCamera, &camera),
			std::string("Could not retrieve camera [") + ToString(iCamera) + "]", -1);

		cameras[iCamera].mCameraRef = camera;
		CHECK_EDS_ERROR(EdsGetDeviceInfo(camera, &cameras[iCamera].mDeviceInfo), "Could not get camera info", -1);

	}


	return childCount;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////



bool Camera::available()
{
	return true;
}

bool Camera::select()
{
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
	return mDeviceInfo.szDeviceDescription;
}

bool Camera::iso(IsoValue v)
{
	return true;
}

Camera::IsoValue Camera::iso()
{
	EdsDataType dataType;
	EdsUInt32 dataSize;
	CHECK_EDS_ERROR(EdsGetPropertySize(mCameraRef, kEdsPropID_Tv, 0, &dataType, &dataSize), "Could not get propery size", ISO_INVALID);


}

std::vector<Camera::IsoValue> Camera::isoValues()
{
	EdsPropertyDesc desc;
	CHECK_EDS_ERROR(EdsGetPropertyDesc(mCameraRef, kEdsPropID_ISOSpeed, &desc), "Could not retrieve Iso propery desc", {});

	std::vector<IsoValue> out;
	out.reserve(desc.numElements);

	for (size_t i = 0; i < desc.numElements; ++i)
		out.push_back((IsoValue)desc.propDesc[i]);

	return out;
}


bool Camera::shutterSpeed(int v)
{
	return true;
}

int Camera::shutterSpeed()
{
	return 0;
}

int Camera::maxShutterSpeed()
{
	return 0;
}

int Camera::minShutterSpeed()
{
	return 0;
}

bool Camera::aperture(int v)
{
	return true;
}

int Camera::aperture()
{
	return 0;
}

std::vector<Camera::ApertureValue> Camera::apertureValues()
{
	EdsPropertyDesc desc;
	CHECK_EDS_ERROR(EdsGetPropertyDesc(mCameraRef, kEdsPropID_Av, &desc), "Could not retrieve Av propery desc", {});

	std::vector<ApertureValue> out;
	out.reserve(desc.numElements);

	for (size_t i = 0; i < desc.numElements; ++i)
		out.push_back((ApertureValue)desc.propDesc[i]);

	return out;
}

bool Camera::focus(int value)
{
	return true;
}

int Camera::focus()
{
	return 0;
}

bool Camera::shoot(const std::string& name, const std::string& directory)
{
	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////