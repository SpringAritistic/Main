#include "stdafx.h"
#include <set>

#include "x_framework/macro.h"
#include "x_framework/tools/math_tools.h"
#include "x_framework/tools/file_tools.h"
#include "x_framework/tools/xml_tools.h"
#include "x_framework/tools/global_parameters.h"
#include "x_framework/interface/console_interface.h"
#include "x_framework/interface/window_interface.h"
#include "x_framework/interface/cmd_container_interface.h"
#include "x_framework/interface/app_ui_interface.h"

#include "THGridControl/THGridControlMac.h"
#include "THGridControl/GridGlobalData.h"

#include "app_ribbon_bar.h"
#include "help_configs.h"
#include "app_configs.h"
#include "app_skin_manager.h"
#include "main_application.h"

#include "main_frame.h"



#define MSG_DO_FUNCTION      WM_USER+1101



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


//////////////////////////////////////////////////////////////////////////

struct private_workspace_close_postmessage_parameters
{
	// 以下参数在关闭时有效
	int			closing_action_type = 0;	// 0表示根据类名来关闭，1表示根据索引来关闭
	bool		force_close = false;		// 不提示保存信息，直接调用窗口关闭
	CString		window_class;
	int			window_index = -1;

	// 以下参数在添加时有效
	CString		node_tag;
	int			index = -1;
	int			image_index = -1;
};

struct private_workspace_added_postmessage_parameters
{
	// 以下参数在关闭时有效
	CString		window_class;
	CString		node_tag;
	int			index = -1;
	int			image_index = -1;
};

struct private_do_command_postmessage_parameters
{
	CString domain;
	CString cmd;
	CString param;
};

//////////////////////////////////////////////////////////////////////////

main_frame::main_frame()
{
	m_is_style_changing.store(false);

	set_app_ui_interface(this);
	initialize_theme_style_settings();

	m_docking_pane_layout = nullptr;
	m_ribbon_bar = nullptr;

	m_current_theme = ID_THEME_STYLEBLUE2010; // 默认为Office 2010蓝色风格
	m_is_ribbon_mini_size = false;
	m_docking_panes_be_created = false;

	main_application* app = (main_application*) AfxGetApp();
	if (app)
	{
		int nValue = (int) app->GetProfileInt(_T("Skin"), _T("ThemeType"), ID_THEME_STYLESILVER2010);
		if (nValue == ID_THEME_STYLEBLUE2007
			|| nValue == ID_THEME_STYLESILVER2007
			|| nValue == ID_THEME_STYLEBLACK2007
			|| nValue == ID_THEME_STYLEBLUE2010
			|| nValue == ID_THEME_STYLESILVER2010
			|| nValue == ID_THEME_STYLEBLACK2010
			|| nValue == ID_THEME_STYLE_LIGHT2013
			|| nValue == ID_THEME_STYLE_DARK2013)
		{
			m_current_theme = nValue;
		}
		else
		{
			m_current_theme = ID_THEME_STYLEBLUE2010;
		}

		nValue = (int) app->GetProfileInt(_T("Skin"), _T("RibbonMini"), 0);
		m_is_ribbon_mini_size = (1 == nValue ? true : false);
	}

	// 初始状态提前切换好可以避免第一次界面显示时框架主题切换出现闪烁
	theme_style_settings setting = get_style_and_settings();
	m_bAllowDwm = (setting.m_enable_aero ? TRUE : FALSE);
}

main_frame::~main_frame()
{
	m_docking_pane_config.m_container.clear(); // 清空uiData
	m_docking_pane_layout = nullptr; // 不要delete，框架会在关闭窗口前保存状态并释放对象
	XTPSkinManager()->LoadSkin(NULL); // 清理皮肤资源
}


BEGIN_MESSAGE_MAP(main_frame, CXTPFrameWnd)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_HELPINFO()
	ON_WM_DROPFILES()
	ON_XTP_CREATECONTROL()

	ON_COMMAND_RANGE(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, on_recent_file_list)
	ON_UPDATE_COMMAND_UI(ID_FILE_RECENTDOCUMENTS, on_update_ribbon_recentdocs)

	// 停靠面板创建或从文件加载位置和大小时会调用（也可以使用派生并实现虚函数的方式响应）
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, on_docking_pane_notify)

	// Ribbon消息响应
	ON_COMMAND_RANGE(IDS_RIBBON_UI_MIN, IDS_RIBBON_UI_MAX, on_all_ribbon_tab_command)
	ON_UPDATE_COMMAND_UI_RANGE(IDS_RIBBON_UI_MIN, IDS_RIBBON_UI_MAX, on_update_all_ribbon_tab_command)
	
	// Ribbon file menu bar消息响应
	ON_COMMAND_RANGE(IDS_RIBBON_FILE_MENU_MIN, IDS_RIBBON_FILE_MENU_MAX, on_all_ribbon_file_menu_command)
	ON_UPDATE_COMMAND_UI_RANGE(IDS_RIBBON_FILE_MENU_MIN, IDS_RIBBON_FILE_MENU_MAX, on_update_all_ribbon_file_menu_command)

	// Ribbon quick access bar消息响应
	ON_COMMAND_RANGE(IDS_RIBBON_QUICK_ACCESS_MIN, IDS_RIBBON_QUICK_ACCESS_MAX, on_all_ribbon_quick_access_command)
	ON_UPDATE_COMMAND_UI_RANGE(IDS_RIBBON_QUICK_ACCESS_MIN, IDS_RIBBON_QUICK_ACCESS_MAX, on_update_all_ribbon_quick_access_command)

	// Ribbon bar options区域消息响应
	ON_COMMAND_RANGE(IDS_RIBBON_OPTIONS_MIN, IDS_RIBBON_OPTIONS_MAX, on_all_ribbon_options_command)
	ON_UPDATE_COMMAND_UI_RANGE(IDS_RIBBON_OPTIONS_MIN, IDS_RIBBON_OPTIONS_MAX, on_update_all_ribbon_options_command)

	// 加速键消息响应
	ON_COMMAND_RANGE(ID_ACCELERATOR_CMD_MIN, ID_ACCELERATOR_CMD_MAX, on_all_accelerator_command)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ACCELERATOR_CMD_MIN, ID_ACCELERATOR_CMD_MAX, on_update_all_accelerator_command)

	// 主题切换消息响应
	ON_COMMAND_RANGE(ID_THEME_STYLEBLUE2007, ID_THEME_STYLE_DARK2013, on_command_skin_and_theme_menu_item)
	ON_UPDATE_COMMAND_UI_RANGE(ID_THEME_STYLEBLUE2007, ID_THEME_STYLE_DARK2013, on_update_skin_and_theme_menu_item)

	ON_MESSAGE(MSG_DO_FUNCTION, on_msg_do_functor)

END_MESSAGE_MAP()



int main_frame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{  
	if (lpCreateControl->nID == ID_FILE_RECENTDOCUMENTS)
	{
		lpCreateControl->pControl = new CXTPRibbonControlSystemRecentFileList();
		return TRUE;
	}
 
	return FALSE;
}

BOOL main_frame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.hMenu = nullptr;
	cs.lpszClass = _T("app_main_frame");

	if (!CXTPFrameWnd::PreCreateWindow(cs)) return FALSE;

	// 注册窗口类的目的是为了改变窗口背景（其它方式没这种方式平滑，且可以更彻底的去除菜单）
	// 注册窗口时不能添加CS_HREDRAW风格，否则窗口可能会在顶部留下一条白色的残影，而且CS_HREDRAW | CS_VREDRAW根本不需要添加
	// 窗口背景会根据资源自动处理，此处暂给窗口色，以防止主窗口没有HBRUSH不绘制客户区导致窗口有残影
	CXTPDrawHelpers::RegisterWndClass(AfxGetInstanceHandle(), cs.lpszClass,  /*CS_HREDRAW |CS_VREDRAW | */CS_DBLCLKS,
		AfxGetApp()->LoadIcon(IDR_MAINFRAME), (HBRUSH) (COLOR_WINDOW + 1));

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE; // 此风格要放在注册后处理才能生效

	// cs.style |= WS_MAXIMIZE; 不在此处设置窗口最大化,会造成启动时窗口从小变大有视觉残留

	// 用mfc的注册函数也一样可以
	//cs.lpszClass = AfxRegisterWndClass(/*CS_HREDRAW | CS_VREDRAW | */CS_DBLCLKS, 
	//	::LoadCursor(NULL, IDC_ARROW), (HBRUSH) (COLOR_WINDOW + 1), AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	return TRUE;
}

BOOL main_frame::PreTranslateMessage(MSG* pMsg)
{
	if (m_accelerator_map.get_accelerator_handle() && WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		// 加速键如果主程序处理了就不要让底层处理，这样可以避免触发主程序未定义的加速键
		if (::TranslateAccelerator(GetSafeHwnd(), m_accelerator_map.get_accelerator_handle(), pMsg)) return TRUE;
	}

	return CXTPFrameWnd::PreTranslateMessage(pMsg);
}

CXTPControl* main_frame::get_ribbon_control(const CString& domain, const CString& cmd, const CString& param /*= _T("")*/, bool compare_param /*= false*/)
{
	app_ribbon_bar* rib = get_db_ribbon_bar();
	if (!rib || !rib->GetSafeHwnd()) return nullptr;

	int id = m_ribbon_config.get_id(domain, cmd, param, compare_param);
	if (-1 == id) return nullptr;

	return rib->get_control(id);
}

void main_frame::select_ribbon_tab(const CString& tab_name)
{
	if (!m_ribbon_bar || !m_ribbon_bar->GetSafeHwnd()) return;
	m_ribbon_bar->select_tab(tab_name);
}

void main_frame::show_ribbon_context_tabs(const std::vector<CString>& tab_names)
{
	if (!m_ribbon_bar || !m_ribbon_bar->GetSafeHwnd()) return;
	m_ribbon_bar->active_context_tabs_always_by_tab_name(tab_names);
}

