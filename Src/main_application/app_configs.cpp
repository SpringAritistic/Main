#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/tools/xml_tools.h"
#include "x_framework/tools/string_tools.h"
#include "x_framework/tools/file_tools.h"

#include "app_configs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


bool app_configs::m_enable_splash = true;
bool app_configs::m_enable_skin = false;
CString app_configs::m_application_name = _T("");
bool app_configs::m_show_start_page = false;
CString app_configs::m_image_name = _T("");
bool app_configs::m_only_one_instance = false;


void app_configs::load_settings_from_xml()
{
	// ¿ªÊ¼¶ÁÈ¡xmlÅäÖÃ
	CString xml_file_full_path = file_tools::get_app_path_with_xg() + _T("gui_xml\\app_configs.xml");
	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_SUCCESS != doc.LoadFile(string_tools::CString_to_string(xml_file_full_path).c_str())) return;

	tinyxml2::XMLElement* node = doc.FirstChildElement("app_configs");
	if (!node || node->NoChildren()) return;

	tinyxml2::XMLElement* splash_settings = node->FirstChildElement("splash_settings");
	m_enable_splash = get_xml_node_attribute_bool(splash_settings, _T("enable_splash"));

	tinyxml2::XMLElement* splash_image = node->FirstChildElement("splash_image");
	m_image_name = get_xml_node_attribute_str(splash_image, _T("image_name"));

	tinyxml2::XMLElement* start_page_settings = node->FirstChildElement("start_page_settings");
	m_show_start_page = get_xml_node_attribute_bool(start_page_settings, _T("show_start_page"));
	
	tinyxml2::XMLElement* skin_settings = node->FirstChildElement("skin_settings");
	m_enable_skin = get_xml_node_attribute_bool(skin_settings, _T("enable_skin"));
	
	tinyxml2::XMLElement* application_settings = node->FirstChildElement("application_settings");
	m_application_name = get_xml_node_attribute_str(application_settings, _T("application_name"));
	m_only_one_instance = get_xml_node_attribute_bool(application_settings, _T("only_one_instance"));
}

bool app_configs::is_skin_enable()
{
	return m_enable_skin;
}
