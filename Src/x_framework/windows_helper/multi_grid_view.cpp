#include "StdAfx.h"
#include "../resource.h"

#include "THGridControl/THGridControlMac.h"
#include "THGridControl/THExcel.h"
#include "THGridControl/THDataBase.h"

#include "../macro.h"
#include "../tools/table_templates.h"

#include "multi_grid_view.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


//////////////////////////////////////////////////////////////////////////
// first为excel的id，second为是否被用了，如果指定id不存在，也当作未被使用
static map<unsigned int, bool> all_excel_id_in_app;
//////////////////////////////////////////////////////////////////////////


// 任何一个多表视图中的表格都用此函数分配窗口id
unsigned int make_excel_id_in_multi_grid_view()
{
	// 记住最后一次分配的id,下一次分配的时候从这个值开始检测，可以提高效率，避免每次都从开始id检测
	static unsigned int current_id = ID_MULTI_GRID_START;
	
	unsigned int tmp_id = current_id + 1;
	if (tmp_id > ID_MULTI_GRID_END) tmp_id = ID_MULTI_GRID_START;

	while (true)
	{
		bool is_used = false;
		auto it = all_excel_id_in_app.find(tmp_id);
		if (it != all_excel_id_in_app.end())
			is_used = it->second;
		else
			is_used = false;

		if (!is_used)
		{
			current_id = tmp_id;
			return current_id;
		}
		
		++tmp_id;
		if (tmp_id > ID_MULTI_GRID_END) tmp_id = ID_MULTI_GRID_START;
		
		// 检测是否全部的id都检测了一遍
		// 尽量返回一个范围内的值，至少同一个界面上没有两个平铺开窗口id都相同的时候能正常响应
		if (tmp_id == current_id) return ID_MULTI_GRID_START;
	}

	// 尽量返回一个范围内的值，至少同一个界面上没有两个平铺开窗口id都相同的时候能正常响应
	return ID_MULTI_GRID_START;
}

// 多表视图中的表格控制销毁后及时回收窗口id以便再利用
void recycle_excel_id_in_multi_grid_view(unsigned int id)
{
	all_excel_id_in_app[id] = false;
}


IMPLEMENT_SERIAL(multi_grid_view, CFormView, 1)
BEGIN_MESSAGE_MAP(multi_grid_view, CFormView)
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_EX_RANGE(GN_CELLCHANGE_POST, ID_MULTI_GRID_START ,ID_MULTI_GRID_END, on_cell_value_changed) 
	ON_NOTIFY_EX_RANGE(GN_REMOVEROWS_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_remove_data_in_grid) //表格删记录
	ON_NOTIFY_EX_RANGE(GN_INSERTROWS_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_add_data_in_grid) //表格增记录
	ON_NOTIFY_EX_RANGE(GN_COLWIDTH_ED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_row_col_size_changed) //列宽改变
	ON_NOTIFY_EX_RANGE(GN_ROWHEIGHT_ED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_row_col_size_changed) //行高改变
	ON_NOTIFY_EX_RANGE(GN_BUTTONCLICKED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_button_clicked) //单击表格中的按钮
	ON_NOTIFY_EX_RANGE(GN_FOCUSCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_focus_changed) //表格选中单元格改变
END_MESSAGE_MAP()


multi_grid_view::multi_grid_view(UINT nIDTemplate, bool need_delete_self_post_nc_destroy/* = true*/)
: CFormView( 0 == nIDTemplate ? IDD_AUTO_FORM : nIDTemplate )
, m_auto_resize_excel(true)
, m_total_width(0)
, m_total_height(0)
, m_back_color(RGB(255, 255, 255))
, m_is_initialize(false)
, m_need_delete_self_post_nc_destroy(need_delete_self_post_nc_destroy)
{
}

multi_grid_view::~multi_grid_view(void)
{
	clear_all_excels_and_data();
}

void multi_grid_view::add_excel(const CString& template_name)
{
	if (template_name.IsEmpty()) return;

	add_excel(template_name, table_templates::create_data_base(template_name));
}

void multi_grid_view::add_excel(const CString& index_name, CTHDataBase* instance)
{
	if (!instance) return;

	multi_grid_item_info info;
	info.m_excel = new CTHExcel();
	info.m_db = instance;
	info.m_wnd_id = make_excel_id_in_multi_grid_view();
	info.m_is_visible = true;
	info.m_db_name = index_name;

	info.m_excel->CreateDirect(this, info.m_wnd_id, CRect(0, 0, 100, 100), FALSE);
	info.m_excel->SetUsingDataBase(info.m_db);
	m_all_excels.push_back(info);
}