void main_frame::add_recent_file_list(const CString& file_path_name)
{
	for (int i = 0; i < the_main_app.get_recent_list_item_count(); ++i)
		if (0 == file_path_name.CompareNoCase(the_main_app.get_recent_list_item_full_path(i))) return;

	try
	{
		// mfc框架对历史列表的添加时机有要求，本程序由外部手动添加，并没有使用mfc的文档视图机制，因此会有异常，这
		// 个异常不影响添加历史列表的功能，屏蔽掉
		// 另外，添加历史列表时mfc内部使用了一些很老的API接口，接口要求传入的路径不得使用"/"而是要使用"\\"，否则
		// 会有断言
		CString tmp_file_name = file_path_name;
		tmp_file_name.Replace(_T('/'), _T('\\'));
		the_main_app.AddToRecentFileList(tmp_file_name);
	}
	catch (...)
	{
		// nothing;
	}
}

void main_frame::get_recent_file_list(std::vector<CString>& file_list)
{
	file_list.clear();

	for (int i = 0; i < the_main_app.get_recent_list_item_count(); ++i)
		file_list.push_back(the_main_app.get_recent_list_item_full_path(i));
}

bool main_frame::docking_panes_be_created()
{
	return m_docking_panes_be_created;
}

bool main_frame::do_function_by_post_message(functor_by_message* fun)
{
	return (TRUE == PostMessage(MSG_DO_FUNCTION, (WPARAM) fun, NULL));
}

bool main_frame::do_function_by_send_message(functor_by_message* fun)
{
	return (TRUE == (BOOL)SendMessage(MSG_DO_FUNCTION, (WPARAM) fun, NULL));
}

CDocument* main_frame::GetActiveDocument()
{
	// 重载这个虚函数避开mfc的有效性检查
	return nullptr;
}

bool main_frame::close_working_space_view_by_class(const CString& class_name, bool force_close /*= false*/)
{
	for (int i = m_workspace_view.get_tab_count() - 1; i >= 0; --i)
	{
		tuple<CString, CString, CWnd*> info;
		if (false == m_workspace_view.get_tab_window_and_info(info, i)) continue;

		if (std::get<0>(info) == class_name) m_workspace_view.close_tab_by_index(i, force_close);
	}

	return true;
}

bool main_frame::close_working_space_view_by_tag(const CString& tag, bool force_close /*= false*/)
{
	for (int i = m_workspace_view.get_tab_count() - 1; i >= 0; --i)
	{
		tuple<CString, CString, CWnd*> info;
		if (false == m_workspace_view.get_tab_window_and_info(info, i)) continue;

		if (std::get<1>(info) == tag) m_workspace_view.close_tab_by_index(i, force_close);
	}

	return true;
}

bool main_frame::close_working_space_view_by_index(int index, bool force_close/* = false*/)
{
	return m_workspace_view.close_tab_by_index(index, force_close);
}

int main_frame::get_working_space_view_count()
{
	return m_workspace_view.get_tab_count();
}

bool main_frame::get_tab_window_and_info(std::tuple<CString, CString, window_interface*>& info, int index) const
{
	tuple<CString, CString, CWnd*> wnd_info;
	if (!m_workspace_view.get_tab_window_and_info(wnd_info, index)) return false;

	window_interface* wx = dynamic_cast<window_interface*>(std::get<2>(wnd_info));
	info = make_tuple(std::get<0>(wnd_info), std::get<1>(wnd_info), wx);

	return true;
}

int main_frame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//lpCreateStruct->dwExStyle |= WS_EX_ACCEPTFILES; // 支持文件拖放打开
	if (-1 == CXTPFrameWnd::OnCreate(lpCreateStruct)) return -1;

	// 支持文件拖放打开，在vista之后的系统中有uac机制，新加了以下两个api接口，用于处理低权限的程序往高版本的程
	// 序发不了消息的情况，否则以管理员权限启动的程序会无法通过拖放文件的方式来打开文件，xp则没有这个接口，也不
	// 用处理这种情况也能正常接收拖放消息（没有权限限制）
#ifdef WINVER
#	if WINVER >= 0x0600 // Vista

	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD); // 0x0049 == WM_COPYGLOBALDATA

#	endif
#endif

	DragAcceptFiles(TRUE);

	// 不要过早放到应用程序的实例化中，否则会因皮肤单例而阻塞住程序，但是得早于设置皮肤的过滤模块之前
	// 设置好皮肤管理器前不要使用接口获取管理器单例，否则一个对象在使用的同时删除它会崩溃，另外，XTP皮肤管理器的
	// 构造函数会自动将this设置为默认的管理器，设置时不得再删除，否则把自身给删除了
	CXTPSkinManager::SetSkinManager(new app_skin_manager(), FALSE);

	// 框架初始化工具栏
	if (!InitCommandBars()) return -1;

	// 创建Ribbon工具栏
	SetMenu(nullptr); // 先去除主窗口的菜单
	CXTPCommandBars* bars = GetCommandBars();
	if (!bars) return -1;
	if (!create_ribbon_bar(bars)) return -1;

	// 创建状态栏
	if (!create_status_bar()) return -1;

	// 创建停靠面板管理器
	create_docking_pane_manager();
	initialize_docking_panes_config();
	if (m_docking_pane_config.m_force_create_by_frame) create_docking_panes();

	// 设置程序默认标题
	set_title(_T(""));

	// 创建工作区子窗口
	create_working_space();

	// 创建完成后切换到想要显示的主题
	on_command_skin_and_theme_menu_item(m_current_theme); // 设置界面风格

	// 加载加速键映射表
	m_accelerator_map.create_accelerator_table_by_xml();

	return 0;
}

void main_frame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CXTPFrameWnd::OnGetMinMaxInfo(lpMMI);

	// 防止窗口过小ribbon隐藏破坏界面美感
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 300;
}

void main_frame::OnSysCommand(UINT nID, LPARAM lParam)
{
	CXTPFrameWnd::OnSysCommand(nID, lParam);
}

void main_frame::OnDropFiles(HDROP drop_info)
{
	int drop_count = DragQueryFile(drop_info, -1, NULL, 0);
	if (drop_count >= 1)
	{
		TCHAR file_path_name[MAX_PATH + 1] = { 0 };
		DragQueryFile(drop_info, 0, file_path_name, MAX_PATH); // 只要第一个
		DragFinish(drop_info); // 拖放结束后,释放内存

		CString ext = file_tools::get_ext_from_file_path_name(file_path_name);
		if (0 != ext.CompareNoCase(_T("dbr")))
		{
			AfxMessageBox(_T("无法识别的文件类型"), MB_OK | MB_ICONERROR);
			return;
		}

		cmd_trigger_manager::fire(_T("app"), _T("open"), file_path_name);
	}

	// 不要再调用基类打开文件了
	// CXTPFrameWnd::OnDropFiles(drop_info);
}

BOOL main_frame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 这是以前桥博里面的特殊需求和做法（有不合常理的窗口要处理）现在注掉不用，因为窗口在数据清理的过程中可能
	// 会要求关闭，调用到本函数后，再次获取工作区窗口时窗口已经析构，再根据动态类型查询相关对象时会崩溃
	// [操作：在零件库中来回切换窗口然后点根节点的“清空”]可重现
	//window_interface* wnd_ix = get_current_working_space_windows();
	//CWnd* wnd = dynamic_cast<CWnd*>(wnd_ix);
	//
	//// let the view have first crack at the command
	//if (wnd && wnd->GetSafeHwnd() && wnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	// otherwise, do default handling
	return CXTPFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void main_frame::OnSize(UINT flag, int cx, int cy)
{
	CXTPFrameWnd::OnSize(flag, cx, cy); 
}

void main_frame::OnClose()
{
	CRect rt;
	if (!IsZoomed())
	{
		GetWindowRect(&rt);
	}
	else
	{
		// 已经最大化了，要得到最大化之前的窗口尺寸和位置
		WINDOWPLACEMENT pl;
		GetWindowPlacement(&pl);
		rt.left = pl.rcNormalPosition.left;
		rt.top = pl.rcNormalPosition.top;
		rt.right = pl.rcNormalPosition.right;
		rt.bottom = pl.rcNormalPosition.bottom;
	}

	app_size_config_helper::save_last_main_frame_pos_and_size(rt);
	app_size_config_helper::save_maxmize_flag(IsZoomed() ? true : false);

	// 必须先交给命令响应对象处理，以确保其便于安排数据和界面的先后处理机制，然后框架再处理界面
	cmd_trigger_manager::fire(_T("sys"), _T("quit"), _T(""));
	
	// 善后处理一下数据中心，界面，即使命令对象已经处理了再进行处理也没问题，顺序不管，如果要干涉，请命
	// 令处理对象在自己响应的消息中正确处理好界面逻辑，而不是在这个地方改
	if (data_center::is_open()) data_center::close_file();
	close_all_workspace_window(true);
	destroy_all_docking_panes();

	CXTPFrameWnd::OnClose();
}

XTPToolTipStyle main_frame::get_tooltip_style() const
{
	return get_style_and_settings().m_tooltip_style;
}

std::pair<XTPPaintTheme, CString> main_frame::get_paint_style() const
{
	theme_style_settings setting = get_style_and_settings();
	return make_pair(setting.m_paint_theme, setting.m_theme_dll_ini_config_file_name);
}

XTPDockingPanePaintTheme main_frame::get_docking_panes_style() const
{
	return get_style_and_settings().m_docking_pane_theme;
}

