#include "StdAfx.h"

// 表格的头文件有问题，内部进行lib连接和命名空间的声明，放在头文件包含会大量扩散lib指令的处理及命名空间污
// 染问题，因此放在cpp中包含，另外，表格头文件中已经声明了表格的命名空间，当前的这个cpp对应的头文件不得再
// 使用namespace方式声明，否则变成了嵌套的同名命名空间造成编译失败，而应该使用不带namespace的方式声明
#include "THGridControl/THGridControlMac.h"
#include "THGridControl/THGridControlInc.h"

#include "..\tools\table_templates.h"
#include "../tools/string_tools.h"

#include "dialog_sub_grid.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace ZYH_GRID;


IMPLEMENT_SERIAL(dialog_sub_grid, app_dialog_base, 1)
BEGIN_MESSAGE_MAP(dialog_sub_grid, app_dialog_base)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, on_clicked_ok)
	ON_NOTIFY_EX(GN_CELLCHANGE_POST, IDC_STATIC_SUB_GRID, on_cell_value_changed)
	ON_NOTIFY_EX(GN_REMOVEROWS_POST, IDC_STATIC_SUB_GRID, on_remove_data_in_grid) //表格删记录
	ON_NOTIFY_EX(GN_INSERTROWS_POST, IDC_STATIC_SUB_GRID, on_add_data_in_grid) //表格增记录
	ON_NOTIFY_EX(GN_COLWIDTH_ED, IDC_STATIC_SUB_GRID, on_row_col_size_changed) //列宽改变
	ON_NOTIFY_EX(GN_ROWHEIGHT_ED, IDC_STATIC_SUB_GRID, on_row_col_size_changed) //行高改变
	ON_NOTIFY_EX(GN_BUTTONCLICKED, IDC_STATIC_SUB_GRID, on_cell_button_clicked) //单击表格中的按钮
	ON_NOTIFY_EX(GN_FOCUSCHANGE_POST, IDC_STATIC_SUB_GRID, on_cell_focus_changed) //表格选中单元格改变
END_MESSAGE_MAP()


dialog_sub_grid::dialog_sub_grid(CWnd* parent /*= nullptr*/) 
	: app_dialog_base(dialog_sub_grid::IDD, parent)
{
	m_excel = nullptr;
	m_db = nullptr;
	m_default_left_pos = 0;
	m_default_top_pos = 0;
	m_default_height = 100;
	m_default_width = 100;
}

dialog_sub_grid::dialog_sub_grid(const CString& template_name, int default_height, int default_width,
	const CString& title /*= _T("")*/, CWnd* parent /*= nullptr*/)
	: app_dialog_base(dialog_sub_grid::IDD, parent), m_template_name(template_name),
	m_default_height(default_height), m_default_width(default_width), m_title(title)
{
	m_excel = nullptr;
	m_db = nullptr;
	m_default_left_pos = 0;
	m_default_top_pos = 0;
	if (m_title.IsEmpty()) m_title = m_template_name;	 
}

dialog_sub_grid::~dialog_sub_grid()
{
	safe_delete(m_excel);
	safe_delete(m_db);
}

void dialog_sub_grid::get_data_from_ui()
{

}

void dialog_sub_grid::set_data_to_ui()
{

}

BOOL dialog_sub_grid::OnInitDialog()
{
	app_dialog_base::OnInitDialog();

	// 自定义窗口初始位置和大小
	CRect wk_rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, (PVOID)&wk_rect, NULL);

	CRect custom_rect;
	custom_rect.left = m_default_left_pos;
	custom_rect.top = m_default_top_pos;
	custom_rect.bottom = m_default_top_pos + max(m_default_height, 100);
	custom_rect.right = m_default_left_pos + max(m_default_width, 100);

	if (custom_rect.left <= 0)
		custom_rect.MoveToX(max(0, (wk_rect.Width() - custom_rect.Width()) / 2));

	if (custom_rect.top <= 0)
		custom_rect.MoveToY(max(0, (wk_rect.Height() - custom_rect.Height()) / 2));

	MoveWindow(&custom_rect);


	m_excel = new CTHExcel();
	m_excel->CreateFromStatic(this, IDC_STATIC_SUB_GRID, TRUE);

	SetWindowText(m_title);
	set_grid_data_base();
	if (m_db) m_excel->SetUsingDataBase(m_db);

	set_data_to_ui();
	m_excel->ReFresh(TRUE);
	adjust_window_layout();

	return TRUE;
}

void dialog_sub_grid::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_button_ok);
	DDX_Control(pDX, IDCANCEL, m_button_cancel);
}

