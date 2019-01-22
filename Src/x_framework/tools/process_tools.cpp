#include "stdafx.h"

#include <utility>
#include "file_tools.h"
#include "string_tools.h"
#include "common_tools.h"
#include "../interface/console_interface.h"

#include "process_tools.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


// 内部类不导出
class x_process_runner
{
public:
	x_process_runner();
	~x_process_runner(); // 析构时会清理掉正在执行的关联进程

public:
	// 初始化内部状态，also_clear_call_back为true表示保留回调函数及相关参数
	void initialize(bool also_clear_call_back = true);

	// 创建进程前指定进程信息及回调函数中传回的标识参数
	// 如果要在进程结束时进行相应的操作，目前有两种方法：
	// 第一个参数为任务名称，第二个参数为退出码
	// 1.在起进程时指定wait_for_termination为true
	// 2.在起进程时指定wait_for_termination为false并使用set_call_back_function设置回调函数
	// 强烈推荐第二种方法，并配合类的成员函数或全局函数来实现
	void set_call_back_function(const CString& task_name,
		std::function<void(const CString&, DWORD&)> end_call_back, CString* param_1, DWORD* param_2,
		std::function<bool()> will_be_stop);

	CString get_task_name() const;

	// 在运行过程中检测是否要停止的标志
	void set_need_stop_flag_funtion(std::function<bool()> need_stop);

	// 进程是否结束
	bool is_process_finished() const;

	// 启动一个进程并交由当前类的实例进行关联管理,如果已经有关联的进程，则先结束原进程再创建新进程
	// 成功执行所有操作返回true，否则返回false
	// time_out仅当wait_for_termination为true时有效，为0表示不等待，为INFINITE表示一直等待直到进程结束
	// exit_code为进程结束时的返回码，仅当wait_for_termination为true时有效
	bool create_process(DWORD& exit_code, const CString& program_path_name, const CString& parameter = _T(""),
		const CString& default_dir = _T(""), bool wait_for_termination = true, size_t time_out = INFINITE);

	// 强行停止任务并结束进程
	void force_stop();

private:
	// 检查当前进程状态
	// is_finished		进程是否执行结束
	// exit_code		进程调用结束则返回结束代码，无法检测或未结束则值赋为0
	// check_time_out	检测进程状态的超时时间，如果超时，则is_finished返回false
	void check_process_state(bool& is_finished, DWORD& exit_code, size_t check_time_out = 50);

	// 等待进程结束，内部处理了一些细节，不会阻塞GUI进程
	// 如果是正常结束返回0，如果是检测到外部停止标志或其它原因导致停止则返回非0值
	// 内部标志要求强制停止返回1
	// 外部回调要求强制停止返回2
	void wait_for_finish(DWORD& exit_code);

private:
	STARTUPINFO				m_start_info;
	HANDLE					m_process_handle;

	std::function<void(const CString&, DWORD&)> m_end_call_back;
	CString*				m_param_1;
	DWORD*					m_param_2;

	CString					m_task_name;
	bool					m_is_finished;
	HANDLE					m_pipe_std_output_read;
	HANDLE					m_pipe_std_output_write;
	bool					m_will_stop_by_inner_flag;
	std::function<bool()>	m_will_stop_by_call_back;	// 执行过程中调用此函数检测是否外部要求此任务结束，默认为空对象
};


// 内部类不导出
class x_task_manager
{
private:
	x_task_manager();
	~x_task_manager();

public:
	static void remove_finished_tasks();
	static bool has_task(const CString& task_name);
	static void force_stop_all_tasks();
	static x_process_runner* get_process_task(const CString& name);

	// 如果任务是要求等待完成才返回，则正常结束返回0，否则返回相关的非0的退出码
	// 如果任务是不要求等待，异步完成的，则会在完成时调用相应的回调函数，此接口直接返回0
	static DWORD do_process_task(const CString& name, const x_process_object& obj);

private:
	static std::vector<x_process_runner> m_process_runners;
};


//////////////////////////////////////////////////////////////////////////

std::vector<x_process_runner> x_task_manager::m_process_runners;

//////////////////////////////////////////////////////////////////////////


x_process_runner::x_process_runner()
{
	initialize();
}

x_process_runner::~x_process_runner()
{
	m_process_handle = nullptr;
}

void x_process_runner::set_call_back_function(const CString& task_name,
	std::function<void(const CString&, DWORD&)> end_call_back, CString* param_1, DWORD* param_2,
	std::function<bool()> will_be_stop)
{
	m_task_name = task_name;
	m_end_call_back = end_call_back;
	m_param_1 = param_1;
	m_param_2 = param_2;
	m_will_stop_by_call_back = will_be_stop;
}

