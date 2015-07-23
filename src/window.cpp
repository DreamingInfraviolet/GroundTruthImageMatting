#include "window.h"

Window::Window(int width, int height)
	: nana::form(nana::API::make_center(width, height),
				 nana::appearance(true, true, true, false, true, false, false)),
	mCameraSelection(*this, true),
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
	if (!initialiseCameras())
		return false;

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

	//Fill iso values:
	for (auto it = Camera::isoMappings.mForwardMap.begin(); it != Camera::isoMappings.mForwardMap.end(); ++it)
		//If a valid settinf
		if (it->first != 0xffffffff)
		mIsoBox.push_back(nana::charset(it->second, nana::unicode::utf8));

	//Fill shutter values:
	for (auto it = Camera::shutterSpeedMappings.mForwardMap.begin(); it != Camera::shutterSpeedMappings.mForwardMap.end(); ++it)
		//If a valid settinf
		if (it->first != 0xffffffff)
		mShutterBox.push_back(nana::charset(it->second, nana::unicode::utf8));

	//Fill aperture values:
	for (auto it = Camera::isoMappings.mForwardMap.begin(); it != Camera::isoMappings.mForwardMap.end(); ++it)
		//If a valid settinf
		if (it->first != 0xffffffff)
		mIsoBox.push_back(nana::charset(it->second, nana::unicode::utf8));

	combox mApertureBox;

	spinbox mTimerSpinbox;

	checkbox mSaveBmpCheckbox;
	checkbox mSaveRawCheckbox;


	mMainPlace.div("");

	mMainPlace.collocate();

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
	return 0;
}