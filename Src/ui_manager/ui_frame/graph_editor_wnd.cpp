#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/windows_helper/multi_grid_view.h"
#include "x_framework/interface/app_ui_interface.h"

#include "project_manager/macro.h"
#include "project_manager/data_center/graph_data_base.h"

#include "THGridControl/THGridControlMac.h"
#include "THGridControl/common.h"
#include "THGridControl/THDataBase.h"
#include "THGridControl/THExcel.h"
#include "THGridControl/THBaseExcel.h"

#include "ACADLL/ImpExpMacro.h"
#include "ACADLL/GraphBase.h"
#include "ACADLL/Graph.h"

using namespace std;
#include "BwDesign/BWDesignMacro.h"
#include "BWDesign\BWGraph.h"
#include "BwDesign/View.h"

#include "graph_editor_wnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;
using namespace BWDSGNMOD;


#define ID_GRAPH_COMMON_MULTI_GRID_TAB_WINDOW		300
#define ID_GRAPH_COMMON_WINDOW						301
#define ID_GRAPH_ADVANCE_MULTI_GRID_TAB_WINDOW		302
#define ID_GRAPH_ADVANCE_WINDOW						303



tab_addvance_navigate_button::tab_addvance_navigate_button(CXTPTabManager* manager, graph_editor_wnd* editor)
	: CXTPTabManagerNavigateButton(manager, xtpTabNavigateButtonActiveFiles, xtpTabNavigateButtonAlways)
{
	m_editor = editor;
	m_strToolTip = _T("高级参数设置"); // 基类的成员
}

tab_addvance_navigate_button::~tab_addvance_navigate_button()
{
}

void tab_addvance_navigate_button::DrawEntry(CDC* pDC, CRect rc, COLORREF clr, BOOL bEnabled)
{
	UNUSED_ALWAYS(clr);

	// 用xtp现成的画一个，如果以后觉得不合适再手工用dc画
	XTPPrimitiveSymbol nSymbol = xtpPrimitiveSymbolHome;
	XTPPrimitiveDrawer()->DrawSymbol(pDC, nSymbol, rc, RGB(90, 90, 90)/*clr*/, COLORREF_NULL, !bEnabled);
}

void tab_addvance_navigate_button::PerformClick(HWND hWnd, CPoint pt)
{
	CXTPTabManagerNavigateButton::PerformClick(hWnd, pt);

	if (m_editor) m_editor->change_to_advance_window();
}


tab_common_navigate_button::tab_common_navigate_button(CXTPTabManager* manager, graph_editor_wnd* editor)
	: CXTPTabManagerNavigateButton(manager, xtpTabNavigateButtonActiveFiles, xtpTabNavigateButtonAlways)
{
	m_editor = editor;
	m_strToolTip = _T("常用参数设置"); // 基类的成员
}

tab_common_navigate_button::~tab_common_navigate_button()
{
}

void tab_common_navigate_button::DrawEntry(CDC* pDC, CRect rc, COLORREF clr, BOOL bEnabled)
{
	UNUSED_ALWAYS(clr);

	// 用xtp现成的画一个，如果以后觉得不合适再手工用dc画
	XTPPrimitiveSymbol nSymbol = xtpPrimitiveSymbolHome;
	XTPPrimitiveDrawer()->DrawSymbol(pDC, nSymbol, rc, RGB(90, 90, 90)/*clr*/, COLORREF_NULL, !bEnabled);
}

void tab_common_navigate_button::PerformClick(HWND hWnd, CPoint pt)
{
	CXTPTabManagerNavigateButton::PerformClick(hWnd, pt);

	if (m_editor) m_editor->change_to_normal_window();
}




