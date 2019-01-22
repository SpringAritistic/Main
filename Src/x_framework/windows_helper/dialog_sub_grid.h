#pragma once
#include <array>
#include <vector>
#include <utility>
#include "../resource.h"
#include "app_dialog_base.h"


// 表格的头文件有问题，内部进行lib连接和命名空间的声明，放在头文件包含会大量扩散lib指令的处理及命名空间污
// 染问题，因此放在cpp中包含，另外，表格头文件中已经声明了表格的命名空间，当前的这个cpp对应的头文件不得再
// 使用namespace方式声明，否则变成了嵌套的同名命名空间造成编译失败，而应该使用不带namespace的方式声明，
// 具体使用本二级表工具时，在cpp中先直接包含#include "THGridControl/THGridControlInc.h即可，因此，
// 以下声明相当于已经引入ZYH_GRID命名空间后作的声明

class CTHExcel;
class CTHDataBase;

// CTHCELL.h的头文件有严重问题，它包含了表格的THGridControlMac.h文件，后者已经将代码放在表格命名空间下
// 了，而CTHCELL.h又使用一下，相当于在表格命名空间内部又嵌套一个同名的命名空间，因此必须再加一下表格命名空间
namespace ZYH_GRID
{
	class CTHCell;
}

struct NM_THGRID; // 不在任何命名空间下



// 二级单表对话框基类，当对话框有图标时自动设置为主程序的图标
// 二级对话框由此派生，需重写get_data_from_ui/set_data_to_ui/
class X_FRAMEWORK_API dialog_sub_grid : public app_dialog_base
{
	DECLARE_SERIAL(dialog_sub_grid)
	DECLARE_MESSAGE_MAP()

public:
	dialog_sub_grid(CWnd* parent = nullptr);
	dialog_sub_grid(const CString& template_name, int default_height, int default_width,
		const CString& title = _T(""), CWnd* parent = nullptr);

	virtual ~dialog_sub_grid();

public:
	enum { IDD = IDD_DIALOG_SUB_GRID };

public:
	CString m_text;
	CString m_title;
	CString m_template_name;
	int m_default_left_pos; // 为0会自动居中
	int m_default_top_pos; // 为0会自动居中
	int m_default_height;
	int m_default_width;
	
protected:
	virtual void get_data_from_ui();
	virtual void set_data_to_ui();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void adjust_window_layout();
	virtual void set_grid_data_base(); // 初始化内部的m_db对象，默认实现从m_template_name实例化表格

	// 提供虚函数，可以让派生类简化消息响应的代码，因为表格的ID没有放出来
	virtual bool grid_remove_data_in_grid(NM_THGRID* notify_struct, LRESULT* result);
	virtual bool grid_add_data_in_grid(NM_THGRID* notify_struct, LRESULT* result);
	virtual bool grid_row_col_size_changed(NM_THGRID* notify_struct, LRESULT* result);
	virtual bool grid_cell_value_changed(NM_THGRID* notify_struct, LRESULT* result);
	virtual bool grid_cell_button_clicked(NM_THGRID* notify_struct, LRESULT* result);
	virtual bool grid_cell_focus_changed(NM_THGRID* notify_struct, LRESULT* result);

protected:
	CTHExcel* m_excel;
	CTHDataBase* m_db;
	CButton m_button_ok;
	CButton m_button_cancel;

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void on_clicked_ok();

	afx_msg BOOL on_remove_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_add_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_row_col_size_changed(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_cell_value_changed(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_cell_button_clicked(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_cell_focus_changed(UINT id, NMHDR* notify_struct, LRESULT* result);
};



// 含有两列的二级行表，主要用于输入数组形式的key/value对应的数据，如地震波、坐标等
// 默认单元格类型为文本框，如果有特殊要求需改写set_cell_type()虚函数
class X_FRAMEWORK_API dialog_sub_grid_key_value_array : public dialog_sub_grid
{
public:
	dialog_sub_grid_key_value_array(CWnd* parent = nullptr);
	dialog_sub_grid_key_value_array(const CString& title, int default_height, int default_width,
		const CString& key_title, const CString& value_title, CWnd* parent = nullptr,
		int index_width = 50, int key_width = 70, int value_width = 70);

	virtual ~dialog_sub_grid_key_value_array();

public:
	// 返回数据
	void get_result_data_pair(std::vector<std::pair<CString, CString>>& data) const;
	void get_result_data_array(std::vector<std::array<CString, 2>>& data) const;
	void get_result_data_vector(std::vector<std::vector<CString>>& data) const;
	void get_result_data_string(CString& data) const;

	// 设置界面数据
	void set_result_data_pair(const std::vector<std::pair<CString, CString>>& data);
	void set_result_data_array(const std::vector<std::array<CString, 2>>& data);
	void set_result_data_vector(const std::vector<std::vector<CString>>& data);
	void set_result_data_string(const CString& data, TCHAR row_split_char = _T(';'), TCHAR column_split_char = _T(','));

private:
	TCHAR	m_row_split_char = _T(';');
	TCHAR	m_column_split_char = _T(',');
	CString m_key_title;
	CString m_value_title;
	std::vector<std::pair<CString, CString>> m_matrix_data;

protected:
	virtual void get_data_from_ui();
	virtual void set_data_to_ui();
	virtual void set_grid_data_base();
	virtual void set_cell_type(int col_index, ZYH_GRID::CTHCell* cell); // 1：key,2:value
};

