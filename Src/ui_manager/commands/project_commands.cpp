#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/tools/file_tools.h"
#include "x_framework/interface/data_center_interface.h"
#include "x_framework/interface/window_interface.h"

#include "project_commands.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;



IMPLEMENT_DYNAMIC(create_project_dialog, app_dialog_base)
BEGIN_MESSAGE_MAP(create_project_dialog, app_dialog_base)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER_PATH, on_clicked_button_browser_path)
	ON_BN_CLICKED(IDOK, on_clicked_button_ok)
END_MESSAGE_MAP()


create_project_dialog::create_project_dialog(CWnd* parent /*= nullptr*/) : app_dialog_base(IDD, parent)
{
}

create_project_dialog::~create_project_dialog()
{
}

void create_project_dialog::DoDataExchange(CDataExchange* dx)
{
	app_dialog_base::DoDataExchange(dx);

	DDX_Text(dx, IDC_PROJECT_NAME, m_ui_project_name);
	DDX_Text(dx, IDC_PROJECT_PATH, m_ui_project_path);
}

BOOL create_project_dialog::OnInitDialog()
{
	app_dialog_base::OnInitDialog();

	CWinApp* win_app = AfxGetApp();
	if (!win_app) return FALSE;

	CString last_saved_string = win_app->GetProfileString(_T("create_project"), _T("project_path"), _T(""));
	if (last_saved_string.IsEmpty() || !PathIsDirectory(last_saved_string) || !PathFileExists(last_saved_string))
	{
		TCHAR desk_top_buffer[MAX_PATH + 1] = { 0 };
		SHGetSpecialFolderPath(GetSafeHwnd(), desk_top_buffer, CSIDL_DESKTOP, FALSE);
		last_saved_string = desk_top_buffer;
	}

	m_ui_project_name = _T("新项目");
	m_ui_project_path = last_saved_string;

	UpdateData(FALSE);

	return TRUE;
}

void create_project_dialog::on_clicked_button_browser_path()
{
	CFolderPickerDialog dlg;
	if (IDOK != dlg.DoModal()) return;

	CString path = dlg.GetPathName();
	CEdit* model_name = (CEdit*) GetDlgItem(IDC_PROJECT_PATH);
	if (model_name && model_name->GetSafeHwnd()) model_name->SetWindowText(path);
}

void create_project_dialog::on_clicked_button_ok()
{
	UpdateData(TRUE);

	m_ui_project_name.Trim();
	m_ui_project_path.Trim();

	if (m_ui_project_name.IsEmpty())
	{
		AfxMessageBox(_T("项目名称不能为空"), MB_OK | MB_ICONERROR);
		return;
	}

	if (m_ui_project_path.IsEmpty())
	{
		AfxMessageBox(_T("项目路径不能为空"), MB_OK | MB_ICONERROR);
		return;
	}

	CString ext = file_tools::get_ext_from_file_path_name(m_ui_project_name);
	if (ext.IsEmpty()) m_ui_project_name += _T(".") + data_center::get_data_file_suffix();

	m_full_path_name = file_tools::get_path_from_file_path_name_with_xg(m_ui_project_path) + m_ui_project_name;

	// 保存用户选择的项目目录
	CWinApp* win_app = AfxGetApp();
	if (win_app) win_app->WriteProfileString(_T("create_project"), _T("project_path"), m_ui_project_path);

	CDialogEx::OnOK();
}


//////////////////////////////////////////////////////////////////////////


project_commands::project_commands() : cmd_container_interface(true)
{
}

project_commands::~project_commands()
{
}

bool project_commands::fire(const CString& domain, const CString& cmd, const CString& param /*= _T("")*/, int drive_mode /*= 0*/)
{
	if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("new")))
		on_new(param, drive_mode);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("open")))
		on_open(param, drive_mode);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("close_project")))
		on_close_project(param, drive_mode);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("quit")))
		on_quit(param, drive_mode);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("save")))
		on_save(param, drive_mode);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("save_as")))
		on_save_as(param, drive_mode);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("start_page")))
		on_start_page(param, drive_mode);
	else
		; // nothing

	return false;
}

