#include "stdafx.h"
#include "UDP_Server.h"

UDP_Server::UDP_Server()
{
	WSADATA wsd = { 0 };
	WSAStartup(0x0202, &wsd);
}


UDP_Server::~UDP_Server()
{
	WSACleanup();
}


int sendData(SOCKET m_socket, char* dataIn, int len)
{
	if (!dataIn || len < 0 || m_socket < 0)
	{
		return -1;
	}

	int send_ret = 0;
	int send_ret_add = 0;
	int send_len = len;

	while ((send_ret = send(m_socket, dataIn + send_ret_add, send_len - send_ret_add, 0)) < (send_len - send_ret_add))
	{
		if (send_ret < 0)
		{
			return -2;
		}
		send_ret_add += send_ret;
	}

	return send_ret_add;
}


int recvData(SOCKET m_socket, char* dataOUT, unsigned long recv_len)
{
	if (recv_len < 0 || m_socket < 0)
	{
		return -1;
	}

	int recv_ret = 0;
	unsigned long recv_ret_add = 0;

	char *recvBuf = (char*)malloc(sizeof(char)*recv_len+1);

	if (!recvBuf)
	{
		TRACE("malloc error\n");
		return -1;
	}

	while ( 1 )
	{
		recv_ret = recv(m_socket, recvBuf + recv_ret_add, recv_len - recv_ret_add, 0);
		if (recv_ret < 0)
		{
			TRACE("recv error.\n");
			free(recvBuf);
			return -2;
		}
		recv_ret_add += recv_ret;

		if (recv_ret_add >= recv_len)
		{
			break;
		}

	}

	if (dataOUT != NULL)
	{
		memcpy(dataOUT, recvBuf, sizeof(char)*recv_ret_add);
	}
	
	free(recvBuf);

	return 0;
}


void setnonblocking(SOCKET *sockfd)
{
	if (*sockfd < 0)
	{
		return;
	}

	int iMode = 1; //0：阻塞
	int ret = ioctlsocket(*sockfd, FIONBIO, (u_long FAR*) &iMode);//非阻塞设置
	if (ret < 0)
	{
		return;
	}

}

void setTimeOut(SOCKET *sockfd)
{
	if (*sockfd < 0)
	{
		return;
	}

	// 设置超时
	struct timeval timeout;
	timeout.tv_sec = 1000 * 3;//秒
	timeout.tv_usec = 0;//微秒

	if (setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == -1)
	{
		AfxMessageBox(_T("set time out false"));
		return;
	}
}

void showCurLineChar(CListBox *m_listbox, char* cStr)
{
	if (!m_listbox || !cStr)
	{
		TRACE("showCurLineChar Input error\n");
		return;
	}
	int count = 0;
	count = m_listbox->GetCount();//获取总行数

	USES_CONVERSION;
	A2CW(W2A(CString(cStr)));
	m_listbox->InsertString(count, A2CW(W2A(CString(cStr))));
	m_listbox->SetCurSel(count);//显示最后一行
}


bool ifSocketTrue(SOCKET *sock)
{
	if (INVALID_SOCKET == *sock)
	{
		DWORD m_dwErr = GetLastError();
		return false;
	}

	if (*sock <= 0)
	{
		return false;
	}

	return true;
}