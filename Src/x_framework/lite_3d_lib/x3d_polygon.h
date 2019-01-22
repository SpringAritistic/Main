#pragma once

#include "x3d_definition.h"
#include "x3d_data_base.h"


//////////////////////////////////////////////////////////////////////////
typedef x3d_vector x3d_line_loop;
//////////////////////////////////////////////////////////////////////////

class x3d_line;

// 由多种线元相接组成的不间断的多段线
class X_FRAMEWORK_API x3d_polygon : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_polygon)

public:
	x3d_polygon();
	virtual ~x3d_polygon();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	// 是否为空，点的数量小于2则为空
	bool is_empty() const;

	// 获得线的总长
	double get_length() const;

	// 在指定长度处截断，操作失败时什么也不做
	// reserve_front为true表示删除后面部分，保留前面部分，为false表示删除前面部分，保留后面部分
	void trim_at_pos(double length, bool reserve_front = true);

	// 延长指定的长度，操作失败时什么也不做
	// ext_front为true表示延长头部，为false表示延长尾部
	// 延长时会求端部点处的导线，并沿导线作直线延伸
	void extend_length(double ext_len, bool ext_front = false);

	// 清空所有点
	void clear_points();

	// 得到索引值为index的点的坐标(从0开始)
	x3d_point get_point(size_t index) const;

	// 添加一个新点，to_front为true表示在头部添加
	void add_point(x3d_point new_point, bool to_front = false);

	// 添加新点集，ext_front为true表示在头部添加
	void add_points(std::vector<x3d_point>& points, bool ext_front = false);

	// 在指定长度处插入点
	void insert_point(double length);

	// 添加直线段，直线沿向量vec的方向，延伸length的长度，操作失败时什么也不做
	void add_line(x3d_vector vec, double length, bool to_front = false);

	// 添加另外一个polygon
	void connect_polygon(x3d_polygon& another, bool to_front = false);	

	// 与指定的线元求交点，操作成功返回true，否则返回false
	// 当指定延长方式时，均按端点处的导线方向延长
	// 在任意平面上投影上求交（包装点、直线、向量、面的数据结构），如果面为空则表示直接求三维空间中的交
	// 点，如果交点有多个，则只会返回找到的第一个点
	// precision为交点判断精度（线在最近点上的距离小于等于此值则认为是相交于一点）
	bool get_all_intersection(std::vector<std::pair<x3d_point, x3d_intersection_type>>& points,
		x3d_line& l, x3d_extend_type et = x3d_extend_type::none, double precision = 1e-6);

	// 求与直线到第一个交点，若不存在返回false
	bool get_first_intersection(x3d_point& p, x3d_line& l, x3d_extend_type et = x3d_extend_type::none, double precision = 1e-6);

	// 求与直线最后一个交点，若不存在返回false
	bool get_last_intersection(x3d_point& p, x3d_line& l, x3d_extend_type et = x3d_extend_type::none, double precision = 1e-6);

	// 求与直线最近的一个交点，若不存在返回false
	// 若与线段有交点，则得到第一个交点
	bool get_nearest_intersection(x3d_point& p, x3d_line& l, x3d_extend_type et = x3d_extend_type::none, double precision = 1e-6);

	// 判断一个点是否在线上
	// ？？？在任意平面上投影上求交（包装点、直线、向量、面的数据结构），如果面为空则表示直接求三维空间中的交
	// 点，如果交点有多个，则只会返回找到的第一个点
	// precision为交点判断精度（线在最近点上的距离小于等于此值则认为是相交于一点）
	bool is_in_line(x3d_point& p, double precision = 1e-6);

	// 返回离起点指定距离处的点坐标，超出线或无法求解时返回false，否则返回true
	bool get_point_at_pos(x3d_point& pt, double dist);

	// 得到径向偏移的的多段线(偏移距离为正表示沿多段线方向向右偏移；偏移保证z坐标不变;对象由外部接管)
	x3d_polygon* offset(double dist) const;
	x3d_polygon& self_offset(double dist);

	// 把x3d_graph转化成polygon
	void transfer_from(x3d_graph& src);

	// 把polygon转成section
	void transfer_to_section(x3d_action_section& sec);

public:
	// 线元经常需要在两端延长和缩短，使用deque综合性能要优于vector
	std::deque<x3d_point> m_all_points;
};
