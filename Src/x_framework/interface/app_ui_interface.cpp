#include "stdafx.h"

#include "window_interface.h"
#include "console_interface.h"

#include "app_ui_interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


functor_by_message::functor_by_message()
{
}

functor_by_message::~functor_by_message()
{
}

app_ui_interface::app_ui_interface()
{
}

app_ui_interface::~app_ui_interface()
{
}

bool app_ui_interface::is_working_space_view_exist(const CString& class_name, const CString& tag)
{
	vector<pair<CString, CString>> wnd_info;
	get_all_working_space_windows_info(wnd_info);

	for (auto& x : wnd_info)
		if (x.first == class_name && tag == x.second) return true;

	return false;
}

void app_ui_interface::select_working_space_view(const CString& class_name, const CString& tag)
{
	if (!is_working_space_view_exist(class_name, tag)) return;
	
	// 添加工作区窗口的接口有个特性：如果已经存在，则不添加，而是直接切换到相应窗口，因此利用它来完成切换
	add_working_space_view(class_name, tag);
}

int app_ui_interface::get_working_space_view_index(const CString& class_name, const CString& tag)
{
	vector<pair<CString, CString>> wnd_info;
	get_all_working_space_windows_info(wnd_info);

	int wnd_index = -1;
	for (size_t i = 0; i < wnd_info.size(); ++i)
	{
		if (wnd_info[i].first == class_name && tag == wnd_info[i].second)
		{
			wnd_index = (int)i;
			break;
		}
	}

	return wnd_index;
}

int app_ui_interface::get_working_space_view_index(const window_interface* wnd)
{
	vector<window_interface*> all_wnds;
	get_all_working_space_windows(all_wnds);

	int wnd_index = -1;
	for (size_t i = 0; i < all_wnds.size(); ++i)
	{
		if (all_wnds[i] == wnd)
		{
			wnd_index = (int)i;
			break;
		}
	}

	return wnd_index;
}

int app_ui_interface::add_working_space_view(const CString window_class, const CString node_tag, int index, int image_index)
{
	// 添加工作区窗口可能会根据需要来判断是否要关掉一些工作区窗口（如超过了最大窗口数），此时关掉的窗口会调用相应的
	// 保存功能，由于保存时可能触发数据中心的数据响应并间接导致树控件的节点销毁及重加载，销毁时这前已经送过来的消息
	// 参数已经失效析构了，因此参数如果使用引用则对象也无效了，所以函数原型的参数类型不得使用引用

	return -1;
}

int app_ui_interface::add_working_space_view(window_interface* wnd, const CString node_tag, int index /*= -1*/, int image_index /*= -1*/)
{
	return -1;
}

void app_ui_interface::get_all_working_space_windows(std::vector<window_interface*>& wnds)
{
	wnds.clear();
}


void app_ui_interface::update_all_working_space_window_captions()
{

}

window_interface* app_ui_interface::get_current_working_space_windows()
{
	return nullptr;
}

void app_ui_interface::get_current_working_space_active_window(std::pair<CString, CString>& info)
{
	info = make_pair(_T(""), _T(""));
}

void app_ui_interface::get_all_working_space_windows_info(std::vector<std::pair<CString, CString>>& info)
{
	info.clear();
}

void app_ui_interface::get_all_docking_pane_window(std::vector<window_interface*>& wnds)
{
	wnds.clear();
}

void app_ui_interface::get_all_docking_pane_window_by_class(std::vector<window_interface*>& wnds, const CString& class_name)
{
	wnds.clear();
}

window_interface* app_ui_interface::get_docking_pane_window_by_identify(const CString& app_identify)
{
	return nullptr;
}

window_interface* app_ui_interface::get_docking_pane_window_by_id(int id)
{
	return nullptr;
}

void app_ui_interface::destroy_all_docking_panes()
{

}

void app_ui_interface::create_all_docking_panes()
{

}

int app_ui_interface::get_docking_pane_window_id(window_interface* wnd)
{
	return 0;
}

void app_ui_interface::hide_docking_pane_by_id(int id)
{

}

void app_ui_interface::show_docking_pane_by_id(int id)
{

}

void app_ui_interface::hide_docking_pane_by_class(const CString& class_name)
{

}

void app_ui_interface::show_docking_pane_by_class(const CString& class_name)
{

}

