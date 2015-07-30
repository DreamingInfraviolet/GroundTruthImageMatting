#include <cassert>
#include <memory>
#include <ctime>
#include <cstdio>
#include "EDSDK.h"
#include "camera.h"
#include "io.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Definition of basic static types
CameraList* CameraList::mInstance = nullptr;
Camera*		CameraList::mActiveCamera = nullptr;

//Definition of property mappings
const PropertyMap Camera::isoMappings =
{
	{ 0x00000028, "ISO 6" },
	{ 0x00000030, "ISO 12" },
	{ 0x00000038, "ISO 25" },
	{ 0x00000040, "ISO 50" },
	{ 0x00000048, "ISO 100" },
	{ 0x0000004b, "ISO 125" },
	{ 0x0000004d, "ISO 160" },
	{ 0x00000050, "ISO 200" },
	{ 0x00000053, "ISO 250" },
	{ 0x00000055, "ISO 320" },
	{ 0x00000058, "ISO 400" },
	{ 0x0000005b, "ISO 500" },
	{ 0x0000005d, "ISO 640" },
	{ 0x00000060, "ISO 800" },
	{ 0x00000063, "ISO 1000" },
	{ 0x00000065, "ISO 1250" },
	{ 0x00000068, "ISO 1600" },
	{ 0x00000070, "ISO 3200" },
	{ 0x00000078, "ISO 6400" },
	{ 0x00000080, "ISO 12800" },
	{ 0x00000088, "ISO 25600" },
	{ 0xffffffff, "Invalid" }
};

const PropertyMap Camera::apertureMappings =
{
	{ 0x08, "1" },
	{ 0x0B, "1.1" },
	{ 0x0C, "1.2" },
	{ 0x0D, "1.2 (1/3)" },
	{ 0x10, "1.4" },
	{ 0x13, "1.6" },
	{ 0x14, "1.8" },
	{ 0x40, "11" },
	{ 0x43, "13 (1/3)" },
	{ 0x44, "13" },
	{ 0x45, "14" },
	{ 0x48, "16" },
	{ 0x4B, "18" },
	{ 0x4C, "19" },
	{ 0x15, "1.8 (1/3)" },
	{ 0x18, "2" },
	{ 0x1B, "2.2" },
	{ 0x1C, "2.5" },
	{ 0x1D, "2.5 (1/3)" },
	{ 0x20, "2.8" },
	{ 0x23, "3.2" },
	{ 0x24, "3.5" },
	{ 0x25, "3.5 (1/3)" },
	{ 0x28, "4" },
	{ 0x2B, "4.5" },
	{ 0x2C, "4.5 (1/3)" },   //guess - docs claimed "4.5"
	{ 0x2D, "5.0" },
	{ 0x30, "5.6" },
	{ 0x33, "6.3" },
	{ 0x34, "6.7" },
	{ 0x35, "7.1" },
	{ 0x38, "8" },
	{ 0x3B, "9" },
	{ 0x3C, "9.5" },
	{ 0x3D, "10" },
	{ 0x4D, "20" },
	{ 0x50, "22" },
	{ 0x53, "25" },
	{ 0x54, "27" },
	{ 0x55, "29" },
	{ 0x58, "32" },
	{ 0x5B, "36" },
	{ 0x5C, "38" },
	{ 0x5D, "40" },
	{ 0x60, "45" },
	{ 0x63, "51" },
	{ 0x64, "54" },
	{ 0x65, "57" },
	{ 0x68, "64" },
	{ 0x6B, "72" },
	{ 0x6C, "76" },
	{ 0x6D, "80" },
	{ 0x70, "91" },
	{ 0xffffffff, "Invalid" }
};


