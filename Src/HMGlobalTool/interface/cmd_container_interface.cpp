#include "stdafx.h"

//#include "../macro.h"
#include "../tools/file_tools.h"
#include "../tools/tinyxml2.h"
#include "../tools/xml_tools.h"
#include "../tools/string_tools.h"

#include "window_interface.h"

#include "cmd_container_interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

//////////////////////////////////////////////////////////////////////////

std::map<cmd_container_interface*, bool> cmd_trigger_manager::m_command_objects;

//////////////////////////////////////////////////////////////////////////

cmd_container_interface::cmd_container_interface(bool auto_gc/* = false*/)
{
	cmd_trigger_manager::register_command_object(this, auto_gc);
}

cmd_container_interface::~cmd_container_interface()
{
	cmd_trigger_manager::unregister_command_object(this);
}

bool cmd_container_interface::fire(const CString& domain, const CString& cmd,
	const CString& param/* = _T("")*/, int drive_mode/* = 0*/)
{
	return false;
}

void cmd_container_interface::get_status(const CString& domain, const CString& cmd,
	bool& enable, bool& checked, bool& is_transparent)
{
	enable = false;
	checked = false;
	is_transparent = false;
}

bool cmd_container_interface::is_transparent_command(const CString& domain, const CString& command)
{
	bool enable = false, checked = false, is_transparent = false;
	get_status(domain, command, enable, checked, is_transparent);
	return is_transparent;
}

void cmd_container_interface::set_command_filter(std::vector<std::tuple<CString, CString, bool>>& cmds_can_do) const
{
	cmds_can_do.clear();
}

bool cmd_container_interface::is_command_supported(const CString& domain, const CString& cmd) const
{
	if (cmd.IsEmpty()) return false;

	vector<tuple<CString, CString, bool>> cmds_can_do;
	set_command_filter(cmds_can_do);
	if (cmds_can_do.empty())
	{
		// 只有域为空或者为app的命令才是常规命令，系统命令（域为sys）及命令对象自己的局部命令（如section）则不是常规命令
		bool is_normal_command = false;
		if (domain.IsEmpty() || domain.CompareNoCase(_T("app"))) is_normal_command = true;

		// 常规命令可以投递到没有设置过滤器的命令对象中进行处理
		return is_normal_command;
	}

	CString full_command_text = cmd_container_interface::merge_and_fill_domain(domain, cmd);

	auto& it = std::find_if(cmds_can_do.begin(), cmds_can_do.end(), [&](tuple<CString, CString, bool>& cmd)
	{
		if (std::get<1>(cmd).IsEmpty()) return false;

		CString cur_full_cmd_text = cmd_container_interface::merge_and_fill_domain(std::get<0>(cmd), std::get<1>(cmd));
		return (0 == cur_full_cmd_text.CompareNoCase(full_command_text));
	});

	return (it != cmds_can_do.end());
}

void cmd_container_interface::parse_command(const CString& cmd_full_text, CString& domain, CString& cmd)
{
	domain = _T("");
	cmd = _T("");
	bool is_transparent = false;
	parse_command(cmd_full_text, domain, cmd, is_transparent);
}

void cmd_container_interface::parse_command(const CString& cmd_full_text, CString& domain, CString& cmd, bool& is_transparent)
{
	domain = _T("");
	cmd = _T("");
	is_transparent = false;

	vector<CString> all_items;
	string_tools::split_string(all_items, cmd_full_text, _T("."));
	if (all_items.empty()) return;
	for (auto& x : all_items) x.Trim();

	// 只有一项则当作命令处理
	if (all_items.size() == 1)
	{
		domain = _T("");
		cmd = all_items.front();
		is_transparent = false;
	}
	else
	{
		all_items.resize(3, _T(""));
		domain = all_items[0];
		cmd = all_items[1];
		is_transparent = (_T("1") == all_items[2] ? true : false);
	}
}

void cmd_container_interface::parse_and_fill_command(const CString& cmd_full_text, CString& domain, CString& cmd)
{
	parse_command(cmd_full_text, domain, cmd);
	if (domain.IsEmpty()) domain = _T("app");
}

CString cmd_container_interface::parse_command_domain(const CString& cmd_full_text)
{
	CString domain;
	CString cmd;
	parse_command(cmd_full_text, domain, cmd);
	return domain;
}

CString cmd_container_interface::parse_and_fill_command_domain(const CString& cmd_full_text)
{
	CString result = parse_command_domain(cmd_full_text);
	if (result.IsEmpty()) result = _T("app");
	
	return result;
}

CString cmd_container_interface::parse_command_cmd(const CString& cmd_full_text)
{
	CString domain;
	CString cmd;
	parse_command(cmd_full_text, domain, cmd);
	return cmd;
}

