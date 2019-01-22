#include "StdAfx.h"
#include "Resource.h"

#include "main_frame.h"

#include "docking_pane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


IMPLEMENT_SERIAL(single_pane_data, CObject, 1)

single_pane_data::single_pane_data()
{
	m_id = 0;
	m_start_load = false;
	m_select = false;
	m_no_close_button = false;
	m_no_caption = false;
	m_no_dockable = false;
	m_no_hideable = false;
}

single_pane_data::single_pane_data(const single_pane_data& src)
{
	*this = src;
}

single_pane_data::~single_pane_data()
{
}

single_pane_data& single_pane_data::operator=(const single_pane_data& src)
{
	if (this == &src) return *this;

	m_id = src.m_id;
	m_pane_title = src.m_pane_title;
	m_tab_title = src.m_tab_title;
	m_start_load = src.m_start_load;
	m_select = src.m_select;
	m_no_close_button = src.m_no_close_button;
	m_no_caption = src.m_no_caption;
	m_no_dockable = src.m_no_dockable;
	m_no_hideable = src.m_no_hideable;
	m_icon_file = src.m_icon_file;
	m_child_window_class = src.m_child_window_class;
	m_child_window_id = src.m_child_window_id;
	m_app_identify = src.m_app_identify;

	return *this;
}

void single_pane_data::init_single_pane_data(tinyxml2::XMLElement* node)
{
	if (!node) return;

	m_id = 0; // 创建时会自动分配
	m_pane_title = get_xml_node_attribute_str(node, _T("pane_title"));
	m_tab_title = get_xml_node_attribute_str(node, _T("tab_title"));
	m_start_load = get_xml_node_attribute_bool(node, _T("start_load"));
	m_select = get_xml_node_attribute_bool(node, _T("selected"));
	m_no_close_button = get_xml_node_attribute_bool(node, _T("no_close_button"));
	m_no_caption = get_xml_node_attribute_bool(node, _T("no_caption"));
	m_no_dockable = get_xml_node_attribute_bool(node, _T("no_dockable"));
	m_no_hideable = get_xml_node_attribute_bool(node, _T("no_hideable"));
	m_icon_file = get_xml_node_attribute_str(node, _T("icon_file"));
	m_child_window_class = get_xml_node_attribute_str(node, _T("child_window_class"));
	m_child_window_id = 0; // 创建时会自动分配
	m_app_identify = get_xml_node_attribute_str(node, _T("app_identify"));
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(pane_container_data, CObject, 1)

pane_container_data::pane_container_data()
{
	m_width = 0;
	m_height = 0;
	m_show = false;
}

pane_container_data::pane_container_data(const pane_container_data& src)
{
	*this = src;
}

pane_container_data::~pane_container_data()
{
}

pane_container_data& pane_container_data::operator=(const pane_container_data& src)
{
	if (this == &src) return *this;

	m_direction_by_frame = src.m_direction_by_frame;
	m_app_identify = src.m_app_identify;
	m_neighbor_identify = src.m_neighbor_identify;
	m_width = src.m_width;
	m_height = src.m_height;
	m_show = src.m_show;
	m_panes = src.m_panes;

	return *this;
}

void pane_container_data::init_pane_container_data(tinyxml2::XMLElement* node)
{
	m_panes.clear();

	if (!node) return;

	tinyxml2::XMLElement* panes_node = node->FirstChildElement("pane");
	if (!panes_node) return;

	m_direction_by_frame = get_xml_node_attribute_str(node, _T("direction_by_frame"));
	m_app_identify = get_xml_node_attribute_str(node, _T("app_identify"));
	m_neighbor_identify = get_xml_node_attribute_str(node, _T("neighbor_identify"));
	m_width = get_xml_node_attribute_int(node, _T("width"));
	m_height = get_xml_node_attribute_int(node, _T("height"));
	m_show = get_xml_node_attribute_bool(node, _T("show"));

	do
	{
		single_pane_data single_pane;
		single_pane.init_single_pane_data(panes_node);
		m_panes.push_back(single_pane);

		panes_node = panes_node->NextSiblingElement("pane");
	} while (panes_node);
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(docking_pane_data, CObject, 1)

docking_pane_data::docking_pane_data()
{
	m_force_create_by_frame = false;
	m_auto_layout = true;
}

docking_pane_data::docking_pane_data(const docking_pane_data& src)
{
	*this = src;
}

docking_pane_data::~docking_pane_data()
{
}

docking_pane_data& docking_pane_data::operator=(const docking_pane_data& src)
{
	if (this == &src) return *this;

	m_container = src.m_container;

	return *this;
}

void docking_pane_data::init_docking_pane_data(tinyxml2::XMLElement* node)
{
	m_container.clear();
	if (!node) return;

	tinyxml2::XMLElement* container_node = node->FirstChildElement("container");
	if (!container_node || container_node->NoChildren()) return;

	do
	{
		pane_container_data container;
		container.init_pane_container_data(container_node);

		if (!container.m_panes.empty()) m_container.push_back(container);

		container_node = container_node->NextSiblingElement("container");
	} while (container_node && !container_node->NoChildren());
}

single_pane_data* docking_pane_data::get_pane_data_by_pane_wnd_id(int id)
{
	for (pane_container_data& container : m_container)
		for (single_pane_data& pane_data : container.m_panes)
			if (pane_data.m_id == id) return &pane_data;

	return nullptr;
}
