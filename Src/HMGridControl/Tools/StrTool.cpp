
//1/24/2019  5:36:01 PM
#include "stdafx.h"
#include "StrTool.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
_HM_GridControl_BEGIN



void split_string(std::vector<CString>& result, const CString& text, const CString& split)
{
	result.clear();
	if (_T("") == text) return;

	bool need_split_by_space = (split.Find(_T(' ')) >= 0);

	CString text_copied = text;
	if (need_split_by_space)
	{
		text_copied.Trim(_T(' ')); // 默认去除空白字符（空格，回车，制表符），此处仅去除空格字符
		for (int i = 0; i < text_copied.GetLength(); ++i)
		{
			if (_T(' ') != text_copied[i]) continue;

			// 把当前空格字符后面的连续空格字符全部去除
			int del_count = 0;
			for (int j = i + 1; j < text_copied.GetLength(); ++j)
			{
				if (_T(' ') == text_copied[j])
					++del_count;
				else
					break;
			}

			if (del_count > 0) text_copied.Delete(i + 1, del_count);
		}
	}

	CString tmp_str;
	for (int i = 0; i < text_copied.GetLength(); ++i)
	{
		if (-1 == split.Find(text_copied[i])) // 如果第i个字符不是分隔符
		{
			tmp_str += text_copied[i];
		}
		else
		{
			// 第一个字符为分隔符认为之前有一个空串，在这也插入
			result.push_back(tmp_str);
			tmp_str = _T("");
		}
	}
	// 把残留的是最后一个字符串加到结果中（有可能是个空串）
	// 最后一个字符为分隔符认为后面有一个空串，也会插入一个空串
	result.push_back(tmp_str);
}
std::vector<CString> split_string(const CString& text, const CString& split/* = _T(" ,;") */)
{
	std::vector<CString> result;
	split_string(result, text, split);
	return result;
}
void split_matrix_string(std::vector<std::vector<CString>>& result,
	const CString& text, TCHAR row_split /*= _T('\n')*/, TCHAR column_split /*= _T('\t')*/)
{
	result.clear();
	CString r_split_text;
	r_split_text.AppendChar(row_split);
	CString c_split_text;
	c_split_text.AppendChar(column_split);

	vector<CString> rows;
	split_string(rows, text, r_split_text);
	for (CString& row : rows)
	{
		vector<CString> columns;
		split_string(columns, row, c_split_text);
		result.push_back(columns);
	}
}

void merge_string(CString& result, const std::vector<CString>& all_strings,
	const CString& split /*= _T(",")*/, bool end_with_split/* = false*/)
{
	result = _T("");

	if (all_strings.empty()) return;
	if (split.IsEmpty()) return;

	result = all_strings[0];
	for (size_t i = 1; i < all_strings.size(); ++i)
		result = result + split + all_strings[i];

	if (!result.IsEmpty() && end_with_split) result += split;
}

CString merge_string(const std::vector<CString>& all_strings,
	const CString& split /*= _T(",")*/, bool end_with_split/* = false*/)
{
	CString result;
	merge_string(result, all_strings, split, end_with_split);
	return result;
}

void merge_matrix_string(CString& result, const std::vector<std::vector<CString>>& all_strings,
	TCHAR row_split /*= _T('\n')*/, TCHAR column_split /*= _T('\t')*/, bool end_with_split/* = false*/)
{
	result = _T("");

	for (size_t row = 0; row < all_strings.size(); ++row)
	{
		for (size_t col = 0; col < all_strings[row].size(); ++col)
		{
			result += all_strings[row][col];
			if (col + 1 < all_strings[row].size()) result.AppendChar(column_split);
		}

		if (row + 1 < all_strings.size()) result.AppendChar(row_split);
	}

	if (!result.IsEmpty() && end_with_split) result.AppendChar(row_split);
}



vector<size_t> FindIndexs(const std::vector<CString>& inStr, const std::vector<CString>& findStr)
{
	vector<size_t>vec;
	if (inStr.size() > 0 && findStr.size() > 0)
	{
		for (const auto& item : findStr)
		{
			auto it = std::find(inStr.begin(), inStr.end(), item);
			if (it != inStr.end())
				vec.emplace_back(std::distance(inStr.begin(), it));
		}
	}
	return vec;
}

void SelfRepalceToNormReturn(CString &str)
{
	str.Replace("\r\n", "\n");
}
CString RepalceToNormReturn(const CString &strOri)
{
	CString str(strOri);
	SelfRepalceToNormReturn(str);
	return str;
}
vector<CString>GetMutiStr(const CString& strSource)
{
	vector<CString>vecStr;
	CString str = RepalceToNormReturn( strSource);
	split_string(vecStr, str, "\n");
	return vecStr; 
}

CSize GetTextExtent(CDC& dc, const CString& strSource, bool ExtX , bool ExtY)
{
	vector<CString>&vecStr = GetMutiStr(strSource);
	CSize size;
	for (CString &strTitle : vecStr)
	{
		CSize item= dc.GetTextExtent(strTitle);
		size.cx = max(size.cx, item.cx);
		size.cy += item.cy;
	}
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	return size + CSize((ExtX ? (tm.tmAveCharWidth + tm.tmMaxCharWidth) / 2 : 0), (ExtY? tm.tmOverhang:0));//适当增加扩展
}
void ShowText(CDC& dc, const CString& str, int EachHeight)
{
	vector<CString>&vecStr = GetMutiStr(str);
	for (size_t i = 0; i < vecStr.size();++i)
	{
		dc.TextOut(0, i*EachHeight, vecStr[i]);
	}

}



_HM_GridControl_END
