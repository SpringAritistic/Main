#include "stdafx.h"
#include "THGridControl/THGridControlMac.h"
#include "THGridControl/THBaseExcel.h"
#include "THGridControl/THDataBase.h"

#include "../macro.h"
#include "../tools/table_templates.h"

#include "tab_single_gird_editor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define ID_TAB_WINDOW				300
#define ID_SINGLE_GRID_WINDOW		301

using namespace std;
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(tabed_single_grid_control, CWnd, 1)
BEGIN_MESSAGE_MAP(tabed_single_grid_control,CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY_EX_RANGE(GN_CELLCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_value_changed)
	ON_NOTIFY_EX_RANGE(GN_BUTTONCLICKED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_button_clicked)
	ON_NOTIFY_EX_RANGE(GN_FOCUSCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_focus_changed)
END_MESSAGE_MAP()
tabed_single_grid_control::tabed_single_grid_control(tab_single_gird_editor* editor)
{
	m_editor = editor;
	m_grid_ui_observer = nullptr;
	m_bk_color = RGB(255, 255, 255);
}


tabed_single_grid_control::~tabed_single_grid_control()
{
	
}

bool tabed_single_grid_control::load_data_to_ui(tab_single_grid_observer* data)
{
	static int grid_tab_page_start_id = 1000;
	static int grid_tab_page_end_id = 2000;
	clear_ui_data();

	int grid_tab_page_current_id = grid_tab_page_start_id;
	m_grid_ui_observer = data;

	if (!data) return false;

	std::vector<std::pair<CString, CString>> all_tables;
	data->get_table_template_names(all_tables);

	for (auto&x : all_tables)
	{
		grid_tab_page_current_id++;
		if (grid_tab_page_current_id > grid_tab_page_end_id)
			grid_tab_page_current_id = grid_tab_page_start_id;

		// 创建表格窗口对象
		CTHExcel* grid_page = new CTHExcel();	
		if (!grid_page) continue;
		grid_page->CreateDirect(&m_grid_tabs, grid_tab_page_current_id,CRect());

		CTHDataBase* cur_database =  table_templates::create_data_base(x.second);
		if (!cur_database) continue;
		grid_page->SetUsingDataBase(cur_database);


		// -1表示总是添加到最后面
		m_grid_tabs.InsertItem(-1, x.first, grid_page->GetSafeHwnd());

		// 登记到缓存中
		map<CString, CTHDataBase*> current_tab_dbs;
		current_tab_dbs.insert(std::make_pair(x.first, grid_page->GetDataBase()));

		m_all_db_grids.insert(current_tab_dbs.begin(), current_tab_dbs.end());
	}
	if (m_grid_tabs.GetItemCount() > 0) m_grid_tabs.SetCurSel(0);

	m_grid_ui_observer->load_data_to_ui(m_all_db_grids);

	for (auto& x : m_all_db_grids)
	{
		if (!x.second) continue;

		CTHBaseExcel* excel = x.second->GetExcel();
		if (excel) excel->ReFresh(TRUE); // after setting data to db, calling this method to redraw ui
	}

	make_all_children_window_back_color_same_as_this();

	return true;
}

bool tabed_single_grid_control::save_data_from_ui()
{
	if (!m_grid_ui_observer) return false;

	for (auto& x : m_all_db_grids)
	{
		if (!x.second) continue;

		CTHBaseExcel* excel = x.second->GetExcel();
		if (excel) excel->Update(); // 取数据之前必须调这个成员，否则处于编辑状态的数据可能无法回写
	}

	m_grid_ui_observer->save_data_from_ui(m_all_db_grids);
	return true;

}

void tabed_single_grid_control::set_window_backcolor(COLORREF bk_color)
{
	m_bk_color = bk_color;
	make_all_children_window_back_color_same_as_this();
}

void tabed_single_grid_control::add_mode_navigate_button(CXTPTabManagerNavigateButton* button)
{
	if (!button) return;

	CXTPTabManagerNavigateButtons* tab_buttons = m_grid_tabs.GetNavigateButtons();
	if (tab_buttons)
	{
		// 对象由tab控件接管，不用delete
		tab_buttons->Add(button);
	}
}

multi_grid_tab_control* tabed_single_grid_control::get_grid_tab_control()
{
	return &m_grid_tabs;
}

bool tabed_single_grid_control::all_databases_be_modified() const
{
	if (m_all_db_grids.empty()) return false;

	for (auto& x : m_all_db_grids)
	{
		if (x.second->IsUserModifyData())
			return true;
	}
	return false;

}

void tabed_single_grid_control::clear_ui_data()
{
	m_grid_tabs.DeleteAllItems();
	m_all_db_grids.clear();

	for (auto& x : m_created_multi_grid)
		if (x) safe_delete(x);

	m_created_multi_grid.clear();
}

void tabed_single_grid_control::make_all_children_window_back_color_same_as_this()
{
	for (CTHExcel* wnd : m_created_multi_grid)
		if (wnd && wnd->GetSafeHwnd()) {/*nothing*/} ;
}

int tabed_single_grid_control::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER;
	if (-1 == CWnd::OnCreate(lpCreateStruct)) return -1;

	DWORD wnd_style = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP;
	m_grid_tabs.Create(wnd_style, CRect(), this, ID_TAB_WINDOW);

	m_grid_tabs.GetPaintManager()->m_bBoldSelected = TRUE;
	m_grid_tabs.GetPaintManager()->m_clientFrame = xtpTabFrameSingleLine;
	m_grid_tabs.GetPaintManager()->SetPosition(xtpTabPositionBottom);
	m_grid_tabs.GetPaintManager()->SetAppearance(xtpTabAppearanceVisualStudio2010);
	m_grid_tabs.GetPaintManager()->SetColor(xtpTabColorDefault);

	return 0;
}

