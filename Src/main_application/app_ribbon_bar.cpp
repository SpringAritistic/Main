#include "StdAfx.h"
#include "Resource.h"

#include "x_framework/macro.h"
#include "x_framework/tools/file_tools.h"
#include "x_framework/interface/cmd_container_interface.h"
#include "x_framework/interface/window_interface.h"
#include "x_framework/interface/app_ui_interface.h"

#include "main_application.h"
#include "main_frame.h"

#include "app_ribbon_bar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_DYNCREATE(app_ribbon_bar, CXTPRibbonBar)
BEGIN_MESSAGE_MAP(app_ribbon_bar, CXTPRibbonBar)
END_MESSAGE_MAP()

app_ribbon_bar::app_ribbon_bar(void)
{
	m_recent_file_list_count = 5;
	m_recent_list.clear();
	m_file_path_max_length = 35;
}

app_ribbon_bar::~app_ribbon_bar( void )
{

}

bool app_ribbon_bar::initlize_ribbon_bar(const ribbon_bar_config_data& ribbon_ui_config)
{
	if (!GetSafeHwnd()) return false; // 必须要控件已经创建才能调用初始化函数

	m_context_tab_info_cache.clear();

	SetMenu(nullptr); // 清除系统自带的主菜单（如果有的话）
	initlize_ribbon_bar_file_menu(ribbon_ui_config);

	//////////////////////////////////////////////////////////////////////////

	CXTPRibbonTab* pTab = nullptr;
	CXTPRibbonGroup* pGroup = nullptr;
	CXTPControl* tmp_ctrl = nullptr;

	for (size_t i = 0; i < ribbon_ui_config.m_all_tabs.size(); ++i)
	{
		const ribbon_bar_data_tab& tabData = ribbon_ui_config.m_all_tabs[i];

		pTab = AddTab(tabData.m_tab_caption);
		if (!pTab) continue;
		
		// 安排当前选项卡下的所有控件图标及图标索引
		int nControlMaxIconID = tabData.GetMaxIconIndex(); // 统计当前选项卡下最大的图标索引值
		if (nControlMaxIconID >= 0)
		{
			// 初始化图标索引数值
			// xml中的索引从0开始，而xtp中创建Ribbon时要求图标索引与命令ID相同，界面库会自动匹配，所以
			// 此处应该将所有初始值改为IDS_RIBBON_UI_INVALID表示无法获取图标索引（因为所有内部自动分配
			// 的命令ID都不会为此值，图标索引定为这个值可以保证界面无法访问到，加之xtp要求图标索引为
			// UINT的数据，无法赋负数，所以才这样处理）
			UINT* pUINTArray = new UINT[nControlMaxIconID + 1]; // 个数比索引多一个
			for (int iconIndex = 0; iconIndex <= nControlMaxIconID; ++iconIndex)
				pUINTArray[iconIndex] = IDS_RIBBON_UI_INVALID; // 全部赋无效的命令

			// 对使用到的图标索引赋到对应的元素位置
			vector<ribbon_control_id_and_icon_id> vecID = tabData.get_control_and_icon_id();
			for (size_t szIconIndex = 0; szIconIndex < vecID.size(); ++szIconIndex)
			{
				// 有效图标的索引必须与命令ID一致才能被XTP库匹配正确
				if (vecID[szIconIndex].icon_id <= nControlMaxIconID && vecID[szIconIndex].icon_id >= 0)
					pUINTArray[vecID[szIconIndex].icon_id] = vecID[szIconIndex].id;
			}
			
			CString strPath = file_tools::get_app_path_with_xg() + _T("gui_xml\\") + tabData.m_icon_file_name;

			CImage img;
			// if (S_OK != img.Load(strPath)) continue; // 图像加载不成功不能线束本次循环，就让界面继续创建无图标的形式即可
			if (S_OK == img.Load(strPath))
			{
				HBITMAP hBitmap = img.Detach();
				if (NULL != hBitmap)
				{
					CBitmap bitmap;
					bitmap.Attach(hBitmap);

					// 图标元素个数比最大索多一个，超过的索引无图标
					GetImageManager()->SetIcons(bitmap, pUINTArray, nControlMaxIconID + 1,
						CSize(32, 32), xtpImageNormal, (tabData.m_image_has_alpha ? TRUE : FALSE));
				}
			}

			safe_delete_array(pUINTArray);
		}

		// 创建选项卡及其内部所有控件
		pTab->SetKeyboardTip(tabData.m_tab_acc_key);
		if (false == tabData.m_context_name.IsEmpty())
		{
			pTab->SetContextTab(tabData.m_context_color, tabData.m_context_name);
			pTab->SetVisible(FALSE); // 创建的时候隐藏
			
			// 只有上下文标签会被存储，固定的是不会被存储的
			if (false == tabData.m_context_name.IsEmpty())
				m_context_tab_info_cache.push_back( make_tuple(pTab, tabData.m_active_view_class, tabData.m_context_auto_active) );
		}

		for (size_t j = 0; j < tabData.m_all_groups.size(); ++j)
		{
			const ribbon_bar_data_group& group = tabData.m_all_groups[j];

			pGroup = pTab->AddGroup(group.m_group_caption);
			pGroup->ShowOptionButton(FALSE);

			for (size_t k = 0; k < group.m_control_in_group.size(); ++k)
			{
				ribbon_bar_data_controlbase* pCtrlData = group.m_control_in_group[k];
				if (!pCtrlData) continue;

				if (xtpControlError == pCtrlData->get_ribbon_control_type()) continue;

				tmp_ctrl = pGroup->Add(pCtrlData->get_ribbon_control_type(), pCtrlData->m_id);
				if (!tmp_ctrl) continue;

				pCtrlData->init_ribbon_control(tmp_ctrl);
			}
		}
	}



	//////////////////////////////////////////////////////////////////////////

	 //设置主界面右边几个图标
	{
		UINT nIDs[] =
		{
			IDS_RIBBON_OPTIONS_MINI_MODE,
			IDS_RIBBON_OPTIONS_THEMECHANGE,
			IDS_RIBBON_OPTIONS_ABOUT
		};

		GetImageManager()->SetIcons(IDS_RIBBON_UI_OTHERS, nIDs, _countof(nIDs), CSize(16, 16), xtpImageNormal);

		tmp_ctrl = GetControls()->Add(xtpControlButton, IDS_RIBBON_OPTIONS_MINI_MODE);
		tmp_ctrl->SetFlags(xtpFlagRightAlign);
		tmp_ctrl->SetIconId(IDS_RIBBON_OPTIONS_MINI_MODE);
		tmp_ctrl->SetTooltip(_T("功能区最小化"));
	
		tmp_ctrl = GetControls()->Add(xtpControlButtonPopup /* xtpControlPopup */, IDS_RIBBON_OPTIONS_THEMECHANGE);
		CXTPControlPopup* theme = (CXTPControlPopup*)tmp_ctrl;
		theme->SetFlags(xtpFlagRightAlign);
		CMenu theme_menu;
		theme_menu.LoadMenu(IDR_MENU_OPTIONS);
		theme->SetCommandBar(theme_menu.GetSubMenu(0));
		theme->SetIconId(IDS_RIBBON_OPTIONS_THEMECHANGE);
		//theme->SetCaption(_T("主题"));
		theme->SetTooltip(_T("切换界面主题风格"));
		
		tmp_ctrl = GetControls()->Add(xtpControlButton, IDS_RIBBON_OPTIONS_ABOUT);
		tmp_ctrl->SetFlags(xtpFlagRightAlign);
		tmp_ctrl->SetIconId(IDS_RIBBON_OPTIONS_ABOUT);
		//tmp_ctrl->SetCaption(_T("关于"));
		tmp_ctrl->SetTooltip(_T("软件相关提示"));
	}

	// 快速访问栏
	{
		const ribbon_quick_access_bar& temp_access_bar = ribbon_ui_config.m_quick_access;

		vector<unsigned int> button_ids;
		for (const auto& x : temp_access_bar.m_all_buttons)
			button_ids.push_back(x.m_id);

		// icon id must be sort
		std::sort(button_ids.begin(), button_ids.end());
		
		if (button_ids[0] != IDS_RIBBON_QUICK_ACCESS_MIN)
			button_ids.push_back(IDS_RIBBON_QUICK_ACCESS_MIN);

		std::sort(button_ids.begin(), button_ids.end());

		for (int i = 0; i < (int)button_ids.size() - 1; ++i)
		{
			if ((button_ids[i + 1] - button_ids[i]) > 1)
			{
				button_ids.push_back(button_ids[i] + 1);
				std::sort(button_ids.begin(), button_ids.end());
			}
		}

		CString strPath = file_tools::get_app_path_with_xg() + _T("gui_xml\\") + temp_access_bar.m_file_name;

		CImage img;
		if (S_OK == img.Load(strPath))
		{
			HBITMAP hBitmap = img.Detach();
			if (NULL != hBitmap)
			{
				CBitmap bitmap;
				bitmap.Attach(hBitmap);

				// 图标元素个数比最大索多一个，超过的索引无图标
				GetImageManager()->SetIcons(bitmap, button_ids.data(), button_ids.size(),
					CSize(16, 16), xtpImageNormal, (temp_access_bar.m_image_has_alpha ? TRUE : FALSE));
			}
		}

		for (const auto& x : temp_access_bar.m_all_buttons)
		{
			tmp_ctrl = GetQuickAccessControls()->Add(xtpControlButton, x.m_id);
			tmp_ctrl->SetCaption(x.m_caption);
			tmp_ctrl->SetTooltip(x.m_tool_tip);
			tmp_ctrl->SetIconId(x.m_id);
			tmp_ctrl->SetDescription(_T(""));
			tmp_ctrl->SetCaption(x.m_caption);
			tmp_ctrl->SetHideFlag(xtpHideGeneric,x.m_is_hide);
		}

		GetQuickAccessControls()->CreateOriginalControls();
	}

	SetCloseable(FALSE);

	// Call it only if your Ribbon is support customization using Ribbon Customization page.
	// EnableCustomization(TRUE);

	EnableFrameTheme(TRUE);

	// 以下三个风格在主题发生变化时仍要调用
	//SetGroupsCaptionVisible(FALSE);
	//EnableQuickAccessPosContextMenuItem(FALSE);
	//EnableQuickAccessCustomItem(FALSE);

	return true;
}

