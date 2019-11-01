#include "thread_test.h"

std::mutex mtx;

thread_test::thread_test()
{

}

thread_test::~thread_test()
{
    //等各个线程退出后，进程才结束，否则进程强制结束了，线程可能还没反应过来；
    pthread_exit(NULL);

}

void thread_test::thread_init()
{
    //参数依次是：创建的线程id，线程参数，调用的函数，传入的函数参数
    //注意创建的函数    必须是静态的函数 static所修饰
    int ret = pthread_create(&m_tid[0], NULL, thread_test::thread_func, NULL);
    if (ret != 0)
    {
       printf("pthread_create error: error_code=%d \n",ret);

       return;
    }

    //上锁
    ::mtx.lock();
    int i = 0;
    while(1)
    {
        if(i++ == 5)
        {
            break;
        }
        printf("the main test thread \n");
      
    }
    //解锁
    ::mtx.unlock();
}

// 线程的运行函数
void* thread_test::thread_func(void* th)
{
    ::mtx.lock();
    printf("thread_func lock start ...\n");



    ::mtx.unlock();
    printf("lock end.\n");


}