void multi_grid_view::clear_all_excels_and_data()
{
	// 清空界面窗口资源和对象
	for (multi_grid_item_info& x : m_all_excels)
	{
		recycle_excel_id_in_multi_grid_view(x.m_wnd_id); // 回收已经分配的窗口id并清空id缓存
		safe_delete(x.m_excel);
		safe_delete(x.m_db);
	}

	m_all_excels.clear();
}

void multi_grid_view::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	if (m_is_initialize) return;

	m_is_initialize = true;

	resize_excels();
}

void multi_grid_view::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	resize_excels();
}
int multi_grid_view::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return TRUE;
}

HRGN ValidateChildren(HWND hwnd, HRGN parent)
{
	HWND child = ::GetWindow(hwnd, GW_CHILD);
	if (!child) return parent;
	if (!::IsWindow(child)) return parent;

	while(child)
	{
		if (IsWindowVisible(child))
		{
			RECT rect;
			GetWindowRect(child, &rect);
			ScreenToClient(hwnd, (LPPOINT)&rect.left);
			ScreenToClient(hwnd, (LPPOINT)&rect.right);
			HRGN rgn = CreateRectRgnIndirect(&rect);
			CombineRgn(parent, parent, rgn, RGN_DIFF);
			DeleteObject(rgn);
		}

		child = GetWindow(child, GW_HWNDNEXT);
	}
	return parent;
}

BOOL multi_grid_view::OnEraseBkgnd(CDC* dc)
{
	// return CFormView::OnEraseBkgnd(dc);

	RECT clientRect;
	GetClientRect(&clientRect);

	// 为防止闪烁，将有效窗口的区域从重绘区域的去除掉
	HRGN rgn = CreateRectRgnIndirect(&clientRect);
	rgn = ValidateChildren(m_hWnd, rgn);
	SelectClipRgn(dc->m_hDC, rgn);
	DeleteObject(rgn);

	CBrush brush;
	brush.CreateSolidBrush( m_back_color);
	FillRect(dc->m_hDC, &clientRect, (HBRUSH)brush);
	brush.DeleteObject();

	return TRUE;
}

