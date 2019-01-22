#include "StdAfx.h"

#include "steel_data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


steel_design_item::steel_design_item()
{
	m_using_string_value = false;
}

steel_design_item::steel_design_item( const CString& id, const CString& value )
{
	m_id = id;
	m_using_string_value = false;
	m_value = value;
}

steel_design_item::~steel_design_item()
{

}

void steel_design_item::set( const CString& id, const CString& value )
{
	m_id = id;
	m_using_string_value = false;
	m_value = value;
}

void steel_design_item::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_id"), m_id);
		mt.write(_T("m_using_string_value"), m_using_string_value);
		mt.write(_T("m_value"), m_value);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_id"), m_id);
			mt.read(_T("m_using_string_value"), m_using_string_value);
			mt.read(_T("m_value"), m_value);
		}
	}
}

steel_basic_item::steel_basic_item()
{

}

steel_basic_item::steel_basic_item( const CString& id,  const CString& diameter,
	const CString& steel_type, const CString& hook_type, const CString& distance, const CString& force_type )
{
	m_id = id;
	m_diameter = diameter;
	m_steel_type = steel_type;
	m_hook_type = hook_type;
	m_distance = distance;
	m_force_type = force_type;
}

steel_basic_item::~steel_basic_item()
{

}

void steel_basic_item::set( const CString& id, const CString& diameter, const CString& steel_type,
	const CString& hook_type, const CString& distance, const CString& force_type )
{
	m_id = id;
	m_diameter = diameter;
	m_steel_type = steel_type;
	m_hook_type = hook_type;
	m_distance = distance;
	m_force_type = force_type;

}

void steel_basic_item::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_id"), m_id);
		mt.write(_T("m_diameter"), m_diameter);
		mt.write(_T("m_steel_type"), m_steel_type);
		mt.write(_T("m_hook_type"), m_hook_type);
		mt.write(_T("m_distance"), m_distance);
		mt.write(_T("m_force_type"), m_force_type);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_id"), m_id);
			mt.read(_T("m_diameter"), m_diameter);
			mt.read(_T("m_steel_type"), m_steel_type);
			mt.read(_T("m_hook_type"), m_hook_type);
			mt.read(_T("m_distance"), m_distance);
			mt.read(_T("m_force_type"), m_force_type);
		}
	}
}

steel_layout_item::steel_layout_item()
{

}

steel_layout_item::steel_layout_item(const CString& id, const CString& sub_id,
	const CString& space_distance, const CString& space_count )
{
	m_id = id;
	m_sub_id = sub_id;
	m_space_distance = space_distance;
	m_space_count = space_count;
}

steel_layout_item::~steel_layout_item()
{

}

void steel_layout_item::set(const CString& id, const CString& sub_id,
	const CString& space_distance, const CString& space_count )
{
	m_id = id;
	m_sub_id = sub_id;
	m_space_distance = space_distance;
	m_space_count = space_count;

}

void steel_layout_item::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_id"), m_id);
		mt.write(_T("m_sub_id"), m_sub_id);
		mt.write(_T("m_space_distance"), m_space_distance);
		mt.write(_T("m_space_count"), m_space_count);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_id"), m_id);
			mt.read(_T("m_sub_id"), m_sub_id);
			mt.read(_T("m_space_distance"), m_space_distance);
			mt.read(_T("m_space_count"), m_space_count);
		}
	}
}

steel_edge_item::steel_edge_item()
{

}

steel_edge_item::steel_edge_item(const CString& id, const CString& sub_id, 
	const CString& location, const CString& logic_type, const CString& edge_distance,
	const CString& round_level, const CString& round_type, const CString& expecting_edge )
{
	m_id = id;
	m_sub_id = sub_id;
	m_location = location;
	m_logic_type = logic_type;
	m_edge_distance = edge_distance;
	m_round_level = round_level;
	m_round_type = round_type;
	m_edge_expect = expecting_edge;
}

steel_edge_item::~steel_edge_item()
{

}

