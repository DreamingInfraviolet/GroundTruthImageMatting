#include "window.h"
#include "actionthread.h"
#include "io.h"
#include "camera.h"
#include <thread>
Window::Window(int argc, char** argv) {}

bool Window::initialise()
{
	ui.setupUi(this);

	connect(ui.ButtonAdd, SIGNAL(pressed()), this, SLOT(buttonAddEvent()));
	connect(ui.ButtonRemove, SIGNAL(pressed()), this, SLOT(buttonRemoveEvent()));
	connect(ui.ButtonUp, SIGNAL(pressed()), this, SLOT(buttonUpEvent()));
	connect(ui.ButtonDown, SIGNAL(pressed()), this, SLOT(buttonDownEvent()));

	ui.BoxProcessedformat->addItems({ "tiff", "bmp", "jpg", "png" });
	return true;
}

bool Window::initialiseCamera()
{

}

Window* Window::create(int argc, char** argv, EventSystem* system)
{
	Window* out = new Window(argc, argv);
	if (!out)
		return nullptr;
	if (!out->initialise())
	{
		delete out;
		return nullptr;
	}
	else
	{
		out->mEventSystem = system;
		return out;
	}
}

Window::~Window(){}

void Window::buttonAddEvent()
{
	while (true){ Inform("Pressed"); }
}

void Window::buttonRemoveEvent()
{
	while (true){ Inform("Pressed"); }
}
void Window::buttonUpEvent()
{
	while (true){ Inform("Pressed"); }
}

void Window::buttonDownEvent()
{
	while (true){ Inform("Pressed"); }
}
