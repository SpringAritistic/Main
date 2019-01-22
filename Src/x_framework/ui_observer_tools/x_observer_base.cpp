#include "stdafx.h"
#include "x_mvc_editor_base.h"

#include "x_observer_base.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;



IMPLEMENT_SERIAL(x_observer_base, CObject,1)

x_observer_base::x_observer_base(x_mvc_editor_base* wnd, const CString& node_tag)
{
	m_data_node_tag = node_tag;
	m_owner_window = wnd;
}

x_observer_base::x_observer_base(x_observer_base&)
{

}

x_observer_base::x_observer_base(x_observer_base&&)
{

}

void x_observer_base::operator=(const x_observer_base&&)
{
}

void x_observer_base::operator=(const x_observer_base&)
{
}

x_observer_base::~x_observer_base()
{
	m_owner_window = nullptr;
}

void x_observer_base::project_data_notify(const CString& node_tag, notifer_action ac)
{
}

CString x_observer_base::get_title()
{
	data_node_base* dn = data_center::get_data_node(get_data_node_tag());
	return (dn ? dn->get_node_name() : _T(""));
}

const CString& x_observer_base::get_data_node_tag() const
{
	return m_data_node_tag;
}

x_mvc_editor_base* x_observer_base::get_owner_window()
{
	return m_owner_window;
}

void x_observer_base::update_observer_ui()
{
	// 取一下巧，multi_grid_view的重绘会调用excel的resize_excel，这里简单触发重绘即可，不用直接调用表格处理
	CWnd* wnd = dynamic_cast<CWnd*>(get_owner_window());
	if (wnd && wnd->GetSafeHwnd()) wnd->Invalidate();

	//for (auto& x : m_all_db_grids)
	//{
	//	if (!x.second) continue;

	//	CTHBaseExcel* excel = x.second->GetExcel();
	//	if (excel) excel->ReFresh(TRUE); // after setting data to db, calling this method to redraw ui
	//}

	//for (auto& x : m_created_multi_grid)
	//	if (x) x->resize_excels();
}

void x_observer_base::update_window_title()
{
	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		int index = app->get_working_space_view_index(get_owner_window());
		if (index < 0) return;

		app->update_working_space_tab_caption(index);
	}
}
