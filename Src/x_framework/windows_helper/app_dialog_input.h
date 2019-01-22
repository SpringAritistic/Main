#pragma once
#include "../resource.h"

#include "app_dialog_base.h"

class X_FRAMEWORK_API app_dialog_input : public CXTPResizeDialog
{
	DECLARE_DYNAMIC(app_dialog_input)
	DECLARE_MESSAGE_MAP()

public:
	app_dialog_input(CWnd* pParent = NULL);
	virtual ~app_dialog_input();

public:
	enum { IDD = IDD_APP_DIALOG_INPUT };

public:
	CString						m_result_name;					// 输入结果，如果创建对象时指定内容则作为默认内容填充到输入框
	bool						m_checking_boot_result_name;	// 窗口加载的时候修正默认人结果名称保证与已有的名称不重复（默认false)
	std::vector<CString>		m_exist_names;					// 不允许出现的输入结果
	int							m_default_width;				// 窗口默认宽度
	int							m_default_left_pos;				// 默认为0会自动居中
	int							m_default_top_pos;				// 默认为0会自动居中
	CString						m_caption;						// 窗口默认标题
	CString						m_label;						// 窗口默认提示文字
	CString						m_repeat_message;				// 如果输入内容与已经存在的内容重复时弹出此消息，为空弹出默认消息

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* dx);
	virtual void OnOK();

protected:
	void adjust_window_layout();
};