bool main_frame::create_ribbon_bar(CXTPCommandBars* command_bars)
{
	// 先清空uiData
	m_ribbon_config.m_all_tabs.clear();

	if (!command_bars) return false;

	// 开始读取xml配置
	CString xml_file_full_path = file_tools::get_app_path_with_xg() + _T("gui_xml\\app_ribbon.xml");
	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_SUCCESS != doc.LoadFile(string_tools::CString_to_string(xml_file_full_path).c_str())) return false;

	tinyxml2::XMLElement* node = doc.FirstChildElement("app_ribbon");
	if (!node || node->NoChildren()) return true;

	m_ribbon_config.initlize_config_data(node);

	m_ribbon_bar = (app_ribbon_bar*) command_bars->Add(_T("app_ribbon_bar"), xtpBarTop, RUNTIME_CLASS(app_ribbon_bar));
	if (!m_ribbon_bar) return false;

	// Ribbon 的界面配置数据已经加载了
	m_ribbon_bar->initlize_ribbon_bar(m_ribbon_config);
	m_ribbon_bar->EnableQuickAccessCustomItem(FALSE);

	EnableToolTips(FALSE); // 禁用系统原先的ToolTip

	// 设置Ribbon风格的ToolTip
	CXTPToolTipContext* pToolTipContext = GetCommandBars()->GetToolTipContext();
	if (pToolTipContext)
	{
		CXTPToolTipContext* pToolTipContext = GetCommandBars()->GetToolTipContext();
		pToolTipContext->SetStyle(get_tooltip_style());
		pToolTipContext->ShowTitleAndDescription();
		pToolTipContext->ShowImage(TRUE, 0);
		pToolTipContext->SetMargin(XTP_DPI(CRect(2, 2, 2, 2)));
		pToolTipContext->SetMaxTipWidth(XTP_DPI_X(200));
		pToolTipContext->SetFont(command_bars->GetPaintManager()->GetIconFont());
		pToolTipContext->SetDelayTime(TTDT_INITIAL, 300);
	}

	// 显示Ribbon中配置的快捷键
	command_bars->GetCommandBarsOptions()->bShowKeyboardTips = TRUE;
	command_bars->GetCommandBarsOptions()->bToolBarAccelTips = TRUE;

	// 启用DPI自适应模型，在高分屏模式下图标自动绽放
	command_bars->GetPaintManager()->m_bAutoResizeIcons = TRUE;
	command_bars->GetCommandBarsOptions()->SetDPIScallingOptions(TRUE);

	return true;
}

bool main_frame::create_status_bar()
{
	return true; // 目前用不到，暂时注掉

	//std::vector<UINT> indicators(1, ID_SEPARATOR);

	//if (-1 == m_status_bar.Create(this)) return false;
	//m_status_bar.SetIndicators(&indicators[0], 10001);

	//m_status_bar.SetCommandBars(GetCommandBars());
	//m_status_bar.SetDrawDisabledText(false);
	//m_status_bar.GetStatusBarCtrl().SetMinHeight(22);

	////////add_status_progress();

	////////std::vector<UINT> xmlindicators;
	////////Cx_ConfigSection items(m_frameNode.GetSection(L"statusbar/indicators"));
	////////for (int i = 0;; i++)
	////////{
	////////	Cx_ConfigSection item(items.GetSectionByIndex(L"item", i));
	////////	UINT id = GetNodeID(item, L"id");
	////////	if (0 == id)
	////////		break;
	////////	xmlindicators.push_back(id);
	////////	if (ID_SEPARATOR == id && xmlindicators.front() == id)
	////////		xmlindicators.erase(xmlindicators.begin());
	////////}

	////////for (size_t i = 0; i < xmlindicators.size(); ++i)
	////////{
	////////	m_status_bar.AddIndicator(xmlindicators[i]);
	////////}

	//m_status_bar.EnableCustomization(TRUE);

	//return true;
}

void main_frame::add_status_progress()
{
	//// Create the edit control and add it to the status bar
	//if (!m_wndProgCtrl.Create(WS_CHILD | WS_VISIBLE/*|PBS_SMOOTH*/,
	//	CRect(0, 0, 0, 0), &m_status_bar, 0))
	//{
	//	TRACE0("Failed to create edit control.\n");
	//	return;
	//}

	//// add the indicator to the status bar.
	//CXTPStatusBarPane* pPane = m_status_bar.AddIndicator(ID_INDICATOR_PROG);

	//// Initialize the pane info and add the control.
	//int nIndex = m_status_bar.CommandToIndex(ID_INDICATOR_PROG);
	//ASSERT(nIndex != -1);

	//pPane->SetCaption(pPane->GetText());
	//pPane->SetText(_T(""));

	//m_status_bar.SetPaneWidth(nIndex, 180);
	//m_status_bar.SetPaneStyle(nIndex, m_status_bar.GetPaneStyle(nIndex) | SBPS_NORMAL);
	//m_status_bar.AddControl(&m_wndProgCtrl, ID_INDICATOR_PROG, FALSE);

	//pPane->SetCustomizationVisible(FALSE);
}

void main_frame::hide_docking_pane_by_id(int id)
{
	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			if (id == y.m_child_window_id)
			{
				m_pane_manager.ClosePane(y.m_id);
				return;
			}
		}
	}
}

void main_frame::hide_docking_pane_by_class(const CString& class_name)
{
	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			if (0 == y.m_child_window_class.CompareNoCase(class_name))
			{
				m_pane_manager.ClosePane(y.m_id);
				return;
			}
		}
	}
}

void main_frame::hide_docking_pane_by_identify(const CString& identify)
{
	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			if (0 == y.m_app_identify.CompareNoCase(identify))
			{
				m_pane_manager.ClosePane(y.m_id);
				return;
			}
		}
	}
}

void main_frame::show_docking_pane_by_id(int id)
{
	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			if (id == y.m_child_window_id)
			{
				m_pane_manager.ShowPane(y.m_id, FALSE);
				return;
			}
		}
	}
}

void main_frame::show_docking_pane_by_class(const CString& class_name)
{
	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			if (0 == y.m_child_window_class.CompareNoCase(class_name))
			{
				m_pane_manager.ShowPane(y.m_id, FALSE);
				return;
			}
		}
	}
}

void main_frame::show_docking_pane_by_identify(const CString& identify)
{
	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			if (0 == y.m_app_identify.CompareNoCase(identify))
			{
				m_pane_manager.ShowPane(y.m_id, FALSE);
				return;
			}
		}
	}
}

void main_frame::reload_docking_pane_by_id(int id, const CString& node_tag)
{
	window_interface* ix = get_docking_pane_window_by_id(id);
	if (ix) ix->load_data_to_ui(node_tag);
}

void main_frame::reload_docking_pane_data_by_identify(const CString& identify, const CString& node_tag)
{
	for (auto& x : m_docking_pane_config.m_container)
		for (single_pane_data& y : x.m_panes)
			if (0 == y.m_app_identify.CompareNoCase(identify))
				reload_docking_pane_by_id(y.m_child_window_id, node_tag);
}

void main_frame::reload_docking_pane_data_by_class(const CString& class_name, const CString& node_tag)
{
	for (auto& x : m_docking_pane_config.m_container)
		for (single_pane_data& y : x.m_panes)
			if (0 == y.m_child_window_class.CompareNoCase(class_name))
				reload_docking_pane_by_id(y.m_child_window_id, node_tag);
}

void main_frame::destroy_all_docking_panes()
{
	// 要在窗口关闭之前保存当前布局和版本
	app_size_config_helper::save_docking_panes_config_version(m_docking_pane_config.m_version);

	if (m_docking_pane_layout)
	{
		m_pane_manager.GetLayout(m_docking_pane_layout);
		m_docking_pane_layout->Save(_T("docking_pane_layout"));
		safe_delete(m_docking_pane_layout); // 关闭前销毁
	}

	// 先清理面板内部的子窗口
	// 清理动态创建的停靠面板窗口
	for (auto& x : m_all_created_panes_child_wnd)
	{
		if (nullptr == x) continue;

		CView* view_wnd = dynamic_cast<CView*>(x);
		CFrameWnd* frame_wnd = dynamic_cast<CFrameWnd*>(x);
		window_interface* wnd_x = dynamic_cast<window_interface*>(x);

		if (wnd_x) wnd_x->pre_close(_T(""));

		if (x->GetSafeHwnd()) x->SendMessage(WM_CLOSE);// x->DestroyWindow();
		if (view_wnd || frame_wnd)
			x = nullptr; // 析构函数已经delete,但没有置为nullptr
		else
			safe_delete(x);
	}
	m_all_created_panes_child_wnd.clear();

	// 再清理面板
	m_pane_manager.DestroyAll();
	m_docking_panes_be_created = false;
}

void main_frame::create_all_docking_panes()
{
	if (!m_docking_panes_be_created) create_docking_panes();
}

void main_frame::get_all_working_space_windows(std::vector<window_interface*>& wnds)
{
	wnds.clear();

	int tab_count = m_workspace_view.get_tab_count();
	for (int i = 0; i < tab_count; ++i)
	{
		window_interface* wnd = dynamic_cast<window_interface*>(m_workspace_view.get_tab_window(i));
		if (wnd) wnds.push_back(wnd);
	}
}

void main_frame::update_all_working_space_window_captions()
{
	if (!GetSafeHwnd() || !m_workspace_view.GetSafeHwnd()) return; // 主窗口创建前也会有消息触发引起标题更新，要作判断

	m_workspace_view.update_tab_caption(-1);
}

void main_frame::get_current_working_space_active_window(std::pair<CString, CString>& info)
{
	int cur_index = m_workspace_view.get_current_tab_index();
	if (-1 == cur_index)
	{
		info = make_pair(_T(""), _T(""));
		return;
	}

	// 0：窗口类名， 1：管理对象tag，2：窗口指针
	tuple<CString, CString, CWnd*> info_detail;
	bool is_ok = m_workspace_view.get_tab_window_and_info(info_detail, cur_index);
	if (!is_ok)
	{
		info = make_pair(_T(""), _T(""));
		return;
	}

	info = make_pair(std::get<0>(info_detail), std::get<1>(info_detail));
}

window_interface* main_frame::get_current_working_space_windows()
{
	int cur_index = m_workspace_view.get_current_tab_index();
	if (-1 == cur_index) return nullptr;

	// 0：窗口类名， 1：管理对象tag，2：窗口指针
	tuple<CString, CString, CWnd*> info_detail;
	bool is_ok = m_workspace_view.get_tab_window_and_info(info_detail, cur_index);
	if (!is_ok) return nullptr;

	return dynamic_cast<window_interface*>(std::get<2>(info_detail));
}

void main_frame::get_all_working_space_windows_info(std::vector<std::pair<CString, CString>>& info)
{
	info.clear();

	int tab_count = m_workspace_view.get_tab_count();
	for (int i = 0; i < tab_count; ++i)
	{
		// 获取指定标签的窗口附加信息 0：窗口类名， 1：管理对象tag，2：窗口指针
		tuple<CString, CString, CWnd*> window_info;
		if (!m_workspace_view.get_tab_window_and_info(window_info, i)) continue;

		info.push_back(make_pair(std::get<0>(window_info), std::get<1>(window_info)));
	}
}

