#pragma once

#ifndef __HMGLOBALTOOL_TOOLS_CLIPBOARD_H__
#define __HMGLOBALTOOL_TOOLS_CLIPBOARD_H__



// 剪切板操作工具类
class X_FRAMEWORK_API clipboard_tool
{
private:
	clipboard_tool() = delete;
	~clipboard_tool() = delete;

public:
	static CString get_text();
	static bool get_text(CString& text);
	static bool set_text(const CString& text);

	// 读取或写入excel或主流软件的行列式表格文本数据（\t分隔列\n分隔行）
	static bool get_matirx_text(std::vector<std::vector<CString>>& data, TCHAR row_split = _T('\n'), TCHAR column_split = _T('\t'));
	static bool set_matrix_text(const std::vector<std::vector<CString>>& data, TCHAR row_split = _T('\n'), TCHAR column_split = _T('\t'));
};
#endif //__HMGLOBALTOOL_TOOLS_CLIPBOARD_H__

