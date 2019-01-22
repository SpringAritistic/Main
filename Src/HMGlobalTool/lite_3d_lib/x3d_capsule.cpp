#include "stdafx.h"
#include "../tools/compare_tools.h"

#include "x3d_capsule.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_capsule, x3d_geometry_base, 1)

x3d_capsule::x3d_capsule()
{
	m_radius = 0.0;
	m_length = 0.0;
}

x3d_capsule::x3d_capsule(double r, double l)
{
	set_data(r, l);
}

x3d_capsule::x3d_capsule(const x3d_point& p, double r, double l)
{
	set_data(p, r, l);
}

x3d_capsule::~x3d_capsule()
{
}

void x3d_capsule::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_capsule* src_obj = dynamic_cast<const x3d_capsule*>(&node);
	if (!src_obj) return;

	m_radius = src_obj->m_radius;
	m_length = src_obj->m_length;
}

void x3d_capsule::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_radius, 0.0)) return;
	if (compare_tools::is_less_equal(m_length, 0.0)) return;

	// 计算赤道上的旋转角度步进量
	double angle_step = 2.0 * M_PI / calc_real_span_count(2.0 * M_PI * m_radius);
	vector<double> round_angle_value;
	for (double ai = 0.0; compare_tools::is_less(ai, 2.0 * M_PI); ai += angle_step)
		round_angle_value.push_back(ai);

	round_angle_value.push_back(2.0 * M_PI);

	// 计算圆柱侧表面的顶点
	std::pair<CString, x3d_graph_unit> cylinder_unit;
	cylinder_unit.first = _T("cylinder");
	cylinder_unit.second.m_drawing_type = x3d_drawing_type::triangles_trip;
	for (double x : round_angle_value)
	{
		x3d_point pt_up(m_radius * cos(x),m_radius * sin(x),m_length * 0.5);
		x3d_point pt_down(m_radius * cos(x), m_radius * sin(x), -m_length * 0.5);

		x3d_vector nm_up(pt_up - x3d_point(0, 0, m_length * 0.5));
		x3d_vector nm_down(pt_down - x3d_point(0, 0, -m_length * 0.5));

		cylinder_unit.second.m_vertexs.push_back(x3d_vertex(pt_up, nm_up.get_unit_vector())); // 法线必须是单位向量
		cylinder_unit.second.m_vertexs.push_back(x3d_vertex(pt_down, nm_down.get_unit_vector())); // 法线必须是单位向量
	}
	graph.m_all_units.insert(cylinder_unit);
	
	// 计算上半球顶点
	round_angle_value.pop_back();
	vector<double> half_ball_angle_value;
	for (double ai = 0.0; compare_tools::is_less(ai, M_PI_2); ai += angle_step)
		half_ball_angle_value.push_back(ai);

	half_ball_angle_value.push_back(M_PI_2);

	x3d_point up_move_vect(0.0, 0.0, 0.5 * m_length);
	vector<vector<x3d_vertex>> all_points_up;
	for (double x : round_angle_value)
	{
		vector<x3d_vertex> half_round;
		for (double y : half_ball_angle_value)
		{
			x3d_point pt(m_radius * sin(y) * cos(x), m_radius * sin(y) * sin(x), m_radius * cos(y));
			half_round.push_back(x3d_vertex(pt+ up_move_vect + m_base_point, pt.get_unit_vector())); // 法线必须是单位向量
		}

		all_points_up.push_back(half_round);
	}

	// 生成模型的三角面
	auto it = graph.m_all_units.insert(make_pair(_T("up_half_ball"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit = (*it.first).second.m_vertexs;

	for (size_t i = 0; i < all_points_up.size(); ++i)
	{
		size_t cur_index = i;
		size_t next_index = (i + 1 == all_points_up.size() ? 0 : i + 1); // 要把最后一跟经纬与0度的经纬连接起来

		for (size_t j = 0; j < all_points_up[cur_index].size(); ++j)
		{
			graph_unit.push_back(all_points_up[next_index][j]);
			graph_unit.push_back(all_points_up[cur_index][j]);
		}
	}


	// 计算下半球顶点
	vector<double> down_half_ball_angle_value;
	for (double ai = M_PI_2; compare_tools::is_less(ai, M_PI); ai += angle_step)
		down_half_ball_angle_value.push_back(ai);

	down_half_ball_angle_value.push_back(M_PI);

	x3d_point down_move_vect(0.0, 0.0, - 0.5 * m_length);
	vector<vector<x3d_vertex>> all_points_down;
	for (double x : round_angle_value)
	{
		vector<x3d_vertex> half_round;
		for (double y : down_half_ball_angle_value)
		{
			x3d_point pt(m_radius * sin(y) * cos(x), m_radius * sin(y) * sin(x), m_radius * cos(y));
			half_round.push_back(x3d_vertex(pt + down_move_vect + m_base_point, pt.get_unit_vector())); // 法线必须是单位向量
		}

		all_points_down.push_back(half_round);
	}

	// 生成模型的三角面
	auto item = graph.m_all_units.insert(make_pair(_T("down_half_ball"), x3d_graph_unit()));
	(*item.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit_down = (*item.first).second.m_vertexs;

	for (size_t i = 0; i < all_points_down.size(); ++i)
	{
		size_t cur_index = i;
		size_t next_index = (i + 1 == all_points_down.size() ? 0 : i + 1); // 要把最后一跟经纬与0度的经纬连接起来

		for (size_t j = 0; j < all_points_down[cur_index].size(); ++j)
		{
			graph_unit_down.push_back(all_points_down[next_index][j]);
			graph_unit_down.push_back(all_points_down[cur_index][j]);
		}
	}
}

void x3d_capsule::set_data(double r, double l)
{
	set_data(x3d_point(0, 0, 0), r, l);
}

void x3d_capsule::set_data(const x3d_point& p, double r, double l)
{
	m_base_point = p;
	m_radius = r;
	m_length = l;
}