void main_frame::get_all_docking_pane_window(std::vector<window_interface*>& wnds)
{
	wnds.clear();

	CXTPDockingPaneInfoList& pane_list = m_pane_manager.GetPaneList();
	POSITION pos = pane_list.GetHeadPosition();
	while (pos)
	{
		XTP_DOCKINGPANE_INFO& info = pane_list.GetNext(pos);
		CXTPDockingPane* pane = info.pPane;
		if (!pane || pane->IsClosed()) continue;

		CWnd* child = pane->GetChild();
		if (!child || !child->GetSafeHwnd()) continue;

		window_interface* ix = dynamic_cast<window_interface*>(child);
		if (ix) wnds.push_back(ix);
	}
}

void main_frame::get_all_docking_pane_window_by_class(std::vector<window_interface*>& wnds, const CString& class_name)
{
	wnds.clear();

	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			// 为空则不过滤
			if (!class_name.IsEmpty() && 0 != y.m_child_window_class.CompareNoCase(class_name)) continue;

			CXTPDockingPane* pane = m_pane_manager.FindPane(y.m_id);
			if (!pane) continue;;

			CWnd* child = pane->GetChild();
			window_interface* child_interface = dynamic_cast<window_interface*>(child);
			if (child_interface) wnds.push_back(child_interface);
		}
	}
}

window_interface* main_frame::get_docking_pane_window_by_identify(const CString& app_identify)
{
	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			if (0 != y.m_app_identify.CompareNoCase(app_identify)) continue;

			CXTPDockingPane* pane = m_pane_manager.FindPane(y.m_id);
			if (!pane) return (LRESULT)nullptr;

			CWnd* child = pane->GetChild();
			window_interface* child_interface = dynamic_cast<window_interface*>(child);
			if (child_interface) return child_interface;
		}
	}

	return nullptr;
}

void main_frame::save_all_workspace_window_ui_data()
{
	m_workspace_view.save_all_window_data();
}

void main_frame::save_all_docking_pane_ui_data()
{
	CXTPDockingPaneInfoList& pane_list = m_pane_manager.GetPaneList();

	POSITION pos = pane_list.GetHeadPosition();
	while (pos)
	{
		XTP_DOCKINGPANE_INFO& info = pane_list.GetNext(pos);
		CXTPDockingPane* pane = info.pPane;
		if (!pane || pane->IsClosed()) continue;

		CWnd* child = pane->GetChild();
		if (!child || !child->GetSafeHwnd()) continue;

		window_interface* ix = dynamic_cast<window_interface*>(child);
		if (ix) ix->save_data_from_ui();
	}
}

bool main_frame::close_all_workspace_window(bool force_close/* = false*/)
{
	return m_workspace_view.close_all_window(force_close);
}

void main_frame::set_title(const CString& title)
{
	if (!GetSafeHwnd()) return;

	CString app_name = app_configs::m_application_name;
	if (title.IsEmpty())
		SetWindowText(app_name);
	else
		SetWindowText(app_name + _T(" - ") + title);
}

CString main_frame::get_title() const
{
	CString app_name = app_configs::m_application_name;
	app_name += _T(" - ");

	CString title;
	if (GetSafeHwnd()) GetWindowText(title);

	if (0 == app_name.CompareNoCase(title.Left(app_name.GetLength())))
	{
		title.Delete(0, app_name.GetLength());
	}

	return title;
}


void main_frame::update_working_space_tab_caption(int index)
{
	if (index < 0 || index >= get_working_space_view_count()) return;

	tuple<CString, CString, window_interface*> info;
	if (!get_tab_window_and_info(info, index)) return;

	if (nullptr == std::get<2>(info)) return;
	CString new_caption = std::get<2>(info)->get_title();

	m_workspace_view.set_tab_caption(index, new_caption);
}

void main_frame::create_docking_pane_manager()
{
	m_pane_manager.InstallDockingPanes(this);

	m_pane_manager.SetCaptionDirection(xtpPaneCaptionHorizontal);
	m_pane_manager.SetDefaultPaneOptions(0); // 默认状态，无菜单按钮
	m_pane_manager.SetShowContentsWhileDragging(TRUE);
	m_pane_manager.EnableKeyboardNavigate(xtpPaneKeyboardUseCtrlTab);
	m_pane_manager.SetAlphaDockingContext(TRUE);
	m_pane_manager.UseSplitterTracker(TRUE); // m_pane_manager.UseSplitterTracker(FALSE);
	m_pane_manager.SetStickyFloatingFrames(TRUE);
	m_pane_manager.SetThemedFloatingFrames(TRUE);
	m_pane_manager.SetSideDockingMargin(CRect(2, 2, 2, 2));
	m_pane_manager.EnableSideDocking();

	m_pane_manager.GetPaintManager()->GetTabPaintManager()->m_clientFrame = xtpTabFrameSingleLine;
	m_pane_manager.GetPaintManager()->GetTabPaintManager()->m_bStaticFrame = TRUE;
	m_pane_manager.GetPaintManager()->GetTabPaintManager()->m_bShowIcons = FALSE;
	m_pane_manager.GetPaintManager()->GetTabPaintManager()->m_bFillBackground = TRUE;
	m_pane_manager.GetPaintManager()->GetTabPaintManager()->m_bShowTabs = TRUE;
	m_pane_manager.GetPaintManager()->GetTabPaintManager()->m_bBoldSelected = TRUE;

	if (app_configs::is_skin_enable())
	{
		m_pane_manager.SetTheme(get_docking_panes_style());
		m_pane_manager.GetPaintManager()->GetPanelPaintManager()->SetAppearance(xtpTabAppearancePropertyPage2007);
		m_pane_manager.GetPaintManager()->GetPanelPaintManager()->SetColor(xtpTabColorDefault);
	}
	else
	{
		m_pane_manager.SetTheme(xtpPaneThemeWinNative);
		m_pane_manager.GetPaintManager()->GetPanelPaintManager()->SetAppearance(xtpTabAppearancePropertyPage);
		m_pane_manager.GetPaintManager()->GetPanelPaintManager()->SetColor(xtpTabColorWinNative);
	}

	m_pane_manager.GetPaintManager()->GetPanelPaintManager()->SetLayout(xtpTabLayoutSizeToFit);
}

void main_frame::initialize_docking_panes_config()
{
	// 先清空uiData
	m_docking_pane_config.m_container.clear();

	// 开始读取xml配置
	CString xml_file_full_path = file_tools::get_app_path_with_xg() + _T("gui_xml\\app_docking_panes.xml");
	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_SUCCESS != doc.LoadFile(string_tools::CString_to_string(xml_file_full_path).c_str())) return;

	tinyxml2::XMLElement* node = doc.FirstChildElement("app_docking_panes");
	if (!node || node->NoChildren()) return;
	m_docking_pane_config.m_force_create_by_frame = get_xml_node_attribute_bool(node, _T("force_create_by_frame"));
	m_docking_pane_config.m_auto_layout = get_xml_node_attribute_bool(node, _T("auto_layout"));
	m_docking_pane_config.m_version = get_xml_node_attribute_str(node, _T("config_version"));

	m_docking_pane_config.init_docking_pane_data(node);
}

