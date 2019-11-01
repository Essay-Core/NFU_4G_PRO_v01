
// NFU_4G_PRO.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CNFU_4G_PROApp: 
// 有关此类的实现，请参阅 NFU_4G_PRO.cpp
//

class CNFU_4G_PROApp : public CWinApp
{
public:
	CNFU_4G_PROApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CNFU_4G_PROApp theApp;