BEGIN_MESSAGE_MAP(graph_graphic_view, CAcadCtrl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

graph_graphic_view::graph_graphic_view(graph_editor_wnd* editor)
{
	m_editor = editor;
	m_graphic_ui_data = nullptr;
	m_back_color = RGB(255, 255, 255);
	m_bw_graph = nullptr;
}

graph_graphic_view::~graph_graphic_view()
{
	safe_delete(m_bw_graph);
}

bool graph_graphic_view::load_data_to_ui(graph_graphic_ui_observer_base* data)
{
	m_graphic_ui_data = data;
	refresh_graph_window();
	return true;
}

bool graph_graphic_view::save_data_from_ui(graph_graphic_ui_observer_base* data)
{
	return true;
}

void graph_graphic_view::set_window_backcolor(COLORREF bk_color)
{
	m_back_color = bk_color;
}

BOOL graph_graphic_view::create_from_static(CWnd* parent, UINT id, bool has_border /*= TRUE*/)
{
	BOOL result = CreateFromStatic(parent, id);
	if (GetSafeHwnd())
	{
		if (has_border)
			ModifyStyle(NULL, WS_BORDER);
		else
			ModifyStyle(WS_BORDER, NULL);
	}

	return result;
}

BOOL graph_graphic_view::create_direct(CWnd* parent, UINT id, const RECT& rect, bool has_border)
{
	BOOL result = CreateDirect(parent, id, rect);

	if (GetSafeHwnd())
	{
		if (has_border)
			ModifyStyle(NULL, WS_BORDER);
		else
			ModifyStyle(WS_BORDER, NULL);
	}

	return result;
}

void graph_graphic_view::refresh_graph_window()
{
	if (!GetSafeHwnd() || !m_graphic_ui_data) return;

	safe_delete(m_bw_graph); // 先清空图纸数据

	graph_data_base* current_graph = dynamic_cast<graph_data_base*>(data_center::get_data_node(m_graphic_ui_data->m_graph_data_tag));
	if (current_graph && !current_graph->m_graph_dxf_file_name.IsEmpty())
	{
		// 出图结果从生成的文件加载而不是从出图生成的graph数据加载，主要的目的是为了与出图的代码解藕，出图代码
		// 存在大量智能指针误用造成相互依赖的情况，出图后极易造成内存攀升不降的情况，因此，出图的保存文件路径并
		// 重新加载，而不是直接从出图结果数据加载对程序的稳定性更好，而且保存路径可以缩小项目文件大小
		
		m_bw_graph = new CBWGraph();
		//m_bw_graph->ReadDXF(_T("e:\\123.dxf"), TRUE);
		m_bw_graph->ReadDXF(data_center::get_project_working_folder_path(true) + current_graph->m_graph_dxf_file_name, TRUE);
		SetGraphBase(m_bw_graph);
	}
	
	SetGraphBase(m_bw_graph); // 如果为空表示直接清空
	RefreshAll();
}

void graph_graphic_view::on_project_data_deleted(const CString& node_tag)
{

}

void graph_graphic_view::on_project_data_new(const CString& node_tag)
{

}

void graph_graphic_view::on_project_data_changed(const CString& node_tag)
{

}

void graph_graphic_view::on_project_data_notify(const CString& node_tag, notifer_action ac)
{
	if (!m_graphic_ui_data) return;
	if (m_graphic_ui_data->m_graph_data_tag != node_tag) return; // 不是本图纸的响应

	// 图纸出图结束，会触发一次update消息
	if (notifer_action::post_updated_node == ac) refresh_graph_window();
}

int graph_graphic_view::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | WS_BORDER;
	//lpCreateStruct->style &= ~WS_BORDER;
	//if (-1 == CAcadCtrl::OnCreate(lpCreateStruct)) return -1;

	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		app->exclude_skined_window(m_ModelCtrl.GetSafeHwnd());
		app->exclude_skined_window(m_PaperCtrl.GetSafeHwnd());
	}

	//Initialize();
	InitialRule();
	SetCmdLine(nullptr);
	SetGraphBase(nullptr);
	RefreshAll();

	return 0;
}

BOOL graph_graphic_view::OnEraseBkgnd(CDC* dc)
{
	//CRect rt;
	//GetClientRect(&rt);

	//CBrush br;
	//br.CreateSolidBrush(m_back_color);
	//dc->FillRect(&rt, &br);

	return TRUE;
}




