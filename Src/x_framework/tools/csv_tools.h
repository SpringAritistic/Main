#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_CSV_TOOLS_H__
#define __HMGLOBALTOOL_TOOLS_CSV_TOOLS_H__


// 注意本工具用于导入、导出csv格式，但不支持单元格中有回车换行符（Excel中使用Alt + Enter输入）
class X_FRAMEWORK_API csv_tools
{
public:
	static void export_to_csv_file(const CString& strFileName, const std::vector<std::vector<CString>>& data);
	static std::vector<std::vector<CString>> import_data_from_csv(const CString& strFileName, bool make_same_colum_count = true);
	static void import_data_from_csv(const CString& strFileName, std::vector<std::vector<CString>>& data, bool make_same_colum_count = true);

private:
	csv_tools() = delete;
	csv_tools(const csv_tools&) = delete;
	~csv_tools() = delete;
	void operator=(const csv_tools&) = delete;
};
#endif //__HMGLOBALTOOL_TOOLS_CSV_TOOLS_H__
