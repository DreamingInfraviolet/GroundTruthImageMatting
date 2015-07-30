#include "actionclass.h"
#include <SDL.h>
#include "io.h"
#include "camera.h"
#include <thread>
#include <qcolor.h>
#include <ctime>

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

bool ActionClass::shootSequence(std::chrono::time_point<std::chrono::system_clock> startTime,
    const QStringList& colours, bool saveProcessed, bool saveRaw, const std::string& processedExtension)
{
    Inform("Shooting sequence");

    CHECK_CAMERA(false);

    //Initialise SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		Error("Could not initialise SDL");
		return false;
	}

	SDL_Window *win = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
		SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (!win)
	{
		Error("Could not create SDL window.");
		return false;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1,0);

	if (!ren)
	{
		Error("Could not create SDL renderer");
		return false;
	}
	SDL_ShowCursor(false);


    std::vector < std::pair<QColor, ImageRaw>  > rawImages;
    Camera* camera = CameraList::instance()->activeCamera();

    //Go!

    // For each colour
    for (auto it = colours.begin(); it != colours.end(); ++it)
    {

        QColor colour = QColor(std::string(it->toUtf8()).c_str());

        //Update screen a few times just in case with the colour
        for (int i = 0; i < 4; ++i)
        {
			//Run the sfml loop, closing if needed
			SDL_Event event;
			while (SDL_PollEvent(&event)) {}

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
			SDL_Event event;
            while (SDL_PollEvent(&event)) {}
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
	SDL_ShowCursor(true);
	SDL_Quit();

    Inform("Processing images");

    //Prepare name
    std::string path = "C:\\Anima\\ImageBackgroundRemoval\\build\\src\\captured\\";
	time_t t = time(0);
    for (size_t i = 0; i < rawImages.size(); ++i)
    {
        std::string baseName =
			generateFilePathNoExtension(path, std::string(rawImages[i].first.name().toUtf8()), t) + ".";

        if (saveRaw)
            rawImages[i].second.saveToFile(baseName + "cr2");
        if (saveProcessed)
			rawImages[i].second.saveProcessed(baseName + processedExtension);
    }

    Inform("Done");

    return true;
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
		ToString(now->tm_sec) + "." +
        ToString(now->tm_min) + "." +
        ToString(now->tm_hour) + "_" +
        ToString(now->tm_mday) + "-" +
        ToString(now->tm_mon) + "-" +
        ToString(now->tm_year) + "_" +
        colour,
		folder);
}