void app_ribbon_bar::initlize_ribbon_bar_file_menu(const ribbon_bar_config_data &ribbon_ui_config)
{
	CMenu file_menu;
	file_menu.CreateMenu();

	for (const auto &x : ribbon_ui_config.m_file_menu.m_menu_items)
	{
		if (_T("-") == x.m_caption)
			file_menu.AppendMenu(MF_SEPARATOR);
		else
			file_menu.AppendMenu(MF_STRING | MF_UNCHECKED, x.m_id, x.m_caption);
	}
	file_menu.AppendMenu(MF_STRING | MF_UNCHECKED | MF_MENUBARBREAK, ID_FILE_RECENTDOCUMENTS, _T("最近打开的文件"));

	SetMenu(nullptr); // 清除系统自带的主菜单（如果有的话）

	const ribbon_file_menu_bar& temp_file_menu_bar = ribbon_ui_config.m_file_menu;

	vector<unsigned int> button_ids;
	for (const auto& x : temp_file_menu_bar.m_menu_items)
	{
		if (_T("-") != x.m_caption)
			button_ids.push_back(x.m_id);
	}

	std::sort(button_ids.begin(), button_ids.end());

	if (button_ids[0] != IDS_RIBBON_FILE_MENU_MIN)
		button_ids.push_back(IDS_RIBBON_FILE_MENU_MIN);

	std::sort(button_ids.begin(), button_ids.end());

	for (int i = 0; i < (int)button_ids.size() - 1; ++i)
	{
		if ((button_ids[i + 1] - button_ids[i]) > 1)
		{
			button_ids.push_back(button_ids[i] + 1);
			std::sort(button_ids.begin(), button_ids.end());
		}
	}

	CString strPath = file_tools::get_app_path_with_xg() + _T("gui_xml\\") + temp_file_menu_bar.m_image_file_name;

	CImage img;
	if (S_OK == img.Load(strPath))
	{
		HBITMAP hBitmap = img.Detach();
		if (NULL != hBitmap)
		{
			CBitmap bitmap;
			bitmap.Attach(hBitmap);

			// 图标元素个数比最大索多一个，超过的索引无图标
			GetImageManager()->SetIcons(bitmap, button_ids.data(), button_ids.size(),
				CSize(32, 32), xtpImageNormal, (temp_file_menu_bar.m_image_has_alpha ? TRUE : FALSE));
		}
	}

	CXTPControlPopup* pControlFile = AddSystemButton();
	pControlFile->SetCommandBar(&file_menu);
	pControlFile->SetIconId(IDR_MAINFRAME); // 如果图标尺寸不是32*32，则要重新使用GetImageManager()->SetIcons()加载
	pControlFile->GetCommandBar()->SetIconSize(CSize(32, 32));
	pControlFile->SetCaption(_T("文件"));
	pControlFile->SetKeyboardTip(_T("F"));
	pControlFile->SetTooltip(_T("点击执行文件操作"));
}

