#include "window.h"
#include "io.h"
#include <qcolordialog.h>
#include "actionclass.h"
#include <fstream>



Window* Window::sWindow = nullptr;

bool Window::initialise()
{
	Inform("Initialising window");

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

	ui.CheckSaveProcessed->setChecked(true);
	ui.CheckSaveRaw->setChecked(true);

	if (!(mActionClass = std::unique_ptr<ActionClass>(ActionClass::create())).get())
		return false;

	//Set selectable values
	std::vector<int> apList = mActionClass->ennumeratePossibleValues(Camera::EnnumerableProperties::Aperture);
	std::vector<int> isList = mActionClass->ennumeratePossibleValues(Camera::EnnumerableProperties::ISO);
	std::vector<int> shList = mActionClass->ennumeratePossibleValues(Camera::EnnumerableProperties::ShutterSpeed);

	int currentAp = mActionClass->aperture();
	int currentIs = mActionClass->iso();
	int currentSh = mActionClass->shutter();

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

	std::fstream colourFile("colours.txt", std::ios::in);
	if (colourFile.fail())
	{
		Warning("Could not open colours.txt - Colours will not be reloaded.");
	}
	else
	{
		//Read file
		std::string tmp;
		while (getline(colourFile, tmp))
		{
			QString qs(tmp.c_str());
			qs = qs.trimmed();

			if (qs.size() == 0)
				continue;

			QColor colour(qs);

			if (colour.isValid() == false)
			{
				Warning("Invalid colour found in colours.txt");
				continue;
			}
			else
				mColourModel->setStringList(mColourModel->stringList() << colour.name());
		}
	}

	Inform("Window ready");
	return true;
}

Window* Window::create()
{
	if (sWindow)
		return nullptr;
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
		out->sWindow = out;
		return out;
	}
}

Window* Window::instance() { return sWindow; }

Window::~Window() { sWindow = nullptr; }

void Window::buttonAddEvent()
{
	QColor colour = QColorDialog::getColor(Qt::white);
	if (!colour.isValid())
		return;

	mColourModel->setStringList(mColourModel->stringList() << colour.name());

	updateColourFile();
}

void Window::buttonRemoveEvent()
{
	auto removeIndex = ui.ListColours->currentIndex();
	if (removeIndex.isValid())
		mColourModel->removeRow(removeIndex.row());

	updateColourFile();
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

	updateColourFile();
}

void Window::buttonDownEvent()
{
	auto currentIndex = ui.ListColours->currentIndex();
	if (!currentIndex.isValid())
		return;

	auto list = mColourModel->stringList();
	if (currentIndex.row() >= list.size() - 1)
		return;

	list.move(currentIndex.row(), currentIndex.row() + 1);
	mColourModel->setStringList(list);

	ui.ListColours->setCurrentIndex(mColourModel->index(currentIndex.row() + 1));

	updateColourFile();
}

void Window::changeIsoEvent(int value)
{
	mActionClass->iso(std::string(ui.BoxIso->itemText(value).toUtf8()));
}

void Window::changeApertureEvent(int value)
{
	mActionClass->aperture(std::string(ui.BoxAperture->itemText(value).toUtf8()));
}

void Window::changeShutterEvent(int value)
{
	mActionClass->shutter(std::string(ui.BoxShutter->itemText(value).toUtf8()));
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

	//Get and validate colours
	QStringList colours = mColourModel->stringList();
	if (colours.size() == 0)
	{
		Inform("Can not take photos with no colours selected.");
		return;
	}
	for (auto it = colours.begin(); it != colours.end(); ++it)
		if (!QColor(std::string(it->toUtf8()).c_str()).isValid())
		{
			Error(std::string(("Invalid colour " + *it + ", Aborting sequence.").toUtf8()));
			return;
		}

	if (!saveRaw && !saveProcessed)
	{
		Inform("Can not take photos: No target saving format");
		return;
	}

	setCursor(Qt::BlankCursor);
	disableEvents();
	//Shoot sequence
	if (!mActionClass->shootSequence(startTime, colours, saveProcessed, saveRaw, processedExtension))
		Error("Failed taking image sequence");
	enableEvents();
	setCursor(Qt::ArrowCursor);
}

void Window::disableEvents()
{
	this->installEventFilter(&mFilter);
}

void Window::enableEvents()
{
	this->removeEventFilter(&mFilter);
}

void Window::updateColourFile()
{
	//Try to reopen file, clearing it:
	std::fstream colourFile("colours.txt", std::ios::out | std::ios::trunc);
	if (colourFile.fail())
	{
		Warning("Could not open colours.txt - Colours will not be saved.");
		return;
	}

	auto colours = mColourModel->stringList();

	for (auto it = colours.begin(); it != colours.end(); ++it)
		colourFile << std::string(it->toUtf8()) << "\n";
}