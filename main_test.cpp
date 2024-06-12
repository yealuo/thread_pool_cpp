#include <iostream>
#include "thread_pool.h"

bool print_test(int count) {
    for (int i = 0; i < count; ++i) {
        cout << i << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }

    return true;
}

int main() {
    thread_pool t_pool(5);
    auto res1 = t_pool.thread_pool_post(print_test, 3);
    auto res2 = t_pool.thread_pool_post(print_test, 5);

    cout << (res1.get() ? "true" : "false") << endl;
    cout << (res2.get() ? "true" : "false") << endl;

    return 0;
}
