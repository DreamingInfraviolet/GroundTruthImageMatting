#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <cassert>
#include "propertymap.h"
#include "EDSDKTypes.h"
#include "image.h"
#include "colour.h"

class ImageRaw;

/**
* These few classes compose an event system that are used to
* connect the camera GUI to the actual action thread.
* This allows for a responsive GUI, separation of interface and function
* and a convenient interface between threads.
* Events are meant to go from the GUI thread to the action thread.
* The GUI is thread safe, so there is no need for a two-way system.
* */


/**
* Serves as a base event type for the other event types.
* */
class Event
{
public:
	friend class EventSystem;

	enum class Type {
					Get,
					Set,
					Ennumerate,
					Shoot,
					Meta,
					None,
					EnnumerateCameras
	};

	virtual ~Event() {}

	/** Creates a generic event. */
	Event()
		: type(Type::None) {}

	/** Creates a specialised event. */
	Event(const Type _type)
		: type(_type) {}

	Type type;

	virtual Event* clone() const { return new Event(*this); }
};

/**
* Allows one to set camera events.
* */
class SetEvent : public Event
{
public:

	enum class SetType
	{
		Iso, Aperture, Shutter
	};

	SetEvent(const int v, const SetType type) : Event(Event::Type::Set),
		value(v),
		setType(type) {}

	friend class EventSystem;

	int get() { return value; }

	int value;
	SetType setType;

	virtual SetEvent* clone() const override
	{
		SetEvent* event = new SetEvent(*this);
		return event;
	}
};

/**
* Allows one to send a shoot event, and then optionally to retrieve the resulting image.
* */
class ShootEvent : public Event
{
	friend class EventSystem;

public:

	//Delay in seconds.
	int delay;
	bool saveBMP, saveRAW;
	std::vector<Colour> colours;

public:

	ShootEvent(int delay_, bool saveBMP_, bool saveRAW_, const std::vector<Colour>& colours_) : Event(Event::Type::Shoot),
		delay(delay_),
		saveBMP(saveBMP_),
		saveRAW(saveRAW_),
		colours(colours_){}

	virtual ShootEvent* clone() const override
	{
		ShootEvent* event = new ShootEvent(*this);
		return event;
	}
};

/*
* Stores events that are not direct commands, instead serving to allow the program to function.
* */
class MetaEvent : public Event
{
	friend class EventSystem;

public:

	enum class MetaType
	{
		Shutdown
	};

	MetaType metaType;

	MetaEvent(const MetaType type) : Event(Event::Type::Meta),
		metaType(type) {}

	virtual MetaEvent* clone() const override
	{
		MetaEvent* event = new MetaEvent(*this);
		return event;
	}
};

/**
* This class serves as the driving force between the one way event system.
* The GUI thread send()s event, and the action thread poll()s them.
* It is thread safe.
* */
class EventSystem
{
private:
	std::queue<std::shared_ptr<Event>> mEvents;
	std::mutex mMutex, mBlockMutex;
	std::condition_variable mBlockCondition;
public:

	/** Trivial constructor. */
	EventSystem();

	/**Deleted copy constructor and operator. */
	EventSystem(const EventSystem&) = delete;
	void operator = (const EventSystem&) = delete;

	/** Sends an event to the action thread. */
	void send(const Event& e);

	/** Sends all of the given inputs. */
	void send(const std::initializer_list<Event>& list);

	/** pops the next list off the queue and presents it.
	* If there is nothing to poll, the method blocks the thread until there is.
	* */
	std::shared_ptr<Event> poll();
};