void app_ribbon_bar::active_context_tabs_by_window_class(
	const std::vector<CString>& all_active_view_class /*= std::vector<CString>(0)*/,
	bool is_selected /*= true*/)
{
	if (m_context_tab_info_cache.empty()) return;

	// all_active_view_class为空表示所有上下文菜单都隐藏

	CXTPRibbonTab* sel_tab = nullptr; // 记录第一个激活的上下文菜单

	for (size_t i = 0; i < m_context_tab_info_cache.size(); ++i)
	{
		const tuple<CXTPRibbonTab*, CString, bool>& item = m_context_tab_info_cache[i];
		if (nullptr == std::get<0>(item)) continue;

		// 拆解当前tab关联的所有窗口类
		vector<CString> all_names;
		string_tools::split_string(all_names, std::get<1>(item));

		if (all_names.empty() || all_active_view_class.empty())
		{
			// 没有激活串或者all_active_view_class为空表示不会被激活
			std::get<0>(item)->SetVisible(FALSE);
			continue;
		}

		// 如果此标签设置了不自动激活则不显示
		if (false == std::get<2>(item))
		{
			app_ui_interface* app = get_app_ui_interface();
			if (app)
			{
				window_interface* wnd = app->get_current_working_space_windows();
				if (wnd) wnd->active_context_ribbon_tab();
			}

			// 不自动激活是为了让用户手动处理，框架不能把用户设置覆盖了，切换到新窗口后加载响应的tab，此处不
			// 隐藏，后面的代码也会因为此tab与传入的窗口类不匹配而被隐藏，因此不会有后遗症[Add by WYZ]
			// std::get<0>(item)->SetVisible(FALSE);
			continue;
		}

		// 依次使用传入的窗口类在当前tab关联的所有窗口类中查找是否存在，存在则激活，否则不激活
		bool bExist = false;

		for (auto& x : all_active_view_class)
		{
			if (x.IsEmpty()) continue; // 为空忽略

			for (auto& y : all_names)
			{
				if (0 == x.CompareNoCase(y))
				{
					bExist = true;
					break;
				}
			}

			if (bExist) break;
		}

		if (bExist)
		{
			std::get<0>(item)->SetVisible(TRUE);
		
			// 已经有要选中的标签则不再赋值
			if (nullptr == sel_tab) sel_tab = std::get<0>(item);
		}
		else
		{
			std::get<0>(item)->SetVisible(FALSE);
		}
	}

	// 激活第一个
	if (sel_tab && is_selected) SetCurSel(sel_tab->GetIndex());
}

void app_ribbon_bar::active_context_tabs_always_by_tab_name(const std::vector<CString>& all_active_tabs)
{
	//for (const CString& tab_name : all_active_tabs)
	//{
	//	// 只处理上下文标签
	//	bool need_show = false;
	//	for (tuple<CXTPRibbonTab*, CString, bool>& x : m_context_tab_info_cache)
	//	{
	//		CXTPRibbonTab* tab = std::get<0>(x);
	//		if (!tab) continue;

	//		if (0 == tab->GetCaption().CompareNoCase(tab_name))
	//		{
	//			tab->SetVisible(TRUE);
	//			need_show = true;
	//			break; // 多个标签可以对应一个窗口，只要找到就可以了
	//		}
	//	}

	//	if (!need_show) tab->SetVisible(FALSE); // 循环结束后才能知道要隐藏		
	//}


	// 只处理上下文标签
	for (tuple<CXTPRibbonTab*, CString, bool>& x : m_context_tab_info_cache)
	{
		CXTPRibbonTab* tab = std::get<0>(x);
		if (!tab) continue;

		bool finded_tab = false;
		for (const CString& tab_name : all_active_tabs)
		{
			if (0 == tab->GetCaption().CompareNoCase(tab_name))
			{
				finded_tab = true;
				break;
			}
		}

		tab->SetVisible(finded_tab ? TRUE : FALSE);
	}
}

void app_ribbon_bar::select_tab(const CString& tab_name)
{
	for (int i = 0; i < GetTabCount(); ++i)
	{
		CXTPRibbonTab* tab = GetTab(i);
		if (!tab) return;
		
		// 标签不显示不切换，对于上下文标签来说，经常出现不同分组的上下文标签名相同，但不同窗口只显示相应的分
		// 组，这时界面上不重复，因此要兼容这种情况,但显示出来的所有标签均不可重名，否则请合为一组
		if (FALSE == tab->IsVisible()) continue;

		if (0 == tab->GetCaption().CompareNoCase(tab_name))
		{
			SetCurSel(i);
			return;
		}
	}
}

CXTPControl* app_ribbon_bar::get_control(int id) const
{
	for (int i = 0; i < GetTabCount(); ++i)
	{
		CXTPRibbonTab* tab = GetTab(i);
		if (!tab) continue;

		CXTPRibbonGroups* gps = tab->GetGroups();
		if (!gps) continue;

		for (int j = 0; j < gps->GetCount(); ++j)
		{
			CXTPRibbonGroup* gp = gps->GetAt(j);
			if (!gp) continue;

			CXTPControl* ctrl = gp->FindControl(id);
			if (ctrl) return ctrl;
		}
	}

	return nullptr;
}

void app_ribbon_bar::OnTabChanged(CXTPRibbonTab* tab)
{
	CXTPRibbonBar::OnTabChanged(tab);

	CString param = tab->GetContextCaption() + _T("#") + tab->GetCaption();
	cmd_trigger_manager::fire(_T("app"), _T("ribbon_tab_select_changed"), param);
}


IMPLEMENT_SERIAL(ribbon_bar_data_controlbase, CObject, 1)

ribbon_bar_data_controlbase::ribbon_bar_data_controlbase()
	: CObject()
{
	// 控件ID，整个Ribbon中不能重复，而且最好与界面同一窗口上的菜单栏、工具栏中的ID不相同
	m_id = 0;

	m_caption = _T("");
	m_style = xtpButtonIconAndCaptionBelow;
	
	// 图标ID，每一个选项卡下的所有图标ID均不相同（如果相同则图标一样）
	// 注意，默认值不能赋为大于等于0的值，因为有些控件是没有图标的，这样默认为0时会将图标
	// 索引为0的控件图标冲掉，-1表示无效
	m_icon_id = -1;
	
	m_tool_tip = _T("");
	m_acc_key = _T("");		// 键盘选择键
	m_command = _T("");

}

ribbon_bar_data_controlbase::ribbon_bar_data_controlbase( const ribbon_bar_data_controlbase& control )
{
	*this = control;
}

ribbon_bar_data_controlbase::~ribbon_bar_data_controlbase()
{

}

void ribbon_bar_data_controlbase::init_ribbon_control(CXTPControl* control)
{
	if (NULL == control || FALSE == control->IsKindOf(RUNTIME_CLASS(CXTPControl))) return;

	control->SetID(m_id);
	control->SetCaption(m_caption);
	control->SetStyle(m_style);
	control->SetIconId(m_id);				// xml中的图标ID是方便用户编辑的，程序内部最终会被转换成控件ID了，所以此处要使用控件的图标ID
	control->SetTooltip(m_caption);
	control->SetDescription(m_tool_tip);
	
	control->SetKeyboardTip(m_acc_key);
}

