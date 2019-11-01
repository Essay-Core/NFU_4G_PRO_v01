#include "socket_client.h"

std::mutex mtx;

socket_client::socket_client()
{
    //构造中使用 new,防止错误中断退出时发生内存泄漏
    //消息头
    //qDebug() << m_date->get_time();
    //m_msghead = new stMsgHead;
    //m_date = new date_time;

    memset(m_msghead, 0, sizeof (stMsgHead));
    m_msghead_len = sizeof(stMsgHead);

    m_heart = new stHeartBeat;

    m_heart->val = 0;
    //初始化套接字
    socket_init();
}

socket_client::~socket_client()
{
//     delete m_msghead;
//     delete m_heart;
    //delete m_date;
    socket_discon();
    // qDebug() << m_date->get_time();
}

int socket_client::socket_init()
{
    //初始化套接字
     if(WSAStartup(MAKEWORD(2,2),&wsd) != 0)
    {
        //qDebug() << "WSAStartup failed!";
        return -1;
    }

    //创建套接字，使用tcp流套接字
    m_socket = socket(AF_INET,SOCK_STREAM, 0);
    if(m_socket == INVALID_SOCKET)
    {
       //qDebug() << "socket init failed!";
       return -1;
    }

    char* ip = "127.0.0.1";
    u_short port = 9211;
    socket_connect(ip, port);
    return 0;
}

//为什么要传地址才可以？SOCKET *m_socket
int socket_client::socket_connect(char *ip, u_short port)
{
    if( !ip || port < 0)
        return -1;

    int n_serbAddlen = sizeof(SOCKADDR_IN);

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.S_un.S_addr = inet_addr(ip);
    ser_addr.sin_port = htons(port);

    //连接服务端，使用外部同名函数的方法是添加::
	int retValue = ::connect(m_socket, (SOCKADDR*)&ser_addr, n_serbAddlen);
    if(SOCKET_ERROR == retValue)
    {
       // qDebug() << "connect failed:" << WSAGetLastError();
        return -1;
    }
    //qDebug() << "connect successed " << "IP:"<< ip << "port:" << port;

    //线程初始化
    //thread_init();
    //thread_func(this);

    return 0;
}

/*
发送已一段数据，数据已处理完传参进来，及长度
接收数据
分析： 数据头+json格式 数据
返回结果 需要什么样的结果？只需要将json字符串返回
*/
int socket_client::send_recv(char *data, int send_len, char* result_js)
{
    //发送数据
    int send_ret = 0;
    int send_ret_add = 0;
    while((send_ret = send(m_socket, data +send_ret_add, send_len-send_ret_add, 0)) < send_len)
    {
        send_len = send_len - send_ret;
        send_ret_add += send_ret;
    }

    //接收数据
    char recvdata[256] = {0};
    int recv_ret = recv(m_socket, recvdata,sizeof (recvdata),0);

    memset(m_msghead, 0, sizeof (stMsgHead));
    memcpy((char*)result_js, recvdata, sizeof (stMsgHead));
    memcpy(result_js, recvdata+sizeof (stMsgHead), strlen(recvdata)-sizeof (stMsgHead));

	return 0;
}
// 
// int socket_client::socket_send_recv()
// {
//     //json格式数据请求
//     char ch_stReqLogin[128] = {0};
// 
//     //生成js格式字符串
//     cJSON *js_stReqLogin;
//     js_stReqLogin = cJSON_CreateObject();
// 
//     cJSON_AddStringToObject(js_stReqLogin, "uid", "admin");
//     cJSON_AddStringToObject(js_stReqLogin, "passwd","");
//     cJSON_AddStringToObject(js_stReqLogin, "nonce", "");
// 
//     sprintf(ch_stReqLogin,"%s",cJSON_PrintUnformatted(js_stReqLogin));
//     m_js_len = strlen(ch_stReqLogin);
// 
//     //消息头
//     sprintf(m_msghead->startCode,"DMSG");
//     m_msghead->msgType = htonl(MSGT_Login);
//     m_msghead->sysCate = htonl(SYST_FACE);
//     m_msghead->payloadLen = htonl(strlen(ch_stReqLogin));
// 
//     char sendbuf[sizeof (stMsgHead) + sizeof(ch_stReqLogin)] = {0};
// 
//     //消息头+json数据尾
//     memcpy(sendbuf, (char*)m_msghead, sizeof(stMsgHead));
//     memcpy(sendbuf+sizeof(stMsgHead), ch_stReqLogin, sizeof(ch_stReqLogin));
// 
//     //第一次发送数据
//     //发送的长度需要提前计算，不能使用sizeof()或strlen()
//     int send_len = m_msghead_len + m_js_len;
// 
//     ::mtx.lock();
//     int send_ret = send(m_socket, sendbuf, send_len, 0);
// 
//     //第一次接收数据
//     char recvdata[256] = {0};
//     int recv_ret = recv(m_socket, recvdata,sizeof (recvdata),0);
//     ::mtx.unlock();
// 
//     qDebug() << "recv_ret:" <<recv_ret;
//     qDebug() << "recvdata:" << recvdata;
// 
//     memset(m_msghead, 0, sizeof (stMsgHead));
//     memcpy((char*)m_msghead, recvdata, sizeof (stMsgHead));
// 
//     char json_str[128] = {0};
//     memcpy(json_str, recvdata+sizeof (stMsgHead), ntohl(m_msghead->payloadLen));
//     qDebug() << "recvdata:" << json_str;
// 
//     //////////////////////////////////////////////////
// 
//     char passwd_buf[64] = {0};
//     int passwd_int = 0;
//     m_js_t->analyze_js(json_str, "ec",passwd_buf, &passwd_int);
//     m_js_t->analyze_js(json_str, "ed",passwd_buf, &passwd_int);
// 
//     //获取用户密码
//     char result[64] = {0};
//     DUserCheck("admin","123456",passwd_buf,result);
//     qDebug() <<"\n DUserCheck result：" << result;
// 
//     //二次发送登陆指令
//     memset(sendbuf, 0, sizeof (sendbuf));
// 
//     //生成js格式字符串
//     cJSON *js;
//     js = cJSON_CreateObject();
//     char js_data[128] = {0};
// 
//     cJSON_AddStringToObject(js, "uid", "admin");
//     cJSON_AddStringToObject(js, "passwd",result);       //MD5返回结果 result
//     cJSON_AddStringToObject(js, "nonce", passwd_buf);   //服务端返回密码 passwd_buf
// 
//     sprintf(js_data,"%s",cJSON_PrintUnformatted(js));
//     qDebug() << "js_data:" << js_data;
// 
//     //设置消息头
//     memset(m_msghead, 0, sizeof (stMsgHead));
// 
//     sprintf(m_msghead->startCode,"DMSG");
//     m_msghead->msgType = htonl(MSGT_Login);
//     m_msghead->sysCate = htonl(SYST_FACE);
//     m_msghead->payloadLen = htonl(strlen(js_data));
//     qDebug() << "sizeof (js_data)" << sizeof (js_data);
//     qDebug() << "strlen(js_data)" << strlen(js_data);
// 
//     //清空
//     char second_send_buf[256] = {0};
// 
//     //消息头+jison数据
//     memcpy(second_send_buf, (char*)m_msghead, sizeof (stMsgHead));
//     memcpy(second_send_buf+sizeof (stMsgHead), js_data, strlen(js_data));
// 
//     send_len = m_msghead_len + strlen(js_data);
//     send_ret = 0;
//     int send_ret_add = 0;
// 
//     ::mtx.lock();
//     while((send_ret = send(m_socket, second_send_buf +send_ret_add, send_len-send_ret_add, 0)) < send_len)
//     {
//         send_len = send_len - send_ret;
//         send_ret_add += send_ret;
//     }
// 
//     qDebug() << "send_ret:" << send_ret;
// 
//     //第二次接收数据
//     memset(recvdata, 0, sizeof (recvdata));
//     recv_ret = recv(m_socket, recvdata,sizeof (recvdata),0);
// 
//     ::mtx.unlock();
// 
//     memset(m_msghead, 0, sizeof (stMsgHead));
//     memcpy((char*)m_msghead, recvdata, sizeof (stMsgHead));
// 
//     memset(json_str, 0, sizeof (json_str));
//     memcpy(json_str, recvdata+sizeof (stMsgHead), ntohl(m_msghead->payloadLen));
//     qDebug() << "recvdata:" << json_str;
// 
//     return 0;
// }
// 

