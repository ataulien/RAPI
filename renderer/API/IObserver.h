#pragma once

class IObserver
{
public:

	/**
	 * Called when the observer got notified
	 */
	virtual void OnNotified(unsigned int id, void* userData) = 0;
};