void ribbon_bar_data_controlbase::init_ribbon_control_data( tinyxml2::XMLElement* node )
{
	if (!node) return;

	m_caption = get_xml_node_attribute_str(node, _T("caption"));
	m_style = (XTPButtonStyle) get_xml_node_attribute_int(node, _T("style"));
	m_icon_id = get_xml_node_attribute_int(node, _T("icon_index"));
	m_tool_tip = get_xml_node_attribute_str(node, _T("tooltip"));
	m_acc_key = get_xml_node_attribute_str(node, _T("key"));
	
	CString command_and_parameter_text = get_xml_node_attribute_str(node, _T("cmd"));
	vector<CString> cmd_items;
	string_tools::split_string(cmd_items, command_and_parameter_text, _T("|"));
	cmd_items.resize(2);
	m_command = cmd_items.front();
	m_cmd_params = cmd_items.back();
}

ribbon_bar_data_controlbase* ribbon_bar_data_controlbase::operator=( const ribbon_bar_data_controlbase& control )
{
	m_id			= control.m_id;
	m_caption	= control.m_caption;
	m_style		= control.m_style;
	m_icon_id		= control.m_icon_id;
	m_tool_tip	= control.m_tool_tip;
	m_acc_key		= control.m_acc_key;
	m_command		= control.m_command;
	m_cmd_params	= control.m_cmd_params;

	return this;
}

ribbon_bar_data_controlbase* ribbon_bar_data_controlbase::clone_control_data() const
{
	// 从堆上使用new分配派生类对象并拷贝数据后返回派生类指针，生成的对象所占内存由外部调用的
	// 地方处理，派生类自己生成自己的对象并使用拷贝函数复制数据并返回基类指针，最好不要调用
	// 基础的接口

	ribbon_bar_data_controlbase* control = new ribbon_bar_data_controlbase();
	*control = *this;
	return control;
}

XTPControlType ribbon_bar_data_controlbase::get_ribbon_control_type() const
{
	return xtpControlButton;
}

std::vector<ribbon_control_id_and_icon_id> ribbon_bar_data_controlbase::get_control_and_icon_id() const
{
	// 各个派生类仅计算自己新加的带ID的命令控件，再加上基类的即可
	vector<ribbon_control_id_and_icon_id> vecID;

	ribbon_control_id_and_icon_id nID;
	nID.id = m_id;
	nID.icon_id = m_icon_id;

	vecID.push_back(nID);

	return vecID;
}

int ribbon_bar_data_controlbase::set_control_id( int start_id )
{
	m_id = start_id;
	++start_id;

	return start_id;
}

bool ribbon_bar_data_controlbase::get_command_and_parameter(int id, std::array<CString, 3>& info) const
{
	info[0] = _T("");
	info[1] = _T("");
	info[2] = _T("");

	if (m_id != id) return false;

	cmd_container_interface::parse_command(m_command, info[0], info[1]);
	info[2] = m_cmd_params;

	return true;
}

void ribbon_bar_data_controlbase::get_all_control_id_and_cmd(std::vector<std::pair<int, CString>>& info) const
{
	info.clear();
	info.push_back(make_pair(m_id, m_command));
}

ribbon_bar_data_group::ribbon_bar_data_group()
{
	m_control_in_group.clear();

}

ribbon_bar_data_group::ribbon_bar_data_group( const ribbon_bar_data_group& group )
{
	*this = group;
}

ribbon_bar_data_group::~ribbon_bar_data_group()
{
	for (size_t i = 0; i < m_control_in_group.size(); ++i)
		delete m_control_in_group[i];

	m_control_in_group.clear();
}

ribbon_bar_data_group* ribbon_bar_data_group::operator=( const ribbon_bar_data_group& group )
{
	m_group_caption = group.m_group_caption;

	for (size_t i = 0; i < m_control_in_group.size(); ++i)
		delete m_control_in_group[i];

	m_control_in_group.clear();

	for (size_t i = 0; i < group.m_control_in_group.size(); ++i)
		m_control_in_group.push_back(group.m_control_in_group[i]->clone_control_data());

	return this;
}

void ribbon_bar_data_group::InitRibbonGroupData( tinyxml2::XMLElement* node )
{
	if (!node) return;

	m_group_caption = get_xml_node_attribute_str(node, _T("caption"));

	tinyxml2::XMLElement* control_node = node->FirstChildElement("control");
	m_control_in_group.clear();
	if (!control_node) return;

	do
	{
		CString controlType = get_xml_node_attribute_str(control_node, _T("type"));

		CObject* pObj = CRuntimeClass::CreateObject(controlType);
		if (pObj)
		{
			ribbon_bar_data_controlbase* pControl = dynamic_cast<ribbon_bar_data_controlbase*>(pObj);
			if (!pControl)
			{
				safe_delete(pObj); // 防止在错写成其它支持动态创建的类而成功创建，因此要删除
				continue; // 如果不是控件类或派生类则丢弃
			}

			pControl->init_ribbon_control_data(control_node);
			m_control_in_group.push_back(pControl);
		}

		control_node = control_node->NextSiblingElement("control");
	} while (control_node);
}

int ribbon_bar_data_group::GetMaxIconIndex() const
{
	int nMax = -1;
	vector<ribbon_control_id_and_icon_id> vecIconID;
	for (size_t i = 0; i < m_control_in_group.size(); ++i)
	{
		vector<ribbon_control_id_and_icon_id> vecIconID = m_control_in_group[i]->get_control_and_icon_id();

		for (size_t j = 0; j < vecIconID.size(); ++j)
			if (nMax < vecIconID[j].icon_id) nMax = vecIconID[j].icon_id;
	}

	return nMax;
}

std::vector<ribbon_control_id_and_icon_id> ribbon_bar_data_group::get_control_and_icon_id() const
{
	std::vector<ribbon_control_id_and_icon_id> vecID;
	for (size_t i = 0; i < m_control_in_group.size(); ++i)
	{
		std::vector<ribbon_control_id_and_icon_id> vecGPID = m_control_in_group[i]->get_control_and_icon_id();
		if (!vecGPID.empty()) vecID.insert(vecID.end(), vecGPID.begin(), vecGPID.end());
	}

	return vecID;

}

ribbon_bar_data_tab::ribbon_bar_data_tab()
{
	m_tab_caption = _T("");
	m_icon_file_name = _T("");
	m_image_has_alpha = false;
	m_context_name = _T("");
	m_context_color = xtpRibbonTabContextColorNone;
	m_active_view_class = _T("");
	m_context_auto_active = false;

	m_all_groups.clear();
}

ribbon_bar_data_tab::~ribbon_bar_data_tab()
{

}