CString cmd_container_interface::merge_domain_and_cmd(const CString& domain, const CString& cmd)
{
	if (domain.IsEmpty()) return cmd;
	return domain + _T(".") + cmd;
}

bool cmd_container_interface::parse_transparent_flag(const CString& cmd_full_text)
{
	CString domain = _T("");
	CString cmd = _T("");
	bool is_transparent = false;
	parse_command(cmd_full_text, domain, cmd, is_transparent);
	return is_transparent;
}

CString cmd_container_interface::merge_and_fill_domain(const CString& domain, const CString& cmd)
{
	if (domain.IsEmpty()) return _T("app.") + cmd;
	return domain + _T(".") + cmd;
}

CString cmd_container_interface::merge_full_command(const CString& domain, const CString& cmd, bool is_transparent)
{
	return domain + _T(".") + cmd + _T(".") + (is_transparent ? _T("1") : _T("0"));
}

CString& cmd_container_interface::fill_command_domain(CString& cmd)
{
	CString domain;
	CString cmd_text;
	parse_command(cmd, domain, cmd_text);

	domain.Trim();
	if (domain.IsEmpty()) cmd = _T("app.") + cmd_text;
	return cmd;
}

command_post_doing_function::command_post_doing_function()
{
}

command_post_doing_function::~command_post_doing_function()
{
}

void command_post_doing_function::call()
{
	cmd_trigger_manager::fire(m_domain, m_command, m_parameter);
}

bool cmd_trigger_manager::register_command_object(cmd_container_interface* cmd_obj, bool auto_gc/* = false*/)
{
	if (!cmd_obj) return false;

	auto it = m_command_objects.find(cmd_obj);
	if (it != m_command_objects.end())
	{
		it->second = auto_gc;
		return true; // 修改属性也算注册成功
	}

	m_command_objects.insert(make_pair(cmd_obj, auto_gc));
	return true;
}

bool cmd_trigger_manager::unregister_command_object(cmd_container_interface* cmd_obj)
{
	if (!cmd_obj) return false;

	bool auto_gc = false;
	cmd_container_interface* copy_obj = nullptr;

	auto& it = m_command_objects.find(cmd_obj);
	if (it != m_command_objects.end())
	{
		auto_gc = it->second;
		if (auto_gc)
		{
			// 千万不可直接使用迭代器进行delete，因为当外部调用对象的delete自动反注册时在析构函数中触发此
			// 处的delete，然后又触发析构函数，造成析构函数递归导致程序崩溃，因此要将对象先记下来，然后从
			// 容器中移除，再对副本进行delete，这时因为在内部注册集合中找不到对象而不再导致递归，但也会对
			// 同一对象调用两次delete，一次是外部，另一次是此处对副本调用（注意，外部有可能不是调用delete
			// 而是手工反注册，此时没问题，只要考虑两次delete的情况）
			// 由于存在上述情况，因此，头文件中有重要说明：
			// 注意：如果一个对象被注册为自动管理，则允许外部手工调用这个对象的反注册，但是[[[禁止]]]管理这个对象的
			//      内存，如delete等，否则反注册崩溃！！！
			// 此处作注释以再次强调，自动对象外部不得干涉其内存管理

			// delete(it->first); // 指针为key，不能修改为nullptr,稍后直接从容器中移除即可
			copy_obj = it->first;
		}

		m_command_objects.erase(it); // 不自动回收的移除即可
	}

	safe_delete(copy_obj);

	return auto_gc;
}

void cmd_trigger_manager::remove_all_command()
{
	// 由于对象析构时会自动反注册，因此全部移除时调用了delete过程中，然后反注册中再次delete必然导致崩溃，因
	// 此，要先将所有注册对象放到新容器中，然后不析构对象，直接clear()避开反注册，然后再对新容器中的对象进行
	// 反注册和析构，此时由于对象已经不在内容的保存注册对象的容器中，因此不会再次被delete，保证程序正常运行，
	// 而外部对象用户手工delete时会触发反注册，并不会造成这种现象
	vector<pair<cmd_container_interface *, bool>> all_object;
	for (auto& x : m_command_objects)
		all_object.push_back(x);

	// 清空，以屏蔽析构时自动反注册再次delete
	m_command_objects.clear();

	for (auto& x : all_object)
	{
		if (nullptr == x.first) continue;
		if (true == x.second) delete(x.first); // 指针为key，不能修改为nullptr,稍后直接从容器中移除即可
	}

	all_object.clear();
}

