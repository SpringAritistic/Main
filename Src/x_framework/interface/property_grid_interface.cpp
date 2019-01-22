#include "stdafx.h"
#include <memory>

#include "property_grid_interface.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


property_notification_interface::property_notification_interface(bool enable_notification/* = false*/)
{
	if (enable_notification) enable_notification_to_me(true);
}

property_notification_interface::~property_notification_interface()
{
	enable_notification_to_me(false);
}

void property_notification_interface::on_property_items_changed(std::vector<CXTPPropertyGridItem*>& prop_items)
{
}

void property_notification_interface::set_property_items(CXTPPropertyGrid& prop_grid)
{
}

void property_notification_interface::show_property_window(bool set_focus /*= false*/)
{
	property_window_interface* wnd = get_property_window();
	if (wnd) wnd->show_window(this, set_focus);
}

void property_notification_interface::clear_property_window()
{
	property_window_interface* wnd = get_property_window();
	if (wnd) wnd->clear_content();
}

void property_notification_interface::enable_notification_to_me(bool enable)
{
	if (enable)
		property_window_interface::regist_reciver(this);
	else
		property_window_interface::unregist_reciver(this);
}


//////////////////////////////////////////////////////////////////////////

std::mutex property_window_interface::m_mutex_reciver_list;
std::set<property_notification_interface*> property_window_interface::m_registed_prop_reciver;
property_notification_interface* property_window_interface::m_last_obj = nullptr;


property_window_interface::property_window_interface()
{
	m_last_obj = nullptr;
}

property_window_interface::~property_window_interface()
{
	m_last_obj = nullptr;
}

void property_window_interface::show_window(property_notification_interface* obj, bool set_focus /*= false*/)
{
	m_last_obj = obj;
}

void property_window_interface::clear_content()
{
	m_last_obj = nullptr;
}

bool property_window_interface::is_content_empty()
{
	return true;
}

void property_window_interface::regist_reciver(property_notification_interface* obj)
{
	if (!obj) return;

	lock_guard<std::mutex> tmp_guard(m_mutex_reciver_list);
	m_registed_prop_reciver.insert(obj);
}

void property_window_interface::unregist_reciver(property_notification_interface* obj)
{
	if (!obj) return;

	lock_guard<std::mutex> tmp_guard(m_mutex_reciver_list);
	auto it = m_registed_prop_reciver.find(obj);
	if (m_registed_prop_reciver.end() == it) return;

	m_registed_prop_reciver.erase(it);
	if (m_last_obj == obj) m_last_obj = nullptr;
}

bool property_window_interface::is_registed(property_notification_interface* obj)
{
	if (!obj) return false;

	lock_guard<std::mutex> tmp_guard(m_mutex_reciver_list);
	return m_registed_prop_reciver.end() != m_registed_prop_reciver.find(obj);
}

void property_window_interface::apply_propery_items_to_registered_obj(std::vector<CXTPPropertyGridItem*>& prop_items)
{
	if (prop_items.empty() || !m_last_obj) return;

	lock_guard<std::mutex> tmp_guard(m_mutex_reciver_list);
	auto it = m_registed_prop_reciver.find(m_last_obj);
	if (it != m_registed_prop_reciver.end())
		m_last_obj->on_property_items_changed(prop_items);
}

//////////////////////////////////////////////////////////////////////////

static property_window_interface* g_property_window_obj = nullptr;

property_window_interface* get_property_window()
{
	static auto base_property = make_unique<property_window_interface>();

	// 属性窗口很特殊，因此为了确保业务代码不会因适配不同UI而导致取到空对象造成崩溃或到处充斥着判断代码，当外
	// 部并没有注册命令对象时自动生成基类的对象返回
	if (g_property_window_obj)
		return g_property_window_obj;
	else
		return base_property.get();

	return g_property_window_obj;
}

void set_property_window(property_window_interface* obj)
{
	// 程序一般不会频繁的创建、销毁或切换属性窗口，一般是程序启动时指定，关闭时销毁，因此暂不考虑多线程并发的问题，一切从简
	g_property_window_obj = obj;
}
