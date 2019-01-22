#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"

#include <memory>

#include "x_framework/macro.h"
#include "x_framework/interface/console_interface.h"
#include "x_framework/tools/file_tools.h"
#include "x_framework/tools/auto_load_module.h"
#include "x_framework/tools/table_templates.h"
#include "x_framework/interface/cmd_container_interface.h"

#include "splash_window.h"
#include "mini_dump.h"
#include "main_frame.h"
#include "help_configs.h"
#include "app_size_config_helper.h"
#include "app_configs.h"

#include "main_application.h"


using namespace Gdiplus;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////////

// 利用全局变量实现转储功能的优先加载（放在the_main_app前），使用静态变量的原因是将变量作用域限制在当前文件中
static bool the_auto_enable_dumper = (mini_dump_helper::enable_dump(true), true);

// 唯一的一个 main_application 对象
main_application the_main_app;

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class about_dialog : public CDialogEx
{
	DECLARE_MESSAGE_MAP()

public:
	about_dialog();

public:
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
};


BEGIN_MESSAGE_MAP(about_dialog, CDialogEx)
END_MESSAGE_MAP()

about_dialog::about_dialog() : CDialogEx(about_dialog::IDD)
{
}

void about_dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

//////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(main_application, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, on_app_about)
END_MESSAGE_MAP()


main_application::main_application()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = NULL; // AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	// 为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Tonghao.main_application.v2"));

	m_gdiplusToken = NULL;
}

main_application::~main_application()
{
}

bool register_window_class_by_name(const CString& class_name)
{
	WNDCLASS wndcls;
	memset(&wndcls, 0, sizeof(WNDCLASS));

	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = nullptr;
	wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = class_name;

	if (!AfxRegisterClass(&wndcls)) return false;

	return true;
}

BOOL main_application::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要使用 ComCtl32.dll 版本 6 或更高版本来启用可
	// 视化方式，则需要 InitCommonControlsEx()。  否则将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES; // 将它设置为包括所有要在应用程序中使用的公共控件类。
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableTaskbarInteraction(FALSE);
	InitTooltipManager();

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	AfxInitRichEdit2();

	// 尽量提前加载一下主程序相关配置，并确保mfc正确初始化
	app_configs::load_settings_from_xml();
	
	// 更改用于存储设置的注册表项,如果未使用这些功能并希望减小,最终可执行文件的大小，则应移除
	SetRegistryKey(app_configs::m_application_name);
	// LoadStdProfileSettings(4);  // Load standard INI file options (including MRU) // 已由框架接管处理，不再需要

	// 更改CWinAppEx::m_pszAppName的值就可以更改AfxMessageBox()的标题栏
	set_application_name(app_configs::m_application_name);

	// LoadAccelerators(); // 已由框架接管处理，不再需要

	if (app_configs::m_only_one_instance)
	{
		m_single_instance_mutex = ::CreateMutex(NULL, FALSE, app_configs::m_application_name);
		if (ERROR_ALREADY_EXISTS == GetLastError())
		{
			AfxMessageBox(_T("程序已经运行!"), MB_OK | MB_ICONWARNING);
			return FALSE;
		}
	}

	// 窗口创建动作必须放在EnableTaskbarInteraction之后，之所以要注册窗口是因为常规窗口要创建的话必须要有WS_CHILD风格，
	// 这个风格要求指定父窗口（指定为桌面会有异常），如果改为模态对话框则可以正常显示，但会阻塞主窗口，因此要注册一个新的窗口类
	// 来专门处理splash
	CWaitCursor tmp_wait_cursor;
	std::unique_ptr<splash_window> wnd(nullptr);
	if (app_configs::m_enable_splash && register_window_class_by_name(_T("app_splash")))
	{
		wnd.reset(new splash_window()); // 指针析构时会自动销毁窗口
		wnd->Create();
	}

	// 加载历史列表
	load_recent_list_to_mfc();

	// 加载求助配置信息
	help_configs::initialize_by_xml();

	// 尽量在前期准备工作完成后调整
	// 命令返回true表示终止程序创建（会按正常的退出流程处理，不要强制使用exit退出，否则不会调用程序实例退
	// 出的代码，导致一些特殊资源不能被正确释放，如com的资源计数等
	if (true == cmd_trigger_manager::fire(_T("sys"), _T("pre_create_main_window"), _T(""))) return FALSE;

	// 若要创建主窗口，此代码将创建新的框架窗口对象，然后将其设置为应用程序的主窗口对象
	main_frame* frame = new main_frame();
	if (!frame) return FALSE;

	frame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr, nullptr);
	frame->ShowWindow(SW_HIDE);
	m_pMainWnd = frame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	CString param = cmdInfo.m_strFileName;
	param.Trim();
	// 有文件要自动打开时先处理停靠窗口的创建工作
	if (!param.IsEmpty()) frame->create_docking_panes();
	cmd_trigger_manager::fire(_T("sys"), _T("post_create_main_window_pre_open_file"), _T("")); // 放到停靠面板创建后触发

	// 如果命令以"|"开头，则表示自定义的参数，不按照操作系统的常规行为处理
	if (_T("|") == param.Left(1))
	{
		CString custom_param = param.Mid(1); // 把开头的一个"|"去掉（如果还有则认为是用户数据）
		cmd_trigger_manager::fire(_T("sys"), _T("custom_command_when_start"), custom_param);
	}
	else if (!param.IsEmpty())
	{
		// 处理文件打开操作，通过命令行而不是命令管理中心调用可以记用户从界面上看到进行的操作输出并记录到日志，而且不会触发相应的notify
		cmd_trigger_manager::fire(_T("app"), _T("open"), param); // 要不要打开起始页由app.open命令内部决定处理
	}
	else
	{
		if (app_configs::m_show_start_page)
			cmd_trigger_manager::fire(_T("sys"), _T("open_start_page"));
	}

	// splash窗口会自动析构，但析构时可能要数秒的时间，这时窗口在销毁，分层窗口的透明处会失效变为黑的，因此在窗口析构
	// 前先把窗口隐藏就看不到这种不和谐的样子了
	if (app_configs::m_enable_splash && wnd) wnd->ShowWindow(SW_HIDE);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	// if (!ProcessShellCommand(cmdInfo)) return FALSE; // 所有的文档管理mfc已经没有意义了，因为桥博没有用mfc的文档视图模型

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	frame->MoveWindow(app_size_config_helper::get_last_main_frame_pos_and_size());
	frame->redraw_frame(); // 尽量放在后面，以及某些窗口创建过晚导致主题绘制不全（注意不要放在显示前，否则多次绘制可能会闪）
	frame->ShowWindow(app_size_config_helper::get_maximize_flag() ? SW_MAXIMIZE : SW_NORMAL);
	frame->UpdateWindow();

	cmd_trigger_manager::fire(_T("sys"), _T("post_create_main_window_post_open_file"), _T(""));

	return TRUE;
}

