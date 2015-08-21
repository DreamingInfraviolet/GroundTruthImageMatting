#include "actionclass.h"
#include <SDL.h>
#include "io.h"
#include "camera.h"
#include <thread>
#include <qcolor.h>
#include <cstdio>
#include <ctime>
#include "window.h"
#include "rawrgbeds.h"
#include "groundtruth.h"
#include "qprocess.h"

//Disable CHECK_CAMERA warning with empty arguments.
#pragma warning (disable: 4003)

//Disable warning about using localtime.
#pragma warning (disable: 4996)

#define CHECK_CAMERA(ret) {                                                                                         \
                            if(CameraList::instance()==nullptr || CameraList::instance()->activeCamera()==nullptr)  \
                                {                                                                                   \
                                    Warning("Using camera at " __FILE__ " in line " + ToString(__LINE__) +          \
                                            " without valid instance.");                                            \
                                    return ret;                                                                     \
                                }                                                                                   \
                          }


ActionClass* ActionClass::sActionClass = nullptr;

bool ActionClass::initialise()
{
	//Initialise camera system
	Inform("Initialising camera");
	mCameraList = std::unique_ptr<CameraList>(CameraList::create());

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

	Inform("Camera ready");
	return true;
}

ActionClass* ActionClass::create()
{
    if (sActionClass)
        return nullptr;
    ActionClass* out = new ActionClass();
    if (!out)
        return out;

    if (!out->initialise())
    {
        delete out;
        return nullptr;
    }

    out->sActionClass = out;
    return out;
}

ActionClass::~ActionClass()
{
    sActionClass = nullptr;
}

int ActionClass::iso()
{
    CHECK_CAMERA(-1);
    return CameraList::instance()->activeCamera()->iso();
}

int ActionClass::aperture()
{
    CHECK_CAMERA(-1);
    return CameraList::instance()->activeCamera()->aperture();
}

int ActionClass::shutter()
{
    CHECK_CAMERA(-1);
    return CameraList::instance()->activeCamera()->shutterSpeed();
}

int ActionClass::whiteBalance()
{
	CHECK_CAMERA(-1);
	return CameraList::instance()->activeCamera()->whiteBalance();
}

void ActionClass::iso(const std::string& text)
{
    CHECK_CAMERA();
    CameraList::instance()->activeCamera()->iso(Camera::isoMappings[text]);
}
void ActionClass::aperture(const std::string& text)
{
    CHECK_CAMERA();
    CameraList::instance()->activeCamera()->aperture(Camera::apertureMappings[text]);
}
void ActionClass::shutter(const std::string& text)
{
    CHECK_CAMERA();
    CameraList::instance()->activeCamera()->shutterSpeed(Camera::shutterSpeedMappings[text]);
}

void ActionClass::whiteBalance(const std::string& text)
{
	CHECK_CAMERA();
	CameraList::instance()->activeCamera()->whiteBalance(Camera::whiteBalanceMappings[text]);
}

bool ActionClass::shootSequence(std::chrono::time_point<std::chrono::system_clock> startTime,
	const QStringList& colours, bool saveProcessed, bool saveRaw, bool saveGroundTruth,
	const std::string& processedExtension, const std::string& path)
{
	bool success = true;

    Inform("Shooting sequence");
    CHECK_CAMERA(false);

    //Initialise SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		Error("Could not initialise SDL");
		return false;
	}

	//Must not return before uninitialising SDL.

	//Shoot
	auto foregroundImages = shootPictures(colours, true, startTime);
	decltype(foregroundImages) backgroundImages;

	if (foregroundImages.size() == 0)
		success = false;

	//Take Ground Truth pictures
	if (success && saveGroundTruth)
	{
		assert(colours.size() >= 5);

		//Wait for user
		Inform("Ground Truth stage: remove the object");
		int secondsToWait = Window::instance()->showGroundTruthDialog();
		if (secondsToWait == -1)
			saveGroundTruth = false;
		else
		{
			auto backgroundStartTime = std::chrono::system_clock::now() + std::chrono::seconds(secondsToWait);
			backgroundImages = shootPictures(colours, true, backgroundStartTime);

			if (backgroundImages.size() == 0)
				success = false;
		}
	}

	//Save images
	if (success)
		Inform("Processing images");
	time_t t = time(0);

	auto foregroundRgbs = saveImages(foregroundImages, path, "_foreground",
		t, saveRaw, saveProcessed, processedExtension);
	foregroundImages.clear();

	auto backgroundRgbs = saveImages(backgroundImages, path, "_background",
		t, saveRaw, saveProcessed, processedExtension);
	backgroundImages.clear();

	if (foregroundRgbs.size() == 0 || backgroundRgbs.size() == 0)
		success = false;

	//Generate ground truth
	if (success && saveGroundTruth)
	{
		if (!generateGroundTruth(foregroundRgbs, backgroundRgbs, path, t))
			success = false;
	}

	SDL_Quit();
    Inform("Done");
	return success;
}

