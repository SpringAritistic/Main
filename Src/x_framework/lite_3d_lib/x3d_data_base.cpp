#include "stdafx.h"

#include "..\tools\compare_tools.h"
#include "..\tools\math_tools.h"
#include "x3d_line.h"

#include "x3d_data_base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;



IMPLEMENT_SERIAL(x3d_node_base, data_node_base, 1)

x3d_node_base::x3d_node_base()
{
	m_is_hidden = false;
	m_is_graph_group = false;
}

x3d_node_base::~x3d_node_base()
{
}

void x3d_node_base::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	data_node_base::copy(node, copy_tag);

	const x3d_node_base* src_obj = dynamic_cast<const x3d_node_base*>(&node);
	if (!src_obj) return;

	m_is_hidden = src_obj->m_is_hidden;
	m_is_graph_group = src_obj->m_is_graph_group;
}


void x3d_node_base::hide(bool is_hidden /*= false*/)
{
	m_is_hidden = is_hidden;
}

bool x3d_node_base::is_hidden() const
{
	return m_is_hidden;
}

bool x3d_node_base::is_graph_group() const
{
	return m_is_graph_group;
}


IMPLEMENT_SERIAL(x3d_node_group, x3d_node_base, 1)
x3d_node_group::x3d_node_group()
{
	m_is_graph_group = true;
}

x3d_node_group::~x3d_node_group()
{

}

void x3d_node_group::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_node_base::copy(node, copy_tag);

	const x3d_node_group* src_obj = dynamic_cast<const x3d_node_group*>(&node);
	if (!src_obj) return;
}

x3d_graph* x3d_node_group::add_child_graph(x3d_geometry_base& gp)
{
	x3d_graph* child = new x3d_graph();
	child->m_all_units.clear();
	gp.regenerate_vertext(*child);
	add_child_node(child);
	return child;
}

x3d_graph* x3d_node_group::add_child_graph(x3d_graph* gp)
{
	if (gp)
	{
		add_child_node(gp);
		return gp;
	}
	else
	{
		return nullptr;
	}
}

void x3d_node_group::set_color(x3d_color cl)
{
	travel_current_node_tree([&](data_node_base* cur)
	{
		if (!cur || cur == this) return false; // group本身不带图元，图元都打包在子节点中
		x3d_graph* gp = dynamic_cast<x3d_graph*>(cur);
		if (gp) gp->set_color(cl, false);

		return false;
	});
}

void x3d_node_group::enable_light(bool enable /*= true*/)
{
	m_enable_light = enable;
}

bool x3d_node_group::is_light_enabled() const
{
	return m_enable_light;
}

void x3d_node_group::enable_depth_test(bool enable /*= true*/)
{
	m_enable_depth_test = enable;
}

bool x3d_node_group::is_depth_test_enabled() const
{
	return m_enable_depth_test;
}

bool x3d_node_group::calc_bound_box(x3d_aabb_box& box, bool only_self /*= false*/) const
{
	bool result_is_init = false;
	travel_current_node_tree([&](const data_node_base* cur)
	{
		const x3d_graph* gp = dynamic_cast<const x3d_graph*>(cur);
		if (!gp) return false;

		if (gp->is_graph_group()) return false; // group本身不带图元，图元都打包在子节点中

		x3d_aabb_box nd_box;
		if (gp->calc_bound_box(nd_box, true)) // travel_current_node_tree本身就是递归遍历了，不要再计算子图元
		{
			box.include_box(nd_box);
			result_is_init = true;
		}

		return false;
	});

	return result_is_init;
}

x3d_aabb_box::x3d_aabb_box()
{
}

x3d_aabb_box::x3d_aabb_box(const x3d_point& min_pt, const x3d_point& max_pt)
{
	set_data(min_pt, max_pt);
}

x3d_aabb_box::~x3d_aabb_box()
{
}

void x3d_aabb_box::set_min_pt(const x3d_point& pt)
{
	m_min_pt = pt;
	check_data();
}

void x3d_aabb_box::set_max_pt(const x3d_point& pt)
{
	m_max_pt = pt;
	check_data();
}

void x3d_aabb_box::set_data(const x3d_point& min_pt, const x3d_point& max_pt)
{
	m_min_pt = min_pt;
	m_max_pt = max_pt;
	check_data();
}

x3d_point x3d_aabb_box::get_min_pt() const
{
	return m_min_pt;
}

x3d_point x3d_aabb_box::get_max_pt() const
{
	return m_max_pt;
}

double x3d_aabb_box::calc_radius() const
{
	return 0.0;
}

void x3d_aabb_box::include_point(const x3d_point& pt)
{
	// 已经确保不会违反aabb包围盒的坐标要求，因此不用再调用check_data了
	m_min_pt.m_x = min(m_min_pt.m_x, pt.m_x);
	m_min_pt.m_y = min(m_min_pt.m_y, pt.m_y);
	m_min_pt.m_z = min(m_min_pt.m_z, pt.m_z);

	m_max_pt.m_x = max(m_max_pt.m_x, pt.m_x);
	m_max_pt.m_y = max(m_max_pt.m_y, pt.m_y);
	m_max_pt.m_z = max(m_max_pt.m_z, pt.m_z);
}

void x3d_aabb_box::include_box(const x3d_aabb_box& box)
{
	// 内部会确保不会违反aabb包围盒的坐标要求，因此不用再调用check_data了
	include_point(box.get_min_pt());
	include_point(box.get_max_pt());
}

void x3d_aabb_box::include_graph(const x3d_graph& gp)
{
	x3d_aabb_box gp_box;
	if (gp.calc_bound_box(gp_box))
		include_box(gp_box);
}

void x3d_aabb_box::exclude_point(const x3d_point& pt, bool reserve_min_part/* = true*/)
{
	if (!is_point_in_box(pt)) return;

	// 一个点会把包围盒划分为8个部分，单一个保留小的部分无法适应所有情况，有问题，得重新梳理？？？？？？？？？？？？？？？？？？？
	if (reserve_min_part)
		m_max_pt = pt;
	else
		m_min_pt = pt;

	check_data();
}

void x3d_aabb_box::exclude_box(const x3d_aabb_box& box, bool reserve_min_part/* = true*/)
{
	// 判断包围盒是否完全不在本包围盒内
	if (!is_point_in_box(box.get_min_pt()) && !is_point_in_box(box.get_max_pt())) return;

	// 判断传入的包围盒是否完全在本包围盒内
}

