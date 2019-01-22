#include "stdafx.h"

#include "x3d_polygon.h"
#include "x3d_action_fuse.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_action_fuse, x3d_action_base, 1)

x3d_action_fuse::x3d_action_fuse()
{
	m_section_begin = nullptr;
	m_section_end = nullptr;
}


x3d_action_fuse::~x3d_action_fuse()
{
	safe_delete(m_section_begin);
	safe_delete(m_section_end);
}

void insert_point(vector<x3d_action_section::point>& pts, size_t last, size_t now, size_t n)
{
	vector<double> len;   // 计算并存储每段长度，用于计算每段需插入多少个点
	double total_len = 0.0;
	for (size_t i = last; i < now; ++i)
	{
		double tmp = pts[i].p.get_distance(pts[i + 1].p);
		len.push_back(tmp);
		total_len += tmp;
	}

	int count = 0, total_count = 0;
	for (size_t j = last; j < now; ++j)
	{
		if (j + 1 != now)
			count = (int)(n * len[j - last] / total_len + 0.5);  // 按每段长度分量来确定需插入几个点
		else
			count = n - total_count;   // 若是最后一处则直接计算总插入点的差值

		const x3d_vector vec = (pts[j + total_count + 1].p - pts[j + total_count].p) * (1 / (double)(count + 1));  // 计算插入每个点的变化向量
		auto it = inserter(pts, pts.begin() + j + total_count + 1);  // 定义一个插入迭代器
		for (int i = 1; i <= count; ++i)
			*it = x3d_action_section::point(pts[j + total_count].p + vec * i);

		total_count += count;
	}
}

 // 求平面p1p2p3与平面p2p3p4的平均法向量
x3d_vector get_norvec_by_fourpoints(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3, const x3d_point& p4)
{
	x3d_vector v1(p1, p2), v2(p1, p3), v3(p2, p4), v4(p2, p3);
	x3d_vector n1 = v1.get_cross_vector(v2).get_unit_vector();
	x3d_vector n2 = v3.get_cross_vector(v4).get_unit_vector();
	x3d_vector nm = n1 + n2;
	return nm.normalize();
}

void generate_begin_end_section_copy(x3d_action_section& beg_sec, x3d_action_section& end_sec)
{
	size_t begin_index = 0, end_index = 0;

	auto it = beg_sec.m_all_points.rbegin();  // 手动给初始及终止面点集结尾加上相同的关键字
	it->key = _T("Thisis_End");
	it = end_sec.m_all_points.rbegin();
	it->key = _T("Thisis_End");

	vector<x3d_action_section::point>& begin_points_copy = beg_sec.m_all_points;
	vector<x3d_action_section::point>& end_points_copy = end_sec.m_all_points;

	for (size_t i = 0; i < begin_points_copy.size(); ++i)
	{
		if (!begin_points_copy[i].key.IsEmpty())
		{
			for (size_t j = end_index; j < end_points_copy.size(); ++j)
			{
				if (end_points_copy[j].key == begin_points_copy[i].key)
				{
					size_t b = i - begin_index;
					size_t e = j - end_index;
					if (b < e)
					{
						insert_point(begin_points_copy, begin_index, i, e - b);
						begin_index += e;
						end_index = j;
						break;
					}
					else if (b > e)
					{
						insert_point(end_points_copy, end_index, j, b - e);
						end_index += b;
						begin_index = i;
						break;
					}
					end_index = j;
					begin_index = i;
					break;
				}
			}
		}
	}
}

void get_surround_graph_by_section(x3d_graph& result, const x3d_action_section& begin_copy,
	const x3d_action_section& end_copy)
{
	const vector<x3d_action_section::point>& begin_points_copy = begin_copy.m_all_points;
	const vector<x3d_action_section::point>& end_points_copy = end_copy.m_all_points;

	if (begin_points_copy.size() != end_points_copy.size()) return;

	// 插入周围的面点集
	auto it_surround = result.m_all_units.insert(make_pair(_T("surround_surface"), x3d_graph_unit()));
	vector<x3d_vertex>& graph_unit = (*it_surround.first).second.m_vertexs;

	// 生成周围的面
	if (begin_copy.m_nm_type == x3d_normal_type::plane)  // 平面法向量方式，面之间区别明显
	{
		// 为保证每个面的向量统一且相互间无影响，把每个面的四个点拆成两个独立的三角形共六个点分别存进去
		(*it_surround.first).second.m_drawing_type = x3d_drawing_type::triangles;
		for (size_t i = 0; i + 1 < end_points_copy.size(); ++i)
		{
			x3d_vector nm = get_norvec_by_fourpoints(begin_points_copy[i].p,
				begin_points_copy[i + 1].p, end_points_copy[i].p, end_points_copy[i + 1].p);
			if (!begin_points_copy[i].is_left) nm = -nm;

			graph_unit.push_back(x3d_vertex(begin_points_copy[i].p, nm));
			graph_unit.push_back(x3d_vertex(end_points_copy[i].p, nm));
			graph_unit.push_back(x3d_vertex(end_points_copy[i + 1].p, nm));
			graph_unit.push_back(x3d_vertex(begin_points_copy[i].p, nm));
			graph_unit.push_back(x3d_vertex(begin_points_copy[i + 1].p, nm));
			graph_unit.push_back(x3d_vertex(end_points_copy[i + 1].p, nm));
		}
	}
	else if (begin_copy.m_nm_type == x3d_normal_type::vertex) // 顶点法向量方式，面之间区别不明显，适用光滑曲面
	{
		(*it_surround.first).second.m_drawing_type = x3d_drawing_type::triangles_trip;
		for (size_t i = 0; i < end_points_copy.size(); ++i)
		{
			x3d_vector nm;
			if (i + 1 < end_points_copy.size())
				nm = get_norvec_by_fourpoints(begin_points_copy[i].p, begin_points_copy[i + 1].p,
				end_points_copy[i].p, end_points_copy[i + 1].p);
			else
				nm = get_norvec_by_fourpoints(begin_points_copy[i].p, begin_points_copy[1].p,
				end_points_copy[i].p, end_points_copy[1].p);

			if (!begin_points_copy[i].is_left) nm = -nm;

			graph_unit.push_back(x3d_vertex(begin_points_copy[i].p, nm));
			graph_unit.push_back(x3d_vertex(end_points_copy[i].p, nm));
		}
	}
	else
	{
		// nothing
	}
}

