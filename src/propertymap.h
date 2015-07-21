#pragma once
#include <initializer_list>
#include <string>
#include <map>

/**
* The mapping between formal camera properties settable by the API and the actual human-readable values
* seems arbitrary, so these classes act to provide a simple mechanism for two-way mapping:
* To map camera property values to text,
* To map text to camera property values.
* Implemented using two std::maps, optimised for lookup performance.
* */

class PropertyMap
{
public:
	std::map<int,std::string> mForwardMap;
	std::map<std::string,int> mBackwardMap;

	/**
	* A simple constructor initialising the map.
	* @param list The list of pairs describing the id=>string mapping.
	* */
	PropertyMap(const std::initializer_list<std::pair<int, std::string> >&  list);

	/**
	* Returns the property string given the corresponding id.
	* Returns "Not Found" if not found.
	* */
	std::string operator[] (const int propertyId) const;

	/**
	* Returns the property id given the corresponding string.
	* Returns -1 if not found.
	* */
	int         operator[] (const std::string& propertyStr) const;
};