void x3d_aabb_box::exclude_graph(const x3d_graph& gp, bool reserve_min_part/* = true*/)
{
	x3d_aabb_box box;
	if (!gp.calc_bound_box(box, false)) return;

	exclude_box(box, reserve_min_part);
}

bool x3d_aabb_box::is_point_in_box(const x3d_point& pt) const
{
	if (compare_tools::is_less(pt.m_x, m_min_pt.m_x)) return false;
	if (compare_tools::is_great(pt.m_x, m_max_pt.m_x)) return false;

	if (compare_tools::is_less(pt.m_y, m_min_pt.m_y)) return false;
	if (compare_tools::is_great(pt.m_y, m_max_pt.m_y)) return false;

	if (compare_tools::is_less(pt.m_z, m_min_pt.m_z)) return false;
	if (compare_tools::is_great(pt.m_z, m_max_pt.m_z)) return false;

	return true;
}

void x3d_aabb_box::check_data()
{
	x3d_point min, max;

	// 不可以边检查边改
	min.m_x = min(m_min_pt.m_x, m_max_pt.m_x);
	min.m_y = min(m_min_pt.m_y, m_max_pt.m_y);
	min.m_z = min(m_min_pt.m_z, m_max_pt.m_z);

	max.m_x = max(m_min_pt.m_x, m_max_pt.m_x);
	max.m_y = max(m_min_pt.m_y, m_max_pt.m_y);
	max.m_z = max(m_min_pt.m_z, m_max_pt.m_z);

	m_min_pt = min;
	m_max_pt = max;
}

x3d_graph_unit::x3d_graph_unit(x3d_drawing_type dt /*= x3d_drawing_type::none*/)
{
	m_drawing_type = dt;
}

x3d_graph_unit::~x3d_graph_unit()
{

}

void x3d_graph_unit::set_color(x3d_color cl)
{
	for (auto& x : m_vertexs)
		x.m_color = cl;
}



IMPLEMENT_SERIAL(x3d_graph, x3d_node_base, 1)
x3d_graph::x3d_graph()
{
}

x3d_graph::~x3d_graph()
{
}

x3d_graph::x3d_graph(x3d_graph& src, bool copy_tag /*= false*/)
{
	copy(src, copy_tag);
}

x3d_graph::x3d_graph(x3d_geometry_base& src)
{
	convert_from(src);
}

void x3d_graph::copy(const data_node_base& node, bool copy_tag /*= false*/)
{
	x3d_node_base::copy(node, copy_tag);

	const x3d_graph* src_obj = dynamic_cast<const x3d_graph*>(&node);
	if (!src_obj) return;
	
	m_all_units = src_obj->m_all_units;
	
	m_enable_light = src_obj->m_enable_light;
	m_enable_depth_test = src_obj->m_enable_depth_test;
}

void x3d_graph::set_color(x3d_color cl, bool only_self/* = false*/)
{
	auto fun_set_single_node_color = [](const x3d_color& color, x3d_graph* nd)
	{
		if (!nd) return;
		for (auto& x : nd->m_all_units)
			x.second.set_color(color);
	};

	if (only_self)
	{
		fun_set_single_node_color(cl, this);
		return;
	}

	travel_current_node_tree([&](data_node_base* cur)
	{
		fun_set_single_node_color(cl, dynamic_cast<x3d_graph*>(cur));
		return false;
	});
}

void x3d_graph::enable_light(bool enable/* = true*/)
{
	m_enable_light = enable;
}

bool x3d_graph::is_light_enabled() const
{
	return m_enable_light;
}

void x3d_graph::enable_depth_test(bool enable /*= true*/)
{
	m_enable_depth_test = enable;
}

bool x3d_graph::is_depth_test_enabled() const
{
	return m_enable_depth_test;
}

bool x3d_graph::calc_bound_box(x3d_aabb_box& box, bool only_self /*= false*/) const
{
	auto fun_calc_single_node_box = [](x3d_aabb_box& nd_box, const x3d_graph* nd)
	{
		x3d_point min_pt, max_pt;
		bool is_init = false;

		if (!nd) return false;
		for (auto& x : nd->m_all_units)
		{
			for (const x3d_vertex& y : x.second.m_vertexs)
			{
				if (!is_init)
				{
					min_pt = y.m_point;
					max_pt = y.m_point;
					is_init = true;
				}
				else
				{
					min_pt.m_x = min(min_pt.m_x, y.m_point.m_x);
					min_pt.m_y = min(min_pt.m_y, y.m_point.m_y);
					min_pt.m_z = min(min_pt.m_z, y.m_point.m_z);

					max_pt.m_x = max(max_pt.m_x, y.m_point.m_x);
					max_pt.m_y = max(max_pt.m_y, y.m_point.m_y);
					max_pt.m_z = max(max_pt.m_z, y.m_point.m_z);
				}
			}
		}

		nd_box.set_data(min_pt, max_pt);
		return is_init;
	};

	if (only_self) return fun_calc_single_node_box(box, this);

	bool result_is_init = false;
	travel_current_node_tree([&](const data_node_base* cur)
	{
		x3d_aabb_box nd_box;

		// 无法计算时不得将默认的无效包围盒合并到结果中，以免造成结果突变导致范围变大，比
		// 如把（0,0,0）点给纳入到包围盒中等情况
		if (fun_calc_single_node_box(nd_box, dynamic_cast<const x3d_graph*>(cur)))
		{
			result_is_init = true;
			box.include_box(nd_box);
		}

		return false;
	});

	return result_is_init;
}

void x3d_graph::convert_from(x3d_geometry_base& gp)
{
	m_all_units.clear();
	gp.regenerate_vertext(*this);
}

x3d_graph* x3d_graph::add_child_graph(x3d_geometry_base& gp)
{
	x3d_graph* child = new x3d_graph();
	child->m_all_units.clear();
	gp.regenerate_vertext(*child);
	add_child_node(child);
	return child;
}

x3d_graph* x3d_graph::add_child_graph(x3d_graph* gp)
{
	if (gp)
	{
		add_child_node(gp);
		return gp;
	}
	else
	{
		return nullptr;
	}
}


void x3d_graph::apply_trans_matrix(const x3d_trans_matrix& mat)
{
    travel_current_node_tree([&](data_node_base* cur_item)
    {
        x3d_graph* graph = dynamic_cast<x3d_graph*>(cur_item);

        for (auto& x : graph->m_all_units)
            for (auto& y : x.second.m_vertexs)
                mat.transform_vertex_self(y.m_point);

        return false;
    });
}


