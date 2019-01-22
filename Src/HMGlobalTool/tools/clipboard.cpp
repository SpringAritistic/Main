#include "stdafx.h"
#include <string>

#include "string_tools.h"

#include "clipboard.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;



CString clipboard_tool::get_text()
{
	CString text;
	get_text(text);
	return text;
}

bool clipboard_tool::get_text(CString& text)
{
	text = _T("");

	if (!::OpenClipboard(nullptr)) return false;
	if (::IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		HGLOBAL global_handle = ::GetClipboardData(CF_UNICODETEXT);
		if (!global_handle)
		{
			::CloseClipboard();
			return false;
		}

		wchar_t* data_buffer = (wchar_t*)::GlobalLock(global_handle);
		if (!data_buffer)
		{
			::CloseClipboard();
			return false;
		}

		size_t data_size_in_byte = (size_t)::GlobalSize(global_handle) / sizeof(wchar_t);
		if (0 == data_size_in_byte)
		{
			::GlobalUnlock(global_handle);
			::CloseClipboard();
			return false;
		}

		wstring uni_text = L"";
		for (size_t i = 0; i < data_size_in_byte; ++i)
			uni_text.append(1, data_buffer[i]);

		uni_text[data_size_in_byte - 1] = L'\0';

		::GlobalUnlock(global_handle);
		::CloseClipboard();

		text = string_tools::wstring_to_CString(uni_text);
		return true;
	}
	else if (::IsClipboardFormatAvailable(CF_TEXT))
	{
		HGLOBAL global_handle = ::GetClipboardData(CF_TEXT);
		if (global_handle)
		{
			::CloseClipboard();
			return false;
		}

		char* data_buffer = (char*)::GlobalLock(global_handle);
		if (!data_buffer)
		{
			::CloseClipboard();
			return false;
		}

		size_t data_size_in_byte = (size_t)::GlobalSize(global_handle) / sizeof(char);
		if (0 == data_size_in_byte)
		{
			::GlobalUnlock(global_handle);
			::CloseClipboard();
			return false;
		}

		string mbcs_text = "";
		for (size_t i = 0; i < data_size_in_byte; ++i)
			mbcs_text.append(1, data_buffer[i]);

		mbcs_text[data_size_in_byte - 1] = L'\0';

		::GlobalUnlock(global_handle);
		::CloseClipboard();

		text = string_tools::string_to_CString(mbcs_text);
		return true;
	}
	else
	{
		::CloseClipboard();
		return false;
	}
}

bool clipboard_tool::set_text(const CString& text)
{
	wstring uni_text = string_tools::CString_to_wstring(text);
	if (!::OpenClipboard(nullptr)) return false;

	::EmptyClipboard();
	HGLOBAL global_handle = ::GlobalAlloc(GMEM_ZEROINIT, (uni_text.length() + 1) * sizeof(wchar_t));
	if (!global_handle)
	{
		::CloseClipboard();
		return false;
	}

	wchar_t* data_buffer = (wchar_t*)::GlobalLock(global_handle);
	if (!data_buffer)
	{
		::CloseClipboard();
		return false;
	}

	for (size_t i = 0; i < uni_text.length(); ++i)
		data_buffer[i] = uni_text[i];
		
	data_buffer[uni_text.length()] = L'\0';

	::GlobalUnlock(global_handle);
	::SetClipboardData(CF_UNICODETEXT, global_handle);
	::CloseClipboard();

	return true;
}

bool clipboard_tool::get_matirx_text(std::vector<std::vector<CString>>& data,
	TCHAR row_split /*= _T('\n')*/, TCHAR column_split /*= _T('\t')*/)
{
	data.clear();

	CString text;
	if (false == get_text(text)) return false;

	// windows上一般换行按\r\n处理
	text.Replace(_T("\r"), _T(""));
	string_tools::split_matrix_string(data, text, row_split, column_split);

	// 末尾的空行不要
	for (int i = ((int)data.size()) - 1; i >= 0; --i)
		if (data[i].empty())
			data.erase(data.begin() + i);

	return true;
}

bool clipboard_tool::set_matrix_text(const std::vector<std::vector<CString>>& data,
	TCHAR row_split /*= _T('\n')*/, TCHAR column_split /*= _T('\t')*/)
{
	CString text;
	string_tools::merge_matrix_string(text, data, row_split, column_split);
	// windows上一般换行按\r\n处理
	text.Replace(_T("\n"), _T("\r\n"));
	return set_text(text);
}
