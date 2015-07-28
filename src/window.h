#pragma once

#include <mutex>
#include <QtGui>
#include <QApplication>
#include "ui_mainui.h"
#include "io.h"
#include "camera.h"
#include <memory>

/**
* This class is meant to act as the main force driving the program.
* It specifies the user interface, and selects the actions to perform.
* */
class Window : public QMainWindow
{
	Q_OBJECT //You *must* include this macro !!!!
private:


	/**
	* Main constructor creating the window. Sets the state to idle.
	* Hidden: use create() to allow for error reporting.
	* */
	Window(int argc, char** argv);

	/** Performs geleral initialisation. Returns true upon success. */
	bool initialise();

	//Initialises the main camera.
	bool initialiseCamera();


	Ui::MainWindow ui;//This ui object gives you access to your ui widgets
	std::unique_ptr<QStringListModel> mColourModel; //Used to control the colour list
	std::unique_ptr<CameraList> mCameraList;
public:

	/**
	* Creates a new window, or returns null upon failure.
	* */
	static Window* create(int argc, char** argv);

	/**
	* Destructor cleaning up resources.
	*/
	~Window();


	public slots:
	void buttonAddEvent();
	void buttonRemoveEvent();
	void buttonUpEvent();
	void buttonDownEvent();
	void changeIsoEvent(int value);
	void changeApertureEvent(int value);
	void changeShutterEvent(int value);
	void shootEvent();
};