void steel_edge_item::set(const CString& id, const CString& sub_id, const CString& location,
	const CString& logic_type, const CString& edge_distance, const CString& round_level,
	const CString& round_type, const CString& expecting_edge )
{
	m_id = id;
	m_sub_id = sub_id;
	m_location = location;
	m_logic_type = logic_type;
	m_edge_distance = edge_distance;
	m_round_level = round_level;
	m_round_type = round_type;
	m_edge_expect = expecting_edge;
}

void steel_edge_item::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_id"), m_id);
		mt.write(_T("m_sub_id"), m_sub_id);
		mt.write(_T("m_location"), m_location);
		mt.write(_T("m_logic_type"), m_logic_type);
		mt.write(_T("m_edge_distance"), m_edge_distance);
		mt.write(_T("m_round_level"), m_round_level);
		mt.write(_T("m_round_type"), m_round_type);
		mt.write(_T("m_edge_expect"), m_edge_expect);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_id"), m_id);
			mt.read(_T("m_sub_id"), m_sub_id);
			mt.read(_T("m_location"), m_location);
			mt.read(_T("m_logic_type"), m_logic_type);
			mt.read(_T("m_edge_distance"), m_edge_distance);
			mt.read(_T("m_round_level"), m_round_level);
			mt.read(_T("m_round_type"), m_round_type);
			mt.read(_T("m_edge_expect"), m_edge_expect);
		}
	}
}

steel_detail_item::steel_detail_item()
{
	m_using_string_value = false;
}

steel_detail_item::steel_detail_item( const CString& id, const CString& sub_id, const CString& value )
{
	m_id = id;
	m_sub_id = sub_id;
	m_using_string_value = false;
	m_value = value;
}

steel_detail_item::~steel_detail_item()
{

}

void steel_detail_item::set( const CString& id, const CString& sub_id, const CString& value )
{
	m_id = id;
	m_sub_id = sub_id;
	m_using_string_value = false;
	m_value = value;
}

void steel_detail_item::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_id"), m_id);
		mt.write(_T("m_sub_id"), m_sub_id);
		mt.write(_T("m_using_string_value"), m_using_string_value);
		mt.write(_T("m_value"), m_value);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_id"), m_id);
			mt.read(_T("m_sub_id"), m_sub_id);
			mt.read(_T("m_using_string_value"), m_using_string_value);
			mt.read(_T("m_value"), m_value);
		}
	}
}

single_steel_data::single_steel_data()
{
	m_component = _T("");
	m_design_items.clear();
	m_basic_items.clear();
	m_layout_items.clear();
	m_edge_items.clear();
	m_detail_items.clear();
}

single_steel_data::~single_steel_data()
{

}

void single_steel_data::clear_all()
{
	m_component = _T("");
	m_design_items.clear();
	m_basic_items.clear();
	m_layout_items.clear();
	m_edge_items.clear();
	m_detail_items.clear();

}

void set_steel_design_item( steel_design_item& src, const CString& id, const CString& strValue )
{
	src.m_id = id;
	src.m_value = strValue;
}

void set_steel_basic_item( steel_basic_item& src, const CString& sub_id, const CString& value_01,
	const CString& value_02, const CString& value_03, const CString& value_04, const CString& value_05)
{
	src.m_id = sub_id;
	src.m_diameter = value_01;
	src.m_steel_type = value_02;
	src.m_hook_type = value_03;
	src.m_distance = value_04;
	src.m_force_type = value_05;
}

void set_steel_layout_item( steel_layout_item& src, const CString& id,
	const CString& sub_id, const CString& value_01, const CString& value_02)
{
	src.m_id = id;
	src.m_sub_id = sub_id;
	src.m_space_distance = value_01;
	src.m_space_count = value_02;
}

void set_steel_edge_item(steel_edge_item& src, const CString& id, const CString& sub_id,
	const CString& value_01, const CString& value_02, const CString& value_03,
	const CString& value_04, const CString& value_05, const CString& value_06)
{
	src.m_id = id;
	src.m_sub_id = sub_id;
	src.m_location = value_01;
	src.m_logic_type = value_02;
	src.m_edge_distance = value_03;
	src.m_round_level = value_04;
	src.m_round_type = value_05;
	src.m_edge_expect = value_06;
}

