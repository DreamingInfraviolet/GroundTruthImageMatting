#pragma once
/** Defines the RawRgbEds typedef for referencing raw RGB data in an Eds stream.
 * The format is <width,height,EdsStream>
 * */

#include <tuple>
#include <fstream>
#include "edsstreamcontainer.h"

typedef std::tuple<int, int, EdsStreamContainer> RawRgbEds;

/** Saves the raw RGB values from the eds stream in a custom minimal format for use
by the Ground Truth application. */
static bool SaveRawRgbEds(const std::string& path, const RawRgbEds& rgb)
{
	int width = std::get<0>(rgb);
	int height = std::get<1>(rgb);
	const EdsStreamContainer& container = std::get<2>(rgb);

	if (width*height == 0 || container.size() == 0)
		return false;

	std::fstream out(path, std::ios::out | std::ios::trunc | std::ios::binary);
	if (out.fail())
		return false;

	out.seekp(0);
	out.write((char*)&width, sizeof(int));
	out.write((char*)&height, sizeof(int));
	out.write((char*)container.pointer(), container.size());

	return true;
}