void ribbon_bar_data_tab::InitRibbonTabData( tinyxml2::XMLElement* node )
{
	if (!node) return;

	m_tab_caption = get_xml_node_attribute_str(node, _T("caption"));
	m_tab_acc_key = get_xml_node_attribute_str(node, _T("key"));
	m_icon_file_name = get_xml_node_attribute_str(node, _T("icons_file"));
	m_image_has_alpha = get_xml_node_attribute_bool(node, _T("image_has_alpha"));
	m_context_name = get_xml_node_attribute_str(node, _T("context"));
	m_context_color = (XTPRibbonTabContextColor) get_xml_node_attribute_int(node, _T("context_color"));
	m_active_view_class = get_xml_node_attribute_str(node, _T("active_view_class"));
	m_context_auto_active = get_xml_node_attribute_bool(node, _T("context_auto_active"));

	m_all_groups.clear();

	tinyxml2::XMLElement* group_node = node->FirstChildElement("group");
	if (!group_node || group_node->NoChildren()) return;

	do
	{
		ribbon_bar_data_group groupData;
		groupData.InitRibbonGroupData(group_node);

		// 空的分组也是有效的，原样保存
		m_all_groups.push_back(groupData);

		group_node = group_node->NextSiblingElement("group");
	} while (group_node);
}

int ribbon_bar_data_tab::GetMaxIconIndex() const
{
	int nMax = -1;

	for (size_t i = 0; i < m_all_groups.size(); ++i)
	{
		int nGPMax = m_all_groups[i].GetMaxIconIndex();
		if (nMax < nGPMax) nMax = nGPMax;
	}

	return nMax;
}

std::vector<ribbon_control_id_and_icon_id> ribbon_bar_data_tab::get_control_and_icon_id() const
{
	std::vector<ribbon_control_id_and_icon_id> vecID;
	for (size_t i = 0; i < m_all_groups.size(); ++i)
	{
		std::vector<ribbon_control_id_and_icon_id> vecGPID = m_all_groups[i].get_control_and_icon_id();
		if (!vecGPID.empty()) vecID.insert(vecID.end(), vecGPID.begin(), vecGPID.end());
	}

	return vecID;
}

ribbon_bar_config_data::ribbon_bar_config_data()
{
	m_all_tabs.clear();
}

ribbon_bar_config_data::~ribbon_bar_config_data()
{

}

void ribbon_bar_config_data::initlize_config_data( tinyxml2::XMLElement* node )
{
	if (!node) return;

	//从opton_info中读取信息，创建对话框
	tinyxml2::XMLElement* options_node = node->FirstChildElement("options");
	if (options_node) m_dialog_info.initlize_config_data(options_node);

	m_quick_access.m_all_buttons.clear();
	tinyxml2::XMLElement* quick_bar_node = node->FirstChildElement("quick_access_bar");
	if (quick_bar_node) m_quick_access.initlize_config_data(quick_bar_node);

	m_file_menu.m_menu_items.clear();
	tinyxml2::XMLElement* file_menu_node = node->FirstChildElement("file_menu");
	if (file_menu_node) m_file_menu.initlize_config_data(file_menu_node);

	m_all_tabs.clear();
	tinyxml2::XMLElement* tab_node = node->FirstChildElement("tab");
	if (!tab_node || tab_node->NoChildren()) return;

	do 
	{
		ribbon_bar_data_tab tabData;
		tabData.InitRibbonTabData(tab_node);

		// 不管tab中是否有控件，均为有效的Tab
		m_all_tabs.push_back(tabData);

		tab_node = tab_node->NextSiblingElement("tab");
	} while (tab_node);

	int nID = IDS_RIBBON_UI_MIN;

	// 所有控件加载之后，重新生成内部的控件ID值
	for (size_t i = 0; i < m_all_tabs.size(); ++i)
	{
		ribbon_bar_data_tab& tab = m_all_tabs[i];

		for (size_t iGroup = 0; iGroup < tab.m_all_groups.size(); ++iGroup)
		{
			ribbon_bar_data_group& group = tab.m_all_groups[iGroup];

			for (size_t iControl = 0; iControl < group.m_control_in_group.size(); ++iControl)
			{
				if (!group.m_control_in_group[iControl]) continue;

				nID = group.m_control_in_group[iControl]->set_control_id(nID);
				if (nID > IDS_RIBBON_UI_AUTO_MAX)
				{
					nID = IDS_RIBBON_UI_MIN;
					_ASSERT((_T("Ribbon中可自动配置的命令ID只允许在 IDS_RIBBON_UI_MIN 到 IDS_RIBBON_UI_AUTO_MAX 范围内"), false));
				}
			}
		}
	}
}

bool ribbon_bar_config_data::get_command_and_parameter(int id, std::array<CString, 3>& info) const
{
	info[0] = _T("");
	info[1] = _T("");
	info[2] = _T("");

	for (const ribbon_bar_data_tab& tab : m_all_tabs)
		for (const ribbon_bar_data_group& group : tab.m_all_groups)
			for (ribbon_bar_data_controlbase* ctrl : group.m_control_in_group)
				if (ctrl && ctrl->get_command_and_parameter(id, info)) return true;

	return false;
}

int ribbon_bar_config_data::get_id(const CString& domain, const CString& cmd, const CString& param /*= _T("")*/, bool compare_param /*= false*/)
{
	if (cmd.IsEmpty()) return -1; // 只有命令不可以为空，其它则不限

	vector<pair<int, CString>> id_info;
	get_all_control_id_and_cmd(id_info);

	for (auto& x : id_info)
	{
		array<CString, 3> cmd_info;
		if (!get_command_and_parameter(x.first, cmd_info)) continue;

		if (0 != domain.CompareNoCase(cmd_info[0])) continue;
		if (0 != cmd.CompareNoCase(cmd_info[1])) continue;
		if (compare_param && 0 != param.CompareNoCase(cmd_info[2])) continue;

		return x.first;
	}

	return -1;
}

void ribbon_bar_config_data::get_all_control_id_and_cmd(std::vector<std::pair<int, CString>>& info) const
{
	info.clear();
	for (const ribbon_bar_data_tab& tab : m_all_tabs)
	{
		for (const ribbon_bar_data_group& group : tab.m_all_groups)
		{
			for (ribbon_bar_data_controlbase* ctrl : group.m_control_in_group)
			{
				if (!ctrl) continue;
				vector<pair<int, CString>> current_info;
				ctrl->get_all_control_id_and_cmd(current_info);
				if (!current_info.empty()) info.insert(info.end(), current_info.begin(), current_info.end());
			}
		}
	}
}

void ribbon_bar_config_data::get_all_command_items_of_special_window_class_in_context_tab(std::vector<CString>& cmd_list, const CString& window_class)
{
	cmd_list.clear();
	if (window_class.IsEmpty()) return;

	for (const ribbon_bar_data_tab& tab : m_all_tabs)
	{
		if (tab.m_context_name.IsEmpty() || tab.m_active_view_class.IsEmpty()) continue;
		if (tab.m_active_view_class != window_class) continue; // 类名必须区分大小写

		for (const ribbon_bar_data_group& group : tab.m_all_groups)
		{
			for (ribbon_bar_data_controlbase* ctrl : group.m_control_in_group)
			{
				if (!ctrl) continue;

				vector<pair<int, CString>> info;
				ctrl->get_all_control_id_and_cmd(info);
				for (auto& x : info) cmd_list.push_back(x.second);
			}
		}
	}
}



