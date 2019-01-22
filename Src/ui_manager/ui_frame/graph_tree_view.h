#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/window_interface.h"
#include "x_framework/interface/data_center_interface.h"
#include "x_framework/interface/popup_menu_interface.h"

#include "project_manager/macro.h"
#include "project_manager/data_center/project_data.h"



// 项目树节点类型
enum class project_tree_node_type { unknown, project, graph, folder };

// 项目树节点数据
class project_tree_node_data
{
public:
	project_tree_node_data();
	~project_tree_node_data();

public:
	CString					m_data_node_tag;			// 对应的数据中心数据节点的tag，无对应的则为空
	project_tree_node_type	m_node_type;				// 节点类型
	CString					m_graph_reg_type_name;		// 图纸注册类型,只有图纸节点此数据才有意义
};


class graph_tree_view : public CXTPTreeView, public window_interface,
	public data_node_notify_interface, public popup_menu_interface
{
	DECLARE_SERIAL(graph_tree_view)
	DECLARE_MESSAGE_MAP()

public:
	graph_tree_view();
	virtual ~graph_tree_view();

private:
	virtual bool load_data_to_ui(const CString& data_tag);
	virtual bool save_data_from_ui();
	virtual void on_project_data_notify(const CString& node_tag, notifer_action ac);
	virtual void initialize_menus();
	virtual void app_theme_changed(theme_style_settings& settings);

public:
	void initialize_tree_nodes();
	void clear_tree_nodes();
	HTREEITEM get_tree_node_under_cursor();
	data_node_base* get_data_node_by_tree_item(HTREEITEM item);
	void active_graph_editor(const CString& editor_title, const project_tree_node_data& tree_node_data);
	graph_data_base* create_graph_data_base(const CString& class_name);

private:
	CImageList m_normal_image_list;
	CBitmap m_bitmap;
	HTREEITEM m_last_option_node_handle; // 记住鼠标最后一次操作的树节点句柄

private:
	afx_msg BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void on_tree_node_expanded(NMHDR* nm, LRESULT* lparam);
	afx_msg void on_command_all_popup_menu(UINT id);

	afx_msg void on_menu_project_item_add_graph();
	afx_msg void on_menu_project_item_add_directory();
	afx_msg void on_menu_project_item_draw_all_graphs();
	afx_msg void on_menu_project_item_clear_all();
	afx_msg void on_menu_directory_item_add_graph();
	afx_msg void on_menu_directory_item_draw_all_graphs();
	afx_msg void on_menu_directory_item_clear_all();
	afx_msg void on_menu_directory_item_add_directory();
	afx_msg void on_menu_directory_item_delete();
	afx_msg void on_menu_directory_item_rename();
	afx_msg void on_menu_graph_item_draw_one_graph();
	afx_msg void on_menu_graph_item_delete_one_graph();
	afx_msg void on_menu_graph_item_rename();
	afx_msg void on_menu_graph_open_output_template();
	afx_msg void on_menu_graph_open_import_template();
	afx_msg void on_menu_graph_open_dxf_file();
	afx_msg void on_menu_graph_open_dwg_file();
	afx_msg void on_menu_graph_open_pdf_file();
};
