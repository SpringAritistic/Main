#include "stdafx.h"
#include "../resource.h"

#include "project_manager/macro.h"
#include "project_manager/data_center/graph_data_base.h"
#include "project_manager/data_center/folder_node.h"

#include "graph_grid_ui_observer_base.h"
#include "dialog_create_graph_object.h"
#include "graph_editor_wnd.h"
#include "dialog_create_folder.h"
#include "dialog_rename.h"
#include "graph_drawing_strategy_base.h"

#include "graph_tree_view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;



project_tree_node_data::project_tree_node_data()
{
	m_node_type = project_tree_node_type::unknown;
}

project_tree_node_data::~project_tree_node_data()
{
}

IMPLEMENT_SERIAL(graph_tree_view, CXTPTreeView, 1)
BEGIN_MESSAGE_MAP(graph_tree_view, CXTPTreeView)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND_RANGE(APP_POPUP_MENU_ID_START, APP_POPUP_MENU_ID_END, on_command_all_popup_menu)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, on_tree_node_expanded)
END_MESSAGE_MAP()

graph_tree_view::graph_tree_view()
{
	m_last_option_node_handle = nullptr;
	initialize_menus();
}

graph_tree_view::~graph_tree_view()
{
	// 资源不要再删除，已被image_list接管
	// safe_delete(m_bitmap);
	clear_tree_nodes();
}

bool graph_tree_view::load_data_to_ui(const CString& data_tag)
{
	initialize_tree_nodes();
	return true;
}

bool graph_tree_view::save_data_from_ui()
{
	return true;
}

void graph_tree_view::on_project_data_notify(const CString& node_tag, notifer_action ac)
{
	if (notifer_action::pre_removed_node == ac)
	{
		return; // 删除前不响应，删除后再响应并重新更新树
	}
	else if (notifer_action::post_project_saved_as == ac)
	{
		CTreeCtrl& tree = GetTreeCtrl();
		if (tree.GetSafeHwnd())
		{
			HTREEITEM root = tree.GetRootItem();
			if (root) tree.SetItemText(root, data_center::get_data_file_name());
		}
	}
	else if (notifer_action::pre_project_closed == ac)
	{
		clear_tree_nodes();
	}
	else
	{
		// 以后要根据tag来决定该更新树的哪部分以便提高程序效率，现在可以暂时调用重新创建
		initialize_tree_nodes();
	}
}

void graph_tree_view::initialize_menus()
{
	popup_menu_item* project_menu = add_popup_menu(_T("project"));
	if (project_menu)
	{
		project_menu->add_sub_item(_T("添加图纸...\tAlt+&A"), [&](){ return on_menu_project_item_add_graph(); }, nullptr);
		project_menu->add_sub_item(_T("全部出图\tAlt+&G"), [&](){ return on_menu_project_item_draw_all_graphs(); }, nullptr);
		project_menu->add_sub_item(_T("清空\tAlt+&C"), [&](){ return on_menu_project_item_clear_all(); }, nullptr);
		project_menu->add_sub_item(_T("添加目录...\tAlt+&D"), [&](){ return on_menu_project_item_add_directory(); }, nullptr);
	}

	popup_menu_item* directory_menu = add_popup_menu(_T("directory"));
	if (directory_menu)
	{
		directory_menu->add_sub_item(_T("添加图纸...\tAlt+&A"), [&](){ return on_menu_directory_item_add_graph(); }, nullptr);
		directory_menu->add_sub_item(_T("全部出图\tAlt+&G"), [&](){ return on_menu_directory_item_draw_all_graphs(); }, nullptr);
		directory_menu->add_sub_item(_T("清空\tAlt+&C"), [&](){ return on_menu_directory_item_clear_all(); }, nullptr);
		directory_menu->add_sub_item(_T("添加目录...\tAlt+&D"), [&](){ return on_menu_directory_item_add_directory(); }, nullptr);
		directory_menu->add_sub_item(_T("删除\tAlt+&E"), [&](){ return on_menu_directory_item_delete(); }, nullptr);
		directory_menu->add_sub_item(_T("重命名...\tAlt+&R"), [&](){ return on_menu_directory_item_rename(); }, nullptr);
	}

	popup_menu_item* graph_menu = add_popup_menu(_T("graph"));
	if (graph_menu)
	{
		graph_menu->add_sub_item(_T("出图\tAlt+&G"), [&](){ return on_menu_graph_item_draw_one_graph(); }, nullptr);
		graph_menu->add_sub_item(_T("删除\tAlt+&E"), [&](){ return on_menu_graph_item_delete_one_graph(); }, nullptr);
		graph_menu->add_sub_item(_T("重命名...\tAlt+&R"), [&](){ return on_menu_graph_item_rename(); }, nullptr);
		graph_menu->add_sub_item(_T("-"));
		graph_menu->add_sub_item(_T("导出模板"), [&](){ return on_menu_graph_open_output_template(); },  nullptr);
		graph_menu->add_sub_item(_T("导入模板"), [&](){return on_menu_graph_open_import_template(); }, nullptr);
		graph_menu->add_sub_item(_T("-"));
		graph_menu->add_sub_item(_T("打开dxf图纸"), [&](){ return on_menu_graph_open_dxf_file(); }, nullptr);
		graph_menu->add_sub_item(_T("打开dwg图纸"), [&](){return on_menu_graph_open_dwg_file(); }, nullptr);
		graph_menu->add_sub_item(_T("打开pdf图纸"), [&](){return on_menu_graph_open_pdf_file(); }, nullptr);
	}
}

