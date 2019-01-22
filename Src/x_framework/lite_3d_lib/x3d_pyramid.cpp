#include "stdafx.h"

#include "../tools/compare_tools.h"

#include "x3d_pyramid.h"
#include "x3d_line.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_pyramid, x3d_geometry_base, 1)

x3d_pyramid::x3d_pyramid()
{
    m_sides = 0;
    m_radius = 0;
    m_height = 0;
    m_normal_type = x3d_normal_type::plane;
}

x3d_pyramid::x3d_pyramid(int s, double r, double h)
{
    set_data(s, r, h);
}

x3d_pyramid::x3d_pyramid(const x3d_point& p, int s, double r, double h)
{
    set_data(p, s, r, h);
}


x3d_pyramid::~x3d_pyramid()
{
}

void x3d_pyramid::set_data(int s, double r, double h)
{
    m_sides = s;
    m_radius = r;
    m_height = h;
    m_normal_type = x3d_normal_type::plane;
}

void x3d_pyramid::set_data(const x3d_point& p, int s, double r, double h)
{
	m_base_point = p;
	m_sides = s;
	m_radius = r;
	m_height = h;
    m_normal_type = x3d_normal_type::plane;
}

void x3d_pyramid::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
    x3d_geometry_base::copy(node, copy_tag);
    const x3d_pyramid* src_obj = dynamic_cast<const x3d_pyramid*>(&node);
    if (!src_obj) return;

    m_sides = src_obj->m_sides;
    m_radius = src_obj->m_radius;
    m_height = src_obj->m_height;
}

void x3d_pyramid::regenerate_vertext(x3d_graph& graph)
{
    graph.m_all_units.clear();
    if (m_sides < 3) return;
    if (compare_tools::is_less_equal(m_radius, 0.0)) return;
    if (compare_tools::is_less_equal(m_height, 0.0)) return;

    double angle_step = 2.0 * M_PI / m_sides;
    vector<double> round_angle_value;
    for (double ai = 0.0; compare_tools::is_less(ai, 2.0 * M_PI); ai += angle_step)
        round_angle_value.push_back(ai);

    // 底面顶点计算
    vector<x3d_point> bottom_points;
    x3d_point bottom_center(m_base_point);
    for (auto& x : round_angle_value)
        bottom_points.push_back(x3d_point(m_base_point.m_x + m_radius * sin(x),
        m_base_point.m_y + m_radius*cos(x), m_base_point.m_z));
    
    // 底面首尾相连
    bottom_points.push_back(bottom_points.front());

    // 底面正多边形
    auto bottom_it = graph.m_all_units.insert(make_pair(_T("bottom_surface"), x3d_graph_unit()));
    (*bottom_it.first).second.m_drawing_type = x3d_drawing_type::triangles_fan;
    vector<x3d_vertex>& bottom_graph_unit = (*bottom_it.first).second.m_vertexs;
    bottom_graph_unit.push_back(x3d_vertex(bottom_center, x3d_vector(0.0, 0.0, -1.0)));
    for (auto& x : bottom_points)
        bottom_graph_unit.push_back(x3d_vertex(x, x3d_vector(0.0, 0.0, -1.0)));

    // 侧面三角面
    auto side_it = graph.m_all_units.insert(make_pair(_T("side_surface"), x3d_graph_unit()));
    if (m_normal_type==x3d_normal_type::plane)
    {
    
    (*side_it.first).second.m_drawing_type = x3d_drawing_type::triangles;
    vector<x3d_vertex>& side_graph_unit = (*side_it.first).second.m_vertexs;


    x3d_point top_pt(m_base_point.m_x, m_base_point.m_y, m_base_point.m_z + m_height);
    for (size_t i = 0; i + 1 < bottom_points.size(); ++i)
    {
        size_t cur_index = i;
        size_t next_index = i + 1;
        
        //根据向量积右手定则，第二个参数和第三个参数不能调换
        x3d_plane pn(top_pt, bottom_points[next_index], bottom_points[cur_index]);
        x3d_vector nm = pn.get_normal_vector();

        side_graph_unit.push_back(x3d_vertex(top_pt, nm));
        side_graph_unit.push_back(x3d_vertex(bottom_points[cur_index], nm));
        side_graph_unit.push_back(x3d_vertex(bottom_points[next_index], nm));
    }
    }
    else if (m_normal_type == x3d_normal_type::vertex)
    {
        (*side_it.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
        vector<x3d_vertex>& side_graph_unit = (*side_it.first).second.m_vertexs;

        x3d_point top_pt(m_base_point.m_x, m_base_point.m_y, m_base_point.m_z + m_height);
        for (size_t i = 0; i  < bottom_points.size(); ++i)
        {
            x3d_point top_pt(m_base_point.m_x, m_base_point.m_y, m_base_point.m_z + m_height);

            x3d_line line(bottom_points[i], top_pt);
            x3d_point normal_point = line.get_foot_point(bottom_center);
            x3d_vector nm = (normal_point - bottom_center).get_unit_vector();

            side_graph_unit.push_back(x3d_vertex(bottom_points[i], nm));
            side_graph_unit.push_back(x3d_vertex(top_pt, nm));
        }
    }
    else
    {
        return;
    }
}

void x3d_pyramid::set_normal_type(x3d_normal_type type)
{
    m_normal_type = type;
}


