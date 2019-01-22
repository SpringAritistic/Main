#include "stdafx.h"

#include "../tools/compare_tools.h"
#include "x3d_hemisphere.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_hemisphere, x3d_geometry_base, 1)

x3d_hemisphere::x3d_hemisphere()
{
	m_radius = 0;
	m_if_has_bottom = true;
}


x3d_hemisphere::x3d_hemisphere(double r)
{
	set_data(r);
}

x3d_hemisphere::x3d_hemisphere(const x3d_point& p, double r)
{
	set_data(p, r);
}

x3d_hemisphere::~x3d_hemisphere()
{
}

void x3d_hemisphere::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_hemisphere* src_obj = dynamic_cast<const x3d_hemisphere*>(&node);
	if (!src_obj) return;

	m_radius = src_obj->m_radius;

}

void x3d_hemisphere::set_data(double r)
{
	m_radius = r;
}

void x3d_hemisphere::set_data(const x3d_point& p, double r)
{
	m_base_point = p;
	m_radius = r;
}

void x3d_hemisphere::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_radius, 0.0)) return;

	// 计算赤道上的旋转角度步进量
	double angle_step = 2.0 * M_PI / calc_real_span_count(2.0 * M_PI * m_radius);
	vector<double> round_angle_value;
	for (double ai = 0.0; compare_tools::is_less(ai, 2.0 * M_PI); ai += angle_step)
		round_angle_value.push_back(ai);

	// 计算每根经线上的角度旋转步进量（由于赤道上旋转了一周，因此经线上只要旋转半圈
	vector<double> half_round_angle_value;
	for (double ai = 0; compare_tools::is_less(ai, M_PI_2); ai += angle_step)
		half_round_angle_value.push_back(ai);

	half_round_angle_value.push_back(M_PI_2); // 必须保证两个极点的角度都放在数组中

	// 计算所有顶点坐标及法线
	vector<vector<x3d_vertex>> all_points;
	for (double x : round_angle_value)
	{
		vector<x3d_vertex> single_round;
		for (double y : half_round_angle_value)
		{
			x3d_point pt(m_radius * sin(y) * cos(x), m_radius * sin(y) * sin(x), m_radius * cos(y));
			single_round.push_back(x3d_vertex(pt, pt.get_unit_vector())); // 法线必须是单位向量
		}

		all_points.push_back(single_round);
	}

	// 生成模型的三角面
	auto it = graph.m_all_units.insert(make_pair(_T("surround_face"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit = (*it.first).second.m_vertexs;

	for (size_t i = 0; i < all_points.size(); ++i)
	{
		size_t cur_index = i;
		size_t next_index = (i + 1 == all_points.size() ? 0 : i + 1); // 要把最后一跟经纬与0度的经纬连接起来

		graph_unit.push_back(all_points[next_index][0]);
		graph_unit.push_back(all_points[cur_index][0]);
		for (size_t j = 1; j < all_points[cur_index].size(); ++j)
		{
			graph_unit.push_back(all_points[cur_index][j]);
			graph_unit.push_back(all_points[next_index][j]);
		}
	}

	if (!m_if_has_bottom) return;

	// 半球底面
	auto it_bottom = graph.m_all_units.insert(make_pair(_T("bottom_surface"), x3d_graph_unit()));
	(*it_bottom.first).second.m_drawing_type = x3d_drawing_type::triangles_fan;
	vector<x3d_vertex>& graph_unit_bottom = (*it_bottom.first).second.m_vertexs;

	x3d_vector bottom_nm(0, 0, -1);
	x3d_point bottom_center(0, 0, 0);
	graph_unit_bottom.push_back(x3d_vertex(bottom_center, bottom_nm));
	vector<x3d_point> bottom_points;
	round_angle_value.push_back(2.0 * M_PI);
	for (double x : round_angle_value)
		bottom_points.push_back(x3d_point(m_base_point.m_x + m_radius * cos(x),
		m_base_point.m_y + m_radius * sin(x), m_base_point.m_z));

	for (x3d_point& x : bottom_points)
		graph_unit_bottom.push_back(x3d_vertex(x, bottom_nm));
}
