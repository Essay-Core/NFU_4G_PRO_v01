
// NFU_4G_PRO.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNFU_4G_PROApp: 
// �йش����ʵ�֣������ NFU_4G_PRO.cpp
//

class CNFU_4G_PROApp : public CWinApp
{
public:
	CNFU_4G_PROApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNFU_4G_PROApp theApp;