void set_steel_detail_item(steel_detail_item& src, const CString& id,
	const CString& sub_id, const CString& value_01)
{
	src.m_id = id;
	src.m_sub_id = sub_id;
	src.m_value = value_01;
}

void single_steel_data::add_steel_item(sub_steel_type type, const CString& id,
	const CString& sub_id, const CString& value_01, const CString& value_02, const CString& value_03,
	const CString& value_04, const CString& value_05, const CString& value_06)
{
	if (ST_DESIGN == type)
	{
		steel_design_item design;
		set_steel_design_item(design, id, value_01);
		m_design_items.push_back(design);
	}
	else if (ST_BASIC == type)
	{
		steel_basic_item basic_item;
		set_steel_basic_item(basic_item, id,
			value_01,  value_02, value_03, value_04, value_05);
		m_basic_items.push_back(basic_item);
	}
	else if (ST_LAYOUT == type)
	{
		steel_layout_item layout;
		set_steel_layout_item(layout, id, sub_id, value_01,  value_02);
		m_layout_items.push_back(layout);
	}
	else if (ST_EDEG == type)
	{
		steel_edge_item edge;
		set_steel_edge_item(edge, id, sub_id,
			value_01,  value_02, value_03, value_04, value_05, value_06);
		m_edge_items.push_back(edge);
	}
	else if (ST_DETAIL == type)
	{
		steel_detail_item detail;
		set_steel_detail_item(detail, id, sub_id, value_01);
		m_detail_items.push_back(detail);
	}
	else
	{
		_ASSERTE((_T("add_steel_item( enum type,... ) 类型不存在"), false));
	}
}

bool single_steel_data::get_design_item( const CString& id, steel_design_item& design ) const
{
	for (size_t i = 0; i != m_design_items.size(); ++i)
	{
		if (m_design_items[i].m_id == id)
		{
			design = m_design_items[i];
			return true;
		}
	}
	return false;
}

bool single_steel_data::get_basic_item( const CString& id, steel_basic_item& basic_item ) const
{
	for (size_t i = 0; i != m_basic_items.size(); ++i)
	{
		if (m_basic_items[i].m_id == id)
		{
			basic_item = m_basic_items[i];
			return true;
		}
	}
	return false;

}

bool single_steel_data::get_layout_item( const CString& id, const CString& sub_id, steel_layout_item& layout ) const
{
	for (size_t i = 0; i != m_layout_items.size(); ++i)
	{
		if (m_layout_items[i].m_id == id && m_layout_items[i].m_sub_id == sub_id)
		{
			layout = m_layout_items[i];
			return true;
		}
	}
	return false;

}

bool single_steel_data::get_edge_item( const CString& id, const CString& sub_id, steel_edge_item& edge ) const
{
	for (size_t i = 0; i != m_edge_items.size(); ++i)
	{
		if (m_edge_items[i].m_id == id && m_edge_items[i].m_sub_id == sub_id)
		{
			edge = m_edge_items[i];
			return true;
		}
	}
	return false;

}

bool single_steel_data::get_detail_item( const CString& id, const CString& sub_id, steel_detail_item& detail ) const
{
	for (size_t i = 0; i != m_detail_items.size(); ++i)
	{
		if (m_detail_items[i].m_id == id && m_detail_items[i].m_sub_id == sub_id)
		{
			detail = m_detail_items[i];
			return true;
		}
	}
	return false;

}

void single_steel_data::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_component"), m_component);
		mt.write(_T("m_design_items"), m_design_items);
		mt.write(_T("m_basic_items"), m_basic_items);
		mt.write(_T("m_layout_items"), m_layout_items);
		mt.write(_T("m_edge_items"), m_edge_items);
		mt.write(_T("m_detail_items"), m_detail_items);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_component"), m_component);
			mt.read(_T("m_design_items"), m_design_items);
			mt.read(_T("m_basic_items"), m_basic_items);
			mt.read(_T("m_layout_items"), m_layout_items);
			mt.read(_T("m_edge_items"), m_edge_items);
			mt.read(_T("m_detail_items"), m_detail_items);
		}
	}
}

