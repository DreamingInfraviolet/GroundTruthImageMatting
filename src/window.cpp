#include "window.h"
#include "io.h"
#include "camera.h"
#include <memory>
#include <qcolordialog.h>
#include "camera.h"
#include "openglbox.h"
#include <sstream>
#include "actionclass.h"


Window::Window() {}

bool Window::initialise()
{
	ui.setupUi(this);

	connect(ui.ButtonAdd, SIGNAL(pressed()), this, SLOT(buttonAddEvent()));
	connect(ui.ButtonRemove, SIGNAL(pressed()), this, SLOT(buttonRemoveEvent()));
	connect(ui.ButtonUp, SIGNAL(pressed()), this, SLOT(buttonUpEvent()));
	connect(ui.ButtonDown, SIGNAL(pressed()), this, SLOT(buttonDownEvent()));

	connect(ui.BoxIso, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIsoEvent(int)));
	connect(ui.BoxAperture, SIGNAL(currentIndexChanged(int)), this, SLOT(changeApertureEvent(int)));
	connect(ui.BoxShutter, SIGNAL(currentIndexChanged(int)), this, SLOT(changeShutterEvent(int)));

	connect(ui.ButtonGo, SIGNAL(pressed()), this, SLOT(shootEvent()));


	ui.BoxProcessedformat->addItems({ "tiff", "bmp", "jpg", "png" });

	mColourModel = std::unique_ptr<QStringListModel>(new QStringListModel(this));
	ui.ListColours->setModel(mColourModel.get());
	ui.ListColours->setEditTriggers(QAbstractItemView::NoEditTriggers);

	
	if (!(mActionClass = std::unique_ptr<ActionClass>(ActionClass::create())).get())
		return false;

	return true;
}


Window* Window::create()
{
	Window* out = new Window();
	if (!out)
		return nullptr;
	if (!out->initialise())
	{
		delete out;
		return nullptr;
	}
	else
	{
		return out;
	}
}

Window::~Window(){}

void Window::buttonAddEvent()
{
	QColor colour = QColorDialog::getColor(Qt::white);
	if (!colour.isValid())
		return;

	mColourModel->setStringList(mColourModel->stringList() <<colour.name());
}

void Window::buttonRemoveEvent()
{
	auto removeIndex = ui.ListColours->currentIndex();
	if (removeIndex.isValid())
		mColourModel->removeRow(removeIndex.row());
}
void Window::buttonUpEvent()
{
	auto currentIndex = ui.ListColours->currentIndex();
	if (!currentIndex.isValid())
		return;

	if (currentIndex.row() > 0)
	{
		auto list = mColourModel->stringList();
		list.move(currentIndex.row(), currentIndex.row() - 1);
		mColourModel->setStringList(list);
		ui.ListColours->setCurrentIndex(mColourModel->index(currentIndex.row() - 1));
	}
}

void Window::buttonDownEvent()
{
	auto currentIndex = ui.ListColours->currentIndex();
	if (!currentIndex.isValid())
		return;

	auto list = mColourModel->stringList();
	if (currentIndex.row() >= list.size()-1)
		return;

	list.move(currentIndex.row(), currentIndex.row() + 1);
	mColourModel->setStringList(list);
	
	ui.ListColours->setCurrentIndex(mColourModel->index(currentIndex.row() + 1));
}

void Window::changeIsoEvent(int value)
{
	mActionClass->iso(value);
}

void Window::changeApertureEvent(int value)
{
	mActionClass->aperture();
}

void Window::changeShutterEvent(int value)
{
	mActionClass->shutter();
}


void Window::shootEvent()
{
	//Gather information
	int delay = ui.SpinDelay->value();
	auto startTime = std::chrono::steady_clock::now() + std::chrono::seconds(delay);

	bool saveProcessed = ui.CheckSaveProcessed->isChecked();
	bool saveRaw = ui.CheckSaveRaw->isChecked();

	std::string processedExtension = ui.BoxProcessedformat->itemText(ui.BoxProcessedformat->currentIndex()).toUtf8();

	Inform(std::string("Shooting with saveProcessed=") + (saveProcessed ? "TRUE" : "FALSE") +
		" saveRaw=" + (saveRaw ? "TRUE" : "FALSE") + " delay=" + ToString(delay) +
		" extension=" + processedExtension);

	QStringList colours = mColourModel->stringList();

	//Disable QT window
	
	disableEvents();

	mActionClass->shootSequence();

	enableEvents();

	
}

void Window::disableEvents()
{
	this->installEventFilter(&mFilter);
}

void Window::enableEvents()
{
	this->installEventFilter(this);
}