BEGIN_MESSAGE_MAP(tab_multi_grid_data_wnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY_EX_RANGE(GN_CELLCHANGE_POST, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_value_changed)
	ON_NOTIFY_EX_RANGE(GN_BUTTONCLICKED, ID_MULTI_GRID_START, ID_MULTI_GRID_END, on_cell_button_clicked)
END_MESSAGE_MAP()

tab_multi_grid_data_wnd::tab_multi_grid_data_wnd(graph_editor_wnd* editor)
{
	m_editor = editor;
	m_grid_ui_data = nullptr;
	m_bk_color = RGB(255, 255, 255);
}

tab_multi_grid_data_wnd::~tab_multi_grid_data_wnd()
{
	// 不需要清理界面资源，view会自己清理，只要清理非界面资源即可
}

bool tab_multi_grid_data_wnd::load_data_to_ui(graph_grid_ui_observer_base* grid_data)
{
	static int grid_tab_page_start_id = 1000;
	static int grid_tab_page_end_id = 2000;

	clear_ui_data();
	int grid_tab_page_current_id = grid_tab_page_start_id;
	m_grid_ui_data = grid_data;

	if (!grid_data) return false;

	std::vector<std::pair<CString, std::vector<CString>>> all_tables;
	grid_data->get_table_template_names(all_tables);

	for (auto& x : all_tables)
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

		// 更新窗口后放入到标签页中
		grid_page->OnInitialUpdate();

		// -1表示总是添加到最后面
		m_grid_tabs.InsertItem(-1, x.first, grid_page->GetSafeHwnd());

		// 登记到缓存中
		map<CString, CTHDataBase*> current_tab_dbs;
		grid_page->get_all_data_base(current_tab_dbs);
		m_all_db_grids.insert(current_tab_dbs.begin(), current_tab_dbs.end());
		m_created_multi_grid.push_back(grid_page);
	}

	if (m_grid_tabs.GetItemCount() > 0) m_grid_tabs.SetCurSel(0);

	m_grid_ui_data->load_data_to_ui(m_all_db_grids);

	for (auto& x : m_all_db_grids)
	{
		if (!x.second) continue;

		CTHBaseExcel* excel = x.second->GetExcel();
		if (excel) excel->ReFresh(TRUE); // after setting data to db, calling this method to redraw ui
	}

	for (auto& x : m_created_multi_grid)
		if (x) x->resize_excels();

	make_all_children_window_back_color_same_as_this();

	return true;
}

bool tab_multi_grid_data_wnd::save_data_from_ui(graph_grid_ui_observer_base* data)
{
	if (!m_grid_ui_data) return false;

	for (auto& x : m_all_db_grids)
	{
		if (!x.second) continue;

		CTHBaseExcel* excel = x.second->GetExcel();
		if (excel) excel->Update(); // 取数据之前必须调这个成员，否则处于编辑状态的数据可能无法回写
	}

	m_grid_ui_data->save_data_from_ui(m_all_db_grids);
	return true;
}

void tab_multi_grid_data_wnd::set_window_backcolor(COLORREF bk_color)
{
	m_bk_color = bk_color;
	make_all_children_window_back_color_same_as_this();
}

void tab_multi_grid_data_wnd::add_mode_navigate_button(CXTPTabManagerNavigateButton* button)
{
	if (!button) return;

	CXTPTabManagerNavigateButtons* tab_buttons = m_grid_tabs.GetNavigateButtons();
	if (tab_buttons)
	{
		// 对象由tab控件接管，不用delete
		tab_buttons->Add(button);
	}
}

multi_grid_tab_control* tab_multi_grid_data_wnd::get_grid_tab_control()
{
	return &m_grid_tabs;
}

void tab_multi_grid_data_wnd::on_project_data_deleted(const CString& node_tag)
{
	if (m_grid_ui_data) load_data_to_ui(m_grid_ui_data);

	for (auto& x : m_all_db_grids)
	{
		CTHBaseExcel* excel = x.second->GetExcel();
		if (excel) excel->ReFresh();	// 新数据加载时要刷新，否则数据显示异常
	}
}

