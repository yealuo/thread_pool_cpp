//
// Created by yeluo on 24-6-9.
//

#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <type_traits>

using namespace std;

class thread_pool {
private:
    mutex mtx;
    condition_variable cond_var;
    bool stop;

    queue<function<void()>> task_queue;
    vector<thread> workers;

    void worker_thread();

public:
    explicit thread_pool(size_t thread_num);

    ~thread_pool();

    template<typename Func, typename... Args>
    auto thread_pool_post(Func &&func, Args &&... args) -> future<invoke_result_t<Func, Args...>>;

};

template<typename Func, typename... Args>
auto thread_pool::thread_pool_post(Func &&func, Args &&... args) -> future<invoke_result_t<Func, Args...>> {
    using return_type = invoke_result_t<Func, Args...>;

    auto task = make_shared<packaged_task<return_type()>>(
            bind(forward<Func>(func), forward<Args>(args)...));

    auto res = task->get_future();
    {
        unique_lock<mutex> lock(mtx);
        if (stop) {
            throw std::runtime_error("加入了一个停止的线程池");
        }
        task_queue.emplace([task]() { (*task)(); });
    }
    cond_var.notify_one();
    return res;
}

