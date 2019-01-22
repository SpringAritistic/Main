#include "StdAfx.h"

#include "..\tools\string_tools.h"

#include "hook_setting_data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

frame_hook_setting::frame_hook_setting()
{
}

frame_hook_setting::~frame_hook_setting()
{
}

void frame_hook_setting::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);

		mt.write(_T("m_hook_name"), m_hook_name);
		mt.write(_T("m_ref_line_name"), m_ref_line_name);
		mt.write(_T("m_is_top_hook"), m_is_top_hook);
		mt.write(_T("m_node_index"), m_node_index);
		mt.write(_T("m_node_begin_layer"), m_node_begin_layer);
		mt.write(_T("m_segment_name"), m_segment_name);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_hook_name"), m_hook_name);
			mt.read(_T("m_ref_line_name"), m_ref_line_name);
			mt.read(_T("m_is_top_hook"), m_is_top_hook);
			mt.read(_T("m_node_index"), m_node_index);
			mt.read(_T("m_node_begin_layer"), m_node_begin_layer);
			mt.read(_T("m_segment_name"), m_segment_name);
		}
	}
}

void string_to_hook_settings(const CString& setting_text, std::vector<frame_hook_setting>& setting_data)
{
	if (_T("") == setting_text) return;

	vector<CString> setting_rows = string_tools::split_string(setting_text, _T(";"));
	if (setting_rows.empty())
	{
		frame_hook_setting hook_settings;
		setting_data.push_back(hook_settings); // 放置一个空的数据行,防止用户预留的数据被后面的代码校正后填充上数据
		return;
	}

	for (size_t i = 0; i != setting_rows.size(); ++i)
	{
		frame_hook_setting hook_settings;
		if (_T("") == setting_rows[i]) // 放置一个空的数据行,防止用户预留的数据被后面的代码校正后填充上数据
		{
			setting_data.push_back(hook_settings);
			continue;
		}

		vector<CString> setting_cells = string_tools::split_string(setting_rows[i], _T(","));
		if (setting_cells.size() != 2) setting_cells.resize(2);
		hook_settings.m_hook_name = setting_cells[0];
		hook_settings.m_hook_name.Trim();

		vector<CString> setting_ref_lines = string_tools::split_string(setting_cells[1], _T("["));
		if (setting_ref_lines.size() != 2) setting_ref_lines.resize(2);
		hook_settings.m_ref_line_name = setting_ref_lines[0];
		hook_settings.m_ref_line_name.Trim();

		vector<CString> setting_segment = string_tools::split_string(setting_ref_lines[1], _T("]"));
		if (setting_segment.size() != 2) setting_segment.resize(2);
		hook_settings.m_segment_name = setting_segment[1];
		hook_settings.m_segment_name.Trim();

		// 如果数据残缺,则补为空格,再解析,这样解析之前确保数据长度足够,防止下标越界
		if (setting_segment[0].GetLength() < 3) setting_segment[0] += _T("   "); // 加三个空格,数据长度一定足够了
		// 数据长度满足要求
		if (_T("+") == setting_segment[0].Left(1))
			hook_settings.m_is_top_hook = _T("顶部");
		else if (_T("-") == setting_segment[0].Left(1))
			hook_settings.m_is_top_hook = _T("底部");
		else
			hook_settings.m_is_top_hook = _T("");

		if (_T("S") == setting_segment[0].Right(1))
			hook_settings.m_node_begin_layer = _T("弯起层");
		else if (_T("E") == setting_segment[0].Right(1))
			hook_settings.m_node_begin_layer = _T("弯止层");
		else
			hook_settings.m_node_begin_layer = _T("");

		CString strID = setting_segment[0].Mid(1, setting_segment[0].GetLength() - 2); // 去掉首尾两端的两个字符
		strID.Trim();
		if (_T("") == strID)
			hook_settings.m_node_index = _T("");
		else
			hook_settings.m_node_index.Format(_T("%d"), _ttoi(strID)); // 没有直接进行字符串赋值,是为了过滤掉非数字字符

		setting_data.push_back(hook_settings);
	}
}

void hook_settings_to_string(const std::vector<frame_hook_setting>& setting_data, CString& setting_text)
{
	CString setting_rows;
	setting_text = _T("");
	vector<frame_hook_setting> hook_settings = setting_data; // 参数为const的，要对字符串进行一行修整，所以新定义一个非const的

	for (size_t i = 0; i != hook_settings.size(); ++i)
	{
		// 清除字符串两端的空白字符
		hook_settings[i].m_hook_name.Trim();
		hook_settings[i].m_ref_line_name.Trim();
		hook_settings[i].m_is_top_hook.Trim();
		hook_settings[i].m_node_index.Trim();
		hook_settings[i].m_node_begin_layer.Trim();
		hook_settings[i].m_segment_name.Trim();


		// 如果是用户预留的空行则原样保存空行数据
		if (_T("") == hook_settings[i].m_hook_name
			&& _T("") == hook_settings[i].m_ref_line_name
			&& _T("") == hook_settings[i].m_is_top_hook
			&& _T("") == hook_settings[i].m_node_index
			&& _T("") == hook_settings[i].m_node_begin_layer
			&& _T("") == hook_settings[i].m_segment_name)
		{
			setting_text += _T(" , [   ] "); // 添加一个空数据行
		}
		else
		{
			// 由于有if条件的限制，这里传入的数据起始状态不会全部为空
			// 为防止数据错乱，如果某个字段没有填写信息，则用空格代替
			setting_rows.Format(_T("%s,%s[%s%s%s]%s"),
				(_T("") == hook_settings[i].m_hook_name ? _T(" ") : hook_settings[i].m_hook_name),
				(_T("") == hook_settings[i].m_ref_line_name ? _T(" ") : hook_settings[i].m_ref_line_name),
				(_T("顶部") == hook_settings[i].m_is_top_hook ? _T("+") : _T("-")),
				(_T("") == hook_settings[i].m_node_index ? _T("0") : hook_settings[i].m_node_index), // 这个没有填写表示0
				(_T("弯起层") == hook_settings[i].m_node_begin_layer ? _T("S") : _T("E")),
				(_T("") == hook_settings[i].m_segment_name ? _T(" ") : hook_settings[i].m_segment_name));

			setting_text += setting_rows;
		}
		if (i < hook_settings.size() - 1) setting_text += _T(";"); // 最后一个不加_T(";")
	}
}
