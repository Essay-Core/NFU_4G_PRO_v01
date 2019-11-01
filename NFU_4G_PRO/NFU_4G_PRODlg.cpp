
// NFU_4G_PRODlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NFU_4G_PRO.h"
#include "NFU_4G_PRODlg.h"
#include "afxdialogex.h"

#include "UDP_Server.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNFU_4G_PRODlg 对话框



CNFU_4G_PRODlg::CNFU_4G_PRODlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNFU_4G_PRODlg::IDD, pParent)
{
	//初始化接收数据内存区域
	m_recv_data = (char *)malloc(sizeof(char)* 1024 * 1024 * 1);


	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Queue = new QUEUE;

	//add
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		perror("WSAStartup error");
		return;
	}

	m_flags_IF_exit = 1;
	m_flags_recv_data = 1;
	m_flags_write_data = 1;
	m_flags_if_STOP = 1;

// 	m_th_data = thread(th_recv_data_new, this);
// 	m_th_write = thread(th_write_data_new, this);

	CreateQueue(Queue, MAX_QUEUE_SIZE);

	
}

CNFU_4G_PRODlg::~CNFU_4G_PRODlg()
{
	if (Queue)
	{
		delete Queue;
	}

	free(m_recv_data);

	Clequeue(Queue);
	DestroyQueue(Queue);

	m_flags_IF_exit = 0;
	if (m_th_write.joinable())
	{
		m_th_write.join();
	}

	if (m_th_data.joinable())
	{
		m_th_data.join();
	}
	
	//UDP socket
	closesocket(Usock_data);

	WSACleanup();
}

void CNFU_4G_PRODlg::DoDataExchange(CDataExchange* pDX)
{

	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNFU_4G_PRODlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BT_START, &CNFU_4G_PRODlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_DATA, &CNFU_4G_PRODlg::OnBnClickedData)
//	ON_BN_CLICKED(IDC_MODU_FREQ, &CNFU_4G_PRODlg::OnBnClickedModuFreq)
	ON_BN_CLICKED(IDC_STATUS, &CNFU_4G_PRODlg::OnBnClickedStatus)
	ON_BN_CLICKED(IDC_CONNECT, &CNFU_4G_PRODlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_CLEAN, &CNFU_4G_PRODlg::OnBnClickedClean)

	ON_BN_CLICKED(IDC_STOP, &CNFU_4G_PRODlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_DISCONNECT, &CNFU_4G_PRODlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_DATA_STOP, &CNFU_4G_PRODlg::OnBnClickedDataStop)
END_MESSAGE_MAP()


// CNFU_4G_PRODlg 消息处理程序

BOOL CNFU_4G_PRODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	FILE *f = fopen("config.ini", "rb");
	char freadStr[256] = { 0 };
	fread(freadStr, 1, 256, f);
	fclose(f);

	getJsStr(freadStr, "ip_romote", initmsg.ip_romote);
	getJsStr(freadStr, "ip_local", initmsg.ip_local);

	getJsInt(freadStr, "port_ctrl", &initmsg.port_ctrl);
	getJsInt(freadStr, "port_data", &initmsg.port_data);

	//列表
	m_listbox = (CListBox*)GetDlgItem(IDC_LIST_SHOW);
	m_edit_UDP_port = (CEdit*)GetDlgItem(IDC_EDIT_LOCAL_PORT);
	m_edit_TCP_port = (CEdit*)GetDlgItem(IDC_EDIT_REMO_PORT);
	m_edit_UDP_IP = (CEdit*)GetDlgItem(IDC_EDIT_UDP_SERVER_IP);
	m_edit_TCP_IP = (CEdit*)GetDlgItem(IDC_EDIT_TCP_CLIENT_IP);

	USES_CONVERSION;
	//SetDlgItemText(IDC_FILENAME, A2CW(W2A(CString(m_filename))));
	SetDlgItemInt(IDC_EDIT_LOCAL_PORT, initmsg.port_data);//UDP_LOCAL_SERVER_PORT
	SetDlgItemInt(IDC_EDIT_REMO_PORT, initmsg.port_ctrl);//TCP_REMOTE_SERVER_PORT

	SetDlgItemText(IDC_EDIT_UDP_SERVER_IP, A2CW(W2A(CString(initmsg.ip_local))));//UDP_LOCAL_SERVER_IP
	SetDlgItemText(IDC_EDIT_TCP_CLIENT_IP, A2CW(W2A(CString(initmsg.ip_romote))));//TCP_REMOTE_SERVER_IP


