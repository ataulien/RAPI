#pragma once

#if 0
class RRThreadPool {
public:
	RRThreadPool(size_t){}
};

#else
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace RAPI
{

    class RThreadPool
    {
    public:
        RThreadPool(size_t);

        template<class F, class... Args>
        auto enqueue(F &&f, Args &&... args)
                -> std::future<typename std::result_of<F(Args...)>::type>;

        ~RThreadPool();

        size_t getNumThreads()
        { return num_threads; }

        std::vector<size_t> getThreadIDs();

    private:
        // need to keep track of threads so we can join them
        std::vector<std::thread> workers;
        // the task queue
        std::queue<std::function<void()> > tasks;

        // synchronization
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
        size_t num_threads;
    };

// the constructor just launches some amount of workers
    inline RThreadPool::RThreadPool(size_t threads)
            : stop(false)
    {
        num_threads = threads;

        for (size_t i = 0; i < threads; ++i)
            workers.emplace_back(
                    [this]
                    {
                        for (; ;)
                        {
                            std::function<void()> task;

                            {
                                std::unique_lock<std::mutex> lock(this->queue_mutex);
                                this->condition.wait(lock,
                                                     [&]
                                                     { return this->stop || !this->tasks.empty(); });
                                if (this->stop && this->tasks.empty())
                                    return;
                                task = std::move(this->tasks.front());
                                this->tasks.pop();
                            }

                            task();
                        }
                    }
            );
    }

// add new work item to the pool
    template<class F, class... Args>
    auto RThreadPool::enqueue(F &&f, Args &&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()> >(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped RThreadPool");

            tasks.emplace([task]()
                          { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

// the destructor joins all threads
    inline RThreadPool::~RThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker: workers)
            worker.join();
    }

    inline std::vector<size_t> RThreadPool::getThreadIDs()
    {
#if defined(WIN32) || defined(_WIN32)
        std::vector<size_t> ids;
        for (std::thread& t : workers)
        {
            // Query windows for the thread ID
            ids.push_back(GetThreadId(reinterpret_cast<void*>(t.native_handle())));
        }
        return ids;
#else
        return std::vector<size_t>();
#endif
    }
}

#endif

#endif
