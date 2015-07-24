#include "window.h"
#include "actionthread.h"
#include "io.h"
#include "camera.h"
#include <thread>
Window::Window(int width, int height)
	: nana::form(nana::API::make_center(width, height),
				 nana::appearance(true, true, true, false, true, false, false)),
	mCameraBox(*this, true),
	mDelaySpinbox(*this, true),
	mColours(*this, true),
	mAddColourButton(*this, true),
	mRemoveColourButton(*this, true),
	mUpColourButton(*this, true),
	mDownColourButton(*this, true),
	mIsoLabel(*this, true),
	mIsoBox(*this, true),
	mShutterLabel(*this, true),
	mShutterBox(*this, true),
	mApertureLabel(*this, true),
	mApertureBox(*this, true),
	mGoButton(*this, true),
	mTimerSpinbox(*this, true),
	mSaveBmpCheckbox(*this, true),
	mSaveRawCheckbox(*this, true),
	mMainPlace(*this) {}

bool Window::initialise()
{
	//Launch action thread
	mActionThreadReturn = std::async(std::launch::async, ActionThread, &mEventSystem);

	//Request information about the camera
	std::shared_ptr<EnnumerateEvent> isoEnnumerationEvent (new EnnumerateEvent(EnnumerateEvent::EnnumerateType::Iso));
	std::shared_ptr<EnnumerateEvent> apertureEnnumerationEvent (new EnnumerateEvent(EnnumerateEvent::EnnumerateType::Aperture));
	std::shared_ptr<EnnumerateEvent> shutterEnnumerationEvent (new EnnumerateEvent(EnnumerateEvent::EnnumerateType::Shutter));

	std::shared_ptr<GetEvent> currentIsoEvent (new GetEvent(GetEvent::GetType::Iso));
	std::shared_ptr<GetEvent> currentApertureEvent (new GetEvent(GetEvent::GetType::Aperture));
	std::shared_ptr<GetEvent> currentShutterEvent (new GetEvent(GetEvent::GetType::Shutter));

	std::shared_ptr<EnnumerateCameraEvent> availableCamerasEvent (new EnnumerateCameraEvent());

	//Send data requests
	mEventSystem.send(isoEnnumerationEvent);
	mEventSystem.send(apertureEnnumerationEvent);
	mEventSystem.send(shutterEnnumerationEvent);
	mEventSystem.send(currentIsoEvent);
	mEventSystem.send(currentApertureEvent);
	mEventSystem.send(currentShutterEvent);
	mEventSystem.send(availableCamerasEvent);

	//Set up GUI
	using namespace nana;

	mColours.auto_draw(true);
	mColours.ordered_categories(false);
	mColours.show_header(false);
	mColours.append_header(STR("Colours"), 200);
	auto test = mColours.append(STR("Colours"));
	test.append({ STR("Hi"),STR("Yo") });

	//Set button captions
	mAddColourButton.caption(STR("Add"));
	mRemoveColourButton.caption(STR("Remove"));
	mUpColourButton.caption(STR("Up"));
	mDownColourButton.caption(STR("Down"));
	mGoButton.caption(STR("Go!"));

	//Set label captions
	mIsoLabel.caption(STR("ISO"));
	mShutterLabel.caption(STR("Shutter Speed"));
	mApertureLabel.caption(STR("Aperture"));

	//Get current values
	int currentIso = currentIsoEvent->get();
	int currenAperture = currentApertureEvent->get();
	int currentShutter = currentShutterEvent->get();

	//Fill iso values:
	auto isoEnums = isoEnnumerationEvent.get()->get();
	for (auto it = isoEnums.begin(); it != isoEnums.end(); ++it)
		mIsoBox.push_back(nana::charset(Camera::isoMappings[*it], nana::unicode::utf8));

	//Fill shutter values:
	auto shuttErenums = shutterEnnumerationEvent->get();
	for (auto it = shuttErenums.begin(); it != shuttErenums.end(); ++it)
		mShutterBox.push_back(nana::charset(Camera::shutterSpeedMappings[*it], nana::unicode::utf8));

	//Fill aperture values:
	auto apertureEnums = apertureEnnumerationEvent->get();
	for (auto it = apertureEnums.begin(); it != apertureEnums.end(); ++it)
		mApertureBox.push_back(nana::charset(Camera::isoMappings[*it], nana::unicode::utf8));

	//Fill camera list:
	auto cameraMappings = availableCamerasEvent->get();
	for (auto it = cameraMappings.mForwardMap.begin(); it != cameraMappings.mForwardMap.end(); ++it)
	{
		cameras.emplace_back(*it);
		mCameraBox.push_back(nana::charset(it->second, nana::unicode::utf8));
	}

	combox mApertureBox;
	spinbox mTimerSpinbox;

	checkbox mSaveBmpCheckbox;
	checkbox mSaveRawCheckbox;


	mMainPlace.div("");

	mMainPlace.collocate();

	Inform("Done initialising window");
	show();

	return true;
}

bool Window::initialiseCameras()
{
	return true;
}


Window* Window::create(int width, int height)
{
	Window* out = new Window(width, height);
	if (!out)
		return nullptr;
	if (!out->initialise())
	{
		delete out;
		return nullptr;
	}
	else
		return out;
}

Window::~Window()
{
}


int Window::run()
{
	nana::exec();

	//Kill action thread and get result:
	mEventSystem.send(std::shared_ptr<MetaEvent>(new MetaEvent(MetaEvent::MetaType::Shutdown)));
	return mActionThreadReturn.get();
}