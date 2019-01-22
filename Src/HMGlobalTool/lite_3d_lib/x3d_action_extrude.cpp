#include "stdafx.h"

#include "../tools/compare_tools.h"

#include "x3d_action_extrude.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_action_extrude, x3d_action_base, 1)

x3d_action_extrude::x3d_action_extrude()
{
	m_section = nullptr;
	m_trans_matrix = nullptr;
	m_length = 0.0;
}


x3d_action_extrude::~x3d_action_extrude()
{
	safe_delete(m_section);
	safe_delete(m_trans_matrix);
}

x3d_action_section* get_section_by_function(const x3d_action_section& src, 	function<void(size_t, x3d_action_section::point&)> func)
{
	if (!func) return nullptr;

	x3d_action_section* result = new x3d_action_section(src);
	for (size_t i = 0; i != result->m_all_points.size(); ++i)
		func(i, result->m_all_points[i]);

	return result;
}

//
void x3d_action_extrude::get_surround_graph_by_fuc(x3d_graph& result)
{
	// 插入周围的面点集
	auto it_surround = result.m_all_units.insert(make_pair(_T("surround_surface"), x3d_graph_unit()));
	vector<x3d_vertex>& graph_unit = (*it_surround.first).second.m_vertexs;
	 
	const vector<x3d_action_section::point>& source_points = m_section->m_all_points;
	x3d_action_section* end_sec = get_section_by_function(*m_section, m_trans_fun_callback);
	vector<x3d_action_section::point>& result_points = end_sec->m_all_points;   // 得到拉伸后平面的点集

	// 生成周围的面点集
	if (m_section->m_nm_type == x3d_normal_type::plane)
	{
		(*it_surround.first).second.m_drawing_type = x3d_drawing_type::triangles;
		for (size_t i = 0; i + 1 < result_points.size(); ++i)
		{
			x3d_plane tmp_plane(source_points[i].p, result_points[i + 1].p, result_points[i].p);
            if (!tmp_plane.is_plane())
                tmp_plane.set_data(source_points[i].p, source_points[i + 1].p, result_points[i].p);

			x3d_vector nm = tmp_plane.get_normal_vector();
			if (!source_points[i].is_left) nm = -nm;

			// 为保证每个面的向量统一且相互间无影响，把每个面的四个点拆成两个独立的三角形共六个点分别存进去
			graph_unit.push_back(x3d_vertex(source_points[i].p, nm));
			graph_unit.push_back(x3d_vertex(source_points[i + 1].p, nm));
			graph_unit.push_back(x3d_vertex(result_points[i].p, nm));
			graph_unit.push_back(x3d_vertex(result_points[i].p, nm));
			graph_unit.push_back(x3d_vertex(source_points[i + 1].p, nm));
			graph_unit.push_back(x3d_vertex(result_points[i + 1].p, nm));
		}
	}

	else if (m_section->m_nm_type == x3d_normal_type::vertex)
	{
		(*it_surround.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
		for (size_t i = 0; i < result_points.size(); ++i)
		{
			x3d_plane tmp_plane;
			if (i + 1 < result_points.size())
				tmp_plane.set_data(source_points[i].p, result_points[i + 1].p, result_points[i].p);
			else
				tmp_plane.set_data(source_points[0].p, result_points[1].p, result_points[0].p);

			x3d_vector nm = tmp_plane.get_normal_vector();
			if (!source_points[i].is_left) nm = -nm;

			graph_unit.push_back(x3d_vertex(source_points[i].p, nm));
			graph_unit.push_back(x3d_vertex(result_points[i].p, nm));
		}
	}
	else
	{
		// nothing
	}
	safe_delete(end_sec);
}

void x3d_action_extrude::do_action(x3d_graph& result)
{
	if (!m_section) return;
	
	// 如果传入了通用点变换矩阵
	if (m_trans_matrix)
	{
		m_trans_fun_callback = [&](size_t n, x3d_action_section::point& pt)
		{ m_trans_matrix->transform_vertex_self(pt.p); };
	}

    // 如果传入了垂直拉伸距离的情况
	if (!m_trans_matrix && compare_tools::is_not_equal(m_length, 0))
	{
		vector<x3d_action_section::point>& tmp = m_section->m_all_points;
		if (tmp.size() < 3) return;    // 传入点集小于3则无法确定拉伸方向，直接返回

		x3d_vector dir = x3d_plane(tmp[0].p, tmp[1].p, tmp[2].p).get_normal_vector();  // 得到延伸方向的方向向量
		x3d_vector move_dir = m_reverse ? -dir * m_length : dir * m_length;

		m_trans_fun_callback = [&](size_t n, x3d_action_section::point& pt)	{ pt.p += move_dir; };
	}

	get_surround_graph_by_fuc(result);             // 
}


x3d_action_section* x3d_action_extrude::get_extrude_section()
{
	if (!m_section) return nullptr;

	// 如果传入了通用点变换矩阵
	if (m_trans_matrix)
	{
		m_trans_fun_callback = [&](size_t n, x3d_action_section::point& pt)
		{ m_trans_matrix->transform_vertex_self(pt.p); };
	}

	// 如果传入了垂直拉伸距离的情况
	if (!m_trans_matrix && compare_tools::is_not_equal(m_length, 0))
	{
		vector<x3d_action_section::point>& tmp = m_section->m_all_points;
		if (tmp.size() < 3) return nullptr;    // 传入点集小于3则无法确定拉伸方向，直接返回

		x3d_vector dir = x3d_plane(tmp[0].p, tmp[1].p, tmp[2].p).get_normal_vector();  // 得到延伸方向的方向向量
		x3d_vector move_dir = m_reverse ? -dir * m_length : dir * m_length;

		m_trans_fun_callback = [&](size_t n, x3d_action_section::point& pt)	{ pt.p += move_dir; };
	}

	return get_section_by_function(*m_section, m_trans_fun_callback);
}


void x3d_action_extrude::set_data(x3d_action_section* sec, x3d_trans_matrix* mtx)
{
	m_section = sec;
	m_trans_matrix = mtx;
}

void x3d_action_extrude::set_data(x3d_action_section* sec, double l, bool is_reverse /*= false*/)
{
	m_section = sec;
	m_length = l;
	m_reverse = is_reverse;
}

void x3d_action_extrude::set_data(x3d_action_section* sec, function<void(size_t, x3d_action_section::point&)> func)
{
	m_section = sec;
	m_trans_fun_callback = func;
}

x3d_action_section* x3d_action_extrude::get_section() const
{
	return m_section;
}

x3d_trans_matrix* x3d_action_extrude::get_matrix() const
{
	return m_trans_matrix;
}

// 根据一组平面上的点得到此平面的法向量（未完成）
x3d_vector get_plane_vector(vector<x3d_point>& src)
{
	return src[0];
}




