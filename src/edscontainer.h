#pragma once
#include "EDSDK.h"
#include "EDSDKTypes.h"
#include <memory>
/**
* This class is used to encapsulate an Eds object in a safe, reference-counted manner.
* The Object template is the object to take ownership of.
* The Depends object is an object which must exist for the lifetime of Object, but is not taken ownership of.
* */
template<class Object, class Depends=EdsBaseRef>
class EdsContainer
{
	Depends mDepends = nullptr;
public:

	Object mRef = nullptr;

	/** Trivial constructor. */
	EdsContainer() {}

	/** Copy constructor */
	EdsContainer(const EdsContainer<Object, Depends>& c)
	{
		copyFrom(c);
	}

	/** Move constructor */
	EdsContainer(EdsContainer<Object, Depends>&& c)
	{
		moveFrom(std::move(c));
	}

	/** Copy operator. */
	virtual EdsContainer& operator = (const EdsContainer<Object,Depends>& c)
	{
		copyFrom(c);
		return *this;
	}

	/** Move operator. */
	virtual EdsContainer& operator = (EdsContainer<Object, Depends>&& c)
	{
		moveFrom(std::move(c));
		return *this;
	}

	/** Copies from the source */
	void copyFrom(const EdsContainer<Object, Depends>& e)
	{
		clear();
		mRef = e.mRef;
		EdsRetain(mRef);
		mDepends = e.mDepends;
		EdsRetain(mDepends);
	}

	/** Moves from the source */
	void moveFrom(EdsContainer<Object, Depends>&& e)
	{
		clear();
		mRef = e.mRef;
		e.mRef = nullptr;
		mDepends = e.mDepends;
		e.mDepends = nullptr;
	}

	/** Releases the stream. */
	virtual ~EdsContainer() { clear(); }

	/** This must be set if the image reference may go out of scope before the scream reference. */
	virtual void setDepends(Depends ref)
	{
		mDepends = ref;
		EdsRetain(mDepends);
	}

	/** Deletes the stream */
	virtual void clear()
	{
		if (mRef)
			EdsRelease(mRef);
		if (mDepends)
			EdsRelease(mDepends);
		mRef = nullptr;
		mDepends = nullptr;
	}

};