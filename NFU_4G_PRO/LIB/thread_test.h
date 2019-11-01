#ifndef THREAD_TEST_H
#define THREAD_TEST_H

#include <pthread.h>
#include <mutex>



class thread_test
{
public:
    thread_test();
    ~thread_test();

public:
    pthread_t m_tid[1];

    void thread_init();
    //类成员线程函数必须为 static ,起作用相当于全局函数
    static void* thread_func(void* th);

};

#endif // THREAD_TEST_H