CString x_process_runner::get_task_name() const
{
	return m_task_name;
}

void x_process_runner::set_need_stop_flag_funtion(std::function<bool()> need_stop)
{
	m_will_stop_by_call_back = need_stop;
}

bool x_process_runner::is_process_finished() const
{
	return m_is_finished;
}

void x_process_runner::initialize(bool also_clear_call_back /* = true */)
{
	m_is_finished = true;
	m_will_stop_by_inner_flag = false;

	::ZeroMemory(&m_start_info, sizeof(STARTUPINFO));

	m_start_info.cb = sizeof(STARTUPINFO);
	m_start_info.lpReserved = nullptr;
	m_start_info.lpDesktop = nullptr;				// not used (only in Win NT)
	m_start_info.lpTitle = nullptr;
	m_start_info.dwX = 0;							// win position  ->dwFlags=STARTF_USEPOSITION
	m_start_info.dwY = 0;
	m_start_info.dwXSize = 0;						// win size ->dwFlags=STARTF_USESIZE
	m_start_info.dwYSize = 0;
	m_start_info.dwXCountChars = 0;					// console width ->dwFlags=STARTF_USECOUNTCHARS
	m_start_info.dwYCountChars = 0;					// height
	m_start_info.dwFillAttribute = 0;				// console text & backgr. color

	// STARTF_FORCEOFFFEEDBACK						// dont show hourglass
	// STARTF_SCREENSAVER							// start as screensaver
	// STARTF_USESTDHANDLES							// use the hStd.. handles (see help)
	m_start_info.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; // STARTF_USESTDHANDLES; // STARTF_FORCEONFEEDBACK;  // show hourglass

	m_start_info.wShowWindow = SW_SHOW;				// show window state ->=STARTF_USESHOWWINDOW
	m_start_info.cbReserved2 = 0;
	m_start_info.lpReserved2 = nullptr;
	m_start_info.hStdInput = nullptr;				// input/output/error handles
	m_start_info.hStdOutput = nullptr;				// only used if ->STARTF_USESTDHANDLES
	m_start_info.hStdError = nullptr;

	m_process_handle = nullptr;

	m_pipe_std_output_read = nullptr;
	m_pipe_std_output_write = nullptr;

	// 两个回调函数是在外面设置好的，清空时要注意场合，有需要保留外部的回调函数的只清空状态时要注意参数
	if (also_clear_call_back)
	{
		m_end_call_back = nullptr;
		m_param_1 = nullptr;
		m_param_2 = nullptr;

		m_will_stop_by_call_back = nullptr;
	}
}

bool x_process_runner::create_process(DWORD& exit_code, const CString& program_path_name,
	const CString& parameter /*= _T("")*/, const CString& default_dir /*= _T("")*/,
	bool wait_for_termination /*= false*/, size_t time_out /*= 100*/)
{
	m_is_finished = false;
	m_will_stop_by_inner_flag = false;

	::CloseHandle(m_process_handle);
	::CloseHandle(m_pipe_std_output_read);
	::CloseHandle(m_pipe_std_output_write);

	// 创建管道用于读取命令行的标准输出
	SECURITY_ATTRIBUTES sa = { 0 };
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = nullptr;

	::CreatePipe(&m_pipe_std_output_read, &m_pipe_std_output_write, &sa, 0);
	m_start_info.hStdError = m_pipe_std_output_read;
	m_start_info.hStdOutput = m_pipe_std_output_write;

	PROCESS_INFORMATION process_info;

	CString app_path = file_tools::get_app_path_with_xg();
	CString command_line = program_path_name + _T(" ") + parameter;

	BOOL flag = ::CreateProcess(
		nullptr,								// pointer to name of executable module, not  used!
		(LPTSTR) (LPCTSTR) command_line,		// pointer to command line string
		nullptr,								// pointer to process security attributes
		nullptr,								// pointer to thread security attributes
		TRUE,									// handle inheritance flag
		CREATE_NO_WINDOW,						// CREATE_NO_WINDOW // creation flags
		nullptr,								// pointer to new environment block
		app_path,								// pointer to current directory name
		&m_start_info,							// pointer to STARTUPINFO
		&process_info);							// pointer to PROCESS_INFORMATION

	if (TRUE != flag)
	{
		::CloseHandle(m_pipe_std_output_read);
		::CloseHandle(m_pipe_std_output_write);

		m_process_handle = nullptr;
		m_pipe_std_output_read = nullptr;
		m_pipe_std_output_write = nullptr;

		m_is_finished = true;
		m_will_stop_by_inner_flag = false;
		return false;
	}

	m_process_handle = process_info.hProcess;

	wait_for_finish(exit_code);

	::CloseHandle(m_pipe_std_output_read);
	::CloseHandle(m_pipe_std_output_write);
	m_pipe_std_output_read = nullptr;
	m_pipe_std_output_write = nullptr;
	m_process_handle = nullptr;

	if (m_end_call_back && m_param_1 && m_param_2) m_end_call_back(*m_param_1, *m_param_2);

	return (0 == exit_code); // 为0表示正常结束
}