int socket_client::socket_discon()
{
    if(!m_socket)
        return -1;

    //关闭连接套接字
    closesocket(m_socket);
    WSACleanup();
    //qDebug() << "close socket";


    return 0;
}

void socket_client::thread_init()
{
    //参数依次是：创建的线程id，线程参数，调用的函数，传入的函数参数
    //注意创建的函数    必须是静态的函数 static所修饰
    //pthread_create(&m_tid[0], NULL, thread_func, this);

}

void socket_client::thead_heart(uint8_t val)
{
    //设置消息头
//     memset(m_msghead, 0, sizeof (stMsgHead));
//     
//     sprintf(m_msghead->startCode,"DMSG");
//     m_msghead->msgType = htonl(MSGT_HeartBeat);
//     m_msghead->sysCate = htonl(SYST_FACE);
//     m_msghead->payloadLen = htonl(sizeof(stHeartBeat));
// 
//     memset(m_heart, 0, sizeof(stHeartBeat));
//     m_heart->val = val;
// 
//     char send_data[sizeof (stMsgHead) + sizeof (stHeartBeat)] = {0};
//     int send_len = sizeof (stMsgHead) + sizeof (stHeartBeat);
// 
//     memcpy(send_data, (char*)m_msghead, sizeof (stMsgHead));
//     memcpy(send_data+sizeof (stMsgHead), (char*)m_heart, sizeof (stHeartBeat));
// 
//     send(m_socket, send_data, send_len,0);
// 
//     char recv_data[128] = {0};
//     recv(m_socket, recv_data,sizeof (send_data), 0);
//     qDebug() << "recv_data:" << recv_data;
// 
//     memset(m_heart, 0, sizeof(stHeartBeat));
//     memcpy(m_heart, (char*)recv_data+sizeof (stMsgHead), sizeof (stHeartBeat));
//     qDebug() << "m_heart:" << m_heart->val;

}

// 线程的运行函数
void* thread_func(void* th)
{
//     while (1)
//     {
//         ::mtx.lock();
//         qDebug("thread_func lock start ...");
// 
// 
//         //如何在静态函数调用类成员变量？
//         //定义指针不会进入构造函数，因为没有定义对象，只是一个指针而已
//         socket_client *p = (socket_client*)th;
//         qDebug() << p->m_date->get_time();
// 
//         p->thead_heart(0);
//         p->thead_heart(1);
// 
//         ::mtx.unlock();
//         qDebug("lock end.");
// 
//         Sleep(5000);
//     }

	return NULL;
}
