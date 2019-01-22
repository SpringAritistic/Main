#include "stdafx.h"


#include "../tools/string_tools.h"
#include "tendon_data.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

tendon_data_basetype::tendon_data_basetype()
{

}

tendon_data_basetype::~tendon_data_basetype()
{

}

void tendon_data_basetype::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_name);
		mt & x_wrapper_macro(m_materail_type);
		mt & x_wrapper_macro(m_numbers);
		mt & x_wrapper_macro(m_start_anchor);
		mt & x_wrapper_macro(m_end_anchor);
		mt & x_wrapper_macro(m_corrugated_pipe);
		mt & x_wrapper_macro(m_tension_stress);
		mt & x_wrapper_macro(m_tension_concrete_strength);
		mt & x_wrapper_macro(m_tension);
		mt & x_wrapper_macro(m_linker);
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_name);
			mt & x_wrapper_macro(m_materail_type);
			mt & x_wrapper_macro(m_numbers);
			mt & x_wrapper_macro(m_start_anchor);
			mt & x_wrapper_macro(m_end_anchor);
			mt & x_wrapper_macro(m_corrugated_pipe);
			mt & x_wrapper_macro(m_tension_stress);
			mt & x_wrapper_macro(m_tension_concrete_strength);
			mt & x_wrapper_macro(m_tension);
			mt & x_wrapper_macro(m_linker);
		}
		else
		{
			//nothing
		}
	}

}

tendon_data_section::tendon_data_section()
{

}

tendon_data_section::~tendon_data_section()
{

}

void tendon_data_section::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_x_curvename); 
		mt & x_wrapper_macro(m_x); 
		mt & x_wrapper_macro(m_y_curvename); 
		mt & x_wrapper_macro(m_y); 
		mt & x_wrapper_macro(m_r); 
		mt & x_wrapper_macro(m_type); 
		mt & x_wrapper_macro(m_angle); 
		mt & x_wrapper_macro(m_line_dir); 
		mt & x_wrapper_macro(m_s); 
		mt & x_wrapper_macro(m_e); 
		mt & x_wrapper_macro(m_semodifier); 
		mt & x_wrapper_macro(m_rmodifier); 
		mt & x_wrapper_macro(m_zu); 
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_x_curvename);
			mt & x_wrapper_macro(m_x);
			mt & x_wrapper_macro(m_y_curvename);
			mt & x_wrapper_macro(m_y);
			mt & x_wrapper_macro(m_r);
			mt & x_wrapper_macro(m_type);
			mt & x_wrapper_macro(m_angle);
			mt & x_wrapper_macro(m_line_dir);
			mt & x_wrapper_macro(m_s);
			mt & x_wrapper_macro(m_e);
			mt & x_wrapper_macro(m_semodifier);
			mt & x_wrapper_macro(m_rmodifier);
			mt & x_wrapper_macro(m_zu);
		}
		else
		{
			//nothing
		}
	}
}

tendon_data_bend_type::tendon_data_bend_type()
{

}

tendon_data_bend_type::~tendon_data_bend_type()
{

}

void tendon_data_bend_type::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_name); 
		mt & x_wrapper_macro(m_porlines); 
		mt & x_wrapper_macro(m_sections); 
		mt & x_wrapper_macro(m_joint_list);
		mt & x_wrapper_macro(m_strObjectID);
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_name);
			mt & x_wrapper_macro(m_porlines);
			mt & x_wrapper_macro(m_sections);
			mt & x_wrapper_macro(m_joint_list);
			mt & x_wrapper_macro(m_strObjectID);
		}
		else
		{
			//nothing
		}
	}
}

tendon_data_userline::tendon_data_userline()
{

}

tendon_data_userline::~tendon_data_userline()
{

}

void tendon_data_userline::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_x); 
		mt & x_wrapper_macro(m_y); 
		mt & x_wrapper_macro(m_r); 
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_x);
			mt & x_wrapper_macro(m_y);
			mt & x_wrapper_macro(m_r);
		}
		else
		{
			//nothing
		}
	}
}

