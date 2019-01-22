#include "stdafx.h"
#include <assert.h>

#include "../tools/compare_tools.h"

#include "x3d_line.h"
#include "x3d_polygon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_polygon, x3d_geometry_base, 1)

x3d_polygon::x3d_polygon()
{
}

x3d_polygon::~x3d_polygon()
{
}

void x3d_polygon::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_geometry_base::copy(node, copy_tag);

	const x3d_polygon* src_obj = dynamic_cast<const x3d_polygon*>(&node);
	if (!src_obj || src_obj == this) return;

	m_all_points = src_obj->m_all_points;
}

void x3d_polygon::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();

	auto it = graph.m_all_units.insert(make_pair(_T("polygon"), x3d_graph_unit()));
	(*it.first).second.m_drawing_type = x3d_drawing_type::line_strip;
	vector<x3d_vertex>& unit = (*it.first).second.m_vertexs;

	x3d_vector nm(0, 0, 1);
	for (auto x : m_all_points)
		unit.push_back(x3d_vertex(x + m_base_point, nm));
}

bool x3d_polygon::is_empty() const
{
	return m_all_points.size() < 2;
}

double x3d_polygon::get_length() const
{
	double len = 0.0;
	if (is_empty()) return 0.0;

	for (size_t i = 0; i + 1 < m_all_points.size(); ++i)
	{
		double tmp = m_all_points[i].get_distance(m_all_points[i + 1]);
		len += tmp;
	}

	return len;
}

void x3d_polygon::trim_at_pos(double length, bool reserve_front /*= true*/)
{
	if (is_empty()) return;
	double len = 0.0, tmp_len = 0.0;
	size_t i = 0;
	for (; (i != m_all_points.size() - 1) && (len < length); ++i)
	{
		double tmp_len = m_all_points[i].get_distance(m_all_points[i + 1]);
		len += tmp_len;
	}

	if (compare_tools::is_less(len, length)) return;    // 总长小于给定的长度，则直接返回

	double remain = length - len + tmp_len;
	double n = remain / tmp_len;
	x3d_line(m_all_points[i], m_all_points[i + 1]).self_scale(n);

	auto it = m_all_points.begin() + i;
	if (!reserve_front)
	{
		m_all_points.erase(it, m_all_points.end());
		return;
	}
	m_all_points.erase(m_all_points.begin(), it);
}

void x3d_polygon::extend_length(double ext_len, bool ext_front /*= false*/)
{
	
	size_t n = m_all_points.size();
	if (n < 2) return;

	if (!ext_front)
	{
		x3d_line end_line(m_all_points[n - 1], m_all_points[n - 2]);
		double ex = (end_line.get_length() + ext_len) / end_line.get_length();
		x3d_point new_point = end_line.scale(ex);
		m_all_points.push_back(new_point);
	}

	x3d_line end_line(m_all_points[1], m_all_points[0]);
	double ex = (end_line.get_length() + ext_len) / end_line.get_length();
	x3d_point new_point = end_line.scale(ex);
	m_all_points.push_front(new_point);
}

void x3d_polygon::clear_points()
{
	m_all_points.clear();
}

x3d_point x3d_polygon::get_point(size_t index) const
{
	return m_all_points[index];
}

void x3d_polygon::add_point(x3d_point new_point, bool to_front /*= false*/)
{
	if (to_front)
		m_all_points.push_front(new_point);
	else
		m_all_points.push_back(new_point);
}

void x3d_polygon::add_points(std::vector<x3d_point>& points, bool ext_front /*= false*/)
{
	if (ext_front)
		for (auto it = points.crend(); it != points.crbegin(); ++it)
			m_all_points.push_front(*it);
	else
		for (auto& x : points)
			m_all_points.push_back(x);
}

void x3d_polygon::insert_point(double length)
{

}

void x3d_polygon::add_line(x3d_vector vec, double length, bool to_front /*= false*/)
{
	if (vec.is_all_zero()) return;

	if (!to_front)
	{
		x3d_point last_point = m_all_points.back();
		x3d_point new_point = last_point.get_point_by_vector(vec, length);
		m_all_points.push_back(new_point);
		return;
	}

	x3d_point last_point = m_all_points.front();
	x3d_point new_point = last_point.get_point_by_vector(vec, length);
	m_all_points.push_front(new_point);
}

void x3d_polygon::connect_polygon(x3d_polygon& another, bool to_front /*= false*/)
{
	if (another.is_empty()) return;

	if (!to_front)
	{
		m_all_points.assign(another.m_all_points.begin(), another.m_all_points.end());
		return;
	}

	m_all_points.insert(m_all_points.begin(), another.m_all_points.begin(), another.m_all_points.end());
}


bool x3d_polygon::get_all_intersection(std::vector<std::pair<x3d_point, x3d_intersection_type>>& points, x3d_line& l, x3d_extend_type et /*= x3d_extend_type::none*/, double precision /*= 1e-6*/)
{
	if (is_empty()) return false;

	points.clear();
	for (size_t i = 0; i + 1 < m_all_points.size(); ++i)
	{
		x3d_line tmp_line(m_all_points[i], m_all_points[i + 1]);
		pair<x3d_point, x3d_intersection_type> point;
		if (tmp_line.is_cross(l, point, et, precision))
			points.push_back(point);
	}
	if (points.empty()) return false;
	return true;
}

