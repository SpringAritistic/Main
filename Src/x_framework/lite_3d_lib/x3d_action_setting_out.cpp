#include "stdafx.h"

#include "../tools/compare_tools.h"
#include "../tools/math_tools.h"

#include "x3d_action_setting_out.h"
#include "x3d_polygon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(x3d_action_setting_out, x3d_action_base, 1)

x3d_action_setting_out::x3d_action_setting_out()
{
}


x3d_action_setting_out::~x3d_action_setting_out()
{
}

void insert_points(vector<x3d_action_section::point>& pts, size_t last, size_t now, size_t n)
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
x3d_vector get_normalvec_by_fourpoints(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3, const x3d_point& p4)
{
	x3d_vector v1(p1, p2), v2(p1, p3), v3(p2, p4), v4(p2, p3);
	x3d_vector n1 = v1.get_cross_vector(v2).get_unit_vector();
	x3d_vector n2 = v3.get_cross_vector(v4).get_unit_vector();
	x3d_vector nm = n1 + n2;
	return nm.normalize();
}

void generate_begin_end_points_copy(x3d_action_section& beg_sec, x3d_action_section& end_sec)
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
						insert_points(begin_points_copy, begin_index, i, e - b);
						begin_index += e;
						end_index = j;
						break;
					}
					else if (b > e)
					{
						insert_points(end_points_copy, end_index, j, b - e);
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

// 根据起始面及终止面的点集生成周围的面
void get_surround_graph_by_points(x3d_graph& result, const x3d_action_section& begin_copy, 
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
			x3d_vector nm = get_normalvec_by_fourpoints(begin_points_copy[i].p,
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
				nm = get_normalvec_by_fourpoints(begin_points_copy[i].p, begin_points_copy[i + 1].p,
				end_points_copy[i].p, end_points_copy[i + 1].p);
			else
				nm = get_normalvec_by_fourpoints(begin_points_copy[i].p, begin_points_copy[1].p,
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


void x3d_action_setting_out::do_action(x3d_graph& result)
{
	vector<double> length_radio;
	double total_path_length = 0.0;
	for (size_t i = 0; i + 1 < m_path.size(); ++i)
	{
		double tmp = m_path[i].get_distance(m_path[i + 1]);
		total_path_length += tmp;
	}

	x3d_action_section sec_begin_copy = *m_path_section[0].second;
	x3d_action_section sec_end_copy = *m_path_section[1].second;
	generate_begin_end_points_copy(sec_begin_copy, sec_end_copy);

	vector<x3d_vector> change_vec;
	size_t point_count = m_path.size();
	x3d_vector begin_to_end(m_path[0], m_path[point_count - 1]);
	for (size_t i = 0; i < sec_begin_copy.m_all_points.size(); ++i)
	{
		x3d_vector v = sec_end_copy.m_all_points[i].p - begin_to_end - sec_begin_copy.m_all_points[i].p;
		change_vec.push_back(v);
	}

	x3d_action_section last_sec(sec_begin_copy);
	x3d_action_section next_sec(last_sec);
	for (size_t i = 0; i + 1 < m_path.size(); ++i)
	{
		x3d_vector tmp_vec(m_path[i], m_path[i + 1]);
		double tmp_len = tmp_vec.get_module();
		double radio = tmp_len / total_path_length;
		vector<x3d_vector> change_vec_copy = change_vec;

		for (size_t j = 0; j < next_sec.m_all_points.size(); ++j)
			next_sec.m_all_points[j].p += (tmp_vec + change_vec_copy[j] * radio);

		get_surround_graph_by_points(result, last_sec, next_sec);
		last_sec = next_sec;
	}
}