void x3d_graph::rotate(const x3d_vector& vec, double a)
{
	x3d_trans_matrix mx;
	mx.rotate(vec, a);
	apply_trans_matrix(mx);
}

void x3d_graph::rotate(const x3d_point& p1, const x3d_point& p2, double a)
{
	x3d_trans_matrix mx;

	mx.rotate(p1, p2, a);
	apply_trans_matrix(mx);
}

void x3d_graph::move(double dx, double dy, double dz)
{
	x3d_trans_matrix mx;

	mx.move(dx, dy, dz);
	apply_trans_matrix(mx);
}

void x3d_graph::scale(double sx, double sy, double sz)
{
	x3d_trans_matrix mx;

	mx.scale(sx, sy, sz);
	apply_trans_matrix(mx);
}

void x3d_graph::scale(const x3d_vector& vec, double s)
{
	x3d_trans_matrix mx;

	mx.scale(vec, s);
	apply_trans_matrix(mx);
}

void x3d_graph::import_3ds(const CString& path_name)
{

}

void x3d_graph::export_3ds(const CString& path_anme)
{

}

void x3d_graph::import_stl(const CString& path_anme)
{

}

void x3d_graph::export_stl(const CString& path_anme)
{

}



x3d_trans_matrix::x3d_trans_matrix(const x3d_trans_matrix& src)
{
	m_00 = src.m_00, m_01 = src.m_01, m_02 = src.m_02, m_03 = src.m_03;
	m_10 = src.m_10, m_11 = src.m_11, m_12 = src.m_12, m_13 = src.m_13;
	m_20 = src.m_20, m_21 = src.m_21, m_22 = src.m_22, m_23 = src.m_23;
	m_30 = src.m_30, m_31 = src.m_31, m_32 = src.m_32, m_33 = src.m_33;
}

x3d_trans_matrix::x3d_trans_matrix(bool is_identity /*= true*/)
{
	initialize(is_identity);
}

x3d_trans_matrix::~x3d_trans_matrix()
{
}

void x3d_trans_matrix::normalize()
{
	initialize(true);
}

void x3d_trans_matrix::initialize(bool is_identity /*= true*/)
{
	// 单位矩阵更常用
	m_00 = 1.0; m_01 = 0.0; m_02 = 0.0; m_03 = 0.0;
	m_10 = 0.0; m_11 = 1.0; m_12 = 0.0; m_13 = 0.0;
	m_20 = 0.0; m_21 = 0.0; m_22 = 1.0; m_23 = 0.0;
	m_30 = 0.0; m_31 = 0.0; m_32 = 0.0; m_33 = 1.0;

	if (!is_identity) m_00 = m_11 = m_22 = m_33 = 0.0;
}

void x3d_trans_matrix::rotate(const x3d_vector& vec, double a)
{
	a = math_tools::normalize_angle(a);
	if (compare_tools::is_equal(a, 0.0) || vec.is_all_zero()) return;

	double sin_a = sin(a);
	double cos_a = cos(a);

	x3d_trans_matrix mx;
	mx.m_00 = vec.m_x * vec.m_x * (1.0 - cos_a) + cos_a;
	mx.m_01 = vec.m_x * vec.m_y * (1.0 - cos_a) + vec.m_z * sin_a;
	mx.m_02 = vec.m_x * vec.m_z * (1.0 - cos_a) - vec.m_y * sin_a;
	mx.m_03 = 0.0;

	mx.m_10 = vec.m_x * vec.m_y * (1.0 - cos_a) - vec.m_z * sin_a;
	mx.m_11 = vec.m_y * vec.m_y * (1.0 - cos_a) + cos_a;
	mx.m_12 = vec.m_y * vec.m_z * (1.0 - cos_a) + vec.m_x * sin_a;
	mx.m_13 = 0.0;

	mx.m_20 = vec.m_x * vec.m_z * (1.0 - cos_a) + vec.m_y * sin_a;
	mx.m_21 = vec.m_y * vec.m_z * (1.0 - cos_a) - vec.m_x * sin_a;
	mx.m_22 = vec.m_z * vec.m_z * (1.0 - cos_a) + cos_a;
	mx.m_23 = 0.0;

	mx.m_30 = 0.0;
	mx.m_31 = 0.0;
	mx.m_32 = 0.0;
	mx.m_33 = 1.0;

	*this = mx * (*this);
}

void x3d_trans_matrix::rotate(const x3d_point& p1, const x3d_point& p2, double a)
{
	a = math_tools::normalize_angle(a);
	x3d_vector v = (p2 - p1).get_unit_vector();
	if (compare_tools::is_equal(a, 0.0) || v.is_all_zero()) return;

	double sin_a = sin(a);
	double cos_a = cos(a);
	 
	x3d_trans_matrix mx;
	mx.m_00 = v.m_x * v.m_x + (v.m_y * v.m_y + v.m_z * v.m_z) * cos_a;
	mx.m_01 = v.m_x * v.m_y * (1.0 - cos_a) + v.m_z * sin_a;
	mx.m_02 = v.m_x * v.m_z * (1.0 - cos_a) - v.m_y * sin_a;
	mx.m_03 = 0.0;

	mx.m_10 = v.m_x * v.m_y * (1.0 - cos_a) - v.m_z * sin_a;
	mx.m_11 = v.m_y * v.m_y + (v.m_x * v.m_x + v.m_z * v.m_z) * cos_a;
	mx.m_12 = v.m_y * v.m_z * (1.0 - cos_a) + v.m_x * sin_a;
	mx.m_13 = 0.0;

	mx.m_20 = v.m_x * v.m_z * (1.0 - cos_a) + v.m_y * sin_a;
	mx.m_21 = v.m_y * v.m_z * (1.0 - cos_a) - v.m_x * sin_a;
	mx.m_22 = v.m_z * v.m_z + (v.m_y * v.m_y + v.m_x * v.m_x) * cos_a;
	mx.m_23 = 0.0;

	mx.m_30 = (p1.m_x * (v.m_y * v.m_y + v.m_z * v.m_z) - v.m_x * (p1.m_y * v.m_y + p1.m_z * v.m_z)) * (1.0 - cos_a) + (p1.m_y * v.m_z - p1.m_z * v.m_y) * sin_a;
	mx.m_31 = (p1.m_y * (v.m_x * v.m_x + v.m_z * v.m_z) - v.m_y * (p1.m_x * v.m_x + p1.m_z * v.m_z)) * (1.0 - cos_a) + (p1.m_z * v.m_x - p1.m_x * v.m_z) * sin_a;
	mx.m_32 = (p1.m_z * (v.m_y * v.m_y + v.m_x * v.m_x) - v.m_z * (p1.m_x * v.m_x + p1.m_y * v.m_y)) * (1.0 - cos_a) + (p1.m_x * v.m_y - p1.m_y * v.m_x) * sin_a;
	mx.m_33 = 1.0;

	*this = mx * (*this);

}