void main_frame::create_docking_panes()
{
	m_docking_panes_be_created = true;

	CString docking_panes_config_version = app_size_config_helper::get_docking_panes_config_version();

	const int pane_wnd_min_id = 10100; // 不要和其它窗口冲突
	const int pane_wnd_max_id = 19100; // 不要和其它窗口冲突
	int pane_wnd_id = pane_wnd_min_id;

	// 1.先分配好各窗口的id和子窗口的id
	for (pane_container_data& container : m_docking_pane_config.m_container)
	{
		for (single_pane_data& pane_data : container.m_panes)
		{
			pane_data.m_id = pane_wnd_id;
			
			// 一般不会用完，为防止用户不断关窗口，不断创建导致少量窗口却占用大量id,因此可以在达到最大值时返回最小值
			++pane_wnd_id;
			if (pane_wnd_id > pane_wnd_max_id) pane_wnd_id = pane_wnd_min_id;


			pane_data.m_child_window_id = pane_wnd_id;
			
			++pane_wnd_id;
			if (pane_wnd_id > pane_wnd_max_id) pane_wnd_id = pane_wnd_min_id;
			++pane_wnd_id;
		}
	}

	safe_delete(m_docking_pane_layout);
	m_docking_pane_layout = m_pane_manager.CreateLayout();

	// 2.如果按布局加载///////////////////////////////////////////////////////////////
	if (m_docking_pane_config.m_version == docking_panes_config_version
		&& m_docking_pane_config.m_auto_layout
		&& m_docking_pane_layout->Load(_T("docking_pane_layout")))
	{
		m_pane_manager.SetLayout(m_docking_pane_layout);
		
		// xtp不负责创建子窗口，要手动处理
		for (pane_container_data& container : m_docking_pane_config.m_container)
		{
			for (single_pane_data& pane_data : container.m_panes)
			{
				CXTPDockingPane* pane_cur = m_pane_manager.FindPane(pane_data.m_id);
				if (!pane_cur)
				{
					app_print_info(MT_ERROR, _T("程序内部错误：m_pane_manager.SetLayout(m_docking_pane_layout)未能正确创建面板"));
					continue;
				}

				if (!create_single_pane_child_window(pane_data, pane_data.m_child_window_id, pane_cur)) continue;
				// reload_docking_pane_by_id(pane_data.m_child_window_id, _T("")); // 停靠窗口目前用不到路径 // 创建函数的内部
			}
		}

		return;
	}

	// 3.如果是按xml创建///////////////////////////////////////////////////////////////

	// 先创建所有的面板，再根据xml的停靠及依赖关系进行布置
	for (pane_container_data& container : m_docking_pane_config.m_container)
	{
		CRect container_rect(0, 0, container.m_width <= 0 ? 300 : container.m_width, container.m_height <= 0 ? 120 : container.m_height);

		CXTPDockingPane* pane_pre = nullptr;
		CXTPDockingPane* pane_select = nullptr;
		for (single_pane_data& pane_data : container.m_panes)
		{			
			CXTPDockingPane* pane_cur = m_pane_manager.CreatePane(pane_data.m_id, container_rect, xtpPaneDockLeft, pane_pre);
			if (!pane_cur) continue;

			// 子窗口创建失败不要continue，继续往后设置面板的相关参数，因为框架不操作子窗口，只操作pane
			// if (!create_single_pane_child_window(pane_data, pane_wnd_id, pane_cur)) continue;
			create_single_pane_child_window(pane_data, pane_wnd_id, pane_cur);

			// 设置面板参数
			pane_cur->SetTitle(pane_data.m_pane_title + _T("\n") + pane_data.m_tab_title);

			DWORD options = 0;
			if (pane_data.m_no_close_button) options |= xtpPaneNoCloseable;
			if (pane_data.m_no_caption) options |= xtpPaneNoCaption;
			if (pane_data.m_no_hideable) options |= xtpPaneNoHideable;
			if (pane_data.m_no_dockable) options |= xtpPaneNoDockable;
			pane_cur->SetOptions(options);
			
			if (pane_pre) m_pane_manager.AttachPane(pane_cur, pane_pre);
			pane_pre = pane_cur;
		}
	}

	auto direction_text_to_pos = [](const CString& direction)
	{
		XTPDockingPaneDirection pos;
		if (0 == direction.CompareNoCase(_T("left")))
			pos = xtpPaneDockLeft;
		else if (0 == direction.CompareNoCase(_T("right")))
			pos = xtpPaneDockRight;
		else if (0 == direction.CompareNoCase(_T("top")))
			pos = xtpPaneDockTop;
		else if (0 == direction.CompareNoCase(_T("bottom")))
			pos = xtpPaneDockBottom;
		else
			pos = xtpPaneDockLeft;

		return pos;
	};

	// 调整各面板的布置位置及选中、是否隐藏等属性
	for (pane_container_data& container : m_docking_pane_config.m_container)
	{
		// 依赖关系最好只有向前依赖，否则结果如何不管
		// 先查找所依赖的面板
		CXTPDockingPane* pane_neighbor = nullptr;
		if (!container.m_neighbor_identify.IsEmpty())
		{
			for (pane_container_data& container_temp : m_docking_pane_config.m_container)
			{
				if (&container_temp == &container) continue; // 不能自己依赖自己
				if (0 != container.m_neighbor_identify.CompareNoCase(container_temp.m_app_identify)) continue;

				for (single_pane_data& pane_data_temp : container_temp.m_panes)
				{
					CXTPDockingPane* pane_cur = m_pane_manager.FindPane(pane_data_temp.m_id);
					if (pane_cur && !pane_cur->IsClosed()) // 关闭的无法依赖
					{
						pane_neighbor = pane_cur;
						break;
					}
				}
				
				if (pane_neighbor) break;
			}
		}

		CXTPDockingPane* pane_pre = nullptr;
		CXTPDockingPane* pane_select = nullptr;

		for (single_pane_data& pane_data : container.m_panes)
		{
			CXTPDockingPane* pane_cur = m_pane_manager.FindPane(pane_data.m_id);
			if (!pane_cur) continue;

			// 一个容器中的所有面板均依次用选项卡方式排在一起
			if (pane_pre)
				m_pane_manager.AttachPane(pane_cur, pane_pre);
			else
				m_pane_manager.DockPane(pane_cur, direction_text_to_pos(container.m_direction_by_frame), pane_neighbor); // 同一容器只有第一个m_start_load为true的选项卡会进入这个分支

			if (!pane_data.m_start_load) // 不加载的也要创建，只不过在必要的时候才显示，否则需要的时候m_pane_manager不知道怎么创建
			{
				m_pane_manager.ClosePane(pane_cur); // 只要不销毁，m_pane_manager就能再次在需要的时候创建出来，并且会在close时的位置创建出来
				// pane_pre = pane_cur; // 关闭会影响附加关系，因此pane_pre只能是上一个没有关闭的面板
			}
			else
			{
				pane_pre = pane_cur; // 被close的不能再用于attach，被hide的可以，因此放在处理完是否显示之后
				if (pane_data.m_select && nullptr == pane_select) pane_select = pane_cur; // 只记录第一个选中的，且show为true的
			}
		}

		if (nullptr == pane_select) pane_select = pane_pre; // 选中最后一个创建的
		if (pane_select) // 指针为空时保持默认行为
		{
			if (container.m_show)
				m_pane_manager.ShowPane(pane_select);
			else
				m_pane_manager.HidePane(pane_select); // 隐藏为一个停靠标签
		}
	}

	// 创建好以后读取当前布局，以便保存用
	m_pane_manager.GetLayout(m_docking_pane_layout);
}

bool main_frame::create_single_pane_child_window(single_pane_data& pane_data, int pane_wnd_id, CXTPDockingPane* pane_cur)
{
	if (!pane_cur) return false;
	if (pane_data.m_child_window_class.IsEmpty()) return true; // 没有子窗口要创建也算是创建成功

	// 要求窗口类必须定义DECLARE_SERIAL宏（可以不用写序列化代码,基类可以不定义序列化宏），而不是定义DECLARE_CREATE，否则创建不成功
	CObject* obj = CRuntimeClass::CreateObject(pane_data.m_child_window_class);
	if (!obj)
	{
		safe_delete(obj);
		return false;
	}

	CWnd* child_wnd = dynamic_cast<CWnd*>(obj);
	if (!child_wnd)
	{
		safe_delete(obj);
		return false;
	}

	m_all_created_panes_child_wnd.push_back(child_wnd);

	child_wnd->Create(nullptr, nullptr, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		CRect(), this, pane_data.m_child_window_id);

	pane_cur->Attach(child_wnd); // 窗口交由xtp管理

	CView* view = dynamic_cast<CView*>(child_wnd);
	if (view) view->OnInitialUpdate();

	reload_docking_pane_by_id(pane_data.m_child_window_id, _T("")); // 停靠窗口目前没有需要路径的情况

	return true;
}

void main_frame::reload_all_docking_panes_data()
{
	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			CXTPDockingPane* pane = m_pane_manager.FindPane(y.m_id);
			if (!pane) continue;

			CWnd* child = pane->GetChild();
			if (!child) continue;

			window_interface* ix = dynamic_cast<window_interface*>(child);
			if (ix) ix->load_data_to_ui(_T("")); // 目前停靠窗口还没有需要路径的情况
		}
	}
}

void main_frame::create_working_space()
{
	// 主客口的客户区子窗口，必须使用特定的窗口类和ID，才能被mfc接管，否则要自己计算中心的窗口位置和大小，并且要自己
	// 响应size消息（不同的主题和界面布局会导致这个位置不一样，比如直接获取主窗口的客户区是包含停靠窗口、工具栏、菜单
	// 栏在内的
	m_workspace_view.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CRect(), this, AFX_IDW_PANE_FIRST);

	return;
}

