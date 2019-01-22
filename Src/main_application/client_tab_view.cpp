#include "StdAfx.h"
#include "resource.h"

#include "x_framework/macro.h"
#include "x_framework/interface/console_interface.h"
#include "x_framework/interface/window_interface.h"
#include "x_framework/interface/cmd_container_interface.h"

#include "main_frame.h"

#include "client_tab_view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


BEGIN_MESSAGE_MAP(client_tab_view, CXTPTabControl)
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


client_tab_view::client_tab_view()
{
	m_last_right_clicked_tab = nullptr;
}

client_tab_view::~client_tab_view()
{

}

// 根据当前活动窗口激活或隐藏相应的ribbon上下文标签,仅供框架调用，不对外开放（对外开的接口与此不同）
void auto_active_ribbon_context_tabs(const vector<CString>& all_active_view_class)
{
	main_frame* db_frame = dynamic_cast<main_frame*>(AfxGetMainWnd());
	if (!db_frame || !db_frame->GetSafeHwnd()) return;

	app_ribbon_bar* bar = db_frame->get_db_ribbon_bar();
	if (!bar || !bar->GetSafeHwnd()) return;

	bar->active_context_tabs_by_window_class(all_active_view_class, true);
}

void client_tab_view::reset_tab_status()
{
	int tab_count = GetItemCount();

	// 只有一个选项卡时隐藏标签
	if (tab_count <= 1)
		GetPaintManager()->m_bShowTabs = FALSE;
	else
		GetPaintManager()->m_bShowTabs = TRUE;
	
	int sel_tab = GetCurSel();

	if (tab_count >= 1)
	{
		ShowWindow(SW_SHOW);
		SetCurSel(sel_tab);

		vector<CString> all_active_view_class;
		tuple<CString, CString, CWnd*> info;
		if (get_tab_window_and_info(info, sel_tab))
			all_active_view_class.push_back(std::get<0>(info));

		auto_active_ribbon_context_tabs(all_active_view_class); // 激活关联的上下文ribbon标签，并隐藏无关联的上下文ribbon标签

		// 窗口选中状态变化后发个命令，有些窗口要根据这个信息作相应处理，如树节点要显示相应的加载标记
		cmd_trigger_manager::fire(_T("sys"), _T("editor_loaded"), std::get<1>(info));
	}
	else
	{
		ShowWindow(SW_HIDE);
		auto_active_ribbon_context_tabs(vector<CString>()); // 关闭所有的上下文ribbon标签
		
		// 窗口选中状态变化后发个命令，有些窗口要根据这个信息作相应处理，如树节点要显示相应的加载标记
		cmd_trigger_manager::fire(_T("sys"), _T("editor_loaded"), _T("")); // 空路径相当于没有任何编辑器被加载
	}

	// 标签隐藏或显示会因客户区的窗口不自动调整而留下一条残影，此时tab并没有变动尺寸，所以不会触发size调整机制，因此手动触发一下
	SendMessage(WM_SIZE, 0, 0);
}

int client_tab_view::get_tab_count() const
{
	// 一般来说内部的数据个数和界面的标签个数是对应的，如果不对应则说明程序有问题，因此只要返回内部数据的个数即可

	return (int) m_all_windows.size();
}

