#include "stdafx.h"

#include <vector>

#include "csv_tools.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

// 用于切换区域语言
class raii_change_location
{
public:
	raii_change_location(int category, const TCHAR* locale)
	{
		m_category = category;
		m_locale = _tsetlocale(category, locale);
	}

	~raii_change_location()
	{
		_tsetlocale(m_category, m_locale);
	}

private:
	raii_change_location() = delete;
	raii_change_location(const raii_change_location&) = delete;
	void operator=(const raii_change_location&) = delete;

private:
	int m_category;
	CString m_locale;
};

//////////////////////////////////////////////////////////////////////////

void csv_tools::export_to_csv_file(const CString& strFileName, const vector<vector<CString>>& data)
{
	// if (data.empty()) return; // 数据为空也创建一个csv文件，只不过不填写数据

	// vc++的unicode字符串文件写入时如果有中文会有bug无法正确写入，必须要切换区域
	raii_change_location raii_loc(LC_ALL, _T("chs"));

	CStdioFile file(strFileName, CFile::modeCreate | CFile::modeWrite);
	try
	{
		CString strFileText;
		CString strCell;

		// 将数据转为逗号分隔每一行
		for (size_t i = 0; i < data.size(); ++i)
		{
			const vector<CString>& dataRow = data[i];

			for (size_t j = 0; j < dataRow.size(); ++j)
			{
				strCell = dataRow[j];
				strCell.Replace(_T("\""), _T("\"\"")); // 单元格中有双引号，写到文件要变为两个连续双引号
				
				// 单元格中包含了逗号和双引号以及换行，则整个内容要有双引号括起来
				if (-1 != strCell.FindOneOf(_T(",\"\r\n"))) strCell = _T("\"") + strCell + _T("\"");

				strFileText += strCell;
				if (j < dataRow.size() - 1) strFileText += _T(","); // 最后一个后面不要逗号
			}

			if (i < data.size() - 1) strFileText += _T("\n"); // 最后一行不要换行符
		}

		file.WriteString(strFileText);
	}
	catch (...)
	{
		// Nothing;
	}

	file.Close();
}

std::vector<std::vector<CString>> csv_tools::import_data_from_csv(const CString& strFileName, bool make_same_colum_count/* = true*/)
{
	vector<vector<CString>> data;
	import_data_from_csv(strFileName, data, make_same_colum_count);
	return data;
}

void put_one_string_to_row_item(vector<CString>& row_items, const CString& item, bool link_to_last_row)
{
	if (link_to_last_row)
	{
		if (row_items.empty())
			row_items.push_back(item);
		else
			// 引号引起来的单元格中允许有换行，要补上(不按windows系统默认补为\r\n，通用性好，而且在excel中测试过打开正常)
			row_items.back().Append(_T('\n') + item);
	}
	else
	{
		row_items.push_back(item);
	}
}

// 解析一行数据，如果数据行中含有换行，则会把已经正确解析的放到result中，最后一个没有完整的数据则放在result的最后
// 面，在下一次解析时会增量添加到最后一个字符串的后面
// 如果所有数据均能正确解析完，返回true（发生数据错误时会自动处理以保证尽最大可能得到尽可能多的数据）如果有未解析完
// 的数据需要再下一行的数据继续解析则返回false
// link_to_last_row为true表示当前行与result中的最后一个元素是csv中的同一行数据（一般情况下，解析返回值为false
// 时，此值传true，否则传false
// 注意传入的字符串中不能含有回车换行符
bool prive_split_csv_row_ex(vector<CString>& result, const CString& row, bool link_to_last_row)
{
	TCHAR current_char = _T('\0'), next_char = _T('\0');
	bool current_pos_in_cell = (link_to_last_row ? true : false); // 当前字符串是否处于双引号引起来的字符串中
	bool linked_to_last_row_has_been_done = false; // 如果当前行要链接到上一行，则需要链接的第一个单元格处理完成后就会置为true，后面的不再链接到上一行，而是正常处理
	CString cell_text = _T("");
	
	int row_count = row.GetLength();
	for (int i = 0; i < row_count; /* nothing */)
	{
		current_char = row[i];
		next_char = (i == row_count - 1 ? _T('\0') : row[i + 1]);

		if (_T(',') == current_char && !current_pos_in_cell)
		{
			if (current_pos_in_cell)
			{
				// 如果参数要求链到上一行则说明逗号在双引号中，属性单元格中的正常内容，不是列分隔符
				cell_text += current_char;
				
				// 如果已经到最后一个字符，则直接处理掉
				if (i == row_count - 1)
				{
					put_one_string_to_row_item(result, cell_text, link_to_last_row && false == linked_to_last_row_has_been_done);
					return current_pos_in_cell;
				}
			}
			else
			{
				put_one_string_to_row_item(result, cell_text, link_to_last_row && false == linked_to_last_row_has_been_done);
				linked_to_last_row_has_been_done = true; // 当前逗号不在引号内，是有效的列分隔符，此时不论需不需要链接到前一行都必须把此标记置为true表示后面的列不需要再考虑链接到上一行的情况
				cell_text.Empty();
			}

			++i;
			continue;
		}
		else if (_T('\"') == current_char && _T('\"') == next_char)
		{
			cell_text += _T('\"'); // 比使用_T("\"")运行代价小

			// 如果两个连续的引号在当前行的最末尾则直接处理掉
			if (i == row_count - 2) // 注意这里减2而不是减1
			{
				put_one_string_to_row_item(result, cell_text, link_to_last_row && false == linked_to_last_row_has_been_done);
				return current_pos_in_cell;
			}

			i += 2; // 一次跳过两个引号
			continue;
		}
		else if (_T('\"') == current_char && _T('\"') != next_char)
		{
			current_pos_in_cell = !current_pos_in_cell; // return 前要处理掉此标志

			// 如果已经到最后一个字符，则直接处理掉
			if (i == row_count - 1)
			{
				put_one_string_to_row_item(result, cell_text, link_to_last_row && false == linked_to_last_row_has_been_done);
				return current_pos_in_cell;
			}

			++i;
			continue;
		}
		else
		{
			cell_text += current_char;
			
			if (i == row_count - 1)
			{
				put_one_string_to_row_item(result, cell_text, link_to_last_row && false == linked_to_last_row_has_been_done);
				return current_pos_in_cell;
			}
			
			++i;
		}
	}

	return current_pos_in_cell;
}

void csv_tools::import_data_from_csv(const CString& strFileName, std::vector<std::vector<CString>>& data, bool make_same_colum_count/* = true*/)
{
	data.clear();

	CStdioFile file(strFileName, CFile::modeRead);
	try
	{
		CString csv_row;
		vector<CString> result;
		bool row_finished = true;
		// The CString version of ReadString() removes the '\n' if present; the LPTSTR version does not.
		while (TRUE == file.ReadString(csv_row))
		{
			row_finished = (false == prive_split_csv_row_ex(result, csv_row, !row_finished));
			if (row_finished)
			{
				data.push_back(result);
				result.clear();
			}
		}

		if (!row_finished)
		{
			data.push_back(result);
			result.clear();
		}
	}
	catch (...)
	{
		// nothing;
	}
	file.Close();

	// 确保所有行的数据列一样多，列按最大列计算，不足的数据以空格填充
	if (make_same_colum_count)
	{
		size_t szMax = 0;
		for (auto& x : data)
			if (x.size() > szMax) szMax = x.size();

		if (0 == szMax)
			data.clear();
		else
			for (auto& x : data) x.resize(szMax, _T(""));
	}
}