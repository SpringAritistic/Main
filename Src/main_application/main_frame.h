#pragma once
#include <tuple>
#include <atomic>

#include "x_framework/macro.h"
#include "x_framework/interface/console_interface.h"
#include "x_framework/interface/app_ui_interface.h"
#include "x_framework/interface/cmd_container_interface.h"
#include "x_framework/interface/data_center_interface.h"

#include "app_ribbon_bar.h"
#include "docking_pane.h"
#include "client_tab_view.h"
#include "workspace_view.h"
#include "accelerator_map.h"


class window_interface;
class CXTPDockingPaneLayout;


// main_frame 窗口
class main_frame : public CXTPFrameWnd, public app_ui_interface,
	public CXTPCommandBarsFrameHook, public data_node_notify_interface
{
	DECLARE_MESSAGE_MAP()

public:
	main_frame();
	virtual ~main_frame();

public:
	app_ribbon_bar* get_db_ribbon_bar();
	void redraw_frame();

protected:
	virtual int add_working_space_view(const CString window_class, const CString node_tag, int index = -1, int image_index = -1);
	virtual int add_working_space_view(window_interface* wnd, const CString node_tag, int index = -1, int image_index = -1);
	virtual window_interface* get_docking_pane_window_by_id(int id);
	virtual int get_docking_pane_window_id(window_interface* wnd);
	virtual void hide_docking_pane_by_id(int id);
	virtual void hide_docking_pane_by_class(const CString& class_name);
	virtual void hide_docking_pane_by_identify(const CString& identify);
	virtual void show_docking_pane_by_id(int id);
	virtual void show_docking_pane_by_class(const CString& class_name);
	virtual void show_docking_pane_by_identify(const CString& identify);
	virtual void reload_docking_pane_by_id(int id, const CString& node_tag);
	virtual void reload_docking_pane_data_by_identify(const CString& identify, const CString& node_tag);
	virtual void reload_docking_pane_data_by_class(const CString& class_name, const CString& node_tag);
	virtual void destroy_all_docking_panes();
	virtual void create_all_docking_panes();
	virtual void get_all_working_space_windows(std::vector<window_interface*>& wnds);
	virtual void update_all_working_space_window_captions();
	virtual void get_current_working_space_active_window(std::pair<CString, CString>& info);
	virtual window_interface* get_current_working_space_windows();
	virtual void get_all_working_space_windows_info(std::vector<std::pair<CString, CString>>& info);
	virtual void get_all_docking_pane_window(std::vector<window_interface*>& wnds);
	virtual void get_all_docking_pane_window_by_class(std::vector<window_interface*>& wnds, const CString& class_name);
	virtual window_interface* get_docking_pane_window_by_identify(const CString& app_identify);
	virtual void save_all_workspace_window_ui_data();
	virtual void save_all_docking_pane_ui_data();
	virtual bool close_all_workspace_window(bool force_close = false);
	virtual void set_title(const CString& title);
	virtual CString get_title() const;
	virtual void update_working_space_tab_caption(int index);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CXTPControl* get_ribbon_control(const CString& domain, const CString& cmd, const CString& param = _T(""), bool compare_param = false);
	virtual void select_ribbon_tab(const CString& tab_name);
	virtual void show_ribbon_context_tabs(const std::vector<CString>& tab_names);
	virtual void add_recent_file_list(const CString& file_path_name);
	virtual void get_recent_file_list(std::vector<CString>& file_list);
	virtual bool docking_panes_be_created();
	virtual bool do_function_by_post_message(functor_by_message* fun);
	virtual bool do_function_by_send_message(functor_by_message* fun);
	virtual CDocument* GetActiveDocument();
	virtual bool close_working_space_view_by_class(const CString& class_name, bool force_close = false);
	virtual bool close_working_space_view_by_tag(const CString& tag, bool force_close = false);
	virtual bool close_working_space_view_by_index(int index, bool force_close = false);
	virtual int get_working_space_view_count();
	virtual bool get_tab_window_and_info(std::tuple<CString, CString, window_interface*>& info, int index) const;
	virtual theme_style_settings get_style_and_settings() const;
	virtual void on_project_data_notify(const CString& node_tag, notifer_action ac);
	virtual bool is_style_changing() const;

protected:
	void create_docking_pane_manager();
	void initialize_docking_panes_config();
	void create_docking_panes();
	bool create_single_pane_child_window(single_pane_data& pane_data, int pane_wnd_id, CXTPDockingPane* pane_cur);
	void reload_all_docking_panes_data();
	void create_working_space();
	void initialize_theme_style_settings();
	XTPToolTipStyle get_tooltip_style() const;
	std::pair<XTPPaintTheme, CString> get_paint_style() const;
	XTPDockingPanePaintTheme get_docking_panes_style() const;
	void set_frame_theme();
	bool create_ribbon_bar(CXTPCommandBars* command_bars);
	void set_skin_style();
	void set_grid_color();
	bool create_status_bar();
	void add_status_progress();

protected:
	CXTPDockingPaneManager m_pane_manager;
	docking_pane_data m_docking_pane_config;
	CXTPDockingPaneLayout* m_docking_pane_layout;
	bool m_docking_panes_be_created;

	app_ribbon_bar* m_ribbon_bar; // 仅记录RibbonBar的指针，内存由XTP管理
	ribbon_bar_config_data m_ribbon_config;

	workspace_view m_workspace_view;
	//CXTPStatusBar m_status_bar;
	//CXTPProgressCtrl m_progress_ctrl;
	accelerator_map m_accelerator_map;

protected:
	// 以下变量用于实现界面主题功能
	int m_current_theme; // 当前主题模式
	bool m_is_ribbon_mini_size; // 功能区是否最小化
	std::map<int, theme_style_settings> m_all_style_settings;
	std::atomic<bool> m_is_style_changing;
	std::vector<CWnd*> m_all_created_panes_child_wnd;

protected:
	friend class app_size_config_helper;
	friend class main_application;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT flag, int cx, int cy);
	afx_msg void OnClose();
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDropFiles(HDROP drop_info);
	afx_msg BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	afx_msg void on_update_ribbon_exit(CCmdUI *pCmdUI);
	afx_msg void on_update_ribbon_recentdocs(CCmdUI *pCmdUI);

	afx_msg void on_command_skin_and_theme_menu_item(UINT id);
	afx_msg void on_update_skin_and_theme_menu_item(CCmdUI *pCmdUI);

	afx_msg LRESULT on_docking_pane_notify(WPARAM wParam, LPARAM lParam);
	
	afx_msg void on_recent_file_list(UINT id);

	afx_msg void on_all_ribbon_tab_command(UINT id);
	afx_msg void on_update_all_ribbon_tab_command(CCmdUI *pCmdUI);

	afx_msg void on_all_accelerator_command(UINT id);
	afx_msg void on_update_all_accelerator_command(CCmdUI *pCmdUI);

	// 通过消息执行指定调用
	afx_msg LRESULT on_msg_do_functor(WPARAM wparam, LPARAM lparam);

	//快速访问栏的消息响应函数
	afx_msg void on_all_ribbon_quick_access_command(UINT id);
	afx_msg void on_update_all_ribbon_quick_access_command(CCmdUI *pCmdUI);

	//文件菜单的消息响应函数
	afx_msg void on_all_ribbon_file_menu_command(UINT id);
	afx_msg void on_update_all_ribbon_file_menu_command(CCmdUI *pCmdUI);

	//options按钮消息响应函数
	afx_msg void on_all_ribbon_options_command(UINT id);
	afx_msg void on_update_all_ribbon_options_command(CCmdUI *pCmdUI);
};