void main_frame::initialize_theme_style_settings()
{
	m_all_style_settings.clear();
	{
		theme_style_settings style;
		style.m_style_id = ID_THEME_STYLEBLUE2007;
		style.m_style_type = app_theme_type::theme_office_2007_blue;
		style.m_tooltip_style = xtpToolTipOffice2007;
		style.m_docking_pane_theme = xtpPaneThemeResource;
		style.m_tab_appearance = xtpTabAppearanceVisualStudio2010;
		style.m_paint_theme = xtpThemeRibbon;
		style.m_ribbon_system_button_style = xtpButtonAutomatic;
		style.m_theme_dll_ini_config_file_name = _T("OFFICE2007BLUE.ini");
		style.m_enable_aero = false;
		style.m_skin_file_name = _T("Office2007.cjstyles");
		// style.m_skin_file_ini_section_name = _T("NormalBlue.ini");
		style.m_skin_file_ini_section_name = _T("LightBlue.ini");
		
		// 如果使用皮肤，一律使用xtpTabColorDefault由皮肤加载，否则指定具体样式
		style.m_tab_color = xtpTabColorDefault; // xtpTabColorResource;
		style.m_window_back_color = RGB(240, 248, 255);
		
		m_all_style_settings[style.m_style_id] = style;
	}
	{
		theme_style_settings style;
		style.m_style_id = ID_THEME_STYLEBLACK2007;
		style.m_style_type = app_theme_type::theme_office_2007_black;
		style.m_tooltip_style = xtpToolTipOffice2007;
		style.m_docking_pane_theme = xtpPaneThemeResource;
		style.m_tab_appearance = xtpTabAppearanceVisualStudio2010;
		style.m_paint_theme = xtpThemeRibbon;
		style.m_ribbon_system_button_style = xtpButtonAutomatic;
		style.m_theme_dll_ini_config_file_name = _T("OFFICE2007BLACK.ini");
		style.m_enable_aero = false;
		style.m_skin_file_name = _T("Office2007.cjstyles");
		style.m_skin_file_ini_section_name = _T("NormalBlack.ini");

		// 如果使用皮肤，一律使用xtpTabColorDefault由皮肤加载，否则指定具体样式
		style.m_tab_color = xtpTabColorDefault; // xtpTabColorResource;
		style.m_window_back_color = RGB(230, 230, 230);

		m_all_style_settings[style.m_style_id] = style;
	}
	{
		theme_style_settings style;
		style.m_style_id = ID_THEME_STYLESILVER2007;
		style.m_style_type = app_theme_type::theme_office_2007_silver;
		style.m_tooltip_style = xtpToolTipOffice2007;
		style.m_docking_pane_theme = xtpPaneThemeResource;
		style.m_tab_appearance = xtpTabAppearanceVisualStudio2010;
		style.m_paint_theme = xtpThemeRibbon;
		style.m_ribbon_system_button_style = xtpButtonAutomatic;
		style.m_theme_dll_ini_config_file_name = _T("OFFICE2007SILVER.ini");
		style.m_enable_aero = false;
		style.m_skin_file_name = _T("Office2007.cjstyles");
		style.m_skin_file_ini_section_name = _T("NormalSilver.ini");

		// 如果使用皮肤，一律使用xtpTabColorDefault由皮肤加载，否则指定具体样式
		style.m_tab_color = xtpTabColorDefault; // xtpTabColorResource;
		style.m_window_back_color = RGB(250, 250, 250);

		m_all_style_settings[style.m_style_id] = style;
	}
	{
		theme_style_settings style;
		style.m_style_id = ID_THEME_STYLESILVER2010;
		style.m_style_type = app_theme_type::theme_office_2010_silver;
		style.m_tooltip_style = xtpToolTipOffice2007; // xtpToolTipOffice;
		style.m_docking_pane_theme = xtpPaneThemeResource;
		style.m_tab_appearance = xtpTabAppearanceVisualStudio2010;
		style.m_paint_theme = xtpThemeRibbon;
		style.m_ribbon_system_button_style = xtpButtonCaption;
		style.m_theme_dll_ini_config_file_name = _T("OFFICE2010SILVER.ini");
		style.m_enable_aero = true;
		style.m_skin_file_name = _T("Office2010.cjstyles");
		style.m_skin_file_ini_section_name = _T("NormalSilver.ini");

		// 如果使用皮肤，一律使用xtpTabColorDefault由皮肤加载，否则指定具体样式
		style.m_tab_color = xtpTabColorDefault; // xtpTabColorResource;
		style.m_window_back_color = RGB(250, 250, 250);

		m_all_style_settings[style.m_style_id] = style;
	}
	{
		theme_style_settings style;
		style.m_style_id = ID_THEME_STYLEBLUE2010;
		style.m_style_type = app_theme_type::theme_office_2010_blue;
		style.m_tooltip_style = xtpToolTipOffice2007; // xtpToolTipOffice;
		style.m_docking_pane_theme = xtpPaneThemeResource;
		style.m_tab_appearance = xtpTabAppearanceVisualStudio2010;
		style.m_paint_theme = xtpThemeRibbon;
		style.m_ribbon_system_button_style = xtpButtonCaption;
		style.m_theme_dll_ini_config_file_name = _T("OFFICE2010BLUE.ini");
		style.m_enable_aero = true;
		style.m_skin_file_name = _T("Office2010.cjstyles");
		style.m_skin_file_ini_section_name = _T("NormalBlue.ini");

		// 如果使用皮肤，一律使用xtpTabColorDefault由皮肤加载，否则指定具体样式
		style.m_tab_color = xtpTabColorDefault; // xtpTabColorResource;
		style.m_window_back_color = RGB(240, 248, 255);

		m_all_style_settings[style.m_style_id] = style;
	}
	{
		theme_style_settings style;
		style.m_style_id = ID_THEME_STYLEBLACK2010;
		style.m_style_type = app_theme_type::theme_office_2010_black;
		style.m_tooltip_style = xtpToolTipOffice2007; // xtpToolTipOffice;
		style.m_docking_pane_theme = xtpPaneThemeResource;
		style.m_tab_appearance = xtpTabAppearanceVisualStudio2010;
		style.m_paint_theme = xtpThemeRibbon;
		style.m_ribbon_system_button_style = xtpButtonCaption;
		style.m_theme_dll_ini_config_file_name = _T("OFFICE2010BLACK.ini");
		style.m_enable_aero = true;
		style.m_skin_file_name = _T("Office2010.cjstyles");
		style.m_skin_file_ini_section_name = _T("NormalBlack.ini");

		// 如果使用皮肤，一律使用xtpTabColorDefault由皮肤加载，否则指定具体样式
		style.m_tab_color = xtpTabColorDefault; // xtpTabColorResource;
		style.m_window_back_color = RGB(230, 230, 230);

		m_all_style_settings[style.m_style_id] = style;
	}
	{
		theme_style_settings style;
		style.m_style_id = ID_THEME_STYLE_LIGHT2013;
		style.m_style_type = app_theme_type::theme_office_2013_white;
		style.m_tooltip_style = xtpToolTipOffice2013;
		style.m_docking_pane_theme = xtpPaneThemeOffice2013;
		style.m_tab_appearance = xtpTabAppearanceOffice2013;
		style.m_paint_theme = xtpThemeOffice2013;
		style.m_ribbon_system_button_style = xtpButtonCaption;
		style.m_theme_dll_ini_config_file_name = _T("Office2013Word.ini"); // _T("Office2013WordGrayLight.ini");
		style.m_enable_aero = false;
		style.m_skin_file_name = _T("Office2013.cjstyles");
		style.m_skin_file_ini_section_name = _T("NormalWhite.ini");
		
		// 如果使用皮肤，一律使用xtpTabColorDefault由皮肤加载，否则指定具体样式
		style.m_tab_color = xtpTabColorDefault; // xtpTabColorOffice2013;
		style.m_window_back_color = RGB(250, 250, 250);

		m_all_style_settings[style.m_style_id] = style;
	}
	{
		theme_style_settings style;
		style.m_style_id = ID_THEME_STYLE_DARK2013;
		style.m_style_type = app_theme_type::theme_office_2013_dark;
		style.m_tooltip_style = xtpToolTipOffice2013;
		style.m_docking_pane_theme = xtpPaneThemeOffice2013;
		style.m_tab_appearance = xtpTabAppearanceOffice2013;
		style.m_paint_theme = xtpThemeOffice2013;
		style.m_ribbon_system_button_style = xtpButtonCaption;
		style.m_theme_dll_ini_config_file_name = _T("OFFICE2013WORDGRAYDARK.ini");
		style.m_enable_aero = false;
		style.m_skin_file_name = _T("Office2013.cjstyles");
		style.m_skin_file_ini_section_name = _T("NormalDark.ini");
		
		// 如果使用皮肤，一律使用xtpTabColorDefault由皮肤加载，否则指定具体样式
		style.m_tab_color = xtpTabColorDefault; // xtpTabColorOffice2013;
		style.m_window_back_color = RGB(240, 240, 240);

		m_all_style_settings[style.m_style_id] = style;
	}
}

theme_style_settings main_frame::get_style_and_settings() const
{
	auto it = m_all_style_settings.find(m_current_theme);
	if (it == m_all_style_settings.end())
		return app_ui_interface::get_style_and_settings();
	else
		return it->second;
}

void main_frame::on_project_data_notify(const CString& node_tag, notifer_action ac)
{
}

bool main_frame::is_style_changing() const
{
	// 务必支持多线程，否则OSG渲染线程和主进程同时访问会有问题
	return m_is_style_changing.load();
}

void show_or_hide_window_tree(CWnd* top_wnd, CWnd* bottom_wnd, bool show /*= false*/)
{
	if (!bottom_wnd) return;
	if (!top_wnd) return;
	if (bottom_wnd->GetSafeHwnd() == GetDesktopWindow()) return;
	if (top_wnd == bottom_wnd) return;

	if (!bottom_wnd->GetSafeHwnd()) return;
	if (show)
		bottom_wnd->ShowWindow(SW_SHOW);
	else
		bottom_wnd->ShowWindow(SW_HIDE);

	show_or_hide_window_tree(top_wnd, bottom_wnd->GetParent(), show);
}

class select_flage_setting_raii_helper
{
public:
	select_flage_setting_raii_helper(std::atomic<bool>& select_flag)
		: m_select_flag(select_flag)
	{
		m_select_flag = true;
	}

	~select_flage_setting_raii_helper()
	{
		m_select_flag = false;
	}

private:
	std::atomic<bool>& m_select_flag;
};

void main_frame::on_command_skin_and_theme_menu_item(UINT id)
{
	if (!app_configs::is_skin_enable()) return;

	m_is_style_changing.store(true);
	m_current_theme = id;

	set_grid_color();
	set_skin_style();
	set_frame_theme();
	redraw_frame();

	// 主框架完成后通知所有的wwindow_interface主题已经改变
	theme_style_settings setting = get_style_and_settings();
	set<window_interface*> all_wnds = window_interface::get_all_window_instances();
	for (window_interface* x : all_wnds)
		if (x) x->app_theme_changed(setting);

	// 保存当前主题以便下次启动时原样显示
	main_application* app = (main_application*) AfxGetApp();
	if (app) app->WriteProfileInt(_T("Skin"), _T("ThemeType"), m_current_theme);

	m_is_style_changing.store(false);
}

void main_frame::set_frame_theme()
{
	CXTPCommandBars* command_bars = GetCommandBars();
	if (!command_bars) return;

	app_ribbon_bar* ribbon_bar = DYNAMIC_DOWNCAST(app_ribbon_bar, command_bars->GetMenuBar());
	if (!ribbon_bar) return;

	theme_style_settings setting = get_style_and_settings();
	ribbon_bar->GetSystemButton()->SetStyle(setting.m_ribbon_system_button_style);

	if (app_configs::is_skin_enable())
	{
		CXTPToolTipContext* tool_tip_context = command_bars->GetToolTipContext();
		if (tool_tip_context) tool_tip_context->SetStyle(get_tooltip_style());

		// 有aero效果要先禁用框架主题，后面再根据需要再调一次启动或禁用，原因是xtp的acro效果标记变量是静态变量，且只
		// 会在启用主题的时候才会用于初始化内部状态（这是demo的正宗用法），此处为提高效率，减少切换时的大量重绘带来的
		// 闪烁和违和感，优化为只在aero状态变化时才调用
		BOOL old_bAllowDwm = m_bAllowDwm;
		m_bAllowDwm = (setting.m_enable_aero ? TRUE : FALSE);
		if (old_bAllowDwm != m_bAllowDwm) ribbon_bar->EnableFrameTheme(FALSE);

		XTPSetOfficeSetBackImage(GetCommandBars(), XTPSpring); // 只有office 2013主题才生效
		ribbon_bar->EnableFrameTheme(TRUE);
		RefreshFrameStyle();
	}
	else
	{
		CXTPToolTipContext* tool_tip_context = command_bars->GetToolTipContext();
		if (tool_tip_context) tool_tip_context->SetStyle(xtpToolTipStandard);

		ribbon_bar->EnableFrameTheme(FALSE);
		XTPSetOfficeSetBackImage(GetCommandBars(), XTPNone); // 只有office 2013主题才生效
		ribbon_bar->EnableFrameTheme(FALSE);
		RefreshFrameStyle();
	}

	// 主窗口处理主题会统一调用，此处不要单独调
	//command_bars->GetPaintManager()->RefreshMetrics();
	//command_bars->RedrawCommandBars();

	//SendMessage(WM_NCPAINT);
	//RedrawWindow(0, 0, RDW_ALLCHILDREN | RDW_INVALIDATE);
}