const PropertyMap Camera::shutterSpeedMappings =
{
	{ 0x0C, "Bulb" },
	{ 0x10, "30\"" },
	{ 0x13, "25\"" },
	{ 0x14, "20\"" },
	{ 0x15, "20\" (1/3)" },
	{ 0x18, "15\"" },
	{ 0x1B, "13\"" },
	{ 0x5D, "1/25" },
	{ 0x60, "1/30" },
	{ 0x63, "1/40" },
	{ 0x64, "1/45" },
	{ 0x65, "1/50" },
	{ 0x68, "1/60" },
	{ 0x6B, "1/80" },
	{ 0x1C, "10\"" },
	{ 0x1D, "10\" (1/3)" },
	{ 0x20, "8\"" },
	{ 0x23, "6\" (1/3)" },
	{ 0x24, "6\"" },
	{ 0x25, "5\"" },
	{ 0x28, "4\"" },
	{ 0x2B, "3\"2" },
	{ 0x2C, "3\"" },
	{ 0x2D, "2\"5" },
	{ 0x30, "2\"" },
	{ 0x33, "1\"6" },
	{ 0x34, "1\"5" },
	{ 0x35, "1\"3" },
	{ 0x38, "1\"" },
	{ 0x3B, "0\"8" },
	{ 0x3C, "0\"7" },
	{ 0x3D, "0\"6" },
	{ 0x40, "0\"5" },
	{ 0x43, "0\"4" },
	{ 0x44, "0\"3" },
	{ 0x45, "0\"3 (1/3)" },
	{ 0x48, "1/4" },
	{ 0x4B, "1/5" },
	{ 0x4C, "1/6" },
	{ 0x4D, "1/6 (1/3)" },
	{ 0x50, "1/8" },
	{ 0x53, "1/10 (1/3)" },
	{ 0x54, "1/10" },
	{ 0x55, "1/13" },
	{ 0x58, "1/15" },
	{ 0x5B, "1/20 (1/3)" },
	{ 0x5C, "1/20" },
	{ 0x6C, "1/90" },
	{ 0x6D, "1/100" },
	{ 0x70, "1/125" },
	{ 0x73, "1/160" },
	{ 0x74, "1/180" },
	{ 0x75, "1/200" },
	{ 0x78, "1/250" },
	{ 0x7B, "1/320" },
	{ 0x7C, "1/350" },
	{ 0x7D, "1/400" },
	{ 0x80, "1/500" },
	{ 0x83, "1/640" },
	{ 0x84, "1/750" },
	{ 0x85, "1/800" },
	{ 0x88, "1/1000" },
	{ 0x8B, "1/1250" },
	{ 0x8C, "1/1500" },
	{ 0x8D, "1/1600" },
	{ 0x90, "1/2000" },
	{ 0x93, "1/2500" },
	{ 0x94, "1/3000" },
	{ 0x95, "1/3200" },
	{ 0x98, "1/4000" },
	{ 0x9B, "1/5000" },
	{ 0x9C, "1/6000" },
	{ 0x9D, "1/6400" },
	{ 0xA0, "1/8000" },
	{ 0xffffffff, "Invalid" }
};




CameraList::CameraList()
{
	Inform("Creating camera list");
}

