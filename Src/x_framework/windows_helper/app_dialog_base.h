#pragma once
#include <afxdialogex.h>


// 桥博中对话框基类，当对话框有图标时自动设置为主程序的图标
class X_FRAMEWORK_API app_dialog_base : public CDialogEx
{
	DECLARE_SERIAL(app_dialog_base)
	DECLARE_MESSAGE_MAP()

public:
	app_dialog_base();
	app_dialog_base(UINT template_id, CWnd* parent = nullptr);
	app_dialog_base(LPCTSTR template_name, CWnd* parent = nullptr);
	virtual ~app_dialog_base();

protected:
	virtual BOOL OnInitDialog();
};

// 不合适改基类的对话框可以在创建或初始化时调用此接口设置
X_FRAMEWORK_API void set_window_icon_same_as_application(CWnd* wnd);