void main_frame::set_skin_style()
{
	CXTPSkinManager* skin_manager = XTPSkinManager();
	if (!skin_manager) return;

	if (!app_configs::is_skin_enable())
	{
		skin_manager->LoadSkin(nullptr, nullptr); // 不使用皮肤
		CXTPPaintManager::SetTheme(xtpThemeNone);
		return;
	}

	theme_style_settings setting = get_style_and_settings();

	// 不要手工处理，xtp库已经在后台能自动加载其自带的资源dll了，除非用的不是自带的
	//////////////////////////////////////////////////////////////////////////
	// HMODULE handle = AfxGetResourceHandle(); // 如果资源不存在，则加载mfc的
	// HMODULE handle = ::LoadLibrary(file_tools::get_app_path_with_xg() + _T("styles\\Office2007.dll"));
	// HMODULE handle = ::LoadLibrary(file_tools::get_app_path_with_xg() + _T("styles\\Office2010.dll"));
	// if (handle) ((CXTPResourceTheme*) GetCommandBars()->GetPaintManager())->SetImageHandle(handle, setting.m_paint_theme_ini_section_name);
	//////////////////////////////////////////////////////////////////////////

	// xtp库有bug，菜单也使用皮肤后会导致打开对话框中右键菜单显示异常，暂时没有时间调试这个问题，先注掉
	skin_manager->SetApplyOptions(xtpSkinApplyMetrics | xtpSkinApplyFrame | xtpSkinApplyColors/* | xtpSkinApplyMenus*/);
	if (setting.m_skin_file_name.IsEmpty())
		skin_manager->LoadSkin(nullptr, nullptr); // 不使用皮肤
	else
		skin_manager->LoadSkin(file_tools::get_app_path_with_xg() + _T("styles\\") + setting.m_skin_file_name, setting.m_skin_file_ini_section_name);
	
	XTPThemeDLL()->SetHandle(setting.m_theme_dll_ini_config_file_name);
	if (app_configs::is_skin_enable())
		CXTPPaintManager::SetTheme(setting.m_paint_theme);
	else
		CXTPPaintManager::SetTheme(xtpThemeNone);
}

void main_frame::set_grid_color()
{
	theme_style_settings setting = get_style_and_settings();

	if (app_theme_type::theme_office_2007_blue == setting.m_style_type
		|| app_theme_type::theme_office_2010_blue == setting.m_style_type)
	{
		CGridGlobalData::SetFilePathName(file_tools::get_app_path_with_xg() + _T("styles\\blue_grid_settings.ini"), true);
		CGridGlobalData::Refresh();
	}
	else if (app_theme_type::theme_office_2007_black == setting.m_style_type
		|| app_theme_type::theme_office_2010_black == setting.m_style_type)
	{
		CGridGlobalData::SetFilePathName(file_tools::get_app_path_with_xg() + _T("styles\\black_grid_settings.ini"), true);
		CGridGlobalData::Refresh();
	}
	else if (app_theme_type::theme_office_2007_silver == setting.m_style_type
		|| app_theme_type::theme_office_2010_silver == setting.m_style_type
		|| app_theme_type::theme_office_2013_white == setting.m_style_type)
	{
		CGridGlobalData::SetFilePathName(file_tools::get_app_path_with_xg() + _T("styles\\white_grid_settings.ini"), true);
		CGridGlobalData::Refresh();
	}
	else if (app_theme_type::theme_office_2013_dark == setting.m_style_type)
	{
		CGridGlobalData::SetFilePathName(file_tools::get_app_path_with_xg() + _T("styles\\dark_grid_settings.ini"), true);
		CGridGlobalData::Refresh();
	}
	else
	{
		// nothing;
	}
}

int main_frame::add_working_space_view(const CString window_class, const CString node_tag, int index, int image_index)
{
	// 由于项目保存的时候会把打开的工作区窗口也保存下来，经常在管理类或工作区窗口类的类名变化时，导致打开旧项目的
	// 窗口被实例化出来的对象是损坏的，如果把这种窗口也恢复显示出来会导致界面崩溃，因此，对于这种界面数据已经有问
	// 题的项目，提供了一种方式：即如果窗口打开时按住了Ctrl键，则放弃将此窗口打开并添加到工作区，为了防止当用户误
	// 操作，因此按住Ctrl时给出提示以便选择
	// 按下Ctrl打开工作区会被认为是修复模式
	if (0xff == HIBYTE(GetKeyState(VK_CONTROL))) // 高位全1表示按下，否则没按
	{
		if (IDYES == AfxMessageBox(_T("修复模式下将放弃打开工作区窗口操作，确定按修复模式处理吗？"), MB_YESNO | MB_DEFBUTTON1 | MB_ICONWARNING))
			return -1;
	}

	// 添加工作区窗口可能会根据需要来判断是否要关掉一些工作区窗口（如超过了最大窗口数），此时关掉的窗口会调用相应的
	// 保存功能，由于保存时可能触发数据中心的数据响应并间接导致树控件的节点销毁及重加载，销毁时这前已经送过来的消息
	// 参数已经失效析构了，因此参数如果使用引用则对象也无效了，所以函数原型的参数类型不得使用引用

	if (!m_workspace_view.GetSafeHwnd()) return -1;

	//modify cg 2015-10-16
	int flag = m_workspace_view.add_tab_window_and_info(window_class, node_tag, index, image_index);
	if (flag != -1)
	{
		// 框架不要干涉焦点的事情，否则有些子窗口的焦点将被框架干扰而不正常
		//db_command_line* pComdLine = dynamic_cast<db_command_line*>(GetCommandLineInterface());
		//if (pComdLine) pComdLine->SetFocus();
	}

	return flag;
}

int main_frame::add_working_space_view(window_interface* wnd, const CString node_tag, int index /*= -1*/, int image_index /*= -1*/)
{
	if (!wnd) return -1;

	return m_workspace_view.add_tab_working_space_view(wnd, node_tag, index, image_index);
}

window_interface* main_frame::get_docking_pane_window_by_id(int id)
{
	for (auto& x : m_docking_pane_config.m_container)
	{
		for (single_pane_data& y : x.m_panes)
		{
			if (id == y.m_child_window_id)
			{
				CXTPDockingPane* pane = m_pane_manager.FindPane(y.m_id);
				if (!pane) continue;

				CWnd* child = pane->GetChild();
				if (!child) continue;

				return dynamic_cast<window_interface*>(child);
			}
		}
	}

	return nullptr;
}

int main_frame::get_docking_pane_window_id(window_interface* wnd)
{
	CWnd* wnd_object = dynamic_cast<CWnd*>(wnd);
	if (!wnd_object || !wnd_object->GetSafeHwnd()) return 0;

	// 非对话框也可以用这个成员得到id值
	return wnd_object->GetDlgCtrlID();
}

void main_frame::redraw_frame()
{
	if (!GetSafeHwnd()) return;

	CXTPCommandBars* bars = GetCommandBars();
	if (!bars) return;

	bars->GetPaintManager()->RefreshMetrics();

	// 启用DPI自适应模型，在高分屏模式下图标自动绽放
	bars->GetPaintManager()->m_bAutoResizeIcons = TRUE;
	bars->GetCommandBarsOptions()->SetDPIScallingOptions(TRUE);

	bars->GetImageManager()->RefreshAll();
	bars->RedrawCommandBars();
	SendMessage(WM_NCPAINT);

	// 状态栏窗口重绘
	//if (m_status_bar.GetSafeHwnd()) m_status_bar.Invalidate();

	// 停靠窗口重绘
	if (m_pane_manager.GetSafeHwnd())
	{
		app_ui_interface* app = get_app_ui_interface();
		if (!app) return;

		theme_style_settings theme = app->get_style_and_settings();

		m_pane_manager.SetTheme(theme.m_docking_pane_theme);
		m_pane_manager.GetPaintManager()->GetPanelPaintManager()->SetAppearance(theme.m_tab_appearance);
		m_pane_manager.GetPaintManager()->GetPanelPaintManager()->SetLayout(xtpTabLayoutAutoSize);
		m_pane_manager.GetPaintManager()->GetPanelPaintManager()->SetColor(theme.m_tab_color);

		m_pane_manager.GetPaintManager()->RefreshMetrics();
		m_pane_manager.UpdateWindow(); // 停靠面板框架重绘
		m_pane_manager.RedrawPanes(); // 所有被管理的停靠面板窗口重绘
	}

	RedrawWindow(0, 0, RDW_ALLCHILDREN | RDW_INVALIDATE);
}

app_ribbon_bar* main_frame::get_db_ribbon_bar()
{
	CXTPCommandBars* pBars = GetCommandBars();
	if (!pBars) return NULL;

	for (int i = 0; i < pBars->GetCount(); ++i)
	{
		CXTPCommandBar* tmp = pBars->GetAt(i);
		if (0 == tmp->GetTitle().CompareNoCase("app_ribbon_bar")) // 名称由创建时指定
			return dynamic_cast<app_ribbon_bar*>(tmp);
	}

	return nullptr;
}

void main_frame::on_update_skin_and_theme_menu_item(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_current_theme == (int) pCmdUI->m_nID);
}

