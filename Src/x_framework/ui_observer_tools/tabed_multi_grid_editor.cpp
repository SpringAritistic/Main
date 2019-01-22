#include "stdafx.h"

#include "THGridControl/THGridControlMac.h"
#include "THGridControl/common.h"
#include "THGridControl/THDataBase.h"
#include "THGridControl/THExcel.h"
#include "THGridControl/THBaseExcel.h"

#include "../interface/app_ui_interface.h"
#include "../tools/table_templates.h"
#include "x_mvc_editor_base.h"

#include "tabed_multi_grid_editor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


#define ID_ACC_TAB_WINDOW				300
#define ID_ACC_MULTI_GRID_WINDOW		301


BEGIN_MESSAGE_MAP(tabed_multi_grid_control, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY_EX_RANGE(GN_CELLCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_value_changed)
	ON_NOTIFY_EX_RANGE(GN_BUTTONCLICKED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_button_clicked)
	ON_NOTIFY_EX_RANGE(GN_FOCUSCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_focus_changed)
END_MESSAGE_MAP()

tabed_multi_grid_control::tabed_multi_grid_control(tabed_multi_grid_editor* editor)
{
	m_editor = editor;
	m_grid_ui_observer = nullptr;
	m_bk_color = RGB(255, 255, 255);
}

tabed_multi_grid_control::~tabed_multi_grid_control()
{
	// 不需要清理界面资源，view会自己清理，只要清理非界面资源即可
}

bool tabed_multi_grid_control::load_data_to_ui(tab_multi_grid_observer* observer)
{
	static int grid_tab_page_start_id = 1000;
	static int grid_tab_page_end_id = 2000;
	clear_ui_data();

	int grid_tab_page_current_id = grid_tab_page_start_id;
	m_grid_ui_observer = observer;

	if (!observer) return false;

	typedef pair<CString, CTHDataBase*> table_inst;
	typedef pair<CString, vector<table_inst>> tab_tables;
	typedef pair<CString, vector<CString>> tab_table_names;

	vector<tab_table_names> observer_table_names;
	observer->get_table_template_names(observer_table_names);

	vector<tab_tables> all_table_instance;
	for (tab_table_names& x : observer_table_names)
	{
		if (x.first.IsEmpty() || x.second.empty()) continue;

		vector<table_inst> tables_in_one_tab;
		for (auto& table_name : x.second)
			tables_in_one_tab.push_back(make_pair(table_name, table_templates::create_data_base(table_name)));

		all_table_instance.insert(all_table_instance.end(), make_pair(x.first, tables_in_one_tab));
	}

	vector<tab_tables> observer_tables;
	observer->get_table_instances(observer_tables);
	all_table_instance.insert(all_table_instance.end(), observer_tables.begin(), observer_tables.end());

	std::sort(all_table_instance.begin(), all_table_instance.end(), [&](tab_tables& ft, tab_tables& bk)
	{
		return !observer->is_tab_name_at_front(ft.first, bk.first);
	});

	for (tab_tables& x : all_table_instance)
	{
		grid_tab_page_current_id++;
		if (grid_tab_page_current_id > grid_tab_page_end_id)
			grid_tab_page_current_id = grid_tab_page_start_id;

		// 创建表格窗口对象
		multi_grid_view* grid_page = new multi_grid_view();

		// 构件界面元素
		grid_page->create_direct(&m_grid_tabs, grid_tab_page_current_id, CRect(), FALSE);
		grid_page->enable_auto_resize_excel(TRUE);
		grid_page->assign_all_excels(x.second);

		// -1表示总是添加到最后面
		m_grid_tabs.InsertItem(-1, x.first, grid_page->GetSafeHwnd());

		// 登记到缓存中
		map<CString, CTHDataBase*> current_tab_dbs;
		grid_page->get_all_data_base(current_tab_dbs);

		m_all_db_grids.insert(current_tab_dbs.begin(), current_tab_dbs.end());
		m_created_multi_grid.push_back(grid_page);
	}
	if (m_grid_tabs.GetItemCount() > 0) m_grid_tabs.SetCurSel(0);

	m_grid_ui_observer->load_data_to_ui(m_all_db_grids);

	for (auto& x : m_all_db_grids)
	{
		if (!x.second) continue;

		CTHBaseExcel* excel = x.second->GetExcel();
		if (excel) excel->ReFresh(TRUE); // after setting observer to db, calling this method to redraw ui
	}

	for (auto& x : m_created_multi_grid)
		if (x) x->resize_excels();

	make_all_children_window_back_color_same_as_this();

	return true;
}

bool tabed_multi_grid_control::save_data_from_ui()
{	
	if (!m_grid_ui_observer) return false;

	bool any_db_saved = false;
	for (auto& x : m_all_db_grids)
	{
		if (!x.second) continue;
		if (FALSE == x.second->IsUserModifyData()) continue;

		any_db_saved = true;
		CTHBaseExcel* excel = x.second->GetExcel();
		if (excel) excel->Update(); // 取数据之前必须调这个成员，否则处于编辑状态的数据可能无法回写
	}

	m_grid_ui_observer->save_data_from_ui(m_all_db_grids);
	
	// 把表格的修改标志置为未修改
	for (auto& x : m_all_db_grids)
		x.second->SetUserModify(FALSE);

	return any_db_saved;
}

void tabed_multi_grid_control::set_window_backcolor(COLORREF bk_color)
{
	m_bk_color = bk_color;
	make_all_children_window_back_color_same_as_this();
}

void tabed_multi_grid_control::add_mode_navigate_button(CXTPTabManagerNavigateButton* button)
{
	if (!button) return;

	CXTPTabManagerNavigateButtons* tab_buttons = m_grid_tabs.GetNavigateButtons();
	if (tab_buttons)
	{
		// 对象由tab控件接管，不用delete
		tab_buttons->Add(button);
	}
}

multi_grid_tab_control* tabed_multi_grid_control::get_grid_tab_control()
{
	return &m_grid_tabs;
}

bool tabed_multi_grid_control::any_databases_be_modified() const
{
	if (m_all_db_grids.empty()) return false;

	for (auto& x : m_all_db_grids)
	{
		if (x.second->IsUserModifyData())
			return true;
	}
	return false;
}

void tabed_multi_grid_control::clear_ui_data()
{
	// 不能清理，首先view自己会先清理子窗口，然后delete自己且没有置为nullptr，此处已经不需要清理了，如果清理反倒
	// 因为未置为nullptr的悬挂指针导致崩溃
	//for (auto x : m_all_tab_page)
	//{
	//	if (x) x->DestroyWindow();
	//	safe_delete(x);
	//}

	m_grid_tabs.DeleteAllItems();
	m_all_tab_page.clear();
	m_all_db_grids.clear();

	for (auto& x : m_created_multi_grid)
		if (x) safe_delete(x);

	m_created_multi_grid.clear();
}

void tabed_multi_grid_control::make_all_children_window_back_color_same_as_this()
{
	for (multi_grid_view* wnd : m_created_multi_grid)
		if (wnd && wnd->GetSafeHwnd()) wnd->set_back_color(m_bk_color);
}

int tabed_multi_grid_control::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER;
	//lpCreateStruct->style &= ~WS_BORDER;
	if (-1 == CWnd::OnCreate(lpCreateStruct)) return -1;

	DWORD wnd_style = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP;
	m_grid_tabs.Create(wnd_style, CRect(), this, ID_ACC_TAB_WINDOW);
	// m_grid_tabs.GetPaintManager()->m_nMinTabWidth = 110;
	// m_grid_tabs.GetPaintManager()->m_nMaxTabWidth = 110;
	// m_grid_tabs.GetPaintManager()->m_nFixedTabWidth = 110;
	// m_grid_tabs.GetPaintManager()->m_bVerticalTextTopToBottom = FALSE;
	// m_grid_tabs.GetPaintManager()->SetLayout(xtpTabLayoutRotated);
	// m_grid_tabs.GetPaintManager()->m_rcButtonMargin = CRect(2, 2, 2, 2);
	m_grid_tabs.GetPaintManager()->m_bBoldSelected = TRUE;
	m_grid_tabs.GetPaintManager()->m_clientFrame = xtpTabFrameSingleLine;
	m_grid_tabs.GetPaintManager()->SetPosition(xtpTabPositionBottom);
	m_grid_tabs.GetPaintManager()->SetAppearance(xtpTabAppearanceVisualStudio2010);
	m_grid_tabs.GetPaintManager()->SetColor(xtpTabColorDefault);

	return 0;

}

