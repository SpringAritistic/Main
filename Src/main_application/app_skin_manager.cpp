#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/tools/file_tools.h"

#include "app_skin_manager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


app_skin_manager_window_filter::app_skin_manager_window_filter()
{
}

app_skin_manager_window_filter::~app_skin_manager_window_filter()
{
}

BOOL app_skin_manager_window_filter::FilterWindow(HWND hWnd, LPCTSTR lpszClassName, LPCREATESTRUCT lpcs)
{
	// 构造的时候就加载要过滤掉的窗口类，注意这个窗口类为向系统注册的窗口类，不是CWnd的派生类名！！！
	for (auto& x : app_skin_manager::m_filter_out_registed_window_class_names)
		if (0 == x.CompareNoCase(lpszClassName)) return TRUE;

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////

std::vector<CString> app_skin_manager::m_filter_out_registed_window_class_names;
std::vector<CString> app_skin_manager::m_excluded_module_names;
app_skin_manager_window_filter app_skin_manager::m_skin_filter;


app_skin_manager::app_skin_manager()
{
	load_config_info_from_xml();

	SetWindowFilter(&m_skin_filter);

	for (const auto& x : m_excluded_module_names)
		ExcludeModule(x);
}

app_skin_manager::~app_skin_manager()
{
}

void app_skin_manager::load_config_info_from_xml()
{
	CString xml_file_full_path = file_tools::get_app_path_with_xg() + _T("gui_xml\\skin_manager_filter.xml");
	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_SUCCESS != doc.LoadFile(string_tools::CString_to_string(xml_file_full_path).c_str())) return;

	tinyxml2::XMLElement* node = doc.FirstChildElement("skin_manager_filter");
	if (!node || node->NoChildren()) return;

	tinyxml2::XMLElement* class_node = node->FirstChildElement("registed_window_class_names");
	if (class_node)
	{
		tinyxml2::XMLElement* item_node = class_node->FirstChildElement("item");
		while (item_node)
		{
			CString name = get_xml_node_attribute_str(item_node, _T("name"));
			if (!name.IsEmpty()) m_filter_out_registed_window_class_names.push_back(name);

			item_node = item_node->NextSiblingElement("item");
		}
	}

	tinyxml2::XMLElement* module_node = node->FirstChildElement("filter_out_module_names");
	if (module_node)
	{
		tinyxml2::XMLElement* item_node = module_node->FirstChildElement("item");
		while (item_node)
		{
			CString name = get_xml_node_attribute_str(item_node, _T("name"));
			if (!name.IsEmpty()) m_excluded_module_names.push_back(name);

			item_node = item_node->NextSiblingElement("item");
		}
	}
}