void x3d_trans_matrix::move(double dx, double dy, double dz)
{
	if (compare_tools::is_equal(dx, 0.0)
		&& compare_tools::is_equal(dy, 0.0)
		&& compare_tools::is_equal(dz, 0.0)) return;

	x3d_trans_matrix mx;
	mx.m_00 = 1.0;		mx.m_01 = 0.0;		mx.m_02 = 0.0;		mx.m_03 = 0.0;
	mx.m_10 = 0.0;		mx.m_11 = 1.0;		mx.m_12 = 0.0;		mx.m_13 = 0.0;
	mx.m_20 = 0.0;		mx.m_21 = 0.0;		mx.m_22 = 1.0;		mx.m_23 = 0.0;
	mx.m_30 = dx;		mx.m_31 = dy;		mx.m_32 = dz;		mx.m_33 = 1.0;

	*this = mx * (*this);
}

void x3d_trans_matrix::scale(double sx, double sy, double sz)
{
	if (compare_tools::is_equal(sx, 0.0)
		&& compare_tools::is_equal(sy, 0.0)
		&& compare_tools::is_equal(sz, 0.0)) return;

	if (sx < 0) sx = 1.0;
	if (sy < 0) sx = 1.0;
	if (sz < 0) sx = 1.0;
	x3d_trans_matrix mx;
	mx.m_00 = sx;		mx.m_01 = 0.0;		mx.m_02 = 0.0;		mx.m_03 = 0.0;
	mx.m_10 = 0.0;		mx.m_11 = sy;		mx.m_12 = 0.0;		mx.m_13 = 0.0;
	mx.m_20 = 0.0;		mx.m_21 = 0.0;		mx.m_22 = sz;		mx.m_23 = 0.0;
	mx.m_30 = 0.0;		mx.m_31 = 0.0;		mx.m_32 = 0.0;		mx.m_33 = 1.0;

	*this = mx * (*this);
}

void x3d_trans_matrix::scale(const x3d_vector& vec, double s)
{
	s = math_tools::normalize_angle(s);
	if (compare_tools::is_equal(s, 0.0) || vec.is_all_zero()) return;

	x3d_trans_matrix mx;
	mx.m_00 = 1.0 + (s - 1.0) * vec.m_x * vec.m_x;
	mx.m_01 = (s - 1.0)* vec.m_x * vec.m_y;
	mx.m_02 = (s - 1.0)* vec.m_x * vec.m_z;
	mx.m_03 = 0.0;

	mx.m_10 = (s - 1.0)* vec.m_x * vec.m_y;
	mx.m_11 = 1.0 + (s - 1.0) * vec.m_y * vec.m_y;
	mx.m_12 = (s - 1.0)* vec.m_y * vec.m_z;
	mx.m_13 = 0.0;

	mx.m_20 = (s - 1.0)* vec.m_x * vec.m_z;
	mx.m_21 = (s - 1.0)* vec.m_z * vec.m_y;
	mx.m_22 = 1.0 + (s - 1.0) * vec.m_z * vec.m_z;
	mx.m_23 = 0.0;

	mx.m_30 = 0.0;
	mx.m_31 = 0.0;
	mx.m_32 = 0.0;
	mx.m_33 = 1.0;

	*this = mx * (*this);
}

void x3d_trans_matrix::scale(const x3d_point& p, double sx, double sy, double sz)
{
	if (compare_tools::is_equal(sx, 0.0)
		&& compare_tools::is_equal(sy, 0.0)
		&& compare_tools::is_equal(sz, 0.0)) return;

	if (sx < 0) sx = 1.0;
	if (sy < 0) sx = 1.0;
	if (sz < 0) sx = 1.0;

	x3d_trans_matrix mx;
	mx.m_00 = sx;		      mx.m_01 = 0.0;		    mx.m_02 = 0.0;		      mx.m_03 = 0.0;
	mx.m_10 = 0.0;		      mx.m_11 = sy;		        mx.m_12 = 0.0;		      mx.m_13 = 0.0;
	mx.m_20 = 0.0;		      mx.m_21 = 0.0;		    mx.m_22 = sz;		      mx.m_23 = 0.0;
	mx.m_30 = (1 - sx)*p.m_x; mx.m_31 = (1 - sy)*p.m_y;	mx.m_32 = (1 - sz)*p.m_z; mx.m_33 = 1.0;

	*this = mx * (*this);
}

x3d_vector x3d_trans_matrix::get_rotation() const
{
	x3d_vector res;

	// ...不知道要获取相对哪个向量的旋转量了，此接口可能实用价值不大，可能会删掉

	return res;
}

x3d_vector x3d_trans_matrix::get_move() const
{
	return x3d_vector(m_30, m_31, m_32); // 没有去除转动量等其它迭加变换造成的移动量，相当于最终的移动量
}

x3d_vector x3d_trans_matrix::get_scale() const
{
	return x3d_vector(m_00, m_11, m_22);
}

void x3d_trans_matrix::no_move()
{
	m_30 = m_31 = m_32 = 0.0;
}

x3d_point& x3d_trans_matrix::transform_vertex_self(x3d_point& pt) const
{
	// 相当于右乘变换矩阵
	double dx = 0.0, dy = 0.0, dz = 0.0;
	dx = pt.m_x * m_00 + pt.m_y * m_10 + pt.m_z * m_20 + m_30;
	dy = pt.m_x * m_01 + pt.m_y * m_11 + pt.m_z * m_21 + m_31;
	dz = pt.m_x * m_02 + pt.m_y * m_12 + pt.m_z * m_22 + m_32;
	
	pt.m_x = dx;
	pt.m_y = dy;
	pt.m_z = dz;
	return pt;
}

x3d_point x3d_trans_matrix::transform_vertex(const x3d_point& pt) const
{
	x3d_point tmp_pt = pt;
	transform_vertex_self(tmp_pt);
	return tmp_pt;
}

