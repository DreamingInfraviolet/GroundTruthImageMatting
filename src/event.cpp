#include "event.h"

EventSystem::EventSystem()
{
}

void EventSystem::send(const std::shared_ptr<Event>& e)
{
	std::lock_guard<std::mutex> guard(mMutex);
	mEvents.push(e);
	mBlockCondition.notify_one();
}

void EventSystem::send(const std::initializer_list<std::shared_ptr<Event>>& list)
{
	for (auto it = list.begin(); it != list.end(); ++it)
		send(*it);
}

std::shared_ptr<Event> EventSystem::poll()
{
	std::unique_lock<std::mutex> guard(mMutex);
	std::unique_lock<std::mutex> block(mBlockMutex);
	while (mEvents.size() == 0)
	{
		//Unlock to allow for the queue to be filled, and wait.
		guard.unlock();
		mBlockCondition.wait(block);
		guard.lock();
	}

	std::shared_ptr<Event> out = mEvents.front();
	mEvents.pop();
	return out;
}