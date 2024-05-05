#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class ThreadPool
{
public:
    ThreadPool(int threads);
    template <class F, class... Args>
    void enqueue(F &&function, Args &&...args);
    ~ThreadPool(); 

private:
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks;
    std::condition_variable _condition;
    std::mutex _queue_mutex;
    bool _stop = false;
};

// we use the inline keyword
// because in theory it makes things quicker 
inline ThreadPool::ThreadPool(int threads)
{
    for (int i = 0; i < threads; i++)
    {
        _workers.emplace_back(
            [this]
            {
                while (true)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->_queue_mutex);
                        this->_condition.wait(lock, [this]
                                              { return this->_stop || !this->_tasks.empty(); });
                        if (this->_stop && this->_tasks.empty())
                        {
                            return;
                        }
                        task = std::move(this->_tasks.front());
                        this->_tasks.pop();
                    }

                    task();
                }
            });
    }
}

inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _stop = true;
    }
    _condition.notify_all();
    for (std::thread &worker : _workers)
    {
        worker.join();
    }
}

template <class F, class... Args>
void ThreadPool::enqueue(F &&function, Args &&...args)
{
    auto task = std::bind(std::forward<F>(function), std::forward<Args>(args)...);
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _tasks.emplace(task);
    }
    _condition.notify_one();
}