//Potential memory leak
std::vector < std::pair<QColor, ImageRaw>  > ActionClass::shootPictures
	(const QStringList& colours, bool delay,
	std::chrono::time_point<std::chrono::system_clock> startTime)
{
	SDL_ShowCursor(false);

	SDL_Window *win = SDL_CreateWindow("Colour", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
		SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (!win)
	{
		Error("Could not create SDL window.");
		return{};
	}


	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);

	if (!ren)
	{
		Error("Could not create SDL renderer");
		SDL_DestroyWindow(win);
		return{};
	}

	std::vector < std::pair<QColor, ImageRaw>  > foregroundImages;
	Camera* camera = CameraList::instance()->activeCamera();

	//Go!

	// For each colour
	for (auto it = colours.begin(); it != colours.end(); ++it)
	{

		QColor colour = QColor(*it);

		//Update screen a few times just in case with the colour
		for (int i = 0; i < 4; ++i)
		{
			//Run the sdl loop, closing if needed
			SDL_Event e;
			while (SDL_PollEvent(&e)) {}

			SDL_SetRenderDrawColor(ren, colour.red(), colour.green(), colour.blue(), 255);
			SDL_RenderClear(ren);
			SDL_RenderPresent(ren);
		}

		//if it is our first time, wait the delay about before continuing now that we're ready.
		if (it == colours.begin())
		{
			//Wait until we should start
			auto currentTime = std::chrono::steady_clock::now();
			std::this_thread::sleep_for(startTime - currentTime);
		}


		//Wait a little to ensure the screen has refreshed before sending shoot request
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
		camera->shoot();

		//Loop must be called while camera is not ready
		do
		{
			SDL_Event e;
			while (SDL_PollEvent(&e)) {}
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		} while (!camera->readyToShoot());


		//Retrieve image
		foregroundImages.push_back(std::make_pair(colour, camera->retrieveLastImage()));
		if (foregroundImages.back().second.failed())
		{
			Error("Error retrieving image");
			foregroundImages.pop_back();
			continue;
		}
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_ShowCursor(true);

	return foregroundImages;

}

std::vector<int> ActionClass::ennumeratePossibleValues(Camera::EnnumerableProperties ep)
{
    CHECK_CAMERA({});
    return CameraList::instance()->activeCamera()->ennumeratePossibleValues(ep);
}

std::string ActionClass::generateFilePath(const std::string& folder,
	const std::string& colour, time_t time_)
{
	time_t t = time_;   // get time now
    struct tm * now = localtime(&t);
	return appendNameToPath(
		ToString(now->tm_year) + "_" +
		ToString(now->tm_mday) + "-" +
		ToString(now->tm_mon) + "-" +
		ToString(now->tm_hour) + "_" +
		ToString(now->tm_min) + "." +
		ToString(now->tm_sec) + "." +
        colour,
		folder);
}

bool ActionClass::generateGroundTruth(std::vector<RawRgbEds>& foreground,
	std::vector<RawRgbEds>& background, const std::string& path, time_t t)
{
	//Save temp images
	Inform("Saving ground truth temporaries");

	assert(foreground.size() >= 5 && background.size() >= 5);

	//Generate file names
	QStringList fTempNames;
	QStringList bTempNames;
	QStringList aName = { generateFilePath(path, "A.png", t).c_str() };
	QStringList fName = { generateFilePath(path, "F.png", t).c_str() };
	QStringList afName = { generateFilePath(path, "AF.png", t).c_str() };

	for (size_t i = 0; i < 5; ++i)
		fTempNames.append(generateFilePath(path, "_temp_f_" + ToString(i) + ".rawrgb", t).c_str());
	for (size_t i = 0; i < 5; ++i)
		bTempNames.append(generateFilePath(path, "_temp_b_" + ToString(i) + ".rawrgb", t).c_str());

	//Save images
	for (size_t i = 0; i < 5; ++i)
		if (!SaveRawRgbEds(std::string(fTempNames[i].toUtf8()), foreground[i]))
		{
			Error("Could not save " + std::string(fTempNames[i].toUtf8()));
			return false;
		}
	for (size_t i = 0; i < 5; ++i)
		if (!SaveRawRgbEds(std::string(bTempNames[i].toUtf8()), background[i]))
		{
			Error("Could not save " + std::string(bTempNames[i].toUtf8()));
			return false;
		}

	//Clear buffers
	foreground.clear();
	background.clear();

	//Execute ground truth application
	Inform("Executing ground truth application");
	QProcess* gtProcess = new QProcess(Window::instance());
	QStringList collectiveArgs = { fTempNames + bTempNames + aName + fName + afName };
	int code = gtProcess->execute("GroundTruth.exe", collectiveArgs);
	delete gtProcess;

	//Delete temporary files
	for (auto it = fTempNames.begin(); it != fTempNames.end(); ++it)
		std::remove(it->toUtf8());
	for (auto it = bTempNames.begin(); it != bTempNames.end(); ++it)
		std::remove(it->toUtf8());

	if (code != 0)
	{
		Error("Non zero return code: " + ToString(code));
		return false;
	}

	return true;
}

std::vector<RawRgbEds> ActionClass::saveImages(std::vector < std::pair<QColor, ImageRaw>  > & images,
	const std::string& path, const std::string& nameSuffix,
	time_t t, bool saveRaw, bool saveProcessed, const std::string& processedExtension)
{
	if (images.size() == 0)
		return{};

	std::vector<RawRgbEds> out;
	out.reserve(images.size());

	for (size_t i = 0; i < images.size(); ++i)
	{
		//Save raw
		if (saveRaw)
		{
			std::string pathRaw = generateFilePath(
				path, std::string(images[i].first.name().toUtf8()) + nameSuffix, t) + ".cr2";
			if (!images[i].second.saveToFile(pathRaw))
				return{};
		}

		//Get RGB
		out.push_back(images[i].second.findRgb());
		if (std::get<2>(out.back()).size() == 0)
			return{};

		//Save processed
		if (saveProcessed)
		{
			std::string pathProcessed = generateFilePath(
				path, std::string(images[i].first.name().toUtf8()) + nameSuffix, t) + "." + processedExtension;
			images[i].second.saveProcessed(pathProcessed, &out.back());
		}
	}

	return out;
}