void tendon_data_userline::string_to_vecUserline(const CString& text, std::vector<tendon_data_userline>& result)
{
	result.clear();
	std::vector<CString> data_divide_by_semicolon;
	string_tools::split_string(data_divide_by_semicolon, text, _T(";"));

	for (size_t i = 0; i != data_divide_by_semicolon.size(); ++i)
	{
		vector<CString> vecstirng;
		string_tools::split_string(vecstirng, data_divide_by_semicolon[i], _T(","));
		if (vecstirng.empty()) return;
		tendon_data_userline tmp_curve;
		tmp_curve.m_x = _ttof(vecstirng[0]);
		tmp_curve.m_y = _ttof(vecstirng[1]);
		tmp_curve.m_r = _ttof(vecstirng[2]);

		result.push_back(tmp_curve);
	}

}

void tendon_data_userline::vecUserline_to_string(CString& text, const std::vector<tendon_data_userline>& vecUserline)
{
	text = _T("");
	for (size_t i = 0; i != vecUserline.size(); ++i)
	{
		CString m_x = string_tools::num_to_string(vecUserline[i].m_x);
		CString m_y = string_tools::num_to_string(vecUserline[i].m_y);
		CString m_r = string_tools::num_to_string(vecUserline[i].m_r);

		text += m_x + _T(",") + m_y + _T(",") + m_r + _T(",") + _T(";");
	}
}

tendon_data_type::tendon_data_type()
{

}

tendon_data_type::~tendon_data_type()
{

}

void tendon_data_type::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_name);
		mt & x_wrapper_macro(m_base_type);
		mt & x_wrapper_macro(m_vertical_bend_type);
		mt & x_wrapper_macro(m_horizon_bend_type);
		mt & x_wrapper_macro(m_tendon_property);
		mt & x_wrapper_macro(m_drawing_name);
		mt & x_wrapper_macro(m_sub_type);
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_name);
			mt & x_wrapper_macro(m_base_type);
			mt & x_wrapper_macro(m_vertical_bend_type);
			mt & x_wrapper_macro(m_horizon_bend_type);
			mt & x_wrapper_macro(m_tendon_property);
			mt & x_wrapper_macro(m_drawing_name);
			mt & x_wrapper_macro(m_sub_type);
		}
		else
		{
			//nothing
		}
	}
}

tendon_data_anchor_plane::tendon_data_anchor_plane()
{

}

tendon_data_anchor_plane::~tendon_data_anchor_plane()
{

}

void tendon_data_anchor_plane::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_anchor_name);
		mt & x_wrapper_macro(m_anchor_tendon);
		mt & x_wrapper_macro(m_gear_anchor_name);
		mt & x_wrapper_macro(m_reference_surface);
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_anchor_name);
			mt & x_wrapper_macro(m_anchor_tendon);
			mt & x_wrapper_macro(m_gear_anchor_name);
			mt & x_wrapper_macro(m_reference_surface);			
		}
		else
		{
			//nothing
		}
	}
}

tendon_data_tension_batch::tendon_data_tension_batch()
{

}

tendon_data_tension_batch::~tendon_data_tension_batch()
{

}

void tendon_data_tension_batch::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_batch_name);
		mt & x_wrapper_macro(m_tension_tendon);
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_batch_name);
			mt & x_wrapper_macro(m_tension_tendon);
		}
		else
		{
			//nothing
		}
	}
}

tendon_data_refline::tendon_data_refline()
{

}

tendon_data_refline::~tendon_data_refline()
{

}

