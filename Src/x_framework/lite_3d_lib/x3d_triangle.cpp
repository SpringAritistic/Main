#include "stdafx.h"
#include "x3d_triangle.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_triangle, x3d_geometry_base, 1)

x3d_triangle::x3d_triangle()
{
	m_a = x3d_point(0,0,0);
	m_b = x3d_point(50, 150, -100);
	m_c = x3d_point(100, 200, 300);
}


x3d_triangle::x3d_triangle(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3)
{
	m_a = p1;
	m_b = p2;
	m_c = p3;
}

x3d_triangle::~x3d_triangle()
{
}

void x3d_triangle::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);
	const x3d_triangle* src_obj = dynamic_cast<const x3d_triangle*>(&node);
	if (!src_obj) return;

	m_a = src_obj->m_a;
	m_b = src_obj->m_b;
	m_c = src_obj->m_c;
}

void x3d_triangle::regenerate_vertext(x3d_graph& graph)
{
	x3d_graph_unit unit;
	unit.m_drawing_type = x3d_drawing_type::line_loop;
	x3d_vector nm(0, 0, 1);

	unit.m_vertexs.push_back(x3d_vertex(m_a + m_base_point, nm));
	unit.m_vertexs.push_back(x3d_vertex(m_b + m_base_point, nm));
	unit.m_vertexs.push_back(x3d_vertex(m_c + m_base_point, nm));

	graph.m_all_units[_T("unit")] = unit;

}