void project_commands::get_status(const CString& domain, const CString& cmd, bool& enable, bool& checked)
{
	enable = false;
	checked = false;

	if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("new")))
		on_update_new(enable, checked);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("open")))
		on_update_open(enable, checked);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("close_project")))
		on_update_close_project(enable, checked);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("quit")))
		on_update_quit(enable, checked);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("save")))
		on_update_save(enable, checked);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("save_as")))
		on_update_save_as(enable, checked);
	else if (0 == domain.CompareNoCase(_T("sys")) && 0 == cmd.CompareNoCase(_T("start_page")))
		on_update_start_page(enable, checked);
	else
		; // nothing
}

void project_commands::set_command_filter(std::vector<std::pair<CString, CString>>& cmds_can_do) const
{
	cmds_can_do.push_back({ _T("sys"), _T("new") });
	cmds_can_do.push_back({ _T("sys"), _T("open") });
	cmds_can_do.push_back({ _T("sys"), _T("close_project") });
	cmds_can_do.push_back({ _T("sys"), _T("quit") });
	cmds_can_do.push_back({ _T("sys"), _T("save") });
	cmds_can_do.push_back({ _T("sys"), _T("save_as") });
	cmds_can_do.push_back({ _T("sys"), _T("start_page") });
}

bool project_commands::on_new(const CString& param, int drive_mode)
{
	if (!data_center::get_data_root()) return false;

	create_project_dialog dlg;
	if (IDOK != dlg.DoModal()) return false;

	if (data_center::is_open()) data_center::close_file();

	// 先禁用数据通知，因为这个时候界面还没有呢，收不到，注意括号不能省略，否则tmp对象的生命周期不正确
	{
		data_node_notify_guard tmp(false);

		// 创建模型数据
		data_center::new_file(dlg.m_full_path_name);
		if (!data_center::is_open()) return false; // 创建模型失败直接返回

		// 创建界面元素
		app_ui_interface* app = get_app_ui_interface();
		if (app)
		{
			app->set_title(file_tools::get_full_name_from_file_path_name(dlg.m_full_path_name));
			app->post_close_all_workspace_window(); // 必须使用post方式，否则起始页中创建会崩溃
			app->create_all_docking_panes();
		}
	}

	// 手工触发一次界面通知，尽量以post机制，以便确保界面正常工作后才收到更新
	data_node_base* nd = data_center::get_data_root();
	if (nd) nd->post_notify_if_enable(notifer_action::post_project_loaded);

	return false;
}

bool project_commands::on_open(const CString& param, int drive_mode)
{
	CWaitCursor cur_tmp;

	CString path_name = param; // 支持无界面时使用参数直接打开指定文件
	path_name.Trim();
	if (path_name.IsEmpty())
	{
		CFileDialog dlg(TRUE, _T("*.") + data_center::get_data_file_suffix(), _T(""),
			OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, data_center::get_data_file_filter());
		if (IDOK != dlg.DoModal()) return false;

		path_name = dlg.GetPathName();
	}

	// 先禁用数据通知，因为这个时候界面还没有呢，收不到，注意括号不能省略，否则tmp对象的生命周期不正确
	{
		data_node_notify_guard tmp(false);
		if (false == data_center::open_file(path_name)) return false;
	}

	// 手工触发一次界面通知，尽量以post机制，以便确保界面正常工作后才收到更新
	data_node_base* nd = data_center::get_data_root();
	if (nd) nd->post_notify_if_enable(notifer_action::post_project_loaded);

	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		app->set_title(file_tools::get_full_name_from_file_path_name(path_name));
		app->post_close_all_workspace_window(); // 必须使用post方式，否则起始页中创建会崩溃
		app->create_all_docking_panes();
	}

	return false;
}

