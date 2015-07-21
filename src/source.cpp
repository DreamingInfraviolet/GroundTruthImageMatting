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

	for (Camera& cam : list->cameras)
	{
		cam.select();
		std::cout << cam.name()<<"\n";

		/*
		cam.iso(Camera::ISO_100);
		cam.shutterSpeed(100);
		cam.aperture(10);
		cam.focus(20);
		cam.shoot("test.raw", "captured/test");
		*/
	}

	return 0;
}