void x_process_runner::check_process_state(bool& is_finished, DWORD& exit_code, size_t check_time_out /*= 50*/)
{
	is_finished = false;
	exit_code = 0;

	if (!m_process_handle)
	{
		is_finished = true;
		return;
	}

	DWORD wait_code = ::WaitForSingleObject(m_process_handle, check_time_out);
	if (wait_code == WAIT_OBJECT_0)
	{
		// The state of the specified object is signaled.
		is_finished = true;
		::GetExitCodeProcess(m_process_handle, &exit_code);
	}
	else
	{
		is_finished = false;
	}
}

bool reset_output_message_prefix_and_return_status(CString& text, const CString& prefix)
{
	CString valid_prefix = prefix + _T("：");
	if (valid_prefix == text.Left(valid_prefix.GetLength()))
	{
		text.Delete(0, valid_prefix.GetLength());
		text = prefix + _T("：") + text; // 使用中文冒号
		return true;
	}

	valid_prefix = prefix + _T(":");
	if (valid_prefix == text.Left(valid_prefix.GetLength()))
	{
		text.Delete(0, valid_prefix.GetLength());
		text = prefix + _T("：") + text; // 使用中文冒号
		return true;
	}

	valid_prefix = prefix;
	if (valid_prefix == text.Left(valid_prefix.GetLength()))
	{
		text.Delete(0, valid_prefix.GetLength());
		text = prefix + _T("：") + text; // 使用中文冒号
		return true;
	}

	return false;
}

void dump_pipe_message_to_output(const CString& text)
{
	CString output_text = text;
	if (reset_output_message_prefix_and_return_status(output_text, _T("错误")))
		app_print_info(output_text, OO_LOG, MT_ERROR);
	else if (reset_output_message_prefix_and_return_status(output_text, _T("警告")))
		app_print_info(output_text, OO_LOG, MT_WARNING);
	else if (reset_output_message_prefix_and_return_status(output_text, _T("严重警告")))
		app_print_info(output_text, OO_LOG, MT_SERIOUS_WARNING);
	else if (reset_output_message_prefix_and_return_status(output_text, _T("调试")))
		app_print_info(output_text, OO_LOG, MT_DEBUG);
	else
		app_print_info(output_text, OO_LOG, MT_INFO);
}

void x_process_runner::wait_for_finish(DWORD& exit_code)
{
	exit_code = 0;

	// 4k的缓冲区（缓冲区太小的话，根本读不过写入的速度，会导致缓冲区中的信息溢出并丢失，太大的话内存分配容易失败）
	const int read_bytes_count_once = 4096;
	char text_buffer_in_byte[read_bytes_count_once + 1] = { 0 };
	DWORD number_of_bytes_to_read = 0;

	string delay_text; // 用于存储最后一行没有换行符前的内容，待遇到结束符时再输出(这种情况原则上属于非法数据，往控制台输出的字符串是没有结束符的，它自己先崩了)

	while (true)
	{
		bool is_finished = false;

		// 切忌延时必须为0,否则在计算过程中每一次异步检查判断都要阻塞掉相应的延时时长，这对程序的性能是一大损坏
		// 用0表示不等待，判断完直接返回状态，正是我要的效果
		check_process_state(is_finished, exit_code, 0);

		x_do_ui_events();
		if (m_will_stop_by_inner_flag) // 检测外部是否强制要求停止运行
		{
			force_stop();
			exit_code = 1;
			return;
		}

		bool need_stop = (m_will_stop_by_call_back ? m_will_stop_by_call_back() : false); // 检测外部的停止标志
		if (need_stop)
		{
			force_stop();
			exit_code = 2;
			return;
		}

		//确定管道内是否有数据,If the function succeeds, the return value is nonzero.
		if (TRUE == ::PeekNamedPipe(m_pipe_std_output_read, text_buffer_in_byte, read_bytes_count_once, &number_of_bytes_to_read, nullptr, nullptr))
		{
			if (number_of_bytes_to_read <= 0) // 管道中没有数据
			{
				if (is_finished) break;

				Sleep(100); // 没有数据先让线程休眠一下，否则如果消息队列中没有消息要处理，管道也没有数据，此时CPU是全速运行的
				continue;
			}

			// 读取管理中的数据，如果数据不足则阻塞进程，If the function succeeds, the return value is nonzero (TRUE).
			// 读取后的数据会从管理中移除
			if (FALSE == ::ReadFile(m_pipe_std_output_read, &text_buffer_in_byte, number_of_bytes_to_read, &number_of_bytes_to_read, nullptr))
			{
				if (is_finished) break; else continue;
			}

			text_buffer_in_byte[number_of_bytes_to_read] = '\0';
			for (int i = 0; i < (int) number_of_bytes_to_read; ++i)
			{
				if ('\r' == text_buffer_in_byte[i] || '\n' == text_buffer_in_byte[i])
				{
					if (delay_text.empty()) continue; // 连续多个换行不输出
					dump_pipe_message_to_output(string_tools::string_to_CString(delay_text));
					delay_text = _T("");

					need_stop = (m_will_stop_by_call_back ? m_will_stop_by_call_back() : false); // 检测外部的停止标志
					if (need_stop)
					{
						force_stop();
						exit_code = 2;
						return;
					}
				}
				else
				{
					delay_text += text_buffer_in_byte[i];
				}

				// 每处理500个字符检测一下状态
				if (0 == i % 500)
				{
					need_stop = (m_will_stop_by_call_back ? m_will_stop_by_call_back() : false); // 检测外部的停止标志
					if (need_stop)
					{
						force_stop();
						exit_code = 2;
						return;
					}
				}
			}
		}

		if (is_finished) break;
	}

	if (!delay_text.empty()) dump_pipe_message_to_output(string_tools::string_to_CString(delay_text));
	exit_code = 0;
}

