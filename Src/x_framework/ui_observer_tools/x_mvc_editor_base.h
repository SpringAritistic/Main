#pragma once
#include <vector>
#include <map>
#include <memory>

#include "../windows_helper/multi_grid_view.h"
#include "../interface/data_center_interface.h"
#include "../interface/window_interface.h"
#include "x_observer_base.h"



// 本工具只能用于工作区编辑器窗口的辅助生成，其它位置的窗口不支持
class X_FRAMEWORK_API x_mvc_editor_base : public CWnd,
	public window_interface, public data_node_notify_interface
{
	DECLARE_DYNCREATE(x_mvc_editor_base)
	DECLARE_MESSAGE_MAP()

public:
	x_mvc_editor_base();
	virtual ~x_mvc_editor_base();

public:
	virtual bool load_data_to_ui(const CString& data_tag);
	virtual bool save_data_from_ui();
	virtual CString get_title();
	virtual void app_theme_changed(theme_style_settings& settings);
	virtual void on_project_data_notify(const CString& node_tag, notifer_action ac);
	virtual bool need_save_ui_data();

public:
	int get_self_index_in_working_space() const; // 得不到返回-1
	void set_data_node_tag(const CString& tag); // 关联编辑器和内部observer的数据
	void set_ui_observer(x_observer_base* observer); // 关联编辑器内部的observer并正确设置各种相互依赖关系
	CString get_data_node_tag() const;
	x_observer_base* get_ui_observer() const;

private:
	CString						m_data_tag;
	x_observer_base*			m_ui_observer;
	CString						m_custom_title;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);
};

// 激活一个编辑器窗口并添加到程序工作区间（主要是简化手工处理的过程）
// 数据和窗口观察类可以为空，表示此窗口不需要外部数据和流程交互，如起始页
// 注意：使用了x_mvc_editor_base机制的编辑器窗口请使用此接口往框架上激活
X_FRAMEWORK_API void active_workspace_mvc_editor(const CString& data_node_tag,
	const CString& ui_observer_class_name, const CString& mvc_editor_class_name);
