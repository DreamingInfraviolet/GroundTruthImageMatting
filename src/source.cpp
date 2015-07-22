#include "camera.h"
#include "io.h"

#include <iostream>
#include <memory>

//Temporary
#include <Windows.h>
#include <thread>

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

		cam.shoot("shot.cr2", "C:\\Anima\\ImageBackgroundRemoval\\build\\src\\captured");

		while (true)
		{
			MSG Msg;
			while (PeekMessage(&Msg,NULL,0,0,1))
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		}

		cam.deselect();

	}

	return 0;
}