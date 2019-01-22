#include "stdafx.h"

#include "../tools/compare_tools.h"

#include "x3d_ball.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_ball, x3d_geometry_base, 1)

x3d_ball::x3d_ball()
{
	m_radius = 0.0;
}



x3d_ball::x3d_ball(double r)
{
	set_data(r);
}

x3d_ball::x3d_ball(const x3d_point& p, double r)
{
	set_data(p, r);
}

x3d_ball::~x3d_ball()
{
}

void x3d_ball::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_ball* src_obj = dynamic_cast<const x3d_ball*>(&node);
	if (!src_obj) return;

	m_radius = src_obj->m_radius;
}


void x3d_ball::set_data(double r)
{
	m_base_point = x3d_point(0, 0, 0);
	m_radius = r;
}

void x3d_ball::set_data(const x3d_point& p, double r)
{
	m_base_point = p;
	m_radius = r;
}

void x3d_ball::regenerate_vertext(x3d_graph& graph)
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
	for (double ai = 0; compare_tools::is_less(ai, M_PI); ai += angle_step)
        half_round_angle_value.push_back(ai);

	half_round_angle_value.push_back(M_PI); // 必须保证两个极点的角度都放在数组中

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
		
		for (size_t j = 0; j < all_points[cur_index].size(); ++j)
		{
			graph_unit.push_back(all_points[next_index][j]);
			graph_unit.push_back(all_points[cur_index][j]);
		}
	}
}