void app_ui_interface::hide_docking_pane_by_identify(const CString& identify)
{

}

void app_ui_interface::show_docking_pane_by_identify(const CString& identify)
{

}

void app_ui_interface::reload_docking_pane_by_id(int id, const CString& node_tag)
{

}

void app_ui_interface::reload_docking_pane_data_by_identify(const CString& identify, const CString& node_tag)
{

}

void app_ui_interface::reload_docking_pane_data_by_class(const CString& class_name, const CString& node_tag)
{

}

void app_ui_interface::save_all_workspace_window_ui_data()
{

}

void app_ui_interface::save_all_docking_pane_ui_data()
{

}

bool app_ui_interface::close_all_workspace_window(bool force_close/* = false*/)
{
	return true;
}

void app_ui_interface::set_title(const CString& title)
{

}

CString app_ui_interface::get_title() const
{
	return _T("");
}

CXTPControl* app_ui_interface::get_ribbon_control(const CString& domain, const CString& cmd, const CString& param /*= _T("")*/, bool compare_param /*= false*/)
{
	return nullptr;
}

void app_ui_interface::update_working_space_tab_caption(int index)
{

}

void app_ui_interface::select_ribbon_tab(const CString& tab_name)
{

}

void app_ui_interface::show_ribbon_context_tabs(const std::vector<CString>& tab_names)
{

}

void app_ui_interface::add_recent_file_list(const CString& file_path_name)
{

}

void app_ui_interface::get_recent_file_list(std::vector<CString>& file_list)
{
	file_list.clear();
}

bool app_ui_interface::docking_panes_be_created()
{
	return false;
}

bool app_ui_interface::do_function_by_post_message(functor_by_message* fun)
{
	return false;
}

bool app_ui_interface::do_function_by_send_message(functor_by_message* fun)
{
	return false;
}

bool app_ui_interface::close_working_space_view_by_class(const CString& class_name, bool force_close /*= false*/)
{
	return false;
}

bool app_ui_interface::close_working_space_view_by_tag(const CString& tag, bool force_close /*= false*/)
{
	return false;
}

bool app_ui_interface::close_working_space_view_by_index(int index, bool force_close /* = false*/)
{
	return false;
}

int app_ui_interface::get_working_space_view_count()
{
	return 0;
}

bool app_ui_interface::get_tab_window_and_info(std::tuple<CString, CString, window_interface*>& info, int index) const
{
	return false;
}


//////////////////////////////////////////////////////////////////////////

class working_space_option_by_message : public functor_by_message
{
public:
	working_space_option_by_message() {}
	~working_space_option_by_message() {}

public:
	virtual void call()
	{
		app_ui_interface* app = get_app_ui_interface();
		if (!app) return;

		if (0 == m_option_type)
		{
			if (0 == m_add_type)
				app->add_working_space_view(m_window_class, m_node_tag, m_tab_index, m_image_index);
			else if (1 == m_add_type)
				app->add_working_space_view(m_window_ix, m_node_tag, m_tab_index, m_image_index);
			else
				; // nothing;
		}
		else if (1 == m_option_type)
		{
			if (0 == m_close_type)
				app->close_all_workspace_window(m_force_close);
			else if (1 == m_close_type)
				app->close_working_space_view_by_class(m_window_class, m_force_close);
			else if (2 == m_close_type)
				app->close_working_space_view_by_index(m_tab_index, m_force_close);
			else if (3 == m_close_type)
				app->close_working_space_view_by_tag(m_node_tag, m_force_close);
			else
				; // nothing;
		}
	}

public:
	int					m_option_type = 0;				// 0表示添加工作区窗口，1表示关闭工作区窗口
	int					m_add_type = 0;					// 添加动作，0表示按窗口类名添加，1表示按窗口对象添加
	int					m_close_type = 0;				// 关闭类型，0表示全部关闭，1表示按窗口类名关闭，2表示按窗口索引下标关闭，3表示按关联的tag来关

	CString				m_window_class;					// 添加或关闭时的窗口类
	window_interface*	m_window_ix = nullptr;			// 添加时的窗口对象
	CString				m_node_tag;						// 添加时的数据节点tag
	int					m_tab_index = -1;				// 添加表示插入的位置，关闭表示要关的索引
	int					m_image_index = -1;				// 添加表示图标索引
	bool				m_force_close = false;			// 关闭时是否强制关闭
};