void tab_multi_grid_data_wnd::on_project_data_new(const CString& node_tag)
{
	if (m_grid_ui_data) load_data_to_ui(m_grid_ui_data);

}

void tab_multi_grid_data_wnd::on_project_data_changed(const CString& node_tag)
{
	if (m_grid_ui_data) load_data_to_ui(m_grid_ui_data);
}

void tab_multi_grid_data_wnd::clear_ui_data()
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

void tab_multi_grid_data_wnd::make_all_children_window_back_color_same_as_this()
{
	for (multi_grid_view* wnd : m_created_multi_grid)
		if (wnd && wnd->GetSafeHwnd()) wnd->set_back_color(m_bk_color);
}

int tab_multi_grid_data_wnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER;
	//lpCreateStruct->style &= ~WS_BORDER;
	if (-1 == CWnd::OnCreate(lpCreateStruct)) return -1;

	DWORD wnd_style = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP;
	m_grid_tabs.Create(wnd_style, CRect(), this, ID_GRAPH_COMMON_MULTI_GRID_TAB_WINDOW);
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

void tab_multi_grid_data_wnd::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);
	m_grid_tabs.MoveWindow(0, 0, cx, cy);
}

BOOL tab_multi_grid_data_wnd::on_cell_value_changed(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*) notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_grid_tabs.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	multi_grid_view* cur_view = (cur_wnd ? dynamic_cast<multi_grid_view*>(cur_wnd) : nullptr);
	if (!cur_view) return FALSE;

	CTHExcel* excel = cur_view->find_excel(id);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	if (m_grid_ui_data) m_grid_ui_data->cell_value_changed(db_clicked, nm_msg);
	return TRUE;
}

BOOL tab_multi_grid_data_wnd::on_cell_button_clicked(UINT id, NMHDR * notify_struct, LRESULT * result)
{
	if (!notify_struct) return FALSE;
	NM_THGRID* nm_msg = (NM_THGRID*) notify_struct;

	CXTPTabManagerItem* cur_tab_item = m_grid_tabs.GetSelectedItem();
	CWnd* cur_wnd = (cur_tab_item ? CWnd::FromHandle(cur_tab_item->GetHandle()) : nullptr);
	multi_grid_view* cur_view = (cur_wnd ? dynamic_cast<multi_grid_view*>(cur_wnd) : nullptr);
	if (!cur_view) return FALSE;

	CTHExcel* excel = cur_view->find_excel(id);
	if (!excel) return FALSE;

	CTHDataBase* db_clicked = excel->GetDataBase();
	if (!db_clicked) return FALSE;

	if (m_grid_ui_data) m_grid_ui_data->cell_button_be_clicked(db_clicked, nm_msg);
	return TRUE;
}