#ifdef TTT
	//1,初始化UPD服务端，等待接收数据
	//创建收发数据服务端
	Usock_data = socket(AF_INET, SOCK_DGRAM, 0);
	if (Usock_data == INVALID_SOCKET)
	{
		perror("init socket error");
		exit(1);
	}

	sockaddr_in sAddr = { 0 };
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(initmsg.port_data);//UDP_TEST_PORT UDP_LOCAL_SERVER_PORT
	sAddr.sin_addr.S_un.S_addr = inet_addr(initmsg.ip_local); //initmsg UDP_TEST_IP

	int addrLen = sizeof(sockaddr);
	int retBind = ::bind(Usock_data, (sockaddr*)&sAddr, addrLen);	
#endif


	char showStr[128] = { 0 };

//	sprintf_s(showStr, "Init show Box OK!");
//	showCurLineChar(m_listbox, showStr);
	
	sprintf_s(showStr, "ip_local: %s,		port_data: %d", initmsg.ip_local, initmsg.port_data);
	showCurLineChar(m_listbox, showStr);

	sprintf_s(showStr, "ip_romote: %s,  	port_ctrl: %d", initmsg.ip_romote, initmsg.port_ctrl);
	showCurLineChar(m_listbox, showStr);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNFU_4G_PRODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNFU_4G_PRODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNFU_4G_PRODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNFU_4G_PRODlg::OnBnClickedStart()
{
	if (!ifSocketTrue(&m_socket))
	{
		sprintf_s(m_showStr, "pls click CONNECT bt!");
		showCurLineChar(m_listbox, m_showStr);
		return;
	}
	
	memset((char*)&stZynqHead, 0, sizeof(stZynqHead));
	stZynqHead.type = DT_START;
	int index = 1;
	stZynqHead.data = (char*)&index;
	stZynqHead.size = 12;

	int send_ret = 0;
	int send_ret_add = 0;
	int send_len = stZynqHead.size;
		
	send_ret = sendData(m_socket, (char*)&stZynqHead, send_len);
	if (send_ret < 0)
	{
		TRACE("sendData error\n");
		return;
	} 
	
	TRACE("pls sleep 5s\n");
	Sleep(5000);

	//接收数据头
	int recv_ret= recv(m_socket, (char*)&stZynqHead, 12, 0);
	if (recv_ret < 0)
	{
		TRACE("recv error\n");
		return;
	}

	if (stZynqHead.type != DT_MODU_FREQ)
	{
		TRACE("recv error type.\n");
		return;
	}

	//recv_ret = recv(m_socket, )

	//TRACE("stZynqHead.size:%d\n", stZynqHead.size);
	//recvData(m_socket, NULL,  stZynqHead.size-4);

	//开始接收数据
	m_flags_recv_data = 2;
	
	//sprintf_s(m_showStr, "start recv data...");
	//showCurLineChar(m_listbox, m_showStr);

}

void CNFU_4G_PRODlg::OnBnClickedStatus()
{
	if (!ifSocketTrue(&m_socket))
	{
		sprintf_s(m_showStr, "pls click CONNECT bt!");
		showCurLineChar(m_listbox, m_showStr);
		return;
	}

	// TODO:  在此添加控件通知处理程序代码
	//发生状态消息
	//发生调频消息
	memset((char*)&stZynqHead, 0, sizeof(stZynqHead));
	stZynqHead.type = DT_STATUS;
	int index = 1;
	stZynqHead.data = (char*)&index;
	stZynqHead.size = 8;

	//发送后接收回复数据并解析。数据为比特位状态信息160bit  == 20 字节
	//1字节 == 8bit
	//int == 4字节
	//char == 1字节

	sendData(m_socket, (char*)&stZynqHead, stZynqHead.size);
	
	char statusStr[32] = { 0 };
	int ret = recv(m_socket, statusStr,  28, 0);
	stZYNQ_Header *stHeader = (stZYNQ_Header*)statusStr;
	if (stHeader->type != DT_STATUS_REACT)
	{
		TRACE("recv type error\n");
		return;
	}

	TRACE("size:%d, type:%d, data:%s", stHeader->size, stHeader->type, stHeader->data);
	sprintf_s(m_showStr, "size:%d, type:%d, data:%s", stHeader->size, stHeader->type, stHeader->data);
	showCurLineChar(m_listbox, m_showStr);
	return;
}


