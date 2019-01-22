#include "stdafx.h"
#include "../tools/compare_tools.h"

#include "x3d_ring.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_ring, x3d_geometry_base, 1)

x3d_ring::x3d_ring()
{
	inner_radius = 0.0;
	outer_radius = 0.0;
}

x3d_ring::x3d_ring(double inner, double outer)
{
	set_data(inner, outer);
}


x3d_ring::x3d_ring(const x3d_point& p, double inner, double outer)
{
	set_data(p, inner, outer);
}

x3d_ring::~x3d_ring()
{
}

void x3d_ring::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_ring* src_obj = dynamic_cast<const x3d_ring*>(&node);
	if (!src_obj) return;

	inner_radius = src_obj->inner_radius;
	outer_radius = src_obj->outer_radius;

}

void x3d_ring::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(inner_radius,0.0)) return;
	if (compare_tools::is_less_equal(outer_radius, 0.0)) return;

	double  ring_radius = (inner_radius + outer_radius)*0.5;
	//圆环分割的单位增量（角度）
	double deta = M_PI * 2 / calc_span_count(2 * ring_radius * M_PI);
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
			double point_z = (outer_radius - inner_radius)*0.5*sin(y);
			double point_x = ring_radius * cos(x) + (outer_radius - inner_radius)*0.5*cos(y)*cos(x);
			double point_y = ring_radius * sin(x) + (outer_radius - inner_radius)*0.5*cos(y)*sin(x);
			x3d_point pt(point_x, point_y, point_z);
			x3d_vector nm = (pt - x3d_point(ring_radius * cos(x),ring_radius * sin(x),0)).get_unit_vector();

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

void x3d_ring::set_data(double inner, double outer)
{
	m_base_point = x3d_point(0, 0, 0);
	inner_radius = inner;
	outer_radius = outer;
}

void x3d_ring::set_data(const x3d_point& p, double inner, double outer)
{
	m_base_point = p;
	inner_radius = inner;
	outer_radius = outer;
}

