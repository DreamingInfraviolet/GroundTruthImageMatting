#include "actionclass.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

bool ActionClass::initialiseCameraSystem()
{
	//Initialise camera system
	Inform("Initialising camera");
	mCameraList = std::unique_ptr<CameraList> (CameraList::create(true));

	if (!mCameraList.get())
		return false;

	if (mCameraList->ennumerate() == 0)
	{
		Error("No cameras found");
		return false;
	}

	Camera& mainCamera = mCameraList->cameras[0];
	if (!mainCamera.select())
	{
		Error("Could not select the main camera.");
		return false;
	}

	//Set selectable values
	std::vector<int> apList = mainCamera.ennumeratePossibleValues(Camera::EnnumerableProperties::Aperture);
	std::vector<int> isList = mainCamera.ennumeratePossibleValues(Camera::EnnumerableProperties::ISO);
	std::vector<int> shList = mainCamera.ennumeratePossibleValues(Camera::EnnumerableProperties::ShutterSpeed);

	int currentAp = mainCamera.aperture();
	int currentIs = mainCamera.iso();
	int currentSh = mainCamera.shutterSpeed();

	for (size_t i = 0; i < apList.size(); ++i)
	{
		ui.BoxAperture->addItem(Camera::apertureMappings[apList[i]].c_str());
		if (apList[i] == currentAp)
			ui.BoxAperture->setCurrentIndex(i);
	}

	for (size_t i = 0; i < isList.size(); ++i)
	{
		ui.BoxIso->addItem(Camera::isoMappings[isList[i]].c_str());
		if (isList[i] == currentIs)
			ui.BoxIso->setCurrentIndex(i);
	}

	for (size_t i = 0; i < shList.size(); ++i)
	{
		ui.BoxShutter->addItem(Camera::shutterSpeedMappings[shList[i]].c_str());
		if (shList[i] == currentSh)
			ui.BoxShutter->setCurrentIndex(i);
	}

	return true;
}