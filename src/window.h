#pragma once
#include <QtGui>
#include <memory>
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

    /** Performs global initialisation. Returns true upon success. */
    bool initialise();

    //Auto generated UI object
    Ui::MainWindow ui;

    //Used to control the colour list
    std::unique_ptr<QStringListModel> mColourModel;
    
    //The internal action class.
    std::unique_ptr<ActionClass> mActionClass;

    //This is used to disable the QT event system when SFML is used.
    //Otherwise SFML can not function properly.
    class EventFilter : public QObject
    {
    public:
        bool eventFilter(QObject * watched, QEvent * event) override { return true; }
    } mFilter;

    //A static pointer to the current window.
    static Window* sWindow;

	/** Updates colours.txt with the currently selected colours. */
	void updateColourFile();
public:

    /** Creates a new window. Returns null if failed or if a window already exists. */
    static Window* create();

    /** Deregisters the window. */
    ~Window();

    /** Disables the QT event system. */
    void disableEvents();

    /** Enables the QT event system. */
    void enableEvents();

    /** Returns the current instance of the window. */
    static Window* instance();

    public slots:
    /** Adds a new colour. */
    void buttonAddEvent();

    /** Removes a selected colour. */
    void buttonRemoveEvent();

    /** Brings the selected colour up. */
    void buttonUpEvent();

    /** Brings the selected colour down. */
    void buttonDownEvent();

    /** Changes the camera ISO. */
    void changeIsoEvent(int value);

    /** Changes the camera perture size. */
    void changeApertureEvent(int value);

    /** Changes the shutter duration. */
    void changeShutterEvent(int value);

    /** Takes and saves a sequence of images with the given parameters. */
    void shootEvent();
};