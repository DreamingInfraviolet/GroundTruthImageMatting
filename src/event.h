#pragma once
#include <nana/basic_types.hpp> 
#include <queue>
#include <future>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <cassert>

#include "propertymap.h"
#include "EDSDKTypes.h"
#include "image.h"

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
};

/**
* Allows one to get camera events by sending a request and later fetching the result form the future.
* */
class GetEvent : public Event
{
	friend class EventSystem;
	std::shared_future<int> value;
	std::shared_ptr<std::promise<int> > promise;

public:

	enum class GetType
	{
		Iso, Aperture, Shutter
	};


	void set(int n)
	{
		promise->set_value(n);
	}

	int get()
	{
		return value.get();
	}

	GetEvent(const GetType type) : Event(Event::Type::Get),
		getType(type)
	{
		promise = decltype(promise)(new std::promise<int>());
		value = promise->get_future();
	}

	GetType getType;
};

/**
* Allows one to fetch ennumerations of possible values and fetch them later from the future.
* */
class EnnumerateEvent : public Event
{
	friend class EventSystem;
	std::shared_future<std::vector<int> > value;
	std::shared_ptr<std::promise<std::vector<int> > > promise;

public:

	enum class EnnumerateType
	{
		Iso, Aperture, Shutter
	};

	EnnumerateType ennumerateType;

	void set(const std::vector<int>& ennumeration)
	{
		promise->set_value(ennumeration);
	}


	std::vector<int> get()
	{
		return value.get();
	}

	EnnumerateEvent(const EnnumerateType type) : Event(Event::Type::Ennumerate),
		ennumerateType(type)
	{
		promise = decltype(promise)(new std::promise<std::vector<int> >());
		value = promise->get_future();
	}
};

/**
* Allows one to send a shoot event, and then optionally to retrieve the resulting image.
* */
class ShootEvent : public Event
{
	friend class EventSystem;

	//Retrieve the result here if needed. Ignore otherwise.
	std::shared_future<std::shared_ptr<ImageRaw> > image;
	std::shared_ptr<std::promise<std::shared_ptr<ImageRaw> > > promise;

public:

	//Delay in seconds.
	int delay;
	bool saveBMP, saveRAW;
	std::vector<nana::color> colours;

public:

	std::shared_ptr<ImageRaw> getImage()
	{
		return image.get();
	}

	ShootEvent(int delay_, bool saveBMP_, bool saveRAW_, const std::vector<nana::color>& colours_) : Event(Event::Type::Shoot),
		delay(delay_),
		saveBMP(saveBMP_),
		saveRAW(saveRAW_),
		colours(colours_)
	{
		promise = decltype(promise)(new std::promise<std::shared_ptr<ImageRaw> >());
		image = promise.get()->get_future();
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
};

/*
* Ennumerates the cameras, returning a bidirectional mapping between camera pointers and their names.
* */
class EnnumerateCameraEvent : public Event
{
	friend class EventSystem;

	std::shared_future<PropertyMapTmp<EdsCameraRef, std::string> > value;
	std::shared_ptr<std::promise<PropertyMapTmp<EdsCameraRef, std::string> > > promise;

public:
	EnnumerateCameraEvent() : Event(Event::Type::EnnumerateCameras)
	{
		promise = decltype(promise)(new std::promise<PropertyMapTmp<EdsCameraRef, std::string> >());
		value = promise->get_future();
	}

	PropertyMapTmp<EdsCameraRef, std::string> get()
	{
		return value.get();
	}

	void set(const PropertyMapTmp<EdsCameraRef, std::string>& mapping)
	{
		promise->set_value(mapping);
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
	std::queue<std::shared_ptr<Event> > mEvents;
	std::mutex mMutex, mBlockMutex;
	std::condition_variable mBlockCondition;
public:

	/** Trivial constructor. */
	EventSystem();

	/**Deleted copy constructor and operator. */
	EventSystem(const EventSystem&) = delete;
	void operator = (const EventSystem&) = delete;

	/** Sends an event to the action thread. */
	void send(const std::shared_ptr<Event>& e);

	/** pops the next list off the queue and presents it.
	* If there is nothing to poll, the method blocks the thread until there is.
	* */
	std::shared_ptr<Event> poll();
};