#include "StdAfx.h"

#include <vector>

#include "../tools/math_tools.h"

#include "frame_data.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


frame_data_node::frame_data_node()
{
}

frame_data_node::~frame_data_node()
{

}

void frame_data_node::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);

		mt.write(_T("m_ref_line"), m_ref_line);
		mt.write(_T("m_dist_to_ref_line"), m_dist_to_ref_line);
		mt.write(_T("m_begin_layer"), m_begin_layer);
		mt.write(_T("m_end_layer"), m_end_layer);
		mt.write(_T("m_transposed"), m_transposed);
		mt.write(_T("m_diameter"), m_diameter);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_ref_line"), m_ref_line);
			mt.read(_T("m_dist_to_ref_line"), m_dist_to_ref_line);
			mt.read(_T("m_begin_layer"), m_begin_layer);
			mt.read(_T("m_end_layer"), m_end_layer);
			mt.read(_T("m_transposed"), m_transposed);
			mt.read(_T("m_diameter"), m_diameter);
		}
	}
}

frame_data_hook::frame_data_hook()
{
}

frame_data_hook::~frame_data_hook()
{

}

void frame_data_hook::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);

		mt.write(_T("m_index_name"), m_index_name);
		mt.write(_T("m_hook_type"), m_hook_type);
		mt.write(_T("m_hook_length"), m_hook_length);
		mt.write(_T("m_arc_radius"), m_arc_radius);
		mt.write(_T("m_hook_end_type"), m_hook_end_type);
		mt.write(_T("m_hook_end_angle"), m_hook_end_angle);
		mt.write(_T("m_hook_direction"), m_hook_direction);
		mt.write(_T("m_binded_segment"), m_binded_segment);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_index_name"), m_index_name);
			mt.read(_T("m_hook_type"), m_hook_type);
			mt.read(_T("m_hook_length"), m_hook_length);
			mt.read(_T("m_arc_radius"), m_arc_radius);
			mt.read(_T("m_hook_end_type"), m_hook_end_type);
			mt.read(_T("m_hook_end_angle"), m_hook_end_angle);
			mt.read(_T("m_hook_direction"), m_hook_direction);
			mt.read(_T("m_binded_segment"), m_binded_segment);
		}
	}
}

CString frame_data_hook::get_general_hook_name()
{
	return _T("Õ®”√Õ‰π≥");
}

frame_data_piece::frame_data_piece()
{
	m_is_symmetric = false;
	m_need_weld = true;
	m_has_top_perforation_steel = true;
	m_has_bottom_perforation_steel = true;
}

frame_data_piece::~frame_data_piece()
{

}

void frame_data_piece::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);

		mt.write(_T("m_index_name"), m_index_name);
		mt.write(_T("m_need_weld"), m_need_weld);
		mt.write(_T("m_is_symmetric"), m_is_symmetric);
		mt.write(_T("m_steel_type"), m_steel_type);
		mt.write(_T("m_diameter"), m_diameter);
		mt.write(_T("m_hook_weld_length"), m_hook_weld_length);
		mt.write(_T("m_top_space"), m_top_space);
		mt.write(_T("m_bottom_space"), m_bottom_space);
		mt.write(_T("m_top_flag"), m_top_flag);
		mt.write(_T("m_bottom_flag"), m_bottom_flag);
		mt.write(_T("m_bent_angle"), m_bent_angle);
		mt.write(_T("m_arc_steel_radius"), m_arc_steel_radius);
		mt.write(_T("m_mid_bent_radius"), m_mid_bent_radius);
		mt.write(_T("m_mid_weld_length"), m_mid_weld_length);
		mt.write(_T("m_has_top_perforation_steel"), m_has_top_perforation_steel);
		mt.write(_T("m_has_bottom_perforation_steel"), m_has_bottom_perforation_steel);
		mt.write(_T("m_hook_settings"), m_hook_settings);
		mt.write(_T("m_top_nodes"), m_top_nodes);
		mt.write(_T("m_bottom_nodes"), m_bottom_nodes);
		mt.write(_T("m_laction"), m_laction);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_index_name"), m_index_name);
			mt.read(_T("m_need_weld"), m_need_weld);
			mt.read(_T("m_is_symmetric"), m_is_symmetric);
			mt.read(_T("m_steel_type"), m_steel_type);
			mt.read(_T("m_diameter"), m_diameter);
			mt.read(_T("m_hook_weld_length"), m_hook_weld_length);
			mt.read(_T("m_top_space"), m_top_space);
			mt.read(_T("m_bottom_space"), m_bottom_space);
			mt.read(_T("m_top_flag"), m_top_flag);
			mt.read(_T("m_bottom_flag"), m_bottom_flag);
			mt.read(_T("m_bent_angle"), m_bent_angle);
			mt.read(_T("m_arc_steel_radius"), m_arc_steel_radius);
			mt.read(_T("m_mid_bent_radius"), m_mid_bent_radius);
			mt.read(_T("m_mid_weld_length"), m_mid_weld_length);
			mt.read(_T("m_has_top_perforation_steel"), m_has_top_perforation_steel);
			mt.read(_T("m_has_bottom_perforation_steel"), m_has_bottom_perforation_steel);
			mt.read(_T("m_hook_settings"), m_hook_settings);
			mt.read(_T("m_top_nodes"), m_top_nodes);
			mt.read(_T("m_bottom_nodes"), m_bottom_nodes);
			mt.read(_T("m_laction"), m_laction);
		}
	}
}

frame_data::frame_data()
{
	m_general_hook.m_index_name = frame_data_hook::get_general_hook_name();
}

frame_data::~frame_data()
{

}

pair<int, CString> frame_data::get_ref_line_info(const CString& ref_line_full_name)
{
	bool is_end_with_number = false;

	int end_number_count = 0;
	for (int i = ref_line_full_name.GetLength() - 1; i >= 0; --i)
	{
		TCHAR cur_char = ref_line_full_name[i];
		if (!isdigit(cur_char)) break;

		++end_number_count;
		is_end_with_number = true;
	}

	if (!is_end_with_number) return make_pair(-1, _T(""));

	CString num_text = ref_line_full_name.Right(end_number_count);
	CString ref_line_type_name = ref_line_full_name.Left(ref_line_full_name.GetLength()- end_number_count);
	return make_pair(_ttoi(num_text), ref_line_type_name);
}

void frame_data::serialize_members(member_rw_interface& mt)
{
	data_node_base::serialize_members(mt);
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);

		mt.write(_T("m_all_frames"), m_all_frames);
		mt.write(_T("m_general_hook"), m_general_hook);
		mt.write(_T("m_all_hooks"), m_all_hooks);
	}
	else
	{
		mt.read(_T("version"), version);
		if (1 == version)
		{
			mt.read(_T("m_all_frames"), m_all_frames);
			mt.read(_T("m_general_hook"), m_general_hook);
			mt.read(_T("m_all_hooks"), m_all_hooks);
		}
	}
}

frame_data_piece* frame_data::get_sepcial_frame(const CString& name)
{
	for (frame_data_piece& x : m_all_frames)
		if (0 == x.m_index_name.CompareNoCase(name)) return &x;

	return nullptr;
}

bool frame_data::parse_frame_expression(IN const CString& text, OUT vector<CString>& result)
{
	return math_tools::parse_multiply_expression(text, result);
}