IMPLEMENT_SERIAL(ribbon_bar_data_button, ribbon_bar_data_controlbase, 1)
ribbon_bar_data_button::ribbon_bar_data_button()
	: ribbon_bar_data_controlbase()
{

}

ribbon_bar_data_button::~ribbon_bar_data_button()
{

}

void ribbon_bar_data_button::init_ribbon_control(CXTPControl* control)
{
	ribbon_bar_data_controlbase::init_ribbon_control(control);

	// if (!control || FALSE == control->IsKindOf(CXTPControlButton)) return;
	
}

void ribbon_bar_data_button::init_ribbon_control_data(tinyxml2::XMLElement* node)
{
	ribbon_bar_data_controlbase::init_ribbon_control_data(node);
}

ribbon_bar_data_controlbase* ribbon_bar_data_button::clone_control_data() const
{
	ribbon_bar_data_button* pButton = new ribbon_bar_data_button();
	*pButton = *this; // 基类的数据也拷贝了

	return pButton;
}

XTPControlType ribbon_bar_data_button::get_ribbon_control_type() const
{
	return xtpControlButton;
}

std::vector<ribbon_control_id_and_icon_id> ribbon_bar_data_button::get_control_and_icon_id() const
{
	return ribbon_bar_data_controlbase::get_control_and_icon_id();
}

int ribbon_bar_data_button::set_control_id( int start_id )
{
	return ribbon_bar_data_controlbase::set_control_id(start_id);
}

IMPLEMENT_SERIAL(ribbon_bar_data_check_box, ribbon_bar_data_controlbase, 1)

ribbon_bar_data_check_box::ribbon_bar_data_check_box()
	: ribbon_bar_data_controlbase()
{
	m_is_checked = false;

	// 图标ID，每一个选项卡下的所有图标ID均不相同（如果相同则图标一样）
	// 注意，默认值不能赋为大于等于0的值，因为有些控件是没有图标的，这样默认为0时会将图标
	// 索引为0的控件图标冲掉，-1表示无效
	m_icon_id = -1;

}

ribbon_bar_data_check_box::~ribbon_bar_data_check_box()
{

}

void ribbon_bar_data_check_box::init_ribbon_control( CXTPControl* control )
{
	ribbon_bar_data_controlbase::init_ribbon_control(control);

	if (NULL == control || FALSE == control->IsKindOf(RUNTIME_CLASS(CXTPControlCheckBox))) return;

	control->SetChecked((m_is_checked ? TRUE : FALSE));

}

XTPControlType ribbon_bar_data_check_box::get_ribbon_control_type() const
{
	return xtpControlCheckBox;
}

void ribbon_bar_data_check_box::init_ribbon_control_data(tinyxml2::XMLElement* node)
{
	ribbon_bar_data_controlbase::init_ribbon_control_data(node);

	// 复选框禁止设置图标，但要设置ID，基类中默认图标索引为-1，表示不设置图标
	// 读取xml文档时可能会因为用户错误设置或者包装的xml读取工具读取到0或其它非常值，影响到
	// 复选框的图标索引，所以基类处理之后，复选框派生类必须将图标索引再次标记为-1
	m_icon_id = -1;

	// 本构件的数据与基类数据一致,不过在初始化基类的数据之后,还要设置一下选中状态
	if (!node) m_is_checked = false;

	m_is_checked = get_xml_node_attribute_bool(node, _T("checked"));
}

ribbon_bar_data_controlbase* ribbon_bar_data_check_box::clone_control_data() const
{
	ribbon_bar_data_check_box* control = new ribbon_bar_data_check_box();
	*control = *this;
	return control;
}

std::vector<ribbon_control_id_and_icon_id> ribbon_bar_data_check_box::get_control_and_icon_id() const
{
	return ribbon_bar_data_controlbase::get_control_and_icon_id();
}

int ribbon_bar_data_check_box::set_control_id( int start_id )
{
	return ribbon_bar_data_controlbase::set_control_id(start_id);
}

IMPLEMENT_SERIAL(ribbon_bar_data_combo, ribbon_bar_data_controlbase, 1)

ribbon_bar_data_combo::ribbon_bar_data_combo()
{
	m_all_item_list.clear();
	m_width = 60;
}

ribbon_bar_data_combo::~ribbon_bar_data_combo()
{

}

void ribbon_bar_data_combo::init_ribbon_control( CXTPControl* control )
{
	// 初始化基类的一些东西，复选框不支持设置标题文字，所以基类的caption属性无效
	ribbon_bar_data_controlbase::init_ribbon_control(control);

	if (NULL == control || FALSE == control->IsKindOf(RUNTIME_CLASS(CXTPControlComboBox))) return;

	CXTPControlComboBox* pComCtrl = (CXTPControlComboBox*)control;
	pComCtrl->ResetContent(); // 清空所有列表项目
	pComCtrl->SetWidth(m_width);

	int nSelectedIndex = -1; // 记录第一个被设置选中属性的索引
	
	for (size_t i = 0; i < m_all_item_list.size(); ++i)
	{
		pComCtrl->AddString(m_all_item_list[i].text);
		
		if (-1 == nSelectedIndex && m_all_item_list[i].is_selected)
			nSelectedIndex = (int)i;
	}

	if (-1 == nSelectedIndex) nSelectedIndex = 0; // 没有任何项被选中则默认选中第0个
	
	pComCtrl->SetCurSel(nSelectedIndex);
}

XTPControlType ribbon_bar_data_combo::get_ribbon_control_type() const
{
	return xtpControlComboBox;
}

void ribbon_bar_data_combo::init_ribbon_control_data(tinyxml2::XMLElement* node)
{
	m_all_item_list.clear();

	ribbon_bar_data_controlbase::init_ribbon_control_data(node);
	if (!node) return;

	m_width = get_xml_node_attribute_int(node, _T("width"));
	if (m_width <= 0) m_width = 60; // xml中没有指定则使用默认值

	tinyxml2::XMLElement* control_node = node->FirstChildElement("list_item");
	if (!control_node) return;

	bool sel_index = false; // 是否有选中项

	do
	{
		list_item item;
		item.text = get_xml_node_attribute_str(control_node, _T("text"));
		item.is_selected = get_xml_node_attribute_bool(control_node, _T("selected"));

		// 第一次出现被选中的条目
		if (!sel_index && item.is_selected)
			sel_index = true;
		else		
			item.is_selected = false; // 不可以有多个同时设置选中，强制改为非选中

		m_all_item_list.push_back(item);
		control_node = control_node->NextSiblingElement("list_item");
	} while (control_node);
}

ribbon_bar_data_controlbase* ribbon_bar_data_combo::clone_control_data() const
{
	ribbon_bar_data_combo* control = new ribbon_bar_data_combo();
	*control = *this;
	return control;
}

std::vector<ribbon_control_id_and_icon_id> ribbon_bar_data_combo::get_control_and_icon_id() const
{
	return ribbon_bar_data_controlbase::get_control_and_icon_id();
}

