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

auto comp = [] (const std::pair<int32_t, std::function<void()>>& a, const std::pair<int32_t, std::function<void()>>& b)
        {
            return a.first > b.first;
        };


class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, int32_t priority, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
    std::vector<std::thread::id> getThreadIds();
    ~ThreadPool();
private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::priority_queue<std::pair<int32_t, std::function<void()>>, 
                        std::vector<std::pair<int32_t, std::function<void()>>>,
                        decltype(comp)> tasks;
    
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};
 
// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads) :
    tasks(comp),
    stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back(
            [this]
            {
                for(;;)
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    while(!this->stop && this->tasks.empty())
                        this->condition.wait(lock);
                    if(this->stop && this->tasks.empty())
                        return;
                    std::function<void()> task(this->tasks.top().second);
                    this->tasks.pop();
                    lock.unlock();
                    task();
                }
            }
        );
}

inline std::vector<std::thread::id> ThreadPool::getThreadIds()
{
    std::lock_guard<std::mutex> lock(queue_mutex);
    std::vector<std::thread::id> result;

    for(const auto& thread : workers)
        result.push_back(thread.get_id());

    return result;
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, int32_t priority, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    typedef typename std::result_of<F(Args...)>::type return_type;
    
    // don't allow enqueueing after stopping the pool
    if(stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push({priority, [task](){ (*task)(); }});
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(size_t i = 0;i<workers.size();++i)
        workers[i].join();
}

#endif
