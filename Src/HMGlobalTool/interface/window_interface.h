#pragma once
#ifndef __HMGLOBALTOOL_INTERFACE_WINDOW_INTERFACE_H__
#define __HMGLOBALTOOL_INTERFACE_WINDOW_INTERFACE_H__	

#include <set>
#include "app_ui_interface.h"

class HM_GlobalTool_EXT window_interface
{
public:
	window_interface();
	virtual ~window_interface();

public:
	// 加载数据，传入数据节点的tag,如果没有传空字符串
	virtual bool load_data_to_ui(const CString& data_tag);

	// 保存数据，传入数据节点的tag与加载数据时传入的一样，也可以自己记录数据加载时的tag，只是为了派生类省事，才将tag再次传入
	virtual bool save_data_from_ui();
	
	// 返回窗口标题，引接口会在加载数据后调用，以便窗口能根据数据自由决定标题
	virtual CString get_title();
	
	//窗口关闭之前需要处理时 ，重载此函数
	virtual void pre_close(const CString& node_tag);

	//窗口关闭之前需要会调用此接口，如果返回false，则不会关闭并提示detail_info信息，此接口会在PreClose之前被调用
	virtual bool can_be_close(CString& detail_info);

	// 当前窗口是否有数据改动需要保存（基类实现默认返回false，表示不保存）
	virtual bool need_save_ui_data();

	// 当要求窗口强制post方式刷新重绘的消息时调用
	virtual void invalidate(bool will_erase_background = false);

	// 系统主题改变时调用，如果是窗口创建时系统不会调用此接口，要自己根据主程序接口获取当前显示的主题并手动调用此接口
	virtual void app_theme_changed(theme_style_settings& settings);
	
	// 激活自己的上下文标签（仅当ribbon.xml中配置为手工激活时会被调用，自动时由框架处理）
	virtual void active_context_ribbon_tab();
	
public:
	static std::set<window_interface*> get_all_window_instances();
};
#endif //__HMGLOBALTOOL_INTERFACE_WINDOW_INTERFACE_H__
