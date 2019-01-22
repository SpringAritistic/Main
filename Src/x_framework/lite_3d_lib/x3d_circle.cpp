#include "stdafx.h"
#include "../tools/compare_tools.h"

#include "x3d_circle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_circle, x3d_geometry_base, 1)

x3d_circle::x3d_circle()
{
	m_radius = 0.0;
}

x3d_circle::x3d_circle(double r, x3d_plane_type tp/* = x3d_plane_type::oxy*/)
{
	set_data(r, tp);
}

x3d_circle::x3d_circle(x3d_point& p, double r, x3d_plane_type tp/* = x3d_plane_type::oxy*/)
{
	set_data(p, r, tp);
}

x3d_circle::~x3d_circle()
{
}

void x3d_circle::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_circle* src_obj = dynamic_cast<const x3d_circle*>(&node);
	if (!src_obj) return;

	m_radius = src_obj->m_radius;
}

void x3d_circle::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_radius, 0.0)) return;

	double angle_step = 2.0 * M_PI / calc_real_span_count(2.0 * M_PI * m_radius);
	vector<double> angle_value;
	for (double ai = 0.0; compare_tools::is_less(ai, 2.0 * M_PI); ai += angle_step)
		angle_value.push_back(ai);

	angle_value.push_back(2.0 * M_PI);
	if (angle_value.size() < 3) return;

	auto it = graph.m_all_units.insert(make_pair(_T("unit"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::line_strip;
	vector<x3d_vertex>& graph_unit = (*it.first).second.m_vertexs;

	x3d_vector nm(0, 0, 1);
	if (x3d_plane_type::oxy == m_type)
	{
		for (double x : angle_value)
		{
			x3d_point pn(m_radius*cos(x), m_radius*sin(x), 0);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (x3d_plane_type::oxz == m_type)
	{
		for (double x : angle_value)
		{
			x3d_point pn(m_radius*cos(x), 0, m_radius*sin(x));
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (x3d_plane_type::oyz == m_type)
	{
		for (double x : angle_value)
		{
			x3d_point pn(0, m_radius*cos(x), m_radius*sin(x));
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else
	{
		// nothing
	}
}


void x3d_circle::set_data(double r, x3d_plane_type tp/* = x3d_plane_type::oxy*/)
{
	m_base_point = x3d_point(0, 0, 0);
	m_radius = r;
	m_type = tp;
}

void x3d_circle::set_data(x3d_point& p, double r, x3d_plane_type tp/* = x3d_plane_type::oxy*/)
{
	m_base_point = p;
	m_radius = r;
	m_type = tp;
}
