
#include <QtGui>
#include <QApplication>
#include "window.h"
#include <memory>

int main(int argc, char *argv[])
{
QApplication a(argc, argv);
a.setAttribute(Qt::AA_ShareOpenGLContexts);

std::unique_ptr<Window> w(Window::create());
if (w.get() == nullptr)
	return 1;


w->show();
return a.exec();

}


/*
#include "camera.h"
#include <memory>
#include "io.h"
#include <iostream>
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
			while (PeekMessage(&Msg, NULL, 0, 0, 1))
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
		Inform("Saving rgba");
		imageRgba.saveToFile("C:\\Anima\\ImageBackgroundRemoval\\build\\src\\captured\\shot.tiff");

		cam.deselect();

	}

	return 0;
}
*/

/*
int main()
{
	Window* window = Window::create(800, 600);
	if (window)
		return window->run();
	else
		return -1;
}
*/


/*
#include "camera.h"
#include "io.h"

#include <iostream>
#include <memory>



#include<nana/gui.hpp>

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/spinbox.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/checkbox.hpp>

int main()
{

	using namespace nana;
	form window(API::make_center(800,600));
	window.bgcolor(color(244,244,244));
	

	//Use this for delay
	//spinbox spbox(window, rectangle{ 10, 10, 100, 30 });
	//spbox.range(0, 100, 1); //Set the range of integers


	//Define colour box
	listbox colours(window, true);
	colours.size(nana::size(350,337));
	colours.auto_draw(true);
	colours.ordered_categories(false);
	colours.show_header(false);
	colours.append_header(STR("Colours"), 200);
	auto test = colours.append(STR("Colours"));
	test.append({ STR("Hi"),STR("Yo") });


	//Define colour control buttons
	button addColourButton(window);
	addColourButton.caption(STR("Add"));
	button removeColourButton(window, true);
	removeColourButton.caption("Remove");
	button upColourButton(window, true);
	upColourButton.caption("Down");
	button downColourButton(window, true);
	downColourButton.caption("Up");

	//Define iso control
	label isoLabel(window, true);
	combox isoBox(window, true);
	
	//Define shutterSpeed control
	label shutterLabel(window, true);
	combox shutterBox(window, true);

	//Define aperture control
	label apertureLabel(window, true);
	combox apertureBox(window, true);


	//Define go button
	button goButton(window, true);

	//Define timer spinbox
	spinbox timerSpinbox(window, true);

	//Define saving checkboxes
	checkbox saveBmpCheckbox(window, true);
	checkbox saveRawCheckbox(window, true);

	place mainPlace(window);
	mainPlace.div("");
	mainPlace.collocate();
	
	
	window.show();
	exec();
}
*/
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