bool x3d_polygon::get_first_intersection(x3d_point& p, x3d_line& l, x3d_extend_type et /*= x3d_extend_type::none*/, double precision /*= 1e-6*/)
{
	if (is_empty()) return false;

	for (size_t i = 0; i + 1 < m_all_points.size(); ++i)
	{
		x3d_line tmp_line(m_all_points[i], m_all_points[i + 1]);
		pair<x3d_point, x3d_intersection_type> point;
		if (tmp_line.is_cross(l, point, et, precision))
		{
			p = point.first;
			return true;
		}
	}
	return false;
}

bool x3d_polygon::get_last_intersection(x3d_point& p, x3d_line& l, x3d_extend_type et /*= x3d_extend_type::none*/, double precision /*= 1e-6*/)
{
	if (is_empty()) return false;

	for (size_t i = m_all_points.size(); i != 0; --i)
	{
		x3d_line tmp_line(m_all_points[i], m_all_points[i - 1]);
		pair<x3d_point, x3d_intersection_type> point;
		if (tmp_line.is_cross(l, point, et, precision))
		{
			p = point.first;
			return true;
		}
	}
	return false;
}

bool x3d_polygon::get_nearest_intersection(x3d_point& p, x3d_line& l, x3d_extend_type et /*= x3d_extend_type::none*/, double precision /*= 1e-6*/)
{
	if (is_empty()) return false;

	typedef pair<x3d_point, x3d_intersection_type> inter_result;
	vector<inter_result> points;
	for (size_t i = 0; i + 1 != m_all_points.size(); ++i)
	{
		x3d_line tmp_line(m_all_points[i], m_all_points[i + 1]);
		inter_result point;
		if (tmp_line.is_cross(l, point, et, precision))
		{
			p = point.first;
			if (point.second == x3d_intersection_type::self) return true;  // 如果找到交点在线段上的则直接返回结果
			points.push_back(point);                                       // 交点在线段延长线上，则存入vector
		}
	}

	if (points.empty()) return false;

	sort(points.begin(), points.end(), [&](inter_result& p1, inter_result& p2)    // 把所有交点按到线段的距离排序
	{
		double mlen1 = min(p1.first.get_distance_square(l.m_s), p1.first.get_distance_square(l.m_e));
		double mlen2 = min(p2.first.get_distance_square(l.m_s), p2.first.get_distance_square(l.m_e));
		return compare_tools::is_less(mlen1, mlen2);
	});

	p = points[0].first;
	return true;
}

bool x3d_polygon::is_in_line(x3d_point& p, double precision /*= 1e-6*/)
{
	if (is_empty()) return false;

	for (size_t i = 0; i < m_all_points.size() - 1; ++i)
	{
		x3d_line tmp_line(m_all_points[i], m_all_points[i + 1]);
		if (tmp_line.relation_with_point(p) == x3d_intersection_type::self) return true;
	}
	return false;
}

bool x3d_polygon::get_point_at_pos(x3d_point& pt, double dist)
{
	if (is_empty()) return false;
	double len = 0.0, tmp_len = 0.0;
	size_t i = 0;
	for (; (i < m_all_points.size() - 1) && (len < dist); ++i)
	{
		double tmp_len = m_all_points[i].get_distance(m_all_points[i + 1]);
		len += tmp_len;
	}

	if (compare_tools::is_less(len, dist)) return false;    // 总长小于给定的长度，则直接返回

	double remain = dist - len + tmp_len;
	double n = remain / tmp_len;
	x3d_line last_line(m_all_points[i], m_all_points[i + 1]);
	pt = last_line.scale(n);
	return true;
}

x3d_polygon* x3d_polygon::offset(double dist) const
{
	x3d_polygon* pl = new x3d_polygon();
	*pl = *this;
	pl->self_offset(dist);
	return pl;
} 

x3d_polygon& x3d_polygon::self_offset(double dist)
{
	if (m_all_points.size() < 2) return *this;

	x3d_line pre_offline;
	for (size_t i = 0; i + 1 < m_all_points.size(); ++i)
	{
		x3d_line tmp_offline(m_all_points[i], m_all_points[i + 1]);
		tmp_offline.self_offset(dist);
		if (i == 0)
		{
			m_all_points[0] = tmp_offline.m_s;
			pre_offline = tmp_offline;
			continue;
		}

		x3d_point cross_point;
		pre_offline.get_cross_point(tmp_offline, cross_point);

		m_all_points[i] = cross_point;

		//double dis = 0.0;
		//if(i > 1) dis = cross_point.get_distance(m_all_points[i - 1]);
		//if (dis >= 10000.0)
		//{
		//	assert(false);
		//}

		pre_offline = tmp_offline;
	}

	m_all_points.back() = pre_offline.m_e;
	return *this;
} 

void x3d_polygon::transfer_from(x3d_graph& src)
{
	for (auto& x : src.m_all_units)
		for (auto& y : x.second.m_vertexs)
			m_all_points.push_back(y.m_point);
}

void x3d_polygon::transfer_to_section(x3d_action_section& sec)
{
	for (auto& x : m_all_points)
		sec.m_all_points.push_back(x3d_action_section::point(x));
}

