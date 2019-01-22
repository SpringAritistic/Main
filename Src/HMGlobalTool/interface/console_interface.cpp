#include "stdafx.h"

#include <memory>

#include "../tools/file_tools.h"
#include "../tools/string_tools.h"

#include "app_ui_interface.h"
#include "cmd_container_interface.h"

#include "console_interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;



console_session_interface::console_session_interface()
{
}

console_session_interface::~console_session_interface()
{
	end_console_session();
}

void console_session_interface::session_be_finished()
{
}

void console_session_interface::on_console_action(int action_type)
{
}

void console_session_interface::notify_console_char_input(UINT char_value, UINT repeat_count, UINT flags)
{
	console_interface* con = get_console_interface();
	if (!con) return;

	if (VK_ESCAPE == char_value)
	{
		con->notify_console_action(0);
		return;
	}
	else if (VK_RETURN == char_value || VK_SPACE == char_value)
	{
		// 处理命令执行过程中，直接转发，否则按新命令来触发
		if (is_doing_command())
		{
			con->notify_console_action(VK_RETURN == char_value ? 1 : 2);
		}
		else
		{
			CString input_text = con->get_current_input();
			if (!input_text.IsEmpty())
			{
				CString domain, cmd;
				cmd_container_interface::parse_and_fill_command(input_text, domain, cmd);
				if (cmd.IsEmpty()) return;
				cmd_trigger_manager::fire(domain, cmd);
			}
		}

		// 如果是空格则空格暂不作为输入，控制台通知触发后直接返回（即命令过程中输入不了空格）
		return;
	}
	else
	{
		; // nothing
	}

	if (con->m_current_session_obj == this)
		con->on_char_input(char_value, repeat_count, flags);
}

void console_session_interface::request_console_session()
{
	console_interface* con = get_console_interface();
	if (!con) return;
	
	if (con->m_current_session_obj == this) return;

	if (con->m_current_session_obj)
		con->m_current_session_obj->session_be_finished();

	con->m_current_session_obj = this;
}

void console_session_interface::end_console_session()
{
	console_interface* con = get_console_interface();
	if (!con) return;
	if (con->m_current_session_obj != this) return; // 没有会话关系时无权结束会话

	con->m_current_session_obj = nullptr;
}


//////////////////////////////////////////////////////////////////////////


console_interface::console_interface()
{
	m_total_error_count = 0;
	m_error_count = 0;
	m_warning_count = 0;
	m_serious_warning_count = 0;
	m_default_prompt = _T("命令：");
	m_current_session_obj = nullptr;

	reset_log_file_default_file_path_name();
}

console_interface::~console_interface()
{

}

CString console_interface::get_current_input() const
{
	return _T("");
}

void console_interface::clear_current_input()
{

}

void console_interface::add_prompt(const CString& prompt, bool new_line /*= true*/, bool end_line/* = false*/)
{

}

void console_interface::add_input(const CString& str)
{

}

void console_interface::clear_all_output()
{
	// m_total_error_count = 0;此值供外部有特殊用处，只能使用手动调用接口清空
	m_error_count = 0;
	m_warning_count = 0;
	m_serious_warning_count = 0;
}

void console_interface::notify_console_action(int action_type)
{
	bool will_do_command = false;
	if (nullptr == m_current_session_obj) will_do_command = true;
	if (m_current_session_obj && !m_current_session_obj->is_doing_command()) will_do_command = true;

	if (m_current_session_obj && (m_current_session_obj->is_doing_command() || 0 == action_type))
		m_current_session_obj->on_console_action(action_type);

	CString input_text = get_current_input();
	input_text.Trim();

	CString domain, command, param;

	// 界面输入的命令和参数之间用|隔开（命令行不允许输入空格，空格当作是动作处理）
	vector<CString> cmd_and_param = string_tools::split_string(input_text, _T("|"));
	cmd_and_param.resize(2);
	param = cmd_and_param.back();

	cmd_container_interface::parse_and_fill_command(cmd_and_param.front(), domain, command);
	if (command.IsEmpty()) return;

	// 按新起的命令处理，如果目前仍处理命令执行过程中，是不能再触发命令的（有特例，透明命令可以在命令执行过程中处理）
	if (will_do_command)
	{
		// 0表示按了ESC键
		if (input_text.IsEmpty() || 0 == action_type)
		{
			add_prompt(get_default_prompt(),false, true);
			return;
		}

		if (cmd_trigger_manager::has_any_object_can_do(domain, command))
			cmd_trigger_manager::fire(domain, command, param);
		else
			add_prompt(get_default_prompt() + _T("非法命令"), true, true);
	}
	else
	{
		// 检测是不是透明命令，是则直接触发，不是则忽略，因为常规命令是不可以嵌套运行的
		if (cmd_trigger_manager::is_transparent_command(domain, command))
			cmd_trigger_manager::fire(domain, command, param);
		else if (!m_current_session_obj->is_doing_command())
			add_prompt(_T("无效输入"), true, true); // 交互过程中不干涉前导符，则会话对象控制并处理
		else
			; // 此时不是新命令执行也不是透明命令，那只能当作是常规输入了，由命令处理对象自行判断并处理
	}
}

