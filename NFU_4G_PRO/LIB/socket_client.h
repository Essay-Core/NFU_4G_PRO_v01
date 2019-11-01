#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

/*
 * #系统默认库只需直接添加库名即可使用
 * LIBS += -lws2_32
 * 添加socket头文件
 *#include <WinSock2.h>

 _CRT_SECURE_NO_WARNINGS: 
	在属性 -> c++ -> 预处理器 -> 预处理器定义 -> 添加_CRT_SECURE_NO_WARNINGS

添加外部的头文件以及源文件时，不使用预编译头 才能编译通过
*/

#include <WinSock2.h>
// #include <json_test.h>
//#include "def_msgs.h"
// #include <dUserCheck.h>
// #include "date_time.h"

// #include <pthread.h>
// #include <QDebug>
#include <mutex>

#pragma comment(lib,"ws2_32.lib")


class socket_client
{
public:
    socket_client();
    ~socket_client();

public:
    int socket_init();
    int socket_connect(char *ip, u_short port);
    int socket_send_recv();
    int send_recv(char *data, int len, char* result_js);
    int socket_discon();

//线程
public:
    //pthread_t m_tid[1];

	void thread_init();
     void thead_heart(uint8_t val);

public:
    WSADATA wsd;
    SOCKET m_socket;
    SOCKADDR_IN ser_addr;

 //   stMsgHead *m_msghead;
    size_t m_msghead_len;
    size_t m_js_len;

//    stReqLogin *m_reqLogin;

    //json_test *m_js_t;
 //   stHeartBeat *m_heart;
 //   date_time *m_date;
    bool m_bval;

private:

};

//类成员线程函数必须为 static ,起作用相当于全局函数
void* thread_func(void* th);

#endif // SOCKET_CLIENT_H