int main_application::ExitInstance()
{
	if (m_single_instance_mutex)
		CloseHandle(m_single_instance_mutex);

	AfxOleTerm(FALSE);
	GdiplusShutdown(m_gdiplusToken);
	return CWinAppEx::ExitInstance();
}

CString main_application::get_recent_list_item_full_path(int index)
{
	if (!m_pRecentFileList) return _T("");
	if (index < 0 || index > m_pRecentFileList->GetSize() - 1) return _T("");

	return (*m_pRecentFileList)[index];
}

int main_application::get_recent_list_item_count()
{
	if (m_pRecentFileList)
		return m_pRecentFileList->GetSize();
	else
		return 0;
}

void main_application::set_application_name(const CString& name)
{
	// 更改CWinAppEx::m_pszAppName的值就可以更改AfxMessageBox()的标题栏
	// First free the string allocated by MFC at CWinAppEx startup.
	// The string is allocated before InitInstance is called.
	free((void*) m_pszAppName); // 字符串使用allocated申请的，尽量使用free释放，不用delete
	// Change the name of the application file.
	// The CWinAppEx destructor will free the memory.
	m_pszAppName = _tcsdup(name);
}

void main_application::load_recent_list_to_mfc()
{
	if (m_pRecentFileList) safe_delete(m_pRecentFileList);

	CXTPRecentFileList* recent_list = new CXTPRecentFileList(0, _T("Recent File List"), _T("File%d"), 9);
	recent_list->ReadList();

	// 把路径失效的历史列表条目删除
	for (int i = 0; i < recent_list->GetCount(); /* nothing */)
	{
		CXTPRecentFileListItem* item = recent_list->GetItem(i);
		if (item)
		{
			CString full_path = item->GetPathName();
			if (full_path.IsEmpty() || !PathFileExists(full_path))
			{
				recent_list->Remove(i);
				continue;
			}
		}

		++i;
	}
	m_pRecentFileList = recent_list;
}

void main_application::on_app_about()
{
	about_dialog aboutDlg;
	aboutDlg.DoModal();
}
