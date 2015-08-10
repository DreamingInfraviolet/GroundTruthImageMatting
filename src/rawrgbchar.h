#pragma once
/** Defines the RawRgbEds typedef for referencing raw RGB data in an char vector.
 * The format is <width,height,vector>
 * */

#include <tuple>
#include <vector>
#include <fstream>
#include "io.h"

typedef std::tuple<int, int, std::vector<unsigned char> > RawRgbChar;

/** Loads the raw RGB values into a char stream, intended for the Ground Truth application. */
static bool LoadRawRgb(const std::string& path, RawRgbChar& out)
{
	Inform("Loading raw RGB " + path);
	std::fstream in(path, std::ios::in | std::ios::binary);
	if (in.fail())
		return false;

	int& width = std::get<0>(out);
	int& height = std::get<1>(out);
	std::vector<unsigned char>& container = std::get<2>(out);

	in.seekg(0);
	in.read((char*)&width, sizeof(int));
	in.read((char*)&height, sizeof(int));

	if (width*height == 0)
		return false;

	container.resize(width*height*sizeof(unsigned char) * 3);
	in.read((char*)&container[0], container.size());

	return true;
}
