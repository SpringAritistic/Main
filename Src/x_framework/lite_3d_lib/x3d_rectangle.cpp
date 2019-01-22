#include "stdafx.h"

#include "../tools/compare_tools.h"

#include "x3d_rectangle.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_rectangle, x3d_geometry_base, 1)

x3d_rectangle::x3d_rectangle()
{
	m_l = 0;
	m_w = 0;
}


x3d_rectangle::x3d_rectangle(double l, double w)
{
    set_data(l, w);
}

x3d_rectangle::x3d_rectangle(const x3d_point& p, double l, double w)
{
    set_data(p, l, w);
}


x3d_rectangle::~x3d_rectangle()
{
}

void x3d_rectangle::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_rectangle* src_obj = dynamic_cast<const x3d_rectangle*>(&node);
	if (!src_obj) return;

	m_l = src_obj->m_l;
	m_w = src_obj->m_w;

}

void x3d_rectangle::set_data(double l, double w)
{
	m_l = l;
    m_w = w;
}

void x3d_rectangle::set_data(const x3d_point& p, double l, double w)
{
	m_base_point = p;
	m_l = l;
	m_w = w;
}

void x3d_rectangle::regenerate_vertext(x3d_graph& graph)
{
    graph.m_all_units.clear();
    if (compare_tools::is_less_equal(m_l, 0.0)) return;
    if (compare_tools::is_less_equal(m_w, 0.0)) return;

	auto it = graph.m_all_units.insert(make_pair(_T("lines"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::line_loop;
	vector<x3d_vertex>& unit = (*it.first).second.m_vertexs;

	x3d_vector nm(0, 0, 1);
	x3d_point p1(-m_l / 2, -m_w / 2, 0);
	x3d_point p2(m_l / 2, -m_w / 2, 0);
	x3d_point p3(m_l / 2, m_w / 2, 0);
	x3d_point p4(-m_l / 2, m_w / 2, 0);
	unit.push_back(x3d_vertex(p1 + m_base_point, nm));
	unit.push_back(x3d_vertex(p2 + m_base_point, nm));
	unit.push_back(x3d_vertex(p4 + m_base_point, nm));
	unit.push_back(x3d_vertex(p3 + m_base_point, nm));
}
