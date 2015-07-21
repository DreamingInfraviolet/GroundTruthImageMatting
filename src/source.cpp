#include "camera.h"
#include "io.h"

#include <iostream>
#include <memory>

int main(int argc, char** argv)
{
	std::auto_ptr<CameraList> list(CameraList::create(true));

	if (list.get() == nullptr)
	{
		Error("Could not initialise camera SDK.");
		return 1;
	}

	if (list->ennumerate() == 0)
	{
		Error("No cameras found.");
		return 2;
	}

	std::cout << "Found " << list->cameras.size() << " cameras.\n";

	if (list->cameras.size() > 0)
	{
		Camera& cam = list->cameras[0];
		cam.select();

		/*
		auto iso = cam.ennumeratePossibleValues(Camera::EnnumerableProperties::ISO);
		std::cout << "Found " << iso.size() << " iso values: ";
		for (auto i : iso)
			std::cout << CameraList::isoMappings[i] << " ";
		std::cout << "\n\n\n";

		auto ape = cam.ennumeratePossibleValues(Camera::EnnumerableProperties::Aperture);
		std::cout << "Found " << ape.size() << " aperture values: ";
		for (auto i : ape)
			std::cout << CameraList::apertureMappings[i] << " ";
		std::cout << "\n";

		auto shu = cam.ennumeratePossibleValues(Camera::EnnumerableProperties::ShutterSpeed);
		std::cout << "Found " << shu.size() << " shutter speed values: ";
		for (auto i : shu)
			std::cout << CameraList::shutterSpeedMappings[i] << " ";
		std::cout << "\n";
		*/

		/*
		Inform("Testing iso.");
		cam.iso();
		cam.iso(0x00000055);
		cam.iso();

		Inform("Testing aperture.");
		cam.aperture();
		cam.aperture(0x2D);
		cam.aperture();

		Inform("Testing shutter speed.");
		cam.shutterSpeed();
		cam.shutterSpeed(0x38);
		cam.shutterSpeed();
		*/

		cam.shoot("shot.raw", "captured");

		cam.deselect();
	}

	return 0;
}