BEGIN_MESSAGE_MAP(graph_common_window, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()
graph_common_window::graph_common_window(graph_editor_wnd* editor)
	: m_grid_wnd(editor), m_graphic_wnd(editor), m_editor(editor)
{
}


graph_common_window::~graph_common_window()
{
}

bool graph_common_window::load_data_to_ui(const CString& data_tag)
{
	if (!GetSafeHwnd()) return false;

	if (m_graphic_wnd.GetSafeHwnd())
	{
		m_graphic_observer->m_graph_data_tag = data_tag;
		m_graphic_wnd.load_data_to_ui(m_graphic_observer.get());
	}

	if (m_grid_wnd.GetSafeHwnd())
	{
		m_grid_observer->m_graph_data_tag = data_tag;
		m_grid_wnd.load_data_to_ui(m_grid_observer.get());
	}

	return true;
}

bool graph_common_window::save_data_from_ui()
{
	if (m_graphic_wnd.GetSafeHwnd())
		m_graphic_wnd.save_data_from_ui(m_graphic_observer.get());

	if (m_grid_wnd.GetSafeHwnd())
		m_grid_wnd.save_data_from_ui(m_grid_observer.get());

	return true;
}

void graph_common_window::set_window_backcolor(COLORREF bk_color)
{
	if (m_grid_wnd.GetSafeHwnd()) m_grid_wnd.set_window_backcolor(bk_color);
	if (m_graphic_wnd.GetSafeHwnd()) m_graphic_wnd.set_window_backcolor(bk_color);
	// m_split_wnd控件不用处理，会随主题变化
}

int graph_common_window::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	lpCreateStruct->style &= ~WS_BORDER;
	if (-1 == CWnd::OnCreate(lpCreateStruct)) return -1;

	DWORD wnd_style = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE /*| WS_CLIPSIBLINGS | WS_BORDER*/;
	if (-1 == m_split_wnd.CreateStatic(this, 1, 2)) return -1;
	// m_split_wnd.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE);

	m_graphic_wnd.create_direct(&m_split_wnd, m_split_wnd.IdFromRowCol(0, 0), CRect(), false);
	m_grid_wnd.Create(nullptr, nullptr, wnd_style, CRect(), &m_split_wnd, m_split_wnd.IdFromRowCol(0, 1));
	m_grid_wnd.add_mode_navigate_button(new tab_addvance_navigate_button(m_grid_wnd.get_grid_tab_control(), m_editor));

	m_split_wnd.EnableBorder(FALSE);
	m_split_wnd.EnableFullDrag(FALSE);
	m_split_wnd.SetTheme(xtpControlThemeDefault);
	m_split_wnd.SetColumnInfo(0, 450, 10);
	//m_split_wnd.SetColumnInfo(1, 300, 10); // xtp拆分条不能两列都设置
	m_split_wnd.SetRowInfo(0, 300, 10);
	m_split_wnd.RecalcLayout();

	return 0;
}

void graph_common_window::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);

	if (m_split_wnd.GetSafeHwnd())
	{
		m_split_wnd.MoveWindow(0, 0, cx, cy);
		m_split_wnd.RecalcLayout();
	}
}


BEGIN_MESSAGE_MAP(graph_advance_window, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()
graph_advance_window::graph_advance_window(graph_editor_wnd* editor)
	: m_grid_wnd(editor), m_editor(editor)
{
}


graph_advance_window::~graph_advance_window()
{
}

bool graph_advance_window::load_data_to_ui(const CString& data_tag)
{
	if (!GetSafeHwnd()) return false;

	if (m_grid_wnd.GetSafeHwnd())
	{
		m_grid_observer->m_graph_data_tag = data_tag;
		m_grid_wnd.load_data_to_ui(m_grid_observer.get());
	}

	return true;
}

bool graph_advance_window::save_data_from_ui()
{
	if (m_grid_wnd.GetSafeHwnd())
		m_grid_wnd.save_data_from_ui(m_grid_observer.get());

	return true;
}

void graph_advance_window::set_window_backcolor(COLORREF bk_color)
{
	if (m_grid_wnd.GetSafeHwnd()) m_grid_wnd.set_window_backcolor(bk_color);
}

int graph_advance_window::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	lpCreateStruct->style &= ~WS_BORDER;
	if (-1 == CWnd::OnCreate(lpCreateStruct)) return -1;

	DWORD wnd_style = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE /*| WS_CLIPSIBLINGS | WS_BORDER*/;
	m_grid_wnd.Create(nullptr, nullptr, wnd_style, CRect(), this, ID_GRAPH_ADVANCE_MULTI_GRID_TAB_WINDOW);
	m_grid_wnd.add_mode_navigate_button(new tab_common_navigate_button(m_grid_wnd.get_grid_tab_control(), m_editor));

	return 0;
}

void graph_advance_window::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);

	if (m_grid_wnd.GetSafeHwnd()) m_grid_wnd.MoveWindow(0, 0, cx, cy);
}