x3d_trans_matrix& x3d_trans_matrix::operator*(const x3d_trans_matrix& right_mx)
{
	// OpenGL在多种变换同时施加到顶点上时以相反的顺序矩阵相乘，此处和它保持一致（相当于右乘）
	// 内部并没有存为数组，写成按行列号循环计算的代码虽然简洁，但调用过程明显比直接写要复杂，因此直接写成手工计算方式
	m_00 = m_00 * right_mx.m_00 + m_01 * right_mx.m_10 + m_02 * right_mx.m_20 + m_03 * right_mx.m_30;
	m_10 = m_10 * right_mx.m_00 + m_11 * right_mx.m_10 + m_12 * right_mx.m_20 + m_13 * right_mx.m_30;
	m_20 = m_20 * right_mx.m_00 + m_21 * right_mx.m_10 + m_22 * right_mx.m_20 + m_23 * right_mx.m_30;
	m_30 = m_30 * right_mx.m_00 + m_31 * right_mx.m_10 + m_32 * right_mx.m_20 + m_33 * right_mx.m_30;

	m_01 = m_00 * right_mx.m_01 + m_01 * right_mx.m_11 + m_02 * right_mx.m_21 + m_03 * right_mx.m_31;
	m_11 = m_10 * right_mx.m_01 + m_11 * right_mx.m_11 + m_12 * right_mx.m_21 + m_13 * right_mx.m_31;
	m_21 = m_20 * right_mx.m_01 + m_21 * right_mx.m_11 + m_22 * right_mx.m_21 + m_23 * right_mx.m_31;
	m_31 = m_30 * right_mx.m_01 + m_31 * right_mx.m_11 + m_32 * right_mx.m_21 + m_33 * right_mx.m_31;

	m_02 = m_00 * right_mx.m_02 + m_01 * right_mx.m_12 + m_02 * right_mx.m_22 + m_03 * right_mx.m_32;
	m_12 = m_10 * right_mx.m_02 + m_11 * right_mx.m_12 + m_12 * right_mx.m_22 + m_13 * right_mx.m_32;
	m_22 = m_20 * right_mx.m_02 + m_21 * right_mx.m_12 + m_22 * right_mx.m_22 + m_23 * right_mx.m_32;
	m_32 = m_30 * right_mx.m_02 + m_31 * right_mx.m_12 + m_32 * right_mx.m_22 + m_33 * right_mx.m_32;

	m_03 = m_00 * right_mx.m_03 + m_01 * right_mx.m_13 + m_02 * right_mx.m_23 + m_03 * right_mx.m_33;
	m_13 = m_10 * right_mx.m_03 + m_11 * right_mx.m_13 + m_12 * right_mx.m_23 + m_13 * right_mx.m_33;
	m_23 = m_20 * right_mx.m_03 + m_21 * right_mx.m_13 + m_22 * right_mx.m_23 + m_23 * right_mx.m_33;
	m_33 = m_30 * right_mx.m_03 + m_31 * right_mx.m_13 + m_32 * right_mx.m_23 + m_33 * right_mx.m_33;

	return *this;
}

double x3d_trans_matrix::get_value(int row_index, int column_index) const
{
	if (0 == row_index)
	{
		if (0 == column_index) return m_00;
		else if (1 == column_index) return m_01;
		else if (2 == column_index) return m_02;
		else if (3 == column_index) return m_03;
		else return 0.0;
	}
	else if (1 == row_index)
	{
		if (0 == column_index) return m_10;
		else if (1 == column_index) return m_11;
		else if (2 == column_index) return m_12;
		else if (3 == column_index) return m_13;
		else return 0.0;
	}
	else if (2 == row_index)
	{
		if (0 == column_index) return m_20;
		else if (1 == column_index) return m_21;
		else if (2 == column_index) return m_22;
		else if (3 == column_index) return m_23;
		else return 0.0;
	}
	else if (3 == row_index)
	{
		if (0 == column_index) return m_30;
		else if (1 == column_index) return m_31;
		else if (2 == column_index) return m_32;
		else if (3 == column_index) return m_33;
		else return 0.0;
	}
	else
	{
		return 0.0;
	}
}

void x3d_trans_matrix::set_value(int row_index, int column_index, double val)
{
	if (0 == row_index)
	{
		if (0 == column_index) m_00 = val;
		else if (1 == column_index) m_01 = val;
		else if (2 == column_index) m_02 = val;
		else if (3 == column_index) m_03 = val;
		else; // nothing
	}
	else if (1 == row_index)
	{
		if (0 == column_index) m_10 = val;
		else if (1 == column_index) m_11 = val;
		else if (2 == column_index) m_12 = val;
		else if (3 == column_index) m_13 = val;
		else; // nothing
	}
	else if (2 == row_index)
	{
		if (0 == column_index) m_20 = val;
		else if (1 == column_index) m_21 = val;
		else if (2 == column_index) m_22 = val;
		else if (3 == column_index) m_23 = val;
		else; // nothing
	}
	else if (3 == row_index)
	{
		if (0 == column_index) m_30 = val;
		else if (1 == column_index) m_31 = val;
		else if (2 == column_index) m_32 = val;
		else if (3 == column_index) m_33 = val;
		else; // nothing
	}
	else
	{
		// nothing
	}
}

int x3d_trans_matrix::get_row_count() const
{
	return 4;
}

int x3d_trans_matrix::get_column_count() const
{
	return 4;
}

void x3d_trans_matrix::get_values(std::vector<std::vector<double>>& vals) const
{
	vals.resize(get_row_count());
	for (auto& x : vals) x.resize(get_column_count());

	for (int i = 0; i < (int) vals.size(); ++i)
		for (int j = 0; j < (int) vals[i].size(); ++j)
			vals[i][j] = get_value(i, j);
}

void x3d_trans_matrix::get_values(std::array<std::array<double, 4>, 4>& vals) const
{
	for (int i = 0; i < (int) vals.size(); ++i)
		for (int j = 0; j < (int) vals[i].size(); ++j)
			vals[i][j] = get_value(i, j);
}

void x3d_trans_matrix::set_values(std::vector<std::vector<double>>& vals)
{
	for (int i = 0; i < (int) vals.size(); ++i)
		for (int j = 0; j < (int) vals[i].size(); ++j)
			set_value(i, j, vals[i][j]);
}