void graph_tree_view::app_theme_changed(theme_style_settings& settings)
{
	CTreeCtrl& tree = GetTreeCtrl();
	if (tree.GetSafeHwnd()) tree.SetBkColor(settings.m_window_back_color);
}

HTREEITEM insert_tree_data_node(CTreeCtrl& tree, HTREEITEM parent_item, data_node_base* cur_node_data)
{
	CString node_text = (cur_node_data ? cur_node_data->get_node_name() : _T(""));

	// 不是图纸节点不需要附带数据
	graph_data_base* gd = dynamic_cast<graph_data_base*>(cur_node_data);
	if (gd)
	{
		HTREEITEM node_handle = tree.InsertItem(node_text, 0, 0, parent_item, nullptr);
		if (nullptr == node_handle) return nullptr;

		project_tree_node_data* tree_item_data = new project_tree_node_data();
		tree_item_data->m_data_node_tag = gd->get_node_tag();
		tree_item_data->m_graph_reg_type_name = gd->m_graph_type_name;
		tree_item_data->m_node_type = project_tree_node_type::graph;
		tree.SetItemData(node_handle, (DWORD_PTR)tree_item_data);

		return node_handle;
	}

	folder_node* fd = dynamic_cast<folder_node*>(cur_node_data);
	if (fd)
	{
		HTREEITEM node_handle = tree.InsertItem(node_text, 1, 1, parent_item, nullptr);
		if (nullptr == node_handle) return nullptr;

		project_tree_node_data* tree_item_data = new project_tree_node_data();
		tree_item_data->m_data_node_tag = fd->get_node_tag();
		// tree_item_data->m_grap_resister_info;
		tree_item_data->m_node_type = project_tree_node_type::folder;
		tree.SetItemData(node_handle, (DWORD_PTR) tree_item_data);

		return node_handle;
	}

	// 有些节点特殊，不一定要有节点数据，如项目根节点
	HTREEITEM node_handle = tree.InsertItem(node_text, parent_item, nullptr);
	if (nullptr == node_handle) return nullptr;

	tree.SetItemData(node_handle, (DWORD_PTR)nullptr);
	return node_handle;
}