void tendon_data_refline::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_name);
		mt & x_wrapper_macro_enum(m_type);
		mt & x_wrapper_macro(m_refline);
		mt & x_wrapper_macro(m_dist);
		mt & x_wrapper_macro(m_userline);
		mt & x_wrapper_macro(m_note);
		mt & x_wrapper_macro(m_guid);
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_name);
			mt & x_wrapper_macro_enum(m_type);
			mt & x_wrapper_macro(m_refline);
			mt & x_wrapper_macro(m_dist);
			mt & x_wrapper_macro(m_userline);
			mt & x_wrapper_macro(m_note);
			mt & x_wrapper_macro(m_guid);
		}
		else
		{
			//nothing
		}
	}
}

tendon_data_layout::tendon_data_layout()
{

}

tendon_data_layout::~tendon_data_layout()
{

}

void tendon_data_layout::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_tendon_name); 
		mt & x_wrapper_macro(m_refline_name); 
		mt & x_wrapper_macro(m_portrait_layout); 
		mt & x_wrapper_macro(m_cross_layout); 
		mt & x_wrapper_macro(m_guid);
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_tendon_name);
			mt & x_wrapper_macro(m_refline_name);
			mt & x_wrapper_macro(m_portrait_layout);
			mt & x_wrapper_macro(m_cross_layout);
			mt & x_wrapper_macro(m_guid);
		}
		else
		{
			//nothing
		}
	}
}

tendon_data_info::tendon_data_info()
{

}

tendon_data_info::~tendon_data_info()
{

}

void tendon_data_info::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_application_pos); 
		mt & x_wrapper_macro(m_all_layout); 
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_application_pos);
			mt & x_wrapper_macro(m_all_layout);
		}
		else
		{
			//nothing
		}
	}
}


IMPLEMENT_SERIAL(tendon_data, data_node_base, 1)
tendon_data::tendon_data()
{

}

tendon_data::~tendon_data()
{

}
void tendon_data::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt & x_wrapper_macro(version);

		mt & x_wrapper_macro(m_all_basetype);
		mt & x_wrapper_macro(m_all_tendon_type);
		mt & x_wrapper_macro(m_all_anchor_plane);
		mt & x_wrapper_macro(m_all_tension_batch);
		mt & x_wrapper_macro(m_all_refline);
		mt & x_wrapper_macro(m_all_ver_bend_type);
		mt & x_wrapper_macro(m_all_hor_bend_type);
		mt & x_wrapper_macro(m_all_tendon_info);
		//mt & x_wrapper_macro(m_pSetting);
	}
	else
	{
		  mt & x_wrapper_macro(version);
		if (1 == version)
		{
			mt & x_wrapper_macro(m_all_basetype);
			mt & x_wrapper_macro(m_all_tendon_type);
			mt & x_wrapper_macro(m_all_anchor_plane);
			mt & x_wrapper_macro(m_all_tension_batch);
			mt & x_wrapper_macro(m_all_refline);
			mt & x_wrapper_macro(m_all_ver_bend_type);
			mt & x_wrapper_macro(m_all_hor_bend_type);
			mt & x_wrapper_macro(m_all_tendon_info);
			//mt & x_wrapper_macro(m_pSetting);
		}
		else
		{
			//nothing
		}
	}
}

//tendon_data tendon_data::tendon_data(const tendon_data& Other)
//{
//
//}
//
//
//
//tendon_data& tendon_data::operator=(const tendon_data& Other)
//{
//
//}

void tendon_data::clearall()
{
	m_all_basetype.clear();
	m_all_tendon_type.clear();
	m_all_anchor_plane.clear();
	m_all_tension_batch.clear();
	m_all_refline.clear();
	m_all_ver_bend_type.clear();
	m_all_hor_bend_type.clear();
	m_all_tendon_info.clear();
}

tendon_data tendon_data::get_tendon_data_by_name(const CString& name) const
{
	tendon_data result = *this;
	result.m_all_tendon_info.clear();
	for (size_t i = 0; i != m_all_tendon_info.size(); ++i)
	{
		const tendon_data_info& tendon = m_all_tendon_info[i];
		if (0 == tendon.m_application_pos.Compare(name))
		{
			result.m_all_tendon_info.push_back(tendon);
		}
	}
	return result;
}

