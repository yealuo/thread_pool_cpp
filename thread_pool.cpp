//
// Created by yeluo on 24-6-9.
//
#include "thread_pool.h"

thread_pool::thread_pool(size_t thread_num) : stop(false) {
    for (int i = 0; i < thread_num; ++i)
        workers.emplace_back([this]() { worker_thread(); });
}

thread_pool::~thread_pool() {
    {
        lock_guard<mutex> lock(mtx);
        stop = true;
    }
    cond_var.notify_all();
    for (thread &t: workers)
        t.join();
}

void thread_pool::worker_thread() {
    while (true) {
        function<void()> task;
        {
            unique_lock<mutex> lock(mtx);
            cond_var.wait(lock, [this]() { return stop || !task_queue.empty(); });
            if (stop && task_queue.empty()) return;
            task = std::move(task_queue.front());
            task_queue.pop();
        }
        task();
    }
}
