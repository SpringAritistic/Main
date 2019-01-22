#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"

class main_application : public CWinAppEx
{
	DECLARE_MESSAGE_MAP()

public:
	main_application();
	~main_application();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	CString get_recent_list_item_full_path(int index);
	int get_recent_list_item_count();

protected:
	void set_application_name(const CString& name);
	void load_recent_list_to_mfc();

protected:
	ULONG_PTR m_gdiplusToken;
	HANDLE m_single_instance_mutex = nullptr;

protected:
	afx_msg void on_app_about();
};

extern main_application the_main_app;
