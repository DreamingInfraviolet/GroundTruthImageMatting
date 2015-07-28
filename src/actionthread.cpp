#include "event.h"
#include "io.h"
#include "camera.h"
#include "window.h"

int ActionThread(EventSystem* eventSystem, Window* window)
{
	Inform("Running action thread");

	//Initialise camera system
	Inform("Initialising camera");
	std::unique_ptr<CameraList> cameraList(CameraList::create(true));

	if (!cameraList.get())
		goto QUIT_ACTION_THREAD;

	if (cameraList->ennumerate() == 0)
	{
		Error("No cameras found");
		goto QUIT_ACTION_THREAD;
	}
	
	Camera& mainCamera = cameraList->cameras[0];
	if (!mainCamera.select())
	{
		Error("colould not select the main camera.");
		goto QUIT_ACTION_THREAD;
	}

	while (true)
	{
		std::shared_ptr<Event> eptr = eventSystem->poll();
		Event& event = *eptr;

		switch (event.type)
		{
		case Event::Type::Set:
			Inform("Receiving set event");
		{
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
			MetaEvent metaEvent = static_cast<MetaEvent&>(event);

			switch (metaEvent.metaType)
			{
			case MetaEvent::MetaType::Shutdown:
				return 0; //Normal return
			}
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

	//Emergency return, shutting down the window.
	//Goto statement justification: Cleanest way to quit to the error code without returning.
	QUIT_ACTION_THREAD:
	Inform("Closing window due to error");
	window->close();
	return 3;
}