void single_steel_data::set_design_item( IN const steel_design_item& design )
{
	for (size_t i = 0; i != m_design_items.size(); ++i)
	{
		if (m_design_items[i].m_id == design.m_id)
		{
			m_design_items[i] = design;
			return;
		}
	}
	add_steel_item(ST_DESIGN, design.m_id, _T(""), design.m_value);
}

void single_steel_data::set_basic_item( IN const steel_basic_item& basic_item )
{
	for (size_t i = 0; i != m_basic_items.size(); ++i)
	{
		if (m_basic_items[i].m_id == basic_item.m_id)
		{
			m_basic_items[i] = basic_item;
			return;
		}
	}

	add_steel_item(ST_BASIC,basic_item.m_id, _T(""),
		basic_item.m_diameter, basic_item.m_steel_type, basic_item.m_hook_type, basic_item.m_distance, basic_item.m_force_type);
}

void single_steel_data::set_layout_item( IN const steel_layout_item& layout )
{
	for (size_t i = 0; i != m_layout_items.size(); ++i)
	{
		if (m_layout_items[i].m_id == layout.m_id&& m_layout_items[i].m_sub_id == layout.m_sub_id)
		{
			m_layout_items[i] = layout;
			return;
		}
	}
	add_steel_item(ST_LAYOUT,	layout.m_id, layout.m_sub_id,	layout.m_space_distance, layout.m_space_count);
}

void single_steel_data::set_edge_item( IN const steel_edge_item& edge )
{
	for (size_t i = 0; i != m_edge_items.size(); ++i)
	{
		if (m_edge_items[i].m_id == edge.m_id	&& m_edge_items[i].m_sub_id == edge.m_sub_id)
		{
			m_edge_items[i] = edge;
			return;
		}
	}
	add_steel_item(ST_EDEG,edge.m_id, edge.m_sub_id, edge.m_location,	
		edge.m_logic_type, edge.m_edge_distance,	edge.m_round_level, edge.m_round_type, edge.m_edge_expect);
}

void single_steel_data::set_detail_item( IN const steel_detail_item& detail )
{
	for (size_t i = 0; i != m_detail_items.size(); ++i)
	{
		if (m_detail_items[i].m_id == detail.m_id	&& m_detail_items[i].m_sub_id ==detail.m_sub_id)
		{
			m_detail_items[i] = detail;
			return;
		}
	}
	add_steel_item(ST_DETAIL,	detail.m_id, detail.m_sub_id, detail.m_value);
}

void single_steel_data::set_component( IN const CString& component )
{
	m_component = component;
}

CString single_steel_data::get_component() const
{
	return m_component;
}

steel_data::steel_data()
{
	m_all_steel_grids.clear();
}

steel_data::~steel_data()
{

}

void steel_data::set_all_design_item( IN const steel_design_item& design )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].set_design_item(design);
	}
}

void steel_data::set_all_design_item( const CString& id, const CString& strValue )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_design_item item;
		if (false == m_all_steel_grids[i].get_design_item(id, item)) continue;

		item.m_value = strValue;
		m_all_steel_grids[i].set_design_item(item);
	}
}

void steel_data::set_all_basic_item( IN const steel_basic_item& basic_item )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].set_basic_item(basic_item);
	}
}

void steel_data::set_all_basic_item( const CString& id, const CString& diameter,
	const CString& steel_type, const CString& hook_type, const CString& distance, const CString& force_type )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_basic_item item;
		if (false == m_all_steel_grids[i].get_basic_item(id, item)) continue;

		item.m_diameter = diameter;
		item.m_steel_type = steel_type;
		item.m_hook_type = hook_type;
		item.m_distance = distance;
		item.m_force_type = force_type;

		m_all_steel_grids[i].set_basic_item(item);
	}
}

void steel_data::set_all_layout_item( IN const steel_layout_item& layout )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].set_layout_item(layout);
	}
}