void console_interface::get_last_command(CString& domain, CString& command)
{
	domain = m_last_command.first;
	command = m_last_command.second;
}

void console_interface::clear_last_command()
{
	m_last_command.first = _T("");
	m_last_command.second = _T("");
}

void console_interface::set_default_prompt(const CString& prompt)
{
	m_default_prompt = prompt;
}

CString console_interface::get_default_prompt() const
{
	return m_default_prompt;
}

void console_interface::print_message_count_report()
{
	CString report_text;
	report_text.Format("总计错误数：%ld，严重警告数：%ld，警告数：%ld .",
		m_error_count, m_serious_warning_count, m_warning_count);

	print_info_implement(OO_UI_LOG, MT_INFO, report_text);
}

void console_interface::save_text_to_log_file(const CString& text)
{
	m_log_file.output_text(text);
}

void console_interface::reset_log_file_path_name(const CString& text)
{
	m_log_file.set_file_path(text);
}

void console_interface::reset_log_file_default_file_path_name()
{
	// 不能使用app的路径，要使用x_framework的路径，因为主程序有可能是单元测试或其它引导程序，使用app的路径会
	// 导致特殊情况下文件输出到引导程序的路径下，因此改为框架模块的路径更通用
	CString default_path_name = file_tools::get_x_framework_path() + _T("\\main_application_log.txt");
	m_log_file.set_file_path(default_path_name);
}

void console_interface::print_info(const TCHAR* format, ...)
{
	CString result;

	va_list argList;
	va_start(argList, format);
	result.FormatV(format, argList);
	va_end(argList);

	print_info_implement(OO_UI_LOG, MT_INFO, result);
}

void console_interface::print_info(message_type type, const TCHAR* format, ...)
{
	CString result;

	va_list argList;
	va_start(argList, format);
	result.FormatV(format, argList);
	va_end(argList);

	print_info_implement(OO_UI_LOG, type, result);
}

void console_interface::print_info(output_object out, message_type type, const TCHAR* format, ...)
{
	CString result;

	va_list argList;
	va_start(argList, format);
	result.FormatV(format, argList);
	va_end(argList);

	print_info_implement(out, type, result);
}

void console_interface::print_info_implement(output_object out, message_type type, const CString& text)
{
	// 不要直接++，防止多线程竞态
	if (MT_ERROR == type)
	{
		::InterlockedIncrement(&m_error_count);
		::InterlockedIncrement(&m_total_error_count);
	}
	else if (MT_WARNING == type)
	{
		::InterlockedIncrement(&m_warning_count);
	}
	else if (MT_SERIOUS_WARNING == type)
	{
		::InterlockedIncrement(&m_serious_warning_count);
	}
	else
	{
		// nothing;
	}

	// 如果没有任何一个派生的控制台对象注册到框架中，则直接输出到日志，注册过就按派生类的行为不要干涉输出
	if (nullptr == get_console_interface() && OO_UI_LOG == out || OO_LOG == out)
		save_text_to_log_file(text);
}

long console_interface::get_total_error_count() const
{
	return m_total_error_count;
}

void console_interface::reset_total_error_count()
{
	m_total_error_count = 0;
}

//////////////////////////////////////////////////////////////////////////

static console_interface* g_console_obj = nullptr;


class null_base_console : public console_interface
{
	virtual void on_char_input(UINT char_value, UINT repeat_count, UINT flags) {}
	virtual void print_info_implement(output_object out, message_type type, const CString& text) {}
};

console_interface* get_console_interface()
{
	static auto base_console = make_unique<null_base_console>();

	// 控制台很特殊，整个程序可能到处都在利用它进行信息输出（不一定有交互），因此为了确保业务代码不会因适配不同UI而
	// 导致取到空对象造成崩溃或到处充斥着判断代码，当外部并没有注册命令对象时自动生成基类的对象返回
	if (g_console_obj)
		return g_console_obj;
	else
		return base_console.get();
}

void set_console_interface(console_interface* console_obj)
{
	g_console_obj = console_obj;
}

//////////////////////////////////////////////////////////////////////////


void app_print_info(const TCHAR* format, ...)
{
	CString result;

	va_list argList;
	va_start(argList, format);
	result.FormatV(format, argList);
	va_end(argList);

	app_print_info(OO_UI_LOG, MT_INFO, result);
}

void app_print_info(message_type type, const TCHAR* format, ...)
{
	CString result;

	va_list argList;
	va_start(argList, format);
	result.FormatV(format, argList);
	va_end(argList);

	app_print_info(OO_UI_LOG, type, result);
}

void app_print_info(output_object out, message_type type, const TCHAR* format, ...)
{
	CString result;

	va_list argList;
	va_start(argList, format);
	result.FormatV(format, argList);
	va_end(argList);

	console_interface* cmd = get_console_interface();
	if (cmd)
	{
		cmd->print_info(out, type, result);
		return;
	}
}
