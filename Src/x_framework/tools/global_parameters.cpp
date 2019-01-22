#include "stdafx.h"

#include <map>
#include <utility>

#include "global_parameters.h"
#include <mutex>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////////

// 必须加static，主因是控制其作用域不扩散到此文件名造成名称冲突
static std::map<CString, CString> all_parameters_string;
static std::map<int, int> all_parameters_int;
static std::mutex parameters_options_mutex_string;
static std::mutex parameters_options_mutex_int;

//////////////////////////////////////////////////////////////////////////


void global_parameters::set_parameter(const CString name, const CString& value)
{
	std::lock_guard<std::mutex> lock(parameters_options_mutex_string);
	all_parameters_string[name] = value;
}

void global_parameters::set_parameter(int id, int value)
{
	std::lock_guard<std::mutex> lock(parameters_options_mutex_int);
	all_parameters_int[id] = value;
}

CString global_parameters::get_parameter(const CString name)
{
	CString val;
	get_parameter(name, val);
	return val;
}

void global_parameters::get_parameter(const CString name, CString& value)
{
	std::lock_guard<std::mutex> lock(parameters_options_mutex_string);
	const auto it = all_parameters_string.find(name);
	if (it == all_parameters_string.end())
		value = _T("");
	else
		value = it->second;
}

void global_parameters::get_parameter(int id, int& value)
{
	std::lock_guard<std::mutex> lock(parameters_options_mutex_int);
	const auto it = all_parameters_int.find(id);
	if (it == all_parameters_int.end())
		value = -1;
	else
		value = it->second;
}

int global_parameters::get_parameter(int id)
{
	int res = -1;
	get_parameter(id, res);
	return res;
}

bool global_parameters::parameter_exist(const CString name)
{
	std::lock_guard<std::mutex> lock(parameters_options_mutex_string);
	auto it = all_parameters_string.find(name);
	return (it != all_parameters_string.end());
}

bool global_parameters::parameter_exist(int id)
{
	std::lock_guard<std::mutex> lock(parameters_options_mutex_int);
	auto it = all_parameters_int.find(id);
	return (it != all_parameters_int.end());
}


//////////////////////////////////////////////////////////////////////////


auto_set_global_parameters::auto_set_global_parameters(const CString& name, const CString& value_begin, const CString& value_end)
{
	m_type = 0;

	m_name = _T("");
	m_value_begin_string = _T("");
	m_value_end_string = _T("");

	if (name.IsEmpty()) return;

	m_name = name;
	m_value_begin_string = value_begin;
	m_value_end_string = value_end;

	m_undo_fun = nullptr;

	m_id = 0;
	m_int_value_begin = 0;
	m_int_value_end = 0;

	global_parameters::set_parameter(m_name, m_value_begin_string);
}

auto_set_global_parameters::auto_set_global_parameters(std::function<void()> set_fun, std::function<void()> undo_fun)
{
	m_type = 1;
	m_undo_fun = undo_fun;
	set_fun();

	m_id = 0;
	m_int_value_begin = 0;
	m_int_value_end = 0;
}

auto_set_global_parameters::auto_set_global_parameters(int id, int value_begin, int value_end)
{
	m_type = 2;
	m_undo_fun = nullptr;

	m_id = id;
	m_int_value_begin = value_begin;
	m_int_value_end = value_end;
}

auto_set_global_parameters::~auto_set_global_parameters()
{
	if (0 == m_type)
	{
		global_parameters::set_parameter(m_name, m_value_end_string);
		m_name = _T("");
		m_value_begin_string = _T("");
		m_value_end_string = _T("");
	}
	else if (1 == m_type)
	{
		m_undo_fun();
	}
	else if (2 == m_type)
	{
		global_parameters::set_parameter(m_id, m_int_value_end);
		m_id = 0;
		m_int_value_begin = 0;
		m_int_value_end = 0;
	}
	else
	{
		// nothing;
	}

	m_name = _T("");
	m_value_begin_string = _T("");
	m_value_end_string = _T("");

	m_id = 0;
	m_int_value_begin = 0;
	m_int_value_end = 0;

	m_undo_fun = nullptr;
}

