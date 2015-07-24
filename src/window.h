#pragma once
#include <nana/gui/wvl.hpp> 
#include <nana/gui/widgets/spinbox.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/checkbox.hpp>

#include <future>

#include "cameraguiinfo.h"
#include "event.h"

/**
* This class is meant to act as the main force driving the program.
* It specifies the user interface, and selects the actions to perform.
* */
class Window : public nana::form
{
private:
enum class State {idle, startup, normal, shutdown};

	//Define window elements:

	//Camera selection
	nana::combox mCameraBox;

	//Use this for delay
	nana::spinbox mDelaySpinbox;

	//Define colour box and controls
	nana::listbox mColours;
	nana::button mAddColourButton;
	nana::button mRemoveColourButton;
	nana::button mUpColourButton;
	nana::button mDownColourButton;

	//Define iso control
	nana::label mIsoLabel;
	nana::combox mIsoBox;

	//Define shutterSpeed control
	nana::label mShutterLabel;
	nana::combox mShutterBox;

	//Define aperture control
	nana::label mApertureLabel;
	nana::combox mApertureBox;

	//Define go button
	nana::button mGoButton;

	//Define timer spinbox
	nana::spinbox mTimerSpinbox;

	//Define saving checkboxes
	nana::checkbox mSaveBmpCheckbox;
	nana::checkbox mSaveRawCheckbox;

	nana::place mMainPlace;

	EventSystem mEventSystem;
	std::future<int> mActionThreadReturn;

	std::vector<CameraGuiInfo> cameras;



	/**
	* Main constructor creating the window. Sets the state to idle.
	* Hidden: use create() to allow for error reporting.
	* */
	Window(int width, int height);

	/** Performs geleral initialisation. Returns true upon success. */
	bool initialise();

	/** How should I handle this...? What if a camera gets inserted or removed? I should update CameraList. */
	bool initialiseCameras();

public:

	/**
	* Creates a new window, or returns null upon failure.
	* */
	static Window* create(int width, int height);

	/**
	* Destructor cleaning up resources. Sets the state to shutdown.
	*/
	~Window();

	/**
	* Runs the main program, returning when the window is closed.
	* Sets the state to idle.
	* @return The return code of the program.
	* */
	int run();
};