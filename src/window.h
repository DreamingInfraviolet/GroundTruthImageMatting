#pragma once

#include <mutex>
#include <QtGui>
#include "ui_mainui.h"

class ActionClass;

/**
* This class is meant to act as the main force driving the program.
* It specifies the user interface, and performs the actions that need to be done.
* */
class Window : public QMainWindow
{
	Q_OBJECT
private:

	/**
	* Main constructor creating the window. Sets the state to idle.
	* Hidden: use create().
	* */
	Window();

	/** Performs general initialisation. Returns true upon success. */
	bool initialise();

	/** Initialises the camera system and selects the first camera upon success. */
	bool initialiseCamera();

	Ui::MainWindow ui;
	std::unique_ptr<QStringListModel> mColourModel; //Used to control the colour list
	
	std::unique_ptr<ActionClass> mActionClass;

	//This is used to disable the QT event system when SFML is used.
	class EventFilter : public QObject
	{
	public:
		EventFilter(){}
		bool eventFilter(QObject * watched, QEvent * event) override { return true; }
	} mFilter;
public:

	/**
	* Creates a new window, or returns null upon failure.
	* */
	static Window* create();

	/**
	* Destructor cleaning up resources.
	*/
	~Window();

	void disableEvents();
	void enableEvents();

signals:
	void showOther();

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