bool cmd_trigger_manager::fire(const CString& domain, const CString& command, const CString& param/* = _T("")*/)
{
	vector<cmd_container_interface*> cmd_objs = get_command_object(domain, command);
	if (cmd_objs.empty()) return false;

	auto single_container_fire = [&](cmd_container_interface* x)
	{
		bool break_command = false;

		if (!x) return break_command;
		if (!x->is_command_supported(domain, command)) return break_command;

		bool is_enable = false, is_check = false, is_transparent = false;
		x->get_status(domain, command, is_enable, is_check, is_transparent);
		if (is_enable) break_command = x->fire(domain, command, param);
		
		return break_command;
	};

	// 主界面最后处理，这样可以让其它的对象截断并改写主界面内置的命令，增强程序的适应性
	// 注意：一些命令会在主界面加载前触发，不能保证界面指针总是有效
	app_ui_interface* app = get_app_ui_interface();
	cmd_container_interface* app_cm = (app ? dynamic_cast<cmd_container_interface*>(app) : nullptr);
	bool app_ui_need_fire = false;

	// 优先触发当前工作区的窗口，以利于同一窗口因加载不同数据而存在多个实例时响应更合理，且用户能干预后续窗口是否继续响应
	window_interface* cur_space_wnd = (app ? app->get_current_working_space_windows() : nullptr);
	cmd_container_interface* cur_space_cmd = (cur_space_wnd ? dynamic_cast<cmd_container_interface*>(cur_space_wnd) : nullptr);
	if (cur_space_cmd)
	{
		if (single_container_fire(cur_space_cmd)) return true; // 返回true表示不再往下路由，哪怕主界面也响应了也禁止它执行
	}

	// 先处理其它对象，并绕过主界面，留待最后处理
	for (cmd_container_interface* x : cmd_objs)
	{
		if (x == cur_space_cmd) continue; // 当前工作区窗口已经处理，跳过
		if (x == app_cm)
		{
			app_ui_need_fire = true;
			continue;
		}

		if (single_container_fire(x)) return true; // 返回true表示不再往下路由，哪怕主界面也响应了也禁止它执行
	}

	// 主界面延迟到最后处理
	if (app_cm && app_ui_need_fire) return single_container_fire(app_cm);

	return false;
}

void cmd_trigger_manager::post_fire(const CString& domain, const CString& command, const CString& param /*= _T("")*/)
{
	app_ui_interface* app = get_app_ui_interface();
	if (!app) return;

	command_post_doing_function* post_doing_function = new command_post_doing_function();
	post_doing_function->m_command = command;
	post_doing_function->m_domain = domain;
	post_doing_function->m_parameter = param;
	app->do_function_by_post_message(post_doing_function);
}

std::vector<cmd_container_interface*> cmd_trigger_manager::get_command_object(
	const CString& domain, const CString& command, int transparent_type/* = 0*/)
{
	vector<cmd_container_interface*> cmd_objs;

	if (command.IsEmpty()) return cmd_objs;

	CString src_full_cmd = cmd_container_interface::merge_and_fill_domain(domain, command);
	for (const pair<cmd_container_interface*, bool>& x : m_command_objects)
	{
		if (!x.first) continue;
		if (1 == transparent_type && x.first->is_transparent_command(domain, command)) continue;
		if (2 == transparent_type && !x.first->is_transparent_command(domain, command)) continue;

		vector<tuple<CString, CString, bool>> cmds_can_do;
		x.first->set_command_filter(cmds_can_do);
		if (cmds_can_do.empty())
		{
			cmd_objs.push_back(x.first); // 为空表示可以响应全部命令
			continue;
		}

		for (tuple<CString, CString, bool>& cmd : cmds_can_do)
		{
			if (std::get<1>(cmd).IsEmpty()) continue;
			CString full_cmd = cmd_container_interface::merge_and_fill_domain(std::get<0>(cmd), std::get<1>(cmd));
			if (0 == src_full_cmd.CompareNoCase(full_cmd))
			{
				cmd_objs.push_back(x.first);
				break;
			}
		}
	}

	return cmd_objs;
}

bool cmd_trigger_manager::has_any_object_can_do(const CString& domain, const CString& command)
{
	vector<cmd_container_interface*> objs = get_command_object(domain, command);
	
	bool has_none_nullptr = false;
	for (auto& x : objs)
	{
		if (nullptr != x)
		{
			has_none_nullptr = true;
			break;
		}
	}

	return has_none_nullptr;
}