LRESULT main_frame::on_docking_pane_notify(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

void main_frame::on_recent_file_list(UINT id)
{
	int index = (int)(id - ID_FILE_MRU_FILE1);
	CString full_path = the_main_app.get_recent_list_item_full_path(index);
	if (full_path.IsEmpty()) return;

	cmd_trigger_manager::fire(_T("app"), _T("open"), full_path);
}

void main_frame::on_all_ribbon_tab_command(UINT id)
{
	// Ribbon中可在xml文档中配置以及采用自定义消息响应的功能在此函数中完成，文件菜单和快速访问栏中
	// 的功能则使用MFC的消息响应来完成
	app_ribbon_bar* rib = get_db_ribbon_bar();
	if (!rib) return;
	if (!rib->GetSafeHwnd()) return;

	array<CString, 3> cmd_info;
	if (m_ribbon_config.get_command_and_parameter(id, cmd_info))
		cmd_trigger_manager::fire(cmd_info[0], cmd_info[1], cmd_info[2]);
}

// 根据一个CWnd得到它的window_interface指针，如果窗口不是window_interface派生类，则往窗口树向上找最近的一个，如果找不到返回nullptr
window_interface* get_window_interface_by_CWnd(CWnd* wnd)
{
	if (!wnd) return nullptr;

	window_interface* pInterface = dynamic_cast<window_interface*>(wnd);
	HWND desk = GetDesktopWindow();
	while (!pInterface)
	{
		wnd = wnd->GetParent();
		if (wnd->GetSafeHwnd() == desk) break;;

		pInterface = dynamic_cast<window_interface*>(wnd);
	}

	return pInterface;
}

void main_frame::on_update_all_ribbon_tab_command(CCmdUI *pCmdUI)
{
	if (!m_workspace_view.GetSafeHwnd())
	{
		pCmdUI->Enable(FALSE);
		// pCmdUI->SetCheck(0); // Enable为false时界面库有bug，会强制改为UnCheck，如果设置选中则会闪烁
		return;
	}

	app_ribbon_bar* pRibbonBar = get_db_ribbon_bar();
	if (!pRibbonBar || !pRibbonBar->GetSafeHwnd())
	{
		pCmdUI->Enable(FALSE);
		// pCmdUI->SetCheck(0); // Enable为false时界面库有bug，会强制改为UnCheck，如果设置选中则会闪烁
		return;
	}

	if (IDS_RIBBON_OPTIONS_MINI_MODE == pCmdUI->m_nID)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
		return;
	}

	// 先得到相应的命令信息
	std::array<CString, 3> cmd_info;
	if (!m_ribbon_config.get_command_and_parameter(pCmdUI->m_nID, cmd_info)) return;

	CString domain = cmd_info[0];
	CString cmd = cmd_info[1];

	// 没有在xml配置的ribbon命令有自己专门的响应函数，不进入下面可配置控件的处理流程，以免干扰
	if (cmd.IsEmpty())
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
		return;
	}

	// 如果当前活动窗口无法处理或未处理，则转到命令处理类进行处理
	bool is_enable = false, is_checked = false, is_transparent = false;
	cmd_trigger_manager::get_command_state(domain, cmd, is_enable, is_checked, is_transparent);
	pCmdUI->Enable(is_enable ? TRUE : FALSE);

	// Enable为false时界面库有bug，会强制改为UnCheck，如果设置选中则会闪烁
	if (is_enable)
		pCmdUI->SetCheck(is_checked ? 1 : 0);
	else
		pCmdUI->SetCheck(0);
}

void main_frame::on_all_accelerator_command(UINT id)
{
	CString cmd = m_accelerator_map.get_accelerator_command_by_id(id);
	cmd.Trim();
	if (cmd.IsEmpty()) return;

	CString cmd_domain, cmd_text;
	cmd_container_interface::parse_and_fill_command(cmd, cmd_domain, cmd_text);
	if (cmd_text.IsEmpty()) return;

	cmd_trigger_manager::fire(cmd_domain, cmd_text, _T("")); // 加速键启动的命令是没有路径的
}

void main_frame::on_update_all_accelerator_command(CCmdUI *pCmdUI)
{
	// 要根据命令是否是Enable为true才调整，否则不调整
	// ...
	pCmdUI->Enable(TRUE);
}

LRESULT main_frame::on_msg_do_functor(WPARAM wparam, LPARAM lparam)
{
	functor_by_message* fun = (functor_by_message*) wparam;
	if (!fun) return FALSE;

	fun->call();
	safe_delete(fun);

	return TRUE;
}

void main_frame::on_all_ribbon_quick_access_command(UINT id)
{
	// Ribbon中可在xml文档中配置以及采用自定义消息响应的功能在此函数中完成，文件菜单和快速访问栏中
	// 的功能则使用MFC的消息响应来完成

	ribbon_quick_access_bar& quick_access_bar = m_ribbon_config.m_quick_access;
	array<CString, 3> cmd_info;
	if (quick_access_bar.get_command_and_parameter(id, cmd_info))
		cmd_trigger_manager::fire(cmd_info[0], cmd_info[1], cmd_info[2]);
}

void main_frame::on_update_all_ribbon_quick_access_command(CCmdUI *pCmdUI)
{
	if (!m_workspace_view.GetSafeHwnd())
	{
		pCmdUI->Enable(FALSE);
		// pCmdUI->SetCheck(0); // Enable为false时界面库有bug，会强制改为UnCheck，如果设置选中则会闪烁
		return;
	}

	app_ribbon_bar* pRibbonBar = get_db_ribbon_bar();
	if (!pRibbonBar || !pRibbonBar->GetSafeHwnd())
	{
		pCmdUI->Enable(FALSE);
		// pCmdUI->SetCheck(0); // Enable为false时界面库有bug，会强制改为UnCheck，如果设置选中则会闪烁
		return;
	}

	// 先得到相应的命令信息
	std::array<CString, 3> cmd_info;
	ribbon_quick_access_bar quick_access_bar = m_ribbon_config.m_quick_access;
	if (!quick_access_bar.get_command_and_parameter(pCmdUI->m_nID, cmd_info)) return;

	CString domain = cmd_info[0];
	CString cmd = cmd_info[1];

	// 没有在xml配置的ribbon命令有自己专门的响应函数，不进入下面可配置控件的处理流程，以免干扰
	if (cmd.IsEmpty())
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
		return;
	}

	// 如果当前活动窗口无法处理或未处理，则转到命令处理类进行处理
	bool is_enable = false, is_checked = false, is_transparent = false;
	cmd_trigger_manager::get_command_state(domain, cmd, is_enable, is_checked, is_transparent);
	pCmdUI->Enable(is_enable ? TRUE : FALSE);

	// Enable为false时界面库有bug，会强制改为UnCheck，如果设置选中则会闪烁
	if (is_enable)
		pCmdUI->SetCheck(is_checked ? 1 : 0);
	else
		pCmdUI->SetCheck(0);
}

void main_frame::on_all_ribbon_file_menu_command(UINT id)
{
	ribbon_file_menu_bar& file_menu_bar = m_ribbon_config.m_file_menu;
	array<CString, 3> cmd_info;
	if (file_menu_bar.get_command_and_parameter(id, cmd_info))
		cmd_trigger_manager::fire(cmd_info[0], cmd_info[1], cmd_info[2]);
}

void main_frame::on_update_all_ribbon_file_menu_command(CCmdUI *pCmdUI)
{
	if (!m_workspace_view.GetSafeHwnd())
	{
		pCmdUI->Enable(FALSE);
		// pCmdUI->SetCheck(0); // Enable为false时界面库有bug，会强制改为UnCheck，如果设置选中则会闪烁
		return;
	}

	app_ribbon_bar* pRibbonBar = get_db_ribbon_bar();
	if (!pRibbonBar || !pRibbonBar->GetSafeHwnd())
	{
		pCmdUI->Enable(FALSE);
		// pCmdUI->SetCheck(0); // Enable为false时界面库有bug，会强制改为UnCheck，如果设置选中则会闪烁
		return;
	}

	// 先得到相应的命令信息
	std::array<CString, 3> cmd_info;
	ribbon_file_menu_bar file_menu_bar = m_ribbon_config.m_file_menu;
	if (!file_menu_bar.get_command_and_parameter(pCmdUI->m_nID, cmd_info)) return;

	CString domain = cmd_info[0];
	CString cmd = cmd_info[1];

	// 没有在xml配置的ribbon命令有自己专门的响应函数，不进入下面可配置控件的处理流程，以免干扰
	if (cmd.IsEmpty())
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
		return;
	}

	// 如果当前活动窗口无法处理或未处理，则转到命令处理类进行处理
	bool is_enable = false, is_checked = false, is_transparent = false;
	cmd_trigger_manager::get_command_state(domain, cmd, is_enable, is_checked, is_transparent);
	pCmdUI->Enable(is_enable ? TRUE : FALSE);

	// Enable为false时界面库有bug，会强制改为UnCheck，如果设置选中则会闪烁
	if (is_enable)
		pCmdUI->SetCheck(is_checked ? 1 : 0);
	else
		pCmdUI->SetCheck(0);
}

void main_frame::on_all_ribbon_options_command(UINT id)
{
	// Ribbon中可在xml文档中配置以及采用自定义消息响应的功能在此函数中完成，文件菜单和快速访问栏中
	// 的功能则使用MFC的消息响应来完成

	app_ribbon_bar* pRibbonBar = get_db_ribbon_bar();
	if (!pRibbonBar) return;
	if (!pRibbonBar->GetSafeHwnd()) return;

	if (id == IDS_RIBBON_OPTIONS_MINI_MODE)
	{
		m_is_ribbon_mini_size = !(pRibbonBar->IsRibbonMinimized()); // 设置为相反的状态
		pRibbonBar->SetRibbonMinimized(m_is_ribbon_mini_size);
	}
	else if(id == IDS_RIBBON_OPTIONS_ABOUT)
	{
		CObject* dialog_obj = CRuntimeClass::CreateObject(m_ribbon_config.m_dialog_info.m_about_dialog_class_name);
		CDialog* about_dialog = dynamic_cast<CDialog*>(dialog_obj);
		if (about_dialog) about_dialog->DoModal();
		safe_delete(dialog_obj);
	}
	else
	{
		/*nothing*/
	}
}

void main_frame::on_update_all_ribbon_options_command(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void main_frame::on_update_ribbon_exit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void main_frame::on_update_ribbon_recentdocs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

BOOL main_frame::OnHelpInfo(HELPINFO* pHelpInfo)
{
	pair<CString, CString> info; // first为窗口类，second为管理节点的路径
	get_current_working_space_active_window(info);

	CString page_full_path = help_configs::get_default_page_full_path();

	if (!info.first.IsEmpty())
	{
		page_full_path = help_configs::get_page_full_path_by_window_class_name(info.first);
	}

	// 空字符串不要调用，会显示为空白窗口
	if (!page_full_path.IsEmpty())
		::HtmlHelp(GetSafeHwnd(), page_full_path, HH_DISPLAY_TOPIC, 0);
	else
		::HtmlHelp(GetSafeHwnd(), help_configs::get_default_page_full_path(), HH_DISPLAY_TOPIC, 0);

	return CXTPFrameWnd::OnHelpInfo(pHelpInfo);
}