void x3d_action_fuse::do_action(x3d_graph& result)
{
	if (!m_section_begin || !m_section_end) return;
	size_t begin_index = 0, end_index = 0;

	auto it = m_section_begin->m_all_points.rbegin();  // 手动给初始及终止面点集结尾加上相同的关键字
	it->key = _T("ThisisEnd");
	it = m_section_end->m_all_points.rbegin();
	it->key = _T("ThisisEnd");


	vector<x3d_action_section::point> begin_points_copy = m_section_begin->m_all_points;
	vector<x3d_action_section::point> end_points_copy = m_section_end->m_all_points;

	// 根据点的关键字信息插入点，以保证起始面及终止面的点数相同
	generate_begin_end_section_copy(*m_section_begin, *m_section_end);
	if (m_all_track.empty())     // 如果没有通过回调函数传各点的拉伸轨迹，则直接拉伸
	{
		get_surround_graph_by_section(result, *m_section_begin, *m_section_end);
		return;
	}

	vector<x3d_vector> change_vec;     // 缓存每个点从起点到终点的变换向量
	vector<double>  total_length;      // 缓存每个点变换轨迹的总长，若无轨迹则为0
	for (size_t i = 0; i < m_section_begin->m_all_points.size(); ++i)
	{
		x3d_vector v;
		double l = 0.0;
		if (m_all_track[i])        // 如果输入了轨迹则变换向量计入polygon影响，非线性变换
		{
			deque<x3d_point>& points = m_all_track[i]->m_all_points;
			x3d_vector begin_to_end(points.front(),	points.back());
			v = m_section_end->m_all_points[i].p - begin_to_end - m_section_begin->m_all_points[i].p;

			for (size_t j = 0; j + 1 < points.size(); ++j)
				l += points[j].get_distance(points[j + 1]);
		}
		else                        // 未输入轨迹则按首尾直线线形变换
		{
			v = m_section_end->m_all_points[i].p - m_section_begin->m_all_points[i].p;
			l = 0.0;
		}
		change_vec.push_back(v);
		total_length.push_back(l);
	}

	double line_radio = 1.0 / m_step;

	x3d_action_section last_sec(*m_section_begin);
	x3d_action_section next_sec(last_sec);
	for (size_t i = 0; i < m_step; ++i)
	{
		vector<x3d_vector> change_vec_copy = change_vec;
		for (size_t j = 0; j < next_sec.m_all_points.size(); ++j)
		{
			if (m_all_track[j])        // 如果输入了轨迹则按轨迹变换
			{
				deque<x3d_point>& points = m_all_track[j]->m_all_points;
				x3d_vector tmp_vec(points[i], points[i + 1]);
				double tmp_len = tmp_vec.get_module();
				double radio = tmp_len / total_length[j];
				next_sec.m_all_points[j].p += (tmp_vec + change_vec_copy[j] * radio);
			}
			else                        // 未输入轨迹则按首尾直线线形变换
			{
				next_sec.m_all_points[j].p += change_vec_copy[j] * line_radio;
			}
		}

		get_surround_graph_by_section(result, last_sec, next_sec);
		last_sec = next_sec;
	}
}

void x3d_action_fuse::set_data(x3d_action_section* beg, x3d_action_section* end)
{
	m_section_begin = beg;
	m_section_end = end;
}

void x3d_action_fuse::set_data(x3d_action_section* beg, x3d_action_section* end, size_t step,
	function<x3d_polygon*(size_t)> func)
{
	m_section_begin = beg;
	m_section_end = end;
	m_step = step;

	for (size_t i = 0; i < beg->m_all_points.size(); ++i)
	{
		x3d_polygon* pl = func(i);
		m_all_track.push_back(pl);
	}
}

