#pragma once

////////////////////////////////////////////
#define UDP_TEST_PORT 7788
#define UDP_TEST_IP "127.0.0.1"

#define UDP_LOCAL_SERVER_PORT 5599
#define UDP_LOCAL_SERVER_IP "192.168.3.99"


#define TCP_REMOTE_SERVER_PORT 5588
#define TCP_REMOTE_SERVER_IP "192.168.3.100"

///////////////TCP消息状态类型/////////////////////////////
#define DT_START		001
#define DT_MODU_FREQ	002

#define DT_STOP         003
#define DT_STOP_REACT   004

#define DT_STATUS		005
#define DT_STATUS_REACT 006

#define DT_DATA			007
#define DT_DATA_REACT	008

////////////////////////////////////////////

#define OPEN_FILE_SIZE 130
////////////////////////////////////////////

class UDP_Server
{
public:
	UDP_Server();
	~UDP_Server();

public:
	
	
};

///////////////TCP数据收发/////////////////////////////
int sendData(SOCKET m_socket, char* dataIn, int len);

int recvData(SOCKET m_socket, char* dataOUT, unsigned long recv_len);

///////////设置超时与非阻塞/////////////////////////////////
void setnonblocking(SOCKET *sockfd);
void setTimeOut(SOCKET *sockfd);

///////////////LISTBOX控件写入字符串/////////////////////////////
void showCurLineChar(CListBox *m_listbox, char* cStr);

///////////判断套接字是否为真/////////////////////////////////
bool ifSocketTrue(SOCKET *sock);
