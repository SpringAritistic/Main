#include "stdafx.h"

#include "../tools/compare_tools.h"

#include "x3d_line.h"
#include "x3d_cone.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_cone, x3d_geometry_base, 1)

x3d_cone::x3d_cone()
{
	m_radius = 0.0;
	m_height = 0.0;
}

x3d_cone::x3d_cone(double r, double h)
{
	set_data(r, h);
}

x3d_cone::x3d_cone(const x3d_point& p, double r, double h)
{
	set_data(p, r, h);
}

x3d_cone::~x3d_cone()
{
}

void x3d_cone::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_cone* src_obj = dynamic_cast<const x3d_cone*>(&node);
	if (!src_obj) return;

	m_radius = src_obj->m_radius;
	m_height = src_obj->m_height;
}


void x3d_cone::set_data(double r, double h)
{
	m_base_point = x3d_point(0, 0, 0);
	m_radius = r;
	m_height = h;
}

void x3d_cone::set_data(const x3d_point& p, double r, double h)
{
	m_base_point = p;
	m_radius = r;
	m_height = h;
}

void x3d_cone::regenerate_vertext(x3d_graph& graph)
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
	vector<x3d_point> bottom_points;
	x3d_point bottom_center(m_base_point);
	for (double x : round_angle_value)
		bottom_points.push_back(x3d_point(bottom_center.m_x + m_radius * cos(x),
			bottom_center.m_y + m_radius * sin(x), bottom_center.m_z));

	bottom_points.push_back(bottom_points.front()); // 首尾连接

	// 圆锥底面
	auto it_bottom = graph.m_all_units.insert(make_pair(_T("bottom_surface"), x3d_graph_unit()));
	(*it_bottom.first).second.m_drawing_type = x3d_drawing_type::triangles_fan;
	vector<x3d_vertex>& graph_unit_bottom = (*it_bottom.first).second.m_vertexs;

	// 圆锥侧面
	auto it_surround = graph.m_all_units.insert(make_pair(_T("surround_surface"), x3d_graph_unit()));
	(*it_surround.first).second.m_drawing_type = x3d_drawing_type::triangles;
	vector<x3d_vertex>& graph_unit_surround = (*it_surround.first).second.m_vertexs;

	x3d_vector bottom_nm(0, 0, -1);
	x3d_point pb = get_base_point();
	x3d_point pt = x3d_point(pb.m_x, pb.m_y, pb.m_z + m_height);
	graph_unit_bottom.push_back(x3d_vertex(pb, bottom_nm));
	//graph_unit_surround.push_back(x3d_vertex(pt, bottom_nm));
	for (size_t i = 0; i != bottom_points.size(); ++i)
	{
		graph_unit_bottom.push_back(x3d_vertex(bottom_points[i], bottom_nm));
		//x3d_point fb = x3d_line(pt, bottom_points[i]).get_footpoint_on_line(pb);
		//x3d_vector nm = pb - fb;
		size_t next_index = (i + 1 == bottom_points.size() ? 0 : i + 1);
		x3d_vector nm = x3d_plane(pt, bottom_points[i], bottom_points[next_index]).get_normal_vector();
		nm.normalize();
		graph_unit_surround.push_back(x3d_vertex(pt, nm));
		graph_unit_surround.push_back(x3d_vertex(bottom_points[i], nm));
		graph_unit_surround.push_back(x3d_vertex(bottom_points[next_index], nm));
	}
}