void steel_data::set_all_layout_item( const CString& id, const CString& sub_id,
	const CString& space_distance, const CString& space_count )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_layout_item item;
		if (false == m_all_steel_grids[i].get_layout_item(id, sub_id, item)) continue;

		item.m_space_distance = space_distance;
		item.m_space_count = space_count;

		m_all_steel_grids[i].set_layout_item(item);
	}
}

void steel_data::set_all_edge_item( IN const steel_edge_item& edge )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].set_edge_item(edge);
	}
}

void steel_data::set_all_edge_item( const CString& id, const CString& sub_id,
	const CString& location, const CString& logic_type, const CString& edge_distance,
	const CString& round_level, const CString& round_type, const CString& expecting_edge )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_edge_item item;
		if (false == m_all_steel_grids[i].get_edge_item(id, sub_id, item)) continue;

		item.m_location = location;
		item.m_logic_type = logic_type;
		item.m_edge_distance = edge_distance;
		item.m_round_level = round_level;
		item.m_round_type = round_type;
		item.m_edge_expect = expecting_edge;

		m_all_steel_grids[i].set_edge_item(item);
	}
}

void steel_data::set_all_detail_item( IN const steel_detail_item& detail )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].set_detail_item(detail);
	}
}

void steel_data::set_all_detail_item( const CString& id, const CString& sub_id, const CString& value )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_detail_item item;
		if (false == m_all_steel_grids[i].get_detail_item(id, sub_id, item)) continue;

		item.m_value = value;

		m_all_steel_grids[i].set_detail_item(item);
	}
}

void steel_data::set_all_component( IN const CString& strComponent )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].set_component(strComponent);
	}
}

void steel_data::add_all_design_item( IN const steel_design_item& design )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].add_steel_item(ST_DESIGN, design.m_id, _T(""), design.m_value);
	}
}

void steel_data::add_all_base_item( IN const steel_basic_item& basic_item )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].add_steel_item(ST_BASIC,
			basic_item.m_id, _T(""), basic_item.m_diameter, basic_item.m_steel_type,
			basic_item.m_hook_type, basic_item.m_distance, basic_item.m_force_type);
	}
}

void steel_data::add_all_layout_item( IN const steel_layout_item& layout )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].add_steel_item(ST_LAYOUT,
			layout.m_id, layout.m_sub_id,
			layout.m_space_distance, layout.m_space_count);
	}
}

void steel_data::add_all_edge_item( IN const steel_edge_item& edge )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].add_steel_item(ST_EDEG,
			edge.m_id, edge.m_sub_id, edge.m_location,
			edge.m_logic_type, edge.m_edge_distance,
			edge.m_round_level, edge.m_round_type, edge.m_edge_expect);
	}
}

void steel_data::add_all_detail_item( IN const steel_detail_item& detail )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		m_all_steel_grids[i].add_steel_item(ST_DETAIL, detail.m_id, detail.m_sub_id, detail.m_value);
	}
}

void steel_data::serialize_members(member_rw_interface& mt)
{
	data_node_base::serialize_members(mt);
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_all_steel_grids"), m_all_steel_grids);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_all_steel_grids"), m_all_steel_grids);
		}
	}
}

void steel_data::set_all_basic_item_diameter( const CString& id, const CString& diameter )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_basic_item item;
		if (false == m_all_steel_grids[i].get_basic_item(id, item)) continue;

		item.m_diameter = diameter;
		m_all_steel_grids[i].set_basic_item(item);
	}
}

void steel_data::set_all_basic_item_steel_type( const CString& id, const CString& steel_type )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_basic_item item;
		if (false == m_all_steel_grids[i].get_basic_item(id, item)) continue;

		item.m_steel_type = steel_type;
		m_all_steel_grids[i].set_basic_item(item);
	}
}

void steel_data::set_all_basic_item_hook_type( const CString& id, const CString& hook_type )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_basic_item item;
		if (false == m_all_steel_grids[i].get_basic_item(id, item)) continue;

		item.m_hook_type = hook_type;
		m_all_steel_grids[i].set_basic_item(item);
	}
}

void steel_data::set_all_basic_item_distance( const CString& id, const CString& distance )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_basic_item item;
		if (false == m_all_steel_grids[i].get_basic_item(id, item)) continue;

		item.m_distance = distance;
		m_all_steel_grids[i].set_basic_item(item);
	}
}