void app_ui_interface::post_add_working_space_view_by_class(
	const CString window_class, const CString node_tag, int index /*= -1*/, int image_index /*= -1*/)
{
	working_space_option_by_message* functor = new working_space_option_by_message();
	functor->m_option_type = 0;
	functor->m_add_type = 0;
	functor->m_window_class = window_class;
	functor->m_node_tag = node_tag;
	functor->m_tab_index = index;
	functor->m_image_index = image_index;
	do_function_by_post_message(functor);
}

void app_ui_interface::post_add_working_space_view_by_wnd(
	window_interface* wnd_ix, const CString node_tag, int index /*= -1*/, int image_index /*= -1*/)
{
	working_space_option_by_message* functor = new working_space_option_by_message();
	functor->m_option_type = 0;
	functor->m_add_type = 1;
	functor->m_window_ix = wnd_ix;
	functor->m_node_tag = node_tag;
	functor->m_tab_index = index;
	functor->m_image_index = image_index;
	do_function_by_post_message(functor);
}

void app_ui_interface::post_close_working_space_view_by_class(const CString& class_name, bool force_close /*= false*/)
{
	working_space_option_by_message* functor = new working_space_option_by_message();
	functor->m_option_type = 1;
	functor->m_close_type = 1;
	functor->m_window_class = class_name;
	functor->m_force_close = force_close;
	do_function_by_post_message(functor);
}

void app_ui_interface::post_close_working_space_view_by_tag(const CString& tag, bool force_close /*= false*/)
{
	working_space_option_by_message* functor = new working_space_option_by_message();
	functor->m_option_type = 1;
	functor->m_close_type = 3;
	functor->m_node_tag = tag;
	functor->m_force_close = force_close;
	do_function_by_post_message(functor);
}

void app_ui_interface::post_close_working_space_view_by_index(int index, bool force_close /*= false*/)
{
	working_space_option_by_message* functor = new working_space_option_by_message();
	functor->m_option_type = 1;
	functor->m_close_type = 2;
	functor->m_tab_index = index;
	functor->m_force_close = force_close;
	do_function_by_post_message(functor);
}

void app_ui_interface::post_close_all_workspace_window(bool force_close /*= false*/)
{
	working_space_option_by_message* functor = new working_space_option_by_message();
	functor->m_option_type = 1;
	functor->m_close_type = 0;
	functor->m_force_close = force_close;
	do_function_by_post_message(functor);
}


//////////////////////////////////////////////////////////////////////////


theme_style_settings app_ui_interface::get_style_and_settings() const
{
	// 无效的风格
	theme_style_settings style_unknown;
	style_unknown.m_style_id = -1;
	style_unknown.m_style_type = app_theme_type::theme_unknown;
	style_unknown.m_tooltip_style = xtpToolTipStandard;
	style_unknown.m_docking_pane_theme = xtpPaneThemeWinNative;
	style_unknown.m_paint_theme = xtpThemeNativeWinXP;
	style_unknown.m_ribbon_system_button_style = xtpButtonAutomatic;
	style_unknown.m_theme_dll_ini_config_file_name = _T("");
	style_unknown.m_enable_aero = false;
	style_unknown.m_skin_file_name = _T("");
	style_unknown.m_skin_file_ini_section_name = _T("");
	style_unknown.m_tab_color = xtpTabColorDefault; // xtpTabColorResource;
	style_unknown.m_window_back_color = RGB(255, 255, 255);

	return style_unknown;
}


bool app_ui_interface::is_silent_model() const
{
	return false;
}

bool app_ui_interface::is_style_changing() const
{
	return false;
}

void app_ui_interface::get_silent_model_config(std::map<CString, CString>& configs)
{
	configs.clear();
}

//////////////////////////////////////////////////////////////////////////

static app_ui_interface* global_application = nullptr;

app_ui_interface* get_app_ui_interface()
{
	static unique_ptr<app_ui_interface> tmp_app = make_unique<app_ui_interface>();
	
	if (!global_application)
		return tmp_app.get();
	else
		return global_application;
}

void set_app_ui_interface(app_ui_interface* app)
{
	global_application = app;
}

