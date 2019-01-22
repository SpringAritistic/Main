#include "stdafx.h"

#include <map>

#include "x_framework/macro.h"
#include "x_framework/tools/file_tools.h"

#include "help_configs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


//////////////////////////////////////////////////////////////////////////

CString help_configs::m_default_page;
std::map<CString, CString> help_configs::m_page_configs_by_window_class_names;

//////////////////////////////////////////////////////////////////////////



help_configs::help_configs()
{

}

help_configs::~help_configs()
{

}

help_configs::help_configs(const help_configs&)
{

}

CString help_configs::get_chm_full_path_name()
{
	return file_tools::get_app_path_with_xg() + _T("help\\app_help.chm");
}

void help_configs::initialize_by_xml()
{
	m_default_page = _T("");
	m_page_configs_by_window_class_names.clear();

	CString xml_file_full_path = file_tools::get_app_path_with_xg() + _T("help\\app_help_config.xml");
	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_SUCCESS != doc.LoadFile(string_tools::CString_to_string(xml_file_full_path).c_str())) return;

	tinyxml2::XMLElement* node = doc.FirstChildElement("app_help_config");
	if (!node || node->NoChildren()) return;

	tinyxml2::XMLElement* default_node = node->FirstChildElement("default");
	if (default_node)
	{
		m_default_page = get_xml_node_attribute_str(default_node, _T("page_path_name"));
	}

	tinyxml2::XMLElement* window_by_class_name_node = node->FirstChildElement("window_by_class_name");
	while (window_by_class_name_node)
	{
		CString window_class_name = get_xml_node_attribute_str(window_by_class_name_node, _T("window_class_name"));
		CString page_path_name = get_xml_node_attribute_str(window_by_class_name_node, _T("page_path_name"));

		if (!window_class_name.IsEmpty() && !page_path_name.IsEmpty())
			m_page_configs_by_window_class_names.insert(make_pair(window_class_name, page_path_name));

		window_by_class_name_node = window_by_class_name_node->NextSiblingElement("window_by_class_name");
	}
}

CString help_configs::get_default_page_full_path()
{
	return get_chm_full_path_name() + _T("::/") + m_default_page;
}

CString help_configs::get_page_full_path_by_window_class_name(const CString& window_class_name)
{
	if (window_class_name.IsEmpty()) return _T("");

	auto it = m_page_configs_by_window_class_names.find(window_class_name);
	if (it == m_page_configs_by_window_class_names.end()) return _T("");

	return get_chm_full_path_name() + _T("::/") + it->second;
}
