#pragma once
class Window;
class EventSystem;

/**
* This is the main thread responsible for performing actions for the application.
* It polls events from the window system, reacting appropriately.
* */
int ActionThread(EventSystem* eventSystem);