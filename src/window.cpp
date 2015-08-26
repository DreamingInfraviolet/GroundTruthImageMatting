#include "window.h"
#include "io.h"
#include <qcolordialog.h>
#include "actionclass.h"
#include "qinputdialog.h"
#include <fstream>
#include <qfiledialog.h>


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
	connect(ui.BoxWhiteBalance, SIGNAL(currentIndexChanged(int)), this, SLOT(changeWhiteBalanceEvent(int)));

	connect(ui.ButtonChoosePath, SIGNAL(pressed()), this, SLOT(buttonChangeDirEvent()));

	connect(ui.ButtonGo, SIGNAL(pressed()), this, SLOT(shootEvent()));


	ui.BoxProcessedformat->addItems({ "tiff", "png" });

	mColourModel = std::unique_ptr<QStringListModel>(new QStringListModel(this));
	ui.ListColours->setModel(mColourModel.get());
	ui.ListColours->setEditTriggers(QAbstractItemView::NoEditTriggers);

	if (!(mActionClass = std::unique_ptr<ActionClass>(ActionClass::create())).get())
		return false;

	//Set selectable values
	std::vector<int> apList = mActionClass->ennumeratePossibleValues(Camera::EnnumerableProperties::Aperture);
	std::vector<int> isList = mActionClass->ennumeratePossibleValues(Camera::EnnumerableProperties::ISO);
	std::vector<int> shList = mActionClass->ennumeratePossibleValues(Camera::EnnumerableProperties::ShutterSpeed);

	int currentAp = mActionClass->aperture();
	int currentIs = mActionClass->iso();
	int currentSh = mActionClass->shutter();
	int currentWb = mActionClass->whiteBalance();

	//Populate combo boxes
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

	bool foundShutter = false;
	for (size_t i = 0; i < shList.size(); ++i)
	{
		ui.BoxShutter->addItem(Camera::shutterSpeedMappings[shList[i]].c_str());
		if (shList[i] == currentSh)
		{
			ui.BoxShutter->setCurrentIndex(i);
			foundShutter = true;
		}
	}

	//If shutter is not a valid value (such as bulb mode), change it to a default.
	if (!foundShutter && shList.size() > 0)
	{
		mActionClass->shutter(Camera::shutterSpeedMappings[shList.front()]);
		ui.BoxShutter->setCurrentIndex(0);
	}

	std::vector<int> availableWhiteBalances = { 1, 2, 3, 4, 5, 8, 9 };
	bool foundWhiteBalance = false;
	for (size_t i = 0; i < availableWhiteBalances.size(); ++i)
	{
		ui.BoxWhiteBalance->addItem(Camera::whiteBalanceMappings[availableWhiteBalances[i]].c_str());
		if (availableWhiteBalances[i] == currentWb)
		{
			ui.BoxWhiteBalance->setCurrentIndex(i);
			foundWhiteBalance = true;
		}
	}

	//If no valid white balance set, set default
	if (!foundWhiteBalance)
	{
		mActionClass->whiteBalance("Daylight");
		ui.BoxWhiteBalance->setCurrentIndex(0);
	}

	//Read in colours file
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
	initialised = true;
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
	if (!initialised)
		return;

	QColor colour = QColorDialog::getColor(Qt::white);
	if (!colour.isValid())
		return;

	mColourModel->setStringList(mColourModel->stringList() << colour.name());

	updateColourFile();
}

void Window::buttonRemoveEvent()
{
	if (!initialised)
		return;

	auto removeIndex = ui.ListColours->currentIndex();
	if (removeIndex.isValid())
		mColourModel->removeRow(removeIndex.row());

	updateColourFile();
}
void Window::buttonUpEvent()
{
	if (!initialised)
		return;

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
	if (!initialised)
		return;

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
	if (!initialised)
		return;

	mActionClass->iso(std::string(ui.BoxIso->itemText(value).toUtf8()));
}

void Window::changeApertureEvent(int value)
{
	if (!initialised)
		return;

	mActionClass->aperture(std::string(ui.BoxAperture->itemText(value).toUtf8()));
}

void Window::changeShutterEvent(int value)
{
	if (!initialised)
		return;

	mActionClass->shutter(std::string(ui.BoxShutter->itemText(value).toUtf8()));
}

void Window::changeWhiteBalanceEvent(int value)
{
	if (!initialised)
		return;

	mActionClass->whiteBalance(std::string(ui.BoxWhiteBalance->itemText(value).toUtf8()));
}

void Window::shootEvent()
{
	static bool shooting = false;
	if (shooting)
		return;
	shooting = true;

	if (!initialised)
		return;

	//Gather information
	int delay = ui.SpinDelay->value();
	auto startTime = std::chrono::steady_clock::now() + std::chrono::seconds(delay);

	bool saveProcessed = ui.CheckSaveProcessed->isChecked();
	bool saveRaw = ui.CheckSaveRaw->isChecked();
	bool saveGroundTruth = ui.CheckSaveGroundTruth->isChecked();

	std::string processedExtension = ui.BoxProcessedformat->itemText(ui.BoxProcessedformat->currentIndex()).toUtf8();

	//Get and validate colours
	QStringList colours = mColourModel->stringList();
	if (colours.size() == 0)
	{
		Inform("Can not take photos with no colours selected.");
		shooting = false;
		return;
	}
	for (auto it = colours.begin(); it != colours.end(); ++it)
		if (!QColor(std::string(it->toUtf8()).c_str()).isValid())
		{
			Error(std::string(("Invalid colour " + *it + ", Aborting sequence.").toUtf8()));
			shooting = false;
			return;
		}

	if (!saveRaw && !saveProcessed && !saveGroundTruth)
	{
		Inform("Can not take photos: No target saving format");
		shooting = false;
		return;
	}

	if (saveGroundTruth && colours.size() < 5)
	{
		Inform("Can not take photos: Please select at least five colours for ground truth generation.");
		shooting = false;
		return;
	}

	//setWindowState(Qt::WindowState::WindowMinimized);
	disableEvents();

	//Shoot sequence
	if (!mActionClass->shootSequence(startTime, colours, saveProcessed,
		saveRaw, saveGroundTruth, processedExtension, mSaveDir))
		Error("Failed taking image sequence");

	enableEvents();
	shooting = false;
	//showNormal();
}

void Window::disableEvents()
{
	if (!initialised)
		return;

	this->installEventFilter(&mFilter);
}

void Window::enableEvents()
{
	if (!initialised)
		return;

	this->removeEventFilter(&mFilter);
}

void Window::updateColourFile()
{
	if (!initialised)
		return;

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

void Window::buttonChangeDirEvent()
{
	if (!initialised)
		return;

	QFileDialog fd;
	fd.setFileMode(QFileDialog::Directory);
	fd.setOption(QFileDialog::ShowDirsOnly);
	fd.setViewMode(QFileDialog::Detail);
	int result = fd.exec();
	QString directory;
	if (result)
	{
		directory = fd.selectedFiles()[0];
		mSaveDir = directory.toUtf8();
	}
}

int Window::showGroundTruthDialog()
{
	bool ok;
	int select = QInputDialog::getInt(NULL, "Please remove the object", "Delay in seconds:", 2, 0, 1000, 1, &ok);
	if (!ok)
		return -1;
	else
		return select;
}