void steel_data::set_all_basic_item_force_type( const CString& id, const CString& force_type )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_basic_item item;
		if (false == m_all_steel_grids[i].get_basic_item(id, item)) continue;

		item.m_force_type = force_type;
		m_all_steel_grids[i].set_basic_item(item);
	}
}

void steel_data::set_all_layout_item_distance( const CString& id, const CString& sub_id, const CString& space_distance )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_layout_item item;
		if (false == m_all_steel_grids[i].get_layout_item(id, sub_id, item)) continue;

		item.m_space_distance = space_distance;

		m_all_steel_grids[i].set_layout_item(item);
	}
}

void steel_data::set_all_layout_item_count( const CString& id, const CString& sub_id, const CString& space_count )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_layout_item item;
		if (false == m_all_steel_grids[i].get_layout_item(id, sub_id, item)) continue;

		item.m_space_count = space_count;

		m_all_steel_grids[i].set_layout_item(item);
	}
}

void steel_data::set_all_edge_item_location( const CString& id, const CString& sub_id, const CString& location )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_edge_item item;
		if (false == m_all_steel_grids[i].get_edge_item(id, sub_id, item)) continue;

		item.m_location = location;

		m_all_steel_grids[i].set_edge_item(item);
	}
}

void steel_data::set_all_edge_item_logic_type( const CString& id, const CString& sub_id, const CString& logic_type )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_edge_item item;
		if (false == m_all_steel_grids[i].get_edge_item(id, sub_id, item)) continue;

		item.m_logic_type = logic_type;

		m_all_steel_grids[i].set_edge_item(item);
	}
}

void steel_data::set_all_edge_item_distance( const CString& id, const CString& sub_id, const CString& edge_distance )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_edge_item item;
		if (false == m_all_steel_grids[i].get_edge_item(id, sub_id, item)) continue;

		item.m_edge_distance = edge_distance;

		m_all_steel_grids[i].set_edge_item(item);
	}
}

void steel_data::set_all_edge_item_round_level( const CString& id, const CString& sub_id, const CString& round_level )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_edge_item item;
		if (false == m_all_steel_grids[i].get_edge_item(id, sub_id, item)) continue;

		item.m_round_level = round_level;

		m_all_steel_grids[i].set_edge_item(item);
	}
}

void steel_data::set_all_edge_item_round_type( const CString& id, const CString& sub_id, const CString& round_type )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_edge_item item;
		if (false == m_all_steel_grids[i].get_edge_item(id, sub_id, item)) continue;

		item.m_round_type = round_type;

		m_all_steel_grids[i].set_edge_item(item);
	}
}

void steel_data::set_all_edge_item_edge_expect( const CString& id, const CString& sub_id, const CString& expecting_edge )
{
	for (size_t i = 0; i != m_all_steel_grids.size(); ++i)
	{
		steel_edge_item item;
		if (false == m_all_steel_grids[i].get_edge_item(id, sub_id, item)) continue;

		item.m_edge_expect = expecting_edge;

		m_all_steel_grids[i].set_edge_item(item);
	}
}

single_steel_data& steel_data::get_steel_data(size_t index)
{
	return m_all_steel_grids[index];
}


const single_steel_data& steel_data::get_steel_data(size_t index) const
{
	return m_all_steel_grids[index];
}

bool steel_data::has_steel_data() const
{
	return !m_all_steel_grids.empty();
}

size_t steel_data::get_steel_count() const
{
	return m_all_steel_grids.size();
}

void steel_data::add_single_steel(const single_steel_data& steel)
{
	m_all_steel_grids.push_back(steel);
}

void steel_data::clear_steel_data()
{
	m_all_steel_grids.clear();
}

void steel_data::delete_single_steel(size_t index)
{
	if (index >= m_all_steel_grids.size()) return;

	m_all_steel_grids.erase(m_all_steel_grids.begin() + index);
}

void steel_data::resize_steel_count(size_t index)
{
	m_all_steel_grids.resize(index);
}

