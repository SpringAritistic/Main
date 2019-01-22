#pragma once

#include <map>
#include <vector>
#include "../interface/data_center_interface.h"


class x_mvc_editor_base;


class X_FRAMEWORK_API x_observer_base : public CObject
{
	DECLARE_SERIAL(x_observer_base)

public:
	x_observer_base(x_mvc_editor_base* wnd = nullptr, const CString& node_tag = _T(""));
	virtual ~x_observer_base();

public:
	virtual void project_data_notify(const CString& node_tag, notifer_action ac);
	virtual CString get_title();

public:
	const CString& get_data_node_tag() const;
	x_mvc_editor_base* get_owner_window();

	// 当表格的结构（如行列数）有变化时，是触发更新，否则界面会有绘制错误
	// 加载数据的时候不用处理，框架已经处理
	void update_observer_ui();
	void update_window_title();

private:
	// 数据和窗口只能由创建时被编辑器所指定，observer只有访问权
	CString m_data_node_tag;			// 数据中心数据tag
	x_mvc_editor_base* m_owner_window;

private:
	friend class x_mvc_editor_base;

private:
	// 界面相关对象不允许复制
	x_observer_base(x_observer_base&);
	x_observer_base(x_observer_base&&);
	void operator=(const x_observer_base&);
	void operator=(const x_observer_base&&);
};

