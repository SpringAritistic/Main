#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_GLOBAL_PARAMETERS_H__
#define __HMGLOBALTOOL_TOOLS_GLOBAL_PARAMETERS_H__

#include <functional>

// 全局参数类
class HM_GlobalTool_EXT global_parameters
{
private:
	global_parameters() = delete;
	~global_parameters() = delete;

public:
	// name参数改为不传引用,防止外部传入的字符串生命周期比线程短先析构导致的崩溃
	static void set_parameter(const CString name, const CString& value);	// 多线程安全的，请确保传入的参数在多线程下是可安全访问的
	static CString get_parameter(const CString name);						// 多线程安全的，请确保传入的参数在多线程下是可安全访问的
	static void get_parameter(const CString name, CString& value);			// 注意此接口传入的用于返回结果的value参数如果多个线程可同时访问，则内部设置数据时可能导致崩溃，请注意传入多线程访问安全的对象
	static bool parameter_exist(const CString name);						// 多线程安全的，请确保传入的参数在多线程下是可安全访问的

	// 如果是在线程中使用此全局参数类，尽量使用id形式，字符串方式涉及到字符串对象的构造，内存分配等隐藏的内存操
	// 作，保证外部传入的参数的线程安全是很难的，尤其是容易忽视参数整个传递进程中也要作线程保护，造成崩溃和低效的实现
	// value为-1表示id不存在或无法获得数值
	static void set_parameter(int id, int value);							// 多线程安全的，请确保传入的参数在多线程下是可安全访问的
	static void get_parameter(int id, int& value);							// 注意此接口传入的用于返回结果的value参数如果多个线程可同时访问，则内部设置数据时可能导致崩溃，请注意传入多线程访问安全的对象
	static int get_parameter(int id);										// 注意此接口传入的用于返回结果的value参数如果多个线程可同时访问，则内部设置数据时可能导致崩溃，请注意传入多线程访问安全的对象
	static bool parameter_exist(int id);									// 多线程安全的，请确保传入的参数在多线程下是可安全访问的
};

// 在作用域范围内自动设置和恢复全局参数
class HM_GlobalTool_EXT auto_set_global_parameters
{
public:
	auto_set_global_parameters(const CString& name, const CString& value_begin, const CString& value_end);
	auto_set_global_parameters(int id, int value_begin, int value_end); // id小于1000的由系统保留占用，用户传入的id请确保大于等于于1000
	auto_set_global_parameters(std::function<void()> set_fun, std::function<void()> undo_fun);
	~auto_set_global_parameters();
	auto_set_global_parameters(const auto_set_global_parameters&) = delete;
	auto_set_global_parameters(auto_set_global_parameters&&) = delete;

private:
	int		m_type;

	CString m_name;
	CString m_value_begin_string;
	CString m_value_end_string;
	std::function<void()> m_undo_fun;

	int		m_id;
	int		m_int_value_begin;
	int		m_int_value_end;
};
#endif //__HMGLOBALTOOL_TOOLS_GLOBAL_PARAMETERS_H__
