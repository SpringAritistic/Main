#include "StdAfx.h"


#include "THGridControl/THGridControlMac.h"
#include "THGridControl/THExcel.h"
#include "THGridControl/THDataBase.h"

#include "..\macro.h"
#include "..\tools\table_templates.h"

#include "dialog_tabbed_grids.h"



#define		ID_TABED_GRID_TAB_CONTROL_ID		1001
#define		ID_TABED_GRID_GRID_PAGE_START_ID	1005
#define		ID_TABED_GRID_GRID_PAGE_END_ID		2005


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;



IMPLEMENT_SERIAL(dialog_tabbed_grids, app_dialog_base, 1)
BEGIN_MESSAGE_MAP(dialog_tabbed_grids, app_dialog_base)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDOK, on_button_click_ok)
	ON_NOTIFY_EX_RANGE(GN_CELLCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_value_changed)
	ON_NOTIFY_EX_RANGE(GN_REMOVEROWS_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_remove_data_in_grid) //表格删记录
	ON_NOTIFY_EX_RANGE(GN_INSERTROWS_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_add_data_in_grid) //表格增记录
	ON_NOTIFY_EX_RANGE(GN_COLWIDTH_ED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_row_col_size_changed) //列宽改变
	ON_NOTIFY_EX_RANGE(GN_ROWHEIGHT_ED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_row_col_size_changed) //行高改变
	ON_NOTIFY_EX_RANGE(GN_BUTTONCLICKED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_button_clicked) //单击表格中的按钮
	ON_NOTIFY_EX_RANGE(GN_FOCUSCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_focus_changed) //表格选中单元格改变
END_MESSAGE_MAP()

dialog_tabbed_grids::dialog_tabbed_grids(CWnd* parent /*= nullptr*/) 
	: app_dialog_base(dialog_tabbed_grids::IDD, parent)
{
	m_default_left_pos = 0;
	m_default_top_pos = 0;
	m_default_height = 100;
	m_default_width = 100;
}

dialog_tabbed_grids::~dialog_tabbed_grids()
{
	for (auto& x : m_all_grid_gages)
	{
		safe_delete(std::get<1>(x));
		safe_delete(std::get<2>(x));
	}
	m_all_grid_gages.clear();
}

void dialog_tabbed_grids::set_template_names(std::vector<std::pair<CString, CString>>& temp_names)
{
	temp_names.clear();
}

void dialog_tabbed_grids::get_data_from_ui(std::map<CString, CTHDataBase*>& all_grids)
{

}

void dialog_tabbed_grids::set_data_to_ui(std::map<CString, CTHDataBase*>& all_grids)
{

}

BOOL dialog_tabbed_grids::OnInitDialog()
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


	m_tab_control.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN /* | WS_BORDER*/, CRect(), this, ID_TABED_GRID_TAB_CONTROL_ID);
	// m_tab_control.GetPaintManager()->m_nMinTabWidth = 110;
	// m_tab_control.GetPaintManager()->m_nMaxTabWidth = 110;
	// m_tab_control.GetPaintManager()->m_nFixedTabWidth = 110;
	// m_tab_control.GetPaintManager()->m_bVerticalTextTopToBottom = FALSE;
	// m_tab_control.GetPaintManager()->SetLayout(xtpTabLayoutRotated);
	// m_tab_control.GetPaintManager()->m_rcButtonMargin = CRect(2, 2, 2, 2);
	m_tab_control.GetPaintManager()->m_bBoldSelected = TRUE;
	m_tab_control.GetPaintManager()->m_clientFrame = xtpTabFrameBorder;
	m_tab_control.GetPaintManager()->SetPosition(xtpTabPositionTop);
	m_tab_control.GetPaintManager()->SetAppearance(xtpTabAppearanceVisualStudio2010);
	m_tab_control.GetPaintManager()->SetColor(xtpTabColorDefault);

	std::vector<std::pair<CString, CString>> temp_names;
	set_template_names(temp_names);

	UINT current_page_id = ID_TABED_GRID_GRID_PAGE_START_ID;
	for (auto& x : temp_names)
	{
		CTHDataBase* db = table_templates::create_data_base(x.second);
		if (!db) continue;

		CTHExcel* excel = new CTHExcel();
		excel->CreateDirect(&m_tab_control, current_page_id++, CRect(), FALSE);
		excel->SetUsingDataBase(db);

		m_tab_control.InsertItem(-1, x.first, excel->GetSafeHwnd());
		m_all_grid_gages.push_back(make_tuple(x.second, excel, db));
	}
	if (m_tab_control.GetItemCount() > 0) m_tab_control.SetCurSel(0);

	std::map<CString, CTHDataBase*> all_grids;
	for (auto& x : m_all_grid_gages)
		all_grids[std::get<0>(x)] = std::get<2>(x);
	 
	set_data_to_ui(all_grids);
	for (auto& x : m_all_grid_gages)
	{
		CTHExcel* ex = std::get<1>(x);
		if (ex) ex->ReFresh(TRUE);
	}

	adjust_window_layout();
	return TRUE;
}

