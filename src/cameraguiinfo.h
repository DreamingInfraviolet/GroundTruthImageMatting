#pragma once
#include "EDSDKTypes.h"
#include <string>

/**
* This struct defines the basic information needed by the program GUI regarding a camera.
* */

struct CameraGuiInfo
{
	//The mapping between the camera reference pointer and its name.
	std::pair<EdsCameraRef, std::string> mapping;

	CameraGuiInfo(const std::pair<EdsCameraRef, std::string>& mapping_)
	{
		mapping = mapping_;
	}
};