bool project_commands::on_close_project(const CString& param, int drive_mode)
{
	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		int wnd_count = app->get_working_space_view_count(); // 不要放到循环中取，循环前取一次
		for (int i = wnd_count - 1; i >= 0; --i)
		{
			// 获取指定索引的窗口信息0：窗口类名， 1：管理对象路径，2：窗口指针，成功返回true，否则返回false
			tuple<CString, CString, window_interface*> wnd_info;
			if (!app->get_tab_window_and_info(wnd_info, i)) continue;
			
			// 如果有起始页则不关闭，否则后面再打开会有闪烁
			if (_T("start_page") == std::get<0>(wnd_info) && param == _T("open_start_page")) continue;

			// app->close_working_space_view_using_postmessage_by_index(i, true); // 必须使用post方式
			app->close_working_space_view_by_index(i);
		}

		app->destroy_all_docking_panes();
		app->set_title(_T(""));
		
		if (param == _T("open_start_page"))
			app->add_working_space_view(_T("start_page"), _T("")); // 如果没有起始页则添加，否则不添加
	}

	if (data_center::is_open()) data_center::close_file();

	return false;
}

bool project_commands::on_quit(const CString& param, int drive_mode)
{
	if (data_center::is_open()) data_center::close_file();

	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		app->close_all_workspace_window(true);
		app->destroy_all_docking_panes();
	}
	
	// 程序会自己结束，不用再发
	// AfxPostQuitMessage(0);
	return false;
}

bool project_commands::on_save(const CString& param, int drive_mode)
{
	CWaitCursor tmp;

	set<window_interface*> all_wnds = window_interface::get_all_window_instances();
	for (window_interface* x : all_wnds)
		if (x) x->save_data_from_ui();

	if (data_center::is_open()) data_center::save_file();
	return false;
}

bool project_commands::on_save_as(const CString& param, int drive_mode)
{
	CWaitCursor tmp;

	if (data_center::is_closed()) return false;

	CString path_name = param; // 支持无界面时使用参数直接保存到指定文件
	path_name.Trim();
	if (path_name.IsEmpty())
	{
		CFileDialog dlg(FALSE, _T("*.") + data_center::get_data_file_suffix(),
			data_center::get_data_file_full_path_name(),
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
			data_center::get_data_file_filter());

		if (IDOK != dlg.DoModal()) return false;

		path_name = dlg.GetPathName();
	}

	// 先将界面数据保存到数据中心
	set<window_interface*> all_wnds = window_interface::get_all_window_instances();
	for (window_interface* x : all_wnds)
		if (x) x->save_data_from_ui();

	data_center::save_as_file(path_name);
	data_center::notify_if_enable(_T(""), notifer_action::post_project_saved_as);

	app_ui_interface* app = get_app_ui_interface();
	if (app) app->set_title(file_tools::get_full_name_from_file_path_name(path_name));

	return false;
}

bool project_commands::on_start_page(const CString& param, int drive_mode)
{
	app_ui_interface* app = get_app_ui_interface();
	if (!app) return false;

	app->add_working_space_view(_T("start_page"), _T(""));

	return false;
}

void project_commands::on_update_new(bool& enable, bool& checked)
{
	enable = true;
	checked = false;
}

void project_commands::on_update_open(bool& enable, bool& checked)
{
	enable = true;
	checked = false;
}

void project_commands::on_update_close_project(bool& enable, bool& checked)
{
	enable = true;
	checked = false;
}

void project_commands::on_update_quit(bool& enable, bool& checked)
{
	enable = true;
	checked = false;
}

void project_commands::on_update_save(bool& enable, bool& checked)
{
	enable = true;
	checked = false;
}

void project_commands::on_update_save_as(bool& enable, bool& checked)
{
	enable = true;
	checked = false;
}

void project_commands::on_update_start_page(bool& enable, bool& checked)
{
	enable = true;
	checked = false;
}

