#pragma once
#ifndef __HMGLOBALTOOL_INTERFACE_POPUP_MENU_INTERFACE_H__
#define __HMGLOBALTOOL_INTERFACE_POPUP_MENU_INTERFACE_H__	

#include <functional>
#include <vector>
#include <map>


// 不得和系统的其它命令按钮、菜单、工具条按钮等会触发WM_COMMAND消息的元素的id有重叠
// 注：ribbon从20000-30100，加速键从10000-19999（参见主程序模块的stdafx.h）
#define APP_POPUP_MENU_ID_START  40000
#define APP_POPUP_MENU_ID_END    49999


class popup_menu_interface;

class X_FRAMEWORK_API popup_menu_item
{
public:
	// 构造一个菜单项，update_status函数对象参数见成员m_update_status
	popup_menu_item(const CString& text,
		std::function<void()> on_command = nullptr,
		std::function<void(bool&, bool&, bool&)> update_status = nullptr);

    ~popup_menu_item();

public:
	// 添加一个子菜单项，update_status函数对象参数见成员m_update_status
	popup_menu_item* add_sub_item(const CString& text,
		std::function<void()> on_command = nullptr,
		std::function<void(bool&, bool&, bool&)> update_status = nullptr);

	// sub_mu必须是堆上new出来的对象
	popup_menu_item* add_sub_item(popup_menu_item* sub_mu);

	void clear_items();

private:
	static unsigned make_menu_item_id();

private:
	void make_menu_implement(CMenu& parent_menu);

private:
    CString										m_text;						// 显示文字（为"-"表示分隔符）
    unsigned									m_id = -1;					// 菜单id
	std::function<void(bool&, bool&, bool&)>	m_update_status = nullptr;	// 设置菜单状态获取函数（is_enabled, is_hidden, is_checked）
	std::function<void()>						m_do_command = nullptr;		// 菜单响应函数（如果设置了响应函数但未设置状态获取函数，则默认自动将菜单的enable状态设置为true）
    std::vector<popup_menu_item*>				m_sub_menus;
	std::vector<CMenu*>							m_created_menus;

private:
	friend class popup_menu_interface;
};


class X_FRAMEWORK_API popup_menu_interface
{
public:
	popup_menu_interface();
	virtual ~popup_menu_interface();

public:
	// 弹出菜单，窗口创建完以后调用
	void popup_menu(const CString& tag);

	// 添加一个弹出菜单，并指定标识
	popup_menu_item* add_popup_menu(const CString& tag);

	// 在ON_RANGE_COMMAND(APP_POPUP_MENU_ID_START, APP_POPUP_MENU_ID_END)的响应函数中调用本函数以接管菜单响应
	void on_transform_popup_menu_command(unsigned id);
   
	// 清空菜单资源
	void clear_all_menu_items();

protected:
	// 功能是添加菜单项，请实现后在派生类中调用，建议放在构造函数或创建创建函数中
	virtual void initialize_menus() = 0;

private:
	bool on_transform_popup_menu_command_implement(popup_menu_item *item, unsigned id);

private:
	// 存储各子菜单的根菜单（弹出时不显示，仅显示子菜单项）
	// 派生为只能通过接口添加菜单项，不允许直接操作此数据
	std::map<CString, popup_menu_item*> m_all_menu_roots;
};



#endif //__HMGLOBALTOOL_INTERFACE_POPUP_MENU_INTERFACE_H__