void cmd_trigger_manager::get_command_state(const CString& domain, const CString& command,
	bool& enable, bool& checked, bool& is_transparent)
{
	enable = false;
	checked = false;
	is_transparent = false;
	if (command.IsEmpty()) return;

	CString full_command_text = cmd_container_interface::merge_and_fill_domain(domain, command);

	// 查找所有响应中有过滤条件且能处理此命令的命令处理对象集合A，所有无过滤条件的命令处理对象集合B
	vector<cmd_container_interface*> cmds_can_do;
	vector<cmd_container_interface*> cmds_nothing;

	// 各命令处理对象的获取状态接口尽量不限定为const函数，因此这里不用引用遍历，加之复制的是指针和bool值，代价基本上可以忽略不计
	for (pair<cmd_container_interface*, bool> x : m_command_objects)
	{
		if (!x.first) continue;
		if (!x.first->is_command_supported(domain, command)) continue;

		vector<tuple<CString, CString, bool>> cmds_filter;
		x.first->set_command_filter(cmds_filter);
		if (cmds_filter.empty())
		{
			cmds_nothing.push_back(x.first);
			continue;
		}

		if (x.first->is_command_supported(domain, command)) cmds_can_do.push_back(x.first);
	}

	cmd_container_interface* cmd_filter_et_ct = nullptr; // 在集合A中查找enable和checked均为true的对象，如果有则返回其状态，否则进行后继操作
	cmd_container_interface* cmd_nothing_et_ct = nullptr; // 在集合B中查找enable和checked均为true的对象，如果有则返回其状态，否则进行后继操作
	cmd_container_interface* cmd_filter_et = nullptr; // 在集合A中查找enable为true的对象，如果有则返回其状态，否则进行后继操作
	cmd_container_interface* cmd_nothing_et = nullptr; // 在集合B中查找enable为true的对象，如果有则返回其状态，否则进行后继操作
	cmd_container_interface* cmd_filter_ct = nullptr; // 在集合A中查找checked为true的对象，如果有则返回其状态，否则进行后继操作
	cmd_container_interface* cmd_nothing_ct = nullptr; // 在集合B中查找checked为true的对象，如果有则返回其状态，否则进行后继操作

	auto fun_get_cmd_in_container = [&](vector<cmd_container_interface*>& cmds,
		cmd_container_interface*& cmd_enable_true_and_checked_true,
		cmd_container_interface*& cmd_enable_true,
		cmd_container_interface*& cmd_checked_true)
	{
		cmd_enable_true_and_checked_true = nullptr;
		cmd_enable_true = nullptr;
		cmd_checked_true = nullptr;

		if (cmds.empty()) return;

		for (cmd_container_interface* x : cmds)
		{
			if (!x) break;

			bool cur_enable = false, cur_checked = false, is_transparent = false;
			x->get_status(domain, command, cur_enable, cur_checked, is_transparent);

			if (cur_enable && cur_checked && !cmd_enable_true_and_checked_true) cmd_enable_true_and_checked_true = x;
			if (cur_enable && !cmd_enable_true) cmd_enable_true = x;
			if (cur_checked && !cmd_checked_true) cmd_checked_true = x;
		}
	};

	fun_get_cmd_in_container(cmds_can_do, cmd_filter_et_ct, cmd_filter_et, cmd_filter_ct);
	fun_get_cmd_in_container(cmds_nothing, cmd_nothing_et_ct, cmd_nothing_et, cmd_nothing_ct);

	if (cmd_filter_et_ct)
		cmd_filter_et_ct->get_status(domain, command, enable, checked, is_transparent);
	else if (cmd_nothing_et_ct)
		cmd_nothing_et_ct->get_status(domain, command, enable, checked, is_transparent);
	else if (cmd_filter_et)
		cmd_filter_et->get_status(domain, command, enable, checked, is_transparent);
	else if (cmd_nothing_et)
		cmd_nothing_et->get_status(domain, command, enable, checked, is_transparent);
	else if (cmd_filter_ct)
		cmd_filter_ct->get_status(domain, command, enable, checked, is_transparent);
	else if (cmd_nothing_ct)
		cmd_nothing_ct->get_status(domain, command, enable, checked, is_transparent);
	else
		; // nothing;
}

bool cmd_trigger_manager::is_command_enable(const CString& domain, const CString& command)
{
	bool enable = false, checked = false, is_transparent = false;
	get_command_state(domain, command, enable, checked, is_transparent);
	return enable;
}

bool cmd_trigger_manager::is_command_checked(const CString& domain, const CString& command)
{
	bool enable = false, checked = false, is_transparent = false;
	get_command_state(domain, command, enable, checked, is_transparent);
	return checked;
}


bool cmd_trigger_manager::is_transparent_command(const CString& domain, const CString& command)
{
	bool enable = false, checked = false, is_transparent = false;
	get_command_state(domain, command, enable, checked, is_transparent);
	return is_transparent;
}

//////////////////////////////////////////////////////////////////////////

cmd_container_interface* register_auto_gc_cmd_and_return_object(cmd_container_interface* obj)
{
	bool flag = cmd_trigger_manager::register_command_object(obj, true);
	if (!flag && obj) safe_delete(obj); // 注册失败时要析构自动对象

	return (flag ? obj : nullptr);
}
