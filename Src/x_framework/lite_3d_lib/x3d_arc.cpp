#include "stdafx.h"

#include "../tools/compare_tools.h"

#include "x3d_arc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_arc, x3d_geometry_base, 1)

x3d_arc::x3d_arc()
{
	m_radius = 0.0;
	m_angle = 0.0;
	m_start_angle = 0.0;
}


x3d_arc::x3d_arc(double r, double a, double s, x3d_plane_type tp /*= x3d_plane_type::oxy*/)
{
	set_data(r, a, s, tp);
}

x3d_arc::x3d_arc(const x3d_point& p, double r, double a, double s, x3d_plane_type tp /*= x3d_plane_type::oxy*/)
{
	set_data(p, r, a, s, tp);
}

x3d_arc::~x3d_arc()
{
}

void x3d_arc::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_arc* src_obj = dynamic_cast<const x3d_arc*>(&node);
	if (!src_obj) return;

	m_radius = src_obj->m_radius;
	m_angle = src_obj->m_angle;
	m_start_angle = src_obj->m_start_angle;
}

void x3d_arc::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_radius, 0.0)) return;
	if (compare_tools::is_less_equal(m_angle, 0.0)) return;

	double angle_step = m_angle / calc_real_span_count(m_angle * m_radius);
	vector<double> angle_value;
	for (double ai = 0.0; compare_tools::is_less(ai, m_angle); ai += angle_step)
		angle_value.push_back(ai);

	angle_value.push_back(m_angle);
	auto it = graph.m_all_units.insert(make_pair(_T("unit"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::line_strip;
	vector<x3d_vertex>& graph_unit = (*it.first).second.m_vertexs;

	x3d_vector nm(0, 0, 1);
	if (x3d_plane_type::oxy == m_type)
	{
		for (double x : angle_value)
		{
			x3d_point pn(m_radius*cos(x + m_start_angle), m_radius*sin(x + m_start_angle), 0);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (x3d_plane_type::oxz == m_type)
	{
		for (double x : angle_value)
		{
			x3d_point pn(m_radius*cos(x + m_start_angle), 0, m_radius*sin(x + m_start_angle));
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (x3d_plane_type::oyz == m_type)
	{
		for (double x : angle_value)
		{
			x3d_point pn(0, m_radius*cos(x + m_start_angle), m_radius*sin(x + m_start_angle));
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else
	{
		// nothing
	}
}


void x3d_arc::set_data(double r, double a, double s, x3d_plane_type tp /*= x3d_plane_type::oxy*/)
{
	m_base_point = x3d_point(0, 0, 0);
	m_radius = r;
	m_angle = a;
	m_start_angle = s;
	m_type = tp;
}

void x3d_arc::set_data(const x3d_point& p, double r, double a, double s, x3d_plane_type tp /*= x3d_plane_type::oxy*/)
{
	m_base_point = p;
	m_radius = r;
	m_angle = a;
	m_start_angle = s;
	m_type = tp;
}

x3d_arc2::x3d_arc2()
{

}

x3d_arc2::x3d_arc2(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3)
{
	set_data(p1, p2, p3);
}

x3d_arc2::~x3d_arc2()
{

}

x3d_point x3d_arc2::get_center() const
{
	// 推导过程参考：http://blog.csdn.net/yanmy2012/article/details/8111600
	double a1, b1, c1, d1;
	double a2, b2, c2, d2;
	double a3, b3, c3, d3;

	double x1 = m_p1.m_x, y1 = m_p1.m_y, z1 = m_p1.m_z;
	double x2 = m_p2.m_x, y2 = m_p2.m_y, z2 = m_p2.m_z;
	double x3 = m_p3.m_x, y3 = m_p3.m_y, z3 = m_p3.m_z;

	a1 = (y1*z2 - y2*z1 - y1*z3 + y3*z1 + y2*z3 - y3*z2);
	b1 = -(x1*z2 - x2*z1 - x1*z3 + x3*z1 + x2*z3 - x3*z2);
	c1 = (x1*y2 - x2*y1 - x1*y3 + x3*y1 + x2*y3 - x3*y2);
	d1 = -(x1*y2*z3 - x1*y3*z2 - x2*y1*z3 + x2*y3*z1 + x3*y1*z2 - x3*y2*z1);

	a2 = 2 * (x2 - x1);
	b2 = 2 * (y2 - y1);
	c2 = 2 * (z2 - z1);
	d2 = x1 * x1 + y1 * y1 + z1 * z1 - x2 * x2 - y2 * y2 - z2 * z2;

	a3 = 2 * (x3 - x1);
	b3 = 2 * (y3 - y1);
	c3 = 2 * (z3 - z1);
	d3 = x1 * x1 + y1 * y1 + z1 * z1 - x3 * x3 - y3 * y3 - z3 * z3;

	x3d_point result;
	result.m_x = -(b1*c2*d3 - b1*c3*d2 - b2*c1*d3 + b2*c3*d1 + b3*c1*d2 - b3*c2*d1)
		/ (a1*b2*c3 - a1*b3*c2 - a2*b1*c3 + a2*b3*c1 + a3*b1*c2 - a3*b2*c1);
	result.m_y = (a1*c2*d3 - a1*c3*d2 - a2*c1*d3 + a2*c3*d1 + a3*c1*d2 - a3*c2*d1)
		/ (a1*b2*c3 - a1*b3*c2 - a2*b1*c3 + a2*b3*c1 + a3*b1*c2 - a3*b2*c1);
	result.m_z = -(a1*b2*d3 - a1*b3*d2 - a2*b1*d3 + a2*b3*d1 + a3*b1*d2 - a3*b2*d1)
		/ (a1*b2*c3 - a1*b3*c2 - a2*b1*c3 + a2*b3*c1 + a3*b1*c2 - a3*b2*c1);

	return result;
}

void x3d_arc2::set_data(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3)
{
	m_p1 = p1;
	m_p2 = p2;
	m_p3 = p3;
}
