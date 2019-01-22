#pragma once
#include <vector>
#include <map>

#include "tab_multi_grid_observer.h"
#include "../windows_helper/multi_grid_view.h"
#include "x_mvc_editor_base.h"

class tabed_multi_grid_editor;

class X_FRAMEWORK_API tabed_multi_grid_control :public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	tabed_multi_grid_control(tabed_multi_grid_editor* editor);
	virtual ~tabed_multi_grid_control();

public:
	// 数据对象内存由外部编辑器统一管理，内部仅暂存指针
	bool load_data_to_ui(tab_multi_grid_observer* observer);
	bool save_data_from_ui(); // 有数据保存过返回true，没有任何数据需要保存返回false
	void set_window_backcolor(COLORREF bk_color);
	void add_mode_navigate_button(CXTPTabManagerNavigateButton* button);
	multi_grid_tab_control* get_grid_tab_control();
	bool any_databases_be_modified() const;

private:
	void clear_ui_data();
	void make_all_children_window_back_color_same_as_this();

private:
	multi_grid_tab_control				m_grid_tabs;
	tab_multi_grid_observer*			m_grid_ui_observer;
	COLORREF							m_bk_color;

	// 缓存创建的对象便于索引
	std::vector<CWnd*>					m_all_tab_page;
	std::map<CString, CTHDataBase*>		m_all_db_grids;

	// 记录创建过的多表窗口，实际上是可以不用记的，用以下代码可以取到tab中对应的多表窗口
	// CXTPTabManagerItem* cur_tab_item = m_grid_tabs.GetSelectedItem();
	// CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	// multi_grid_view* cur_view = (cur_wnd ? dynamic_cast<multi_grid_view*>(cur_wnd) : nullptr);
	// 但为了效率还是要缓存一下比较好
	std::vector<multi_grid_view*>		m_created_multi_grid;

	tabed_multi_grid_editor*			m_editor; // 暂存对象，不管理内存

private:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg BOOL on_cell_value_changed(UINT id, NMHDR * notify_struct, LRESULT * result);
	afx_msg BOOL on_cell_button_clicked(UINT id, NMHDR * notify_struct, LRESULT * result);
	afx_msg BOOL on_cell_focus_changed(UINT id, NMHDR * notify_struct, LRESULT * result);
};


class X_FRAMEWORK_API tabed_multi_grid_editor : public x_mvc_editor_base
{
	DECLARE_SERIAL(tabed_multi_grid_editor)
	DECLARE_MESSAGE_MAP()

public:
	tabed_multi_grid_editor();
	virtual ~tabed_multi_grid_editor();

public:
	virtual bool load_data_to_ui(const CString& data_tag);
	virtual bool save_data_from_ui();
	virtual CString get_title();
	virtual void app_theme_changed(theme_style_settings& settings);
	virtual void on_project_data_notify(const CString& node_tag, notifer_action ac);
	virtual bool need_save_ui_data();

private:
	tabed_multi_grid_control*			m_tabbed_grid_wnd;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);
};

