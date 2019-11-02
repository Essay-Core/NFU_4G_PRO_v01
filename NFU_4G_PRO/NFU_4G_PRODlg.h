
// NFU_4G_PRODlg.h : 头文件
//

#pragma once
#include <stdio.h>
#include "afxwin.h"
#include <thread>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")
#include "C_SingleCircuList.h"
#include "cJson/cJSON.h"
#include "cJson/F_cjson.h"

#define MAX_QUEUE_SIZE 64

#define NON_RECV_DATA 1
#define CAN_RECV_DATA 2

typedef unsigned char uint8;
typedef unsigned int uint32;

//使用tcp收发控制消息
//使用UPD接收数据
typedef struct stZYNQ_Header
{
	int size;
	int type;
	char* data;
}stZYNQ_Header;


typedef struct stIpPortInitMsg
{
	char ip_romote[32];
	char ip_local[32];

	int port_ctrl;
	int port_data;

}stIpPortInitMsg;

// CNFU_4G_PRODlg 对话框
class CNFU_4G_PRODlg : public CDialogEx
{
// 构造
public:
	CNFU_4G_PRODlg(CWnd* pParent = NULL);	// 标准构造函数
	~CNFU_4G_PRODlg();
// 对话框数据
	enum { IDD = IDD_NFU_4G_PRO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
public:
	HICON m_hIcon;

	//config
	stIpPortInitMsg initmsg;

	//UDP
	SOCKET Usock_data;
	sockaddr_in clientAddr;

	//TCP
	WSADATA wsd;
	SOCKET m_socket;
	SOCKADDR_IN ser_addr;

	//msg
	stZYNQ_Header stZynqHead;

	//list
	CListBox *m_listbox;

	CIPAddressCtrl *m_IP_local;
	CIPAddressCtrl *m_IP_remote;

	CEdit *m_edit_UDP_port;
	CEdit *m_edit_TCP_port;

	CEdit *m_edit_UDP_IP;
	CEdit *m_edit_TCP_IP;

	PQUEUE Queue;

	char m_ip[32];
	char m_port[16];

	char *m_recv_data;

	//flags
	int m_flags_IF_exit;
	int m_flags_if_STOP;
	int m_flags_recv_data;
	int m_flags_write_data;
	int m_flags_write_finish;
	thread m_th_data;
	thread m_th_write;

	char m_showStr[64];

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedModuFreq();
	afx_msg void OnBnClickedStatus();
	afx_msg void OnBnClickedData();
	afx_msg void OnBnClickedClean();
	afx_msg void OnBnClickedStop();

	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();

	afx_msg void OnBnClickedDataStop();
};
   
int th_recv_data_new(void* th);
int th_write_data_new(void* th);

int ExtraData(int bit_pos, int bitsize,uint8 *data_p);