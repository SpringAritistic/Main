#pragma once
#include "THGridControl/THGridControlMac.h"
#include "THGridControl/THExcel.h"

#include "tab_single_grid_observer.h"
#include "x_mvc_editor_base.h"

class CTHDataBase;
class tab_single_gird_editor;


class X_FRAMEWORK_API tabed_single_grid_control :public CWnd
{
	DECLARE_SERIAL(tabed_single_grid_control)
	DECLARE_MESSAGE_MAP()

public:
	tabed_single_grid_control(tab_single_gird_editor* editor = nullptr);
	virtual ~tabed_single_grid_control();

public:
	// 数据对象内存由外部编辑器统一管理，内部仅暂存指针
	bool load_data_to_ui(tab_single_grid_observer* data);
	bool save_data_from_ui();
	void set_window_backcolor(COLORREF bk_color);
	void add_mode_navigate_button(CXTPTabManagerNavigateButton* button);
	multi_grid_tab_control* get_grid_tab_control();
	bool all_databases_be_modified() const;

private:
	void clear_ui_data();
	void make_all_children_window_back_color_same_as_this();

private:
	multi_grid_tab_control				m_grid_tabs;
	tab_single_grid_observer*			m_grid_ui_observer;
	COLORREF							m_bk_color;

	// 缓存创建的对象便于索引
	std::vector<CTHExcel*>				m_created_multi_grid;
	std::map<CString, CTHDataBase*>		m_all_db_grids;


	tab_single_gird_editor*			m_editor; // 暂存对象，不管理内存

private:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg BOOL on_cell_value_changed(UINT id, NMHDR * notify_struct, LRESULT * result);
	afx_msg BOOL on_cell_button_clicked(UINT id, NMHDR * notify_struct, LRESULT * result);
	afx_msg BOOL on_cell_focus_changed(UINT id, NMHDR * notify_struct, LRESULT * result);
};


//多标签单表的编辑器
class X_FRAMEWORK_API tab_single_gird_editor : public x_mvc_editor_base
{
	DECLARE_SERIAL(tab_single_gird_editor)
	DECLARE_MESSAGE_MAP()

public:
	tab_single_gird_editor();
	virtual ~tab_single_gird_editor();

public:
	virtual bool load_data_to_ui(const CString& data_tag);
	virtual bool save_data_from_ui();
	virtual void on_project_data_notify(const CString& node_tag, notifer_action ac);
	virtual void app_theme_changed(theme_style_settings& settings);

private:
	tabed_single_grid_control*  m_tab_single_grid_control;
	
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);
};

// 激活一个编辑器窗口并添加到程序工作区间（主要是简化手工处理的过程）
X_FRAMEWORK_API void active_tab_single_gird_editor(const CString& data_node_tag, const CString& ui_observer_class_name);