void tabed_multi_grid_control::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);
	m_grid_tabs.MoveWindow(0, 0, cx, cy);
}

BOOL tabed_multi_grid_control::on_cell_value_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*)notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_grid_tabs.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	multi_grid_view* cur_view = (cur_wnd ? dynamic_cast<multi_grid_view*>(cur_wnd) : nullptr);
	if (!cur_view) return FALSE;

	CTHExcel* excel = cur_view->find_excel(id);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	if (m_grid_ui_observer) m_grid_ui_observer->cell_value_changed(db_clicked, nm_msg);
	return TRUE;
}

BOOL tabed_multi_grid_control::on_cell_button_clicked(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*)notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_grid_tabs.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	multi_grid_view* cur_view = (cur_wnd ? dynamic_cast<multi_grid_view*>(cur_wnd) : nullptr);
	if (!cur_view) return FALSE;

	CTHExcel* excel = cur_view->find_excel(id);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	if (m_grid_ui_observer) m_grid_ui_observer->cell_button_be_clicked(db_clicked, nm_msg);
	return TRUE;
}

BOOL tabed_multi_grid_control::on_cell_focus_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*)notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_grid_tabs.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	multi_grid_view* cur_view = (cur_wnd ? dynamic_cast<multi_grid_view*>(cur_wnd) : nullptr);
	if (!cur_view) return FALSE;

	CTHExcel* excel = cur_view->find_excel(id);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	if (m_grid_ui_observer) m_grid_ui_observer->cell_focus_changed(db_clicked, nm_msg);
	return TRUE;
}

