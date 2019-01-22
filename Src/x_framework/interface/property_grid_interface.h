#pragma once
#ifndef __HMGLOBALTOOL_INTERFACE_PROPERTY_GRID_INTERFACE_H__
#define __HMGLOBALTOOL_INTERFACE_PROPERTY_GRID_INTERFACE_H__

#include <set>
#include <mutex>
#include <vector>
#include <utility>
#include "data_center_interface.h"



// 用于接收属性表界面数据变动时触发的通知的对象接口，所有需要能响应属性窗口中数据变动的对象都必须从此接口派生
class X_FRAMEWORK_API property_notification_interface
{
public:
	property_notification_interface(bool enable_notification = true); // 对象创建时默认会收到属性表的数据变动通知
	virtual ~property_notification_interface();

public:
	// 属性表中有内容被修改要求响应对象应用修改时调用，传入的对象不得析构，统一由框架管理
	virtual void on_property_items_changed(std::vector<CXTPPropertyGridItem*>& prop_items);
	
	// 填充适当的属性条目，以便显示到属性表中
	// 所有添加到属性表中的属性条目必须是new出来的，且内存交由框架管理
	virtual void set_property_items(CXTPPropertyGrid& prop_grid);

public:
	// 显示本接口实例化对象的属性窗口，此处仅为便利性而包装接口，可以直接使用属性表接口的相关操作手工完成
	void show_property_window(bool set_focus = false);

	// 清空属性窗口内容，此处仅为便利性而包装接口，可以直接使用属性表接口的相关操作手工完成
	void clear_property_window();

	// 要求属性表将数值变动的消息发到此对象或要求属性表窗口不再将数值变动的动作发到此对象
	void enable_notification_to_me(bool enable);
};


//////////////////////////////////////////////////////////////////////////


// 属性窗口接口，具体的属性窗口必须从此接口派生才能被框架所接受并实现交互，一个程序只支持一个属性表窗口派生类
class X_FRAMEWORK_API property_window_interface
{
public:
	property_window_interface();
	virtual ~property_window_interface();

public:
	// 显示属性窗口，如果属性窗口已经显示，则仅更新内容
	virtual void show_window(property_notification_interface* obj, bool set_focus = false);

	// 清空窗口内容
	virtual void clear_content();

	// 当前窗口显示的是否为空内容
	virtual bool is_content_empty();

public:
	// 管理已注册的数据变动通知对象，属性窗口不负责已注册对象的内存管理
	static void regist_reciver(property_notification_interface* obj);
	static void unregist_reciver(property_notification_interface* obj);
	static bool is_registed(property_notification_interface* obj);
	static void apply_propery_items_to_registered_obj(std::vector<CXTPPropertyGridItem*>& prop_items);
	
private:
	// 由于很多对象是在其它线程（如osg的渲染线程）中创建并注册的，因此要多线程保护，以防与界面线程造成竞态
	static std::mutex m_mutex_reciver_list;
	static std::set<property_notification_interface*> m_registed_prop_reciver;
	static property_notification_interface* m_last_obj;
};


//////////////////////////////////////////////////////////////////////////


// 获取或设置程序所实例化的属性表窗口接口指针
X_FRAMEWORK_API property_window_interface* get_property_window();
X_FRAMEWORK_API void set_property_window(property_window_interface* obj);

#endif //__HMGLOBALTOOL_INTERFACE_PROPERTY_GRID_INTERFACE_H__