int ribbon_bar_data_combo::set_control_id( int start_id )
{
	return ribbon_bar_data_controlbase::set_control_id(start_id);
}

bool ribbon_bar_data_combo::get_command_and_parameter(int id, std::array<CString, 3>& info) const
{
	info[0] = _T("");
	info[1] = _T("");
	info[2] = _T("");

	if (!ribbon_bar_data_controlbase::get_command_and_parameter(id, info)) return false;

	// 命令参数在下拉控件中不由xml指定，框架总是将当前的选中项的文本作为参数
	main_application* app = (main_application*) AfxGetApp();
	if (!app) return false;

	main_frame* main_wnd = dynamic_cast<main_frame*>(app->m_pMainWnd);
	if (!main_wnd) return false;

	app_ribbon_bar* rib = main_wnd->get_db_ribbon_bar();
	if (!rib || !rib->GetSafeHwnd()) return false;

	CXTPControlComboBox* combo = dynamic_cast<CXTPControlComboBox*>(rib->get_control(id));
	if (!combo) return false;

	info[2] = combo->GetText();
	return true;
}

IMPLEMENT_SERIAL(ribbon_bar_data_whole_popup_button, ribbon_bar_data_controlbase, 1)

ribbon_bar_data_whole_popup_button::ribbon_bar_data_whole_popup_button()
{
	m_all_item_list.clear();
}

ribbon_bar_data_whole_popup_button::~ribbon_bar_data_whole_popup_button()
{

}

void ribbon_bar_data_whole_popup_button::init_ribbon_control(CXTPControl* control)
{
	ribbon_bar_data_controlbase::init_ribbon_control(control);

	if (NULL == control || FALSE == control->IsKindOf(RUNTIME_CLASS(CXTPControlPopup))) return;

	CXTPControlPopup* pPopUpCtrl = (CXTPControlPopup*)control;

	CMenu popMenu;
	popMenu.CreateMenu();
	if (nullptr == popMenu.GetSafeHmenu()) return;
	if (m_all_item_list.empty()) return;

	for (size_t i = 0; i < m_all_item_list.size(); ++i)
		popMenu.AppendMenu(MF_STRING, m_all_item_list[i].id,
		m_all_item_list[i].text);

	// xtp库会在加菜单的时候自动查找对应ID的图标并加载显示，所以不用再手动调用设置菜单项图标的代码
	pPopUpCtrl->SetCommandBar(&popMenu);
}

XTPControlType ribbon_bar_data_whole_popup_button::get_ribbon_control_type() const
{
	return xtpControlButtonPopup;
}

void ribbon_bar_data_whole_popup_button::init_ribbon_control_data(tinyxml2::XMLElement* node)
{
	ribbon_bar_data_controlbase::init_ribbon_control_data(node);

	if (!node) return;

	m_all_item_list.clear();

	tinyxml2::XMLElement* control_node = node->FirstChildElement("popup_item");
	if (!control_node) return;

	do
	{
		popup_list_item item;
		//item.nID = itemData->IntAttribute(L"id");
		item.text = get_xml_node_attribute_str(control_node, _T("text"));
		item.icon_id = get_xml_node_attribute_int(control_node, _T("icon_index"));

		CString command_and_parameter_text = get_xml_node_attribute_str(control_node, _T("cmd"));
		vector<CString> cmd_items;
		string_tools::split_string(cmd_items, command_and_parameter_text, _T("|"));
		cmd_items.resize(2);
		item.cmd = cmd_items.front();
		item.cmd_params = cmd_items.back();

		m_all_item_list.push_back(item);

		control_node = control_node->NextSiblingElement("popup_item");
	} while (control_node);
}

ribbon_bar_data_controlbase* ribbon_bar_data_whole_popup_button::clone_control_data() const
{
	ribbon_bar_data_whole_popup_button* control = new ribbon_bar_data_whole_popup_button();
	*control = *this;
	return control;
}

std::vector<ribbon_control_id_and_icon_id> ribbon_bar_data_whole_popup_button::get_control_and_icon_id() const
{
	// 本身也算一个有效的命令按钮
	vector<ribbon_control_id_and_icon_id> vecBase = ribbon_bar_data_controlbase::get_control_and_icon_id();

	// 再加上弹出菜单的条目数（仅包含有效的条目，分隔条不算，因为不可以设置ID，注意不包含二级子菜单）
	if (!m_all_item_list.empty())
	{
		for (size_t i = 0; i < m_all_item_list.size(); ++i)
		{
			ribbon_control_id_and_icon_id nID;

			nID.id = m_all_item_list[i].id;
			nID.icon_id = m_all_item_list[i].icon_id;

			vecBase.push_back(nID);
		}
	}

	return vecBase;
}

int ribbon_bar_data_whole_popup_button::set_control_id( int start_id )
{
	int nResult = ribbon_bar_data_controlbase::set_control_id(start_id);
	
	if (!m_all_item_list.empty())
	{
		for (size_t i = 0; i < m_all_item_list.size(); ++i)
		{
			if (nResult > IDS_RIBBON_UI_AUTO_MAX)
			{
				nResult = IDS_RIBBON_UI_MIN;
				_ASSERT((_T("Ribbon中可自动配置的命令ID只允许在 IDS_RIBBON_UI_MIN 到")
					_T(" IDS_RIBBON_UI_AUTO_MAX 范围内"), false));
			}

			m_all_item_list[i].id = nResult;
			++nResult;
		}
	}

	return nResult;
}

bool ribbon_bar_data_whole_popup_button::get_command_and_parameter(int id, std::array<CString, 3>& info) const
{
	info[0] = _T("");
	info[1] = _T("");
	info[2] = _T("");

	if (ribbon_bar_data_controlbase::get_command_and_parameter(id, info)) return true;

	for (const popup_list_item& item : m_all_item_list)
	{
		if (item.id != id) continue;

		cmd_container_interface::parse_command(item.cmd, info[0], info[1]);
		info[2] = item.cmd_params;

		return true;
	}

	return false;
}

void ribbon_bar_data_whole_popup_button::get_all_control_id_and_cmd(std::vector<std::pair<int, CString>>& info) const
{
	info.clear();
	ribbon_bar_data_controlbase::get_all_control_id_and_cmd(info);

	for (const popup_list_item& item : m_all_item_list)
		info.push_back(make_pair(item.id, item.cmd));
}


IMPLEMENT_SERIAL(ribbon_bar_data_split_popup_button, ribbon_bar_data_whole_popup_button, 1)

ribbon_bar_data_split_popup_button::ribbon_bar_data_split_popup_button()
{

}

ribbon_bar_data_split_popup_button::~ribbon_bar_data_split_popup_button()
{

}

XTPControlType ribbon_bar_data_split_popup_button::get_ribbon_control_type() const
{
	return xtpControlSplitButtonPopup;
}

