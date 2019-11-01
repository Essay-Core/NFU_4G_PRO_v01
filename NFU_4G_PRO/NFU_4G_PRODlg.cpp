
// NFU_4G_PRODlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NFU_4G_PRO.h"
#include "NFU_4G_PRODlg.h"
#include "afxdialogex.h"

#include "UDP_Server.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CNFU_4G_PRODlg �Ի���



CNFU_4G_PRODlg::CNFU_4G_PRODlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNFU_4G_PRODlg::IDD, pParent)
{
	//��ʼ�����������ڴ�����
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


// CNFU_4G_PRODlg ��Ϣ�������

BOOL CNFU_4G_PRODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	FILE *f = fopen("config.ini", "rb");
	char freadStr[256] = { 0 };
	fread(freadStr, 1, 256, f);
	fclose(f);

	getJsStr(freadStr, "ip_romote", initmsg.ip_romote);
	getJsStr(freadStr, "ip_local", initmsg.ip_local);

	getJsInt(freadStr, "port_ctrl", &initmsg.port_ctrl);
	getJsInt(freadStr, "port_data", &initmsg.port_data);

	//�б�
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
	//1,��ʼ��UPD����ˣ��ȴ���������
	//�����շ����ݷ����
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CNFU_4G_PRODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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

	//��������ͷ
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

	//��ʼ��������
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

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//����״̬��Ϣ
	//������Ƶ��Ϣ
	memset((char*)&stZynqHead, 0, sizeof(stZynqHead));
	stZynqHead.type = DT_STATUS;
	int index = 1;
	stZynqHead.data = (char*)&index;
	stZynqHead.size = 8;

	//���ͺ���ջظ����ݲ�����������Ϊ����λ״̬��Ϣ160bit  == 20 �ֽ�
	//1�ֽ� == 8bit
	//int == 4�ֽ�
	//char == 1�ֽ�

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

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
		sprintf_s(m_showStr, "�����ӳɹ�,���ȶϿ�������\n");
		showCurLineChar(m_listbox, m_showStr);
		return;
	}

	char showStr[64] = { 0 };

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�����׽��֣�ʹ��tcp���׽���
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

	//���ó�ʱΪ3s
	setTimeOut(&m_socket);

	GetWindowTextA(*m_edit_TCP_IP, (LPSTR)m_ip, 16);
	GetWindowTextA(*m_edit_TCP_port, (LPSTR)m_port, 8);

	int n_serbAddlen = sizeof(SOCKADDR_IN);
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.S_un.S_addr = inet_addr(m_ip); //UDP_TEST_IP
	ser_addr.sin_port = htons(atoi(m_port));//TCP_REMOTE_SERVER_PORT

	//���ӷ���ˣ�ʹ���ⲿͬ�������ķ��������::
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

	//�����߳̿ɽ�������
	//m_flags_recv_data = 2;
	m_flags_IF_exit = 1;
	m_flags_write_finish = 1;

	m_th_data = thread(th_recv_data_new, this);
	m_th_write = thread(th_write_data_new, this);

	return;
}

void CNFU_4G_PRODlg::OnBnClickedClean()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//����б�
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

		//����д������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	sprintf_s(m_showStr, "�˳�����");
	showCurLineChar(m_listbox, m_showStr);

	//�˳��̣߳���������ֹͣ
	m_flags_IF_exit = -1;

	//ֹͣ�������� != 2
	m_flags_recv_data = 0;

	//�˳�
	CDialog::OnCancel();
}


void CNFU_4G_PRODlg::OnBnClickedDisconnect()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_socket)
	{
		closesocket(m_socket);
		m_socket = NULL;
		sprintf_s(m_showStr, "DisConnect success!");
		showCurLineChar(m_listbox, m_showStr);
	}
	//ֹͣ�������� != 2
	m_flags_recv_data = 0;

	//�˳��̣߳���������ֹͣ
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