void x_process_runner::force_stop()
{
	m_will_stop_by_inner_flag = true;

	::CloseHandle(m_pipe_std_output_read);
	::CloseHandle(m_pipe_std_output_write);
	m_pipe_std_output_read = nullptr;
	m_pipe_std_output_write = nullptr;

	TerminateProcess(m_process_handle, -1);
	CloseHandle(m_process_handle);
}

x_task_manager::x_task_manager()
{

}

x_task_manager::~x_task_manager()
{

}

void x_task_manager::remove_finished_tasks()
{
	for (size_t i = 0; i < m_process_runners.size(); /* nothing */)
	{
		if (m_process_runners[i].get_task_name())
			m_process_runners.erase(m_process_runners.begin() + i);
		else
			++i;
	}
}

bool x_task_manager::has_task(const CString& task_name)
{
	for (auto& x : m_process_runners)
		if (0 == task_name.CompareNoCase(x.get_task_name())) return true;

	return false;
}

void x_task_manager::force_stop_all_tasks()
{
	for (auto& x : m_process_runners) if (x.is_process_finished()) x.force_stop();
}

x_process_runner* x_task_manager::get_process_task(const CString& name)
{
	for (auto& x : m_process_runners)
		if (0 == name.CompareNoCase(x.get_task_name())) return &x;

	return nullptr;
}

DWORD x_task_manager::do_process_task(const CString& name, const x_process_object& obj)
{
	x_task_manager::remove_finished_tasks();

	DWORD exit_code;
	m_process_runners.push_back(x_process_runner());
	x_process_runner& runner = m_process_runners.back();

	runner.initialize(true);
	runner.set_call_back_function(name, obj.m_end_call_back, obj.m_param_1, obj.m_param_2, obj.m_will_be_stop);
	bool flag = runner.create_process(exit_code, obj.m_program_path_name, obj.m_parameter, _T(""), true, INFINITE);

	if (obj.m_wait_for_termination)
		return (flag ? 0 : exit_code); // 正常结束进程返回0，进程创建过程中出现问题或进程被强制停止
	else
		return 0;
}


//////////////////////////////////////////////////////////////////////////


x_process_object::x_process_object(const CString& app_path_name/* = _T("")*/, const CString& param/* = _T("")*/)
{
	m_program_path_name = app_path_name;
	m_parameter = param;

	m_wait_for_termination = true;
	m_time_out = 0;

	m_will_be_stop = nullptr;
	
	m_end_call_back = nullptr;
	m_param_1 = nullptr;
	m_param_2 = nullptr;
}

x_process_object::~x_process_object()
{
}

DWORD do_process_task(const CString& task_name, const x_process_object obj)
{
	return x_task_manager::do_process_task(task_name, obj);
}

bool has_process_task(const CString& task_name)
{
	return x_task_manager::has_task(task_name);
}

void force_stop_all_tasks()
{
	x_task_manager::force_stop_all_tasks();
}