void tabed_single_grid_control::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type,cx,cy);
	m_grid_tabs.MoveWindow(0,0,cx,cy);
}

BOOL tabed_single_grid_control::on_cell_value_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*)notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_grid_tabs.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	CTHExcel* excel = (cur_wnd ? dynamic_cast<CTHExcel*>(cur_wnd) : nullptr);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	if (m_grid_ui_observer) m_grid_ui_observer->cell_value_changed(db_clicked, nm_msg);
	return TRUE;
}

BOOL tabed_single_grid_control::on_cell_button_clicked(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*)notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_grid_tabs.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	CTHExcel* excel = (cur_wnd ? dynamic_cast<CTHExcel*>(cur_wnd) : nullptr);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	if (m_grid_ui_observer) m_grid_ui_observer->cell_button_be_clicked(db_clicked, nm_msg);
	return TRUE;
}

BOOL tabed_single_grid_control::on_cell_focus_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*)notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_grid_tabs.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	CTHExcel* excel = (cur_wnd ? dynamic_cast<CTHExcel*>(cur_wnd) : nullptr);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	if (m_grid_ui_observer) m_grid_ui_observer->cell_focus_changed(db_clicked, nm_msg);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(tab_single_gird_editor,x_mvc_editor_base,1)
BEGIN_MESSAGE_MAP(tab_single_gird_editor, x_mvc_editor_base)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

tab_single_gird_editor::tab_single_gird_editor()
{
	m_tab_single_grid_control = nullptr;
}


tab_single_gird_editor::~tab_single_gird_editor()
{
	m_tab_single_grid_control = nullptr;
}

bool tab_single_gird_editor::load_data_to_ui(const CString& data_tag)
{
	set_data_node_tag(data_tag);
	if (m_tab_single_grid_control)
		m_tab_single_grid_control->load_data_to_ui(dynamic_cast<tab_single_grid_observer*>(get_ui_observer()));
	return true;
}

bool tab_single_gird_editor::save_data_from_ui()
{
	if (m_tab_single_grid_control)
		m_tab_single_grid_control->save_data_from_ui();
	return true;
}

void tab_single_gird_editor::on_project_data_notify(const CString& node_tag, notifer_action ac)
{
	x_observer_base* obs = get_ui_observer();
	if (obs) obs->project_data_notify(node_tag,ac);
}

void tab_single_gird_editor::app_theme_changed(theme_style_settings& settings)
{
	if (m_tab_single_grid_control)
		m_tab_single_grid_control->set_window_backcolor(settings.m_window_back_color);

}

int tab_single_gird_editor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	lpCreateStruct->style &= ~WS_BORDER;
	if (-1 == x_mvc_editor_base::OnCreate(lpCreateStruct)) return -1;

	DWORD wnd_style = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE /*| WS_CLIPSIBLINGS | WS_BORDER*/;
	m_tab_single_grid_control = new tabed_single_grid_control(this);
	m_tab_single_grid_control->Create(nullptr, nullptr, wnd_style, CRect(), this, ID_SINGLE_GRID_WINDOW);

	// 窗口创建时手工设置起始状态的主题
	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		theme_style_settings app_theme = app->get_style_and_settings();
		app_theme_changed(app_theme);
	}
	return 0;
}

void tab_single_gird_editor::OnSize(UINT type, int cx, int cy)
{
	x_mvc_editor_base::OnSize(type, cx, cy);

	if (m_tab_single_grid_control && m_tab_single_grid_control->GetSafeHwnd())
		m_tab_single_grid_control->MoveWindow(0, 0, cx, cy);
}

X_FRAMEWORK_API void active_tab_single_gird_editor(const CString& data_node_tag, const CString& ui_observer_class_name)
{
	active_workspace_mvc_editor(data_node_tag,ui_observer_class_name,_T("tab_single_gird_editor"));
}