ribbon_bar_data_controlbase* ribbon_bar_data_split_popup_button::clone_control_data() const
{
	ribbon_bar_data_split_popup_button* control = new ribbon_bar_data_split_popup_button();
	*control = *this;
	return control;
}

ribbon_bar_data_quick_access_button::ribbon_bar_data_quick_access_button()
{

}

ribbon_bar_data_quick_access_button::~ribbon_bar_data_quick_access_button()
{

}

void ribbon_bar_data_quick_access_button::initlize_config_data(tinyxml2::XMLElement* node) 
{
	if (!node) return;
	m_caption = get_xml_node_attribute_str(node,_T("caption"));
	m_tool_tip = get_xml_node_attribute_str(node, _T("tooltip"));
	m_icon_id = get_xml_node_attribute_int(node,_T("icon_index"));
	m_acc_key = get_xml_node_attribute_str(node,_T("key"));
	m_is_hide = get_xml_node_attribute_bool(node, _T("hide"));

	m_id = IDS_RIBBON_QUICK_ACCESS_MIN + m_icon_id;
	if (m_id > IDS_RIBBON_QUICK_ACCESS_MAX) m_id = IDS_RIBBON_QUICK_ACCESS_MIN;

	CString command_and_parameter_text = get_xml_node_attribute_str(node, _T("cmd"));
	vector<CString> cmd_items;
	string_tools::split_string(cmd_items, command_and_parameter_text, _T("|"));
	cmd_items.resize(2);
	m_cmd = cmd_items.front();
	m_cmd_params = cmd_items.back();
}

bool ribbon_bar_data_quick_access_button::get_command_and_parameter(int id, std::array<CString, 3>& info) const
{
	info[0] = _T("");
	info[1] = _T("");
	info[2] = _T("");

	if (m_id != id) return false;

	cmd_container_interface::parse_command(m_cmd, info[0], info[1]);
	info[2] = m_cmd_params;

	return true;

}

ribbon_bar_data_file_menu_item::ribbon_bar_data_file_menu_item()
{

}

ribbon_bar_data_file_menu_item::~ribbon_bar_data_file_menu_item()
{

}

void ribbon_bar_data_file_menu_item::initlize_config_data(tinyxml2::XMLElement* node) 
{
	if (!node) return;
	m_caption = get_xml_node_attribute_str(node, _T("caption"));
	if (_T("-") == m_caption)
	{
		m_acc_key		= _T("");
		m_cmd		= _T("");
		m_cmd_params	= _T("");
		m_id			= 0;
		m_icon_id		= 0;
	}
	else
	{
		m_icon_id = get_xml_node_attribute_int(node, _T("icon_index"));
		m_acc_key = get_xml_node_attribute_str(node, _T("key"));

		m_id = IDS_RIBBON_FILE_MENU_MIN + m_icon_id;
		if (m_id > IDS_RIBBON_FILE_MENU_MAX) m_id = IDS_RIBBON_FILE_MENU_MIN;

		CString command_and_parameter_text = get_xml_node_attribute_str(node, _T("cmd"));
		vector<CString> cmd_items;
		string_tools::split_string(cmd_items, command_and_parameter_text, _T("|"));
		cmd_items.resize(2);
		m_cmd = cmd_items.front();
		m_cmd_params = cmd_items.back();
	}
}

bool ribbon_bar_data_file_menu_item::get_command_and_parameter(int id, std::array<CString, 3>& info) const
{
	info[0] = _T("");
	info[1] = _T("");
	info[2] = _T("");

	if (m_id != id) return false;

	cmd_container_interface::parse_command(m_cmd, info[0], info[1]);
	info[2] = m_cmd_params;

	return true;
}

ribbon_quick_access_bar::ribbon_quick_access_bar()
{

}

ribbon_quick_access_bar::~ribbon_quick_access_bar()
{

}

void ribbon_quick_access_bar::initlize_config_data(tinyxml2::XMLElement* node)
{
	if (!node) return;
	m_file_name = get_xml_node_attribute_str(node, _T("icons_file"));
	m_image_has_alpha = node->BoolAttribute(_T("image_has_alpha"));

	tinyxml2::XMLElement* item_node = node->FirstChildElement(_T("item"));
	m_all_buttons.clear();
	do
	{
		if (!item_node) return;
		ribbon_bar_data_quick_access_button quick_access_data;
		quick_access_data.initlize_config_data(item_node);
		m_all_buttons.push_back(quick_access_data);

		item_node = item_node->NextSiblingElement("item");
	} while (item_node);
}

bool ribbon_quick_access_bar::has_this_id(UINT id) const
{
	if (m_all_buttons.empty()) return false;

	for (size_t i = 0; i < m_all_buttons.size(); ++i)
	{
		if (m_all_buttons[i].m_id == id) return true;
	}
	return false;
}

bool ribbon_quick_access_bar::get_command_and_parameter(int id, std::array<CString, 3>& info) const
{
	info[0] = _T("");
	info[1] = _T("");
	info[2] = _T("");

	if (!has_this_id(id)) return false;

	for (auto &x: m_all_buttons)
	{
		if (x.get_command_and_parameter(id, info))  return true;
	}

	return false;
}




ribbon_file_menu_bar::ribbon_file_menu_bar()
{

}

ribbon_file_menu_bar::~ribbon_file_menu_bar()
{

}

void ribbon_file_menu_bar::initlize_config_data(tinyxml2::XMLElement* node)
{
	if (!node) return;
	m_image_file_name = get_xml_node_attribute_str(node, _T("icons_file"));
	m_image_has_alpha = get_xml_node_attribute_bool(node, _T("image_has_alpha"));

	tinyxml2::XMLElement* item_node = node->FirstChildElement(_T("item"));
	m_menu_items.clear();
	do
	{
		if (!item_node) return;
		ribbon_bar_data_file_menu_item file_menu_data;
		file_menu_data.initlize_config_data(item_node);
		m_menu_items.push_back(file_menu_data);

		item_node = item_node->NextSiblingElement("item");
	} while (item_node);
}

bool ribbon_file_menu_bar::get_command_and_parameter(int id, std::array<CString, 3>& info) const
{
	info[0] = _T("");
	info[1] = _T("");
	info[2] = _T("");

	auto it = std::find_if(m_menu_items.begin(), m_menu_items.end(), [&](const ribbon_bar_data_file_menu_item& item)
	{
		return id == item.m_id;
	});

	if (m_menu_items.end() == it) return false;

	for (auto &x : m_menu_items)
	{
		if (x.get_command_and_parameter(id, info))  return true;
	}

	return false;
}

option_info::option_info()
{

}

option_info::~option_info()
{

}

void option_info::initlize_config_data(tinyxml2::XMLElement* node)
{
	m_about_dialog_class_name = _T("");

	if (!node || node->NoChildren()) return;

	tinyxml2::XMLElement* dialog_about_node = node->FirstChildElement(_T("about_dialog"));
	if (dialog_about_node)
		m_about_dialog_class_name = get_xml_node_attribute_str(dialog_about_node, _T("dialog_class_name"));
}

