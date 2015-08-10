#pragma once

#include "edscontainer.h"
/** Convenience container for a stream reference */
class EdsStreamContainer : public EdsContainer<EdsStreamRef, EdsImageRef>
{
public:
	void* pointer() const;
	unsigned size() const;
};

