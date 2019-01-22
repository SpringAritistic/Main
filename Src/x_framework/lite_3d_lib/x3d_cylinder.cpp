#include "stdafx.h"

#include "../tools/compare_tools.h"

#include "x3d_cylinder.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_cylinder, x3d_geometry_base, 1)

x3d_cylinder::x3d_cylinder()
{
	m_radius = 0.0;
	m_height = 0.0;
}



x3d_cylinder::x3d_cylinder(double r, double h)
{
	set_data(r, h);
}

x3d_cylinder::x3d_cylinder(const x3d_point& p, double r, double h)
{
	set_data(p, r, h);
}

x3d_cylinder::~x3d_cylinder()
{
}

void x3d_cylinder::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_cylinder* src_obj = dynamic_cast<const x3d_cylinder*>(&node);
	if (!src_obj) return;

	m_radius = src_obj->m_radius;
	m_height = src_obj->m_height;
}


void x3d_cylinder::set_data(double r, double h)
{
	m_base_point = x3d_point(0,0,0);
	m_radius = r;
	m_height = h;
}

void x3d_cylinder::set_data(const x3d_point& p, double r, double h)
{
	m_base_point = p;
	m_radius = r;
	m_height = h;
}

void x3d_cylinder::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_radius, 0.0)) return;
	if (compare_tools::is_less_equal(m_height, 0.0)) return;

	// 计算旋转角度步进量
	double angle_step = 2.0 * M_PI / calc_real_span_count(2.0 * M_PI * m_radius);
	vector<double> round_angle_value;
	for (double ai = 0.0; compare_tools::is_less(ai, 2.0 * M_PI); ai += angle_step)
		round_angle_value.push_back(ai);

	if (round_angle_value.size() < 3) return;

	// 计算顶面的细分坐标点
	vector<x3d_point> top_points;
	x3d_point top_center(m_base_point.m_x, m_base_point.m_y, m_base_point.m_z + m_height / 2.0);
	for (double x : round_angle_value)
	{
		top_points.push_back(x3d_point(m_base_point.m_x + m_radius * cos(x),
			m_base_point.m_y + m_radius * sin(x), m_base_point.m_z + m_height / 2.0));
	}

	top_points.push_back(top_points.front()); // 首尾连接

	// 圆柱顶面
	auto it_top = graph.m_all_units.insert(make_pair(_T("top_surface"), x3d_graph_unit()));
	(*it_top.first).second.m_drawing_type = x3d_drawing_type::triangles_fan;
	vector<x3d_vertex>& graph_unit_top = (*it_top.first).second.m_vertexs;

	x3d_vector top_nm(0, 0, 1);
	graph_unit_top.push_back(x3d_vertex(top_center, top_nm));
	for (x3d_point& x : top_points)
		graph_unit_top.push_back(x3d_vertex(x, top_nm));

	// 圆柱底面
	auto it_bottom = graph.m_all_units.insert(make_pair(_T("bottom_surface"), x3d_graph_unit()));
	(*it_bottom.first).second.m_drawing_type = x3d_drawing_type::triangles_fan;
	vector<x3d_vertex>& graph_unit_bottom = (*it_bottom.first).second.m_vertexs;

	x3d_vector bottom_nm(0, 0, -1);
	x3d_point bottom_center(top_center);
	bottom_center.m_z -= m_height;
	graph_unit_top.push_back(x3d_vertex(bottom_center, bottom_nm));
	for (x3d_point& x : top_points)
	{
		x3d_point pt(x.m_x, x.m_y, x.m_z - m_height);
		graph_unit_bottom.push_back(x3d_vertex(pt, bottom_nm));
	}

	// 圆柱侧面
	auto it_side = graph.m_all_units.insert(make_pair(_T("surround_surface"), x3d_graph_unit()));
	(*it_side.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit_side = (*it_side.first).second.m_vertexs;

	for (x3d_point& x : top_points)
	{
		x3d_point cur_top_pt(x);
		x3d_point cur_bottom_pt(x);
		cur_bottom_pt.m_z -= m_height;

		x3d_vector cur_nm(x.m_x - m_base_point.m_x, x.m_y - m_base_point.m_y, 0.0);
		cur_nm.normalize();

		graph_unit_side.push_back(x3d_vertex(cur_top_pt, cur_nm));
		graph_unit_side.push_back(x3d_vertex(cur_bottom_pt, cur_nm));
	}
}