void implement_initialize_tree_nodes(CTreeCtrl& tree, HTREEITEM parent_item, data_node_base* cur_node_data)
{
	if (!cur_node_data) return;

	HTREEITEM cur_handle = insert_tree_data_node(tree, parent_item, cur_node_data);
	if (!cur_handle) return;

	for (int i = 0; i < (int) cur_node_data->get_children_count(); ++i)
		implement_initialize_tree_nodes(tree, cur_handle, cur_node_data->get_child(i));

	tree.Expand(cur_handle, TVE_EXPAND);
}

void graph_tree_view::initialize_tree_nodes()
{
	CTreeCtrl& tree = GetTreeCtrl();
	if (!tree.GetSafeHwnd()) return;

	clear_tree_nodes();

	m_last_option_node_handle = nullptr;
	
	// 插入根节点耗时太小，不用放到SetRedraw之间了，简化代码逻辑
	HTREEITEM root = tree.InsertItem(data_center::get_data_file_name(), 2, 2, nullptr, nullptr);
	if (data_center::is_closed()) return; // 放在插入根节点后面，否则项目名称节点就显示不了

	// 数据中心的数据根就不用在界面上创建树节点了
	data_node_base* node_data = data_center::get_data_root();
	if (!node_data) return;

	tree.SetRedraw(FALSE); // 放在clear_tree_nodes()之后，因为其内部也有SetRedraw调用

	for (int i = 0; i < (int) node_data->get_children_count(); ++i)
		implement_initialize_tree_nodes(tree, root, node_data->get_child(i));

	tree.Expand(root, TVE_EXPAND);

	tree.SetRedraw(TRUE);
}

void implement_only_clear_tree_data(CTreeCtrl& tree, HTREEITEM cur_item)
{
	// 仅删除树节点的附加数据，树节点本身不删除
	project_tree_node_data* node_data = (project_tree_node_data*) tree.GetItemData(cur_item);
	safe_delete(node_data);
	tree.SetItemData(cur_item, (DWORD_PTR) nullptr);

	// 再删除子节点的数据
	cur_item = tree.GetChildItem(cur_item);
	while (cur_item)
	{
		implement_only_clear_tree_data(tree, cur_item);
		cur_item = tree.GetNextSiblingItem(cur_item);
	}
}

void graph_tree_view::clear_tree_nodes()
{
	m_last_option_node_handle = nullptr;

	CTreeCtrl& tree = GetTreeCtrl();
	if (!tree.GetSafeHwnd()) return;

	HTREEITEM root = tree.GetRootItem();
	if (!root) return;
	
	tree.SetRedraw(FALSE);

	implement_only_clear_tree_data(tree, root);
	tree.DeleteAllItems();

	tree.SetRedraw(TRUE);
}

HTREEITEM graph_tree_view::get_tree_node_under_cursor()
{
	CTreeCtrl& tree = GetTreeCtrl();
	if (!tree.GetSafeHwnd()) return nullptr;

	CPoint point_click;
	GetCursorPos(&point_click);
	ScreenToClient(&point_click);
	
	// 这里用的坐标系与弹出菜单时用的不是同一个，没有在任何一个节点上右击则直接返回
	return tree.HitTest(point_click);
}

data_node_base* graph_tree_view::get_data_node_by_tree_item(HTREEITEM item)
{
	if (!item) return nullptr;

	CTreeCtrl& tree = GetTreeCtrl();
	if (!tree.GetSafeHwnd()) return nullptr;

	// 根节点没有数据，其它节点均有数据
	project_tree_node_data* tree_node = (project_tree_node_data*) tree.GetItemData(m_last_option_node_handle);
	if (!tree_node) return nullptr;

	return data_center::get_data_node(tree_node->m_data_node_tag);
}

