#pragma once
class Window;
class EventSystem;

/**
* This is the main thread responsible for performing actions for the application.
* It polls events from the window system, reacting appropriately.
* Not that this assumes that the window is read to accept input.
* */
int ActionThread(EventSystem* eventSystem, Window* window);