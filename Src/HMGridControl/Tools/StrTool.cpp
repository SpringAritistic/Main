
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

vector<CString>GetMutiStr(const CString& strSource)
{
	vector<CString>vecStr;
	CString str = strSource;
	str.Replace("\r\n", "\n");
	split_string(vecStr, str, "\n");
	return vecStr; 
}

CSize GetTextExtent(CClientDC& dc, const CString& strSource)
{
	vector<CString>&vecStr = GetMutiStr(strSource);
	CSize size;
	for (CString &strTitle : vecStr)
	{
		CSize item= dc.GetTextExtent(strTitle);
		size.cx = max(size.cx, item.cx);
		size.cy += item.cy;
	}
	return size;
}
void ShowText(CClientDC& dc, const CString& str, int EachHeight)
{
	vector<CString>&vecStr = GetMutiStr(str);
	for (size_t i = 0; i < vecStr.size();++i)
	{
		dc.TextOut(0, i*EachHeight, vecStr[i]);
	}

}



_HM_GridControl_END