BEGIN_MESSAGE_MAP(graph_editor_wnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()
graph_editor_wnd::graph_editor_wnd()
	: m_graph_common_wnd(this), m_graph_advance_wnd(this)
{
}

graph_editor_wnd::~graph_editor_wnd()
{
}

int graph_editor_wnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	lpCreateStruct->style &= ~WS_BORDER;
	if (-1 == CWnd::OnCreate(lpCreateStruct)) return -1;

	DWORD wnd_style = WS_CHILD | WS_CLIPCHILDREN/* | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER*/;
	m_graph_common_wnd.Create(nullptr, nullptr, wnd_style, CRect(), this, ID_GRAPH_COMMON_WINDOW);
	m_graph_advance_wnd.Create(nullptr, nullptr, wnd_style, CRect(), this, ID_GRAPH_ADVANCE_WINDOW);

	change_to_normal_window();

	// 窗口创建时手工设置起始状态的主题
	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		theme_style_settings app_theme = app->get_style_and_settings();
		app_theme_changed(app_theme);
	}

	return 0;
}

bool graph_editor_wnd::load_data_to_ui(const CString& data_tag)
{
	m_data_tag = data_tag;

	m_graph_common_wnd.load_data_to_ui(data_tag);
	m_graph_advance_wnd.load_data_to_ui(data_tag);

	return true;
}

bool graph_editor_wnd::save_data_from_ui()
{
	m_graph_common_wnd.save_data_from_ui();
	m_graph_advance_wnd.save_data_from_ui();

	return true;
}

CString graph_editor_wnd::get_title()
{
	graph_data_base* data = dynamic_cast<graph_data_base*>(data_center::get_data_node(m_data_tag));
	if (!data) return _T("");

	return data->get_node_name();
}

void graph_editor_wnd::app_theme_changed(theme_style_settings& settings)
{
	// 子窗口由编辑统一来调用设置风格相关的接口更代价更小，更内聚，不建议让子窗口也从window_interface派生自己
	// 接管风格设置的功能
	m_graph_common_wnd.set_window_backcolor(settings.m_window_back_color);
	m_graph_advance_wnd.set_window_backcolor(settings.m_window_back_color);
}

void graph_editor_wnd::change_to_advance_window()
{
	if (!GetSafeHwnd()) return;
	if (!m_graph_common_wnd.GetSafeHwnd()) return;
	if (!m_graph_advance_wnd.GetSafeHwnd()) return;

	// 先隐藏后显示
	m_graph_common_wnd.ShowWindow(SW_HIDE);
	m_graph_advance_wnd.ShowWindow(SW_SHOW);
	m_graph_advance_wnd.UpdateWindow(); // 必须立即绘制
}

void graph_editor_wnd::change_to_normal_window()
{
	if (!GetSafeHwnd()) return;
	if (!m_graph_common_wnd.GetSafeHwnd()) return;
	if (!m_graph_advance_wnd.GetSafeHwnd()) return;

	// 先隐藏后显示
	m_graph_advance_wnd.ShowWindow(SW_HIDE);
	m_graph_common_wnd.ShowWindow(SW_SHOW);
	m_graph_common_wnd.UpdateWindow(); // 必须立即绘制
}

void graph_editor_wnd::set_common_grid_ui_observer(std::unique_ptr<graph_grid_ui_observer_base>& observer)
{
	m_graph_common_wnd.m_grid_observer = std::move(observer);
}

void graph_editor_wnd::set_common_graphic_ui_observer(std::unique_ptr<graph_graphic_ui_observer_base>& observer)
{
	m_graph_common_wnd.m_graphic_observer = std::move(observer);
}

void graph_editor_wnd::set_advance_grid_ui_observer(std::unique_ptr<graph_grid_ui_observer_base>& observer)
{
	m_graph_advance_wnd.m_grid_observer = std::move(observer);
}

void graph_editor_wnd::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);

	if (m_graph_common_wnd.GetSafeHwnd())
		m_graph_common_wnd.MoveWindow(0, 0, cx, cy);

	if (m_graph_advance_wnd.GetSafeHwnd())
		m_graph_advance_wnd.MoveWindow(0, 0, cx, cy);
}
