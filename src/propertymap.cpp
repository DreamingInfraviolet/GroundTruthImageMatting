#include "propertymap.h"

PropertyMap::PropertyMap(const std::initializer_list<std::pair<int, std::string> >&  list)
{
	for(auto pair : list)
	{
		mForwardMap.insert(pair);
		mBackwardMap.insert(std::make_pair(pair.second, pair.first));
	}
}

std::string PropertyMap::operator[] (const int propertyId) const
{
	auto it = mForwardMap.find(propertyId);
	if (it == mForwardMap.end())
		return "Not Found";
	else
		return it->second;
}

int         PropertyMap::operator[] (const std::string& propertyStr) const
{
	auto it = mBackwardMap.find(propertyStr);
	if (it == mBackwardMap.end())
		return -1;
	else
		return it->second;
}