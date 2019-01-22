#include "stdafx.h"
#include "THGridControl/THGridControlMac.h"
#include "THGridControl/THBaseExcel.h"
#include "THGridControl/THDataBase.h"

#include "../macro.h"
#include "../tools/table_templates.h"

#include "no_tab_single_gird_editor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(no_tab_single_gird_editor,x_mvc_editor_base,1)
BEGIN_MESSAGE_MAP(no_tab_single_gird_editor, x_mvc_editor_base)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_EX_RANGE(GN_CELLCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_value_changed)
	ON_NOTIFY_EX_RANGE(GN_BUTTONCLICKED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_button_clicked)
	ON_NOTIFY_EX_RANGE(GN_FOCUSCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_focus_changed)
END_MESSAGE_MAP()

no_tab_single_gird_editor::no_tab_single_gird_editor()
{
	m_created_grid_view = nullptr;
}


no_tab_single_gird_editor::~no_tab_single_gird_editor()
{
	safe_delete(m_created_grid_view);
	m_created_grid_view = nullptr;
}

bool no_tab_single_gird_editor::load_data_to_ui(const CString& data_tag)
{
	set_data_node_tag(data_tag);

	x_observer_base* obs = get_ui_observer();
	if (!obs) return false;

	no_tab_single_grid_observer* cur_ob = dynamic_cast<no_tab_single_grid_observer* >(obs);
	if (!cur_ob) return false;

	std::pair<CString, CString> table;
	cur_ob->get_table_template_names(table);

	// 创建表格窗口对象
	CTHExcel* grid_page = new CTHExcel();
	if (!grid_page) return false;
	grid_page->CreateDirect(this, 111, CRect());

	CTHDataBase* cur_database = table_templates::create_data_base(table.second);
	if (!cur_database) return false;
	grid_page->SetUsingDataBase(cur_database);

	m_data_base.first = table.first;
	m_data_base.second = cur_database;

	cur_ob->load_data_to_ui(m_data_base);


	CTHBaseExcel* excel = cur_database->GetExcel();
	if (excel) excel->ReFresh(TRUE); // after setting data to db, calling this method to redraw ui
	return true;
}



bool no_tab_single_gird_editor::save_data_from_ui()
{
	x_observer_base* obs = get_ui_observer();
	if (!obs) return false;

	no_tab_single_grid_observer* cur_ob = dynamic_cast<no_tab_single_grid_observer*>(obs);
	if (!cur_ob) return false;

	cur_ob->save_data_from_ui(m_data_base);
	return true;
}

void no_tab_single_gird_editor::on_project_data_notify(const CString& node_tag, notifer_action ac)
{
	x_observer_base* obs = get_ui_observer();
	if (obs) obs->project_data_notify(node_tag,ac);
}


int no_tab_single_gird_editor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	lpCreateStruct->style &= ~WS_BORDER;
	if (-1 == x_mvc_editor_base::OnCreate(lpCreateStruct)) return -1;

	DWORD wnd_style = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE /*| WS_CLIPSIBLINGS | WS_BORDER*/;
	m_created_grid_view = new CTHExcel();
	m_created_grid_view->Create(nullptr, nullptr, wnd_style, CRect(), this, 110);

	//// 窗口创建时手工设置起始状态的主题
	//app_ui_interface* app = get_app_ui_interface();
	//if (app)
	//{
	//	theme_style_settings app_theme = app->get_style_and_settings();
	//	app_theme_changed(app_theme);
	//}
	return 0;
}

void no_tab_single_gird_editor::OnSize(UINT type, int cx, int cy)
{
	x_mvc_editor_base::OnSize(type, cx, cy);

	if (m_created_grid_view && m_created_grid_view->GetSafeHwnd())
		m_created_grid_view->MoveWindow(0, 0, cx, cy);
}

BOOL no_tab_single_gird_editor::on_cell_value_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*)notify_struct;

	if (!m_created_grid_view) return FALSE;
	CTHDataBase* db_clicked = m_created_grid_view->GetDataBase();
	if (!db_clicked) return FALSE;

	x_observer_base* obs = get_ui_observer();
	if (!obs) return false;

	no_tab_single_grid_observer* cur_ob = dynamic_cast<no_tab_single_grid_observer*>(obs);
	if (!cur_ob) return false;


	if (cur_ob) cur_ob->cell_value_changed(db_clicked, nm_msg);
	return TRUE;

}

BOOL no_tab_single_gird_editor::on_cell_button_clicked(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*)notify_struct;

	if (!m_created_grid_view) return FALSE;
	CTHDataBase* db_clicked = m_created_grid_view->GetDataBase();
	if (!db_clicked) return FALSE;

	x_observer_base* obs = get_ui_observer();
	if (!obs) return false;

	no_tab_single_grid_observer* cur_ob = dynamic_cast<no_tab_single_grid_observer*>(obs);
	if (!cur_ob) return false;

	if (cur_ob) cur_ob->cell_button_be_clicked(db_clicked, nm_msg);
	return TRUE;

}

BOOL no_tab_single_gird_editor::on_cell_focus_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*)notify_struct;

	if (!m_created_grid_view) return FALSE;
	CTHDataBase* db_clicked = m_created_grid_view->GetDataBase();
	if (!db_clicked) return FALSE;

	x_observer_base* obs = get_ui_observer();
	if (!obs) return false;

	no_tab_single_grid_observer* cur_ob = dynamic_cast<no_tab_single_grid_observer*>(obs);
	if (!cur_ob) return false;


	if (cur_ob) cur_ob->cell_focus_changed(db_clicked, nm_msg);
	return TRUE;

}

X_FRAMEWORK_API void active_no_tab_single_gird_editor(const CString& data_node_tag, const CString& ui_observer_class_name)
{
	active_workspace_mvc_editor(data_node_tag,ui_observer_class_name,_T("no_tab_single_gird_editor"));
}

