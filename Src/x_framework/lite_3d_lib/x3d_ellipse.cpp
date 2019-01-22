#include "stdafx.h"

#include "../tools/compare_tools.h"

#include "x3d_ellipse.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_ellipse, x3d_geometry_base, 1)

x3d_ellipse::x3d_ellipse()
{
	m_a = 0.0;
	m_b = 0.0;
}


x3d_ellipse::x3d_ellipse(double a, double b)
{
	set_data(a, b);
}

x3d_ellipse::x3d_ellipse(const x3d_point& p, double a, double b)
{
	set_data(p, a, b);
}

x3d_ellipse::~x3d_ellipse()
{
}

void x3d_ellipse::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_ellipse* src_obj = dynamic_cast<const x3d_ellipse*>(&node);
	if (!src_obj) return;

	m_a = src_obj->m_a;
	m_b = src_obj->m_b;
}

void x3d_ellipse::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_a, 0.0)) return;
	if (compare_tools::is_less_equal(m_b, 0.0)) return;

	double angle_step = 2.0 * M_PI / calc_real_span_count(2 * M_PI * m_b + 4 * m_a - 4 * m_b);
	vector<double> angle_value;
	for (double ai = 0.0; compare_tools::is_less(ai, 2.0 * M_PI); ai += angle_step)
		angle_value.push_back(ai);

	if (angle_value.size() < 3) return;

	auto it = graph.m_all_units.insert(make_pair(_T("unit"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::line_loop;
	vector<x3d_vertex>& graph_unit = (*it.first).second.m_vertexs;

	x3d_vector nm(0, 0, 1);
	for (double x : angle_value)
	{
		x3d_point pn(m_a*cos(x), m_b*sin(x), 0);
		graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
	}
}

void x3d_ellipse::set_data(double a, double b)
{
	m_base_point = x3d_point(0, 0, 0);
	m_a = a;
	m_b = b;
}

void x3d_ellipse::set_data(const x3d_point& p, double a, double b)
{
	m_base_point = p;
	m_a = a;
	m_b = b;

}
