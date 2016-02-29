#pragma once
#include <set>
#include "IObserver.h"

namespace RAPI
{
	class RObservable
	{
	public:

		/**
		* Registers an observer
		*/
		void RegisterObserver(IObserver *observer)
		{
			Observers.insert(observer);
		}

		/**
		* Registers an observer-lambda. These are mapped to the given ptr.
		*/
		void RegisterObserver(void* ptr, const std::function<void(unsigned int, void*)>& observerLambda)
		{
			ObserverFns.insert(std::make_pair(ptr, observerLambda));
		}

		/**
		* Unregisters an observer
		*/
		void RemoveObserver(IObserver *observer)
		{
			Observers.erase(observer);
		}

		/**
		* Unregisters an observer by the pointer given together with a lambda-function
		*/
		void RemoveObserver(void* lPtr)
		{
			ObserverFns.erase(lPtr);
		}

		/**
		 * Notifies all registered observers, broadcasting the given data
		 */
		void NotifyAll(unsigned int id, void *userdata)
		{
			for (IObserver *obs : Observers) {
				obs->OnNotified(id, userdata);
			}

			for (auto& fn : ObserverFns) {
				fn.second(id, userdata);
			}
		}

	private:
		std::set<IObserver *> Observers;
		std::map<void*, std::function<void(unsigned int, void*)>> ObserverFns;
	};
}