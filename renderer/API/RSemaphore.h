#pragma once

#include <mutex>
#include <condition_variable>

namespace RAPI
{
	class RSemaphore
	{
	public:
		using native_handle_type = std::condition_variable::native_handle_type;

		explicit RSemaphore(size_t n = 0);

		RSemaphore(const RSemaphore &) = delete;

		RSemaphore &operator=(const RSemaphore &) = delete;

		void notify();

		void wait();

		template<class Rep, class Period>
		bool wait_for(const std::chrono::duration<Rep, Period> &d);

		template<class Clock, class Duration>
		bool wait_until(const std::chrono::time_point<Clock, Duration> &t);

		native_handle_type native_handle();

	private:
		size_t count;
		std::mutex mutex;
		std::condition_variable cv;
	};

	inline RSemaphore::RSemaphore(size_t n)
			: count{n}
	{ }

	inline void RSemaphore::notify()
	{
		std::lock_guard<std::mutex> lock{mutex};
		++count;
		cv.notify_one();
	}

	inline void RSemaphore::wait()
	{
		std::unique_lock<std::mutex> lock{mutex};
		cv.wait(lock, [&] { return count > 0; });
		--count;
	}

	template<class Rep, class Period>
	bool RSemaphore::wait_for(const std::chrono::duration<Rep, Period> &d)
	{
		std::unique_lock<std::mutex> lock{mutex};
		auto finished = cv.wait_for(lock, d, [&] { return count > 0; });

		if (finished)
			--count;

		return finished;
	}

	template<class Clock, class Duration>
	bool RSemaphore::wait_until(const std::chrono::time_point<Clock, Duration> &t)
	{
		std::unique_lock<std::mutex> lock{mutex};
		auto finished = cv.wait_until(lock, t, [&] { return count > 0; });

		if (finished)
			--count;

		return finished;
	}

	inline RSemaphore::native_handle_type RSemaphore::native_handle()
	{
		return cv.native_handle();
	}
}