BOOL multi_grid_view::create_from_static( CWnd* parent, UINT wID, BOOL has_border/*=TRUE*/ )
{
	if (!parent || !parent->GetSafeHwnd()) return FALSE;

	CWnd* pWnd = parent->GetDlgItem(wID);
	_ASSERTE(("GetDlgItem为空", pWnd));
	CRect rect;
	pWnd->GetClientRect(rect);
	pWnd->MapWindowPoints(parent, rect);
	pWnd->DestroyWindow();
	BOOL result = create_direct(parent, wID, rect, has_border);
	SetWindowPos(&wndTop, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

	return result;
}

BOOL multi_grid_view::create_direct(CWnd* parent, UINT wID, const RECT& rect, BOOL has_border )
{
	DWORD style = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_BORDER | WS_TABSTOP;
	if (!has_border) style &= ~WS_BORDER;

	return CreateEx(0, NULL, NULL, style, rect, parent, wID, NULL);
}

void multi_grid_view::set_back_color( COLORREF color )
{
	m_back_color = color;
}

void multi_grid_view::assign_all_excels(const std::vector<CString>& template_names)
{
	vector<pair<CString, CTHDataBase*>> all_db;
	for (auto& x : template_names)
	{
		if (x.IsEmpty()) continue;

		CTHDataBase* db = table_templates::create_data_base(x);
		if (!db) continue;

		all_db.push_back(make_pair(x, db));
	}

	assign_all_excels(all_db);
}

void multi_grid_view::assign_all_excels(const std::vector<std::pair<CString, CTHDataBase*>>& template_names)
{
	clear_all_excels_and_data();

	for (const pair<CString, CTHDataBase*>& grid : template_names)
	{
		if (grid.first.IsEmpty() || !grid.second) continue;
		
		multi_grid_item_info info;
		info.m_excel = new CTHExcel();
		info.m_db = grid.second;
		info.m_wnd_id = make_excel_id_in_multi_grid_view();
		info.m_is_visible = true;
		info.m_db_name = grid.first;

		info.m_excel->CreateDirect(this, info.m_wnd_id, CRect(0, 0, 100, 100), FALSE);
		info.m_excel->SetUsingDataBase(info.m_db);
		m_all_excels.push_back(info);
	}

	resize_excels();
}

void multi_grid_view::resize_excels()
{
	if (!GetSafeHwnd()) return;

	LockWindowUpdate();

	if (m_auto_resize_excel)
	{
		const int left_edge = 5;
		const int top_edge = 5;
		const int spaces = 10; // 表之间隔开10个像

		int scroll_pos_v = 0;
		int scroll_pos_h = 0;
		{
			int scroll_min_pos = 0, scroll_max_pos = 0;
			GetScrollRange(SB_VERT, &scroll_min_pos, &scroll_max_pos);
			scroll_pos_v = GetScrollPos(SB_VERT);
			GetScrollRange(SB_HORZ, &scroll_min_pos, &scroll_max_pos);
			scroll_pos_h = GetScrollPos(SB_HORZ);
		}

		m_total_width = 0;
		m_total_height = 0;
		bool is_fist_visible_wnd_passed = false;
		for (multi_grid_item_info& x : m_all_excels)
		{
			if (!x.m_is_visible)
			{
				x.m_excel->ShowWindow(SW_HIDE);
				continue;
			}
			
			if (false == is_fist_visible_wnd_passed)
			{
				m_total_height += top_edge;
				is_fist_visible_wnd_passed = true;
			}
			else
			{
				m_total_height += spaces;
			}

			int current_width = x.m_excel->GetTotalWidth();
			int current_height = x.m_excel->GetTotalHeight();
			x.m_excel->MoveWindow(left_edge - scroll_pos_h, m_total_height - scroll_pos_v, current_width, current_height);
			x.m_excel->ShowWindow(TRUE);
			x.m_excel->Invalidate();

			m_total_height += current_height;
			m_total_width = max(current_width + left_edge * 2, m_total_width); // 取最宽的一个表宽度作为总宽（注意要考虑边距）
		}

		m_total_height += top_edge; // 再加一个顶边距作为底边距（如果一个表也没有，则只有一个底边距）
	}

	SetScrollSizes(MM_TEXT, CSize(m_total_width, m_total_height));

	UnlockWindowUpdate();
	Invalidate();
}

BOOL multi_grid_view::on_remove_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	resize_excels();
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}
BOOL multi_grid_view::on_add_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	resize_excels();
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}
BOOL multi_grid_view::on_row_col_size_changed(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	resize_excels();
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

BOOL multi_grid_view::on_cell_value_changed(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	resize_excels();
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

BOOL multi_grid_view::on_cell_button_clicked(UINT id, NMHDR *notify_struct, LRESULT *result)
{
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

BOOL multi_grid_view::on_cell_focus_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

CTHDataBase* multi_grid_view::find_data_base( const CString& data_base_name )
{
	for (multi_grid_item_info& x : m_all_excels)
		if (0 == data_base_name.CompareNoCase(x.m_db_name))
			return x.m_db;

	return nullptr;
}

void multi_grid_view::get_all_data_base(std::map<CString, CTHDataBase*>& dbs)
{
	dbs.clear();
	for (multi_grid_item_info& x : m_all_excels)
		dbs.insert(make_pair(x.m_db_name, x.m_db));
}

void multi_grid_view::enable_auto_resize_excel(bool auto_resize_excel)
{
	m_auto_resize_excel = auto_resize_excel;
}

CTHExcel* multi_grid_view::find_excel(const CString &data_base_name)
{
	for (multi_grid_item_info& x : m_all_excels)
		if (0 == data_base_name.CompareNoCase(x.m_db_name))
			return x.m_excel;

	return nullptr;
}

CTHDataBase* multi_grid_view::find_data_base(UINT id)
{
	for (multi_grid_item_info& x : m_all_excels)
		if (id == x.m_wnd_id)
			return x.m_db;

	return nullptr;
}

CTHExcel* multi_grid_view::find_excel(UINT id)
{
	for (multi_grid_item_info& x : m_all_excels)
		if (id == x.m_wnd_id)
			return x.m_excel;

	return nullptr;
}

void multi_grid_view::PostNcDestroy()
{
	if (m_need_delete_self_post_nc_destroy)
		CFormView::PostNcDestroy(); // 内部会delete this;
}

void multi_grid_view::set_excel_visible(const CString& data_base_name, bool show /*= false*/)
{
	bool has_changed = false;
	for (multi_grid_item_info& x : m_all_excels)
	{
		if (0 == data_base_name.CompareNoCase(x.m_db_name))
		{
			if (x.m_is_visible == show) continue;

			x.m_is_visible = show;
			has_changed = true;
			// break; 同名的全部都要处理
		}
	}

	if (has_changed) resize_excels();
}

void multi_grid_view::set_excel_visible(UINT id, bool show /*= false*/)
{
	bool has_changed = false;
	for (multi_grid_item_info& x : m_all_excels)
	{
		if (id == x.m_wnd_id)
		{
			if (x.m_is_visible == show) continue;

			x.m_is_visible = show;
			has_changed = true;
			// break; 同名的全部都要处理
		}
	}

	if (has_changed) resize_excels();
}

bool multi_grid_view::is_excel_visible(const CString& data_base_name) const
{
	for (const multi_grid_item_info& x : m_all_excels)
		if (0 == data_base_name.CompareNoCase(x.m_db_name))
			return x.m_is_visible;

	return false;
}

bool multi_grid_view::is_excel_visible(UINT id) const
{
	for (const multi_grid_item_info& x : m_all_excels)
		if (id == x.m_wnd_id)
			return x.m_is_visible;

	return false;
}



IMPLEMENT_DYNCREATE(multi_grid_tab_control, CXTPTabControl)
BEGIN_MESSAGE_MAP(multi_grid_tab_control, CXTPTabControl)
	ON_NOTIFY_EX_RANGE(GN_CELLCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_value_changed)
	ON_NOTIFY_EX_RANGE(GN_BUTTONCLICKED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_button_clicked)
	ON_NOTIFY_EX_RANGE(GN_FOCUSCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_focus_changed) //表格选中单元格改变
END_MESSAGE_MAP()

multi_grid_tab_control::multi_grid_tab_control()
{
}

multi_grid_tab_control::~multi_grid_tab_control()
{
}

int multi_grid_tab_control::get_tab_header_height() const
{
	if (!GetSafeHwnd()) return 0;

	// 计算tab控件中要显示部分的高度
	CRect tab_rect, tab_small;
	GetWindowRect(&tab_rect);
	tab_small = tab_rect;
	AdjustRect(FALSE, &tab_small);

	return tab_rect.Height() - tab_small.Height();
}

int multi_grid_tab_control::get_tab_border_width() const
{
	// 计算tab控件中要显示部分的高度
	CRect tab_rect, tab_small;
	GetWindowRect(&tab_rect);
	tab_small = tab_rect;
	AdjustRect(FALSE, &tab_small);

	int border_width = 0;
	XTPTabPosition tab_pos = GetPosition();
	if (xtpTabPositionTop == tab_pos)
		border_width = tab_rect.bottom - tab_small.bottom;
	else if (xtpTabPositionBottom == tab_pos)
		border_width = tab_rect.top - tab_small.top;
	else if (xtpTabPositionLeft == tab_pos)
		border_width = tab_rect.right - tab_small.right;
	else if (xtpTabPositionRight == tab_pos)
		border_width = tab_rect.left - tab_small.left;
	else
		border_width = 0;

	return border_width;
}

void multi_grid_tab_control::OnItemClick(CXTPTabManagerItem* pItem)
{
	CWnd* parent = GetParent();
	if (parent) parent->SendMessage(WM_TAB_ITEM_BE_CLICKED, (WPARAM) pItem);

	CXTPTabControl::OnItemClick(pItem);
}

BOOL multi_grid_tab_control::on_remove_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	if (!GetSafeHwnd()) return FALSE;
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

BOOL multi_grid_tab_control::on_add_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	if (!GetSafeHwnd()) return FALSE;
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

BOOL multi_grid_tab_control::on_row_col_size_changed(UINT id, NMHDR* notify_struct, LRESULT* result)
{
	if (!GetSafeHwnd()) return FALSE;
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

BOOL multi_grid_tab_control::on_cell_value_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!GetSafeHwnd()) return FALSE;
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

BOOL multi_grid_tab_control::on_cell_button_clicked(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!GetSafeHwnd()) return FALSE;
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

BOOL multi_grid_tab_control::on_cell_focus_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!GetSafeHwnd()) return FALSE;
	CWnd* parent = GetParent();
	if (!parent || !parent->GetSafeHwnd()) return FALSE;
	*result = parent->SendMessage(WM_NOTIFY, id, (LPARAM) notify_struct);
	return TRUE;
}

