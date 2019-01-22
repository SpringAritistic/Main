#include "stdafx.h"

#include "../tools/compare_tools.h"
#include "x3d_line.h"

#include "x3d_action_rotate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_action_rotate, x3d_action_base, 1)

x3d_action_rotate::x3d_action_rotate()
{
	m_section = nullptr;
	m_line = nullptr;
}


x3d_action_rotate::x3d_action_rotate(x3d_action_section* sec, double ang /*= M_PI*/, double sp /*= M_PI_4 / 4*/)
{
	set_data(sec, ang, sp);
}

x3d_action_rotate::x3d_action_rotate(x3d_action_section* sec, x3d_line* ln, double ang /*= 2 * M_PI*/, int n /*= 16*/)
{
	set_data(sec, ln, ang, n);
}


x3d_action_rotate::~x3d_action_rotate()
{
	safe_delete(m_section);
	safe_delete(m_line);
}

void x3d_action_rotate::do_action(x3d_graph& result)
{
	if (!m_section) return;
	if (!m_line) return;
	if (compare_tools::is_less_equal(m_angle, 0.0)) return;

	x3d_point& p1 = m_line->m_s;
	x3d_point& p2 = m_line->m_e;
	
	// 计算赤道上的旋转角度步进量
	double angle_step = m_angle / m_count;
	vector<double> value;
	for (double ai = 0.0; compare_tools::is_less(ai, m_angle); ai += angle_step)
		value.push_back(ai);

	value.push_back(m_angle);

	// 生成模型的三角面
	auto it = result.m_all_units.insert(make_pair(_T("surround_face"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit = (*it.first).second.m_vertexs;


	for (size_t i = 0; i != m_section->m_all_points.size() - 1; ++i)
	{
		x3d_point& p = m_section->m_all_points[i].p;
		x3d_point& pn = m_section->m_all_points[i + 1].p;
		for (auto x : value)
		{
			x3d_trans_matrix mx;
			mx.rotate(p1, p2, x);
			x3d_point pn1 = mx.transform_vertex(p);
			x3d_point pn2 = mx.transform_vertex(pn);

			graph_unit.push_back(x3d_vertex(pn1, pn1.get_unit_vector()));
			graph_unit.push_back(x3d_vertex(pn2, pn2.get_unit_vector()));

		}
	}
}

void x3d_action_rotate::set_data(x3d_action_section* sec, double ang /*= M_PI*/, double sp /*= M_PI_4 / 4*/)
{
	m_section = sec;
	m_angle = ang;
	m_step = sp;
}

void x3d_action_rotate::set_data(x3d_action_section* sec, x3d_line* ln, double ang /*= 2 * M_PI*/, int n /*= 16*/)
{
	m_section = sec;
	m_line = ln;
	m_angle = ang;
	m_count = n;
}

void x3d_action_rotate::rotate_along_zaxis(x3d_graph& result)
{
	if (!m_section) return;
	if (compare_tools::is_less_equal(m_angle, 0.0)) return;

	x3d_point& p1 = m_line->m_s;
	x3d_point& p2 = m_line->m_e;
	x3d_trans_matrix mx1;
	mx1.move(-p1.m_x, -p1.m_y, -p1.m_z);


	// 计算赤道上的旋转角度步进量
	vector<double> value;
	for (double ai = 0.0; compare_tools::is_less(ai, 2.0 * M_PI); ai += m_step)
		value.push_back(ai);

	value.push_back(0);

	// 生成模型的三角面
	auto it = result.m_all_units.insert(make_pair(_T("surround_face"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit = (*it.first).second.m_vertexs;


	for (size_t i = 0; i != m_section->m_all_points.size() - 1; ++i)
	{
		x3d_point& p = m_section->m_all_points[i].p;
		x3d_point& pn = m_section->m_all_points[i + 1].p;
		for (auto x : value)
		{
			x3d_point pn1(p.m_x*cos(x) - p.m_y*sin(x), p.m_x*sin(x) + p.m_y*cos(x), p.m_z);
			x3d_point pn2(pn.m_x*cos(x) - pn.m_y*sin(x), pn.m_x*sin(x) + pn.m_y*cos(x), pn.m_z);
			graph_unit.push_back(x3d_vertex(pn1, pn1.get_unit_vector()));
			graph_unit.push_back(x3d_vertex(pn2, pn2.get_unit_vector()));

		}
	}
}

