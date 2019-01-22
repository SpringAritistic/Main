#include "stdafx.h"

#include <chrono>
#include <time.inl>
#include <time.h>
#include <sstream>
#include <iomanip>

#include "../tools/string_tools.h"
#include "../tools/file_tools.h"

#include "mini_file_log.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace std::chrono;


class auto_log_file_access_lock
{
public:
	auto_log_file_access_lock(CRITICAL_SECTION* sec) { m_sec = sec; ::EnterCriticalSection(m_sec); }
	~auto_log_file_access_lock() { ::LeaveCriticalSection(m_sec); }
	auto_log_file_access_lock(const auto_log_file_access_lock&) = delete;
	auto_log_file_access_lock(auto_log_file_access_lock&&) = delete;
	auto_log_file_access_lock& operator=(const auto_log_file_access_lock&) = delete;

private:
	CRITICAL_SECTION* m_sec;
};


mini_file_log::mini_file_log()
{
	::InitializeCriticalSection(&m_cs_lock);
}

mini_file_log::~mini_file_log()
{
	m_output_fs.close();
	::DeleteCriticalSection(&m_cs_lock);
}

mini_file_log::mini_file_log(const CString& file_full_path)
{
	::InitializeCriticalSection(&m_cs_lock);
	set_file_path(file_full_path);
}

void mini_file_log::set_file_path(const CString& file_full_path)
{
	CString file_path = file_tools::get_path_from_file_path_name(file_full_path);
	if (!::PathFileExists(file_path))
	{
		if (!file_tools::create_directory_nested(file_path)) return;
	}

	auto_log_file_access_lock tmpp_lock(&m_cs_lock); // 确保多线程访问安全
	if (m_output_fs.is_open()) m_output_fs.close();

	m_output_fs.open(file_full_path, ios::in | ios::trunc);
	if (!m_output_fs.is_open()) return;
}

void mini_file_log::output_text(const CString& text)
{
	time_t tm_tick = system_clock::to_time_t(system_clock::now());
	tm cur_tm;
	localtime_s(&cur_tm, &tm_tick);

	stringstream ss;
	ss << put_time(&cur_tm, _T("%Y-%m-%d %H:%M:%S  "));
	string tm_text(ss.str());
	tm_text += string_tools::CString_to_string(text) + _T("\n");

	auto_log_file_access_lock tmpp_lock(&m_cs_lock); // 确保多线程访问安全
	if (!m_output_fs.is_open()) return; // 如果构造或设置了非法路径则文件会会处于未打开状态，此时直接返回

	m_output_fs.write(tm_text.c_str(), tm_text.length());
	m_output_fs.flush(); // 确保文件缓存实时输出到文件中
}
