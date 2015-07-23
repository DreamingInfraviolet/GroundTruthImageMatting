#include "camera.h"
#include "io.h"

#include <iostream>
#include <memory>

#include<nana/gui.hpp>

int main()
{
	using namespace nana;
	form fm;
	drawing{ fm }.draw([](paint::graphics& graph) {
		graph.string({ 10, 10 }, L"Hello, world!", colors::red);
	});
	fm.events().click(API::exit);
	fm.show();
	exec();
}


/*




//Temporary
#include <Windows.h>

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

		cam.shoot();

		while (!cam.readyToShoot())
		{
			MSG Msg;
			while (PeekMessage(&Msg,NULL,0,0,1))
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		}

		Inform("Processing raw");
		auto image = cam.retrieveLastImage();
		Inform("Saving raw");
		image.saveToFile("C:\\Anima\\ImageBackgroundRemoval\\build\\src\\captured\\shot.cr2");
		Inform("Processing tga");
		auto imageRgba = image.asRGBA();
		Inform("Saving tga");
		imageRgba.saveToFile("C:\\Anima\\ImageBackgroundRemoval\\build\\src\\captured\\shot.tga");

		cam.deselect();

	}

	return 0;
}

*/