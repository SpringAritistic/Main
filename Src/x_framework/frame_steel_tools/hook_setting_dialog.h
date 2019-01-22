#pragma once
#include "../resource.h"
#include "hook_setting_data.h"


class CTHExcel;
class CTHDataBase;


class X_FRAMEWORK_API dialog_node_hook_setting : public CDialog
{
	DECLARE_DYNAMIC(dialog_node_hook_setting)
	DECLARE_MESSAGE_MAP()

public:
	dialog_node_hook_setting(CWnd* parent = nullptr);
	virtual ~dialog_node_hook_setting();

public:
	enum { IDD = IDD_DIALOG_HOOK_SETTING };

public:
	CString		m_setting_info_text;		// 表格中的数据
	CString		m_ref_lines;				// 参考线,用于设置到参考线的下拉列表中
	CString		m_ref_segment;				// 参考段,用于设置到参考段的下拉列表中
	CString		m_hook_name_list;			// 弯钩名称,用于显示到下拉列表中
	CString		m_setment_sketch;			// 参考段的示意图，字符串的典型格式为：THBW-PM-001-A01--D#15990

private:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* dx);
	virtual void OnOK();
	virtual void OnCancel();

private:
	void get_data_from_grid(OUT std::vector<frame_hook_setting>& setting_data);
	void set_data_to_grid(IN const std::vector<frame_hook_setting>& setting_data);
	

private:
	CTHExcel* m_hook_excel;
	CTHDataBase* m_hook_settings_db;

private:
	afx_msg void OnClose();
	afx_msg BOOL on_focus_changed( UINT nID, NMHDR * notify_struct, LRESULT * result );
};
