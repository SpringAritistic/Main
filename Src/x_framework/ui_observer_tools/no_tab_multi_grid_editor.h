#pragma once
#include "THGridControl/THGridControlMac.h"
#include "THGridControl/THExcel.h"

#include "no_tab_multi_grid_observer.h"
#include "x_mvc_editor_base.h"

class CTHDataBase;



//单表无标签的编辑器
class X_FRAMEWORK_API no_tab_multi_gird_editor : public x_mvc_editor_base
{
	DECLARE_SERIAL(no_tab_multi_gird_editor)
	DECLARE_MESSAGE_MAP()

public:
	no_tab_multi_gird_editor();
	virtual ~no_tab_multi_gird_editor();

public:
	virtual bool load_data_to_ui(const CString& data_tag);
	virtual bool save_data_from_ui();
	virtual void on_project_data_notify(const CString& node_tag, notifer_action ac);

private:
	multi_grid_view*					m_created_grid_view;
	std::map<CString, CTHDataBase*>		m_data_base;


protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg BOOL on_cell_value_changed(UINT id, NMHDR * notify_struct, LRESULT * result);
	afx_msg BOOL on_cell_button_clicked(UINT id, NMHDR * notify_struct, LRESULT * result);
	afx_msg BOOL on_cell_focus_changed(UINT id, NMHDR * notify_struct, LRESULT * result);
};

// 激活一个编辑器窗口并添加到程序工作区间（主要是简化手工处理的过程）
X_FRAMEWORK_API void active_no_tab_multi_gird_editor(const CString& data_node_tag, const CString& ui_observer_class_name);

