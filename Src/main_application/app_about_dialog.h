#pragma once

#include "resource.h"
#include "x_framework/macro.h"
#include "x_framework/windows_helper/app_dialog_base.h"


class app_about_dialog : public app_dialog_base
{
	DECLARE_SERIAL(app_about_dialog)
	DECLARE_MESSAGE_MAP()

public:
	app_about_dialog(CWnd* parent = nullptr);
	virtual ~app_about_dialog();

public:
	enum { IDD = IDD_ABOUTBOX };

private:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

private:
	void layout_window();

private:
	afx_msg void on_button_ok_clicked();
	afx_msg void OnSize(UINT flag, int cx, int cy);
	afx_msg void OnSizing(UINT flag, LPRECT rc);
};


