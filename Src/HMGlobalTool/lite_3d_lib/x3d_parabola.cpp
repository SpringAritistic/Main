#include "stdafx.h"

#include "../tools/compare_tools.h"

#include "x3d_parabola.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_parabola, x3d_geometry_base, 1)

x3d_parabola::x3d_parabola()
{
}



x3d_parabola::x3d_parabola(double w, double h, double ll, double rl, direction dr /*= direction::yz*/, float n /*= 2*/)
{
	set_data(w, h, ll, rl, dr, n);
}

x3d_parabola::x3d_parabola(const x3d_point& p, double w, double h, double ll, double rl, direction dr /*= direction::yz*/, float n /*= 2*/)
{
	set_data(p, w, h, ll, rl, dr, n);
}

x3d_parabola::~x3d_parabola()
{
}

void x3d_parabola::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_parabola* src_obj = dynamic_cast<const x3d_parabola*>(&node);
	if (!src_obj) return;

	m_width = src_obj->m_width;
	m_height = src_obj->m_height;
	m_left_length = src_obj->m_left_length;
	m_right_length = src_obj->m_right_length;
	m_dir = src_obj->m_dir;
	m_n = src_obj->m_n;
}

void x3d_parabola::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_n, 1.0)) return;
	if (compare_tools::is_less_equal(m_width, 0.0) || compare_tools::is_less_equal(m_height, 0.0) ||
		compare_tools::is_less(m_left_length, 0.0) || compare_tools::is_less(m_right_length, 0.0)) return;

    double dl = (m_left_length + m_right_length) / calc_real_span_count(m_left_length + m_right_length);

	vector<double> all_x;
	for (double x = -m_left_length; compare_tools::is_less(x, m_right_length); x += dl)
		all_x.push_back(x);

	all_x.push_back(m_right_length);
	auto it = graph.m_all_units.insert(make_pair(_T("unit"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::line_strip;
	vector<x3d_vertex>& graph_unit = (*it.first).second.m_vertexs;

	x3d_vector nm(0, 0, 1);
	double a;                                          // Y = aX`n 的系数a
	if (compare_tools::is_equal(m_n, 2.0))            // 如果是二次抛物线
		a = m_height / (m_width * m_width);    
	else                                             // 如果不是二次幂抛物线
		a = m_height / pow(m_width, m_n);             // 求出Y = aX`n 的系数a

	if (direction::xy == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(a * x * x, -x, 0);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::_xy == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(-a * x * x, -x, 0);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::yx == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(x, a * x * x, 0);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::_yx == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(x, -a * x * x, 0);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::xz == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(a * x * x, 0, -x);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::_xz == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(-a * x * x, 0, -x);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::zx == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(x, 0, a * x * x);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::_zx == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(x, 0, -a * x * x);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::yz == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(0, a * x * x, -x);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::_yz == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(0, -a * x * x, -x);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::zy == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(0, x, a * x * x);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else if (direction::_zy == m_dir)
	{
		for (double x : all_x)
		{
			x3d_point pn(0, x, -a * x * x);
			graph_unit.push_back(x3d_vertex(pn + m_base_point, nm));
		}
	}
	else
	{
		// nothing
	}
}


void x3d_parabola::set_data(double w, double h, double ll, double rl, direction dr /*= direction::yz*/, float n /*= 2*/)
{
	m_width = w;
	m_height = h;
	m_left_length = ll;
	m_right_length = rl;
	m_dir = dr;
	m_n = n;
}

void x3d_parabola::set_data(const x3d_point& p, double w, double h, double ll, double rl, direction dr /*= direction::yz*/, float n /*= 2*/)
{
	m_base_point = p;
	m_width = w;
	m_height = h;
	m_left_length = ll;
	m_right_length = rl;
	m_dir = dr;
	m_n = n;
}

