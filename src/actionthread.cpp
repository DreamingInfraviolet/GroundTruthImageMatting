#include "event.h"
#include "io.h"

int ActionThread(EventSystem* eventSystem)
{
	Inform("Running action thread");



	while (true)
	{
		std::shared_ptr<Event> esp = eventSystem->poll();
		Event& event = *esp.get();

		switch (event.type)
		{
		case Event::Type::Get:
		{
			Inform("Receiving get event");
			GetEvent& getEvent = dynamic_cast<GetEvent&>(event);
			getEvent.set(80);
		}
		break;
		case Event::Type::Set:
			Inform("Receiving set event");
		{
		}
		break;
		case Event::Type::Ennumerate:
		{
			EnnumerateEvent& enumEvent = dynamic_cast<EnnumerateEvent&>(event);
			Inform("Receiving ennumerate event");
			enumEvent.set({ 77,80 });
		}
		break;
		case Event::Type::Shoot:
		{
			Inform("Receiving shoot event");
			ShootEvent& shootEvent = dynamic_cast<ShootEvent&>(event);
		}
		break;
		case Event::Type::Meta:
		{
			Inform("Receiving meta event");
			MetaEvent& metaEvent = dynamic_cast<MetaEvent&>(event);
		}
		break;
		case Event::Type::EnnumerateCameras:
		{
			Inform("Receiving Ennumerate Cameras event in action thread");
			EnnumerateCameraEvent& cameraEvent = dynamic_cast<EnnumerateCameraEvent&>(event);
			cameraEvent.set({});
		}
		break;
		case Event::Type::None:
		{
			Warning("Receiving empty event in action thread");
		}
		break;
		default:
			assert(false);
		}
	}

	return 0;
}