#pragma once
#include <initializer_list>
#include <string>
#include <map>
#include <cassert>

/**
* The mapping between formal camera properties settable by the API and the actual human-readable values
* seems arbitrary, so these classes act to provide a simple mechanism for two-way mapping:
* To map camera property values to text,
* To map text to camera property values.
* Implemented using two std::maps, optimised for lookup performance.
* Asserts false if mapping not found.
* */
template <class LEFT, class RIGHT>
class PropertyMapTmp
{
public:
	std::map<LEFT, RIGHT> mForwardMap;
	std::map<RIGHT, LEFT> mBackwardMap;

	/** Trivial default constructor. */
	PropertyMapTmp() {}

	/**
	* A simple constructor initialising the map.
	* @param list The list of pairs describing the id=>string mapping.
	* */
	PropertyMapTmp(const std::initializer_list<std::pair<LEFT, RIGHT> >&  list)
	{
		for (auto pair : list)
			push_back(pair);
	}

	/** Inserts a pair into the map. */
	void push_back(const std::pair<LEFT, RIGHT>& pair)
	{
		mForwardMap.insert(pair);
		mBackwardMap.insert(std::make_pair(pair.second, pair.first));
	}

	/**
	* Returns the property string given the corresponding id.
	* */
	RIGHT operator[] (const LEFT propertyId) const
	{
		auto it = mForwardMap.find(propertyId);
		if (it == mForwardMap.end())
			assert(false);
		return it->second;
	}

	/**
	* Returns the property id given the corresponding string.
	* */
	LEFT         operator[] (const RIGHT& propertyStr) const
	{
		auto it = mBackwardMap.find(propertyStr);
		if (it == mBackwardMap.end())
			assert(false);
		return it->second;
	}
};

typedef PropertyMapTmp<int, std::string> PropertyMap;