void CNFU_4G_PRODlg::OnBnClickedData()
{
	if (!ifSocketTrue(&m_socket))
	{
		sprintf_s(m_showStr, "pls click CONNECT bt!");
		showCurLineChar(m_listbox, m_showStr);
		return;
	}

	// TODO:  在此添加控件通知处理程序代码
	memset((char*)&stZynqHead, 0, sizeof(stZynqHead));
	stZynqHead.type = DT_DATA;
	stZynqHead.data = NULL;
	stZynqHead.size = 8;

	sendData(m_socket, (char*)&stZynqHead, stZynqHead.size);

#ifdef TTT
	recv(m_socket, (char*)&stZynqHead.size, 4, 0);
	if (stZynqHead.size < 0)
	{
		return;
	}

	TRACE("stZynqHead.size:%d\n", stZynqHead.size);
	recvData(m_socket, NULL,  stZynqHead.size-4);
#endif

}


void CNFU_4G_PRODlg::OnBnClickedConnect()
{
	if (m_socket)
	{
		sprintf_s(m_showStr, "已连接成功,请先断开再连接\n");
		showCurLineChar(m_listbox, m_showStr);
		return;
	}

	char showStr[64] = { 0 };

	// TODO:  在此添加控件通知处理程序代码
	//创建套接字，使用tcp流套接字
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		perror("socket error");
		sprintf_s(showStr, "socket init error!");
		showCurLineChar(m_listbox, showStr);
		return;
	}

	sprintf_s(m_showStr, "socket init success");
	showCurLineChar(m_listbox, m_showStr);

	//设置超时为3s
	setTimeOut(&m_socket);

	GetWindowTextA(*m_edit_TCP_IP, (LPSTR)m_ip, 16);
	GetWindowTextA(*m_edit_TCP_port, (LPSTR)m_port, 8);

	int n_serbAddlen = sizeof(SOCKADDR_IN);
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.S_un.S_addr = inet_addr(m_ip); //UDP_TEST_IP
	ser_addr.sin_port = htons(atoi(m_port));//TCP_REMOTE_SERVER_PORT

	//连接服务端，使用外部同名函数的方法是添加::
	int retValue = ::connect(m_socket, (SOCKADDR*)&ser_addr, n_serbAddlen);
	if (SOCKET_ERROR == retValue)
	{
		sprintf_s(showStr, "Connect error!");
		showCurLineChar(m_listbox, showStr);

		closesocket(m_socket);
		m_socket = NULL;

		return;
	}
	
	sprintf_s(showStr, "Connect success!");
	showCurLineChar(m_listbox, showStr);

	sprintf_s(showStr, "Connect ip:%s,port:%s", m_ip, m_port);
	showCurLineChar(m_listbox, showStr);

	//设置线程可接收数据
	//m_flags_recv_data = 2;
	m_flags_IF_exit = 1;
	m_flags_write_finish = 1;

	m_th_data = thread(th_recv_data_new, this);
	m_th_write = thread(th_write_data_new, this);

	return;
}

