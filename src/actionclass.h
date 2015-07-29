#pragma once

/**
* This class is used by the Window to perform general processing unrelated to the interface.
* */
class CameraList;
class ActionClass
{
	ActionClass();
	bool initialise();
	bool initialiseCameraSystem();

	std::unique_ptr<CameraList> mCameraList;

public:

	static ActionClass* create();
	~ActionClass();

	int iso()
	{

		if (CameraList::instance() == nullptr)
		{
			Error("Invalid camera list.");
			return;
		}
		Camera* camera = CameraList::instance()->activeCamera();
		if (camera == nullptr)
		{
			Error("Invalid camera pointer at " + ToString(__LINE__));
			return;
		}

		camera->iso(Camera::isoMappings[std::string(ui.BoxIso->itemText(value).toUtf8())]);
	}

	int aperture()
	{
		if (CameraList::instance() == nullptr)
		{
			Error("Invalid camera list.");
			return;
		}
		Camera* camera = CameraList::instance()->activeCamera();
		if (camera == nullptr)
		{
			Error("Invalid camera pointer at " + ToString(__LINE__));
			return;
		}

		camera->aperture(Camera::apertureMappings[std::string(ui.BoxAperture->itemText(value).toUtf8())]);
	}
	
	int shutter()
	{

	}

	void iso(int)
	{

	}
	void aperture(int)
	{

	}
	void shutter(int)
	{

	}

	bool shootSequence()
	{

	//Create SFML window
	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(800, 600), "Hello");

	//Wait until we should start
	auto currentTime = std::chrono::steady_clock::now();
	std::this_thread::sleep_for(startTime - currentTime);

	//Go!!!

	// For each colour
	for (auto it = colours.begin(); it != colours.end(); ++it)
	{
		QColor colour = QColor(std::string(it->toUtf8()).c_str());
		
		if (!colour.isValid())
		{
			Warning("Invalid colour!");
		}

		Inform(ToString(colour.red()));
		Inform(ToString(colour.green()));
		Inform(ToString(colour.blue()));


		CameraList* cl = CameraList::instance();
		if (!cl)
		{
			Error("Invalid camera list.");
			return;
		}

		Camera* camera = cl->activeCamera();
		if (!camera)
		{
			Error("Invalid camera.");
			return;
		}
		
		camera->shoot();

		//Call loop twice
		for (int i = 0; i < 2; ++i)
		{
			// check all the window's events that were triggered since the last iteration of the loop.
			//Twice.
			sf::Event event;
			while (window.pollEvent(event))
			{
				// "close requested" event: we close the window
				if (event.type == sf::Event::Closed)
					window.close();
				else if (event.type == sf::Event::KeyPressed)
				{
					sf::Keyboard::Key key = event.key.code;

					switch (key)
					{
					case sf::Keyboard::Key::Escape:
						window.close();
						break;
					}
				}
			}

			window.clear(sf::Color(colour.red(), colour.green(), colour.blue(), 255));
			window.display();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		camera->shoot();

		//Loop must be called while camera is not ready
		do
		{
			sf::Event event;
			while (window.pollEvent(event)) {}
		} while (!camera->readyToShoot());


		auto image = camera->retrieveLastImage();

		std::string path = "C:\\Anima\\ImageBackgroundRemoval\\build\\src\\captured";

		long long time = std::chrono::duration_cast<std::chrono::seconds> (
			std::chrono::steady_clock::now().time_since_epoch()).count();

		if (saveRaw)
			image.saveToFile(appendNameToPath(
			std::string(ToString(time) + ToString(".cr2")),
							path).c_str());

		if (saveProcessed)
		{
			auto imageRgba = image.asRGBA();
			imageRgba.saveToFile(appendNameToPath(
				std::string(ToString(time) + "." + processedExtension),
				path).c_str());
		}
	}
	}
};