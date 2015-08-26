#pragma once
/** Defines the RawRgbEds typedef for referencing raw RGB data in an char vector.
 * The format is <width,height,vector>
 * */

#include <tuple>
#include <vector>
#include <fstream>
#include "io.h"
#include <stdint.h>

typedef std::tuple<int, int, std::vector<uint16_t> > RawRgbChar;

/** Loads the raw RGB values into a char stream, intended for the Ground Truth application. */
static bool LoadRawRgb(const std::string& path, RawRgbChar& out)
{
	std::fstream in(path, std::ios::in | std::ios::binary);
	if (in.fail())
		return false;

	int& width = std::get<0>(out);
	int& height = std::get<1>(out);
	std::vector<uint16_t>& container = std::get<2>(out);

	in.seekg(0);
	in.read((char*)&width, sizeof(int));
	in.read((char*)&height, sizeof(int));

	if (width*height == 0)
		return false;

	container.resize(width*height*3);
	in.read((char*)&container[0], container.size()*sizeof(uint16_t));

	return true;
}