bool client_tab_view::close_tab_and_clear_info(int index, bool force_close/* = false*/)
{
	CWaitCursor wt_cursor;

	tuple<CString, CString, CWnd*> info;
	if (false == get_tab_window_and_info(info, index)) return false;

	// 放在标志检查前调用，有些窗口在pre_close中会做一些数据保存，并在查询保存标志时返回false，要兼容这种情况，尽早调
	CWnd* wnd = std::get<2>(info);
	if (!wnd) return false;

	window_interface* wnd_ix = dynamic_cast<window_interface*>(wnd);
	if (!wnd_ix) return false;

	CString close_detail;
	if (!force_close && !wnd_ix->can_be_close(close_detail))
	{
		app_print_info(MT_DEBUG, _T("workspace window can't be close, windows class name : %s , tab index : %d"), std::get<0>(info), index);
		AfxMessageBox(close_detail, MB_OK | MB_ICONWARNING);
		return false;
	}

	if (!force_close && wnd_ix && wnd_ix->need_save_ui_data())
		save_tab_window_data(index); // 只是界面数据更新到模型中

	// CWnd的成员DestroyWindow会在销毁窗口后置句柄为空，api函数::DestroyWindow则不会，为保证后续代码不特殊处理，统一在销毁后手动置为空
	if (wnd->GetSafeHwnd())
	{
 		::ShowWindow(wnd->GetSafeHwnd() , SW_HIDE);
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);

		// 不要发WM_DESTORY消息（不是每个窗口的清理工作都在销毁的消息响应中处理，资源处理不当的窗口会崩溃，所以换成通用正确的WM_CLOSE消息
		// ，也不要直接调用wnd->Destory()，要不然遇到像osg这样有渲染线程的窗口会导致线程异常，因此使用它的消息队列来处理
		wnd->SendMessage(WM_CLOSE);
		wnd->m_hWnd = nullptr;
		wnd->m_hWndOwner = nullptr;
	}

	// 删除选项卡，内部并不会删除所接管的对象指针，但会释放mfc内部的cmdtarget非界面资源(如消息队列相关资源或com资源等)
	DeleteItem(index);

	// CView会在窗口销毁之后OnPostDestroy调用delete this;外部指针会悬空，因此，此处再删除将崩溃，因此对CView及
	// 其派生类要特殊处理，重载PostNcDestroy虚函数并禁止调用CView的版本即可，也不用再调用delete，工作区间控件会
	// 正确析构，这并不会造成内存泄漏
	safe_delete(wnd); // 工作区的窗口是动态创建的，肯定是在堆上，所以可以delete
	std::get<2>(info) = nullptr;

	// 清理非窗口资源
	m_last_right_clicked_tab = nullptr;

	if (index >= 0 && index < (int) m_all_windows.size())
		m_all_windows.erase(m_all_windows.begin() + index);

	// 根据当前选项卡状态重设选项卡参数
	reset_tab_status();

	return true;
}

bool client_tab_view::close_all_tab_and_clear_info(bool force_close/* = false*/)
{
	// 每个选项卡关闭的时候都会保存，这里不用了
	// save_all_tab_window_data();

	// 从最后一个往前，一是电脑速度慢时视觉效果好，二是防止删除选项卡导致索引变化
	for (int i = get_tab_count() - 1; i >= 0; --i)
		if (!close_tab_and_clear_info(i, force_close)) return false;

	return true;
}

void client_tab_view::save_tab_window_data(int index)
{
	tuple<CString, CString, CWnd*> info;
	if (false == get_tab_window_and_info(info, index)) return;

	CWnd* wnd = std::get<2>(info);
	if (!wnd) return;

	window_interface* win_ix = dynamic_cast<window_interface*>(wnd);
	if (!win_ix) return;

	win_ix->save_data_from_ui();
	m_last_right_clicked_tab = nullptr;
}

void client_tab_view::save_all_tab_window_data()
{
	for (auto& x : m_all_windows)
	{
		CWnd* wnd = std::get<2>(x);

		window_interface* win_ix = dynamic_cast<window_interface*>(wnd);
		if (!win_ix) continue;

		win_ix->save_data_from_ui();
	}

	m_last_right_clicked_tab = nullptr;
}

bool client_tab_view::is_special_window_exist(const CString& window_class, const CString& object_path) const
{
	for (auto& x : m_all_windows)
		if (std::get<0>(x) == window_class && 0 == object_path.CompareNoCase(std::get<1>(x)))
			return true;
	
	return false;
}

int client_tab_view::get_special_window_index(const CString& window_class, const CString& object_path) const
{
	for (size_t i = 0; i < m_all_windows.size(); ++i)
		if (std::get<0>(m_all_windows[i]) == window_class && 0 == object_path.CompareNoCase(std::get<1>(m_all_windows[i])))
			return i;

	return -1;
}

void client_tab_view::remove_special_window_info(const CString& window_class, const CString& object_path)
{
	vector<tuple<CString, CString, CWnd*>> all_new_windows;
	for (auto& x : m_all_windows)
		if (!(std::get<0>(x) == window_class && 0 == object_path.CompareNoCase(std::get<1>(x))))
			all_new_windows.push_back(x);

	m_all_windows = all_new_windows;
}