CameraList* CameraList::create()
{
	if (mInstance != nullptr)
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
	Inform("Shutting down CameraList.");

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
	Inform("Ennumerating cameras.");

	EdsCameraListRef cameraList = NULL;
	CHECK_EDS_ERROR(EdsGetCameraList(&cameraList), "Could not populate the Canon camera list", -1);

	EdsUInt32 childCount;
	CHECK_EDS_ERROR(EdsGetChildCount(cameraList, &childCount), "Could not determine the number of cameras online", -1);

	//cameras.reserve(childCount);

	for (unsigned iCamera = 0; iCamera < childCount; ++iCamera)
	{
		//Get basic camera info
		EdsCameraRef camera;
		CHECK_EDS_ERROR(EdsGetChildAtIndex(cameraList, iCamera, &camera),
			std::string("Could not retrieve camera [") + ToString(iCamera) + "]", -1);

		EdsDeviceInfo* info = new EdsDeviceInfo();
		CHECK_EDS_ERROR_ACT(EdsGetDeviceInfo(camera, info), "Could not get camera info", -1,
			delete info;);

		//Create camera object
		cameras.emplace_back(camera, info);

		//Register object event handler for camera
		CHECK_EDS_ERROR(EdsSetObjectEventHandler(
			camera, kEdsObjectEvent_All,
			&Camera::objectCallback,
			&cameras.back()), "Could not set photo download event handler", -1);

		Inform(std::string("Found Camera ") + cameras.back().name());

	}

	return childCount;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Camera::Camera(EdsCameraRef ref, EdsDeviceInfo* info)
{
	mDeviceInfo = info;
	mCameraRef = ref;

	mReadyToTakePhoto = new std::atomic<bool>();
	*mReadyToTakePhoto = true;
}


Camera::~Camera()
{
	deselect();
	if (mReadyToTakePhoto)
		delete mReadyToTakePhoto;
	if (mDeviceInfo)
		delete mDeviceInfo;
	EdsRelease(mCameraRef);
}

bool Camera::readyToShoot()
{
	return *mReadyToTakePhoto;
}

bool Camera::select()
{
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

	Inform(std::string("Selecting camera ") + name());

	//Lock UI
	EdsSendStatusCommand(mCameraRef, kEdsCameraStatusCommand_UILock, 0);

	//Create session
	CHECK_EDS_ERROR(EdsOpenSession(mCameraRef), "Could not open camera session", false);

	//Start live stream
	int pc = kEdsEvfOutputDevice_PC;
	CHECK_EDS_ERROR(EdsSetPropertyData(mCameraRef, kEdsPropID_Evf_OutputDevice, 0,
									sizeof(kEdsEvfOutputDevice_PC),
									&pc), "could not start live stream", false);

	//Register selection
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
		Inform(std::string("Deselecting camera ") + name());

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


std::vector<int> Camera::ennumeratePossibleValues(EnnumerableProperties ep)
{
	int propertyCode = 0;

	switch (ep)
	{
	case EnnumerableProperties::Aperture:
		Inform("Ennumerating aperture values.");
		propertyCode = kEdsPropID_Av;
		break;
	case EnnumerableProperties::ISO:
		Inform("Ennumerating iso values.");
		propertyCode = kEdsPropID_ISOSpeed;
		break;
	case EnnumerableProperties::ShutterSpeed:
		Inform("Ennumerating shutter values.");
		propertyCode = kEdsPropID_Tv;
		break;
	default:
		assert(false);
	}

	EdsPropertyDesc desc;
	CHECK_EDS_ERROR(EdsGetPropertyDesc(mCameraRef, propertyCode, &desc), "Could not retrieve propery description", {});

	std::vector<int> out;
	out.reserve(desc.numElements);

	for (int i = 0; i < desc.numElements; ++i)
		if(desc.propDesc[i]!=0)
			out.push_back(desc.propDesc[i]);

	return out;
}

bool Camera::iso(int v)
{
	Inform(std::string("Setting iso value ") + ToHexString(v) + " for " + name());
	CHECK_EDS_ERROR(EdsSetPropertyData(mCameraRef, kEdsPropID_ISOSpeed, 0, sizeof(EdsInt32), &v), "Could not set iso property.", false);
	return true;
}

int Camera::iso()
{
	int v;
	CHECK_EDS_ERROR(EdsGetPropertyData(mCameraRef, kEdsPropID_ISOSpeed, 0, sizeof(EdsInt32), &v), "Could not get iso property.", -1);
	Inform(std::string("Retrieving iso value ") + ToHexString(v) + " for " + name());
	return v;
}

bool Camera::shutterSpeed(int v)
{
	Inform(std::string("Setting shutter speed ") + ToString(v) + " for " + name());
	CHECK_EDS_ERROR(EdsSetPropertyData(mCameraRef, kEdsPropID_Tv, 0, sizeof(EdsInt32), &v), "Could not set shutter speed property.", false);
	return true;
}

int Camera::shutterSpeed()
{
	int v;
	CHECK_EDS_ERROR(EdsGetPropertyData(mCameraRef, kEdsPropID_Tv, 0, sizeof(EdsInt32), &v), "Could not get shutter speed property.", -1);
	Inform(std::string("Retrieving shutter speed ") + ToString(v) + " for " + name());
	return v;
}


bool Camera::aperture(int v)
{
	Inform(std::string("Setting aperture value ") + ToString(v) + " for " + name());
	CHECK_EDS_ERROR(EdsSetPropertyData(mCameraRef, kEdsPropID_Av, 0, sizeof(EdsInt32), &v), "Could not set aperture property.", false);
	return true;
}

int Camera::aperture()
{
	int v;
	CHECK_EDS_ERROR(EdsGetPropertyData(mCameraRef, kEdsPropID_Av, 0, sizeof(EdsInt32), &v), "Could not get aperture property.", -1);
	Inform(std::string("Retrieving aperture value ") + ToString(v) + " for " + name());
	return v;
}

bool Camera::shoot()
{	
	Inform("Shooting picture");

	//While we are having message loop issues, just return false for now if not ready:
	if (!*mReadyToTakePhoto)
	{
		Error("Camera not ready to shoot");
		return false;
	}

	//Set shooting mode:
	EdsInt32 shootingMode = 0;
	CHECK_EDS_ERROR(EdsSetPropertyData(mCameraRef, kEdsPropID_DriveMode, 0, sizeof(EdsInt32), &shootingMode),
		"Could not set shooting mode to single shot.", false);

	//Set RAW format
	EdsInt32 rawMode = 0x00640f0f;
	CHECK_EDS_ERROR(EdsSetPropertyData(mCameraRef, kEdsPropID_ImageQuality, 0, sizeof(EdsInt32), &rawMode),
		"Could not get the camera quality information.", false);

	//Set save to computer
	EdsInt32 saveToComputer = kEdsSaveTo_Host;
	CHECK_EDS_ERROR(EdsSetPropertyData(mCameraRef, kEdsPropID_SaveTo, 0, sizeof(EdsInt32), &saveToComputer),
		"Could not set camera save mode.", false);

	//Send shoot command and force further shoot commands to wait
	*mReadyToTakePhoto = false;
	CHECK_EDS_ERROR_ACT(EdsSendCommand(mCameraRef, kEdsCameraCommand_TakePicture, 0), "Could not capture an image", false,
		*mReadyToTakePhoto = true;);

	//Now the user must wait for the object callback to download the image.

	return true;
}

ImageRaw Camera::retrieveLastImage()
{
	ImageRaw img = std::move(mLastImage);
	mLastImage.fail();
	return img;
}

bool Camera::resetShutdownTimer()
{
	CHECK_EDS_ERROR(EdsSendCommand(mCameraRef, kEdsCameraCommand_ExtendShutDownTimer, 0),
		"Could not reset shutdown timer", false);
	return true;
}

EdsError EDSCALLBACK Camera::objectCallback(EdsObjectEvent inEvent, EdsBaseRef inRef, EdsVoid * inContext)
{
	Camera* camera = (Camera*)inContext;

	switch (inEvent)
	{
	case kEdsObjectEvent_DirItemRequestTransfer:
	{
		//Note: Other switch cases may not be taking a photo

		//If ready to take another photo, something is wrong . . . Most likely a photo from the previous camera session.
		if (*camera->mReadyToTakePhoto)
		{
			//Note: This returns the error code
			CHECK_EDS_ERROR(EdsDownloadCancel(inRef), "Could not cancel download request ", err);
			return EDS_ERR_OK;
		}

		Inform("Receiving camera download event ");

		//Get info on camera memory directory
		EdsDirectoryItemInfo dii;
		CHECK_EDS_ERROR_ACT(EdsGetDirectoryItemInfo(inRef, &dii), "Could not retrieve directory item info", err,
			EdsDownloadCancel(inRef); *camera->mReadyToTakePhoto = true;);

		//Create stream
		EdsStreamRef stream = nullptr;
		CHECK_EDS_ERROR_ACT(EdsCreateMemoryStream(dii.size, &stream), "Failed to create image stream", err,
			EdsDownloadCancel(inRef); *camera->mReadyToTakePhoto = true;);

		//Download
		CHECK_EDS_ERROR_ACT(EdsDownload(inRef, dii.size, stream), "Could not download image", err,
			EdsRelease(stream); EdsDownloadCancel(inRef); *camera->mReadyToTakePhoto = true;);
		CHECK_EDS_ERROR_ACT(EdsDownloadComplete(inRef), "Could not send download success confirmation", err,
			EdsRelease(stream); EdsDownloadCancel(inRef); *camera->mReadyToTakePhoto = true;);

		//Get image and information about it
		EdsImageRef image;
		CHECK_EDS_ERROR_ACT(EdsCreateImageRef(stream, &image), "Could not retrieve image ref", err,
			EdsRelease(stream); *camera->mReadyToTakePhoto = true;);

		EdsImageInfo imageInfo;
		CHECK_EDS_ERROR_ACT(EdsGetImageInfo(image, kEdsImageSrc_RAWFullView, &imageInfo),
			"Could not retrieve image info", err,
			EdsRelease(stream); EdsRelease(image); *camera->mReadyToTakePhoto = true;);

		void* imageData;
		CHECK_EDS_ERROR_ACT(EdsGetPointer(stream, &imageData), "Could not retrieve the image pointer", err,
			EdsRelease(stream); EdsRelease(image); *camera->mReadyToTakePhoto = true;);

		ImageRaw img = ImageRaw(image, imageData, dii.size, imageInfo.width, imageInfo.height);
		camera->mLastImage = std::move(img);

		EdsRelease(stream);
		
		//Notify camera that it can take photos again, and notify a waiting thread, if any.
		*camera->mReadyToTakePhoto = true;

		Inform("Image ready");
	}
	break;
	}

	return EDS_ERR_OK;
}

std::vector<unsigned char> Camera::getLiveImage()
{

	EdsStreamRef stream;
	CHECK_EDS_ERROR(EdsCreateMemoryStream(0, &stream), "Could not create stream", {});

	EdsEvfImageRef image;
	CHECK_EDS_ERROR_ACT(EdsCreateEvfImageRef(stream, &image), "Could not create image reference", {},
		EdsRelease(stream));


	int err = EdsDownloadEvfImage(mCameraRef, image);
	if (err != EDS_ERR_OK)
	{
		//If it's not ready, it is not unexpected behaviour.
		if (err != EDS_ERR_OBJECT_NOTREADY)
			Error("Could not download live stream");
		EdsRelease(stream);
		EdsRelease(image);
		return{};
	}

	void* data;
	CHECK_EDS_ERROR_ACT(EdsGetPointer(stream, (EdsVoid**)& data), "Could not get image pointer", {},
		EdsRelease(stream); EdsRelease(image););

	EdsUInt32 dataLen;
	CHECK_EDS_ERROR_ACT(EdsGetLength(stream, &dataLen), "Could not get stream size", {},
		EdsRelease(stream); EdsRelease(image););

	std::vector<unsigned char> out;
	out.resize(dataLen);
	memcpy(&out[0], data, dataLen);

	EdsRelease(stream);
	EdsRelease(image);

	return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////