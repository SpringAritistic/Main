#include "stdafx.h"
#include "window_interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

//////////////////////////////////////////////////////////////////////////

static std::set<window_interface*> all_window_instances; // 静态变量限定作用域为本文件

//////////////////////////////////////////////////////////////////////////

bool window_interface::save_data_from_ui()
{
	return false;
}

window_interface::window_interface()
{
	all_window_instances.insert(this);
}

window_interface::~window_interface()
{
	auto& it = all_window_instances.find(this);
	if (it != all_window_instances.end()) all_window_instances.erase(it);
}

bool window_interface::load_data_to_ui(const CString& data_tag)
{
	return false;
}

CString window_interface::get_title()
{
	return _T("");
}

void window_interface::pre_close(const CString& data_tag)
{

}

bool window_interface::can_be_close(CString& detail_info)
{
	detail_info = _T("");
	return true;
}

bool window_interface::need_save_ui_data()
{
	return false;
}

void window_interface::invalidate(bool will_erase_background /*= false*/)
{
	CWnd* wnd = dynamic_cast<CWnd*>(this);
	if (wnd && wnd->GetSafeHwnd()) wnd->Invalidate(will_erase_background ? TRUE : FALSE);
}

void window_interface::app_theme_changed(theme_style_settings& settings)
{

}

std::set<window_interface*> window_interface::get_all_window_instances()
{
	return all_window_instances; // 返回的是set容器的副本，否则使用的人可能会因为使用过程中程序创建了新窗口而导致容器失效
}

void window_interface::active_context_ribbon_tab()
{
}

