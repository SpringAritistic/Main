#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_MINI_FILE_LOG_H__	
#define __HMGLOBALTOOL_TOOLS_MINI_FILE_LOG_H__

#include <fstream>
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// 没有特殊需求，请不要使用大型的日志工具库，如log4cpp，boost的日志工具等,简单的东西
// 可维护性和性能往往是表现最好的形式[wyz]
//////////////////////////////////////////////////////////////////////////

// 一个极度精简的多线程安全文件日志记录类，仅打算支持文本格式
// 日志类是否使用单例模式由使用者自行决定，本工具类不作强制性的要求
// 文件在构造后将自动打开，直到析构时才关闭，以避免日志输出时每次打开进行文件寻址，极大的提高速度
// 为确保文件在程序异常时能记录下最新的日志且保证文件不损坏，每次输出时会自动强制将缓冲区中内容刷到文件中
// 本类仅用于输出窗口进行日志记录，属于内部私用的小工具，不导出
class HM_GlobalTool_EXT mini_file_log
{
public:
	mini_file_log(); // 必须要再调用set_file_path
	mini_file_log(const CString& file_full_path); // 内部已经调用了set_file_path
	~mini_file_log();
	mini_file_log(const mini_file_log&) = delete;
	mini_file_log(mini_file_log&&) = delete;
	mini_file_log& operator=(const mini_file_log&) = delete;

public:
	// 日志文件会先关闭再新的路径打开，允许在程序执行过程中调用此接口临时改变输出位置
	// 注意：路径中必须给定文件名
	void set_file_path(const CString& file_full_path_name);

	// 在文件尾实时追加文本记录，且当前文本会自动在行首添加时间
	// 为确保输出时的效率，不作容错性检查，如检查文件是否打开等，使用时请确保外部条件已经俱备
	void output_text(const CString& text);

private:
	std::ofstream m_output_fs;
	CRITICAL_SECTION m_cs_lock; // 使用临界区作为多线程访问控制比mutex要省资源，日志库尽量高效
};

#endif //__HMGLOBALTOOL_TOOLS_MINI_FILE_LOG_H__
