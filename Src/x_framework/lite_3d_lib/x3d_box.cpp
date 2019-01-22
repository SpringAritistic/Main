#include "stdafx.h"

#include "../tools/compare_tools.h"
#include "x3d_box.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_box, x3d_geometry_base, 1)

x3d_box::x3d_box()
{
	m_l = 0.0;
	m_w = 0.0;
	m_h = 0.0;
}

x3d_box::x3d_box(double l, double w, double h)
{
	set_data(l, w, h);
}

x3d_box::x3d_box(const x3d_point& p, double l, double w, double h)
{
	set_data(p, l, w, h);
}

x3d_box::~x3d_box()
{
}

void x3d_box::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_box* src_obj = dynamic_cast<const x3d_box*>(&node);
	if (!src_obj) return;

	m_l = src_obj->m_l;
	m_w = src_obj->m_w;
	m_h = src_obj->m_h;
}


void x3d_box::set_data(double l, double w, double h)
{
	m_base_point = x3d_point(0, 0, 0);
	m_l = l;
	m_w = w;
	m_h = h;
}

void x3d_box::set_data(const x3d_point& p, double l, double w, double h)
{
	m_base_point = p;
	m_l = l;
	m_w = w;
	m_h = h;
}

void x3d_box::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
	if (compare_tools::is_less_equal(m_l, 0.0)) return;
	if (compare_tools::is_less_equal(m_w, 0.0)) return;
	if (compare_tools::is_less_equal(m_h, 0.0)) return;

	auto it_1 = graph.m_all_units.insert(make_pair(_T("unit_1"), x3d_graph_unit()));
	(*it_1.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit_1 = (*it_1.first).second.m_vertexs;

	auto it_2 = graph.m_all_units.insert(make_pair(_T("unit_2"), x3d_graph_unit()));
	(*it_2.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit_2 = (*it_2.first).second.m_vertexs;

	auto it_3 = graph.m_all_units.insert(make_pair(_T("unit_3"), x3d_graph_unit()));
	(*it_3.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit_3 = (*it_3.first).second.m_vertexs;

	auto it_4 = graph.m_all_units.insert(make_pair(_T("unit_4"), x3d_graph_unit()));
	(*it_4.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit_4 = (*it_4.first).second.m_vertexs;

	auto it_5 = graph.m_all_units.insert(make_pair(_T("unit_5"), x3d_graph_unit()));
	(*it_5.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit_5 = (*it_5.first).second.m_vertexs;

	auto it_6 = graph.m_all_units.insert(make_pair(_T("unit_6"), x3d_graph_unit()));
	(*it_6.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
	vector<x3d_vertex>& graph_unit_6 = (*it_6.first).second.m_vertexs;

	x3d_point p1(-m_l / 2, m_w / 2, -m_h / 2);
	x3d_point p2(m_l / 2, m_w / 2, -m_h / 2);
	x3d_point p3(m_l / 2, -m_w / 2, -m_h / 2);
	x3d_point p4(-m_l / 2, -m_w / 2, -m_h / 2);
	x3d_point p5(-m_l / 2, m_w / 2, m_h / 2);
	x3d_point p6(m_l / 2, m_w / 2, m_h / 2);
	x3d_point p7(m_l / 2, -m_w / 2, m_h / 2);
	x3d_point p8(-m_l / 2, -m_w / 2, m_h / 2);
	x3d_vector nv1(0, 1, 0);
	x3d_vector nv2(1, 0, 0);
	x3d_vector nv3(0, 0, 1);
	graph_unit_1.push_back(x3d_vertex(p1 + m_base_point, -nv3));
	graph_unit_1.push_back(x3d_vertex(p2 + m_base_point, -nv3));
	graph_unit_1.push_back(x3d_vertex(p4 + m_base_point, -nv3));
	graph_unit_1.push_back(x3d_vertex(p3 + m_base_point, -nv3));
	graph_unit_2.push_back(x3d_vertex(p2 + m_base_point, nv2));
	graph_unit_2.push_back(x3d_vertex(p3 + m_base_point, nv2));
	graph_unit_2.push_back(x3d_vertex(p6 + m_base_point, nv2));
	graph_unit_2.push_back(x3d_vertex(p7 + m_base_point, nv2));
	graph_unit_3.push_back(x3d_vertex(p1 + m_base_point, nv1));
	graph_unit_3.push_back(x3d_vertex(p2 + m_base_point, nv1));
	graph_unit_3.push_back(x3d_vertex(p5 + m_base_point, nv1));
	graph_unit_3.push_back(x3d_vertex(p6 + m_base_point, nv1));
	graph_unit_4.push_back(x3d_vertex(p5 + m_base_point, nv3));
	graph_unit_4.push_back(x3d_vertex(p6 + m_base_point, nv3));
	graph_unit_4.push_back(x3d_vertex(p8 + m_base_point, nv3));
	graph_unit_4.push_back(x3d_vertex(p7 + m_base_point, nv3));
	graph_unit_5.push_back(x3d_vertex(p1 + m_base_point, -nv2));
	graph_unit_5.push_back(x3d_vertex(p4 + m_base_point, -nv2));
	graph_unit_5.push_back(x3d_vertex(p5 + m_base_point, -nv2));
	graph_unit_5.push_back(x3d_vertex(p8 + m_base_point, -nv2));
	graph_unit_6.push_back(x3d_vertex(p3 + m_base_point, -nv1));
	graph_unit_6.push_back(x3d_vertex(p4 + m_base_point, -nv1));
	graph_unit_6.push_back(x3d_vertex(p7 + m_base_point, -nv1));
	graph_unit_6.push_back(x3d_vertex(p8 + m_base_point, -nv1));
}