void x3d_trans_matrix::set_values(std::array<std::array<double, 4>, 4>& vals)
{
	for (int i = 0; i < (int) vals.size(); ++i)
		for (int j = 0; j < (int) vals[i].size(); ++j)
			set_value(i, j, vals[i][j]);
}


x3d_vector::x3d_vector()
{
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;
}


x3d_vector::x3d_vector(double x, double y, double z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

x3d_vector::x3d_vector(const x3d_vector& src)
{
	if (this != &src) *this = src;
}

x3d_vector::x3d_vector(const x3d_point& s, const x3d_point& e)
{
	set_value(s, e);
}

x3d_vector::~x3d_vector()
{
}

x3d_vector& x3d_vector::operator+=(const x3d_vector& vec)
{
	m_x += vec.m_x;
	m_y += vec.m_y;
	m_z += vec.m_z;
	return *this;
}

x3d_vector& x3d_vector::operator-=(const x3d_vector& vec)
{
	m_x -= vec.m_x;
	m_y -= vec.m_y;
	m_z -= vec.m_z;
	return *this;
}


x3d_vector x3d_vector::operator+(const x3d_vector& vec) const
{	
	return x3d_vector(m_x + vec.m_x, m_y + vec.m_y, m_z + vec.m_z);
}


x3d_vector x3d_vector::operator-(const x3d_vector& vec) const
{
	return x3d_vector(m_x - vec.m_x, m_y - vec.m_y, m_z - vec.m_z);
}


x3d_vector x3d_vector::operator-() const
{
	return x3d_vector(-m_x, -m_y, -m_z);
}

x3d_vector& x3d_vector::operator*(double scale)
{
	m_x *= scale;
	m_y *= scale;
	m_z *= scale;
	return *this;
}

double x3d_vector::operator*(const x3d_vector& vec) const
{
	return m_x * vec.m_x + m_y * vec.m_y + m_z * vec.m_z;
}

x3d_vector x3d_vector::operator*(double scale) const
{
	x3d_vector tmp = *this;
	return tmp*scale;
}

x3d_vector& x3d_vector::rotate(const x3d_vector& vec, double a)
{
	x3d_trans_matrix mx;
	mx.rotate(vec, a);
	mx.transform_vertex_self(*this);
	return *this;
}

x3d_vector& x3d_vector::move(const x3d_vector& v, double l)
{
    x3d_vector tv = v.get_unit_vector() * l;
	*this += tv;
	return *this;
}

bool x3d_vector::operator==(const x3d_vector& vec) const
{
	return is_equal(vec, compare_tools::get_precision());
}

x3d_vector& x3d_vector::set_value(double x, double y, double z)
{
	m_x = x;
	m_y = y;
	m_z = z;
	return  *this;
}

x3d_vector& x3d_vector::set_value(const x3d_point& s, const x3d_point& e)
{
	m_x = e.m_x - s.m_x;
	m_y = e.m_y - s.m_y;
	m_z = e.m_z - s.m_z;
	return *this;
}

bool x3d_vector::is_equal(const x3d_point& pt, double pre /*= 1.0e-8*/) const
{
	if (!compare_tools::is_equal(m_x, pt.m_x, pre)) return false;
	if (!compare_tools::is_equal(m_y, pt.m_y, pre)) return false;
	if (!compare_tools::is_equal(m_z, pt.m_z, pre)) return false;
	return true;
}

double x3d_vector::get_module() const
{
	return sqrt(get_module_square());
}

double x3d_vector::get_module_square() const
{
	return m_x * m_x + m_y * m_y + m_z * m_z;
}

x3d_vector x3d_vector::get_unit_vector() const
{
	x3d_vector tmp = *this;
	tmp.normalize();
	return tmp;
}

x3d_vector& x3d_vector::normalize()
{
	double m = get_module();
	if (compare_tools::is_equal(m, 0.0))
	{
		m_x = m_y = m_z = 0.0;
		return *this;
	}

	m_x /= m;
	m_y /= m;
	m_z /= m;

	return *this;
}

double x3d_vector::get_ratio_to_another_vector(const x3d_vector& v) const
{
	if (!is_collinear(v)) return 0.0;
	if (compare_tools::is_not_equal(v.m_x, 0.0)) return m_x / v.m_x;
	if (compare_tools::is_not_equal(v.m_y, 0.0)) return m_y / v.m_y;
	if (compare_tools::is_not_equal(v.m_z, 0.0)) return m_z / v.m_z;

	return 0.0;
}

double x3d_vector::get_distance(const x3d_vector& p) const
{
	return sqrt(get_distance_square(p));
}

double x3d_vector::get_distance_square(const x3d_vector& p) const
{
	return (p.m_x - m_x)*(p.m_x - m_x) + (p.m_y - m_y)*(p.m_y - m_y) + (p.m_z - m_z)*(p.m_z - m_z);
}

x3d_vector x3d_vector::get_cross_vector(const x3d_vector& v) const
{
	return x3d_vector(
		(m_y*v.m_z - m_z*v.m_y),
		(m_z*v.m_x - m_x*v.m_z),
		(m_x*v.m_y - m_y*v.m_x));
}

x3d_vector x3d_vector::get_point_by_vector(const x3d_vector& v, double l) const
{
	x3d_vector tmp = *this;
	tmp.move(v, l);
	return tmp;
}

double x3d_vector::get_angle(const x3d_vector& vec) const
{
	return acos((*this*vec) / sqrt(get_module_square()*vec.get_module_square()));
}

bool x3d_vector::is_collinear(const x3d_vector& vec) const
{
	if (is_all_zero() || vec.is_all_zero()) return true;
	if (compare_tools::is_equal(m_x * vec.m_y, m_y * vec.m_x)
		&& compare_tools::is_equal(m_y * vec.m_z, m_z * vec.m_y)
		&& compare_tools::is_equal(m_x * vec.m_z, m_z * vec.m_x))
		return true;

	return false;
}

bool x3d_vector::is_perpendicular(const x3d_vector& vec)const
{
	return *this*vec == 0;
}

bool x3d_vector::is_all_zero() const
{
	return compare_tools::is_equal(m_x, 0.0)
		&& compare_tools::is_equal(m_y, 0.0)
		&& compare_tools::is_equal(m_z, 0.0);
}



x3d_color::x3d_color()
{
	set_color(0, 0, 0, 1);
}

x3d_color::x3d_color(unsigned char r, unsigned char g, unsigned int b, unsigned char a /*= 255*/)
{
	set_color(r, g, b, a);
}

x3d_color::x3d_color(int r, int g, int b, int a /*= 255*/)
{
	set_color(r, g, b, a);
}

x3d_color::x3d_color(float r, float g, float b, float a /*= 1.0f*/)
{
	set_color(r, g, b, a);
}

x3d_color::x3d_color(double r, double g, double b, double a /*= 1.0*/)
{
	set_color(r, g, b, a);
}

x3d_color::x3d_color(COLORREF rgb, unsigned char a /*= 255*/)
{
	set_color(rgb, a);
}

x3d_color::x3d_color(const x3d_color& src)
{
	if (this != & src) *this = src;
}

x3d_color::~x3d_color()
{
}

void x3d_color::set_color(unsigned char r, unsigned char g, unsigned int b, unsigned char a /*= 255*/)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

void x3d_color::set_color(int r, int g, int b, int a /*= 255*/)
{
	m_r = (unsigned char) r;
	m_g = (unsigned char) g;
	m_b = (unsigned char) b;
	m_a = (unsigned char) a;
}

void x3d_color::set_color(float r, float g, float b, float a /*= 1.0f*/)
{
	m_r = (unsigned char) (r * 255);
	m_g = (unsigned char) (g * 255);
	m_b = (unsigned char) (b * 255);
	m_a = (unsigned char) (a * 255);
}

void x3d_color::set_color(double r, double g, double b, double a /*= 1.0*/)
{
	m_r = (unsigned char) (r * 255);
	m_g = (unsigned char) (g * 255);
	m_b = (unsigned char) (b * 255);
	m_a = (unsigned char) (a * 255);
}

void x3d_color::set_color(COLORREF rgb, unsigned char a /*= 255*/)
{
	m_r = (unsigned char) (GetRValue(rgb));
	m_g = (unsigned char) (GetGValue(rgb));
	m_b = (unsigned char) (GetBValue(rgb));
	m_a = a;
}

COLORREF x3d_color::get_color() const
{
	return RGB(m_r, m_g, m_b);
}

void x3d_color::get_color(float& r, float& g, float& b) const
{
	float a = 1.0f;
	get_color(r, g, b, a);
}

void x3d_color::get_color(float& r, float& g, float& b, float& a) const
{
	r = (float) (m_r / 255.0);
	b = (float) (m_g / 255.0);
	b = (float) (m_b / 255.0);
	a = (float) (m_a / 255.0);
}

void x3d_color::get_color(double& r, double& g, double& b) const
{
	double a = 1.0;
	get_color(r, g, b, a);
}

void x3d_color::get_color(double& r, double& g, double& b, double& a) const
{
	r = m_r / 255.0;
	b = m_g / 255.0;
	b = m_b / 255.0;
	a = m_a / 255.0;
}

unsigned char x3d_color::ri() const
{
	return m_r;
}

void x3d_color::ri(unsigned char r)
{
	m_r = r;
}

unsigned char x3d_color::gi() const
{
	return m_g;
}

void x3d_color::gi(unsigned char g)
{
	m_g = g;
}

unsigned char x3d_color::bi() const
{
	return m_b;
}

void x3d_color::bi(unsigned char b)
{
	m_b = b;
}

unsigned char x3d_color::ai() const
{
	return m_a;
}

void x3d_color::ai(unsigned char a)
{
	m_a = a;
}

float x3d_color::rf() const
{
	return (float) (m_r / 255.0);
}

void x3d_color::rf(float r)
{
	m_r = (unsigned char) (r * 255);
}

float x3d_color::gf() const
{
	return (float) (m_g / 255.0);
}

void x3d_color::gf(float g)
{
	m_g = (unsigned char) (g * 255);
}

float x3d_color::bf() const
{
	return (float) (m_b / 255.0);
}

void x3d_color::bf(float b)
{
	m_b = (unsigned char) (b * 255);
}

float x3d_color::af() const
{
	return (float) (m_a / 255.0);
}

void x3d_color::af(float a)
{
	m_a = (unsigned char) (a * 255);
}

double x3d_color::rd() const
{
	return m_r / 255.0;
}

void x3d_color::rd(double r)
{
	m_r = (unsigned char) (r * 255);
}

double x3d_color::gd() const
{
	return m_g / 255.0;
}

void x3d_color::gd(double g)
{
	m_g = (unsigned char) (g * 255);
}

double x3d_color::bd() const
{
	return m_b / 255.0;
}

void x3d_color::bd(double b)
{
	m_b = (unsigned char) (b * 255);
}

double x3d_color::ad() const
{
	return m_a / 255.0;
}

void x3d_color::ad(double a)
{
	m_a = (unsigned char) (a * 255);
}

x3d_vertex::x3d_vertex()
{

}

x3d_vertex::x3d_vertex(const x3d_point& pt, const x3d_vector& nm)
{
	set(pt, nm);
}

x3d_vertex::x3d_vertex(const x3d_point& pt, const x3d_color& cl /*= x3d_color()*/, const x3d_vector& nm /*= x3d_vector(0, 0, -1)*/)
{
	set(pt, cl, nm);
}

x3d_vertex::~x3d_vertex()
{

}

void x3d_vertex::set(const x3d_point& pt, const x3d_vector& nm)
{
	m_point = pt;
	m_normal_vector = nm;
}

void x3d_vertex::set(const x3d_point& pt, const x3d_color& cl /*= x3d_color()*/, const x3d_vector& nm /*= x3d_vector(0, 0, -1)*/)
{
	m_point = pt;
	m_color = cl;
	m_normal_vector = nm;
}


IMPLEMENT_SERIAL(x3d_geometry_base, x3d_node_base, 1)

x3d_geometry_base::x3d_geometry_base()
{
	m_span_step = 0.0;
	m_span_count = 0.0;
}

x3d_geometry_base::~x3d_geometry_base()
{

}

void x3d_geometry_base::copy(const data_node_base& node, bool copy_tag/* = false*/)
{
	x3d_node_base::copy(node, copy_tag);

	const x3d_geometry_base* src_obj = dynamic_cast<const x3d_geometry_base*>(&node);
	if (!src_obj) return;

	m_span_step = src_obj->m_span_step;
	m_span_count = src_obj->m_span_count;
	m_base_point = src_obj->m_base_point;
}

void x3d_geometry_base::regenerate_vertext(x3d_graph& graph)
{
	graph.m_all_units.clear();
}

void x3d_geometry_base::set_span_step(double step)
{
	m_span_step = step;
}

void x3d_geometry_base::set_span_count(double count)
{
	m_span_count = count;
}

void x3d_geometry_base::set_base_point(const x3d_point& p)
{
	m_base_point = p;
}

double x3d_geometry_base::get_span_step() const
{
	return m_span_step;
}

double x3d_geometry_base::get_span_count() const
{
	return m_span_count;
}

const x3d_point& x3d_geometry_base::get_base_point() const
{
	return m_base_point;
}

double x3d_geometry_base::calc_span_step(double total_len) const
{
	if (compare_tools::is_less_equal(m_span_count, 0.0))
	{
		if (compare_tools::is_less_equal(m_span_step, 0.0))
			return total_len / 16.0; // 分个16段吧
		else
			return m_span_step;
	}

	double result_step = total_len / m_span_count;
	result_step = min(result_step, m_span_step); // 细分步进量越小的越精细

	// 如果为小于等于0,则分个16段吧
	if (compare_tools::is_less_equal(result_step, 0.0))
		result_step = total_len / 16.0;

	return result_step;
}

double x3d_geometry_base::calc_real_span_count(double total_len) const
{
	if (compare_tools::is_less_equal(m_span_step, 0.0))
	{
		if (compare_tools::is_less_equal(m_span_count, 0.0))
			return 16.0; // 分个16段吧
		else
			return m_span_count;
	}

	double result_count = total_len / m_span_step;
	result_count = max(result_count, m_span_count); // 细分数目越大越精细

	// 如果小于等于0,则分个16段吧
	if (compare_tools::is_less_equal(result_count, 0.0))
		result_count = 16.0;

	return result_count;
}

int x3d_geometry_base::calc_span_count(double total_len) const
{
	double span_count = calc_real_span_count(total_len);
	return math_tools::get_great_integer(span_count);
}



IMPLEMENT_SERIAL(x3d_action_base, CObject, 1)

x3d_action_base::x3d_action_base()
{
}

x3d_action_base::x3d_action_base(const x3d_action_base& src)
{
}

x3d_action_base::~x3d_action_base()
{
}

x3d_action_base& x3d_action_base::operator=(const x3d_action_base& data)
{
	return *this;
}

void x3d_action_base::do_action(x3d_graph& result)
{

}



x3d_plane::x3d_plane()
{
}


x3d_plane::x3d_plane(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3)
{
    m_1 = p1;
    m_2 = p2;
    m_3 = p3;
}

x3d_plane::~x3d_plane()
{
}

x3d_vector x3d_plane::get_normal_vector() const
{
	x3d_vector v1 = m_2 - m_1;
	x3d_vector v2 = m_3 - m_1;

	if (v1.is_collinear(v2)) return x3d_vector(0, 0, 0);  // 若三个点不构成一个平面，则返回0

	x3d_vector res (v1.m_y*v2.m_z - v2.m_y*v1.m_z,
		v1.m_z*v2.m_x - v2.m_z*v1.m_x, v1.m_x*v2.m_y - v2.m_x*v1.m_y);

	return res.normalize();
}

x3d_point x3d_plane::get_footpoint_on_plane(const x3d_point& p) const
{
    //求平面法向量
    double a = (m_2.m_y - m_1.m_y)*(m_3.m_z - m_1.m_z) - (m_3.m_y - m_1.m_y)*(m_2.m_z - m_1.m_z);

    double b = (m_3.m_x - m_1.m_x)*(m_2.m_z - m_1.m_z) - (m_2.m_x - m_1.m_x)*(m_3.m_z - m_1.m_z);

    double c = (m_2.m_x - m_1.m_x)*(m_3.m_y - m_1.m_y) - (m_3.m_x - m_1.m_x)*(m_2.m_y - m_1.m_y);
    //由平行关系和垂直关系得到t
    double t;
    t = (a*m_1.m_x + b*m_1.m_y + c*m_1.m_z - (a*p.m_x + b*p.m_y + c*p.m_z)) / (a*a + b*b + c*c);

    //求出投影点
    double x = p.m_x + a*t;
    double y = p.m_y + b*t;
    double z = p.m_z + c*t;

    return x3d_point(x, y, z);
}

bool x3d_plane::is_plane()
{
	x3d_vector v1 = m_2 - m_1;
	x3d_vector v2 = m_3 - m_1;
    x3d_vector v3 = m_3 - m_2;
	if (v1.is_all_zero()) return false;
	if (v2.is_all_zero()) return false;
    if (v3.is_all_zero()) return false;

    v1.normalize();
    v2.normalize();
    if (v1 == v2) return false;  // 若单位向量相等，则三点共线

	return true;
}

bool x3d_plane::is_in_plane(const x3d_point& p) const
{
	x3d_vector v1(m_1, p);
	x3d_vector v2(m_2, p);
	x3d_vector v3(m_3, p);
	double ans = v1.m_x*v2.m_y*v3.m_z + v1.m_y*v2.m_z*v3.m_x + v1.m_z*v2.m_x*v3.m_y   
		- v1.m_z*v2.m_y*v3.m_x - v1.m_x*v2.m_z*v3.m_y - v1.m_y*v2.m_x*v3.m_z;       // 行列式为0则共面

	if (compare_tools::is_equal(ans, 0.0, 1e-6)) return true;
	return false;
}

void x3d_plane::set_data(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3)
{
    m_1 = p1;
    m_2 = p2;
    m_3 = p3;
}

x3d_action_section::x3d_action_section()
{
	m_nm_type = x3d_normal_type::plane;
}

x3d_action_section::x3d_action_section(const x3d_action_section& src)
{
	m_all_points.assign(src.m_all_points.begin(), src.m_all_points.end());
	m_nm_type = src.m_nm_type;
}

x3d_action_section::x3d_action_section(std::vector<x3d_point>& points)
{
	set_data(points);
}


x3d_action_section::x3d_action_section(vector<x3d_action_section::point>& points)
{
	set_data(points);
}

x3d_action_section::~x3d_action_section()
{

}

void x3d_action_section::set_data(std::vector<x3d_point>& points)
{
	for (auto& x : points)
		m_all_points.push_back(point(x));

	m_nm_type = x3d_normal_type::plane;
}

void x3d_action_section::set_data(vector<x3d_action_section::point>& points)
{
	m_all_points = points;
	m_nm_type = x3d_normal_type::plane;

}
