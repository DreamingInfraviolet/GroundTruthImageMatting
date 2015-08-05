#include "actionclass.h"
#include <SDL.h>
#include "io.h"
#include "camera.h"
#include <thread>
#include <qcolor.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "window.h"

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
    Inform("Shooting sequence");
    CHECK_CAMERA(false);

    //Initialise SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		Error("Could not initialise SDL");
		return false;
	}

	//Shoot
	auto rawImages = shootPictures(colours, true, startTime);

	decltype(rawImages) rawBackgroundImages;
	//Take Ground Truth pictures
	if (saveGroundTruth)
	{
		assert(colours.size() >= 5);

		//Only shoot with first five colours
		QStringList groundTruthColours;
		for (int i = 0; i < 5; ++i)
			groundTruthColours.append(colours[i]);

		Inform("Ground Truth stage: remove the object");
		int secondsToWait = Window::instance()->showGroundTruthDialog();
		if (secondsToWait == -1)
		{
			saveGroundTruth = false;
		}
		else
		{
			auto backgroundStartTime = std::chrono::system_clock::now() + std::chrono::seconds(secondsToWait);
			rawBackgroundImages = shootPictures(groundTruthColours, true, backgroundStartTime);
		}
	}


	//Save images
	Inform("Processing images");
	time_t t = time(0);

	
	if (saveRaw)
		for (size_t i = 0; i < rawImages.size(); ++i)
			rawImages[i].second.saveToFile(
				generateFilePathNoExtension(path, std::string(rawImages[i].first.name().toUtf8()), t) + ".cr2");
				
	std::vector<ImageRaw::RawRgb> rawRgbs;
	
	//Process images
	if (saveProcessed || saveGroundTruth)
	for (size_t i = 0; i < rawImages.size(); ++i)
	{
		std::string name = generateFilePathNoExtension(
			path, std::string(rawImages[i].first.name().toUtf8()), t) + "." + processedExtension;

		rawRgbs.push_back(rawImages[i].second.findRgb());

		if (saveProcessed)
			rawImages[i].second.saveProcessed(name, &rawRgbs.back());
	}

	//Destroy raw images
	rawImages.clear();
	//
	
	//Generate and save ground truth
	if (saveGroundTruth)
	{
		Inform("Processing ground truth images");
		if (rawRgbs.size() < 5 || rawBackgroundImages.size() < 5)
			return false;

		//Process and save background images
		std::vector<ImageRaw::RawRgb> backroundRgbs;
		for (size_t i = 0; i < rawBackgroundImages.size(); ++i)
		{
			if (saveRaw)
				rawBackgroundImages[i].second.saveToFile(generateFilePathNoExtension(
					path, std::string(rawBackgroundImages[i].first.name().toUtf8()) + "_background.cr2", t));
	
			backroundRgbs.push_back(rawBackgroundImages[i].second.findRgb());

			if (saveProcessed)
			{
				std::string name = generateFilePathNoExtension(
					path, std::string(rawBackgroundImages[i].first.name().toUtf8()) +
					"_background." + processedExtension, t);
				rawBackgroundImages[i].second.saveProcessed(name, &backroundRgbs.back());
			}
		}

		rawBackgroundImages.clear();

		//Generate ground truth
		Inform("Generating ground truth");

		std::vector<cv::Mat> groundTruth =
			ImageRaw::generateGroundTruth(rawRgbs[0], rawRgbs[1], rawRgbs[2], rawRgbs[3], rawRgbs[4],
			backroundRgbs[0], backroundRgbs[1], backroundRgbs[2], backroundRgbs[3], backroundRgbs[4]);

		if (groundTruth.size()==0)
			return false;

		//Always save Ground truth as tiff
		cv::imwrite(generateFilePathNoExtension(path, std::string("GroundTruth_A.tiff"), t), groundTruth[0]);
		cv::imwrite(generateFilePathNoExtension(path, std::string("GroundTruth_F.tiff"), t), groundTruth[1]);
		cv::imwrite(generateFilePathNoExtension(path, std::string("GroundTruth_AF.tiff"), t), groundTruth[2]);
	}

	SDL_Quit();
    Inform("Done");
    return true;
}

//Potential memory leak
std::vector < std::pair<QColor, ImageRaw>  > ActionClass::shootPictures
	(const QStringList& colours, bool delay,
	std::chrono::time_point<std::chrono::system_clock> startTime)
{
	SDL_ShowCursor(false);

	SDL_Window *win = SDL_CreateWindow("Colour", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
		SDL_WINDOW_SHOWN);// ,SDL_WINDOW_FULLSCREEN_DESKTOP);
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

	std::vector < std::pair<QColor, ImageRaw>  > rawImages;
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
			while (SDL_PollEvent(nullptr)) {}

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
			while (SDL_PollEvent(nullptr)) {}
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		} while (!camera->readyToShoot());


		//Retrieve image
		rawImages.push_back(std::make_pair(colour, camera->retrieveLastImage()));
		if (rawImages.back().second.failed())
		{
			Error("Error retrieving image");
			rawImages.pop_back();
			continue;
		}
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_ShowCursor(true);

	return rawImages;

}

std::vector<int> ActionClass::ennumeratePossibleValues(Camera::EnnumerableProperties ep)
{
    CHECK_CAMERA({});
    return CameraList::instance()->activeCamera()->ennumeratePossibleValues(ep);
}

std::string ActionClass::generateFilePathNoExtension(const std::string& folder,
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