IMPLEMENT_SERIAL(tabed_multi_grid_editor, x_mvc_editor_base, 1)
BEGIN_MESSAGE_MAP(tabed_multi_grid_editor, x_mvc_editor_base)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

tabed_multi_grid_editor::tabed_multi_grid_editor()
{
	m_tabbed_grid_wnd = nullptr;
}

tabed_multi_grid_editor::~tabed_multi_grid_editor()
{
	safe_delete(m_tabbed_grid_wnd);
}

bool tabed_multi_grid_editor::load_data_to_ui(const CString& data_tag)
{
	set_data_node_tag(data_tag);
	if (m_tabbed_grid_wnd)
		m_tabbed_grid_wnd->load_data_to_ui(dynamic_cast<tab_multi_grid_observer*>(get_ui_observer()));
	
	return true;
}

bool tabed_multi_grid_editor::save_data_from_ui()
{
	if (m_tabbed_grid_wnd && m_tabbed_grid_wnd->any_databases_be_modified())
		return m_tabbed_grid_wnd->save_data_from_ui();
	return false;
}

CString tabed_multi_grid_editor::get_title()
{
	return x_mvc_editor_base::get_title();
}

void tabed_multi_grid_editor::app_theme_changed(theme_style_settings& settings)
{
	if (m_tabbed_grid_wnd)
		m_tabbed_grid_wnd->set_window_backcolor(settings.m_window_back_color);
}

void tabed_multi_grid_editor::on_project_data_notify(const CString& node_tag, notifer_action ac)
{
	x_observer_base* obs = get_ui_observer();
	if (obs) obs->project_data_notify(node_tag, ac);
}

bool tabed_multi_grid_editor::need_save_ui_data()
{
	if (!m_tabbed_grid_wnd) return false;

	return m_tabbed_grid_wnd->any_databases_be_modified();
}

int tabed_multi_grid_editor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
		lpCreateStruct->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		lpCreateStruct->style &= ~WS_BORDER;
		if (-1 == x_mvc_editor_base::OnCreate(lpCreateStruct)) return -1;

		DWORD wnd_style = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE /*| WS_CLIPSIBLINGS | WS_BORDER*/;
		m_tabbed_grid_wnd = new tabed_multi_grid_control(this);
		m_tabbed_grid_wnd->Create(nullptr, nullptr, wnd_style, CRect(), this, ID_ACC_MULTI_GRID_WINDOW);

		// 窗口创建时手工设置起始状态的主题
		app_ui_interface* app = get_app_ui_interface();
		if (app)
		{
			theme_style_settings app_theme = app->get_style_and_settings();
			app_theme_changed(app_theme);
		}

		return 0;
}

void tabed_multi_grid_editor::OnSize(UINT type, int cx, int cy)
{
	x_mvc_editor_base::OnSize(type, cx, cy);

	if (m_tabbed_grid_wnd && m_tabbed_grid_wnd->GetSafeHwnd())
		m_tabbed_grid_wnd->MoveWindow(0, 0, cx, cy);
}

