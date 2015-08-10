#include "edsstreamcontainer.h"

void* EdsStreamContainer::pointer() const
{
	void* ptr;
	auto err = EdsGetPointer(mRef, &ptr);
	if (err != EDS_ERR_OK)
		return nullptr;
	else
		return ptr;
}

unsigned EdsStreamContainer::size() const
{
	EdsUInt32 s;
	auto err = EdsGetLength(mRef, &s);
	if (err != EDS_ERR_OK)
		return 0;
	else
		return s;
}