void dialog_sub_grid::adjust_window_layout()
{
	if (!GetSafeHwnd()) return;
	if (!m_button_ok.GetSafeHwnd()) return;
	if (!m_button_cancel.GetSafeHwnd()) return;

	CRect client_rect;
	GetClientRect(&client_rect);

	const int edge_width = 8;

	CRect cancel_rect;
	m_button_cancel.GetWindowRect(cancel_rect);
	cancel_rect.MoveToXY(client_rect.Width() - cancel_rect.Width() - edge_width,
		client_rect.Height() - cancel_rect.Height() - edge_width);
	m_button_cancel.MoveWindow(cancel_rect);

	CRect ok_rect;
	m_button_ok.GetWindowRect(ok_rect);
	ok_rect.MoveToXY(cancel_rect.left - edge_width - ok_rect.Width(), cancel_rect.top);
	m_button_ok.MoveWindow(ok_rect);

	if (m_excel) m_excel->MoveWindow(edge_width, edge_width,
		client_rect.Width() - edge_width * 2, cancel_rect.top - edge_width * 2);
}

void dialog_sub_grid::set_grid_data_base()
{
	m_db = table_templates::create_data_base(m_template_name);
	if (m_db) m_db->SetRowHeight(0, 0);
}

bool dialog_sub_grid::grid_remove_data_in_grid(NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_sub_grid::grid_add_data_in_grid(NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_sub_grid::grid_row_col_size_changed(NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_sub_grid::grid_cell_value_changed(NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_sub_grid::grid_cell_button_clicked(NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_sub_grid::grid_cell_focus_changed(NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

void dialog_sub_grid::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	adjust_window_layout();
}

void dialog_sub_grid::on_clicked_ok()
{
	if (m_excel) m_excel->Update();
	get_data_from_ui();
	CDialogEx::OnOK();
}

BOOL dialog_sub_grid::on_remove_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	UNUSED(id);
	return grid_remove_data_in_grid((NM_THGRID*)notify_struct, result) ? TRUE : FALSE;
}

BOOL dialog_sub_grid::on_add_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	UNUSED(id);
	return grid_add_data_in_grid((NM_THGRID*)notify_struct, result) ? TRUE : FALSE;
}

BOOL dialog_sub_grid::on_row_col_size_changed(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	UNUSED(id);
	return grid_row_col_size_changed((NM_THGRID*)notify_struct, result) ? TRUE : FALSE;
}

BOOL dialog_sub_grid::on_cell_value_changed(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	UNUSED(id);
	return grid_cell_value_changed((NM_THGRID*)notify_struct, result) ? TRUE : FALSE;
}

BOOL dialog_sub_grid::on_cell_button_clicked(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	UNUSED(id);
	return grid_cell_button_clicked((NM_THGRID*)notify_struct, result) ? TRUE : FALSE;
}

BOOL dialog_sub_grid::on_cell_focus_changed(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	UNUSED(id);
	return grid_cell_focus_changed((NM_THGRID*)notify_struct, result) ? TRUE : FALSE;
}


dialog_sub_grid_key_value_array::dialog_sub_grid_key_value_array(CWnd* parent) : dialog_sub_grid(parent)
{

}

dialog_sub_grid_key_value_array::dialog_sub_grid_key_value_array(const CString& title,
	int default_height, int default_width, const CString& key_title, const CString& value_title, 
	CWnd* parent /*= nullptr*/, int index_width /*= 50*/, int key_width /*= 70*/, int value_width /*= 70*/)
	: dialog_sub_grid(_T(""), default_height, default_width, title, parent)
{
	m_key_title = key_title;
	m_value_title = value_title;
}

dialog_sub_grid_key_value_array::~dialog_sub_grid_key_value_array()
{
}

void dialog_sub_grid_key_value_array::get_result_data_pair(std::vector<std::pair<CString, CString>>& data) const
{
	data = m_matrix_data;
}

void dialog_sub_grid_key_value_array::get_result_data_array(std::vector<std::array<CString, 2>>& data) const
{
	data.clear();
	for (auto& x : m_matrix_data)
		data.push_back(/*array<CString, 2>*/{ x.first, x.second });
}

void dialog_sub_grid_key_value_array::get_result_data_vector(std::vector<std::vector<CString>>& data) const
{
	data.clear();
	for (auto& x : m_matrix_data)
		data.push_back(/*array<CString, 2>*/{ x.first, x.second });
}

void dialog_sub_grid_key_value_array::get_result_data_string(CString& data) const
{
	data = m_text;
}

void dialog_sub_grid_key_value_array::set_result_data_pair(const std::vector<std::pair<CString, CString>>& data)
{
	m_text = _T("");
	m_matrix_data = data;
}

void dialog_sub_grid_key_value_array::set_result_data_array(const std::vector<std::array<CString, 2>>& data)
{
	m_text = _T("");
	m_matrix_data.clear();
	for (const auto& x : data)
		m_matrix_data.push_back(make_pair(x[0], x[1]));
}

void dialog_sub_grid_key_value_array::set_result_data_vector(const std::vector<std::vector<CString>>& data)
{
	m_text = _T("");
	m_matrix_data.clear();
	for (const auto& x : data)
		m_matrix_data.push_back(make_pair(x[0], x[1]));
}

void dialog_sub_grid_key_value_array::set_result_data_string(const CString& data,
	TCHAR row_split_char/* = _T(';')*/, TCHAR column_split_char/* = _T(',')*/)
{
	m_text = data;
	m_row_split_char = row_split_char;
	m_column_split_char = column_split_char;

	vector<CString> rows;
	string_tools::split_string(rows, m_text, CString(m_row_split_char));
	for (CString& col_text : rows)
	{
		vector<CString> columns;
		string_tools::split_string(columns, col_text, CString(m_column_split_char));
		columns.resize(2);
		if (columns[0].IsEmpty() && columns[1].IsEmpty()) continue;

		m_matrix_data.push_back(make_pair(columns[0], columns[1]));
	}
}

void dialog_sub_grid_key_value_array::get_data_from_ui()
{
	m_text = _T("");
	m_matrix_data.clear();

	if (!m_db) return;
	if (m_excel) m_excel->Update();

	CTHCell* cell = nullptr;
	for (int i = m_db->GetFixedRowCount(); i < m_db->GetRowCount(); ++i)
	{
		CString sub_first;
		cell = m_db->GetCell(i, 1);
		if (cell) sub_first = cell->GetText();

		CString sub_second;
		cell = m_db->GetCell(i, 2);
		if (cell) sub_second = cell->GetText();

		if (sub_first.IsEmpty() && sub_second.IsEmpty()) continue;

		m_matrix_data.push_back(make_pair(sub_first, sub_second));
		m_text += sub_first + m_column_split_char + sub_second + m_row_split_char;
	}

	// 删除最后一个行分隔符
	if (!m_text.IsEmpty()) m_text.Delete(m_text.GetLength() - 1);
}

void dialog_sub_grid_key_value_array::set_data_to_ui()
{
	if (!m_text.IsEmpty() && m_matrix_data.empty())
	{
		vector<CString> rows;
		string_tools::split_string(rows, m_text, CString(m_row_split_char));
		for (CString& col_text : rows)
		{
			vector<CString> columns;
			string_tools::split_string(columns, col_text, CString(m_column_split_char));
			columns.resize(2);
			if (columns[0].IsEmpty() && columns[1].IsEmpty()) continue;

			m_matrix_data.push_back(make_pair(columns[0], columns[1]));
		}
	}

	int template_row_count = m_db->GetRowCount() - m_db->GetFixedRowCount();
	m_db->RemoveRowsAt(2, template_row_count);

	int row_count = max((int) m_matrix_data.size(), 5);
	m_db->InsertRowPost(1, row_count);

	CTHCell* cell = nullptr;
	for (int i = 0; i < (int) m_matrix_data.size(); ++i)
	{
		//cell = m_db->GetCell(i + m_db->GetFixedRowCount(), 0);
		//cell->SetText(string_tools::num_to_string(i + 1));

		cell = m_db->GetCell(i + m_db->GetFixedRowCount(), 1);
		if (cell) cell->SetText(m_matrix_data[i].first);

		cell = m_db->GetCell(i + m_db->GetFixedRowCount(), 2);
		if (cell) cell->SetText(m_matrix_data[i].second);
	}

	if (m_excel) m_excel->ReFresh(TRUE);
}

void dialog_sub_grid_key_value_array::set_grid_data_base()
{
	safe_delete(m_db);
	m_db = new CTHDataBase();

	m_db->SetRowCount(2);
	m_db->SetColCount(3);

	CTHCell* cell = m_db->GetCell(0, 1);
	if (cell) set_cell_type(1, cell);

	cell = m_db->GetCell(0, 2);
	if (cell) set_cell_type(2, cell);


	cell = m_db->GetCell(0, 0);
	if (cell) cell->SetText(_T("编号"));

	m_db->SetFixedRowCount(1);
	m_db->SetFixedColCount(1);
	m_db->EnableColMenu(TRUE);
	m_db->EnableRowAutoSerialNumber(TRUE);

	m_db->SetAsRowGroup(0, 1, 0);
	m_db->AutoFillRowNo();
}

void dialog_sub_grid_key_value_array::set_cell_type(int col_index, ZYH_GRID::CTHCell* cell)
{
	if (!cell) return;

	if (1 == col_index)
	{
		cell->SetText(m_key_title);
		cell->SetCellType(CT_NORMAL);
		cell->SetReadOnly(FALSE);

		return;
	}

	if (2 == col_index)
	{
		cell->SetText(m_value_title);
		cell->SetCellType(CT_NORMAL);
		cell->SetReadOnly(FALSE);

		return;
	}
}
