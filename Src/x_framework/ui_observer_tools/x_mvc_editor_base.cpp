#include "stdafx.h"

#include "THGridControl/THGridControlMac.h"
#include "THGridControl/common.h"
#include "THGridControl/THDataBase.h"
#include "THGridControl/THExcel.h"
#include "THGridControl/THBaseExcel.h"

#include "../interface/app_ui_interface.h"
#include "x_mvc_editor_base.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_DYNAMIC(x_mvc_editor_base, CWnd)
BEGIN_MESSAGE_MAP(x_mvc_editor_base, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

x_mvc_editor_base::x_mvc_editor_base()
{
	m_ui_observer = nullptr;
}

x_mvc_editor_base::~x_mvc_editor_base()
{
	safe_delete(m_ui_observer);
}

bool x_mvc_editor_base::load_data_to_ui(const CString& data_tag)
{
	return true;
}

bool x_mvc_editor_base::save_data_from_ui()
{
	return true;
}

CString x_mvc_editor_base::get_title()
{
	return (m_ui_observer ? m_ui_observer->get_title() : _T(""));
}

void x_mvc_editor_base::app_theme_changed(theme_style_settings& settings)
{
}

void x_mvc_editor_base::on_project_data_notify(const CString& node_tag, notifer_action ac)
{
}

bool x_mvc_editor_base::need_save_ui_data()
{
	return false;
}

int x_mvc_editor_base::get_self_index_in_working_space() const
{
	app_ui_interface* ui = get_app_ui_interface();
	if (!ui) return -1;

	return ui->get_working_space_view_index(this);
}

void x_mvc_editor_base::set_data_node_tag(const CString& tag)
{
	m_data_tag = tag;
	if (m_ui_observer) m_ui_observer->m_data_node_tag = tag;
}

void x_mvc_editor_base::set_ui_observer(x_observer_base* observer)
{
	m_ui_observer = observer;
	if (m_ui_observer)
	{
		m_ui_observer->m_data_node_tag = m_data_tag;
		m_ui_observer->m_owner_window = this;
	}
}

CString x_mvc_editor_base::get_data_node_tag() const
{
	return m_data_tag;
}

x_observer_base* x_mvc_editor_base::get_ui_observer() const
{
	return m_ui_observer;
}

int x_mvc_editor_base::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
		lpCreateStruct->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		lpCreateStruct->style &= ~WS_BORDER;
		if (-1 == CWnd::OnCreate(lpCreateStruct)) return -1;

		// 窗口创建时手工设置起始状态的主题
		app_ui_interface* app = get_app_ui_interface();
		if (app)
		{
			theme_style_settings app_theme = app->get_style_and_settings();
			app_theme_changed(app_theme);
		}

		return 0;
}

void x_mvc_editor_base::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);
}

void active_workspace_mvc_editor(const CString& data_node_tag,
	const CString& ui_observer_class_name, const CString& mvc_editor_class_name)
{
	if (mvc_editor_class_name.IsEmpty())
	{
		CString err_info = _T("编辑器类名均不能为空，编辑器创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	// 内部使用窗口实例的方式往工作区添加窗口，而不是使用类名，原因是想在外部指定相应的观察者类等，因此要自己检测
	// 对应节点的数据窗口有没有已经添加
	app_ui_interface* app = get_app_ui_interface();
	if (!app)
	{
		CString err_info = _T("无法获取程序实例接口，编辑器创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	// 已创建此窗口则切换到窗口并返回，此接口支持数据为空的检测
	if (app->is_working_space_view_exist(mvc_editor_class_name, data_node_tag))
	{
		app->select_working_space_view(mvc_editor_class_name, data_node_tag);
		return;
	}

	// 可以为空
	//if (data_node_tag.IsEmpty() || ui_observer_class_name.IsEmpty())
	//{
	//	CString err_info = _T("数据类类名、界面类类名均不能为空，编辑器创建失败");
	//	AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
	//	return;
	//}

	unique_ptr<x_observer_base> ui_observer;
	if (!ui_observer_class_name.IsEmpty())
	{
		CObject* ui_observer_obj = CRuntimeClass::CreateObject(ui_observer_class_name);
		if (!ui_observer_obj)
		{
			CString err_info = _T("类 [ ") + ui_observer_class_name + _T(" ] 无法实例化，编辑器创建失败");
			AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
			return;
		}

		ui_observer.reset(dynamic_cast<x_observer_base*>(ui_observer_obj));
		if (!ui_observer)
		{
			safe_delete(ui_observer_obj);
			CString err_info = _T("类 [ ") + ui_observer_class_name + _T(" ] 无法实例化，编辑器创建失败");
			AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
			return;
		}
	}

	unique_ptr<x_mvc_editor_base> editor_wnd;
	{
		CObject* editor_base = CRuntimeClass::CreateObject(mvc_editor_class_name);
		if (!editor_base)
		{
			CString err_info = _T("编辑器类 [ ") + mvc_editor_class_name + _T(" ] 无法实例化，编辑器创建失败");
			AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
			return;
		}

		editor_wnd.reset(dynamic_cast<x_mvc_editor_base*>(editor_base));
		if (!editor_wnd)
		{
			safe_delete(editor_base);
			CString err_info = _T("编辑器类 [ ") + mvc_editor_class_name + _T(" ] 无法实例化，编辑器创建失败");
			AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
			return;
		}
	}

	// 数据由外部创建比放在编辑器内部创建好，一是可以避开数据创建失败时导致的编辑窗口实例化开销，二是更符合展示界面
	// 与数据剥离的原则，三是以后万一数据有效率问题是可以外部使用缓存机制来提升效率并为编辑器直接提供数据对象，四
	// 是万一以后数据随业务变化创建过程变复杂或多样以后，适合使创建类的设计模式来对流程进行组织
	editor_wnd->set_ui_observer(ui_observer.release());
	editor_wnd->set_data_node_tag(data_node_tag);

	// 内部自动增长这个窗口id
	static unsigned int editor_wnd_start_id = 1101;

	// 赋好数据再创建，窗口必须指定父窗口，否则创建失败，此处随意指定一个，添加到工作区框架会自动设置为正确的父窗口
	// 父窗口添加到工作区会被强制改为工作区窗口，但此处必须要指定一个非空的窗口，否则mfc无法创建窗口
	BOOL flag = editor_wnd->Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		CRect(), AfxGetApp()->m_pMainWnd, ++editor_wnd_start_id);

	if (TRUE != flag)
	{
		//safe_delete(editor_wnd);
		AfxMessageBox(_T("编辑器创建失败"), MB_OK | MB_ICONERROR);
		return;
	}

	app->add_working_space_view(editor_wnd.release(), data_node_tag);
}