bool client_tab_view::remove_windows_by_path(const CString& object_path)
{
	// 程序允许添加与管理对象没有对应关系的窗口，这种窗口路径为空，且同一个窗口类只会创建一个，因此如果异常情况下有多个也全关
	// if (object_path.IsEmpty()) return;

	// 不能使用迭代器的方式，因为关闭选项卡的时候会清理窗口和类的对应数据，此时迭代器指向的对象被删除了，会导致它++时失效
	// for (auto& x : m_all_windows)
	
	if (m_all_windows.empty()) return true; // 没有窗口也算作关闭成功
	for (int i = (int) m_all_windows.size() - 1; i >= 0; --i)
	{
		if (0 == object_path.CompareNoCase(std::get<1>(m_all_windows[i])))
		{
			int index = get_special_window_index(std::get<0>(m_all_windows[i]), std::get<1>(m_all_windows[i]));
			if (-1 == index) continue;
			if (!close_tab_and_clear_info(index)) return false;
		}
	}

	return true;
}

int client_tab_view::add_tab_window_and_info(const CString& window_class, const CString& node_tag, int index/* = -1*/, int image_index/* = -1*/)
{
	CWaitCursor wt_cursor;
	// 允许路径为空，表示没有管理对象，这类窗口只能加一个，如起始页
	//if (node_tag.IsEmpty())
	//{
	//	AfxMessageBox(_T("窗口没有关联的管理对象"), MB_OK | MB_ICONERROR);
	//	return -1;
	//}

	if (!GetSafeHwnd())
	{
		AfxMessageBox(_T("工作区容器窗口句柄为空，无法显示窗口"), MB_OK | MB_ICONERROR);
		return -1;
	}

	static int window_id = 1500; // 内部自动维护窗口id，先简单递加，后面有时间再智能处理

	if (window_class.IsEmpty())
	{
		AfxMessageBox(_T("窗口类名为空，无法实例化，请在xml中正确配置窗口类名"), MB_OK | MB_ICONERROR);
		return -1;
	}

	int index_exist = get_special_window_index(window_class, node_tag);
	if (index_exist >= 0) return index_exist;

	CWnd* wnd = dynamic_cast<CWnd*>(CRuntimeClass::CreateObject(window_class));
	if (!wnd)
	{
		AfxMessageBox(_T("窗口类[") + window_class + _T("]未正确注册，无法动态创建"), MB_OK | MB_ICONERROR);
		return -1;
	}

	DWORD wnd_sytle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	wnd_sytle &= ~WS_BORDER;

	CRect rect = CXTPTabManager::GetClientRect();
	
	// 控制在1500-35000之间，程序工作区不可能同时存在几万个先期卡窗口，所以循环是没问题的
	// 如果不循环的话，窗口打开的次数是很可能连续工作较长时间就撑满了，会造成工作区窗口处理不正常
	// 用户不可能同时开3万个窗口，因此多次操作后前面的id就空闲了，因此可以再回头利用
	unsigned int id = ++window_id;
	if (id > 35000) { id = 1500; window_id = 1500; }

	if (!wnd->Create(NULL, NULL, wnd_sytle, rect, this, id))
	{
		AfxMessageBox(_T("窗口类[") + window_class + _T("]实例化后调用窗口创建函数Create失败"), MB_OK | MB_ICONERROR);
		return -1;
	}
	
	window_interface* wnd_ix = dynamic_cast<window_interface*>(wnd);
	if (!wnd_ix)
	{
		AfxMessageBox(_T("窗口类[") + window_class + _T("]没有从window_interface接口以public方式派生，无法添加到工作区"), MB_OK | MB_ICONERROR);
		wnd->DestroyWindow();
		safe_delete(wnd);
		return -1;
	}

	return implement_add_tab_window_and_info(wnd_ix, node_tag, index, image_index);
}

int client_tab_view::add_tab_window_and_info(window_interface* wnd, const CString node_tag, int index /*= -1*/, int image_index /*= -1*/)
{
	CWaitCursor wt_cursor;

	if (!wnd) return -1;

	return implement_add_tab_window_and_info(wnd, node_tag, index, image_index);
}

CWnd* client_tab_view::get_tab_window(int index) const
{
	tuple<CString, CString, CWnd*> info;
	if (false == get_tab_window_and_info(info, index)) return nullptr;

	return std::get<2>(m_all_windows[index]);
}

window_interface* client_tab_view::get_tab_window_interface(int index) const
{
	return dynamic_cast<window_interface*>(get_tab_window(index));
}

