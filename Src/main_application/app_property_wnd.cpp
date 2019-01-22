// DbPropertyWnd.cpp : implementation file

#include "stdafx.h"
#include "resource.h"

#include <utility>

#include "x_framework/macro.h"
#include "x_framework/interface/app_ui_interface.h"

#include "PropertyGrid/XTPPropertyGridView.h"

#include "app_property_wnd.h" 


#ifdef _DEBUG
#define new DEBUG_NEW
#endif 


using namespace std;


#define WM_PROPERTY_ID					501
#define WM_PROPERTY_DATA_RESET			WM_USER + 101
#define WM_PROPERTY_DATA_CLEAR			WM_USER + 102
#define WM_PROPERTY_DATA_IS_EMPTY		WM_USER + 103


BEGIN_MESSAGE_MAP(app_property_grid_control, CXTPPropertyGrid)
	ON_MESSAGE(WM_PROPERTY_DATA_RESET, on_msg_property_reset)
	ON_MESSAGE(WM_PROPERTY_DATA_CLEAR, on_msg_property_clear)
	ON_MESSAGE(WM_PROPERTY_DATA_IS_EMPTY, on_msg_property_is_empty)
END_MESSAGE_MAP()

app_property_grid_control::app_property_grid_control()
{
}

app_property_grid_control::~app_property_grid_control()
{
} 

LRESULT app_property_grid_control::on_msg_property_reset(WPARAM wparam, LPARAM lparam)
{
	property_notification_interface* obj = (property_notification_interface*) (wparam);
	if (!obj) return FALSE;

	ResetContent();
	obj->set_property_items(*this);

	// Invalidate();
	RedrawControl();

	// 内部使用XTP的ShowPane来实现，如果点关闭之后也能正常显示，不论窗口当前是隐藏还是关闭状态，经测试均能在显示时
	// 还原为原来的状态，如原来是浮动的，被关闭后再调用此接口显示时也会是在原来的位置浮动出来，这一点特性有非常好的
	// 用户界面体验！！！对XTP赞一个，不用自己处理那么多界面逻辑
	app_ui_interface* app = get_app_ui_interface();
	if (app) app->show_docking_pane_by_identify(_T("app_property_wnd"));

	return TRUE;
}

LRESULT app_property_grid_control::on_msg_property_clear(WPARAM wparam, LPARAM lparam)
{
	ResetContent();
	RedrawControl();
	return TRUE;
}

LRESULT app_property_grid_control::on_msg_property_is_empty(WPARAM wparam, LPARAM lparam)
{
	BOOL res = (GetCount() <= 0 ? TRUE : FALSE);
	return (LRESULT)res;
}


//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(app_property_wnd, CWnd, 1)
BEGIN_MESSAGE_MAP(app_property_wnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, on_cell_message)
END_MESSAGE_MAP()


app_property_wnd::app_property_wnd()
{
}

app_property_wnd::~app_property_wnd()
{
}

void app_property_wnd::app_theme_changed(theme_style_settings& settings)
{
	m_grid.GetPaintManager()->GetItemMetrics()->m_clrBack = settings.m_window_back_color;
	m_grid.RedrawControl();
}

void app_property_wnd::show_window(property_notification_interface* obj, bool set_focus /*= false*/)
{
	property_window_interface::show_window(obj, set_focus);

	if (!GetSafeHwnd() || !obj) return;

	app_property_grid_control& grid = get_grid_ctrl();
	if (!grid.GetSafeHwnd()) return;

	// 不要直接操作界面元素，以及此接口由非界面线程触发导致gui线程无法正常工作，改由主窗口的消息响应来完成以保证界面工作正常
	grid.SendMessage(WM_PROPERTY_DATA_RESET, (WPARAM)obj, NULL);
}

void app_property_wnd::clear_content()
{
	property_window_interface::clear_content();

	app_property_grid_control& grid = get_grid_ctrl();
	if (!grid.GetSafeHwnd()) return;

	// 不要直接操作界面元素，以及此接口由非界面线程触发导致gui线程无法正常工作，改由主窗口的消息响应来完成以保证界面工作正常
	grid.SendMessage(WM_PROPERTY_DATA_CLEAR, NULL, NULL);
}

bool app_property_wnd::is_content_empty()
{
	app_property_grid_control& grid = get_grid_ctrl();
	if (!grid.GetSafeHwnd()) return true;

	// 不要直接操作界面元素，以及此接口由非界面线程触发导致gui线程无法正常工作，改由主窗口的消息响应来完成以保证界面工作正常
	return TRUE == (BOOL)grid.SendMessage(WM_PROPERTY_DATA_IS_EMPTY, NULL, NULL);
}

BOOL app_property_wnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1) return FALSE;

	set_property_window(this);

	m_grid.Create(CRect(), this, WM_PROPERTY_ID);
	m_grid.SetVariableItemsHeight(TRUE);
	m_grid.EnableToolTips();
	m_grid.EnableTrackingToolTips();

	app_ui_interface* app = get_app_ui_interface();
	if (app) app_theme_changed(app->get_style_and_settings());

	return TRUE;
}

void app_property_wnd::OnDestroy()
{
	set_property_window(nullptr);
	CWnd::OnDestroy();
}

void app_property_wnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (m_grid.GetSafeHwnd()) m_grid.MoveWindow(0, 0, cx, cy);
}

LRESULT app_property_wnd::on_cell_message(WPARAM wParam, LPARAM lParam)
{
	UINT msg_type = (UINT) wParam;
	if (XTP_PGN_ITEMVALUE_CHANGED == msg_type)
	{
		if (!m_grid.GetSafeHwnd()) return FALSE;

		CXTPPropertyGridItem* item = (CXTPPropertyGridItem*)lParam;
		if (!item) return FALSE;

		// 目前只要属性一变就触发相应的应用动作，万一以后要支持批量应用，则接口已经准备好了，因此不要将接口改为只能应用一条属性
		std::vector<CXTPPropertyGridItem*> all_items(1, item);
		apply_propery_items_to_registered_obj(all_items);
		return TRUE;
	}

	return FALSE;
}

app_property_grid_control& app_property_wnd::get_grid_ctrl()
{
	return m_grid;
}