void CNFU_4G_PRODlg::OnBnClickedClean()
{
	// TODO:  在此添加控件通知处理程序代码
	//清空列表
	m_listbox->ResetContent();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
int th_recv_data_new(void* th)
{
	TRACE("th_recv_data_new RUNNING...\n");
	
	CNFU_4G_PRODlg *pp = (CNFU_4G_PRODlg*)th;

	int retLen = 0;
	int Len = sizeof(sockaddr_in);
	int flags = 0;

	_DATA *recvBuf = (_DATA *)malloc(sizeof(_DATA)+1);
	int nu = 0;
	while (true)
	{
		if (pp->m_flags_recv_data == 2)
		{
			memset(recvBuf, 0, sizeof(_DATA));
			retLen = recvData(pp->m_socket, (char*)recvBuf, sizeof(_DATA));
			if (retLen < 0)
			{
				DWORD err = GetLastError();
				pp->m_flags_recv_data = 0; 
				pp->m_flags_write_finish = 0;
				continue;
			}

			while (1)
			{
				if (Enqueue(pp->Queue, recvBuf))
				{
					break;
				}
				Sleep(100);
			}
			Sleep(10);
		}

		if (pp->m_flags_IF_exit <= 0)
		{
			break;
		}		

		Sleep(10);
	}

	free(recvBuf);
	TRACE("th_recv_data_new BYTE!!! nu:%d\n", nu);
	
	return 0;
}

int th_write_data_new(void* th)
{
	TRACE("th_write_data_new RUNNING...\n");

	CNFU_4G_PRODlg *pp = (CNFU_4G_PRODlg*)th;

	FILE *fBuf[OPEN_FILE_SIZE] = { 0 };

	char filename[64] = { 0 };
	int openfileNu = OPEN_FILE_SIZE;

	tm *m_time;
	time_t t;
	t=time(NULL);
	m_time = localtime(&t);
	
	sprintf_s(filename, "./RecvData/%04d%02d%02d-%02d%02d%02d.ini", 
		m_time->tm_year+1900,
		m_time->tm_mon+1,
		m_time->tm_mday,
		m_time->tm_hour,
		m_time->tm_min,
		m_time->tm_sec);

	FILE *fw = fopen(filename, "ab+");
	_DATA *data = new _DATA;
	size_t retWriteLen = 0;
	size_t retWriteTmp = 0;
	size_t dataSize = sizeof(data->data);

	sprintf_s(pp->m_showStr, "Open data file!");
	showCurLineChar(pp->m_listbox, pp->m_showStr);

	while (true)
	{
		memset(data, 0, sizeof(_DATA));

		//出队写入数据
		if (Dequeue(pp->Queue, data))
		{
			retWriteLen = fwrite(data->data, 1, dataSize, fw);
		}

		if (pp->m_flags_write_finish <= 0)
		{
			pp->m_flags_write_finish = 1;
			
			if (fw)
			{
				fclose(fw);
				fw = NULL;
				sprintf_s(pp->m_showStr, "Close data file!");
				showCurLineChar(pp->m_listbox, pp->m_showStr);
			}
			continue;
		}

		if (pp->m_flags_IF_exit <= 0)
		{
			break;
		}
	}

	if (fw)
	{
		fclose(fw);
		fw = NULL;
	}
	
	delete data;

	return 0;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void CNFU_4G_PRODlg::OnBnClickedStop()
{
	// TODO:  在此添加控件通知处理程序代码
	sprintf_s(m_showStr, "退出程序");
	showCurLineChar(m_listbox, m_showStr);

	//退出线程，结束，或停止
	m_flags_IF_exit = -1;

	//停止接收数据 != 2
	m_flags_recv_data = 0;

	//退出
	CDialog::OnCancel();
}


void CNFU_4G_PRODlg::OnBnClickedDisconnect()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_socket)
	{
		closesocket(m_socket);
		m_socket = NULL;
		sprintf_s(m_showStr, "DisConnect success!");
		showCurLineChar(m_listbox, m_showStr);
	}
	//停止接收数据 != 2
	m_flags_recv_data = 0;

	//退出线程，结束，或停止
	m_flags_IF_exit = -1;

	if (m_th_write.joinable())
	{
		m_th_write.join();
	}

	if (m_th_data.joinable())
	{
		m_th_data.join();
	}

}


void CNFU_4G_PRODlg::OnBnClickedDataStop()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!ifSocketTrue(&m_socket))
	{
		sprintf_s(m_showStr, "pls click CONNECT bt!");
		showCurLineChar(m_listbox, m_showStr);
		return;
	}
	
	memset((char*)&stZynqHead, 0, sizeof(stZynqHead));
	stZynqHead.type = DT_STOP;
	int index = 1;
	stZynqHead.data = (char*)&index;
	stZynqHead.size = 12;

	int send_ret = 0;
	int send_ret_add = 0;
	int send_len = stZynqHead.size;
		
	send_ret = sendData(m_socket, (char*)&stZynqHead, send_len);
	if (send_ret < 0)
	{
		TRACE("sendData error\n");
		return;
	} 

	int recv_ret= recv(m_socket, (char*)&stZynqHead, 8, 0);
	if (recv_ret < 0 || stZynqHead.type != DT_STOP_REACT)
	{
		TRACE("recv error\n");
		return;
	}

	return;
}