bool client_tab_view::get_tab_window_and_info(std::tuple<CString, CString, CWnd*>& info, int index) const
{
	int tab_count = GetItemCount();
	if (tab_count <= 0) return false;

	if (index < 0 || index >= tab_count) index = tab_count - 1;

	// 指针不能通过tab控件的HANDLE来构造，这样得到的是真正的CWnd指针，已经无法知道原来的派生类是什么了，所以要自己记录并维护CWnd指针
	if (index >= (int)m_all_windows.size()) return false;

	info = m_all_windows[index];
	return true;
}

int client_tab_view::get_last_right_clicked_tab_index() const
{
	if (!m_last_right_clicked_tab) return -1;

	return m_last_right_clicked_tab->GetIndex();
}

void client_tab_view::clear_last_right_clicked_tab_index()
{
	m_last_right_clicked_tab = nullptr;
}

int client_tab_view::implement_add_tab_window_and_info(window_interface* wnd_ix, const CString node_tag, int index, int image_index)
{
	if (!wnd_ix) return -1;

	CWnd* wnd = dynamic_cast<CWnd*>(wnd_ix);
	if (!wnd || !wnd->GetSafeHwnd()) return -1;

	wnd->SetOwner(this);
	wnd->SetParent(this);
	wnd->ModifyStyle(WS_BORDER, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP);

	// 用户指定的是删除前的索引，选项卡少了，索引也要相应跟着减
	int current_tab_count = get_tab_count();
	
	// -1本来可以直接用的，但为了和放入的数据同步，改为真实的位置
	int tab_count = GetItemCount();
	if (index < 0 || index > tab_count) index = tab_count;

	CString window_class = wnd->GetRuntimeClass()->m_lpszClassName;
	m_all_windows.push_back(make_tuple(window_class, node_tag, wnd));

	// 始终添加到最后，选项卡标题等数据加载后再设置
	CXTPTabManagerItem* added_item = InsertItem(index, _T(""), wnd->GetSafeHwnd(), image_index);

	// 数据先行，再更新界面，数据加载要放在OnInitialUpdate前面，以便窗口能根据内容自动决定在OnInitialUpdate时决定呈现方式
	// 空数据对象也原样传入，用不用由窗口自己决定有些窗口确实是不需要数据的，如起始页
	wnd_ix->load_data_to_ui(node_tag);

	// 加载到工作区后再加载数据，如果是view则先调用OnInitialUpdate
	CView* view = dynamic_cast<CView*>(wnd);
	if (view) view->OnInitialUpdate();

	// 标题放在最后设置，以便窗口能根据数据最终的状态来作处理
	if (added_item) added_item->SetCaption(wnd_ix->get_title());

	// 添加完成后发个命令，有些窗口要根据这个信息作相应处理，如树节点要显示相应的加载标记
	cmd_trigger_manager::fire(_T("sys"), _T("editor_loaded"), node_tag);

	return index;
}

void client_tab_view::OnItemClick(CXTPTabManagerItem* pItem)
{
	int tab_index_before_click = GetCurSel();
	CXTPTabControl::OnItemClick(pItem);
	int tab_index_after_click = GetCurSel();

	if (tab_index_before_click != tab_index_after_click)
	{
		vector<CString> all_active_view_class;
		tuple<CString, CString, CWnd*> info;
		if (get_tab_window_and_info(info, tab_index_after_click))
		{
			all_active_view_class.push_back(std::get<0>(info));
			CWnd* wnd = std::get<2>(info);
			if (wnd && wnd->GetSafeHwnd()) wnd->SetFocus(); // 触发一下编辑器窗口的OnFocus，以便其处理相关信息，如获取命令行会话权等
		}

		auto_active_ribbon_context_tabs(all_active_view_class); // 激活关联的上下文ribbon标签，并隐藏无关联的上下文ribbon标签
	}

	//reset_tab_status();
}

void client_tab_view::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_last_right_clicked_tab = HitTest(point);
	if (!m_last_right_clicked_tab) return;

	CMenu tab_right_menu;
	tab_right_menu.LoadMenu(IDR_MENU_WORKSPACE_TAB);
	CMenu* first_pop_up = tab_right_menu.GetSubMenu(0);
	if (!first_pop_up) return;

	ClientToScreen(&point);
	// 由父窗口响应菜单消息（即由workspace_view响应）
	first_pop_up->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, GetParent());
}

BOOL client_tab_view::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}