void dialog_tabbed_grids::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_button);
	DDX_Control(pDX, IDCANCEL, m_cancel);
}

void dialog_tabbed_grids::adjust_window_layout()
{
	if (!GetSafeHwnd()) return;
	if (!m_button.GetSafeHwnd()) return;
	if (!m_tab_control.GetSafeHwnd()) return;
	if (!m_cancel.GetSafeHwnd()) return;

	CRect client_rect;
	GetClientRect(&client_rect);

	const int edge_width = 8;

	CRect cancel_rect;
	m_cancel.GetWindowRect(cancel_rect);
	cancel_rect.MoveToXY(client_rect.Width() - cancel_rect.Width() - edge_width,
		client_rect.Height() - cancel_rect.Height() - edge_width);
	m_cancel.MoveWindow(cancel_rect);

	CRect ok_rect;
	m_button.GetWindowRect(ok_rect);
	ok_rect.MoveToXY(cancel_rect.left - ok_rect.Width() - edge_width, cancel_rect.top);
	m_button.MoveWindow(ok_rect);	

	m_tab_control.MoveWindow(edge_width, edge_width, client_rect.Width() - edge_width * 2, cancel_rect.top - edge_width * 2);
}

bool dialog_tabbed_grids::grid_remove_data_in_grid(CTHDataBase* db, NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(db);
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_tabbed_grids::grid_add_data_in_grid(CTHDataBase* db, NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(db);
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_tabbed_grids::grid_row_col_size_changed(CTHDataBase* db, NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(db);
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_tabbed_grids::grid_cell_value_changed(CTHDataBase* db, NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(db);
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_tabbed_grids::grid_cell_button_clicked(CTHDataBase* db, NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(db);
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

bool dialog_tabbed_grids::grid_cell_focus_changed(CTHDataBase* db, NM_THGRID* notify_struct, LRESULT* result)
{
	UNUSED(db);
	UNUSED(notify_struct);
	UNUSED(result);

	return false;
}

void dialog_tabbed_grids::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	adjust_window_layout();
}

void dialog_tabbed_grids::on_button_click_ok()
{
	std::map<CString, CTHDataBase*> all_grids;
	for (auto& x : m_all_grid_gages)
		all_grids[std::get<0>(x)] = std::get<2>(x);

	for (auto& x : m_all_grid_gages)
	{
		CTHExcel* ex = std::get<1>(x);
		if (ex) ex->Update();
	}

	get_data_from_ui(all_grids);

	CDialogEx::OnOK();
}

BOOL dialog_tabbed_grids::on_remove_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*) notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_tab_control.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	CTHExcel* excel = (cur_wnd ? dynamic_cast<CTHExcel*>(cur_wnd) : nullptr);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	return grid_remove_data_in_grid(db_clicked, nm_msg, result) ? TRUE : FALSE;
}

BOOL dialog_tabbed_grids::on_add_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*) notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_tab_control.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	CTHExcel* excel = (cur_wnd ? dynamic_cast<CTHExcel*>(cur_wnd) : nullptr);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	return grid_add_data_in_grid(db_clicked, nm_msg, result) ? TRUE : FALSE;
}

BOOL dialog_tabbed_grids::on_row_col_size_changed(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*) notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_tab_control.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	CTHExcel* excel = (cur_wnd ? dynamic_cast<CTHExcel*>(cur_wnd) : nullptr);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	return grid_row_col_size_changed(db_clicked, nm_msg, result) ? TRUE : FALSE;
}

BOOL dialog_tabbed_grids::on_cell_value_changed(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*) notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_tab_control.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	CTHExcel* excel = (cur_wnd ? dynamic_cast<CTHExcel*>(cur_wnd) : nullptr);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	return grid_cell_value_changed(db_clicked, nm_msg, result) ? TRUE : FALSE;
}

BOOL dialog_tabbed_grids::on_cell_button_clicked(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*) notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_tab_control.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	CTHExcel* excel = (cur_wnd ? dynamic_cast<CTHExcel*>(cur_wnd) : nullptr);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	return grid_cell_button_clicked(db_clicked, nm_msg, result) ? TRUE : FALSE;
}

BOOL dialog_tabbed_grids::on_cell_focus_changed(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*) notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_tab_control.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	CTHExcel* excel = (cur_wnd ? dynamic_cast<CTHExcel*>(cur_wnd) : nullptr);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	return grid_cell_focus_changed(db_clicked, nm_msg, result) ? TRUE : FALSE;
}

void dialog_tabbed_grids::OnSizing(UINT nType, LPRECT rect)
{
	adjust_window_layout();
}
