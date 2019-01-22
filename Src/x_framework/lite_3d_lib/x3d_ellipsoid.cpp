#include "stdafx.h"
#include "../tools/compare_tools.h"

#include "x3d_ellipsoid.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_ellipsoid, x3d_geometry_base, 1)

x3d_ellipsoid::x3d_ellipsoid()
{
	m_x_axis_radius = 0.0;
	m_y_axis_radius = 0.0;
	m_z_axis_radius = 0.0;
}


x3d_ellipsoid::x3d_ellipsoid(const x3d_point& p, double m_x, double m_y, double m_z)
{
	set_data(p, m_x, m_y, m_z);
}

x3d_ellipsoid::x3d_ellipsoid(double m_x, double m_y, double m_z)
{
	set_data(m_x, m_y, m_z);
}

x3d_ellipsoid::~x3d_ellipsoid()
{
}

void x3d_ellipsoid::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_ellipsoid* src_obj = dynamic_cast<const x3d_ellipsoid*>(&node);
	if (!src_obj) return;

	m_x_axis_radius = src_obj->m_x_axis_radius;
	m_y_axis_radius = src_obj->m_y_axis_radius;
	m_z_axis_radius = src_obj->m_z_axis_radius;
}

void x3d_ellipsoid::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_x_axis_radius, 0.0)) return;
	if (compare_tools::is_less_equal(m_y_axis_radius, 0.0)) return;
	if (compare_tools::is_less_equal(m_z_axis_radius, 0.0)) return;

	// 计算x-y平面上的旋转角度步进量
	double angle_step = 2.0 * M_PI / calc_real_span_count(2.0 * M_PI * m_x_axis_radius + 4 * (m_x_axis_radius - m_y_axis_radius));
	vector<double> round_angle_value;
	for (double ai = 0.0; compare_tools::is_less(ai, 2.0 * M_PI); ai += angle_step)
		round_angle_value.push_back(ai);

	// 计算每根经线上的角度旋转步进量（由于X-Y平面上旋转了一周，因此经线上只要旋转半圈
	vector<double> half_round_angle_value;
	for (double ai = 0; compare_tools::is_less(ai, M_PI); ai += angle_step)
		half_round_angle_value.push_back(ai);

	half_round_angle_value.push_back(M_PI); // 必须保证两个极点的角度都放在数组中

	// 计算所有顶点坐标及法线
	vector<vector<x3d_vertex>> all_points;
	for (double x : half_round_angle_value)
	{
		vector<x3d_vertex> single_round;
		for (double y : round_angle_value)
		{
			x3d_point pt(m_x_axis_radius * sin(x) * cos(y),
				m_y_axis_radius * sin(x) * sin(y),m_z_axis_radius *cos(x));

			single_round.push_back(x3d_vertex(pt + m_base_point, pt.get_unit_vector())); // 法线必须是单位向量
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

		graph_unit.push_back(all_points[cur_index][0]);
		graph_unit.push_back(all_points[next_index][0]);
		for (size_t j = 1; j < all_points[cur_index].size(); ++j)
		{
			graph_unit.push_back(all_points[cur_index][j]);
			graph_unit.push_back(all_points[next_index][j]);
		}
	}
}

void x3d_ellipsoid::set_data(const x3d_point& p, double m_x, double m_y, double m_z)
{
	m_base_point = p;
	m_x_axis_radius = m_x;
	m_y_axis_radius = m_y;
	m_z_axis_radius = m_z;
}

void x3d_ellipsoid::set_data(double m_x, double m_y, double m_z)
{
	m_base_point = x3d_point(0, 0, 0);
	m_x_axis_radius = m_x;
	m_y_axis_radius = m_y;
	m_z_axis_radius = m_z;
}
