#include "stdafx.h"
#include "../tools/compare_tools.h"

#include "x3d_elliptic_ring.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

#define circle_count 36
IMPLEMENT_SERIAL(x3d_elliptic_ring, x3d_geometry_base, 1)

x3d_elliptic_ring::x3d_elliptic_ring()
{
	m_semi_major_axis = 0.0;
	m_semi_minor_axis = 0.0;
	m_radius = 0.0;
}


x3d_elliptic_ring::x3d_elliptic_ring(double a, double b, double radius)
{
	set_data(a, b, radius);
}

x3d_elliptic_ring::x3d_elliptic_ring(const x3d_point& p, double a, double b, double radius)
{
	set_data(p, a, b, radius);
}

x3d_elliptic_ring::~x3d_elliptic_ring()
{
}

void x3d_elliptic_ring::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_elliptic_ring* src_obj = dynamic_cast<const x3d_elliptic_ring*>(&node);
	if (!src_obj) return;

	m_semi_major_axis = src_obj->m_semi_major_axis;
	m_semi_minor_axis = src_obj->m_semi_minor_axis;
	m_radius = src_obj->m_radius;
}

void x3d_elliptic_ring::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_semi_major_axis, 0.0)) return;
	if (compare_tools::is_less_equal(m_semi_minor_axis, 0.0)) return;
	if (compare_tools::is_less_equal(m_radius, 0.0)) return;

	//圆环分割的单位增量（角度）
	double length = sqrt(m_semi_major_axis * m_semi_major_axis + m_semi_minor_axis * m_semi_minor_axis);
	double total_length = 2.0 * M_PI * m_semi_minor_axis + 4.0 * (m_semi_major_axis - m_semi_minor_axis);
	double deta = M_PI * 2 / calc_span_count(total_length);
	std::vector<double> round_angle_value;
	for (double angle_value = 0; compare_tools::is_less(angle_value, M_PI * 2); angle_value += deta)
	{
		round_angle_value.push_back(angle_value);
	}
	round_angle_value.push_back(M_PI * 2);

	x3d_graph_unit unit;
	unit.m_drawing_type = x3d_drawing_type::triangles_trip;

	//divide the surfaces into the triangle vertices
	std::vector<std::vector<x3d_vertex>> temp_vectex;
	for (double x : round_angle_value)
	{
		std::vector<x3d_vertex> single_circle_data;
		for (double y : round_angle_value)
		{
			double point_z = m_radius * sin(y);
			double point_x = m_semi_major_axis* cos(x) + m_radius*0.5*cos(y)*cos(x);
			double point_y = m_semi_minor_axis * sin(x) + m_radius*0.5*cos(y)*sin(x);
			x3d_point pt(point_x, point_y, point_z);
			x3d_vector nm = (pt - x3d_point(m_semi_major_axis * cos(x), m_semi_minor_axis * sin(x), 0)).get_unit_vector();

			single_circle_data.push_back(x3d_vertex(pt + m_base_point, nm));
		}

		temp_vectex.push_back(single_circle_data);
	}

	//assemble all the triangle vertices to triangle faces
	for (size_t i = 0; i < temp_vectex.size(); ++i)
	{
		size_t cur_index = i;
		size_t next_index = (i + 1 == temp_vectex.size() ? 0 : i + 1); // 要把最后一跟经纬与0度的经纬连接起来

		for (size_t j = 0; j < temp_vectex[cur_index].size(); ++j)
		{
			unit.m_vertexs.push_back(temp_vectex[next_index][j]);
			unit.m_vertexs.push_back(temp_vectex[cur_index][j]);
		}
	}

	graph.m_all_units[_T("unit")] = unit;
}

void x3d_elliptic_ring::set_data(double a, double b, double radius)
{
	m_base_point = x3d_point(0, 0, 0);
	m_semi_major_axis = a;
	m_semi_minor_axis = b;
	m_radius = radius;
}

void x3d_elliptic_ring::set_data(const x3d_point& p, double a, double b, double radius)
{
	m_base_point = p;
	m_semi_major_axis = a;
	m_semi_minor_axis = b;
	m_radius = radius;
}