void graph_tree_view::active_graph_editor(const CString& editor_title, const project_tree_node_data& tree_node_data)
{
	CString class_name;
	CObject* tmp_obj = nullptr;

	graph_register_info info;
	if (false == project_data::get_graph_register_info(tree_node_data.m_graph_reg_type_name, info)) return;

	class_name = info.m_common_grid_class_name;
	tmp_obj = CRuntimeClass::CreateObject(class_name);
	if (!tmp_obj)
	{
		CString err_info = _T("类 [ ") + class_name + _T(" ] 无法实例化，图纸编辑器创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	std::unique_ptr<graph_grid_ui_observer_base> grid_ui(dynamic_cast<graph_grid_ui_observer_base*>(tmp_obj));
	if (!grid_ui)
	{
		safe_delete(tmp_obj);
		CString err_info = _T("类 [ ") + class_name + _T(" ] 无法实例化，图纸编辑器创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	class_name = info.m_advance_grid_class_name;
	tmp_obj = CRuntimeClass::CreateObject(class_name);
	if (!tmp_obj)
	{
		CString err_info = _T("类 [ ") + class_name + _T(" ] 无法实例化，图纸编辑器创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	std::unique_ptr<graph_grid_ui_observer_base> adv_grid_ui(dynamic_cast<graph_grid_ui_observer_base*>(tmp_obj));
	if (!adv_grid_ui)
	{
		safe_delete(tmp_obj);
		CString err_info = _T("类 [ ") + class_name + _T(" ] 无法实例化，图纸编辑器创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	class_name = info.m_graphic_editor_class_name;
	tmp_obj = CRuntimeClass::CreateObject(class_name);
	if (!tmp_obj)
	{
		CString err_info = _T("类 [ ") + class_name + _T(" ] 无法实例化，图纸编辑器创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	std::unique_ptr<graph_graphic_ui_observer_base> graphic_ui(dynamic_cast<graph_graphic_ui_observer_base*>(tmp_obj));
	if (!graphic_ui)
	{
		safe_delete(tmp_obj);
		CString err_info = _T("类 [ ") + class_name + _T(" ] 无法实例化，图纸编辑器创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	graph_editor_wnd* editor_wnd = new graph_editor_wnd();

	// 数据由外部创建比放在编辑内部创建好，一是可以避开数据创建失败时导致的编辑窗口实例化开销，二是更符合展示界面
	// 与数据剥离的原则，三是以后万一数据有效率问题是可以外部使用缓存机制来提升效率并为编辑器直接提供数据对象，四
	// 是万一以后数据随业务变化创建过程变复杂或多样以后，适合使创建类的设计模式来对流程进行组织
	editor_wnd->set_common_grid_ui_observer(grid_ui);
	editor_wnd->set_common_graphic_ui_observer(graphic_ui);
	editor_wnd->set_advance_grid_ui_observer(adv_grid_ui);

	// 内部自动增长这个窗口id
	static unsigned int editor_wnd_start_id = 1001;

	// 赋好数据再创建，窗口必须指定父窗口，否则创建失败，此处随意指定一个，添加到工作区框架会自动设置为正确的父窗口
	BOOL flag = editor_wnd->Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		CRect(), this, ++editor_wnd_start_id);

	if (TRUE != flag)
	{
		safe_delete(editor_wnd);
		AfxMessageBox(_T("图纸编辑器创建失败"), MB_OK | MB_ICONERROR);
		return;
	}

	app_ui_interface* app = get_app_ui_interface();
	if (app) app->add_working_space_view(editor_wnd, tree_node_data.m_data_node_tag);
}

graph_data_base* graph_tree_view::create_graph_data_base(const CString& class_name)
{
	CObject* obj = (CRuntimeClass::CreateObject(class_name));
	if (!obj)
	{
		CString err_info = _T("类 [ ") + class_name + _T(" ] 无法实例化，图纸创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return nullptr;
	}

	graph_data_base* new_node = dynamic_cast<graph_data_base*>(obj);
	if (!new_node)
	{
		CString err_info = _T("类 [ ") + class_name + _T(" ] 无法实例化，图纸创建失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		safe_delete(obj); // 务必删除，否则内存泄漏
		return nullptr;
	}

	// 不用禁用数据更新，直接属性不会触发通知，因为还没有挂接到数据中心
	return new_node;
}

BOOL graph_tree_view::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS/* | TVS_TRACKSELECT*/ | TVS_DISABLEDRAGDROP;
	//lpCreateStruct->style &= ~TVS_LINESATROOT;
	lpCreateStruct->dwExStyle |= TVS_EX_DOUBLEBUFFER | TVS_EX_AUTOHSCROLL | TVS_EX_FADEINOUTEXPANDOS;

	if (-1 == CXTPTreeView::OnCreate(lpCreateStruct)) return FALSE;

	// XTP 17.2.0有bug，创建前预设的风格无效，会被它置为空，创建完成后再被设一次（之前版本没有这个问题）
	GetTreeCtrl().ModifyStyle(NULL, TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS /* | TVS_TRACKSELECT*/ | TVS_DISABLEDRAGDROP);
	//GetTreeCtrl().ModifyStyleEx(NULL,/* TVS_EX_DOUBLEBUFFER | */TVS_EX_AUTOHSCROLL | TVS_EX_FADEINOUTEXPANDOS);

	// 设置树节点图像
	m_bitmap.LoadBitmap(IDB_GRAPH_TREE_ICONS);
	
	m_normal_image_list.Create(16, 16, ILC_COLOR24 | ILC_MASK, 16, 1);
	m_normal_image_list.Add(&m_bitmap, RGB(255, 255, 255));
	
	m_bitmap.DeleteObject();

	GetTreeCtrl().SetImageList(&m_normal_image_list, TVSIL_NORMAL);

	// 窗口创建时手工设置起始状态的主题
	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		theme_style_settings app_theme = app->get_style_and_settings();
		app_theme_changed(app_theme);
	}
	
	m_last_option_node_handle = nullptr;

	return TRUE;
}

void graph_tree_view::OnClose()
{
	clear_tree_nodes();
}

BOOL graph_tree_view::OnEraseBkgnd(CDC* dc)
{
	return CXTPTreeView::OnEraseBkgnd(dc);
}

void graph_tree_view::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_last_option_node_handle = nullptr;

	CXTPTreeView::OnLButtonDblClk(nFlags, point);

	CTreeCtrl& tree = GetTreeCtrl();
	if (!tree.GetSafeHwnd()) return;

	HTREEITEM clicked_item_handle = get_tree_node_under_cursor();
	if (!clicked_item_handle) return; // 没有在任何一个节点上右击则直接返回

	// 把焦点从原来的地方移到当前鼠标点击处对应的条目上
	tree.Select(clicked_item_handle, TVGN_CARET);
	m_last_option_node_handle = clicked_item_handle;

	project_tree_node_data* tree_node_data = (project_tree_node_data*) tree.GetItemData(clicked_item_handle);
	if (!tree_node_data) return;
	if (project_tree_node_type::graph != tree_node_data->m_node_type) return; // 只有图纸双击才有效

	// 传入标题而不是让启动编辑时自己从送往的数据中找节点名是因为以后可能标题文本和树上显示的文本会有需求需要与数据节点名不一样
	data_node_base* node_data = data_center::get_data_node(tree_node_data->m_data_node_tag);
	active_graph_editor(node_data ? node_data->get_node_name() : _T(""), *tree_node_data);
}

void graph_tree_view::OnLButtonDown(UINT nFlags, CPoint point)
{
	CXTPTreeView::OnLButtonDown(nFlags, point);
}

void graph_tree_view::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_last_option_node_handle = nullptr;

	// mfc的CTreeCtrl右键响应有bug，双击右键才能响应，右键菜单不放在此处响应，改放在OnRButtonDown中响应
	// [WYZ]注：改用树控制的Notify消息可正确响应到右键消息，其它消息如左键双击等也可用这种方式，示例如下：
	// ON_NOTIFY(NM_RCLICK, TREE_CTRL_ID, on_tree_node_right_click)
	CXTPTreeView::OnRButtonDown(nFlags, point);

	CTreeCtrl& tree = GetTreeCtrl();
	if (!tree.GetSafeHwnd()) return;

	HTREEITEM root_item_handle = tree.GetRootItem(); // 可以为空
	HTREEITEM clicked_item_handle = get_tree_node_under_cursor();
	if (!clicked_item_handle) return; // 没有在任何一个节点上右击则直接返回
	
	// 把焦点从原来的地方移到当前鼠标点击处对应的条目上
	tree.Select(clicked_item_handle, TVGN_CARET);
	m_last_option_node_handle = clicked_item_handle;

	if (root_item_handle == clicked_item_handle)
	{
		popup_menu(_T("project"));
		return;
	}

	data_node_base* node_base = get_data_node_by_tree_item(clicked_item_handle);
	folder_node* fd = dynamic_cast<folder_node*>(node_base);
	if (fd)
	{
		popup_menu(_T("directory"));
		return;
	}

	graph_data_base* gd = dynamic_cast<graph_data_base*>(node_base);
	if (gd)
	{
		popup_menu(_T("graph"));
		return;
	}
}

void graph_tree_view::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_last_option_node_handle = nullptr;

	// mfc的CTreeCtrl右键响应有bug，双击右键才能响应OnRButtonUp，单击不会响应OnRButtonDown，而是在
	// 右键放开才响应OnRButtonDown，因此右键菜单改放在OnRButtonDown中响应
	// [WYZ]注：改用树控制的Notify消息可正确响应到右键消息，其它消息如左键双击等也可用这种方式，示例如下：
	// ON_NOTIFY(NM_RCLICK, TREE_CTRL_ID, on_tree_node_right_click)
	CXTPTreeView::OnLButtonUp(nFlags, point);
}

void graph_tree_view::on_tree_node_expanded(NMHDR* nm, LRESULT* lparam)
{
}

void graph_tree_view::on_command_all_popup_menu(UINT id)
{
	on_transform_popup_menu_command(id);
}

void graph_tree_view::on_menu_project_item_add_graph()
{
	dialog_create_graph_object dlg;

	// 已有的图纸名称只找直接子节点，因为不同的目录下是可以有同名的图纸
	data_node_base* rt = data_center::get_data_root();
	if (rt)
	{
		for (size_t i = 0; i < rt->get_children_count(); ++i)
		{
			data_node_base* cur_dn = rt->get_child(i);
			if (cur_dn) dlg.m_exist_graph_names.push_back(cur_dn->get_node_name());
		}
	}
	
	if (IDOK != dlg.DoModal()) return;
	
	// 不用禁用数据更新，直接修改属性不会触发通知，因为还没有挂接到数据中心上
	graph_register_info info;
	if (false == project_data::get_graph_register_info(dlg.m_result_type_name, info)) return;

	graph_data_base* new_node = create_graph_data_base(info.m_data_class_name);
	if (!new_node) return;

	new_node->set_node_name(dlg.m_result_graph_name);
	new_node->m_graph_type_name = dlg.m_result_type_name;	
	rt->add_child_node(new_node);
}

void graph_tree_view::on_menu_project_item_add_directory()
{
	data_node_base* rt = data_center::get_data_root();
	if (!rt) return;

	dialog_create_folder dlg;

	// 已有的目录名称只找直接子节点，因为不同的目录下是可以有同名的目录
	for (size_t i = 0; i < rt->get_children_count(); ++i)
	{
		data_node_base* cur_dn = rt->get_child(i);
		if (cur_dn) dlg.m_exist_names.push_back(cur_dn->get_node_name());
	}
	
	if (IDOK != dlg.DoModal()) return;

	folder_node* fn = new folder_node(dlg.m_result_name);
	rt->add_child_node(fn);
}

void graph_tree_view::on_menu_project_item_draw_all_graphs()
{
	AfxMessageBox(_T("暂未实现"), MB_OK | MB_ICONINFORMATION);


	//data_node_base* rt = data_center::get_data_root();
	//if (!rt) return; // 根节点不是图纸目录，不能删除

	//data_node_base* rt_2 = rt->get_child(0);
	//if (rt_2)
	//{
	//	graph_data_base* gb = new graph_data_base();
	//	//rt_2->add_child_node(gb);
	//}
}

void graph_tree_view::on_menu_project_item_clear_all()
{
	data_node_base* rt = data_center::get_data_root();
	if (!rt)  return;
	if (IDYES == AfxMessageBox(_T("确定清空项目！"), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
		rt->delete_all_children();
}

void graph_tree_view::on_menu_directory_item_add_graph()
{
	dialog_create_graph_object dlg;

	// 已有的图纸名称只找直接子节点，因为不同的目录下是可以有同名的图纸
	data_node_base* rt = get_data_node_by_tree_item(m_last_option_node_handle);
	if (rt)
	{
		for (size_t i = 0; i < rt->get_children_count(); ++i)
		{
			data_node_base* cur_dn = rt->get_child(i);
			if (cur_dn) dlg.m_exist_graph_names.push_back(cur_dn->get_node_name());
		}
	}

	if (IDOK != dlg.DoModal()) return;

	// 不用禁用数据更新，直接属性不会触发通知，因为还没有挂接到数据中心
	graph_register_info info;
	if (false == project_data::get_graph_register_info(dlg.m_result_type_name, info)) return;

	graph_data_base* new_node = create_graph_data_base(info.m_data_class_name);
	if (!new_node) return;

	new_node->set_node_name(dlg.m_result_graph_name);
	new_node->m_graph_type_name = dlg.m_result_type_name;
	rt->add_child_node(new_node);
}

void graph_tree_view::on_menu_directory_item_draw_all_graphs()
{
	AfxMessageBox(_T("暂未实现"), MB_OK | MB_ICONINFORMATION);
}

void graph_tree_view::on_menu_directory_item_clear_all()
{
	data_node_base* dt = get_data_node_by_tree_item(m_last_option_node_handle);
	if (!dt) return;

	if (IDYES == AfxMessageBox(_T("确定清空文件夹！"), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
		dt->delete_all_children();
}

void graph_tree_view::on_menu_directory_item_add_directory()
{
	dialog_create_folder dlg;

	// 已有的目录名称只找直接子节点，因为不同的目录下是可以有同名的目录的
	data_node_base* data_node = get_data_node_by_tree_item(m_last_option_node_handle);
	if (!data_node) return;

	for (size_t i = 0; i < data_node->get_children_count(); ++i)
	{
		data_node_base* cur_dn = data_node->get_child(i);
		if (cur_dn) dlg.m_exist_names.push_back(cur_dn->get_node_name());
	}

	if (IDOK != dlg.DoModal()) return;

	folder_node* fn = new folder_node(dlg.m_result_name);
	data_node->add_child_node(fn);
}

void graph_tree_view::on_menu_directory_item_delete()
{
	data_node_base* dt = get_data_node_by_tree_item(m_last_option_node_handle);
	if (!dt || dt == data_center::get_data_root()) return; // 根节点，不能删除

	if (IDYES == AfxMessageBox(_T("确定删除文件夹！"), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
		dt->delete_self_data_node();
}

void graph_tree_view::on_menu_directory_item_rename()
{
	// 已有的目录名称只找直接子节点，因为不同的目录下是可以有同名的目录的
	data_node_base* data_node = get_data_node_by_tree_item(m_last_option_node_handle);
	if (!data_node) return;

	dialog_rename dlg;
	dlg.m_result_name = data_node->get_node_name();

	data_node_base* parent_node = data_node->get_parent();
	if (parent_node)
	{
		for (size_t i = 0; i < parent_node->get_children_count(); ++i)
		{
			data_node_base* cur_dn = parent_node->get_child(i);
			if (cur_dn) dlg.m_exist_names.push_back(cur_dn->get_node_name());
		}
	}

	if (IDOK != dlg.DoModal()) return;

	data_node->set_node_name(dlg.m_result_name);
}

void graph_tree_view::on_menu_graph_item_draw_one_graph()
{
	graph_data_base* data_node = dynamic_cast<graph_data_base*>(get_data_node_by_tree_item(m_last_option_node_handle));
	if (!data_node)
	{
		CString err_info = _T("图纸数据无法索引，绘图失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	graph_register_info info;
	if (false == data_node->get_graph_reg_info(info))
	{
		CString err_info = _T("图纸类型注册信息数据无效，绘图失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	CString strategy_class_name = info.m_drawing_strategy_class_name;
	strategy_class_name.Trim();
	if (strategy_class_name.IsEmpty())
	{
		CString err_info = _T("图纸绘制策略类 [ ") + strategy_class_name + (" ] 无效，绘图失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	CObject* tmp_obj = CRuntimeClass::CreateObject(strategy_class_name);
	if (!tmp_obj)
	{
		CString err_info = _T("图纸绘制策略类 [ ") + strategy_class_name + _T(" ] 无法实例化，绘图失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	std::unique_ptr<graph_drawing_strategy_base> drawing_tool(dynamic_cast<graph_drawing_strategy_base*>(tmp_obj));
	if (!drawing_tool)
	{
		safe_delete(tmp_obj);
		CString err_info = _T("图纸绘制策略类 [ ") + strategy_class_name + _T(" ] 无法实例化，绘图失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		return;
	}

	drawing_tool->m_graph_data_tag = data_node->get_node_tag();

	CString graph_name = data_node->m_graph_type_name + _T("-") + data_node->get_node_name();
	bool flag = drawing_tool->draw(graph_name);
	if (!flag)
	{
		CString err_info = _T("图纸 [ ") + graph_name + _T(" ] 绘制失败");
		AfxMessageBox(err_info, MB_OK | MB_ICONERROR);
		// return; 不要返回，失败也更新，相应的图纸窗口要变为没图纸或仍然显示以前图纸，相当于要重刷一下
	}

	data_node->notify_if_enable(notifer_action::post_updated_node); // 图纸名称变了，图纸也有了
}

void graph_tree_view::on_menu_graph_item_delete_one_graph()
{
	data_node_base* dt = get_data_node_by_tree_item(m_last_option_node_handle);
	if (!dt || dt == data_center::get_data_root()) return; // 根节点，不能删除

	if (IDYES == AfxMessageBox(_T("确定删除图纸！"), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
		dt->delete_self_data_node();
}

void graph_tree_view::on_menu_graph_item_rename()
{
	// 已有的目录名称只找直接子节点，因为不同的目录下是可以有同名的目录的
	data_node_base* data_node = get_data_node_by_tree_item(m_last_option_node_handle);
	if (!data_node) return;

	dialog_rename dlg;
	dlg.m_result_name = data_node->get_node_name();

	data_node_base* parent_node = data_node->get_parent();
	if (parent_node)
	{
		for (size_t i = 0; i < parent_node->get_children_count(); ++i)
		{
			data_node_base* cur_dn = parent_node->get_child(i);
			if (cur_dn) dlg.m_exist_names.push_back(cur_dn->get_node_name());
		}
	}

	if (IDOK != dlg.DoModal()) return;

	data_node->set_node_name(dlg.m_result_name);
}

void graph_tree_view::on_menu_graph_open_output_template()
{
	AfxMessageBox(_T("暂未实现"), MB_OK | MB_ICONINFORMATION);
}

void graph_tree_view::on_menu_graph_open_import_template()
{
	AfxMessageBox(_T("暂未实现"), MB_OK | MB_ICONINFORMATION);
}

void graph_tree_view::on_menu_graph_open_dxf_file()
{
	::ShellExecute(nullptr, _T("open"), _T("E:\\第一册.pdf"), nullptr, nullptr, SW_SHOW);
}

void graph_tree_view::on_menu_graph_open_dwg_file()
{
	::ShellExecute(nullptr, _T("open"), _T("E:\\第一册.pdf"), nullptr, nullptr, SW_SHOW);
}

void graph_tree_view::on_menu_graph_open_pdf_file()
{
	::ShellExecute(nullptr, _T("open"), _T("E:\\第一册.pdf"), nullptr, nullptr, SW_SHOW);
}

